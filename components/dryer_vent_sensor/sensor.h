#include "esphome.h"

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/sensor/sensor.h"

#ifdef ARDUINO_ARCH_ESP32
#include <driver/pcnt.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#endif

#define TEST_PERIOD     86400000 // one day in milliseconds
#define MAX_TEST_TIME   3 // Maximum number of cycles until we see our test response
#define FIRST_TEST      40000 // First test delay (millis)

// We are recording every 20ms (50 points / second and can wait up to 15s, *2 for safety :: 50*2 * 15 = 1500)
#define RING_SIZE 1500
int16_t ring[RING_SIZE] = {0}; // store number of pulses for every 20ms period
uint16_t current_item = 0;

pcnt_config_t pcnt_config = {
    .pulse_gpio_num = 2,        // Will be updated in setup
    .ctrl_gpio_num = -1,
    .lctrl_mode = PCNT_MODE_KEEP,
    .hctrl_mode = PCNT_MODE_KEEP,
    .pos_mode = PCNT_COUNT_INC,
    .neg_mode = PCNT_COUNT_DIS,
    .counter_h_lim = 0,
    .counter_l_lim = 0,
    .unit = PCNT_UNIT_0,
    .channel = PCNT_CHANNEL_0
};

static void IRAM_ATTR timer_intr_handler(void *arg) {
  int pulse_count;
  pcnt_get_counter_value(PCNT_UNIT_0, &pulse_count);
  ring[current_item] = pulse_count;
  pcnt_counter_clear(PCNT_UNIT_0);
  if (current_item >= RING_SIZE - 1){
    current_item = 0;
  }
  else{
    current_item++;
  }
}

namespace esphome {
namespace dryer_vent_sensor {

class DryerVentSensor : public PollingComponent{

 private:
  int pulse_pin_ = 2;
  int test_pin_ = 15;
  unsigned long next_test = FIRST_TEST; // delay the initial collection at least one sample run
  unsigned long time_since_test = 0; // time passed since test
  boolean test_outstanding = false; // if we're waiting for test results
  boolean test_failed = false; // if the last test failed
  uint16_t review_position = 0; // where the polling has reviewed
  boolean in_pulse = false; // track if we are mid pulse
  boolean in_packet = false; //track if we are mid data
  short packet_pulses = 0; //track number of pulses in packet
  uint16_t pulse_length = 0;
  uint16_t break_length = 0;
  boolean clog = false;
  boolean overheat = false;
  uint16_t short_start = 0;
  uint16_t long_start = 0;
  uint16_t short_clog = 0;
  uint16_t long_clog = 0;
  uint16_t short_overheat = 0;
  uint16_t long_overheat = 0;
  uint16_t short_packet = 0;
  uint16_t unknown_packet = 0;
  uint16_t selftest_count = 0;
  
  esphome::sensor::Sensor *overheat_sensor_ = nullptr;
  esphome::sensor::Sensor *clog_sensor_ = nullptr;
  esphome::sensor::Sensor *selftest_sensor_ = nullptr;
  esphome::sensor::Sensor *short_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *short_start_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *long_start_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *short_clog_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *long_clog_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *short_overheat_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *long_overheat_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *unknown_packet_sensor_ = nullptr;
  esphome::sensor::Sensor *selftest_counter_sensor_ = nullptr;
  
 public:
  // constructor
  DryerVentSensor() {} // poll every 15s

  float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; } // wait until most hardware is setup

  void set_pulse_pin(int pin) { pulse_pin_ = pin; }
  void set_test_pin(int pin) { test_pin_ = pin; }
  void set_overheat_sensor(esphome::sensor::Sensor *sensor) { overheat_sensor_ = sensor; }
  void set_clog_sensor(esphome::sensor::Sensor *sensor) { clog_sensor_ = sensor; }
  void set_selftest_sensor(esphome::sensor::Sensor *sensor) { selftest_sensor_ = sensor; }
  void set_short_packet_sensor(esphome::sensor::Sensor *sensor) { short_packet_sensor_ = sensor; }
  void set_short_start_packet_sensor(esphome::sensor::Sensor *sensor) { short_start_packet_sensor_ = sensor; }
  void set_long_start_packet_sensor(esphome::sensor::Sensor *sensor) { long_start_packet_sensor_ = sensor; }
  void set_short_clog_packet_sensor(esphome::sensor::Sensor *sensor) { short_clog_packet_sensor_ = sensor; }
  void set_long_clog_packet_sensor(esphome::sensor::Sensor *sensor) { long_clog_packet_sensor_ = sensor; }
  void set_short_overheat_packet_sensor(esphome::sensor::Sensor *sensor) { short_overheat_packet_sensor_ = sensor; }
  void set_long_overheat_packet_sensor(esphome::sensor::Sensor *sensor) { long_overheat_packet_sensor_ = sensor; }
  void set_unknown_packet_sensor(esphome::sensor::Sensor *sensor) { unknown_packet_sensor_ = sensor; }
  void set_selftest_counter_sensor(esphome::sensor::Sensor *sensor) { selftest_counter_sensor_ = sensor; }
  
  void setup() override {
    // Update pulse pin in config
    pcnt_config.pulse_gpio_num = (gpio_num_t)pulse_pin_;

    // Configure Pulse Counter using legacy ESP-IDF API for compatibility
    pcnt_unit_config(&pcnt_config);

    // Configure Test PIN
    gpio_set_direction((gpio_num_t)test_pin_, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)test_pin_, 1);

