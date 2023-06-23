#include "StatusManager.h"
#include "StatusSetter.h"
#include "StatusSetter16_3.h"

StatusManager::StatusManager()
{
}

StatusManager &StatusManager::getInstance()
{
    static StatusManager instance;
    return instance;
}

StatusSetter &StatusManager::getSetter()
{
    if (!setter)
    {
        // may have to check each time?
        setter = new StatusSetter16_3();
    }
    return *setter;
}

bool StatusManager::isCarrierOverridden()
{
    return getSetter().isCarrierOverridden();
}

std::string StatusManager::getCarrierOverride()
{
    return getSetter().getCarrierOverride();
}

void StatusManager::setCarrier(std::string text)
{
    getSetter().setCarrier(text);
}

void StatusManager::unsetCarrier()
{
    getSetter().unsetCarrier();
}

bool StatusManager::isSecondaryCarrierOverridden()
{
    return getSetter().isSecondaryCarrierOverridden();
}

std::string StatusManager::getSecondaryCarrierOverride()
{
    return getSetter().getSecondaryCarrierOverride();
}

void StatusManager::setSecondaryCarrier(std::string text)
{
    getSetter().setSecondaryCarrier(text);
}

void StatusManager::unsetSecondaryCarrier()
{
    getSetter().unsetSecondaryCarrier();
}

bool StatusManager::isPrimaryServiceBadgeOverridden()
{
    return getSetter().isPrimaryServiceBadgeOverridden();
}

std::string StatusManager::getPrimaryServiceBadgeOverride()
{
    return getSetter().getPrimaryServiceBadgeOverride();
}

void StatusManager::setPrimaryServiceBadge(std::string text)
{
    getSetter().setPrimaryServiceBadge(text);
}

void StatusManager::unsetPrimaryServiceBadge()
{
    getSetter().unsetPrimaryServiceBadge();
}

bool StatusManager::isSecondaryServiceBadgeOverridden()
{
    return getSetter().isSecondaryServiceBadgeOverridden();
}

std::string StatusManager::getSecondaryServiceBadgeOverride()
{
    return getSetter().getSecondaryServiceBadgeOverride();
}

void StatusManager::setSecondaryServiceBadge(std::string text)
{
    getSetter().setSecondaryServiceBadge(text);
}

void StatusManager::unsetSecondaryServiceBadge()
{
    getSetter().unsetSecondaryServiceBadge();
}

bool StatusManager::isDateOverridden()
{
    return getSetter().isDateOverridden();
}

std::string StatusManager::getDateOverride()
{
    return getSetter().getDateOverride();
}

void StatusManager::setDate(std::string text)
{
    getSetter().setDate(text);
}

void StatusManager::unsetDate()
{
    getSetter().unsetDate();
}

bool StatusManager::isTimeOverridden()
{
    return getSetter().isTimeOverridden();
}

std::string StatusManager::getTimeOverride()
{
    return getSetter().getTimeOverride();
}

void StatusManager::setTime(std::string text)
{
    getSetter().setTime(text);
}

void StatusManager::unsetTime()
{
    getSetter().unsetTime();
}

bool StatusManager::isBatteryDetailOverridden()
{
    return getSetter().isBatteryDetailOverridden();
}

std::string StatusManager::getBatteryDetailOverride()
{
    return getSetter().getBatteryDetailOverride();
}

void StatusManager::setBatteryDetail(std::string text)
{
    getSetter().setBatteryDetail(text);
}

void StatusManager::unsetBatteryDetail()
{
    getSetter().unsetBatteryDetail();
}

bool StatusManager::isCrumbOverridden()
{
    return getSetter().isCrumbOverridden();
}

std::string StatusManager::getCrumbOverride()
{
    return getSetter().getCrumbOverride();
}

void StatusManager::setCrumb(std::string text)
{
    getSetter().setCrumb(text);
}

void StatusManager::unsetCrumb()
{
    getSetter().unsetCrumb();
}

bool StatusManager::isCellularServiceShown()
{
    return getSetter().isCellularServiceShown();
}

void StatusManager::showCellularService(bool shown)
{
    getSetter().showCellularService(shown);
}

bool StatusManager::isSecondaryCellularServiceShown()
{
    return getSetter().isSecondaryCellularServiceShown();
}

void StatusManager::showSecondaryCellularService(bool shown)
{
    getSetter().showSecondaryCellularService(shown);
}

bool StatusManager::isDataNetworkTypeOverridden()
{
    return getSetter().isDataNetworkTypeOverridden();
}

int StatusManager::getDataNetworkTypeOverride()
{
    return getSetter().getDataNetworkTypeOverride();
}

void StatusManager::setDataNetworkType(int id)
{
    getSetter().setDataNetworkType(id);
}

void StatusManager::unsetDataNetworkType()
{
    getSetter().unsetDataNetworkType();
}

bool StatusManager::isSecondaryDataNetworkTypeOverridden()
{
    return getSetter().isSecondaryDataNetworkTypeOverridden();
}

int StatusManager::getSecondaryDataNetworkTypeOverride()
{
    return getSetter().getSecondaryDataNetworkTypeOverride();
}

void StatusManager::setSecondaryDataNetworkType(int id)
{
    getSetter().setSecondaryDataNetworkType(id);
}

