#ifndef MOTOR_H
#define MOTOR_H

#include <mbed.h>

/*
 * 2019年F3RC
 * Last modified Aug 29, 2019
 * MDでモーター1つを回すためのクラス．PWMピン1本で速度を指定． GPIOピン1本で回転方向を指定．
 * このMDの場合はDuty比を100%にしても問題ないが，たいていのMDは燃える可能性があるので注意．
 * MDの詳細は https://kra.kibe.la/notes/74 を参照のこと
 * このMDはPWM周波数を 10 kHz = 100 us あたりで使うこと
 */

class Motor
{
private:
    PwmOut &PWM;       // main関数内で使うピンを指定して初期化
    DigitalOut &PHASE; // main関数内で使うピンを指定して初期化
    const uint32_t PWM_PERIOD;
    const bool DIRECTION; // trueのときCWと定義 モーターのつなぎ方によってfalseにする

public:
    /* Constructor */
    Motor(PwmOut &PWM_Pin, DigitalOut &Phase_Pin, uint32_t pwm_period, bool direction);

    void Brake(void);
    void CW(uint32_t pulsewidth);  // Clockwise
    void CCW(uint32_t pulsewidth); // Counterclockwise
};

#endif /* MOTOR_H */