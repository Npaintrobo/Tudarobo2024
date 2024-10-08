#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin();
  Serial.println("Ready.");
  pinMode(32,OUTPUT);//ハンド1 開
  pinMode(33,OUTPUT);//ハンド2 閉
  pinMode(14,OUTPUT);//上下機構1上昇
  pinMode(27,OUTPUT);//上下機構2下降
}

void loop() {
  if (PS4.isConnected()) {
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
