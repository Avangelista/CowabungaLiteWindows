#pragma once

#include <fstream>
#include <cstring>

class StatusSetter {
public:
    virtual bool isCarrierOverridden() = 0;
    virtual std::string getCarrierOverride() = 0;
    virtual void setCarrier(std::string text) = 0;
    virtual void unsetCarrier() = 0;
    virtual bool isSecondaryCarrierOverridden() = 0;
    virtual std::string getSecondaryCarrierOverride() = 0;
    virtual void setSecondaryCarrier(std::string text) = 0;
    virtual void unsetSecondaryCarrier() = 0;
    virtual bool isPrimaryServiceBadgeOverridden() = 0;
    virtual std::string getPrimaryServiceBadgeOverride() = 0;
    virtual void setPrimaryServiceBadge(std::string text) = 0;
    virtual void unsetPrimaryServiceBadge() = 0;
    virtual bool isSecondaryServiceBadgeOverridden() = 0;
    virtual std::string getSecondaryServiceBadgeOverride() = 0;
    virtual void setSecondaryServiceBadge(std::string text) = 0;
    virtual void unsetSecondaryServiceBadge() = 0;
    virtual bool isDateOverridden() = 0;
    virtual std::string getDateOverride() = 0;
    virtual void setDate(std::string text) = 0;
    virtual void unsetDate() = 0;
    virtual bool isTimeOverridden() = 0;
    virtual std::string getTimeOverride() = 0;
    virtual void setTime(std::string text) = 0;
    virtual void unsetTime() = 0;
    virtual bool isBatteryDetailOverridden() = 0;
    virtual std::string getBatteryDetailOverride() = 0;
    virtual void setBatteryDetail(std::string text) = 0;
    virtual void unsetBatteryDetail() = 0;
    virtual bool isCrumbOverridden() = 0;
    virtual std::string getCrumbOverride() = 0;
    virtual void setCrumb(std::string text) = 0;
    virtual void unsetCrumb() = 0;
    virtual bool isCellularServiceOverridden() = 0;
    virtual bool getCellularServiceOverride() = 0;
    virtual void setCellularService(bool shown) = 0;
    virtual void unsetCellularService() = 0;
    virtual bool isSecondaryCellularServiceOverridden() = 0;
    virtual bool getSecondaryCellularServiceOverride() = 0;
    virtual void setSecondaryCellularService(bool shown) = 0;
    virtual void unsetSecondaryCellularService() = 0;
    virtual bool isDataNetworkTypeOverridden() = 0;
    virtual int getDataNetworkTypeOverride() = 0;
    virtual void setDataNetworkType(int id) = 0;
    virtual void unsetDataNetworkType() = 0;
    virtual bool isSecondaryDataNetworkTypeOverridden() = 0;
    virtual int getSecondaryDataNetworkTypeOverride() = 0;
    virtual void setSecondaryDataNetworkType(int id) = 0;
    virtual void unsetSecondaryDataNetworkType() = 0;
    virtual bool isBatteryCapacityOverridden() = 0;
    virtual int getBatteryCapacityOverride() = 0;
    virtual void setBatteryCapacity(int id) = 0;
    virtual void unsetBatteryCapacity() = 0;
    virtual bool isWiFiSignalStrengthBarsOverridden() = 0;
    virtual int getWiFiSignalStrengthBarsOverride() = 0;
    virtual void setWiFiSignalStrengthBars(int id) = 0;
    virtual void unsetWiFiSignalStrengthBars() = 0;
    virtual bool isGSMSignalStrengthBarsOverridden() = 0;
    virtual int getGSMSignalStrengthBarsOverride() = 0;
    virtual void setGSMSignalStrengthBars(int id) = 0;
    virtual void unsetGSMSignalStrengthBars() = 0;
    virtual bool isSecondaryGSMSignalStrengthBarsOverridden() = 0;
    virtual int getSecondaryGSMSignalStrengthBarsOverride() = 0;
    virtual void setSecondaryGSMSignalStrengthBars(int id) = 0;
    virtual void unsetSecondaryGSMSignalStrengthBars() = 0;
    virtual bool isRawWiFiSignalShown() = 0;
    virtual void showRawWiFiSignal(bool shown) = 0;
    virtual bool isRawGSMSignalShown() = 0;
    virtual void showRawGSMSignal(bool shown) = 0;
    virtual bool isDNDHidden() = 0;
    virtual void hideDND(bool hidden) = 0;
    virtual bool isAirplaneHidden() = 0;
    virtual void hideAirplane(bool hidden) = 0;
    virtual bool isCellHidden() = 0;
    virtual void hideCell(bool hidden) = 0;
    virtual bool isWiFiHidden() = 0;
    virtual void hideWiFi(bool hidden) = 0;
    virtual bool isBatteryHidden() = 0;
    virtual void hideBattery(bool hidden) = 0;
    virtual bool isBluetoothHidden() = 0;
    virtual void hideBluetooth(bool hidden) = 0;
    virtual bool isAlarmHidden() = 0;
    virtual void hideAlarm(bool hidden) = 0;
    virtual bool isLocationHidden() = 0;
    virtual void hideLocation(bool hidden) = 0;
    virtual bool isRotationHidden() = 0;
    virtual void hideRotation(bool hidden) = 0;
    virtual bool isAirPlayHidden() = 0;
    virtual void hideAirPlay(bool hidden) = 0;
    virtual bool isCarPlayHidden() = 0;
    virtual void hideCarPlay(bool hidden) = 0;
    virtual bool isVPNHidden() = 0;
    virtual void hideVPN(bool hidden) = 0;

    virtual ~StatusSetter() {}
};
