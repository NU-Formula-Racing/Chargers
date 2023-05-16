#pragma once
#include <array>
#include "can_interface.h"
#include "virtualTimer.h"
#include "I-Charger.h"

class ElconCharger : public ICharger
{
public:
    ElconCharger(ICAN &can_interface, VirtualTimerGroup timer_group) : can_interface_{can_interface}, timer_group_(timer_group) {}

    union Status
    {
        uint8_t all_flags = 0;
        struct flags
        {
            bool HardwareFailure : 1;
            bool OverTemperature : 1;
            bool InputVoltageFault : 1;
            bool StartingState : 1;
            bool CommunicationTimeout : 1;
            bool res1 : 1;
            bool res2 : 1;
            bool res3 : 1;
        } flags;

        Status() = default;
        operator uint8_t() const { return all_flags; }
        Status(uint8_t s) : all_flags{s} {}
    };

    Status GetFaults()
    {
        return Status_Flags;
    }

    void Enable()
    {
        Control = 0;
    }
    void Disable()
    {
        Control = 1;
    }
    void SetVoltageCurrent(float voltage, float current)
    {
        Max_Allowable_Charging_Terminal_Voltage = voltage;
        Max_Allowable_Charging_Current = current;
    }

    float GetOutputVoltage()
    {
        return Output_Voltage * 2;
    }

    float GetOutputCurrent()
    {
        return Output_Current * 2;
    }

    void Tick(uint32_t current_time)
    {
        Message1.GetTransmitTimer().Tick(current_time);
    }

    float GetPowerReference()
    {
        return Max_Allowable_Charging_Terminal_Voltage * Max_Allowable_Charging_Current * 4;
    }

    float GetAvailablePower()
    {
        return GetPowerReference() - (GetOutputCurrent() * GetOutputVoltage());
    }

private:
    ICAN &can_interface_;
    VirtualTimerGroup timer_group_{};

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Max_Allowable_Charging_Terminal_Voltage{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Max_Allowable_Charging_Current{};
    MakeUnsignedCANSignal(bool, 32, 1, 1, 0) Control{};
    CANTXMessage<3> Message1{can_interface_, 0x1806E5F4, 8, 500, timer_group_, Max_Allowable_Charging_Terminal_Voltage,
                             Max_Allowable_Charging_Current, Control};

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Output_Voltage{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Output_Current{};
    MakeUnsignedCANSignal(Status, 32, 8, 1, 0) Status_Flags{};

    CANRXMessage<3> Message2{can_interface_, 0x18FF50E5, Output_Voltage, Output_Current,
                             Status_Flags};
};