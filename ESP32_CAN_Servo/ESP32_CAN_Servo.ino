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

CanFrame rxFrame; 

const int servo1_pin = 32;
const int servo2_pin = 33;

int servo_deg1 = 180, servo_deg2 = 180;

void setup(){
  setCpuFrequencyMhz(160);
  Serial.begin(115200);
  
  if (ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }

  // サーボのピン設定
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
}

void can_deg(){
  if (ESP32Can.readFrame(rxFrame, 10)) {
    if (rxFrame.identifier == TARGET_ID) {
      // 受信データが6バイトであることを確認
      if (rxFrame.data_length_code == 6) {
        char deg1[4] = {0};  // サーボ1用のデータ
        char deg2[4] = {0};  // サーボ2用のデータ
        
        // 最初の3バイトをサーボ1の角度としてコピー
        strncpy(deg1, (char*)rxFrame.data, 3);
        deg1[3] = '\0';  // 終端文字を追加
        
        // 次の3バイトをサーボ2の角度としてコピー
        strncpy(deg2, (char*)rxFrame.data + 3, 3);
        deg2[3] = '\0';  // 終端文字を追加

        // 角度を整数に変換し、範囲をチェック
        servo_deg1 = constrain(atoi(deg1), 0, 180); // 360度までの範囲
        servo_deg2 = constrain(atoi(deg2), 0, 180); // 360度までの範囲

        // 受信データとサーボ角度をデバッグ出力
        Serial.print("Received data: ");
        Serial.print(deg1);
        Serial.print(" ");
        Serial.println(deg2);

        Serial.print("Servo 1 Angle: ");
        Serial.println(servo_deg1);
        Serial.print("Servo 2 Angle: ");
        Serial.println(servo_deg2);
      } else {
        Serial.println("Invalid data length received!");
      }
    }
  }
}

void loop(){
  can_deg(); // CANデータを読み取って角度を更新
  servo1.write(servo_deg1); // サーボ1に角度を設定
  servo2.write(servo_deg2); // サーボ2に角度を設定
  delay(100); // 100ms待機
}
