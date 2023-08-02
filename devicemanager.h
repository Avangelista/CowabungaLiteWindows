#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "utils.h"
#include <vector>
#include <unordered_set>
#include <string>
#include <optional>

struct DeviceInfo {
    std::string UUID;
    std::string Name;
    Version Version;
};

class DeviceManager {
public:
    static DeviceManager& getInstance();

    std::vector<DeviceInfo> loadDevices();

    int getCurrentDeviceIndex();
    void setCurrentDeviceIndex(int index);
    void resetCurrentDevice();

    const std::optional<std::string> getCurrentWorkspace() const;

    const std::optional<std::string> getCurrentUUID() const;
    const std::optional<Version> getCurrentVersion() const;
    const std::optional<std::string> getCurrentName() const;

    bool isDeviceAvailable();
    void setTweakEnabled(Tweak t, bool);
    bool isTweakEnabled(Tweak t);
    std::vector<Tweak> getEnabledTweaks();

    const std::vector<std::string> getAppBundles() const;
    const std::string getAppName(std::string bundle) const;
    const std::optional<std::string> getThemedName(std::string bundle) const;
    const std::optional<std::string> getUserName(std::string bundle) const;
    void setUserName(std::string bundle, std::string name);
    const std::optional<std::vector<char>> getIcon(std::string bundle) const;
    const std::optional<std::vector<char>> getThemedIcon(std::string bundle) const;
    const std::optional<std::string> getUserIcon(std::string bundle) const;
    void setUserIcon(std::string bundle, std::string iconPath);
    void setBorder(std::string bundle, bool border);
    bool getBorder(std::string bundle);
    void setAddToDevice(std::string bundle, bool add);
    const bool getAddToDevice(std::string bundle) const;
    const std::unordered_set<std::string> getAppsToAdd() const;
    void resetUserPrefs(std::string bundle);

    void applyTweaks();
private:
    DeviceManager();
    ~DeviceManager();

    static DeviceManager instance;

    void setCurrentWorkspace(std::string);
    void configureWorkspace(std::string);

    std::optional<DeviceInfo> currentDevice;
    int currentDeviceIndex;
    std::vector<DeviceInfo> devices;
    bool deviceAvailable;

    std::optional<std::unordered_map<std::string, std::unordered_map<std::string, std::variant<std::string, std::vector<char>, int>>>> homeScreenApps;
    std::unordered_set<std::string> appsToAdd;
    std::unordered_set<std::string> borders;

    std::unordered_set<Tweak> enabledTweaks;

    std::optional<std::string> currentWorkspace;

    int restoreBackupToDevice(const std::string&, const std::string&);
};

#endif // DEVICEMANAGER_H