    // Setup timer to check for ticks every 20ms using ESP-IDF timer
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_intr_handler,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "pulse_timer",
        .skip_unhandled_events = false,
    };
    esp_timer_handle_t timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 20000)); // 20ms
  }

  void update() override {
    // This will be called every "update_interval" milliseconds.
    int16_t end_position = current_item;
    ESP_LOGD("dryer_vent_sensor", "Checking %i -> %i", review_position, end_position);
    while (review_position != end_position){
      if (ring[review_position] > 5){ // If we see a any pulse in this block
    if (!in_pulse){ // if we weren't in a pulse already we reset things
      if (!in_packet){
        in_packet = true; //start our packet
        packet_pulses = 1;
      }
      else { // if we were already in a packet this is a new pulse
        packet_pulses ++;
      }
      in_pulse = true; // remember we are now in pulse
      pulse_length = 0; // this is our first pulse
    }
    pulse_length++;
      }
      if (ring[review_position] <= 5){
    if (in_pulse){ // we were in a pulse
      ESP_LOGD("dryer_vent_sensor", "Pulse Length %i", pulse_length);
      if (pulse_length == 1){
        short_packet++;
      }
      in_pulse = false; // we are no longer
      break_length = 0; // reset out own counter
    }
    break_length++;
    if (break_length > 50 && in_packet){ // detect the end of packets
      if (packet_pulses == 1){
        if (pulse_length < 25) {
          ESP_LOGD("dryer_vent_sensor", "1 Pulse Packet, %i long (unknown)", pulse_length);
          short_start++;
        }
        else if (pulse_length > 45){
          ESP_LOGD("dryer_vent_sensor", "1 Pulse Packet, %i long (unknown)", pulse_length);
          long_start++;
        }
        else {
          ESP_LOGD("dryer_vent_sensor", "1 Pulse Packet, %i long (test/startup)", pulse_length);
          if (test_outstanding){
        test_outstanding = false;
        test_failed = false;
        selftest_count++;
          }
        }

      }
      else if (packet_pulses == 3){
        ESP_LOGD("dryer_vent_sensor", "3 Pulse Packet (clog)");
        if (pulse_length < 3){
          short_clog++;
        }
        else if (pulse_length > 7){
          long_clog++;
        }
        clog = true;
      }
      else if (packet_pulses == 5){
        ESP_LOGD("dryer_vent_sensor", "5 Pulse Packet (overheat)");
        overheat = true;
        if (pulse_length < 3){
          short_overheat++;
        }
        else if (pulse_length > 7){
          long_overheat++;
        }
      }
      else {
        ESP_LOGD("dryer_vent_sensor", "%i Pulse Packet (error?)", packet_pulses);
        unknown_packet ++;
      }
      in_packet = false;
    }
    if (break_length > 65000){
      break_length = 101;
    }
      }

      review_position ++;
      if (review_position >= RING_SIZE -1){
    review_position = 0;
      }
    }
    
    // check to see if we need a self test, if we overflowed the wrap will be huge and dont execute until t warps too
    unsigned long t = millis();

    // Testing feature until I can figure out how to make FWUP work with these enabled
    //if (t > 240000){
    //  ESP_LOGD("custom", "Disable interupts: Start");
    //  timerAlarmDisable(timer);
    //  pcnt_counter_pause(PCNT_UNIT_0); // Disable Counter
    //  ESP_LOGD("custom", "Disable interupts: Done");
    //}
    
    if ((t > next_test)){
      ESP_LOGD("dryer_vent_sensor", "starting self test");
      test_outstanding = true; // note that we have a test running
      time_since_test = 0; // reset time we wait to see results
      digitalWrite(test_pin_,LOW); // trigger self test gpio
      delay(25); // leave it for a bit
      digitalWrite(test_pin_,HIGH); // untrigger selt test gpio
      next_test = t + TEST_PERIOD; // update when we want to test again
    }
    if (test_outstanding){
      time_since_test++; // when tests are outstanding count how many times we look for them
      if (time_since_test > MAX_TEST_TIME) {
    ESP_LOGD("dryer_vent_sensor", "self test failed");
    test_failed = true; // if it takes too long declare it failed
    test_outstanding = false;
      }
    }
    //publish all of our data
    if (overheat_sensor_) overheat_sensor_->publish_state(overheat);
    if (clog_sensor_) clog_sensor_->publish_state(clog);
    if (selftest_sensor_) selftest_sensor_->publish_state(test_failed);
    if (short_packet_sensor_) short_packet_sensor_->publish_state((float)short_packet);
    if (short_start_packet_sensor_) short_start_packet_sensor_->publish_state((float)short_start);
    if (long_start_packet_sensor_) long_start_packet_sensor_->publish_state((float)long_start);
    if (short_clog_packet_sensor_) short_clog_packet_sensor_->publish_state((float)short_clog);
    if (long_clog_packet_sensor_) long_clog_packet_sensor_->publish_state((float)long_clog);
    if (short_overheat_packet_sensor_) short_overheat_packet_sensor_->publish_state((float)short_overheat);
    if (long_overheat_packet_sensor_) long_overheat_packet_sensor_->publish_state((float)long_overheat);
    if (unknown_packet_sensor_) unknown_packet_sensor_->publish_state((float)unknown_packet);
    if (selftest_counter_sensor_) selftest_counter_sensor_->publish_state((float)selftest_count);
  }
};

}  // namespace dryer_vent_sensor
}  // namespace esphome