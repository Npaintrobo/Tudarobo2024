#include <PS4Controller.h>
#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

#define CAN_TX       21
#define CAN_RX       22

void send_CAN(int ID, byte value[], int length) {
  CanFrame obdFrame = { 0 };
  obdFrame.identifier = ID;
  obdFrame.extd = 0;
  obdFrame.data_length_code = length;
  for (int i = 0; i < length; i++) {
    obdFrame.data[i] = value[i];
  }
  ESP32Can.writeFrame(obdFrame);
}

void setup() {
  setCpuFrequencyMhz(160);
  Serial.begin(115200);
  if (ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
  } else {
    Serial.println("CAN bus failed!");
  }
  PS4.begin();
  Serial.println("Ready.");
  pinMode(32,OUTPUT);//ハンド1 開
  pinMode(33,OUTPUT);//ハンド2 閉
  pinMode(14,OUTPUT);//上下機構1上昇
  pinMode(27,OUTPUT);//上下機構2下降
}

void send_deg_duty(int ID, int deg, int duty_ratio) {
  char message[7]; // 6桁+null文字
  snprintf(message, sizeof(message), "%03d%03d", deg, duty_ratio); // "359099" のような文字列を作成
  // バイト配列に変換
  byte data[6];
  for (int i = 0; i < 6; i++) {
    data[i] = message[i];
  }
  send_CAN(ID, data, sizeof(data));// CANメッセージを送信
}

void send_servo(int ID, int servo_deg1, int servo_deg2, int servo_deg3, int servo_deg4) {
  char message[13]; // 12桁+null文字
  snprintf(message, sizeof(message), "%03d%03d%03d%03d", servo_deg1, servo_deg2, servo_deg3, servo_deg4); 
  // バイト配列に変換
  byte data[6];
  for (int i = 0; i < 12; i++) {
    data[i] = message[i];
  }
  send_CAN(ID, data, sizeof(data));// CANメッセージを送信
}


void loop() {
  if (PS4.isConnected()) {
    int x_L,y_L,x_R,duty_ratio,deg,deg1,deg2,deg3,servo_deg1,servo_deg2,servo_deg3,servo_deg4;
    x_L = PS4.LStickX();
    y_L = PS4.LStickY();
    if(PS4.RStickX() > 20 && PS4.RStickX() < -20){
      //旋回移動
      x_R = PS4.RStickX();
      duty_ratio = abs(x_R);
      if(x_R >= 0)deg1 = 0;
      else deg1 = 360;
      if(x_R >= 0)deg1 = 60;
      else deg1 = 120;
      if(x_R >= 0)deg1 = 120;
      else deg1 = 240;
    }
    else{
      //全方向移動
      duty_ratio = sqrt((x_L * x_L) + (y_L * y_L));
      if(duty_ratio < 65)duty_ratio = 0;
      else if(duty_ratio >= 65)duty_ratio = (duty_ratio - 65);
      deg = -(atan2(-x_L,-y_L)*180)/PI;
      if(deg < 0)deg = 180 + (180 + deg);
      deg1 = deg;
      deg2 = deg;
      deg3 = deg;
    }
    send_servo(0x122,servo_deg1, servo_deg2, servo_deg3, servo_deg4);
    send_deg_duty(0x123, deg1, duty_ratio);//Unit1
    send_deg_duty(0x124, deg2, duty_ratio);//Unit2
    send_deg_duty(0x125, deg3, duty_ratio);//Unit3
    //マイナスブロック用ハンドを開く
    if(PS4.Up()&&PS4.Down()==0)digitalWrite(32,HIGH);
    else digitalWrite(32,LOW);
    //マイナスブロック用ハンド閉じる
    if(PS4.Up()==0&&PS4.Down())digitalWrite(33,HIGH);
    else digitalWrite(33,LOW);
    //プラスブロック用アーム上昇
    if(PS4.Triangle()&&PS4.Cross()==0)digitalWrite(14,HIGH);
    else digitalWrite(14,LOW);
    //プラスブロック用アーム下降
    if(PS4.Triangle()==0&&PS4.Cross())digitalWrite(27,HIGH);
    else digitalWrite(27,LOW);
  }
  
  else{
    digitalWrite(32,LOW);
    digitalWrite(33,LOW);
    digitalWrite(14,LOW);
    digitalWrite(27,LOW);
  }

}