void StatusManager::unsetSecondaryDataNetworkType()
{
    getSetter().unsetSecondaryDataNetworkType();
}

bool StatusManager::isBatteryCapacityOverridden()
{
    return getSetter().isBatteryCapacityOverridden();
}

int StatusManager::getBatteryCapacityOverride()
{
    return getSetter().getBatteryCapacityOverride();
}

void StatusManager::setBatteryCapacity(int id)
{
    getSetter().setBatteryCapacity(id);
}

void StatusManager::unsetBatteryCapacity()
{
    getSetter().unsetBatteryCapacity();
}

bool StatusManager::isWiFiSignalStrengthBarsOverridden()
{
    return getSetter().isWiFiSignalStrengthBarsOverridden();
}

int StatusManager::getWiFiSignalStrengthBarsOverride()
{
    return getSetter().getWiFiSignalStrengthBarsOverride();
}

void StatusManager::setWiFiSignalStrengthBars(int id)
{
    getSetter().setWiFiSignalStrengthBars(id);
}

void StatusManager::unsetWiFiSignalStrengthBars()
{
    getSetter().unsetWiFiSignalStrengthBars();
}

bool StatusManager::isGsmSignalStrengthBarsOverridden()
{
    return getSetter().isGsmSignalStrengthBarsOverridden();
}

int StatusManager::getGsmSignalStrengthBarsOverride()
{
    return getSetter().getGsmSignalStrengthBarsOverride();
}

void StatusManager::setGsmSignalStrengthBars(int id)
{
    getSetter().setGsmSignalStrengthBars(id);
}

void StatusManager::unsetGsmSignalStrengthBars()
{
    getSetter().unsetGsmSignalStrengthBars();
}

bool StatusManager::isSecondaryGsmSignalStrengthBarsOverridden()
{
    return getSetter().isSecondaryGsmSignalStrengthBarsOverridden();
}

int StatusManager::getSecondaryGsmSignalStrengthBarsOverride()
{
    return getSetter().getSecondaryGsmSignalStrengthBarsOverride();
}

void StatusManager::setSecondaryGsmSignalStrengthBars(int id)
{
    getSetter().setSecondaryGsmSignalStrengthBars(id);
}

void StatusManager::unsetSecondaryGsmSignalStrengthBars()
{
    getSetter().unsetSecondaryGsmSignalStrengthBars();
}

bool StatusManager::isRawWiFiSignalShown()
{
    return getSetter().isRawWiFiSignalShown();
}

void StatusManager::showRawWiFiSignal(bool shown)
{
    getSetter().showRawWiFiSignal(shown);
}

bool StatusManager::isRawGsmSignalShown()
{
    return getSetter().isRawGsmSignalShown();
}

void StatusManager::showRawGsmSignal(bool shown)
{
    getSetter().showRawGsmSignal(shown);
}

bool StatusManager::isDNDHidden()
{
    return getSetter().isDNDHidden();
}

void StatusManager::hideDND(bool hidden)
{
    getSetter().hideDND(hidden);
}

bool StatusManager::isAirplaneHidden()
{
    return getSetter().isAirplaneHidden();
}

void StatusManager::hideAirplane(bool hidden)
{
    getSetter().hideAirplane(hidden);
}

bool StatusManager::isCellHidden()
{
    return getSetter().isCellHidden();
}

void StatusManager::hideCell(bool hidden)
{
    getSetter().hideCell(hidden);
}

bool StatusManager::isWiFiHidden()
{
    return getSetter().isWiFiHidden();
}

void StatusManager::hideWiFi(bool hidden)
{
    getSetter().hideWiFi(hidden);
}

bool StatusManager::isBatteryHidden()
{
    return getSetter().isBatteryHidden();
}

void StatusManager::hideBattery(bool hidden)
{
    getSetter().hideBattery(hidden);
}

bool StatusManager::isBluetoothHidden()
{
    return getSetter().isBluetoothHidden();
}

void StatusManager::hideBluetooth(bool hidden)
{
    getSetter().hideBluetooth(hidden);
}

bool StatusManager::isAlarmHidden()
{
    return getSetter().isAlarmHidden();
}

void StatusManager::hideAlarm(bool hidden)
{
    getSetter().hideAlarm(hidden);
}

bool StatusManager::isLocationHidden()
{
    return getSetter().isLocationHidden();
}

void StatusManager::hideLocation(bool hidden)
{
    getSetter().hideLocation(hidden);
}

bool StatusManager::isRotationHidden()
{
    return getSetter().isRotationHidden();
}

void StatusManager::hideRotation(bool hidden)
{
    getSetter().hideRotation(hidden);
}

bool StatusManager::isAirPlayHidden()
{
    return getSetter().isAirPlayHidden();
}

void StatusManager::hideAirPlay(bool hidden)
{
    getSetter().hideAirPlay(hidden);
}

bool StatusManager::isCarPlayHidden()
{
    return getSetter().isCarPlayHidden();
}

void StatusManager::hideCarPlay(bool hidden)
{
    getSetter().hideCarPlay(hidden);
}

bool StatusManager::isVPNHidden()
{
    return getSetter().isVPNHidden();
}

void StatusManager::hideVPN(bool hidden)
{
    getSetter().hideVPN(hidden);
}
