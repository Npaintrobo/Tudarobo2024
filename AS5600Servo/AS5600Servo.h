#ifndef AS5600SERVO_H
#define AS5600SERVO_H

#include <Arduino.h>  // Arduinoの標準ライブラリをインクルード
#include <Wire.h>     // I2C通信ライブラリをインクルード

class AS5600Servo {
public:
    AS5600Servo(byte deviceAddress);  // センサのI2Cアドレスを受け取る

    void begin();
    uint16_t readAngle();
    void setPID(double kp, double ki, double kd);  // PIDゲイン
    double computePID(double targetAngle);  // PID制御出力を計算する関数
    double getAngle();  // 角度を計算して返す関数

private:
    byte _deviceAddress;  // デバイスのI2Cアドレス

    // PID制御用の変数
    double pretime;  // 前回の時間
    double pree;     // 前回の誤差
    double I;        // 積分項
    double _kp;      // 比例ゲイン
    double _ki;      // 積分ゲイン
    double _kd;      // 微分ゲイン

    const byte RAW_ANGLE_ADDR = 0x0C;  // 角度データが格納されているレジスタのアドレス

    uint16_t readData(byte registerAddress);  // センサのレジスタからデータを読み取る関数
};

#endif
