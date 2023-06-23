#include "StatusSetter16.h"
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
    Extra1StatusBarItem = 44
};

enum class BatteryState : unsigned int
{
    BatteryStateUnplugged = 0
};

struct StatusBarRawData
{
    bool itemIsEnabled[45];
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

namespace {
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

bool StatusSetter16::isCarrierOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideServiceString == 1;
}

std::string StatusSetter16::getCarrierOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.serviceString);
    return text;
}

void StatusSetter16::setCarrier(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideServiceString = 1;
    strcpy(overrides->values.serviceString, text.c_str());
    strcpy(overrides->values.serviceCrossfadeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetCarrier()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideServiceString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isSecondaryCarrierOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryServiceString == 1;
}

std::string StatusSetter16::getSecondaryCarrierOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.secondaryServiceString);
    return text;
}

void StatusSetter16::setSecondaryCarrier(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceString = 1;
    strcpy(overrides->values.secondaryServiceString, text.c_str());
    strcpy(overrides->values.secondaryServiceCrossfadeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetSecondaryCarrier()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isPrimaryServiceBadgeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overridePrimaryServiceBadgeString == 1;
}

std::string StatusSetter16::getPrimaryServiceBadgeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.primaryServiceBadgeString);
    return text;
}

void StatusSetter16::setPrimaryServiceBadge(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overridePrimaryServiceBadgeString = 1;
    strcpy(overrides->values.primaryServiceBadgeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetPrimaryServiceBadge()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overridePrimaryServiceBadgeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isSecondaryServiceBadgeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryServiceBadgeString == 1;
}

std::string StatusSetter16::getSecondaryServiceBadgeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.secondaryServiceBadgeString);
    return text;
}

void StatusSetter16::setSecondaryServiceBadge(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceBadgeString = 1;
    strcpy(overrides->values.secondaryServiceBadgeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetSecondaryServiceBadge()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideSecondaryServiceBadgeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isDateOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDateString == 1;
}

std::string StatusSetter16::getDateOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.dateString);
    return text;
}

void StatusSetter16::setDate(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDateString = 1;
    strcpy(overrides->values.dateString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetDate()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDateString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isTimeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideTimeString == 1;
}

std::string StatusSetter16::getTimeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.timeString);
    return text;
}

void StatusSetter16::setTime(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideTimeString = 1;
    strcpy(overrides->values.timeString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetTime()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideTimeString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isBatteryDetailOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBatteryDetailString == 1;
}

std::string StatusSetter16::getBatteryDetailOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.batteryDetailString);
    return text;
}

void StatusSetter16::setBatteryDetail(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryDetailString = 1;
    strcpy(overrides->values.batteryDetailString, text.c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetBatteryDetail()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryDetailString = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isCrumbOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBreadcrumb == 1;
}

std::string StatusSetter16::getCrumbOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    std::string text = std::string(overrides->values.breadcrumbTitle);
    if (text.size() > 1)
    {
        return text.substr(0, text.size() - 2);
    }
    return "";
}

void StatusSetter16::setCrumb(std::string text)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBreadcrumb = 1;
    strcpy(overrides->values.breadcrumbTitle, text.append(" ▶").c_str());
    applyChanges(overrides);
}

void StatusSetter16::unsetCrumb()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBreadcrumb = 0;
    strcpy(overrides->values.breadcrumbTitle, std::string("").c_str());
    applyChanges(overrides);
}

bool StatusSetter16::isCellularServiceShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

void StatusSetter16::showCellularService(bool shown)
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

bool StatusSetter16::isSecondaryCellularServiceShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularServiceStatusBarItem)] == 1;
}

void StatusSetter16::showSecondaryCellularService(bool shown)
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

bool StatusSetter16::isDataNetworkTypeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDataNetworkType == 1;
}

int StatusSetter16::getDataNetworkTypeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.dataNetworkType;
    return id;
}

void StatusSetter16::setDataNetworkType(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDataNetworkType = 1;
    overrides->values.dataNetworkType = id;
    applyChanges(overrides);
}

void StatusSetter16::unsetDataNetworkType()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideDataNetworkType = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isSecondaryDataNetworkTypeOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryDataNetworkType == 1;
}

int StatusSetter16::getSecondaryDataNetworkTypeOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.secondaryDataNetworkType;
    return id;
}

void StatusSetter16::setSecondaryDataNetworkType(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 1;
    overrides->overrideDataNetworkType = 1;
    overrides->values.dataNetworkType = id;
    applyChanges(overrides);
}

void StatusSetter16::unsetSecondaryDataNetworkType()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularDataNetworkStatusBarItem)] = 0;
    overrides->overrideSecondaryDataNetworkType = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isBatteryCapacityOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideBatteryCapacity == 1;
}

