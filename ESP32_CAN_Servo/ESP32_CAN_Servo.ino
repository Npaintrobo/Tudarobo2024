#include <PS4Controller.h>
#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include <ESP32Servo.h>
#include <stdlib.h>

#define CAN_TX       21
#define CAN_RX       22
#define TARGET_ID    0x122

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

CanFrame rxFrame; 

const int servo1_pin = 32;
const int servo2_pin = 33;
const int servo3_pin = 14;
const int servo4_pin = 27;

int servo_deg1=180,servo_deg2=180,servo_deg3=180,servo_deg4=180;

void setup(){
  setCpuFrequencyMhz(160);
  Serial.begin(115200);
  if (ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  servo3.attach(servo3_pin);
  servo4.attach(servo4_pin);
}

void can_deg(){
  char deg1[4];
  char deg2[4];
  char deg3[4];
  char deg4[4];
  if (ESP32Can.readFrame(rxFrame, 16)) {
    if (rxFrame.identifier == TARGET_ID) {
      for (int i = 0; i < rxFrame.data_length_code; i++) {
        char c = (char)rxFrame.data[i];
      }
    }
  }
  strncpy(deg1, (char*)rxFrame.data, 3);
  deg1[3] = '\0'; 
  strncpy(deg2, (char*)rxFrame.data + 3, 3); 
  deg2[3] = '\0'; 
  strncpy(deg3, (char*)rxFrame.data + 6, 3);
  deg3[3] = '\0'; 
  strncpy(deg4, (char*)rxFrame.data + 9, 3); 
  deg4[3] = '\0'; 
  
  servo_deg1 = atoi(deg1);
  servo_deg2 = atoi(deg2);
  servo_deg3 = atoi(deg3);
  servo_deg4 = atoi(deg4);
}

void loop(){
  can_deg();
  servo1.write(servo_deg1);
  servo2.write(servo_deg2);
  servo3.write(servo_deg3);
  servo4.write(servo_deg4);
}