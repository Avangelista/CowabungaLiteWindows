#include "StatusSetter16_3.h"
#include "../devicemanager.h"

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
    Extra1StatusBarItem = 44,
};

enum class BatteryState : unsigned int
{
    BatteryStateUnplugged = 0
};

struct StatusBarRawData
{
    bool itemIsEnabled[45];
    char padding1;
    char padding2;
    char timeString[64];
    char shortTimeString[64];
    char dateString[256];
    int gsmSignalStrengthRaw;
    int secondaryGsmSignalStrengthRaw;
    int gsmSignalStrengthBars;
    int secondaryGsmSignalStrengthBars;
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
    int wifiSignalStrengthRaw;
    int wifiSignalStrengthBars;
    unsigned int wifiLowDataModeActive : 1;
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
    unsigned int displayRawGsmSignal : 1;
    unsigned int displayRawWifiSignal : 1;
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
    unsigned int wifiLinkWarning : 1;
    unsigned int wifiSearching : 1;
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
    char padding1;
    unsigned int overrideTimeString : 1;
    unsigned int overrideDateString : 1;
    unsigned int overrideGsmSignalStrengthRaw : 1;
    unsigned int overrideSecondaryGsmSignalStrengthRaw : 1;
    unsigned int overrideGsmSignalStrengthBars : 1;
    unsigned int overrideSecondaryGsmSignalStrengthBars : 1;
    unsigned int overrideServiceString : 1;
    unsigned int overrideSecondaryServiceString : 1;
    unsigned int overrideServiceImages : 2;
    unsigned int overrideOperatorDirectory : 1;
    unsigned int overrideServiceContentType : 1;
    unsigned int overrideSecondaryServiceContentType : 1;
    unsigned int overrideWifiSignalStrengthRaw : 1;
    unsigned int overrideWifiSignalStrengthBars : 1;
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
    unsigned int overrideDisplayRawGsmSignal : 1;
    unsigned int overrideDisplayRawWifiSignal : 1;
    unsigned int overridePersonName : 1;
    unsigned int overrideWifiLinkWarning : 1;
    unsigned int overrideSecondaryCellularConfigured : 1;
    unsigned int overridePrimaryServiceBadgeString : 1;
    unsigned int overrideSecondaryServiceBadgeString : 1;
    unsigned int overrideQuietModeImage : 1;
    unsigned int overrideExtra1 : 1;
    StatusBarRawData values;
};

// Getting setting

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

// Override funcs

bool StatusSetter16_3::isCarrierOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideServiceString == 1;
}

std::string StatusSetter16_3::getCarrierOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.serviceString);
    return text;
}

void StatusSetter16_3::setCarrier(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideServiceString = 1;
    strcpy(overrides->values.serviceString, text.c_str());
    strcpy(overrides->values.serviceCrossfadeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetCarrier()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideServiceString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isSecondaryCarrierOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryServiceString == 1;
}

std::string StatusSetter16_3::getSecondaryCarrierOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.secondaryServiceString);
    return text;
}

void StatusSetter16_3::setSecondaryCarrier(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceString = 1;
    strcpy(overrides->values.secondaryServiceString, text.c_str());
    strcpy(overrides->values.secondaryServiceCrossfadeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetSecondaryCarrier()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isPrimaryServiceBadgeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overridePrimaryServiceBadgeString == 1;
}

std::string StatusSetter16_3::getPrimaryServiceBadgeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.primaryServiceBadgeString);
    return text;
}

void StatusSetter16_3::setPrimaryServiceBadge(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overridePrimaryServiceBadgeString = 1;
    strcpy(overrides->values.primaryServiceBadgeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetPrimaryServiceBadge()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overridePrimaryServiceBadgeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isSecondaryServiceBadgeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryServiceBadgeString == 1;
}

std::string StatusSetter16_3::getSecondaryServiceBadgeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.secondaryServiceBadgeString);
    return text;
}

void StatusSetter16_3::setSecondaryServiceBadge(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceBadgeString = 1;
    strcpy(overrides->values.secondaryServiceBadgeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetSecondaryServiceBadge()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceBadgeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isDateOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDateString == 1;
}

std::string StatusSetter16_3::getDateOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.dateString);
    return text;
}

void StatusSetter16_3::setDate(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDateString = 1;
    strcpy(overrides->values.dateString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetDate()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDateString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isTimeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideTimeString == 1;
}

std::string StatusSetter16_3::getTimeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.timeString);
    return text;
}

void StatusSetter16_3::setTime(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideTimeString = 1;
    strcpy(overrides->values.timeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetTime()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideTimeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isBatteryDetailOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBatteryDetailString == 1;
}

std::string StatusSetter16_3::getBatteryDetailOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.batteryDetailString);
    return text;
}

void StatusSetter16_3::setBatteryDetail(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryDetailString = 1;
    strcpy(overrides->values.batteryDetailString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetBatteryDetail()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryDetailString = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isCrumbOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBreadcrumb == 1;
}

std::string StatusSetter16_3::getCrumbOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.breadcrumbTitle);
    if (text.size() > 1)
    {
        return text.substr(0, text.size() - 2);
    }
    return "";
}

void StatusSetter16_3::setCrumb(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBreadcrumb = 1;
    strcpy(overrides->values.breadcrumbTitle, text.append(" ▶").c_str());
    applyChanges(overrides);
}

void StatusSetter16_3::unsetCrumb()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBreadcrumb = 0;
    strcpy(overrides->values.breadcrumbTitle, std::string("").c_str());
    applyChanges(overrides);
}

bool StatusSetter16_3::isCellularServiceShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

