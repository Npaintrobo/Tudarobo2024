#include <PS4Controller.h>
#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include <ESP32Servo.h>

#define CAN_TX       21
#define CAN_RX       22

const int servo1 = 32;
const int servo2 = 33;
const int servo3 = 14;
const int servo4 = 27;

void setup(){
  setCpuFrequencyMhz(160);
  Serial.begin(115200);
  if (ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
  pinMode(servo1,OUTPUT);//サーボ1
  pinMode(servo2,OUTPUT);//サーボ2
  pinMode(servo3,OUTPUT);//サーボ3
  pinMode(servo4,OUTPUT);//サーボ4
}

void loop(){

}