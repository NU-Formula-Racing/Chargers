#pragma once
#include <array>
#include "can_interface.h"
#include "virtualTimer.h"
#include "I-Charger.h"

class GWPCharger : public ICharger
{
public:
    GWPCharger(ICAN &can_interface) : can_interface_{can_interface} {}

    union Status
    {
        uint16_t all_flags = 0;
        struct flags
        {
            bool OUT_OC : 1;
            bool OUT_OV : 1;
            bool OUT_SC : 1;
            bool OUT_UV : 1;
            bool CASE_OT : 1;
            bool PFC_OT : 1;
            bool D2D_OT : 1;
            bool AC_OV : 1;
            bool AC_UV : 1;
            bool SCI_TO : 1;
            bool BUS_ERR : 1;

        } flags;

        Status() = default;
        operator uint16_t() const { return all_flags; }
        Status(uint16_t s) : all_flags{s} {}
    };

    Status GetFaults()
    {
        return Status_Flags;
    }

    void Enable()
    {
        C_ENABLE = 1;
    }
    void Disable()
    {
        C_ENABLE = 0;
    }
    void SetVoltageCurrent(float voltage, float current)
    {
        C_MAX_PWR_Set = current * voltage;
        C_MAX_Vo_Set = voltage;
        C_MAX_Io_Set = current;
    }

    void Tick(uint32_t current_time)
    {
        CHG_Individual_Ctrl.GetTransmitTimer().Tick(current_time);
    }

    float GetOutputVoltage()
    {
        return C_Vo;
    }

    float GetOutputCurrent()
    {
        return C_Io;
    }

    float GetPrimaryTemperature()
    {
        return C_P_TEMP;
    }

    float GetSecondaryTemperature()
    {
        return C_S_TEMP;
    }

    float GetInputVoltage()
    {
        return C_Vac;
    }

    float GetInputCurrent()
    {
        return C_Iac;
    }

    float GetPowerReference()
    {
        return C_PWR_REF;
    }

    float GetAvailablePower()
    {
        return C_AVA_PWR;
    }

private:
    ICAN &can_interface_;

    MakeUnsignedCANSignal(uint16_t, 0, 8, 1, 0) C_ENABLE{};
    MakeUnsignedCANSignal(uint16_t, 8, 16, 0.1, 0) C_PWR_REF{};
    MakeUnsignedCANSignal(uint16_t, 24, 16, 0.1, 0) C_MAX_Vo_Set{};
    MakeUnsignedCANSignal(uint16_t, 40, 16, 0.1, 0) C_MAX_Io_Set{};
    MakeUnsignedCANSignal(uint16_t, 56, 8, 1, 0) C_RES{};
    CANTXMessage<5> CHG_Individual_Ctrl{can_interface_, 0x300, 8, 500, C_ENABLE, C_MAX_Io_Set, C_MAX_Vo_Set, C_PWR_REF, C_RES};

    MakeUnsignedCANSignal(uint16_t, 0, 8, 1, 0) C_STATUS{};
    MakeUnsignedCANSignal(uint16_t, 8, 16, 0.1, 0) C_Iac{};
    MakeUnsignedCANSignal(uint16_t, 24, 16, 0.1, 0) C_Io{};
    MakeUnsignedCANSignal(uint16_t, 40, 16, 0.1, 0) C_Vo{};
    MakeUnsignedCANSignal(uint16_t, 56, 8, 1, 0) C_MAINS_F{};
    CANRXMessage<5> CHG_Status_1{can_interface_, 0x305, C_Iac, C_Io, C_MAINS_F, C_STATUS, C_Vo};

    MakeSignedCANSignal(int8_t, 0, 8, 1, 0) C_P_TEMP{};
    MakeSignedCANSignal(int8_t, 8, 8, 1, 0) C_S_TEMP{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 1, 0) C_Vac{};
    MakeUnsignedCANSignal(uint16_t, 32, 16, 0.1, 0) C_MAX_PWR_Set{};
    MakeUnsignedCANSignal(uint16_t, 48, 8, 0.1, 0) C_AVA_PWR{};
    MakeUnsignedCANSignal(uint16_t, 56, 8, 1, 0) Reserved{};
    CANRXMessage<6> CHG_Status_2{can_interface_, 0x306, C_AVA_PWR, C_MAX_PWR_Set, C_P_TEMP, C_S_TEMP, C_Vac, Reserved};

    MakeUnsignedCANSignal(Status, 0, 16, 1, 0) Status_Flags{};
    CANRXMessage<1> CHG_Error{can_interface_, 0x307, Status_Flags};
};