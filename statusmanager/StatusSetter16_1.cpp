#include "StatusSetter16_1.h"
#include "../devicemanager.h"
#include <iostream>

enum class StatusBarItem : int
{
    TimeStatusBarItem = 0,
    DateStatusBarItem = 1,
    QuietModeStatusBarItem = 2,
    AirplaneModeStatusBarItem = 3,
    CellularSignalStrengthStatusBarItem = 4,
    SecondaryCellularSignalStrengthStatusBarItem = 5,
    CellularServiceStatusBarItem = 6,
    SecondaryCellularServiceStatusBarItem = 7,
    // 8
    CellularDataNetworkStatusBarItem = 9,
    SecondaryCellularDataNetworkStatusBarItem = 10,
    // 11
    MainBatteryStatusBarItem = 12,
    ProminentlyShowBatteryDetailStatusBarItem = 13,
    // 14
    // 15
    BluetoothStatusBarItem = 16,
    TTYStatusBarItem = 17,
    AlarmStatusBarItem = 18,
    // 19
    // 20
    LocationStatusBarItem = 21,
    RotationLockStatusBarItem = 22,
    CameraUseStatusBarItem = 23,
    AirPlayStatusBarItem = 24,
    AssistantStatusBarItem = 25,
    CarPlayStatusBarItem = 26,
    StudentStatusBarItem = 27,
    MicrophoneUseStatusBarItem = 28,
    VPNStatusBarItem = 29,
    // 30
    // 31
    // 32
    // 33
    // 34
    // 35
    // 36
    // 37
    LiquidDetectionStatusBarItem = 38,
    VoiceControlStatusBarItem = 39,
    // 40
    // 41
    // 42
    // 43
    Extra1StatusBarItem = 44
};

enum class BatteryState : unsigned int
{
    BatteryStateUnplugged = 0
};

struct StatusBarRawData
{
    bool itemIsEnabled[45];
    char padding;
    char timeString[64];
    char shortTimeString[64];
    char dateString[256];
    int GSMSignalStrengthRaw;
    int secondaryGSMSignalStrengthRaw;
    int GSMSignalStrengthBars;
    int secondaryGSMSignalStrengthBars;
    char serviceString[100];
    char secondaryServiceString[100];
    char serviceCrossfadeString[100];
    char secondaryServiceCrossfadeString[100];
    char serviceImages[2][100];
    char operatorDirectory[1024];
    unsigned int serviceContentType;
    unsigned int secondaryServiceContentType;
    unsigned int cellLowDataModeActive : 1;
    unsigned int secondaryCellLowDataModeActive : 1;
    int WiFiSignalStrengthRaw;
    int WiFiSignalStrengthBars;
    unsigned int WiFiLowDataModeActive : 1;
    unsigned int dataNetworkType;
    unsigned int secondaryDataNetworkType;
    int batteryCapacity;
    unsigned int batteryState;
    char batteryDetailString[150];
    int bluetoothBatteryCapacity;
    int thermalColor;
    unsigned int thermalSunlightMode : 1;
    unsigned int slowActivity : 1;
    unsigned int syncActivity : 1;
    char activityDisplayId[256];
    unsigned int bluetoothConnected : 1;
    unsigned int displayRawGSMSignal : 1;
    unsigned int displayRawWiFiSignal : 1;
    unsigned int locationIconType : 1;
    unsigned int voiceControlIconType : 2;
    unsigned int quietModeInactive : 1;
    unsigned int tetheringConnectionCount;
    unsigned int batterySaverModeActive : 1;
    unsigned int deviceIsRTL : 1;
    unsigned int lock : 1;
    char breadcrumbTitle[256];
    char breadcrumbSecondaryTitle[256];
    char personName[100];
    unsigned int electronicTollCollectionAvailable : 1;
    unsigned int radarAvailable : 1;
    unsigned int WiFiLinkWarning : 1;
    unsigned int WiFiSearching : 1;
    double backgroundActivityDisplayStartDate;
    unsigned int shouldShowEmergencyOnlyStatus : 1;
    unsigned int secondaryCellularConfigured : 1;
    char primaryServiceBadgeString[100];
    char secondaryServiceBadgeString[100];
    char quietModeImage[256];
    unsigned int extra1 : 1;
};

