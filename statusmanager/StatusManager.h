#pragma once

#include <string>
#include "StatusSetter.h"

class StatusManager
{
public:
    static StatusManager &getInstance();

    bool isCarrierOverridden();
    std::string getCarrierOverride();
    void setCarrier(std::string text);
    void unsetCarrier();
    bool isSecondaryCarrierOverridden();
    std::string getSecondaryCarrierOverride();
    void setSecondaryCarrier(std::string text);
    void unsetSecondaryCarrier();
    bool isPrimaryServiceBadgeOverridden();
    std::string getPrimaryServiceBadgeOverride();
    void setPrimaryServiceBadge(std::string text);
    void unsetPrimaryServiceBadge();
    bool isSecondaryServiceBadgeOverridden();
    std::string getSecondaryServiceBadgeOverride();
    void setSecondaryServiceBadge(std::string text);
    void unsetSecondaryServiceBadge();
    bool isDateOverridden();
    std::string getDateOverride();
    void setDate(std::string text);
    void unsetDate();
    bool isTimeOverridden();
    std::string getTimeOverride();
    void setTime(std::string text);
    void unsetTime();
    bool isBatteryDetailOverridden();
    std::string getBatteryDetailOverride();
    void setBatteryDetail(std::string text);
    void unsetBatteryDetail();
    bool isCrumbOverridden();
    std::string getCrumbOverride();
    void setCrumb(std::string text);
    void unsetCrumb();
    bool isCellularServiceShown();
    void showCellularService(bool shown);
    bool isSecondaryCellularServiceShown();
    void showSecondaryCellularService(bool shown);
    bool isDataNetworkTypeOverridden();
    int getDataNetworkTypeOverride();
    void setDataNetworkType(int id);
    void unsetDataNetworkType();
    bool isSecondaryDataNetworkTypeOverridden();
    int getSecondaryDataNetworkTypeOverride();
    void setSecondaryDataNetworkType(int id);
    void unsetSecondaryDataNetworkType();
    bool isBatteryCapacityOverridden();
    int getBatteryCapacityOverride();
    void setBatteryCapacity(int id);
    void unsetBatteryCapacity();
    bool isWiFiSignalStrengthBarsOverridden();
    int getWiFiSignalStrengthBarsOverride();
    void setWiFiSignalStrengthBars(int id);
    void unsetWiFiSignalStrengthBars();
    bool isGSMSignalStrengthBarsOverridden();
    int getGSMSignalStrengthBarsOverride();
    void setGSMSignalStrengthBars(int id);
    void unsetGSMSignalStrengthBars();
    bool isSecondaryGSMSignalStrengthBarsOverridden();
    int getSecondaryGSMSignalStrengthBarsOverride();
    void setSecondaryGSMSignalStrengthBars(int id);
    void unsetSecondaryGSMSignalStrengthBars();
    bool isRawWiFiSignalShown();
    void showRawWiFiSignal(bool shown);
    bool isRawGSMSignalShown();
    void showRawGSMSignal(bool shown);
    bool isDNDHidden();
    void hideDND(bool hidden);
    bool isAirplaneHidden();
    void hideAirplane(bool hidden);
    bool isCellHidden();
    void hideCell(bool hidden);
    bool isWiFiHidden();
    void hideWiFi(bool hidden);
    bool isBatteryHidden();
    void hideBattery(bool hidden);
    bool isBluetoothHidden();
    void hideBluetooth(bool hidden);
    bool isAlarmHidden();
    void hideAlarm(bool hidden);
    bool isLocationHidden();
    void hideLocation(bool hidden);
    bool isRotationHidden();
    void hideRotation(bool hidden);
    bool isAirPlayHidden();
    void hideAirPlay(bool hidden);
    bool isCarPlayHidden();
    void hideCarPlay(bool hidden);
    bool isVPNHidden();
    void hideVPN(bool hidden);

private:
    StatusManager();
    StatusSetter &getSetter();

    StatusSetter *setter = nullptr;
};
