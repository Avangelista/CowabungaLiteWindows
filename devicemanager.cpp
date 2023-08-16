#include "DeviceManager.h"
#include "CreateBackup.h"
#include "HomeScreenApps.h"
#include "plistmanager.h"
#include "qprocess.h"
#include "qregularexpression.h"
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
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLayout>

DeviceManager::DeviceManager()
{
    // Constructor implementation
    currentDevice = std::nullopt;
    currentDeviceIndex = 0;
    deviceAvailable = false;
}

DeviceManager &DeviceManager::getInstance()
{
    static DeviceManager instance;
    return instance;
}

DeviceManager::~DeviceManager()
{
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

    if (Utils::copyDirectory(sourceDir, workspaceDir))
    {
        // fix this idk
    }
    else
    {
        qDebug() << "Error creating workspace directory";
    }

    setCurrentWorkspace(workspaceDir.toStdString());
}

const std::optional<std::string> DeviceManager::getCurrentWorkspace() const
{
    return this->currentWorkspace;
}

std::vector<DeviceInfo> DeviceManager::loadDevices()
{
    auto devices = std::vector<DeviceInfo>();

    char **device_list = nullptr;
    int device_count = 0;
    idevice_get_device_list(&device_list, &device_count);

    for (int i = 0; i < device_count; ++i)
    {
        char *uuid = device_list[i];

        if (uuid)
        {
            idevice_t device = nullptr;
            idevice_error_t error = idevice_new_with_options(&device, uuid, IDEVICE_LOOKUP_USBMUX);
            lockdownd_client_t client = nullptr;
            lockdownd_error_t lockdownd_error = lockdownd_client_new_with_handshake(device, &client, "DeviceManager");

            if (error == IDEVICE_E_SUCCESS && lockdownd_error == LOCKDOWN_E_SUCCESS)
            {
                plist_t device_info = nullptr;
                lockdownd_error_t info_error = lockdownd_get_value(client, nullptr, "ProductVersion", &device_info);

                if (info_error == LOCKDOWN_E_SUCCESS)
                {
                    char *device_version = nullptr;
                    plist_get_string_val(device_info, &device_version);

                    plist_t device_name_plist = nullptr;
                    lockdownd_error_t name_error = lockdownd_get_value(client, nullptr, "DeviceName", &device_name_plist);

                    if (name_error == LOCKDOWN_E_SUCCESS && device_name_plist && plist_get_node_type(device_name_plist) == PLIST_STRING)
                    {
                        char *device_name = nullptr;
                        plist_get_string_val(device_name_plist, &device_name);

                        DeviceInfo deviceInfo;
                        deviceInfo.UUID = uuid;
                        deviceInfo.Name = (device_name != nullptr) ? device_name : "";
                        deviceInfo.Version = (device_version != nullptr) ? Version(device_version) : Version();

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

    // If the same device is still there, set it as current
    if (this->currentDevice)
    {
        for (size_t i = 0; i < devices.size(); ++i)
        {
            if (devices.at(i).UUID == this->currentDevice->UUID)
            {
                setCurrentDeviceIndex(i);
                return devices;
            }
        }
    }

    // Otherwise, reset the current device
    resetCurrentDevice();
    return devices;
}

int DeviceManager::getCurrentDeviceIndex()
{
    return this->currentDeviceIndex;
}

bool DeviceManager::isDeviceAvailable()
{
    return this->deviceAvailable;
}

void DeviceManager::setCurrentDeviceIndex(int index)
{
    currentDeviceIndex = index;
    currentDevice = devices.at(index);
    // version check
    if (currentDevice->Version >= Version(15))
    {
        this->deviceAvailable = true;
        DeviceManager::configureWorkspace(currentDevice->UUID);
        this->homeScreenApps = HomeScreenApps::getHomeScreenApps();
        // fix - maybe we dont want these to reset?
        this->appsToAdd = {};
        this->borders = {};
        this->allIconsAdded = false;
        this->allIconsBordered = false;
        this->allNamesHidden = false;
    }
    else
    {
        this->deviceAvailable = false;
    }

}

// fix: do something to blank all pages?
void DeviceManager::resetCurrentDevice()
{
    this->currentWorkspace.reset();
    this->homeScreenApps.reset();
    this->appsToAdd = {};
    this->borders = {};
    this->allIconsAdded = false;
    this->allIconsBordered = false;
    this->allNamesHidden = false;
    this->enabledTweaks = {};
    if (this->devices.empty())
    {
        this->currentDeviceIndex = 0;
        this->currentDevice.reset();
        this->deviceAvailable = false;
    }
    else
    {
        this->enabledTweaks.insert(Tweak::SkipSetup);
        DeviceManager::setCurrentDeviceIndex(0);
    }
}

const std::optional<std::string> DeviceManager::getCurrentUUID() const
{
    if (this->currentDevice)
    {
        return this->currentDevice->UUID;
    }
    else
    {
        return std::nullopt;
    }
}

const std::optional<Version> DeviceManager::getCurrentVersion() const
{
    if (this->currentDevice)
    {
        return this->currentDevice->Version;
    }
    else
    {
        return std::nullopt;
    }
}

const std::optional<std::string> DeviceManager::getCurrentName() const
{
    if (this->currentDevice)
    {
        return this->currentDevice->Name;
    }
    else
    {
        return std::nullopt;
    }
}

const std::vector<std::string> DeviceManager::getAppBundles() const {
    std::vector<std::string> bundles;
    for (const auto& pair : *homeScreenApps) {
        bundles.push_back(pair.first);
    }

    return bundles;
}

const std::string DeviceManager::getAppName(std::string bundle) const {
    return std::get<std::string>(homeScreenApps->at(bundle).at("name"));
}

const std::optional<std::string> DeviceManager::getThemedName(std::string bundle) const {
    auto it = homeScreenApps->at(bundle).find("themed_name");
    if (it != homeScreenApps->at(bundle).end()) {
        return std::get<std::string>(it->second);
    }
    return std::nullopt;
}

const std::optional<std::string> DeviceManager::getUserName(std::string bundle) const {
    auto it = homeScreenApps->at(bundle).find("user_name");
    if (it != homeScreenApps->at(bundle).end()) {
        return std::get<std::string>(it->second);
    }
    return std::nullopt;
}

void DeviceManager::setUserName(std::string bundle, std::string name)
{
    homeScreenApps->at(bundle)["user_name"] = name;
}

const std::optional<std::vector<char>> DeviceManager::getIcon(std::string bundle) const {
    auto it = homeScreenApps->at(bundle).find("icon");
    if (it != homeScreenApps->at(bundle).end()) {
        return std::get<std::vector<char>>(it->second);
    }
    return std::nullopt;
}

const std::optional<std::vector<char>> DeviceManager::getThemedIcon(std::string bundle) const {
    auto it = homeScreenApps->at(bundle).find("themed_icon");
    if (it != homeScreenApps->at(bundle).end()) {
        return std::get<std::vector<char>>(it->second);
    }
    return std::nullopt;
}

const std::optional<std::string> DeviceManager::getUserIcon(std::string bundle) const {
    auto it = homeScreenApps->at(bundle).find("user_icon");
    if (it != homeScreenApps->at(bundle).end()) {
        return std::get<std::string>(it->second);
    }
    return std::nullopt;
}

void DeviceManager::setUserIcon(std::string bundle, std::string iconPath) {
    homeScreenApps->at(bundle)["user_icon"] = iconPath;
}

void DeviceManager::setBorder(std::string bundle, bool border) {
    if (border) {
        borders.emplace(bundle);
    } else {
        borders.erase(bundle);
    }
}

const bool DeviceManager::getBorder(std::string bundle) const {
    return borders.count(bundle) == 1;
}

void DeviceManager::setAddToDevice(std::string bundle, bool add) {
    if (add) {
        appsToAdd.emplace(bundle);
    } else {
        appsToAdd.erase(bundle);
    }
}

const bool DeviceManager::getAddToDevice(std::string bundle) const {
    return appsToAdd.count(bundle) == 1;
}

void DeviceManager::resetUserPrefs(std::string bundle) {
    homeScreenApps->at(bundle).erase("user_name");
    homeScreenApps->at(bundle).erase("user_icon");
    appsToAdd.erase(bundle);
    borders.erase(bundle);
}

const std::unordered_set<std::string> DeviceManager::getAppsToAdd() const {
    return appsToAdd;
}

bool DeviceManager::addAllIcons() {
    appsToAdd = {};
    if (!allIconsAdded) {
        for (const auto& pair : *homeScreenApps) {
            appsToAdd.emplace(pair.first);
        }
    }
    allIconsAdded = !allIconsAdded;
    return allIconsAdded;
}

bool DeviceManager::hideAllNames() {
    if (allNamesHidden) {
        for (const auto& pair : *homeScreenApps) {
            if (pair.second.find("user_name") != pair.second.end()) {
                if (std::get<std::string>(pair.second.at("user_name")) == "") {
                    homeScreenApps->at(pair.first).erase("user_name");
                    if (pair.second.find("themed_name") != pair.second.end() && std::get<std::string>(pair.second.at("themed_name")) == "") {
                        homeScreenApps->at(pair.first)["user_name"] = pair.second.at("name");
                    }
                }
            } else {
                if (pair.second.find("themed_name") != pair.second.end() && std::get<std::string>(pair.second.at("themed_name")) == "") {
                    homeScreenApps->at(pair.first)["user_name"] = pair.second.at("name");
                }
            }
        }
    } else {
        for (const auto& pair : *homeScreenApps) {
            homeScreenApps->at(pair.first)["user_name"] = "";
        }
    }
    allNamesHidden = !allNamesHidden;
    return allNamesHidden;
}

bool DeviceManager::borderAllIcons() {
    borders = {};
    if (!allIconsBordered) {
        for (const auto& pair : *homeScreenApps) {
            borders.emplace(pair.first);
        }
    }
    allIconsBordered = !allIconsBordered;
    return allIconsBordered;
}

void DeviceManager::applyTheme(QDir themePath) {
    QStringList pngFiles = themePath.entryList(QStringList() << "*.png");

    QRegularExpression regex("(\\.png$|-large\\.png$|@2x\\.png$|@3x\\.png$)", QRegularExpression::CaseInsensitiveOption);

    for (const QString& pngFile : pngFiles) {
        auto fileName = QFileInfo(pngFile).fileName().replace(regex, "").toStdString();

        auto it = homeScreenApps->find(fileName);
        if (it != homeScreenApps->end()) {
            appsToAdd.emplace(fileName);
            it->second["user_icon"] = themePath.filePath(pngFile).toStdString();
        }
    }
}

void DeviceManager::setTweakEnabled(Tweak t, bool enabled = true)
{
    if (enabled)
    {
        this->enabledTweaks.insert(t);
    }
    else
    {
        this->enabledTweaks.erase(t);
    }
}

bool DeviceManager::isTweakEnabled(Tweak t)
{
    return this->enabledTweaks.find(t) != this->enabledTweaks.end();
}

std::vector<Tweak> DeviceManager::getEnabledTweaks()
{
    auto tweaks = std::vector<Tweak>(this->enabledTweaks.begin(), this->enabledTweaks.end());

    // Sort the vector based on the tweak description
    std::sort(tweaks.begin(), tweaks.end(), [](auto a, auto b)
              { return Tweaks::getTweakData(a).description < Tweaks::getTweakData(b).description; });

    return tweaks;
}

void DeviceManager::removeTweaks(QLabel *statusLabel, bool deepClean) {
    statusLabel->setText("Copying restore files...");

    // Set the source directory path (assuming it's located in the binary directory)
    auto sourceDir = QCoreApplication::applicationDirPath();
    if (deepClean) {
        sourceDir +=  + "/restore-deepclean";
    } else {
        sourceDir +=  + "/restore";
    }
    auto enabledTweaksDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/EnabledTweaks";
    auto enabledTweaksDirectory = QDir(enabledTweaksDirectoryPath);
    if (enabledTweaksDirectory.exists())
    {
        enabledTweaksDirectory.removeRecursively();
    }

    if (Utils::copyDirectory(sourceDir, enabledTweaksDirectoryPath))
    {
        // fix this idk
    }
    else
    {
        qDebug() << "Error creating workspace directory";
    }

    statusLabel->setText("Generating backup...");

    auto backupDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Backup";

    CreateBackup::createBackup(enabledTweaksDirectoryPath, backupDirectoryPath);

    statusLabel->setText("Restoring backup to device...");

    auto success = DeviceManager::restoreBackupToDevice(*DeviceManager::getCurrentUUID(), QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());

    if (success) {
        statusLabel->setText("Done!");
    } else {
        statusLabel->setText("Failed.");
    }
}

void DeviceManager::applyTweaks(QLabel *statusLabel)
{
    auto workspace = DeviceManager::getCurrentWorkspace();

    if (DeviceManager::isTweakEnabled(Tweak::Themes)) {

        statusLabel->setText("Generating theme folder...");

        // Erase theme folder
        auto themeDirectoryPath = QString::fromStdString(*workspace) + "/Theme/HomeDomain/Library/WebClips";
        auto themeDirectory = QDir(themeDirectoryPath);
        if (themeDirectory.exists())
        {
            themeDirectory.removeRecursively();
        }

        // Generate theme folder
        for (auto bundle : DeviceManager::getInstance().getAppsToAdd()) {
            auto name = DeviceManager::getInstance().getAppName(bundle);
            auto themed_name = DeviceManager::getInstance().getThemedName(bundle);
            auto user_name = DeviceManager::getInstance().getUserName(bundle);
            auto icon = DeviceManager::getInstance().getIcon(bundle);
            auto themed_icon = DeviceManager::getInstance().getThemedIcon(bundle);
            auto user_icon = DeviceManager::getInstance().getUserIcon(bundle);
            auto border = DeviceManager::getInstance().getBorder(bundle);
            QString iconDirectoryPath = "Cowabunga_" + QString::fromStdString(bundle + "," + name + ".webclip");
            themeDirectory.mkpath(iconDirectoryPath);
            auto iconDirectory = QDir(themeDirectory.absoluteFilePath(iconDirectoryPath));

            if (user_icon) {
                if (!QFile::copy(QString::fromStdString(*user_icon), iconDirectory.absolutePath() + "/icon.png")) {
                    qDebug() << "Error copying theme icon!";
                }
            } else if (themed_icon) {
                QString iconFilePath = iconDirectory.absoluteFilePath("icon.png");
                QFile file(iconFilePath);
                if (file.open(QIODevice::WriteOnly)) {
                    QDataStream out(&file);
                    out.writeRawData(themed_icon->data(), themed_icon->size());
                    file.close();
                } else {
                    qDebug() << "Error: Unable to write the icon data to file.";
                }
            } else if (icon) {
                QString iconFilePath = iconDirectory.absoluteFilePath("icon.png");
                QFile file(iconFilePath);
                if (file.open(QIODevice::WriteOnly)) {
                    QDataStream out(&file);
                    out.writeRawData(icon->data(), icon->size());
                    file.close();
                } else {
                    qDebug() << "Error: Unable to write the icon data to file.";
                }
            }

            auto plistFilePath = iconDirectory.absoluteFilePath("Info.plist");
            PlistManager::createEmptyPlist(plistFilePath, false);
            auto ApplicationBundleIdentifier = PList::String(bundle);
            PlistManager::setPlistValue(plistFilePath, "ApplicationBundleIdentifier", ApplicationBundleIdentifier);
            if (user_name) {
                auto Title = PList::String(*user_name);
                PlistManager::setPlistValue(plistFilePath, "Title", Title);
            } else if (themed_name) {
                auto Title = PList::String(*themed_name);
                PlistManager::setPlistValue(plistFilePath, "Title", Title);
            } else {
                auto Title = PList::String(name);
                PlistManager::setPlistValue(plistFilePath, "Title", Title);
            }
            if (border) {
                auto IsAppClip = PList::Boolean(true);
                PlistManager::setPlistValue(plistFilePath, "IsAppClip", IsAppClip);
            }
        }
    }

    statusLabel->setText("Copying enabled tweaks...");

    // Erase backup folder
    auto enabledTweaksDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/EnabledTweaks";
    auto enabledTweaksDirectory = QDir(enabledTweaksDirectoryPath);
    if (enabledTweaksDirectory.exists())
    {
        enabledTweaksDirectory.removeRecursively();
    }

    for (auto t : DeviceManager::getEnabledTweaks())
    {
        auto folderName = Tweaks::getTweakData(t).folderName;
        Utils::copyDirectory(QString::fromStdString(*workspace + "/" + folderName), enabledTweaksDirectoryPath);
    }

    statusLabel->setText("Generating backup...");

    auto backupDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Backup";

    CreateBackup::createBackup(enabledTweaksDirectoryPath, backupDirectoryPath);

    statusLabel->setText("Restoring backup to device...");

    auto success = DeviceManager::restoreBackupToDevice(*DeviceManager::getCurrentUUID(), QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());

    if (success) {
        statusLabel->setText("Done!");
    } else {
        statusLabel->setText("Failed.");
    }
}

bool DeviceManager::restoreBackupToDevice(const std::string &udid, const std::string &backupDirectory)
{
    QStringList arguments;
    // do we need "settings"? it seems to make it faster, maybe it messes something?
    arguments << "-u" << QString::fromStdString(udid) << "-s" << "Backup" << "restore" << "--system" << "--skip-apps" << QString::fromStdString(backupDirectory);

    QProcess process;
    process.start("idevicebackup2.exe", arguments);
    process.waitForFinished(-1);

    QByteArray output = process.readAllStandardOutput();
    QByteArray errorOutput = process.readAllStandardError();

    // Split the output into lines using '\n' as the separator
    // AAA Fix using \r\n
    QStringList outputLines = QString(output).split("\r\n");

    // Get the last two lines of the output
    QString lastLine;
    QString secondLastLine;
    if (outputLines.size() >= 3) {
        lastLine = outputLines.at(outputLines.size() - 2);
        secondLastLine = outputLines.at(outputLines.size() - 3);
    } else {
        lastLine = output;
        secondLastLine = errorOutput;
    }

    if (lastLine == "Restore Successful.")
    {
        QMessageBox::information(nullptr, "Success!", "All done! Your device will now restart.\n\nYou should see a black loading screen after entering your passcode - it will disappear after a few seconds.\n\nImportant: If you are presented with a setup, select \"Customize\" > \"Don't transfer apps and data\" and your phone should return to the homescreen as normal.");
        return true;
    }
    QMessageBox detailsMessageBox;
    detailsMessageBox.setWindowTitle("Error!");
    detailsMessageBox.setIcon(QMessageBox::Critical);
    detailsMessageBox.setText(lastLine + "\n" + secondLastLine);
    detailsMessageBox.setTextInteractionFlags(Qt::TextSelectableByMouse);
    detailsMessageBox.setDetailedText(errorOutput + "\n" + output);
    detailsMessageBox.exec();
    return false;
}