struct StatusBarOverrideData
{
    bool overrideItemIsEnabled[45];
    char padding;
    unsigned int overrideTimeString : 1;
    unsigned int overrideDateString : 1;
    unsigned int overrideGSMSignalStrengthRaw : 1;
    unsigned int overrideSecondaryGSMSignalStrengthRaw : 1;
    unsigned int overrideGSMSignalStrengthBars : 1;
    unsigned int overrideSecondaryGSMSignalStrengthBars : 1;
    unsigned int overrideServiceString : 1;
    unsigned int overrideSecondaryServiceString : 1;
    unsigned int overrideServiceImages : 2;
    unsigned int overrideOperatorDirectory : 1;
    unsigned int overrideServiceContentType : 1;
    unsigned int overrideSecondaryServiceContentType : 1;
    unsigned int overrideWiFiSignalStrengthRaw : 1;
    unsigned int overrideWiFiSignalStrengthBars : 1;
    unsigned int overrideDataNetworkType : 1;
    unsigned int overrideSecondaryDataNetworkType : 1;
    unsigned int disallowsCellularDataNetworkTypes : 1;
    unsigned int overrideBatteryCapacity : 1;
    unsigned int overrideBatteryState : 1;
    unsigned int overrideBatteryDetailString : 1;
    unsigned int overrideBluetoothBatteryCapacity : 1;
    unsigned int overrideThermalColor : 1;
    unsigned int overrideSlowActivity : 1;
    unsigned int overrideActivityDisplayId : 1;
    unsigned int overrideBluetoothConnected : 1;
    unsigned int overrideBreadcrumb : 1;
    unsigned int overrideLock;
    unsigned int overrideDisplayRawGSMSignal : 1;
    unsigned int overrideDisplayRawWiFiSignal : 1;
    unsigned int overridePersonName : 1;
    unsigned int overrideWiFiLinkWarning : 1;
    unsigned int overrideSecondaryCellularConfigured : 1;
    unsigned int overridePrimaryServiceBadgeString : 1;
    unsigned int overrideSecondaryServiceBadgeString : 1;
    unsigned int overrideQuietModeImage : 1;
    unsigned int overrideExtra1 : 1;
    StatusBarRawData values;
};

// Getting setting

namespace
{
    void applyChanges(StatusBarOverrideData *overrides)
    {
        auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
        if (!workspace)
            return;
        auto location = *workspace + "/StatusBar/HomeDomain/Library/SpringBoard/statusBarOverrides";

        std::ofstream outfile(location, std::ofstream::binary);
        if (!outfile)
            return;

        char padding[256] = {'\0'};

        outfile.write(reinterpret_cast<char *>(overrides), sizeof(StatusBarOverrideData));
        outfile.write(padding, sizeof(padding));
    }

    StatusBarOverrideData *getOverrides()
    {
        auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
        if (!workspace)
            return nullptr;
        auto location = *workspace + "/StatusBar/HomeDomain/Library/SpringBoard/statusBarOverrides";

        std::ifstream infile(location, std::ifstream::binary);
        if (!infile)
        {
            StatusBarOverrideData *overrides = new StatusBarOverrideData();
            return overrides;
        }

        // Get the file size
        infile.seekg(0, std::ifstream::end);
        std::streampos fileSize = infile.tellg();
        infile.seekg(0, std::ifstream::beg);

        // Check if the file size matches the structure size + padding
        if (fileSize != sizeof(StatusBarOverrideData) + 256)
        {
            StatusBarOverrideData *overrides = new StatusBarOverrideData();
            return overrides;
        }

        StatusBarOverrideData *overrides = new StatusBarOverrideData();
        infile.read(reinterpret_cast<char *>(overrides), sizeof(StatusBarOverrideData));
        return overrides;
    }
}

// Override funcs

bool StatusSetter16_1::isCarrierOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideServiceString == 1;
}

std::string StatusSetter16_1::getCarrierOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.serviceString);
    return text;
}

void StatusSetter16_1::setCarrier(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideServiceString = 1;
    strncpy(overrides->values.serviceString, text.c_str(), 100);
    strncpy(overrides->values.serviceCrossfadeString, text.c_str(), 100);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetCarrier()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideServiceString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isSecondaryCarrierOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryServiceString == 1;
}

std::string StatusSetter16_1::getSecondaryCarrierOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.secondaryServiceString);
    return text;
}

void StatusSetter16_1::setSecondaryCarrier(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceString = 1;
    strncpy(overrides->values.secondaryServiceString, text.c_str(), 100);
    strncpy(overrides->values.secondaryServiceCrossfadeString, text.c_str(), 100);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetSecondaryCarrier()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isPrimaryServiceBadgeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overridePrimaryServiceBadgeString == 1;
}

