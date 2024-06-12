#pragma once
#include <array>

#include "I-Charger.h"
#include "can_interface.h"
#include "virtualTimer.h"

class ElconCharger : public ICharger
{
public:
    ElconCharger(ICAN &can_interface, float max_wattage, float max_current)
        : can_interface_{can_interface}, max_wattage_{max_wattage}, max_current_{max_current}
    {
    }

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

    Status GetFaults() { return Status_Flags; }

    VirtualTimerGroup timer_group{};

    void Initialize()
    {
        can_interface_.RegisterRXMessage(Feedback_);
    }
    void Enable() { Control = 0; }
    void Disable() { Control = 1; }
    void SetVoltageCurrent(float voltage, float current)
    {
        Max_Allowable_Charging_Terminal_Voltage = voltage;
        Max_Allowable_Charging_Current =
            std::min<float>(current, std::min<float>(max_current_, max_wattage_ / Output_Voltage));
    }

    float GetOutputVoltage() { return Output_Voltage * 2; }

    float GetOutputCurrent() { return Output_Current * 2; }

    void Tick(uint32_t current_time)
    {
        timer_group.Tick(current_time);
    }

    float GetPower() { return Max_Allowable_Charging_Terminal_Voltage * Max_Allowable_Charging_Current * 4; }

    float GetAvailablePower() { return GetPower() - (GetOutputCurrent() * GetOutputVoltage()); }

    bool IsConnected()
    {
        return Feedback_.GetLastReceiveTime() != 0 && Feedback_.GetTimeSinceLastReceive() < 2000; // connection timeout in 2 seconds
    }

private:
    ICAN &can_interface_;
    float max_wattage_;
    float max_current_;

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Max_Allowable_Charging_Terminal_Voltage{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Max_Allowable_Charging_Current{};
    MakeUnsignedCANSignal(bool, 32, 1, 1, 0) Control{};
    CANTXMessage<3> Command_{can_interface_,
                             0x1806E5F4,
                             true,
                             8,
                             500,
                             timer_group,
                             Max_Allowable_Charging_Terminal_Voltage,
                             Max_Allowable_Charging_Current,
                             Control};

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Output_Voltage{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Output_Current{};
    MakeUnsignedCANSignal(Status, 32, 8, 1, 0) Status_Flags{};

    CANRXMessage<3> Feedback_{can_interface_, 0x18FF50E5, Output_Voltage, Output_Current, Status_Flags};
};