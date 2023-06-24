#include "StatusManager.h"
#include "StatusSetter15.h"
#include "StatusSetter16.h"
#include "StatusSetter16_1.h"
#include "StatusSetter16_3.h"
#include "../utils.h"
#include "../DeviceManager.h"
#include <QDebug>

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
    auto v = DeviceManager::getInstance().getCurrentVersion();
    if (*v >= Version(16, 3)) {
        setter = new StatusSetter16_3();
    } else if (*v >= Version(16, 1)) {
        qDebug() << "16.1";
        setter = new StatusSetter16_1();
    } else if (*v >= Version(16)) {
        setter = new StatusSetter16();
    } else if (*v >= Version(15)) {
        setter = new StatusSetter15();
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

bool StatusManager::isCellularServiceOverridden()
{
    return getSetter().isCellularServiceOverridden();
}

bool StatusManager::getCellularServiceOverride()
{
    return getSetter().getCellularServiceOverride();
}

void StatusManager::setCellularService(bool shown)
{
    getSetter().setCellularService(shown);
}

void StatusManager::unsetCellularService()
{
    getSetter().unsetCellularService();
}

bool StatusManager::isSecondaryCellularServiceOverridden()
{
    return getSetter().isSecondaryCellularServiceOverridden();
}

bool StatusManager::getSecondaryCellularServiceOverride()
{
    return getSetter().getSecondaryCellularServiceOverride();
}

void StatusManager::setSecondaryCellularService(bool shown)
{
    getSetter().setSecondaryCellularService(shown);
}

void StatusManager::unsetSecondaryCellularService()
{
    getSetter().unsetSecondaryCellularService();
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

bool StatusManager::isGSMSignalStrengthBarsOverridden()
{
    return getSetter().isGSMSignalStrengthBarsOverridden();
}

int StatusManager::getGSMSignalStrengthBarsOverride()
{
    return getSetter().getGSMSignalStrengthBarsOverride();
}

void StatusManager::setGSMSignalStrengthBars(int id)
{
    getSetter().setGSMSignalStrengthBars(id);
}

void StatusManager::unsetGSMSignalStrengthBars()
{
    getSetter().unsetGSMSignalStrengthBars();
}

bool StatusManager::isSecondaryGSMSignalStrengthBarsOverridden()
{
    return getSetter().isSecondaryGSMSignalStrengthBarsOverridden();
}

int StatusManager::getSecondaryGSMSignalStrengthBarsOverride()
{
    return getSetter().getSecondaryGSMSignalStrengthBarsOverride();
}

void StatusManager::setSecondaryGSMSignalStrengthBars(int id)
{
    getSetter().setSecondaryGSMSignalStrengthBars(id);
}

void StatusManager::unsetSecondaryGSMSignalStrengthBars()
{
    getSetter().unsetSecondaryGSMSignalStrengthBars();
}

bool StatusManager::isRawWiFiSignalShown()
{
    return getSetter().isRawWiFiSignalShown();
}

void StatusManager::showRawWiFiSignal(bool shown)
{
    getSetter().showRawWiFiSignal(shown);
}

bool StatusManager::isRawGSMSignalShown()
{
    return getSetter().isRawGSMSignalShown();
}

void StatusManager::showRawGSMSignal(bool shown)
{
    getSetter().showRawGSMSignal(shown);
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