std::string StatusSetter16_1::getPrimaryServiceBadgeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.primaryServiceBadgeString);
    return text;
}

void StatusSetter16_1::setPrimaryServiceBadge(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overridePrimaryServiceBadgeString = 1;
    strncpy(overrides->values.primaryServiceBadgeString, text.c_str(), 100);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetPrimaryServiceBadge()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overridePrimaryServiceBadgeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isSecondaryServiceBadgeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryServiceBadgeString == 1;
}

std::string StatusSetter16_1::getSecondaryServiceBadgeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.secondaryServiceBadgeString);
    return text;
}

void StatusSetter16_1::setSecondaryServiceBadge(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceBadgeString = 1;
    strncpy(overrides->values.secondaryServiceBadgeString, text.c_str(), 100);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetSecondaryServiceBadge()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceBadgeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isDateOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDateString == 1;
}

std::string StatusSetter16_1::getDateOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.dateString);
    return text;
}

void StatusSetter16_1::setDate(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDateString = 1;
    strncpy(overrides->values.dateString, text.c_str(), 256);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetDate()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDateString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isTimeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideTimeString == 1;
}

std::string StatusSetter16_1::getTimeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.timeString);
    return text;
}

void StatusSetter16_1::setTime(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideTimeString = 1;
    strncpy(overrides->values.timeString, text.c_str(), 64);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetTime()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideTimeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isBatteryDetailOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBatteryDetailString == 1;
}

std::string StatusSetter16_1::getBatteryDetailOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.batteryDetailString);
    return text;
}

void StatusSetter16_1::setBatteryDetail(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryDetailString = 1;
    strncpy(overrides->values.batteryDetailString, text.c_str(), 150);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetBatteryDetail()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryDetailString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isCrumbOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBreadcrumb == 1;
}

std::string StatusSetter16_1::getCrumbOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.breadcrumbTitle);
    if (text.size() > 1)
    {
        return text.substr(0, text.size() - 4);
    }
    return "";
}

void StatusSetter16_1::setCrumb(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBreadcrumb = 1;
    strncpy(overrides->values.breadcrumbTitle, text.append(" ▶").c_str(), 256);
    applyChanges(overrides);
}

void StatusSetter16_1::unsetCrumb()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBreadcrumb = 0;
    strncpy(overrides->values.breadcrumbTitle, std::string("").c_str(), 256);
    applyChanges(overrides);
}

bool StatusSetter16_1::isCellularServiceOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

bool StatusSetter16_1::getCellularServiceOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

void StatusSetter16_1::setCellularService(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 1;
    if (shown)
    {
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 1;
    }
    else
    {
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

void StatusSetter16_1::unsetCellularService()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isSecondaryCellularServiceOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] == 1;
}

bool StatusSetter16_1::getSecondaryCellularServiceOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] == 1;
}

void StatusSetter16_1::setSecondaryCellularService(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 1;
    overrides->overrideSecondaryCellularConfigured = 1;
    if (shown)
    {
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 1;
        overrides->values.secondaryCellularConfigured = 1;
    }
    else
    {
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 0;
        overrides->values.secondaryCellularConfigured = 1;
    }

    applyChanges(overrides);
}

void StatusSetter16_1::unsetSecondaryCellularService()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 0;
    overrides->overrideSecondaryCellularConfigured = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isDataNetworkTypeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDataNetworkType == 1;
}

int StatusSetter16_1::getDataNetworkTypeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.dataNetworkType;
    return id;
}

void StatusSetter16_1::setDataNetworkType(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDataNetworkType = 1;
    overrides->values.dataNetworkType = id;
    applyChanges(overrides);
}

void StatusSetter16_1::unsetDataNetworkType()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDataNetworkType = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isSecondaryDataNetworkTypeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryDataNetworkType == 1;
}

int StatusSetter16_1::getSecondaryDataNetworkTypeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.secondaryDataNetworkType;
    return id;
}

void StatusSetter16_1::setSecondaryDataNetworkType(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
    overrides->overrideDataNetworkType = 1;
    overrides->values.dataNetworkType = id;
    applyChanges(overrides);
}

void StatusSetter16_1::unsetSecondaryDataNetworkType()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 0;
    overrides->overrideSecondaryDataNetworkType = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isBatteryCapacityOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBatteryCapacity == 1;
}

int StatusSetter16_1::getBatteryCapacityOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.batteryCapacity;
    return id;
}

