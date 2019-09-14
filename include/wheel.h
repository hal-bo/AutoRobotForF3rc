#ifndef WHEEL_H
#define WHEEL_H

#include <mbed.h>
#include "motor.h"

/* 
 * オムニホイール4輪を制御するためのクラス．
 * motor.hをインクルードする前提．
 * メカナム4輪や，オムニ3輪も少し改造すれば簡単に実装できるはず．
 */

class Wheel
{
private:
    Motor &FR, &FL, &RR, &RL; // FrontRight, FrontLeft, RearRight, RearLeft
    const uint32_t pow;

public:
    /* Constructor */
    Wheel(Motor &, Motor &, Motor &, Motor &, uint32_t pow);

    void Brake(void);
    void North(uint32_t PulseWidth);
    void East(uint32_t PulseWidth);
    void West(uint32_t PulseWidth);
    void South(uint32_t PulseWidth);
    void NorthEast(uint32_t PulseWidth);
    void NorthWest(uint32_t PulseWidth);
    void SouthEast(uint32_t PulseWidth);
    void SouthWest(uint32_t PulseWidth);
    void RotateRight(uint32_t PulseWidth);
    void RotateLeft(uint32_t PulseWidth);

};

#endif /* WHEEL_H */