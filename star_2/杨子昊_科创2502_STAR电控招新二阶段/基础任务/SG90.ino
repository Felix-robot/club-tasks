#include <Servo.h>  // 包含舵机控制库

Servo myservo;      // 创建舵机对象
int servoPin = 11;   // 舵机信号线连接的数字引脚

// 目标角度数组
int angles[] = {0, 30, 45, 90, 135};
int angleCount = 5; // 角度数量

void setup() {
  myservo.attach(servoPin);  // 将舵机连接到指定引脚
}

void loop() {
  // 遍历所有角度
  for(int i = 0; i < angleCount; i++) {
    myservo.write(angles[i]); // 转动到指定角度
    delay(1000);              // 保持1秒
  }
}