#pragma once

#include "StatusSetter.h"

class StatusSetter16_3 : public StatusSetter
{
public:
    bool isCarrierOverridden() override;
    std::string getCarrierOverride() override;
    void setCarrier(std::string text) override;
    void unsetCarrier() override;
    bool isBatteryHidden() override;
    void hideBattery(bool hidden) override;
};
