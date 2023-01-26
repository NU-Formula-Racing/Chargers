#pragma once
#include <array>
#include "can_interface.h"
#include "virtualTimer.h"

class GWPCharger
{
public:
    GWPCharger(ICAN &can_interface) : can_interface_{can_interface} {}

    enum class FaultPositions : int
    {
        OUT_OC = 0,
        OUT_OV,
        OUT_SC,
        OUT_UV,
        CASE_OT,
        PFC_OT,
        D2D_OT,
        CAN_TO,
        AC_OV,
        AC_UV,
        SCI_TO,
        BUS_ERR,

    };

    void Enable();
    void Disable();
    void SetVoltageCurrentAndPower(float voltage, float current, float power)
    {
        C_MAX_PWR_Set = power;
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

    uint16_t GetFaults()
    {
        uint16_t faults = 0;
        faults |= bool(OUT_OC) << int(FaultPositions::OUT_OC);
        faults |= bool(OUT_OV) << int(FaultPositions::OUT_OV);
        faults |= bool(OUT_SC) << int(FaultPositions::OUT_SC);
        faults |= bool(OUT_UV) << int(FaultPositions::OUT_UV);
        faults |= bool(CASE_OT) << int(FaultPositions::CASE_OT);
        faults |= bool(PFC_OT) << int(FaultPositions::PFC_OT);
        faults |= bool(D2D_OT) << int(FaultPositions::D2D_OT);
        faults |= bool(CAN_TO) << int(FaultPositions::CAN_TO);
        faults |= bool(AC_OV) << int(FaultPositions::AC_OV);
        faults |= bool(AC_UV) << int(FaultPositions::AC_UV);
        faults |= bool(SCI_TO) << int(FaultPositions::SCI_TO);
        faults |= bool(BUS_ERR) << int(FaultPositions::BUS_ERR);

        return faults;
    }

private:
    ICAN &can_interface_;

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_ENABLE{};
    CANSignal<uint16_t, 8, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_PWR_REF{};
    CANSignal<uint16_t, 24, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_MAX_Vo_Set{};
    CANSignal<uint16_t, 40, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_MAX_Io_Set{};
    CANSignal<uint8_t, 56, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_RES{};
    CANTXMessage<5> CHG_Individual_Ctrl{can_interface_, 0x300, 8, 500, C_ENABLE, C_MAX_Io_Set, C_MAX_Vo_Set, C_PWR_REF, C_RES};

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_STATUS{};
    CANSignal<uint16_t, 8, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_Iac{};
    CANSignal<uint16_t, 24, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_Io{};
    CANSignal<uint16_t, 40, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_Vo{};
    CANSignal<uint8_t, 56, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_MAINS_F{};
    CANRXMessage<5> CHG_Status_1{can_interface_, 0x305, C_Iac, C_Io, C_MAINS_F, C_STATUS, C_Vo};

    CANSignal<int8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_P_TEMP{};
    CANSignal<int8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_S_TEMP{};
    CANSignal<uint16_t, 16, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> C_Vac{};
    CANSignal<uint16_t, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_MAX_PWR_Set{};
    CANSignal<uint8_t, 48, 8, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> C_AVA_PWR{};
    CANSignal<uint8_t, 56, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Reserved{};
    CANRXMessage<6> CHG_Status_2{can_interface_, 0x306, C_AVA_PWR, C_MAX_PWR_Set, C_P_TEMP, C_S_TEMP, C_Vac, Reserved};

    CANSignal<bool, 0, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> OUT_OC{};
    CANSignal<bool, 1, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> OUT_OV{};
    CANSignal<bool, 2, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> OUT_SC{};
    CANSignal<bool, 3, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> OUT_UV{};
    CANSignal<bool, 4, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> CASE_OT{};
    CANSignal<bool, 5, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> PFC_OT{};
    CANSignal<bool, 6, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> D2D_OT{};
    CANSignal<bool, 7, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> CAN_TO{};
    CANSignal<bool, 8, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> AC_OV{};
    CANSignal<bool, 9, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> AC_UV{};
    CANSignal<bool, 10, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> SCI_TO{};
    CANSignal<bool, 11, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> BUS_ERR{};
    CANRXMessage<12> CHG_Error{can_interface_, 0x307, AC_OV, AC_UV, BUS_ERR, CAN_TO, CASE_OT, D2D_OT, OUT_OC, OUT_OV, OUT_SC, OUT_UV, PFC_OT, SCI_TO};
};