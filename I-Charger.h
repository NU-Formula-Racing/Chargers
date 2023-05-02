#pragma once
#include <array>
#include <vector>

class ICharger
{
public:
    virtual void Enable() = 0;

    virtual void Disable() = 0;

    virtual void SetVoltageCurrent(float voltage, float current) = 0;

    virtual void Tick(uint32_t current_time) = 0;

    virtual float GetOutputVoltage() = 0;

    virtual float GetOutputCurrent() = 0;

    virtual float GetPower() = 0;

    virtual float GetAvailablePower() = 0;
};
