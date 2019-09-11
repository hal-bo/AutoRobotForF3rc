#include "motor.h"

Motor::Motor(PwmOut &PWM_Pin, DigitalOut &Phase_Pin, uint32_t pwm_period = 100, bool direction = true) : PWM(PWM_Pin), PHASE(Phase_Pin), PWM_PERIOD(pwm_period), DIRECTION(direction)
{
    PWM.period_us(PWM_PERIOD);
    PWM.pulsewidth_us(0);
}

void Motor::Brake(void)
{
    PWM.pulsewidth_us(0);
}

void Motor::CW(uint32_t pulsewidth) // Clockwise
{
    if (pulsewidth > PWM_PERIOD * 0.95)
        pulsewidth = PWM_PERIOD * 0.95;

    PWM.pulsewidth_us(pulsewidth);
    PHASE.write(DIRECTION); // trueでCWと定義
}

void Motor::CCW(uint32_t pulsewidth) // Counterclockwise
{
    if (pulsewidth > PWM_PERIOD * 0.95)
        pulsewidth = PWM_PERIOD * 0.95;

    PWM.pulsewidth_us(pulsewidth);
    PHASE.write(!DIRECTION); // falseでCCWと定義
}