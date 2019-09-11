#include "wheel.h"

Wheel::Wheel(Motor &_FR, Motor &_FL, Motor &_RR, Motor &_RL, uint32_t _pow) : FR(_FR), FL(_FL), RR(_RR), RL(_RL), pow(_pow)
{
}

void Wheel::Brake(void)
{
    FR.Brake();
    FL.Brake();
    RR.Brake();
    RL.Brake();
}

void Wheel::North(uint32_t pulsewidth)
{
    FR.CW(pulsewidth);
    FL.CCW(pulsewidth);
    RR.CW(pulsewidth);
    RL.CCW(pulsewidth);
}

void Wheel::East(uint32_t pulsewidth)
{
    FR.CCW(pulsewidth);
    FL.CCW(pulsewidth);
    RR.CW(pulsewidth);
    RL.CW(pulsewidth);
}

void Wheel::West(uint32_t pulsewidth)
{
    FR.CW(pulsewidth);
    FL.CW(pulsewidth);
    RR.CCW(pulsewidth);
    RL.CCW(pulsewidth);
}

void Wheel::South(uint32_t pulsewidth)
{
    FR.CCW(pulsewidth);
    FL.CW(pulsewidth);
    RR.CCW(pulsewidth);
    RL.CW(pulsewidth);
}

void Wheel::NorthEast(uint32_t pulsewidth)
{
    FR.CCW(pulsewidth);
    FL.Brake();
    RR.Brake();
    RL.CW(pulsewidth);
}

void Wheel::NorthWest(uint32_t pulsewidth)
{
    FR.Brake();
    FL.CW(pulsewidth);
    RR.CCW(pulsewidth);
    RL.Brake();
}

void Wheel::SouthEast(uint32_t pulsewidth)
{
    FR.Brake();
    FL.CCW(pulsewidth);
    RR.CW(pulsewidth);
    RL.Brake();
}

void Wheel::SouthWest(uint32_t pulsewidth)
{
    FR.CW(pulsewidth);
    FL.Brake();
    RR.Brake();
    RL.CCW(pulsewidth);
}

