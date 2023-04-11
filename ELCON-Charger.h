#pragma once
#include <array>
#include "can_interface.h"
#include "virtualTimer.h"
#include "I-Charger.h"

class ElconCharger : public ICharger
{
public:
    ElconCharger(ICAN &can_interface) : can_interface_{can_interface} {}

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
    void SetVoltageCurrentAndPower(float voltage, float current)
    {
        Max_Allowable_Charging_Terminal_Voltage_High_Byte = voltage;
        Max_Allowable_Charging_Current_High_Byte = current;
    }

    void Tick(uint32_t current_time)
    {
        CHG_Individual_Ctrl.GetTransmitTimer().Tick(current_time);
    }

    float GetOutputVoltage()
    {
        return Output_Voltage_High_Byte * 2;
    }

    float GetOutputCurrent()
    {
        return Output_Current_High_Byte * 2;
    }

    void Tick(uint32_t current_time)
    {
        CHG_Individual_Ctrl.GetTransmitTimer().Tick(current_time);
    }

    float GetPrimaryTemperature()
    {
        return 20; // no primary temperature signal in ELCON
    }

    float GetSecondaryTemperature()
    {
        return 20; // no secondary temperature signal in ELCON
    }

    float GetInputVoltage()
    {
        return 0; // no input voltage signal in ELCON
    }

    float GetInputCurrent()
    {
        return 0; ////no input voltage signal in ELCON
    }

    float GetPowerReference()
    {
        return Max_Allowable_Charging_Terminal_Voltage_High_Byte * Max_Allowable_Charging_Current_High_Byte * 4;
    }

    float GetAvailablePower()
    {
        return GetPowerReference() - (GetOutputCurrent() * GetOutputVoltage());
    }

private:
    ICAN &can_interface_;

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Max_Allowable_Charging_Terminal_Voltage{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Max_Allowable_Charging_Current{};
    MakeUnsignedCANSignal(bool, 32, 1, 1, 0) Control{};
    CANTXMessage<3> Message1{can_interface, 0x1806E5F4, 8, 500, Max_Allowable_Charging_Terminal_Voltage,
                             Max_Allowable_Charging_Current, Control};

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Output_Voltage{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Output_Current{};
};
MakeUnsignedCANSignal(Status, 32, 8, 0, 0) Status_Flags{};

CANRXMessage<3> Message2{can_interface, 0x18FF50E5, Output_Voltage, Output_Current,
                         Status_Flags};
}
;