int StatusSetter16::getBatteryCapacityOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.batteryCapacity;
    return id;
}

void StatusSetter16::setBatteryCapacity(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryCapacity = 1;
    overrides->values.batteryCapacity = id;
    applyChanges(overrides);
}

void StatusSetter16::unsetBatteryCapacity()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideBatteryCapacity = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isWiFiSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideWiFiSignalStrengthBars == 1;
}

int StatusSetter16::getWiFiSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.WiFiSignalStrengthBars;
    return id;
}

void StatusSetter16::setWiFiSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideWiFiSignalStrengthBars = 1;
    overrides->values.WiFiSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16::unsetWiFiSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideWiFiSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isGSMSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideGSMSignalStrengthBars == 1;
}

int StatusSetter16::getGSMSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.GSMSignalStrengthBars;
    return id;
}

void StatusSetter16::setGSMSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 1;
    overrides->overrideGSMSignalStrengthBars = 1;
    overrides->values.GSMSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16::unsetGSMSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularSignalStrengthStatusBarItem)] = 0;
    overrides->overrideGSMSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isSecondaryGSMSignalStrengthBarsOverridden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideSecondaryGSMSignalStrengthBars == 1;
}

int StatusSetter16::getSecondaryGSMSignalStrengthBarsOverride()
{
    StatusBarOverrideData *overrides = getOverrides();
    int id = overrides->values.secondaryGSMSignalStrengthBars;
    return id;
}

void StatusSetter16::setSecondaryGSMSignalStrengthBars(int id)
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 1;
    overrides->values.itemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 1;
    overrides->overrideSecondaryGSMSignalStrengthBars = 1;
    overrides->values.secondaryGSMSignalStrengthBars = id;
    applyChanges(overrides);
}

void StatusSetter16::unsetSecondaryGSMSignalStrengthBars()
{
    StatusBarOverrideData *overrides = getOverrides();
    overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::SecondaryCellularSignalStrengthStatusBarItem)] = 0;
    overrides->overrideSecondaryGSMSignalStrengthBars = 0;
    applyChanges(overrides);
}

bool StatusSetter16::isRawWiFiSignalShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDisplayRawWiFiSignal == 1;
}

void StatusSetter16::showRawWiFiSignal(bool shown)
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

bool StatusSetter16::isRawGSMSignalShown()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideDisplayRawGSMSignal == 1;
}

void StatusSetter16::showRawGSMSignal(bool shown)
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

bool StatusSetter16::isDNDHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::QuietModeStatusBarItem)] == 1;
}

void StatusSetter16::hideDND(bool hidden)
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

bool StatusSetter16::isAirplaneHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirplaneModeStatusBarItem)] == 1;
}

void StatusSetter16::hideAirplane(bool hidden)
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

bool StatusSetter16::isCellHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularServiceStatusBarItem)] == 1;
}

void StatusSetter16::hideCell(bool hidden)
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

bool StatusSetter16::isWiFiHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CellularDataNetworkStatusBarItem)] == 1;
}

void StatusSetter16::hideWiFi(bool hidden)
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

bool StatusSetter16::isBatteryHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::MainBatteryStatusBarItem)] == 1;
}

void StatusSetter16::hideBattery(bool hidden)
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

bool StatusSetter16::isBluetoothHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::BluetoothStatusBarItem)] == 1;
}

void StatusSetter16::hideBluetooth(bool hidden)
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

bool StatusSetter16::isAlarmHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AlarmStatusBarItem)] == 1;
}

void StatusSetter16::hideAlarm(bool hidden)
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

bool StatusSetter16::isLocationHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::LocationStatusBarItem)] == 1;
}

void StatusSetter16::hideLocation(bool hidden)
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

bool StatusSetter16::isRotationHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::RotationLockStatusBarItem)] == 1;
}

void StatusSetter16::hideRotation(bool hidden)
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

bool StatusSetter16::isAirPlayHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::AirPlayStatusBarItem)] == 1;
}

void StatusSetter16::hideAirPlay(bool hidden)
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

bool StatusSetter16::isCarPlayHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::CarPlayStatusBarItem)] == 1;
}

void StatusSetter16::hideCarPlay(bool hidden)
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

bool StatusSetter16::isVPNHidden()
{
    StatusBarOverrideData *overrides = getOverrides();
    return overrides->overrideItemIsEnabled[static_cast<int>(StatusBarItem::VPNStatusBarItem)] == 1;
}

void StatusSetter16::hideVPN(bool hidden)
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
