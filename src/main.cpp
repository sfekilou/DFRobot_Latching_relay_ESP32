#include <Arduino.h>
#include "driver/rtc_io.h"

#define POWER_RELAY 25 //GPIO to Power DFRobot Relay Module
#define A_COM 26 //GPIO to self 1
#define B_COM 27 //GPIO to self 2

void Switch_Relay(int gpio_value) //force contact between A-COM or B_COM
{
  int R_Off=A_COM;
  int R_On=B_COM;
  if(gpio_value==A_COM)
  {
    R_Off=B_COM;
    R_On=A_COM;
  } 
  digitalWrite(R_Off,LOW); //Make sure that the signal for the coil is off (the 2 coils cannot be powered at the same time)
  digitalWrite(R_On,LOW);  //Make sure that the signal for the coil is off (the 2 coils cannot be powered at the same time)
  delay(5);                //waiting time for security
  digitalWrite(POWER_RELAY,LOW); //power ON the module DFRobot Relay (0.8mA at 3.3V in static current)
  delay(10);               //waiting time for the module to be properly powered
  digitalWrite(R_On,HIGH); //start of signal pulse
  delay(5);                //signal pulse 5ms (manufacturer specifies >= 2ms) current is approx 150mA at 3.3V
  digitalWrite(R_On,LOW);  //end of signal pulse
  delay(10);               //time to wait for the relay action to be completed (manufacturer specifies <= 8ms)
  digitalWrite(POWER_RELAY,HIGH); //power OFF the module DFRobot Relay (<1µA)
}

void setup() {
  rtc_gpio_hold_dis(GPIO_NUM_25); //disactive hold RTC GPIO25, otherwise it is impossible to change its state

  pinMode(POWER_RELAY,OUTPUT);    //GPIO in OUTPUT mode
  digitalWrite(POWER_RELAY,HIGH); //HIGH = module DFRobot Relay OFF
  pinMode(A_COM,OUTPUT);          //GPIO in OUTPUT mode
  digitalWrite(A_COM,LOW);
  pinMode(B_COM,OUTPUT);          //GPIO in OUTPUT mode
  digitalWrite(B_COM,LOW);

  delay(500);
  Switch_Relay(A_COM);
  delay(500);
  Switch_Relay(B_COM);
  delay(500);
  Switch_Relay(A_COM);
  delay(500);
  Switch_Relay(B_COM);
  delay(500);
  
  rtc_gpio_init(GPIO_NUM_25);             // Init RTC GPIO 25
  rtc_gpio_set_direction(GPIO_NUM_25, RTC_GPIO_MODE_OUTPUT_ONLY);  // Set as OUTPUT mode
  rtc_gpio_set_level(GPIO_NUM_25, HIGH);  // Set to HIGH mode (3.3V)
  rtc_gpio_hold_en(GPIO_NUM_25);          // enable hold RTC to maintain it's state (HIGH) during deespsleep
  
  esp_sleep_enable_timer_wakeup(10000000); //timer wakeup = 10 seconds
  esp_deep_sleep_start(); //start deepsleep mode
}  

void loop() {
}
