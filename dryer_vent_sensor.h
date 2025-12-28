
#include "esphome.h"

#include "esphome/core/component.h"
#include "esphome/core/esphal.h"
#include "esphome/components/sensor/sensor.h"

#ifdef ARDUINO_ARCH_ESP32
#include <driver/pcnt.h>
#endif

#define COUNT_PIN       2  // the gpio pin to sense buzzer activity
#define TEST_PIN        15 // the gpio pin to trigger a self test
#define TEST_PERIOD     86400000 // one day in milliseconds
#define MAX_TEST_TIME   3 // Maximum number of cycles until we see our test response
#define FIRST_TEST      40000 // First test delay (millis)


pcnt_config_t pcnt_config = {
        .pulse_gpio_num = COUNT_PIN,    // set gpio for pulse input gpio
        .ctrl_gpio_num = -1,            // no gpio for control
        .lctrl_mode = PCNT_MODE_KEEP,   // when control signal is low, keep the primary counter mode
        .hctrl_mode = PCNT_MODE_KEEP,   // when control signal is high, keep the primary counter mode
        .pos_mode = PCNT_COUNT_INC,     // increment the counter on positive edge
        .neg_mode = PCNT_COUNT_DIS,     // do nothing on falling edge
        .counter_h_lim = 0,             // not alarming, we will service this enough
        .counter_l_lim = 0,             // not counting down
        .unit = PCNT_UNIT_0,            // hard coded to Unit 0, humm
        .channel = PCNT_CHANNEL_0       // hard coded to Channel 0, humm
    };

// We are recording every 20ms (50 points / second and can wait up to 15s, *2 for safety :: 50*2 * 15 = 1500)
#define RING_SIZE 1500
int16_t ring[RING_SIZE] = {0}; // store number of pulses for every 20ms period
uint16_t current_item = 0;

hw_timer_t * timer = NULL;

static void IRAM_ATTR timer_intr_handler() {
  pcnt_get_counter_value(PCNT_UNIT_0, &ring[current_item]);
  pcnt_counter_clear(PCNT_UNIT_0);
  if (current_item >= RING_SIZE - 1){
    current_item = 0;
  }
  else{
    current_item++;
  }
}

class DryerVentSensor : public PollingComponent{

 private:
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
  
  
 public:
  // constructor
  DryerVentSensor() : PollingComponent(15000) {} // poll every 15s

  float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; } // wait until most hardware is setup

  Sensor *overheat_sensor = new Sensor();
  Sensor *clog_sensor = new Sensor();
  Sensor *selftest_sensor = new Sensor();
  Sensor *short_packet_sensor = new Sensor();
  Sensor *short_start_packet = new Sensor();
  Sensor *long_start_packet = new Sensor();
  Sensor *short_clog_packet = new Sensor();
  Sensor *long_clog_packet = new Sensor();
  Sensor *short_overheat_packet = new Sensor();
  Sensor *long_overheat_packet = new Sensor();
  Sensor *unknown_packet_sensor = new Sensor();
  Sensor *selftest_counter = new Sensor();
  
  void setup() override {
    // Configure Pulse PIN
    pinMode(COUNT_PIN,INPUT);

    // Configure pulsecounter
    pcnt_unit_config(&pcnt_config); // Configure pulse counter as above
    pcnt_set_filter_value(PCNT_UNIT_0, 500); //(12.5ns per unit, 50000ns / 10khz pulse, lots of caution but still some filtering) 
    pcnt_filter_enable(PCNT_UNIT_0); // Enable Filter
    pcnt_counter_resume(PCNT_UNIT_0); // Enable Counter

    // Setup timer to check for ticks every 20ms
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &timer_intr_handler, true);
    timerAlarmWrite(timer, 20000, true);
    timerAlarmEnable(timer);
 
    // Configure Test PIN
    pinMode(TEST_PIN,OUTPUT);
    digitalWrite(TEST_PIN,HIGH);
  }

  void update() override {
    // This will be called every "update_interval" milliseconds.
    int16_t end_position = current_item;
    ESP_LOGD("custom", "Checking %i -> %i", review_position, end_position);
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
	  ESP_LOGD("custom", "Pulse Length %i", pulse_length);
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
	      ESP_LOGD("custom", "1 Pulse Packet, %i long (unknown)", pulse_length);
	      short_start++;
	    }
	    else if (pulse_length > 45){
	      ESP_LOGD("custom", "1 Pulse Packet, %i long (unknown)", pulse_length);
	      long_start++;
	    }
	    else {
	      ESP_LOGD("custom", "1 Pulse Packet, %i long (test/startup)", pulse_length);
	      if (test_outstanding){
		test_outstanding = false;
		test_failed = false;
		selftest_count++;
	      }
	    }

	  }
	  else if (packet_pulses == 3){
	    ESP_LOGD("custom", "3 Pulse Packet (clog)");
	    if (pulse_length < 3){
	      short_clog++;
	    }
	    else if (pulse_length > 7){
	      long_clog++;
	    }
	    clog = true;
	  }
	  else if (packet_pulses == 5){
	    ESP_LOGD("custom", "5 Pulse Packet (overheat)");
	    overheat = true;
	    if (pulse_length < 3){
	      short_overheat++;
	    }
	    else if (pulse_length > 7){
	      long_overheat++;
	    }
	  }
	  else {
	    ESP_LOGD("custom", "%i Pulse Packet (error?)", packet_pulses);
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
      ESP_LOGD("custom", "starting self test");
      test_outstanding = true; // note that we have a test running
      time_since_test = 0; // reset time we wait to see results
      digitalWrite(TEST_PIN,LOW); // trigger self test gpio
      delay(25); // leave it for a bit
      digitalWrite(TEST_PIN,HIGH); // untrigger selt test gpio
      next_test = t + TEST_PERIOD; // update when we want to test again
    }
    if (test_outstanding){
      time_since_test++; // when tests are outstanding count how many times we look for them
      if (time_since_test > MAX_TEST_TIME) {
	ESP_LOGD("custom", "self test failed");
	test_failed = true; // if it takes too long declare it failed
	test_outstanding = false;
      }
    }
    //publish all of our data
    overheat_sensor->publish_state(overheat);
    clog_sensor->publish_state(clog);
    selftest_sensor->publish_state(test_failed);
    short_packet_sensor->publish_state(short_packet);
    short_start_packet->publish_state(short_start);
    long_start_packet->publish_state(long_start);
    short_clog_packet->publish_state(short_clog);
    long_clog_packet->publish_state(long_clog);
    short_overheat_packet->publish_state(short_overheat);
    long_overheat_packet->publish_state(long_overheat);
    unknown_packet_sensor->publish_state(unknown_packet);
    selftest_counter->publish_state(selftest_count);
  }
};


