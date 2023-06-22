#include "DeviceManager.h"
#include "CreateBackup.h"
#include "utils.h"
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/restore.h>
#include <plist/plist.h>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDirIterator>
#include <QMessageBox>

DeviceManager::DeviceManager() {
    // Constructor implementation
    currentDevice = std::nullopt;
    currentDeviceIndex = 0;
    deviceAvailable = false;
}

DeviceManager& DeviceManager::getInstance() {
    static DeviceManager instance;
    return instance;
}

DeviceManager::~DeviceManager() {
    // Destructor implementation
}

void DeviceManager::setCurrentWorkspace(std::string str)
{
    this->currentWorkspace = str;
}

void DeviceManager::configureWorkspace(std::string uuid)
{
    // Get the destination directory path
    auto workspaceDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Workspace/" + QString::fromStdString(uuid);

    // Set the source directory path (assuming it's located in the binary directory)
    auto sourceDir = QCoreApplication::applicationDirPath() + "/files";

    if(Utils::copyDirectory(sourceDir, workspaceDir)) {
        // fix this idk
    } else {
        qDebug() << "Error creating workspace directory";
    }

    setCurrentWorkspace(workspaceDir.toStdString());
}

const std::optional<std::string> DeviceManager::getCurrentWorkspace() const
{
    return this->currentWorkspace;
}

std::vector<DeviceInfo> DeviceManager::loadDevices() {
    auto devices = std::vector<DeviceInfo>();

    // cbs to do auto here
    char** device_list = nullptr;
    int device_count = 0;
    idevice_get_device_list(&device_list, &device_count);

    for (int i = 0; i < device_count; ++i) {
        char* uuid = device_list[i];

        if (uuid) {
            idevice_t device = nullptr;
            idevice_error_t error = idevice_new_with_options(&device, uuid, IDEVICE_LOOKUP_USBMUX);
            lockdownd_client_t client = nullptr;
            lockdownd_error_t lockdownd_error = lockdownd_client_new_with_handshake(device, &client, "DeviceManager");

            if (error == IDEVICE_E_SUCCESS && lockdownd_error == LOCKDOWN_E_SUCCESS) {
                plist_t device_info = nullptr;
                lockdownd_error_t info_error = lockdownd_get_value(client, nullptr, "ProductVersion", &device_info);

                if (info_error == LOCKDOWN_E_SUCCESS) {
                    char* device_version = nullptr;
                    plist_get_string_val(device_info, &device_version);

                    plist_t device_name_plist = nullptr;
                    lockdownd_error_t name_error = lockdownd_get_value(client, nullptr, "DeviceName", &device_name_plist);

                    if (name_error == LOCKDOWN_E_SUCCESS && device_name_plist && plist_get_node_type(device_name_plist) == PLIST_STRING) {
                        char* device_name = nullptr;
                        plist_get_string_val(device_name_plist, &device_name);

                        DeviceInfo deviceInfo;
                        deviceInfo.UUID = uuid;
                        deviceInfo.Name = (device_name != nullptr) ? device_name : "";
                        deviceInfo.Version = (device_version != nullptr) ? device_version : "";

                        devices.push_back(deviceInfo);

                        if (device_name)
                            free(device_name);
                    }

                    if (device_name_plist)
                        plist_free(device_name_plist);

                    if (device_version)
                        free(device_version);
                }

                if (device_info)
                    plist_free(device_info);
            }

            if (client)
                lockdownd_client_free(client);

            if (device)
                idevice_free(device);
        }
    }

    if (device_list)
        idevice_device_list_free(device_list);

    this->devices = devices;

    if (this->currentDevice) {
        for (size_t i = 0; i < devices.size(); ++i) {
            if (devices.at(i).UUID == this->currentDevice->UUID) {
                setCurrentDeviceIndex(i);
                return devices;
            }
        }
    }

    resetCurrentDevice();
    return devices;
}

int DeviceManager::getCurrentDeviceIndex() {
    return this->currentDeviceIndex;
}

bool DeviceManager::isDeviceAvailable() {
    return this->deviceAvailable;
}

