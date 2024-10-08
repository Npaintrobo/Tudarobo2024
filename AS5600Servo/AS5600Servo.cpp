#include "AS5600Servo.h"

AS5600Servo::AS5600Servo(byte deviceAddress) {
    _deviceAddress = deviceAddress;
    pretime = 0;
    pree = 0;
    I = 0;
    _kp = 10;  // デフォルト値
    _ki = 0;  // デフォルト値
    _kd = 0.03; // デフォルト値
}

void AS5600Servo::begin() {
    Wire.begin();
    pretime = micros();
}

uint16_t AS5600Servo::readAngle() {
    return readData(RAW_ANGLE_ADDR);
}

uint16_t AS5600Servo::readData(byte registerAddress) {
    Wire.beginTransmission(_deviceAddress);
    Wire.write(registerAddress);
    Wire.endTransmission();
    Wire.requestFrom(_deviceAddress, (byte)2);

    if (Wire.available() >= 2) {
        uint8_t highByte = Wire.read();
        uint8_t lowByte = Wire.read();
        return (highByte << 8) | lowByte;
    }
    return 0;
}

void AS5600Servo::setPID(double kp, double ki, double kd) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

double AS5600Servo::computePID(double targetAngle) {
    double angle = getAngle();  // 読み取った角度データを変換
    double e = targetAngle - angle;  // 誤差を計算

    if (abs(targetAngle - angle) > 180) {
        if (targetAngle < 180) e = (0 - angle) * -1;
        else if (targetAngle > 180) e = (360 - angle) * -1;
    }

    double dt = (micros() - pretime) / 1000000.0;  // 時間差を秒単位で計算
    pretime = micros();  // 現在の時間を保存

    I += e * dt;  // 積分項の更新
    double D = (e - pree) / dt;  // 微分項の計算
    pree = e;  // 前回の誤差を更新

    double out = e * _kp + I * _ki + D * _kd;  // PID制御の出力を計算
    return out;
}

double AS5600Servo::getAngle() {
    uint16_t rawAngle = readAngle();
    double angle = rawAngle / 11.375;
    return angle;
}
