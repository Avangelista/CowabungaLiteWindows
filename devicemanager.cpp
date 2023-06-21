#include "DeviceManager.h"
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDirIterator>

//DeviceManager DeviceManager::instance; // Initialize static instance

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
    QString workspaceDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Workspace/" + QString::fromStdString(uuid);

    // Create the destination directory if it doesn't exist
    QDir().mkpath(workspaceDir);

    // Set the source directory path (assuming it's located in the binary directory)
    QString sourceDir = QCoreApplication::applicationDirPath() + "/files";

    // Create a QDirIterator to iterate over the source directory and its contents
    QDirIterator iterator(sourceDir, QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (iterator.hasNext()) {
        iterator.next();

        QString sourceFilePath = iterator.filePath();
        QString relativeFilePath = iterator.fileInfo().absoluteFilePath().replace(sourceDir, "");
        QString destinationFilePath = workspaceDir + relativeFilePath;

        // Create the destination directory path if necessary
        QDir().mkpath(QFileInfo(destinationFilePath).absolutePath());

        // Copy the file or directory
        if (iterator.fileInfo().isFile()) {
            if (QFile::copy(sourceFilePath, destinationFilePath)) {
//                qDebug() << "Copied file:" << relativeFilePath;
            } else {
//                qDebug() << "Failed to copy file:" << relativeFilePath;
            }
        } else if (iterator.fileInfo().isDir()) {
            if (QDir().mkdir(destinationFilePath)) {
//                qDebug() << "Created directory:" << relativeFilePath;
            } else {
//                qDebug() << "Failed to create directory:" << relativeFilePath;
            }
        }
    }

    setCurrentWorkspace(workspaceDir.toStdString());
}

const std::optional<std::string> DeviceManager::getCurrentWorkspace() const
{
    return this->currentWorkspace;
}

std::vector<DeviceInfo> DeviceManager::loadDevices() {
    std::vector<DeviceInfo> devices;

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
        size_t dotIndex = currentDevice->Version.find('.');
        if (dotIndex != std::string::npos) {
            std::string majorVersionStr = currentDevice->Version.substr(0, dotIndex);
            int majorVersion = std::stoi(majorVersionStr);
            if (majorVersion >= 16) {
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
    this->currentWorkspace = std::nullopt;
    if (this->devices.empty()) {
        this->currentDeviceIndex = 0;
        this->currentDevice = std::nullopt;
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