void StatusSetter16_3::showCellularService(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (shown)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 1;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_3::isSecondaryCellularServiceShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] == 1;
}

void StatusSetter16_3::showSecondaryCellularService(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (shown)
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 1;
        overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 1;
    }
    else
    {
        overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_3::isDataNetworkTypeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDataNetworkType == 1;
}

int StatusSetter16_3::getDataNetworkTypeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.dataNetworkType;
    return id;
}

void StatusSetter16_3::setDataNetworkType(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDataNetworkType = 1;
    overrides->values.dataNetworkType = id;
    applyChanges(overrides);
}

void StatusSetter16_3::unsetDataNetworkType()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDataNetworkType = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isSecondaryDataNetworkTypeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryDataNetworkType == 1;
}

int StatusSetter16_3::getSecondaryDataNetworkTypeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.secondaryDataNetworkType;
    return id;
}

void StatusSetter16_3::setSecondaryDataNetworkType(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
    overrides->overrideDataNetworkType = 1;
    overrides->values.dataNetworkType = id;
    applyChanges(overrides);
}

void StatusSetter16_3::unsetSecondaryDataNetworkType()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 0;
    overrides->overrideSecondaryDataNetworkType = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isBatteryCapacityOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBatteryCapacity == 1;
}

int StatusSetter16_3::getBatteryCapacityOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.batteryCapacity;
    return id;
}

void StatusSetter16_3::setBatteryCapacity(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryCapacity = 1;
    overrides->values.batteryCapacity = id;
    applyChanges(overrides);
}

void StatusSetter16_3::unsetBatteryCapacity()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryCapacity = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isWiFiSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideWifiSignalStrengthBars == 1;
}

int StatusSetter16_3::getWiFiSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.wifiSignalStrengthBars;
    return id;
}

void StatusSetter16_3::setWiFiSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideWifiSignalStrengthBars = 1;
    overrides->values.wifiSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16_3::unsetWiFiSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideWifiSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isGsmSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideGsmSignalStrengthBars == 1;
}

int StatusSetter16_3::getGsmSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.gsmSignalStrengthBars;
    return id;
}

void StatusSetter16_3::setGsmSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 1;
    overrides->overrideGsmSignalStrengthBars = 1;
    overrides->values.gsmSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16_3::unsetGsmSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 0;
    overrides->overrideGsmSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isSecondaryGsmSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryGsmSignalStrengthBars == 1;
}

int StatusSetter16_3::getSecondaryGsmSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.secondaryGsmSignalStrengthBars;
    return id;
}

void StatusSetter16_3::setSecondaryGsmSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 1;
    overrides->overrideSecondaryGsmSignalStrengthBars = 1;
    overrides->values.secondaryGsmSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16_3::unsetSecondaryGsmSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 0;
    overrides->overrideSecondaryGsmSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16_3::isRawWiFiSignalShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDisplayRawWifiSignal == 1;
}

void StatusSetter16_3::showRawWiFiSignal(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (shown)
    {
        overrides->overrideDisplayRawWifiSignal = 1;
        overrides->values.displayRawWifiSignal = 1;
    }
    else
    {
        overrides->overrideDisplayRawWifiSignal = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_3::isRawGsmSignalShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDisplayRawGsmSignal == 1;
}

void StatusSetter16_3::showRawGsmSignal(bool shown)
{
    StatusBarOverrideData *overrides = getOverrides();
    if (shown)
    {
        overrides->overrideDisplayRawGsmSignal = 1;
        overrides->values.displayRawGsmSignal = 1;
    }
    else
    {
        overrides->overrideDisplayRawGsmSignal = 0;
    }

    applyChanges(overrides);
}

bool StatusSetter16_3::isDNDHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::QuietModeStatusBarItem)] == 1;
}

void StatusSetter16_3::hideDND(bool hidden)
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

bool StatusSetter16_3::isAirplaneHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirplaneModeStatusBarItem)] == 1;
}

void StatusSetter16_3::hideAirplane(bool hidden)
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

bool StatusSetter16_3::isCellHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

void StatusSetter16_3::hideCell(bool hidden)
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

bool StatusSetter16_3::isWiFiHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularDataNetworkStatusBarItem)] == 1;
}

void StatusSetter16_3::hideWiFi(bool hidden)
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

bool StatusSetter16_3::isBatteryHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::MainBatteryStatusBarItem)] == 1;
}

void StatusSetter16_3::hideBattery(bool hidden)
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

bool StatusSetter16_3::isBluetoothHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::BluetoothStatusBarItem)] == 1;
}

void StatusSetter16_3::hideBluetooth(bool hidden)
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

bool StatusSetter16_3::isAlarmHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AlarmStatusBarItem)] == 1;
}

void StatusSetter16_3::hideAlarm(bool hidden)
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

bool StatusSetter16_3::isLocationHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::LocationStatusBarItem)] == 1;
}

void StatusSetter16_3::hideLocation(bool hidden)
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

bool StatusSetter16_3::isRotationHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::RotationLockStatusBarItem)] == 1;
}

void StatusSetter16_3::hideRotation(bool hidden)
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

bool StatusSetter16_3::isAirPlayHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirPlayStatusBarItem)] == 1;
}

void StatusSetter16_3::hideAirPlay(bool hidden)
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

bool StatusSetter16_3::isCarPlayHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CarPlayStatusBarItem)] == 1;
}

void StatusSetter16_3::hideCarPlay(bool hidden)
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

bool StatusSetter16_3::isVPNHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::VPNStatusBarItem)] == 1;
}

void StatusSetter16_3::hideVPN(bool hidden)
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
