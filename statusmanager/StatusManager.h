#include <string>
#include "StatusSetter.h"
#include "StatusSetter16_3.h"

class StatusManager
{
public:
    static StatusManager &getInstance();

    void setFileLocation(const std::string &location);
    std::string getFileLocation() const;

    bool isCarrierOverridden();
    std::string getCarrierOverride();
    void setCarrier(std::string text);
    void unsetCarrier();
    bool isBatteryHidden();
    void hideBattery(bool hidden);

private:
    StatusManager();
    StatusSetter &getSetter();

    StatusSetter *setter = nullptr;
    std::string fileLocation;
};