void DeviceManager::setCurrentDeviceIndex(int index) {
    if (index >= 0 && index < devices.size()) {
        currentDeviceIndex = index;
        currentDevice = devices.at(index);
        // version check
        this->deviceAvailable = false;
        auto dotIndex = currentDevice->Version.find('.');
        if (dotIndex != std::string::npos) {
            auto majorVersionStr = currentDevice->Version.substr(0, dotIndex);
            auto majorVersion = std::stoi(majorVersionStr);
            if (majorVersion >= 15) {
                this->deviceAvailable = true;
            }
        }
        // set up workspace
        DeviceManager::configureWorkspace(currentDevice->UUID);
    } else {
        qDebug() << "Invalid device index: " << index;
    }
}

void DeviceManager::resetCurrentDevice() {
    this->currentWorkspace.reset();
    if (this->devices.empty()) {
        this->currentDeviceIndex = 0;
        this->currentDevice.reset();
        this->deviceAvailable = false;
    } else {
        DeviceManager::setCurrentDeviceIndex(0);
    }
}

const std::optional<std::string> DeviceManager::getCurrentUUID() const {
    if (this->currentDevice) {
        return this->currentDevice->UUID;
    } else {
        return std::nullopt;
    }
}

const std::optional<std::string> DeviceManager::getCurrentVersion() const {
    if (this->currentDevice) {
        return this->currentDevice->Version;
    } else {
        return std::nullopt;
    }
}

const std::optional<std::string> DeviceManager::getCurrentName() const {
    if (this->currentDevice) {
        return this->currentDevice->Name;
    } else {
        return std::nullopt;
    }
}

void DeviceManager::setTweakEnabled(Tweak t, bool enabled = true) {
    if (enabled) {
        this->enabledTweaks.insert(t);
        qDebug() << "Tweak added: " << Tweaks::getTweakData(t).description;
    } else {
        this->enabledTweaks.erase(t);
        qDebug() << "Tweak removed: " << Tweaks::getTweakData(t).description;
    }
}

bool DeviceManager::isTweakEnabled(Tweak t) {
    return this->enabledTweaks.find(t) != this->enabledTweaks.end();
}

std::vector<Tweak> DeviceManager::getEnabledTweaks() {
    auto tweaks = std::vector<Tweak>(this->enabledTweaks.begin(), this->enabledTweaks.end());

    // Sort the vector based on the tweak description
    std::sort(tweaks.begin(), tweaks.end(), [](auto a, auto b) {
        return Tweaks::getTweakData(a).description < Tweaks::getTweakData(b).description;
    });

    return tweaks;
}

void DeviceManager::applyTweaks() {
    auto workspace = DeviceManager::getCurrentWorkspace();
    if (!workspace) {
        qDebug() << "where da workspace??";
        return;
    }

    // Erase backup folder
    auto enabledTweaksDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/EnabledTweaks";
    auto enabledTweaksDirectory = QDir(enabledTweaksDirectoryPath);
    if (enabledTweaksDirectory.exists()) {
        enabledTweaksDirectory.removeRecursively();
    }

    for (auto t : DeviceManager::getEnabledTweaks()) {
        auto folderName = Tweaks::getTweakData(t).folderName;
        qDebug() << "Copying tweak " << Tweaks::getTweakData(t).description;
        Utils::copyDirectory(QString::fromStdString(*workspace + "/" + folderName), enabledTweaksDirectoryPath);
    }

    auto backupDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Backup";

    CreateBackup::createBackup(enabledTweaksDirectoryPath.toStdString(), backupDirectoryPath.toStdString());

    DeviceManager::restoreBackupToDevice(*DeviceManager::getCurrentUUID(), QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
}

int DeviceManager::restoreBackupToDevice(const std::string& udid, const std::string& backupDirectory) {
    std::string command = "idevicebackup2.exe -u " + udid + " -s Backup restore --system --skip-apps --no-reboot " + backupDirectory;
    FILE* pipe = _popen(command.c_str(), "r");
    if (pipe == nullptr) {
        std::cerr << "Failed to execute command." << std::endl;
        return 1;
    }
    char buffer[128];
    char last[128];
    while (fgets(buffer, 128, pipe) != nullptr) {
        std::cout << buffer << std::endl;
        strcpy(last, buffer);
    }
    _pclose(pipe);
    auto result = std::string(last);
    if (result == "Restore Successful.\n") {
        QMessageBox::information(nullptr, "Success", "Tweaks applied! Your device will now restart.\n\nImportant: If you are presented with a setup, select \"Don't transfer apps and data\" and your phone should return to the homescreen as normal.");
    } else {
        QMessageBox::critical(nullptr, "Error", QString::fromStdString(result));
    }

    return 0;
}