void StatusSetter16_1::setBatteryCapacity(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryCapacity = 1;
    overrides->values.batteryCapacity = id;
    applyChanges(overrides);
}

void StatusSetter16_1::unsetBatteryCapacity()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryCapacity = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isWiFiSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideWiFiSignalStrengthBars == 1;
}

int StatusSetter16_1::getWiFiSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.WiFiSignalStrengthBars;
    return id;
}

void StatusSetter16_1::setWiFiSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideWiFiSignalStrengthBars = 1;
    overrides->values.WiFiSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16_1::unsetWiFiSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideWiFiSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isGSMSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideGSMSignalStrengthBars == 1;
}

int StatusSetter16_1::getGSMSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.GSMSignalStrengthBars;
    return id;
}

void StatusSetter16_1::setGSMSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 1;
    overrides->overrideGSMSignalStrengthBars = 1;
    overrides->values.GSMSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16_1::unsetGSMSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 0;
    overrides->overrideGSMSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isSecondaryGSMSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryGSMSignalStrengthBars == 1;
}

int StatusSetter16_1::getSecondaryGSMSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.secondaryGSMSignalStrengthBars;
    return id;
}

void StatusSetter16_1::setSecondaryGSMSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 1;
    overrides->overrideSecondaryGSMSignalStrengthBars = 1;
    overrides->values.secondaryGSMSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16_1::unsetSecondaryGSMSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 0;
    overrides->overrideSecondaryGSMSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16_1::isRawWiFiSignalShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDisplayRawWiFiSignal == 1;
}

void StatusSetter16_1::showRawWiFiSignal(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (shown)
    {
        overrides->overrideDisplayRawWiFiSignal = 1;
        overrides->values.displayRawWiFiSignal = 1;
    }
    else
    {
        overrides->overrideDisplayRawWiFiSignal = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isRawGSMSignalShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDisplayRawGSMSignal == 1;
}

void StatusSetter16_1::showRawGSMSignal(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (shown)
    {
        overrides->overrideDisplayRawGSMSignal = 1;
        overrides->values.displayRawGSMSignal = 1;
    }
    else
    {
        overrides->overrideDisplayRawGSMSignal = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isDNDHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::QuietModeStatusBarItem)] == 1;
}

void StatusSetter16_1::hideDND(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::QuietModeStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::QuietModeStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::QuietModeStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isAirplaneHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirplaneModeStatusBarItem)] == 1;
}

void StatusSetter16_1::hideAirplane(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirplaneModeStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::AirplaneModeStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirplaneModeStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isCellHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

void StatusSetter16_1::hideCell(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 0;
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 0;
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isWiFiHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularDataNetworkStatusBarItem)] == 1;
}

void StatusSetter16_1::hideWiFi(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularDataNetworkStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularDataNetworkStatusBarItem)] = 0;
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularDataNetworkStatusBarItem)] = 0;
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isBatteryHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::MainBatteryStatusBarItem)] == 1;
}

void StatusSetter16_1::hideBattery(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::MainBatteryStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::MainBatteryStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::MainBatteryStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isBluetoothHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::BluetoothStatusBarItem)] == 1;
}

void StatusSetter16_1::hideBluetooth(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::BluetoothStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::BluetoothStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::BluetoothStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isAlarmHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AlarmStatusBarItem)] == 1;
}

void StatusSetter16_1::hideAlarm(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AlarmStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::AlarmStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AlarmStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isLocationHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::LocationStatusBarItem)] == 1;
}

void StatusSetter16_1::hideLocation(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::LocationStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::LocationStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::LocationStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isRotationHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::RotationLockStatusBarItem)] == 1;
}

void StatusSetter16_1::hideRotation(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::RotationLockStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::RotationLockStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::RotationLockStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isAirPlayHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirPlayStatusBarItem)] == 1;
}

void StatusSetter16_1::hideAirPlay(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirPlayStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::AirPlayStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirPlayStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isCarPlayHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CarPlayStatusBarItem)] == 1;
}

void StatusSetter16_1::hideCarPlay(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CarPlayStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CarPlayStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CarPlayStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_1::isVPNHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::VPNStatusBarItem)] == 1;
}

void StatusSetter16_1::hideVPN(bool hidden)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (hidden)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::VPNStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::VPNStatusBarItem)] = 0;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::VPNStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}
