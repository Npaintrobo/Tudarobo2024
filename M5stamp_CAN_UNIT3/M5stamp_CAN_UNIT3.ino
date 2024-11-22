#include <Arduino.h>
#include <stdlib.h>
#include <M5Unified.h>
#include <ESP32-TWAI-CAN.hpp>
#include <string.h>
#include "AS5600Servo.h"

#define CAN_TX       0
#define CAN_RX       1
#define TARGET_ID     0x125
AS5600Servo sensor1(0x36);  // AS5600センサー1のアドレス

const int pwm1_1 = 5; // drive1
const int pwm1_2 = 3; // drive2
const int pwm2_1 = 9; // steering1
const int pwm2_2 = 7; // steering2

int drive;
int target_angle;

char drive_c[4];
char target_angle_c[4];

CanFrame rxFrame; 

void setup() {
  setCpuFrequencyMhz(160);
  M5.begin(); 
  USBSerial.begin(115200); 

  if (ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
    USBSerial.println("CAN bus started!");
  } else {
    USBSerial.println("CAN bus failed!");
  }

  sensor1.begin();
  sensor1.setPID(18, 0, 0);  // センサ1のPIDゲイン

  ledcSetup(0, 20000, 10); // PWM周波数=20000Hz, ビット幅=10bit 0チャンネル
  ledcSetup(1, 20000, 10); // PWM周波数=20000Hz, ビット幅=10bit 1チャンネル
  ledcSetup(2, 20000, 10); // PWM周波数=20000Hz, ビット幅=10bit 2チャンネル
  ledcSetup(3, 20000, 10); // PWM周波数=20000Hz, ビット幅=10bit 3チャンネル

  ledcAttachPin(pwm1_1, 0); // pwm1_1ピンにPWM0チャンネルをアタッチ
  ledcAttachPin(pwm1_2, 1); // pwm1_2ピンにPWM1チャンネルをアタッチ
  ledcAttachPin(pwm2_1, 2); // pwm2_1ピンにPWM2チャンネルをアタッチ
  ledcAttachPin(pwm2_2, 3); // pwm2_2ピンにPWM3チャンネルをアタッチ
  
  // 初期化
  digitalWrite(pwm1_1, LOW);
  digitalWrite(pwm1_2, LOW);
  digitalWrite(pwm2_1, LOW);
  digitalWrite(pwm2_2, LOW);
}

void loop() {
  if (ESP32Can.readFrame(rxFrame, 10)) {
    // CAN ID が TARGET_ID と一致するか確認
    if (rxFrame.identifier == TARGET_ID) {
      // 一致する場合、受信データをシリアルに表示
      USBSerial.print("Received CAN ID: ");
      USBSerial.println(rxFrame.identifier, HEX);
      
      // データを文字列に変換して表示
      USBSerial.print("Data: ");
      for (int i = 0; i < rxFrame.data_length_code; i++) {
        char c = (char)rxFrame.data[i];
        if (isPrintable(c)) {
          USBSerial.print(c);
        } else {
          USBSerial.print('.');
        }
      }
      USBSerial.println();

      strncpy(target_angle_c, (char*)rxFrame.data, 3);
      target_angle_c[3] = '\0'; 

      strncpy(drive_c, (char*)rxFrame.data + 3, 3); 
      drive_c[3] = '\0'; 

      target_angle = atoi(target_angle_c); 
      drive = atoi(drive_c); 

      drive = map(drive, 0, 460, 0, 800);
      target_angle = map(target_angle,0,360,360,0);
      if(target_angle == 360)target_angle = 0;
      double x = sensor1.getAngle();
      USBSerial.print("Target Angle: ");
      USBSerial.print(target_angle);
      USBSerial.print("Angle: ");
      USBSerial.print(x);
      USBSerial.print(", Drive: ");
      USBSerial.println(drive);
      
      // PID制御の出力を計算
      auto [output, drive_flag] = sensor1.computePID(target_angle); 

      if(drive_flag = 0){
        digitalWrite(pwm1_1, HIGH);
        digitalWrite(pwm1_2, LOW);
        ledcWrite(0, drive);
      }
      if(drive_flag = 1){
        digitalWrite(pwm1_1, LOW);
        digitalWrite(pwm1_2, HIGH);
        ledcWrite(1, drive);
      }

      if(output > 1000)output = 1000;
      if(output < -1000)output = -1000;
      USBSerial.printf("output");
      USBSerial.println(output);

      // outputに基づいてモーター制御
      if (output < 0) {
          digitalWrite(pwm2_1, LOW);
          digitalWrite(pwm2_2, HIGH);
          ledcWrite(3, output * -1);
      } else if (output > 0) {
          digitalWrite(pwm2_1, HIGH);
          digitalWrite(pwm2_2, LOW);
          ledcWrite(2, (output));
      } else {
          digitalWrite(pwm2_1, LOW);
          digitalWrite(pwm2_2, LOW);
          ledcWrite(2, 0);
          ledcWrite(3, 0);
      }
    }
  }
}