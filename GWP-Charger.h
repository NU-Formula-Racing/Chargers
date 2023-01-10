#pragma once
#include <array>

class GWPCharger
{
public:
    GWPCharger(ICAN &can_interface);

    void Enable();
    void Disable();
    void SetVoltageAndCurrent(float voltage, float current);
    std::array<float, 2> GetVoltageAndCurrent();

    void Tick(); // to keep charger running if enabled

    // function to get actual output voltage
    // function to get actual output current
    // function to get temperature
    // function to get actual output power
private:
};