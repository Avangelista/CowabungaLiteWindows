#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DeviceManager.h"
#include "qdir.h"
#include "qmessagebox.h"
#include "qprocess.h"
#include "qstandardpaths.h"
#include "statusmanager/StatusManager.h"
#include "plistmanager.h"
#include <QDebug>
#include <QWindow>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QScrollBar>
#include <QDesktopServices>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/service.h>
#include <endianness.h>

#define DT_SIMULATELOCATION_SERVICE "com.apple.dt.simulatelocation"

enum {
    SET_LOCATION = 0,
    RESET_LOCATION = 1
};

// Boilerplate

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CowabungaLite)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    ui->pages->setCurrentIndex(static_cast<int>(Page::Home));
    refreshDevices();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Utilities

void MainWindow::updateInterfaceForNewDevice()
{
    // Update names
    MainWindow::updatePhoneInfo();

    if (DeviceManager::getInstance().isDeviceAvailable())
    {
        // load locsim
        ui->locSimCnt->hide();
        ui->loadLocSimBtn->show();
        if (DeviceManager::getInstance().getCurrentVersion() >= Version(17)) {
            ui->loadLocSimBtn->setEnabled(false);
            ui->loadLocSimBtn->setText("Unavailable");
        } else {
            ui->loadLocSimBtn->setEnabled(true);
            ui->loadLocSimBtn->setText("Start Location Simulation");
        }

//        MainWindow::loadExplorePage();
        MainWindow::loadThemesPage();
        MainWindow::loadStatusBar();
        MainWindow::loadControlCenter();
        MainWindow::loadSpringboardOptions();
        MainWindow::loadInternalOptions();
        MainWindow::loadSetupOptions();
    }
    MainWindow::updateEnabledTweaks();
}

// Sidebar

void MainWindow::refreshDevices()
{
    // Clear existing items in the combobox
    ui->devicePicker->clear();

    // Load devices
    auto devices = DeviceManager::getInstance().loadDevices();

    if (devices.empty())
    {
        ui->devicePicker->setEnabled(false);
        ui->devicePicker->addItem(QString("None"), QVariant::fromValue(NULL));
        ui->pages->setCurrentIndex(static_cast<int>(Page::Home));
        ui->homePageBtn->setChecked(true);

        // hide all pages
        ui->locSimPageBtn->hide();
        ui->sidebarDiv1->hide();
        ui->themesPageBtn->hide();
        ui->statusBarPageBtn->hide();
        ui->controlCenterPageBtn->hide();
        ui->springboardOptionsPageBtn->hide();
        ui->internalOptionsPageBtn->hide();
        ui->setupOptionsPageBtn->hide();
        ui->sidebarDiv2->hide();
        ui->applyPageBtn->hide();
    }
    else
    {
        ui->devicePicker->setEnabled(true);
        // Populate the combobox with device names
        for (auto &device : devices)
        {
            auto deviceName = QString("%1").arg(QString::fromStdString(device.Name));
            ui->devicePicker->addItem(deviceName, QVariant::fromValue(device.UUID));
        }

        // show all pages
        ui->locSimPageBtn->show();
        ui->sidebarDiv1->show();
        ui->themesPageBtn->show();
        ui->statusBarPageBtn->show();
        ui->controlCenterPageBtn->show();
        ui->springboardOptionsPageBtn->show();
        ui->internalOptionsPageBtn->show();
        ui->setupOptionsPageBtn->show();
        ui->sidebarDiv2->show();
        ui->applyPageBtn->show();
    }

    // Update selected device
    ui->devicePicker->setCurrentIndex(DeviceManager::getInstance().getCurrentDeviceIndex());

    // Update interface
    MainWindow::updateInterfaceForNewDevice();
}

void MainWindow::on_homePageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::Home));
}

void MainWindow::on_explorePageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::Explore));
    if (!loadedExplorePage) {
        loadedExplorePage = true;
        MainWindow::loadExplorePage();
    }
}

void MainWindow::on_locSimPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::LocSim));
}

void MainWindow::on_themesPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::Themes));
}

void MainWindow::on_statusBarPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::StatusBar));
}

void MainWindow::on_controlCenterPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::ControlCenter));
}

void MainWindow::on_springboardOptionsPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::SpringboardOptions));
}

void MainWindow::on_internalOptionsPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::InternalOptions));
}

void MainWindow::on_setupOptionsPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::SetupOptions));
}

void MainWindow::on_applyPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::Apply));
}

void MainWindow::on_refreshBtn_clicked()
{
    refreshDevices();
}

void MainWindow::on_devicePicker_activated(int index)
{
    DeviceManager::getInstance().setCurrentDeviceIndex(index);
    MainWindow::updateInterfaceForNewDevice();
}

// Home Page

void MainWindow::updatePhoneInfo()
{
    auto name = DeviceManager::getInstance().getCurrentName();
    if (name)
    {
        ui->phoneNameLbl->setText(QString::fromStdString(*name));
    }
    else
    {
        ui->phoneNameLbl->setText("No Device");
    }
    auto version = DeviceManager::getInstance().getCurrentVersion();
    if (version)
    {
        if (DeviceManager::getInstance().isDeviceAvailable())
        {
            if (*version >= Version(17)) {
                ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white;\" href=\"#\">iOS " + QString::fromStdString(version->toString()) + " <span style=\"color: #ffff00;\">Supported, YMMV.</span></a>");
            } else {
                ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white;\" href=\"#\">iOS " + QString::fromStdString(version->toString()) + " <span style=\"color: #32d74b;\">Supported!</span></a>");
            }
        }
        else
        {
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white;\" href=\"#\">iOS " + QString::fromStdString(version->toString()) + " <span style=\"color: #ff453a;\">Not Supported.</span></a>");
        }
    }
    else
    {
        ui->phoneVersionLbl->setText("Please connect a device.");
    }
}

void MainWindow::on_phoneVersionLbl_linkActivated(const QString &link)
{
    auto uuid = DeviceManager::getInstance().getCurrentUUID();
    if (uuid)
    {
        ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(*uuid) + "</a>");
    }
}

void openWebPage(const QString& url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::on_bigMilkBtn_clicked() {
    openWebPage("https://cowabun.ga");
}

void MainWindow::on_avangelistaGitHubBtn_clicked() {
    openWebPage("https://github.com/Avangelista");
}

void MainWindow::on_avangelistaTwitterBtn_clicked() {
    openWebPage("https://twitter.com/AvangelistaDev");
}

void MainWindow::on_avangelistaKoFiBtn_clicked() {
    openWebPage("https://ko-fi.com/avangelista");
}

void MainWindow::on_leminGitHubBtn_clicked() {
    openWebPage("https://github.com/leminlimez");
}

void MainWindow::on_leminTwitterBtn_clicked() {
    openWebPage("https://twitter.com/LeminLimez");
}

void MainWindow::on_leminKoFiBtn_clicked() {
    openWebPage("https://ko-fi.com/leminlimez");
}

void MainWindow::on_sourcelocBtn_clicked() {
    openWebPage("https://twitter.com/sourceloc");
}

void MainWindow::on_iTechBtn_clicked() {
    openWebPage("https://twitter.com/iTechExpert21");
}

void MainWindow::on_libiBtn_clicked() {
    openWebPage("https://libimobiledevice.org");
}

void MainWindow::on_qtBtn_clicked() {
    openWebPage("https://www.qt.io/product/development-tools");
}

void MainWindow::on_discordBtn_clicked() {
    openWebPage("https://discord.gg/Cowabunga");
}

void MainWindow::on_patreonBtn_clicked() {
    openWebPage("https://github.com/Avangelista/CowabungaLiteUniversal");
//    openWebPage("https://www.patreon.com/Cowabunga_iOS");
}

// Explore Page
void MainWindow::addThemeRow(QWidget *parent, const QString &name, const QString &previewUrl, const QString &downloadUrl) {
    QHBoxLayout *layout = new QHBoxLayout;
    QLabel *previewLabel = new QLabel;
    QToolButton *downloadButton = new QToolButton;

    QNetworkAccessManager *imageManager = new QNetworkAccessManager(this);
    QNetworkReply *imageReply = imageManager->get(QNetworkRequest(QUrl(previewUrl)));
    QEventLoop loop;
    QObject::connect(imageReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QImage image;
    image.loadFromData(imageReply->readAll());
    imageReply->deleteLater();
    if (!image.isNull()) {
        QPixmap previewImage = QPixmap::fromImage(image);
        previewLabel->setPixmap(previewImage);
    } else {
        qDebug() << "Failed to load preview image from data";
    }

    QObject::connect(downloadButton, &QToolButton::clicked, [=]() {
        QString zipFilePath = QDir::tempPath() + "/" + name + ".zip";
        QNetworkAccessManager *downloadManager = new QNetworkAccessManager(this);
        QNetworkReply *downloadReply = downloadManager->get(QNetworkRequest(QUrl(downloadUrl)));
        QObject::connect(downloadReply, &QNetworkReply::finished, [=]() {
            if (downloadReply->error() == QNetworkReply::NoError) {
                QFile zipFile(zipFilePath);
                if (zipFile.open(QIODevice::WriteOnly)) {
                    zipFile.write(downloadReply->readAll());
                    zipFile.close();
                    QString themeDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Themes/" + name;
                    Utils::unzip(zipFilePath, themeDirectoryPath, true);
                }
            }
            downloadReply->deleteLater();
            downloadManager->deleteLater();
        });
    });

    layout->addWidget(previewLabel);
    layout->addWidget(new QLabel(name));
    layout->addWidget(downloadButton);

    layout->addWidget(previewLabel);
    layout->addWidget(new QLabel(name));
    layout->addWidget(downloadButton);

    parent->layout()->addItem(layout);
}

void MainWindow::loadExplorePage() {
    ui->exploreSubLbl->setText("Loading...");
    QNetworkAccessManager manager;
    QUrl apiUrl("https://raw.githubusercontent.com/leminlimez/Cowabunga-explore-repo/main/icon-themes.json");
    QNetworkReply *reply = manager.get(QNetworkRequest(apiUrl));
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonArray themesArray = jsonDoc.array();

    QVBoxLayout *exploreThemesLayout = new QVBoxLayout; // Vertical layout for exploreThemesCnt
    exploreThemesLayout->setContentsMargins(0, 0, 0, 0);

    // Create a QWidget to hold the contents of the scroll area
    QWidget *scrollContent = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 20, 0);

    for (const QJsonValue &themeValue : themesArray) {
        QJsonObject themeObj = themeValue.toObject();
        QString name = themeObj["name"].toString();
        QString preview = themeObj["preview"].toString();
        QString url = themeObj["url"].toString();
        auto previewUrl = "https://raw.githubusercontent.com/leminlimez/Cowabunga-explore-repo/main/" + preview;
        auto themeUrl = "https://raw.githubusercontent.com/leminlimez/Cowabunga-explore-repo/main/" + url;
        QString author;
        if (themeObj.contains("contact") && themeObj["contact"].isObject()) {
            QJsonObject contactObj = themeObj["contact"].toObject();

            // Get the first value from the contact object
            if (!contactObj.isEmpty()) {
                QJsonValue firstValue = contactObj.constBegin().value();
                author = firstValue.toString();
            }
        }

        QHBoxLayout *layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(20);

        QToolButton *previewButton = new QToolButton;

        // Download and set the preview icon
        QNetworkAccessManager *imageManager = new QNetworkAccessManager(this);
        QNetworkReply *imageReply = imageManager->get(QNetworkRequest(QUrl(previewUrl)));
        QEventLoop loop;
        QObject::connect(imageReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        QImage image;
        image.loadFromData(imageReply->readAll());
        imageReply->deleteLater();
        if (!image.isNull()) {
            QPixmap previewIcon = QPixmap::fromImage(image)/*.scaled(150, 150, Qt::KeepAspectRatio)*/;
            previewButton->setIcon(QIcon(Utils::createRoundedPixmap(previewIcon, 0.1)));
            previewButton->setIconSize(QSize(150, 150));
            previewButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");
        } else {
            qDebug() << "Failed to load preview image from data";
        }

        QLabel *nameLabel = new QLabel(name);

        QLabel *authorLabel = new QLabel(author);

        QToolButton *downloadButton = new QToolButton;
        downloadButton->setText("Download"); // Set the download text
        connect(downloadButton, &QToolButton::clicked, [this, themeUrl, name, downloadButton]() {
            // Download the ZIP file
            downloadButton->setText("Downloading...");
            qDebug() << "Downloading:" << themeUrl;
            QNetworkAccessManager downloadManager;
            QNetworkReply *downloadReply = downloadManager.get(QNetworkRequest(QUrl(themeUrl)));
            QEventLoop downloadLoop;

            QObject::connect(downloadReply, &QNetworkReply::finished, &downloadLoop, &QEventLoop::quit);
            downloadLoop.exec();

            QByteArray zipData = downloadReply->readAll();
            downloadReply->deleteLater();

            // Save the ZIP file to the created directory
            QString zipFilePath = QDir::tempPath() + "/" + name + ".zip";
            QFile zipFile(zipFilePath);
            if (zipFile.open(QIODevice::WriteOnly)) {
                zipFile.write(zipData);
                zipFile.close();
                qDebug() << "ZIP file downloaded and saved to:" << zipFilePath;
                QString themeDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Themes/" + name;
                Utils::unzip(zipFilePath, themeDirectoryPath, true);
                downloadButton->setText("Done!");
                MainWindow::loadThemes();
            }
        });

        layout->addWidget(previewButton);
        layout->addWidget(nameLabel);
        layout->addWidget(authorLabel); // Add author label
        layout->addWidget(downloadButton);

        QWidget *themeWidget = new QWidget;
        themeWidget->setLayout(layout);

        scrollLayout->addWidget(themeWidget); // Add the theme widget to the scroll layout
    }

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setFrameStyle(QFrame::NoFrame); // Remove the frame/border
    scrollArea->setWidgetResizable(true); // Make the scroll area resizable
    scrollArea->setWidget(scrollContent); // Set the scroll area's content widget

    exploreThemesLayout->addWidget(scrollArea); // Add the scroll area to the main layout
    ui->exploreThemesCnt->setLayout(exploreThemesLayout);

    ui->exploreSubLbl->setText("");
}

// Loc Sim Page
void MainWindow::on_loadLocSimBtn_clicked() {
    ui->loadLocSimBtn->setText("Loading...");

    auto diskDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/DevDisks";
    auto diskDirectory = QDir(diskDirectoryPath);

    if (!diskDirectory.exists()) {
        if (!diskDirectory.mkpath(".")) {
            qDebug() << "Failed to create directory: " << diskDirectoryPath;
            ui->loadLocSimBtn->setText("Failed - Failed to create directory: " + diskDirectoryPath);
            return;
        }
    }

    auto targetVersion = DeviceManager::getInstance().getCurrentVersion();

    QNetworkAccessManager manager;
    QUrl apiUrl("https://api.github.com/repos/mspvirajpatel/Xcode_Developer_Disk_Images/releases");

    QNetworkReply *reply = manager.get(QNetworkRequest(apiUrl));
    QEventLoop loop;

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonArray releasesArray = jsonDoc.array();

    Version closestVersion;
    QString fileName;
    bool foundMatch = false;

    for (const QJsonValue &releaseValue : releasesArray) {
        QJsonObject releaseObj = releaseValue.toObject();
        QString tag_name = releaseObj["tag_name"].toString();

        QStringList tagParts = tag_name.split('.');
        if (tagParts.size() >= 2) {
            int releaseMajor = tagParts[0].toInt();
            int releaseMinor = tagParts[1].toInt();
            int releasePatch = tagParts.size() >= 3 ? tagParts[2].toInt() : 0;

            auto releaseVersion = Version(releaseMajor, releaseMinor, releasePatch);

            // Compare the versions and find the closest match
            if (!foundMatch && releaseVersion <= *targetVersion) {
                closestVersion = releaseVersion;
                foundMatch = true;
                fileName = tag_name;
            } else if (releaseVersion <= *targetVersion &&
                       releaseVersion > closestVersion) {
                closestVersion = releaseVersion;
                fileName = tag_name;
            }
        }
    }

    if (foundMatch) {
        QString versionDirectoryPath = diskDirectory.filePath(fileName);

        if (!diskDirectory.exists(versionDirectoryPath)) {
            // Download the ZIP file
            QString downloadUrl = QString("https://github.com/mspvirajpatel/Xcode_Developer_Disk_Images/releases/download/%1/%1.zip").arg(fileName);
            qDebug() << "Downloading:" << downloadUrl;
            QNetworkAccessManager downloadManager;
            QNetworkReply *downloadReply = downloadManager.get(QNetworkRequest(QUrl(downloadUrl)));
            QEventLoop downloadLoop;

            QObject::connect(downloadReply, &QNetworkReply::finished, &downloadLoop, &QEventLoop::quit);
            downloadLoop.exec();

            QByteArray zipData = downloadReply->readAll();
            downloadReply->deleteLater();

            // Save the ZIP file to the created directory
            QString zipFilePath = QDir::tempPath() + "/" + fileName + ".zip";
            QFile zipFile(zipFilePath);
            if (zipFile.open(QIODevice::WriteOnly)) {
                zipFile.write(zipData);
                zipFile.close();
                qDebug() << "ZIP file downloaded and saved to:" << zipFilePath;
                Utils::unzip(zipFilePath, diskDirectoryPath, false);
                QStringList arguments;
                arguments << "-u" << QString::fromStdString(*(DeviceManager::getInstance().getCurrentUUID())) << versionDirectoryPath + "/DeveloperDiskImage.dmg";

                QProcess process;
                process.start("ideviceimagemounter.exe", arguments);
                process.waitForFinished(-1);

                QByteArray output = process.readAllStandardOutput();
                QByteArray errorOutput = process.readAllStandardError();

                qDebug() << "Standard Output:" << output;
                qDebug() << "Error Output:" << errorOutput;
                // aaaa fix \r\n
                auto errorLines = QString::fromUtf8(output).split("\r\n");
                if (errorLines.size() > 1) {
                    auto error = errorLines.at(errorLines.size() - 2);
                    if (error == "Status: Complete" || error == "Error: ImageMountFailed") {
                        ui->loadLocSimBtn->hide();
                        ui->locSimCnt->show();
                    } else {
                        ui->loadLocSimBtn->setText("Failed - " + error);
                    }
                }
            } else {
                qDebug() << "Failed to save ZIP file:" << zipFilePath;
                ui->loadLocSimBtn->setText("Failed - Failed to save ZIP file: " + zipFilePath);
            }
        } else {
            qDebug() << "Version directory already exists. Skipping download.";
            QStringList arguments;
            arguments << "-u" << QString::fromStdString(*(DeviceManager::getInstance().getCurrentUUID())) << versionDirectoryPath + "/DeveloperDiskImage.dmg";

            QProcess process;
            process.start("ideviceimagemounter.exe", arguments);
            process.waitForFinished(-1);

            QByteArray output = process.readAllStandardOutput();
            QByteArray errorOutput = process.readAllStandardError();

            qDebug() << "Standard Output:" << output;
            qDebug() << "Error Output:" << errorOutput;
            // aaaa fix \r\n
            auto errorLines = QString::fromUtf8(output).split("\r\n");
            if (errorLines.size() > 1) {
                auto error = errorLines.at(errorLines.size() - 2);
                if (error == "Status: Complete" || error == "Error: ImageMountFailed") {
                    ui->loadLocSimBtn->hide();
                    ui->locSimCnt->show();
                } else {
                    ui->loadLocSimBtn->setText("Failed - " + error);
                }
            }
        }
    } else {
        qDebug() << "No matching release found.";
        ui->loadLocSimBtn->setText("Failed - No matching release found.");
    }
}

void MainWindow::on_setLocationBtn_clicked() {
    uint32_t mode = SET_LOCATION;

    idevice_t device = NULL;

    auto udid = DeviceManager::getInstance().getCurrentUUID();

    if (idevice_new_with_options(&device, udid->c_str(), IDEVICE_LOOKUP_USBMUX) != IDEVICE_E_SUCCESS) {
        if (udid) {
            printf("ERROR: Device %s not found!\n", udid->c_str());
        } else {
            printf("ERROR: No device found!\n");
        }
        return;
    }

    lockdownd_client_t lockdown;
    lockdownd_client_new_with_handshake(device, &lockdown, "LocSim");

    lockdownd_service_descriptor_t svc = NULL;
    lockdownd_error_t lerr = lockdownd_start_service(lockdown, DT_SIMULATELOCATION_SERVICE, &svc);
    if (lerr != LOCKDOWN_E_SUCCESS) {
        lockdownd_client_free(lockdown);
        idevice_free(device);
        printf("ERROR: Could not start the simulatelocation service: %s\nMake sure a developer disk image is mounted!\n", lockdownd_strerror(lerr));
        return;
    }
    lockdownd_client_free(lockdown);

    service_client_t service = NULL;

    service_error_t serr = service_client_new(device, svc, &service);

    lockdownd_service_descriptor_free(svc);

    if (serr != SERVICE_E_SUCCESS) {
        lockdownd_client_free(lockdown);
        idevice_free(device);
        printf("ERROR: Could not connect to simulatelocation service (%d)\n", serr);
        return;
    }

    uint32_t l;
    uint32_t s = 0;

    const char* lat = ui->latitudeTxt->text().toStdString().c_str();
    const char* lon = ui->longitudeTxt->text().toStdString().c_str();

    l = htobe32(mode);
    service_send(service, (const char*)&l, 4, &s);
    int len = 4 + strlen(lat) + 4 + strlen(lon);
    char *buf = static_cast<char*>(malloc(len));
    uint32_t latlen;
    latlen = strlen(lat);
    l = htobe32(latlen);
    memcpy(buf, &l, 4);
    memcpy(buf+4, lat, latlen);
    uint32_t longlen = strlen(lon);
    l = htobe32(longlen);
    memcpy(buf+4+latlen, &l, 4);
    memcpy(buf+4+latlen+4, lon, longlen);

    s = 0;
    service_send(service, buf, len, &s);

    free(buf);

    idevice_free(device);
}

void MainWindow::on_resetLocationBtn_clicked() {
    uint32_t mode = RESET_LOCATION;

    idevice_t device = NULL;

    auto udid = DeviceManager::getInstance().getCurrentUUID();

    if (idevice_new_with_options(&device, udid->c_str(), IDEVICE_LOOKUP_USBMUX) != IDEVICE_E_SUCCESS) {
        if (udid) {
            printf("ERROR: Device %s not found!\n", udid->c_str());
        } else {
            printf("ERROR: No device found!\n");
        }
        return;
    }

    lockdownd_client_t lockdown;
    lockdownd_client_new_with_handshake(device, &lockdown, "LocSim");

    lockdownd_service_descriptor_t svc = NULL;
    lockdownd_error_t lerr = lockdownd_start_service(lockdown, DT_SIMULATELOCATION_SERVICE, &svc);
    if (lerr != LOCKDOWN_E_SUCCESS) {
        lockdownd_client_free(lockdown);
        idevice_free(device);
        printf("ERROR: Could not start the simulatelocation service: %s\nMake sure a developer disk image is mounted!\n", lockdownd_strerror(lerr));
        return;
    }
    lockdownd_client_free(lockdown);

    service_client_t service = NULL;

    service_error_t serr = service_client_new(device, svc, &service);

    lockdownd_service_descriptor_free(svc);

    if (serr != SERVICE_E_SUCCESS) {
        lockdownd_client_free(lockdown);
        idevice_free(device);
        printf("ERROR: Could not connect to simulatelocation service (%d)\n", serr);
        return;
    }

    uint32_t l;
    uint32_t s = 0;
    l = htobe32(mode);
    service_send(service, (const char*)&l, 4, &s);
    idevice_free(device);
}

// Themes Page

void MainWindow::on_themesEnabledChk_toggled(bool checked)
{
    ui->themesPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::Themes, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_addAllBtn_clicked() {
    auto state = DeviceManager::getInstance().addAllIcons();
    MainWindow::loadIcons();
}

void MainWindow::on_hideNamesBtn_clicked() {
    auto state = DeviceManager::getInstance().hideAllNames();
    MainWindow::loadIcons();
}

void MainWindow::on_borderAllBtn_clicked() {
    auto state = DeviceManager::getInstance().borderAllIcons();
    MainWindow::loadIcons();
}

void MainWindow::on_themesBtn_clicked() {
    themesEasterEgg = !themesEasterEgg;
    MainWindow::loadIcons();
}

void MainWindow::on_importThemeFolderBtn_clicked() {
    QString selectedFile = QFileDialog::getExistingDirectory(nullptr, "Select Folder", "", QFileDialog::ShowDirsOnly);

    if (!selectedFile.isEmpty()) {
        QFileInfo fileInfo(selectedFile);
        QString folderName = fileInfo.fileName();

        auto themeDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Themes/" + folderName;
        auto themeDirectory = QDir(themeDirectoryPath);

        if (themeDirectory.exists())
        {
            themeDirectory.removeRecursively();
        } else {
            if (!themeDirectory.mkpath("."))
            {
                qDebug() << "Failed to create the directory: " << themeDirectory;
            }
            if (Utils::copyDirectory(selectedFile, themeDirectoryPath)) {
                MainWindow::loadThemes();
            } else {
                qDebug() << "Couldn't copy folder " << selectedFile;
            }
        }
    }
}

void MainWindow::on_importThemeZipBtn_clicked() {
    QString selectedFile = QFileDialog::getOpenFileName(nullptr, "Select Zip File", "", "Zip Files (*.zip)", nullptr, QFileDialog::ReadOnly);

    if (!selectedFile.isEmpty()) {
        QFileInfo fileInfo(selectedFile);
        QRegularExpression regex("(\\.zip$)", QRegularExpression::CaseInsensitiveOption);
        QString zipName = fileInfo.fileName().replace(regex, "");

        auto themeDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Themes/" + zipName;
        auto themeDirectory = QDir(themeDirectoryPath);

        if (themeDirectory.exists()) {
            themeDirectory.removeRecursively();
        } else {
            if (!themeDirectory.mkpath(".")) {
                qDebug() << "Failed to create the directory: " << themeDirectory;
            } else {
                Utils::unzip(selectedFile, themeDirectoryPath, true);
                MainWindow::loadThemes();
            }
        }
    }
}

void applyMaskToImage(QImage& image)
{
    image = image.scaled(QSize(120, 120), Qt::KeepAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);
    QImage mask(":/overlay.png");
    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            QRgb imagePixel = image.pixel(x, y);
            QRgb maskPixel = mask.pixel(x, y);
            imagePixel = qRgba(qRed(imagePixel), qGreen(imagePixel), qBlue(imagePixel), qAlpha(maskPixel));
            image.setPixel(x, y, imagePixel);
        }
    }
}

void MainWindow::loadThemesPage() {
    loadThemes();
    loadIcons();
}

void MainWindow::loadIcons() {
    int scrollPos = -1;
    QScrollArea* scrollArea = ui->iconsCnt->findChild<QScrollArea*>();
    if (scrollArea) {
        scrollPos = scrollArea->verticalScrollBar()->value();
    }

    // Clear the layout
    auto layout1 = ui->iconsCnt->layout();
    if (layout1)
    {
        QLayoutItem* child;
        while ((child = layout1->takeAt(0)) != nullptr)
        {
            delete child->widget(); // Remove and delete the widget
            delete child; // Delete the layout item
        }
        delete layout1; // Delete the layout itself
//        ui->iconsCnt->setLayout(nullptr); // Reset the layout pointer
    }

    // Clear the widget contents (if it's a container widget)
    const QObjectList& children1 = ui->iconsCnt->children();
    for (QObject* child : children1)
    {
        delete child; // Delete each child widget
    }

    // Create a QVBoxLayout to arrange the widgets vertically
    QVBoxLayout* outerLayout = new QVBoxLayout(ui->iconsCnt);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    // Create a scroll area to contain the innerLayout
    QScrollArea* scrollArea2 = new QScrollArea(ui->iconsCnt);
    scrollArea2->setWidgetResizable(true);
    scrollArea2->setFrameStyle(QFrame::NoFrame);

    // Create a QWidget to hold the innerLayout
    QWidget* scrollContentWidget = new QWidget();
    QVBoxLayout* innerLayout = new QVBoxLayout(scrollContentWidget);
    innerLayout->setContentsMargins(0, 0, 20, 0);

    for (auto bundle : DeviceManager::getInstance().getAppBundles()) {
        auto name = DeviceManager::getInstance().getAppName(bundle);
        auto themed_name = DeviceManager::getInstance().getThemedName(bundle);
        auto user_name = DeviceManager::getInstance().getUserName(bundle);
        auto icon = DeviceManager::getInstance().getIcon(bundle);
        auto themed_icon = DeviceManager::getInstance().getThemedIcon(bundle);
        auto user_icon = DeviceManager::getInstance().getUserIcon(bundle);
        auto border = DeviceManager::getInstance().getBorder(bundle);

        QCheckBox* borderCheckBox = new QCheckBox("Border", ui->iconsCnt);
        QCheckBox* addToDeviceCheckBox = new QCheckBox("Add to Device", ui->iconsCnt);
        QLineEdit* nameLineEdit = new QLineEdit(ui->iconsCnt);
        QToolButton* iconButton = new QToolButton(ui->iconsCnt);

        // Border Check Box
        borderCheckBox->setChecked(DeviceManager::getInstance().getBorder(bundle));
        connect(borderCheckBox, &QCheckBox::clicked, [this, bundle, iconButton, addToDeviceCheckBox](bool checked) {
            DeviceManager::getInstance().setBorder(bundle, checked);
            // make icon use mask
            auto icon = DeviceManager::getInstance().getIcon(bundle);
            auto themed_icon = DeviceManager::getInstance().getThemedIcon(bundle);
            auto user_icon = DeviceManager::getInstance().getUserIcon(bundle);
            if (user_icon) {
                if (checked) {
                    auto image = QImage(QString::fromStdString(*user_icon));
                    applyMaskToImage(image);
                    iconButton->setIcon(QPixmap::fromImage(image));
                } else {
                    auto userIcon = QPixmap(QString::fromStdString(*user_icon));
                    iconButton->setIcon(Utils::createRoundedPixmap(userIcon, 0.25));
                }
            } else if (themed_icon) {
                if (checked) {
                    auto image = QImage::fromData(QByteArray(themed_icon->data(), themed_icon->size()));
                    applyMaskToImage(image);
                    iconButton->setIcon(QPixmap::fromImage(image));
                } else {
                    auto image = QPixmap::fromImage(QImage::fromData(QByteArray(themed_icon->data(), themed_icon->size())));
                    iconButton->setIcon(image);
                }
            } else if (icon) {
                if (checked) {
                    auto image = QImage::fromData(QByteArray(icon->data(), icon->size()));
                    applyMaskToImage(image);
                    iconButton->setIcon(QPixmap::fromImage(image));
                } else {
                    auto image = QPixmap::fromImage(QImage::fromData(QByteArray(icon->data(), icon->size())));
                    iconButton->setIcon(image);
                }
            }
            DeviceManager::getInstance().setAddToDevice(bundle, true);
            addToDeviceCheckBox->setChecked(true);
        });

        // Add to Device Check Box
        addToDeviceCheckBox->setChecked(DeviceManager::getInstance().getAddToDevice(bundle));
        connect(addToDeviceCheckBox, &QCheckBox::clicked, [this, bundle](bool checked) {
            DeviceManager::getInstance().setAddToDevice(bundle, checked);
        });

        // Name Line Edit
        nameLineEdit->setPlaceholderText("Hidden Name (" + QString::fromStdString(name) + ")");
        if (user_name) {
            nameLineEdit->setText(QString::fromStdString(*user_name));
        } else if (themed_name) {
            nameLineEdit->setText(QString::fromStdString(*themed_name));
        } else {
            nameLineEdit->setText(QString::fromStdString(name));
        }
        connect(nameLineEdit, &QLineEdit::textEdited, [this, bundle, addToDeviceCheckBox](const QString& name) {
            DeviceManager::getInstance().setUserName(bundle, name.toStdString());
            DeviceManager::getInstance().setAddToDevice(bundle, true);
            addToDeviceCheckBox->setChecked(true);
        });

        // Icon Button
        if (user_icon) {
            if (border) {
                auto image = QImage(QString::fromStdString(*user_icon));
                applyMaskToImage(image);
                iconButton->setIcon(QPixmap::fromImage(image));
            } else {
                auto userIcon = QPixmap(QString::fromStdString(*user_icon));
                iconButton->setIcon(Utils::createRoundedPixmap(userIcon, 0.25));
            }
        } else if (themed_icon) {
            if (border) {
                auto image = QImage::fromData(QByteArray(themed_icon->data(), themed_icon->size()));
                applyMaskToImage(image);
                iconButton->setIcon(QPixmap::fromImage(image));
            } else {
                auto image = QPixmap::fromImage(QImage::fromData(QByteArray(themed_icon->data(), themed_icon->size())));
                iconButton->setIcon(image);
            }
        } else if (icon) {
            if (border) {
                auto image = QImage::fromData(QByteArray(icon->data(), icon->size()));
                applyMaskToImage(image);
                iconButton->setIcon(QPixmap::fromImage(image));
            } else {
                auto image = QPixmap::fromImage(QImage::fromData(QByteArray(icon->data(), icon->size())));
                iconButton->setIcon(image);
            }
        }
        iconButton->setIconSize(QSize(32, 32));
        iconButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");
        connect(iconButton, &QToolButton::clicked, [this, bundle, addToDeviceCheckBox, iconButton]() {
            QString initialPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Themes";
            QString filePath = QFileDialog::getOpenFileName(this, "Select File", initialPath, "App Icons (*.PNG)");

            if (!filePath.isEmpty()) {
                DeviceManager::getInstance().setUserIcon(bundle, filePath.toStdString());
                DeviceManager::getInstance().setAddToDevice(bundle, true);
                addToDeviceCheckBox->setChecked(true);
                if (DeviceManager::getInstance().getBorder(bundle)) {
                    auto image = QImage(filePath);
                    applyMaskToImage(image);
                    iconButton->setIcon(QPixmap::fromImage(image));
                } else {
                    auto image = QPixmap(filePath);
                    iconButton->setIcon(Utils::createRoundedPixmap(image, 0.25));
                }
            }
        });
        // Right click, reset all
        iconButton->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(iconButton, &QToolButton::customContextMenuRequested, [this, bundle, addToDeviceCheckBox, iconButton, themed_icon, icon, nameLineEdit, name, borderCheckBox, themed_name]() {
            DeviceManager::getInstance().resetUserPrefs(bundle);
            addToDeviceCheckBox->setChecked(false);
            borderCheckBox->setChecked(false);
            if (themed_icon) {
                QIcon themedIcon = QIcon(QPixmap::fromImage(QImage::fromData(QByteArray(themed_icon->data(), themed_icon->size()))));
                iconButton->setIcon(themedIcon);
            } else if (icon) {
                QIcon iconData = QIcon(QPixmap::fromImage(QImage::fromData(QByteArray(icon->data(), icon->size()))));
                iconButton->setIcon(iconData);
            }
            if (themed_name) {
                nameLineEdit->setText(QString::fromStdString(*themed_name));
            } else {
                nameLineEdit->setText(QString::fromStdString(name));
            }
        });

        // Create a QHBoxLayout to arrange the icon button, line edit, label, and check box horizontally
        QHBoxLayout* iconLayout = new QHBoxLayout();
        iconLayout->addWidget(iconButton);
        iconLayout->addWidget(nameLineEdit);
        iconLayout->addWidget(borderCheckBox);
        iconLayout->addWidget(addToDeviceCheckBox);

        if (themesEasterEgg) {
            borderCheckBox->show();
        } else {
            borderCheckBox->hide();
        }

        // Add the icon layout to the inner vertical layout
        innerLayout->addLayout(iconLayout);
    }

    // Set the scrollContentWidget as the widget for the scrollArea
    scrollArea2->setWidget(scrollContentWidget);

    // Add the scrollArea2 to the outerLayout
    outerLayout->addWidget(scrollArea2);

    // keep height the same
    if (scrollPos != -1) scrollArea2->verticalScrollBar()->setValue(scrollPos);

    // Set the outer layout on the ui->iconsCnt container
    ui->iconsCnt->setLayout(outerLayout);

    if (themesEasterEgg) {
        ui->borderAllBtn->show();
    } else {
        ui->borderAllBtn->hide();
    }

}

void MainWindow::loadThemes()
{
    auto themesDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Themes";
    auto themesDirectory = QDir(themesDirectoryPath);
    if (!themesDirectory.exists())
    {
        // Create the directory if it doesn't exist
        if (!themesDirectory.mkpath("."))
        {
            qDebug() << "Failed to create the directory: " << themesDirectory;
        }
    }

    // Clear the layout
    QLayout* layout = ui->themesCnt->layout();
    if (layout)
    {
        QLayoutItem* child;
        while ((child = layout->takeAt(0)) != nullptr)
        {
            delete child->widget(); // Remove and delete the widget
            delete child; // Delete the layout item
        }
        delete layout; // Delete the layout itself
//        ui->themesCnt->setLayout(nullptr); // Reset the layout pointer
    }

    // Clear the widget contents (if it's a container widget)
    const QObjectList& children = ui->themesCnt->children();
    for (QObject* child : children)
    {
        delete child; // Delete each child widget
    }

    // Create a QHBoxLayout to arrange the widgets horizontally
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Get a list of all folder names within the directory
    QStringList folderList = themesDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (folderList.isEmpty()) {
        QVBoxLayout *layout = new QVBoxLayout(ui->themesCnt);
        QLabel *label = new QLabel("No themes. Import one, or visit the Explore page.");
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        ui->themesCnt->setFixedHeight(150);
        ui->themesCnt->setLayout(layout);
        return;
    }

    // Iterate through the folder names
    foreach (const QString& folderName, folderList)
    {
        QString folderPath = themesDirectory.filePath(folderName);
        QDir directory(folderPath);
        QStringList nameFilters;
        nameFilters << "*.PNG";
        QStringList pngFiles = directory.entryList(nameFilters, QDir::Files);

        auto numIcons = pngFiles.size();

        // Create the widget containing a container for the background and a button arranged vertically
        QWidget* widget = new QWidget();

        // Create a container widget to hold the iconLayout with background
        QWidget* iconContainer = new QWidget();
        iconContainer->setStyleSheet(":enabled { background-image: url(:/background.png); background-repeat: no-repeat; background-position: center; } :disabled { background-color: gray; }");

        // Load the icon images from the folder
        QPixmap phoneIcon(QString("%1/com.apple.mobilephone-large.png").arg(folderPath));
        QPixmap safariIcon(QString("%1/com.apple.mobilesafari-large.png").arg(folderPath));
        QPixmap photosIcon(QString("%1/com.apple.mobileslideshow-large.png").arg(folderPath));
        QPixmap cameraIcon(QString("%1/com.apple.camera-large.png").arg(folderPath));

        // Check if the icons are null and replace with missing.png if necessary
        if (phoneIcon.isNull()) {
            phoneIcon = QPixmap(":/missing.png");
        }

        if (safariIcon.isNull()) {
            safariIcon = QPixmap(":/missing.png");
        }

        if (photosIcon.isNull()) {
            photosIcon = QPixmap(":/missing.png");
        }

        if (cameraIcon.isNull()) {
            cameraIcon = QPixmap(":/missing.png");
        }

        // Create QToolButtons to display the icon images
        QToolButton* phoneButton = new QToolButton(iconContainer);
        phoneButton->setIcon(QIcon(Utils::createRoundedPixmap(phoneIcon, 0.25))); // Set the radius for rounded corners
        phoneButton->setIconSize(QSize(45, 45));
        phoneButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QToolButton* safariButton = new QToolButton(iconContainer);
        safariButton->setIcon(QIcon(Utils::createRoundedPixmap(safariIcon, 0.25))); // Set the radius for rounded corners
        safariButton->setIconSize(QSize(45, 45));
        safariButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QToolButton* photosButton = new QToolButton(iconContainer);
        photosButton->setIcon(QIcon(Utils::createRoundedPixmap(photosIcon, 0.25))); // Set the radius for rounded corners
        photosButton->setIconSize(QSize(45, 45));
        photosButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QToolButton* cameraButton = new QToolButton(iconContainer);
        cameraButton->setIcon(QIcon(Utils::createRoundedPixmap(cameraIcon, 0.25))); // Set the radius for rounded corners
        cameraButton->setIconSize(QSize(45, 45));
        cameraButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QLabel* name = new QLabel(widget);
        name->setText(folderName + " • " + QString::number(numIcons) + " Icons");
        name->setAlignment(Qt::AlignCenter);

        // Create a QPushButton
        QToolButton* applyButton = new QToolButton(widget);
        applyButton->setText("Apply");
        applyButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(applyButton, &QToolButton::clicked, [this, directory]() {
            DeviceManager::getInstance().applyTheme(directory);
            MainWindow::loadIcons();
        });

        // Create a QPushButton for "Delete"
        QToolButton* deleteButton = new QToolButton(widget);
        deleteButton->setIcon(QIcon(":/icon/trash.svg")); // Set the trash.svg resource icon
        connect(deleteButton, &QToolButton::clicked, [this, directory]() {
            QDir(directory).removeRecursively();
            MainWindow::loadThemes();
        });

        // Create a QHBoxLayout to arrange the icon buttons horizontally
        QHBoxLayout* iconLayout = new QHBoxLayout(iconContainer);
        iconLayout->addWidget(phoneButton);
        iconLayout->addWidget(safariButton);
        iconLayout->addWidget(photosButton);
        iconLayout->addWidget(cameraButton);
        iconContainer->setLayout(iconLayout); // Set iconLayout as the layout for iconContainer

        // Create a QHBoxLayout to arrange the buttons horizontally
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(applyButton);
        buttonLayout->addWidget(deleteButton);

        // Create a QVBoxLayout for the widget and add the iconContainer, buttons, and name to it
        QVBoxLayout* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 9);
        layout->addWidget(iconContainer); // Add iconContainer to the main layout
        layout->addWidget(name);
        layout->addLayout(buttonLayout); // Add the buttons horizontally
        widget->setLayout(layout);

        // Add the widget to the mainLayout (inside QScrollArea)
        mainLayout->addWidget(widget);
    }

    // Create a QWidget to act as the container for the scroll area
    QWidget* scrollWidget = new QWidget();

    // Set the main layout (containing all the widgets) on the scroll widget
    scrollWidget->setLayout(mainLayout);

    // Create a QScrollArea to hold the content widget (scrollWidget)
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true); // Allow the content widget to resize within the scroll area
    scrollArea->setFrameStyle(QFrame::NoFrame); // Remove the outline from the scroll area

    // Set the scrollWidget as the content widget of the scroll area
    scrollArea->setWidget(scrollWidget);

    // Set the size policy of the scroll area to expand in both directions
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Set the scroll area as the central widget of the main window
    QVBoxLayout *scrollLayout = new QVBoxLayout();
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addWidget(scrollArea);
    ui->themesCnt->setFixedHeight(150);
    ui->themesCnt->setLayout(scrollLayout);
}

// Control Center Page
void MainWindow::loadControlCenter() {
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    auto location = QString::fromStdString(*workspace + "/ControlCenter/ManagedPreferencesDomain/mobile/com.apple.replaykit.AudioConferenceControlCenterModule.plist");
    auto value = PlistManager::getPlistValue(location, "SBIconVisibility");
    if (value)
    {
        ui->hideConferenceModulesChk->setChecked(
            !dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->hideConferenceModulesChk->setChecked(false);
    }
}

void MainWindow::on_controlCenterEnabledChk_toggled(bool checked)
{
    ui->controlCenterPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::ControlCenter, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_hideConferenceModulesChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/ControlCenter/ManagedPreferencesDomain/mobile/com.apple.replaykit.AudioConferenceControlCenterModule.plist");
    auto location1 = QString::fromStdString(*workspace + "/ControlCenter/ManagedPreferencesDomain/mobile/com.apple.replaykit.VideoConferenceControlCenterModule.plist");
    if (checked)
    {
        auto node = PList::Boolean(!checked);
        PlistManager::setPlistValue(location, "SBIconVisibility", node);
        PlistManager::setPlistValue(location1, "SBIconVisibility", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "SBIconVisibility");
        PlistManager::deletePlistKey(location1, "SBIconVisibility");
    }
}

// Status Bar Page

void MainWindow::loadStatusBar()
{
    auto sm = StatusManager::getInstance();

    // Primary Cellular
    if (sm.isCellularServiceOverridden()) {
        if (sm.getCellularServiceOverride()) {
            ui->pShowRdo->setChecked(true);
        } else {
            ui->pHideRdo->setChecked(true);
        }
    } else {
        ui->pDefaultRdo->setChecked(true);
    }
    ui->pCarrierChk->setChecked(StatusManager::getInstance().isCarrierOverridden());
    ui->pCarrierTxt->setText(QString::fromStdString(StatusManager::getInstance().getCarrierOverride()));
    ui->pBadgeChk->setChecked(StatusManager::getInstance().isPrimaryServiceBadgeOverridden());
    ui->pBadgeTxt->setText(QString::fromStdString(StatusManager::getInstance().getPrimaryServiceBadgeOverride()));
    ui->pTypeChk->setChecked(StatusManager::getInstance().isDataNetworkTypeOverridden());
    ui->pTypeDrp->setCurrentIndex(StatusManager::getInstance().getDataNetworkTypeOverride());
    ui->pStrengthChk->setChecked(StatusManager::getInstance().isGSMSignalStrengthBarsOverridden());
    auto pos = StatusManager::getInstance().getGSMSignalStrengthBarsOverride();
    ui->pStrengthSld->setValue(pos);
    ui->pStrengthLbl->setText(QString::number(pos) + (pos == 1 ? " Bar" : " Bars"));

    // Secondary Cellular
    if (sm.isSecondaryCellularServiceOverridden()) {
        if (sm.getSecondaryCellularServiceOverride()) {
            ui->sShowRdo->setChecked(true);
        } else {
            ui->sHideRdo->setChecked(true);
        }
    } else {
        ui->sDefaultRdo->setChecked(true);
    }
    ui->sCarrierChk->setChecked(StatusManager::getInstance().isSecondaryCarrierOverridden());
    ui->sCarrierTxt->setText(QString::fromStdString(StatusManager::getInstance().getSecondaryCarrierOverride()));
    ui->sBadgeChk->setChecked(StatusManager::getInstance().isSecondaryServiceBadgeOverridden());
    ui->sBadgeTxt->setText(QString::fromStdString(StatusManager::getInstance().getSecondaryServiceBadgeOverride()));
    ui->sTypeChk->setChecked(StatusManager::getInstance().isSecondaryDataNetworkTypeOverridden());
    ui->sTypeDrp->setCurrentIndex(StatusManager::getInstance().getSecondaryDataNetworkTypeOverride());
    ui->sStrengthChk->setChecked(StatusManager::getInstance().isSecondaryGSMSignalStrengthBarsOverridden());
    pos = StatusManager::getInstance().getSecondaryGSMSignalStrengthBarsOverride();
    ui->sStrengthSld->setValue(pos);
    ui->sStrengthLbl->setText(QString::number(pos) + (pos == 1 ? " Bar" : " Bars"));

    // Time etc.
    ui->timeChk->setChecked(StatusManager::getInstance().isTimeOverridden());
    ui->timeTxt->setText(QString::fromStdString(StatusManager::getInstance().getTimeOverride()));
    ui->breadcrumbChk->setChecked(StatusManager::getInstance().isCrumbOverridden());
    ui->breadcrumbTxt->setText(QString::fromStdString(StatusManager::getInstance().getCrumbOverride()));
    ui->batteryDetailChk->setChecked(StatusManager::getInstance().isBatteryDetailOverridden());
    ui->batteryDetailTxt->setText(QString::fromStdString(StatusManager::getInstance().getBatteryDetailOverride()));
    ui->batteryCapacityChk->setChecked(StatusManager::getInstance().isBatteryCapacityOverridden());
    pos = StatusManager::getInstance().getBatteryCapacityOverride();
    ui->batteryCapacitySld->setValue(pos);
    ui->batteryCapacityLbl->setText(QString::number(pos) + "%");
    ui->wifiStrengthChk->setChecked(StatusManager::getInstance().isWiFiSignalStrengthBarsOverridden());
    pos = StatusManager::getInstance().getWiFiSignalStrengthBarsOverride();
    ui->wifiStrengthSld->setValue(pos);
    ui->wifiStrengthLbl->setText(QString::number(pos) + (pos == 1 ? " Bar" : " Bars"));
    ui->numericWifiChk->setChecked(StatusManager::getInstance().isRawWiFiSignalShown());
    ui->numericCellChk->setChecked(StatusManager::getInstance().isRawGSMSignalShown());

    // Hiding
    ui->hideDNDChk->setChecked(StatusManager::getInstance().isDNDHidden());
    ui->hideAirplaneChk->setChecked(StatusManager::getInstance().isAirplaneHidden());
    ui->hideWifiChk->setChecked(StatusManager::getInstance().isWiFiHidden());
    ui->hideBatteryChk->setChecked(StatusManager::getInstance().isBatteryHidden());
    ui->hideBluetoothChk->setChecked(StatusManager::getInstance().isBluetoothHidden());
    ui->hideAlarmChk->setChecked(StatusManager::getInstance().isAlarmHidden());
    ui->hideLocationChk->setChecked(StatusManager::getInstance().isLocationHidden());
    ui->hideRotationChk->setChecked(StatusManager::getInstance().isRotationHidden());
    ui->hideAirPlayChk->setChecked(StatusManager::getInstance().isAirPlayHidden());
    ui->hideCarPlayChk->setChecked(StatusManager::getInstance().isCarPlayHidden());
    ui->hideVPNChk->setChecked(StatusManager::getInstance().isVPNHidden());
}

void MainWindow::on_statusBarEnabledChk_toggled(bool checked)
{
    ui->statusBarPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::StatusBar, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_pDefaultRdo_clicked()
{
    StatusManager::getInstance().unsetCellularService();
}

void MainWindow::on_pShowRdo_clicked()
{
    StatusManager::getInstance().setCellularService(true);
}

void MainWindow::on_pHideRdo_clicked()
{
    StatusManager::getInstance().setCellularService(false);
}

void MainWindow::on_pCarrierChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setCarrier(ui->pCarrierTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetCarrier();
    }
}

void MainWindow::on_pCarrierTxt_textEdited(const QString &text)
{
    if (ui->pCarrierChk->checkState())
    {
        StatusManager::getInstance().setCarrier(ui->pCarrierTxt->text().toStdString());
    }
}

void MainWindow::on_pBadgeChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setPrimaryServiceBadge(ui->pBadgeTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetPrimaryServiceBadge();
    }
}

void MainWindow::on_pBadgeTxt_textEdited(const QString &text)
{
    if (ui->pBadgeChk->checkState())
    {
        StatusManager::getInstance().setPrimaryServiceBadge(ui->pBadgeTxt->text().toStdString());
    }
}

void MainWindow::on_pTypeChk_clicked(bool checked) {
    if (checked)
    {
        StatusManager::getInstance().setDataNetworkType(ui->pTypeDrp->currentIndex());
    }
    else
    {
        StatusManager::getInstance().unsetDataNetworkType();
    }
}

void MainWindow::on_pTypeDrp_activated(int index) {
    if (ui->pTypeChk->checkState())
    {
        StatusManager::getInstance().setDataNetworkType(index);
    }
}

void MainWindow::on_pStrengthChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setGSMSignalStrengthBars(ui->pStrengthSld->value());
    }
    else
    {
        StatusManager::getInstance().unsetGSMSignalStrengthBars();
    }
}

void MainWindow::on_pStrengthSld_sliderMoved(int pos)
{
    ui->pStrengthLbl->setText(QString::number(pos) + (pos == 1 ? " Bar" : " Bars"));
    if (ui->pStrengthChk->checkState())
    {
        StatusManager::getInstance().setGSMSignalStrengthBars(pos);
    }
}

void MainWindow::on_sDefaultRdo_clicked()
{
    StatusManager::getInstance().unsetSecondaryCellularService();
}

void MainWindow::on_sShowRdo_clicked()
{
    StatusManager::getInstance().setSecondaryCellularService(true);
}

void MainWindow::on_sHideRdo_clicked()
{
    StatusManager::getInstance().setSecondaryCellularService(false);
}

void MainWindow::on_sCarrierChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setSecondaryCarrier(ui->sCarrierTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetSecondaryCarrier();
    }
}

void MainWindow::on_sCarrierTxt_textEdited(const QString &text)
{
    if (ui->sCarrierChk->checkState())
    {
        StatusManager::getInstance().setSecondaryCarrier(ui->sCarrierTxt->text().toStdString());
    }
}

void MainWindow::on_sBadgeChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setSecondaryServiceBadge(ui->sBadgeTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetSecondaryServiceBadge();
    }
}

void MainWindow::on_sBadgeTxt_textEdited(const QString &text)
{
    if (ui->sBadgeChk->checkState())
    {
        StatusManager::getInstance().setSecondaryServiceBadge(ui->sBadgeTxt->text().toStdString());
    }
}

void MainWindow::on_sTypeChk_clicked(bool checked) {
    if (checked)
    {
        StatusManager::getInstance().setSecondaryDataNetworkType(ui->sTypeDrp->currentIndex());
    }
    else
    {
        StatusManager::getInstance().unsetSecondaryDataNetworkType();
    }
}

void MainWindow::on_sTypeDrp_activated(int index) {
    if (ui->sTypeChk->checkState())
    {
        StatusManager::getInstance().setSecondaryDataNetworkType(index);
    }
}

void MainWindow::on_sStrengthChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setSecondaryGSMSignalStrengthBars(ui->sStrengthSld->value());
    }
    else
    {
        StatusManager::getInstance().unsetSecondaryGSMSignalStrengthBars();
    }
}

void MainWindow::on_sStrengthSld_sliderMoved(int pos)
{
    ui->sStrengthLbl->setText(QString::number(pos) + (pos == 1 ? " Bar" : " Bars"));
    if (ui->sStrengthChk->checkState())
    {
        StatusManager::getInstance().setSecondaryGSMSignalStrengthBars(pos);
    }
}

void MainWindow::on_timeChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setTime(ui->timeTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetTime();
    }
}

void MainWindow::on_timeTxt_textEdited(const QString &text)
{
    if (ui->timeChk->checkState())
    {
        StatusManager::getInstance().setTime(ui->timeTxt->text().toStdString());
    }
}

void MainWindow::on_breadcrumbChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setCrumb(ui->breadcrumbTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetCrumb();
    }
}

void MainWindow::on_breadcrumbTxt_textEdited(const QString &text)
{
    if (ui->breadcrumbChk->checkState())
    {
        StatusManager::getInstance().setCrumb(ui->breadcrumbTxt->text().toStdString());
    }
}

void MainWindow::on_batteryDetailChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setBatteryDetail(ui->batteryDetailTxt->text().toStdString());
    }
    else
    {
        StatusManager::getInstance().unsetBatteryDetail();
    }
}

void MainWindow::on_batteryDetailTxt_textEdited(const QString &text)
{
    if (ui->batteryDetailChk->checkState())
    {
        StatusManager::getInstance().setBatteryDetail(ui->batteryDetailTxt->text().toStdString());
    }
}

void MainWindow::on_batteryCapacityChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setBatteryCapacity(ui->batteryCapacitySld->value());
    }
    else
    {
        StatusManager::getInstance().unsetBatteryCapacity();
    }
}

void MainWindow::on_batteryCapacitySld_sliderMoved(int pos)
{
    ui->batteryCapacityLbl->setText(QString::number(pos) + "%");
    if (ui->batteryCapacityChk->checkState())
    {
        StatusManager::getInstance().setBatteryCapacity(pos);
    }
}

void MainWindow::on_wifiStrengthChk_clicked(bool checked)
{
    if (checked)
    {
        StatusManager::getInstance().setWiFiSignalStrengthBars(ui->wifiStrengthSld->value());
    }
    else
    {
        StatusManager::getInstance().unsetWiFiSignalStrengthBars();
    }
}

void MainWindow::on_wifiStrengthSld_sliderMoved(int pos)
{
    ui->wifiStrengthLbl->setText(QString::number(pos) + (pos == 1 ? " Bar" : " Bars"));
    if (ui->wifiStrengthChk->checkState())
    {
        StatusManager::getInstance().setWiFiSignalStrengthBars(pos);
    }
}

void MainWindow::on_numericWifiChk_clicked(bool checked)
{
    StatusManager::getInstance().showRawWiFiSignal(checked);
}

void MainWindow::on_numericCellChk_clicked(bool checked)
{
    StatusManager::getInstance().showRawGSMSignal(checked);
}

void MainWindow::on_hideDNDChk_clicked(bool checked)
{
    StatusManager::getInstance().hideDND(checked);
}

void MainWindow::on_hideAirplaneChk_clicked(bool checked)
{
    StatusManager::getInstance().hideAirplane(checked);
}

void MainWindow::on_hideWifiChk_clicked(bool checked)
{
    StatusManager::getInstance().hideWiFi(checked);
}

void MainWindow::on_hideBatteryChk_clicked(bool checked)
{
    StatusManager::getInstance().hideBattery(checked);
}

void MainWindow::on_hideBluetoothChk_clicked(bool checked)
{
    StatusManager::getInstance().hideBluetooth(checked);
}

void MainWindow::on_hideAlarmChk_clicked(bool checked)
{
    StatusManager::getInstance().hideAlarm(checked);
}

void MainWindow::on_hideLocationChk_clicked(bool checked)
{
    StatusManager::getInstance().hideLocation(checked);
}

void MainWindow::on_hideRotationChk_clicked(bool checked)
{
    StatusManager::getInstance().hideRotation(checked);
}

void MainWindow::on_hideAirPlayChk_clicked(bool checked)
{
    StatusManager::getInstance().hideAirPlay(checked);
}

void MainWindow::on_hideCarPlayChk_clicked(bool checked)
{
    StatusManager::getInstance().hideCarPlay(checked);
}

void MainWindow::on_hideVPNChk_clicked(bool checked)
{
    StatusManager::getInstance().hideVPN(checked);
}

// Springboard Options Page

void MainWindow::loadSpringboardOptions()
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.UIKit.plist");
    auto value = PlistManager::getPlistValue(location, "UIAnimationDragCoefficient");
    if (value)
    {
        double speed = dynamic_cast<PList::Real *>(value)->GetValue();
        ui->UIAnimSpeedLbl->setText(QString::number(speed) + (speed == 1 ? " (Default)" : speed > 1 ? " (Slow)"
                                                                                                    : " (Fast)"));
        ui->UIAnimSpeedSld->setValue(speed * 100);
    }
    else
    {
        ui->UIAnimSpeedLbl->setText("1 (Default)");
        ui->UIAnimSpeedSld->setValue(100);
    }
    location = QString::fromStdString(*workspace + "/SpringboardOptions/ConfigProfileDomain/Library/ConfigurationProfiles/SharedDeviceConfiguration.plist");
    value = PlistManager::getPlistValue(location, "LockScreenFootnote");
    if (value)
    {
        ui->footnoteTxt->setText(QString::fromStdString(dynamic_cast<PList::String *>(value)->GetValue()));
    }
    else
    {
        ui->footnoteTxt->setText("");
    }
    location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist");
    value = PlistManager::getPlistValue(location, "SBDontLockAfterCrash");
    if (value)
    {
        ui->disableLockRespringChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->disableLockRespringChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "SBDontDimOrLockOnAC");
    if (value)
    {
        ui->disableDimmingChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->disableDimmingChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "SBHideLowPowerAlerts");
    if (value)
    {
        ui->disableBatteryAlertsChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->disableBatteryAlertsChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "SBNeverBreadcrumb");
    if (value)
    {
        ui->disableCrumbChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->disableCrumbChk->setChecked(false);
    }
    // value = PlistManager::getPlistValue(location, "SBControlCenterEnabledInLockScreen");
    // if (value)
    // {
    //     ui->enableLSCCChk->setChecked(
    //         dynamic_cast<PList::Boolean *>(value)->GetValue());
    // }
    // else
    // {
    //     ui->enableLSCCChk->setChecked(false);
    // }
    location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.Accessibility.plist");
    value = PlistManager::getPlistValue(location, "StartupSoundEnabled");
    if (value)
    {
        ui->enableShutdownSoundChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->enableShutdownSoundChk->setChecked(false);
    }
    location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.sharingd.plist");
    value = PlistManager::getPlistValue(location, "DiscoverableMode");
    if (value)
    {
        ui->allowAirDropEveryoneChk->setChecked(
            dynamic_cast<PList::String *>(value)->GetValue() == "Everyone");
    }
    else
    {
        ui->allowAirDropEveryoneChk->setChecked(false);
    }
}

void MainWindow::on_springboardOptionsEnabledChk_toggled(bool checked)
{
    ui->springboardOptionsPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::SpringboardOptions, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_footnoteTxt_textEdited(const QString &text)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ConfigProfileDomain/Library/ConfigurationProfiles/SharedDeviceConfiguration.plist");
    if (!text.isEmpty())
    {
        auto node = PList::String(text.toStdString());
        PlistManager::setPlistValue(location, "LockScreenFootnote", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "LockScreenFootnote");
    }
}

void MainWindow::on_UIAnimSpeedSld_sliderMoved(int pos)
{
    double speed = pos / 100.0;
    ui->UIAnimSpeedLbl->setText(QString::number(speed) + (speed == 1 ? " (Default)" : speed > 1 ? " (Slow)"
                                                                                                : " (Fast)"));
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.UIKit.plist");
    if (speed != 1)
    {
        auto node = PList::Real(speed);
        PlistManager::setPlistValue(location, "UIAnimationDragCoefficient", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "UIAnimationDragCoefficient");
    }
}

void MainWindow::on_disableLockRespringChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "SBDontLockAfterCrash", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "SBDontLockAfterCrash");
    }
}

void MainWindow::on_disableDimmingChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "SBDontDimOrLockOnAC", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "SBDontDimOrLockOnAC");
    }
}

void MainWindow::on_disableBatteryAlertsChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "SBHideLowPowerAlerts", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "SBHideLowPowerAlerts");
    }
}

void MainWindow::on_disableCrumbChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "SBNeverBreadcrumb", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "SBNeverBreadcrumb");
    }
}

// void MainWindow::on_enableLSCCChk_clicked(bool checked)
// {
//     auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
//     if (!workspace)
//         return;
//     auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist");
//     if (checked)
//     {
//         auto node = PList::Boolean(checked);
//         PlistManager::setPlistValue(location, "SBControlCenterEnabledInLockScreen", node);
//     }
//     else
//     {
//         PlistManager::deletePlistKey(location, "SBControlCenterEnabledInLockScreen");
//     }
// }

void MainWindow::on_enableShutdownSoundChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.Accessibility.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "StartupSoundEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "StartupSoundEnabled");
    }
}

void MainWindow::on_allowAirDropEveryoneChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.sharingd.plist");
    if (checked)
    {
        auto node = PList::String("Everyone");
        PlistManager::setPlistValue(location, "DiscoverableMode", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "DiscoverableMode");
    }
}

// Setup Options Page

void MainWindow::loadSetupOptions()
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    auto location = QString::fromStdString(*workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist");
    auto value = PlistManager::getPlistValue(location, "SkipSetup");
    if (value)
    {
        ui->skipSetupChk->setChecked(
            dynamic_cast<PList::Array *>(value)->GetSize() != 0);
    }
    else
    {
        ui->skipSetupChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "IsSupervised");
    if (value)
    {
        ui->enableSupervisionChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->enableSupervisionChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "OrganizationName");
    if (value)
    {
        ui->organizationNameTxt->setText(QString::fromStdString(dynamic_cast<PList::String *>(value)->GetValue()));
    }
    else
    {
        ui->organizationNameTxt->setText("");
    }
    location = QString::fromStdString(*workspace + "/SkipSetup/ManagedPreferencesDomain/mobile/com.apple.MobileAsset.plist");
    value = PlistManager::getPlistValue(location, "MobileAssetAssetAudience");
    if (value)
    {
        ui->disableUpdatesChk->setChecked(true);
    }
    else
    {
        ui->disableUpdatesChk->setChecked(false);
    }
}

void MainWindow::on_setupOptionsEnabledChk_toggled(bool checked)
{
    ui->setupOptionsPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::SkipSetup, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_skipSetupChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist");
    if (checked)
    {
        // auto node1 = PList::Boolean(checked);
        // PlistManager::setPlistValue(location, "CloudConfigurationUIComplete", node1);

        auto strings = {
            "Location",
                "Restore",
                "SIMSetup",
                "Android",
                "AppleID",
                "IntendedUser",
                "TOS",
                "Siri",
                "ScreenTime",
                "Diagnostics",
                "SoftwareUpdate",
                "Passcode",
                "Biometric",
                "Payment",
                "Zoom",
                "DisplayTone",
                "MessagingActivationUsingPhoneNumber",
                "HomeButtonSensitivity",
                "CloudStorage",
                "ScreenSaver",
                "TapToSetup",
                "Keyboard",
                "PreferredLanguage",
                "SpokenLanguage",
                "WatchMigration",
                "OnBoarding",
                "TVProviderSignIn",
                "TVHomeScreenSync",
                "Privacy",
                "TVRoom",
                "iMessageAndFaceTime",
                "AppStore",
                "Safety",
                "Multitasking",
                "ActionButton",
                "TermsOfAddress",
                "AccessibilityAppearance",
                "Welcome",
                "Appearance",
                "RestoreCompleted",
                "UpdateCompleted"};

        auto node2 = PList::Array();
        for (const auto &str : strings)
        {
            node2.Append(new PList::String(str));
        }

        PlistManager::setPlistValue(location, "SkipSetup", node2);
    }
    else
    {
        // PlistManager::deletePlistKey(location, "CloudConfigurationUIComplete");
        PlistManager::deletePlistKey(location, "SkipSetup");
    }
}

void MainWindow::on_disableUpdatesChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SkipSetup/ManagedPreferencesDomain/mobile/com.apple.MobileAsset.plist");
    if (checked)
    {
        auto node = PList::String("");
        PlistManager::setPlistValue(location, "MobileAssetAssetAudience", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "MobileAssetSUAllowOSVersionChange");
        PlistManager::deletePlistKey(location, "MobileAssetSUAllowSameVersionFullReplacement");
        PlistManager::deletePlistKey(location, "MobileAssetServerURL-com.apple.MobileAsset.MobileSoftwareUpdate.UpdateBrain");
        PlistManager::deletePlistKey(location, "MobileAssetServerURL-com.apple.MobileAsset.SoftwareUpdate");
        PlistManager::deletePlistKey(location, "MobileAssetServerURL-com.apple.MobileAsset.RecoveryOSUpdateBrain");
        PlistManager::deletePlistKey(location, "MobileAssetServerURL-com.apple.MobileAsset.RecoveryOSUpdate");
        PlistManager::deletePlistKey(location, "MobileAssetAssetAudience");
    }
}

void MainWindow::on_enableSupervisionChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "IsSupervised", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "IsSupervised");
    }
}

void MainWindow::on_organizationNameTxt_textEdited(const QString &text)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist");
    if (!text.isEmpty())
    {
        auto node = PList::String(text.toStdString());
        PlistManager::setPlistValue(location, "OrganizationName", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "OrganizationName");
    }
}

// Internal Options Page

void MainWindow::loadInternalOptions()
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    auto value = PlistManager::getPlistValue(location, "UIStatusBarShowBuildVersion");
    if (value)
    {
        ui->buildVersionChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->buildVersionChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "NSForceRightToLeftWritingDirection");
    if (value)
    {
        ui->RTLChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->RTLChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "MetalForceHudEnabled");
    if (value)
    {
        ui->metalHUDChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->metalHUDChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "AccessoryDeveloperEnabled");
    if (value)
    {
        ui->accessoryChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->accessoryChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "iMessageDiagnosticsEnabled");
    if (value)
    {
        ui->iMessageChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->iMessageChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "IDSDiagnosticsEnabled");
    if (value)
    {
        ui->IDSChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->IDSChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "VCDiagnosticsEnabled");
    if (value)
    {
        ui->VCChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->VCChk->setChecked(false);
    }
    location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.AppStore.plist");
    value = PlistManager::getPlistValue(location, "debugGestureEnabled");
    if (value)
    {
        ui->appStoreChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->appStoreChk->setChecked(false);
    }
    location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.mobilenotes.plist");
    value = PlistManager::getPlistValue(location, "DebugModeEnabled");
    if (value)
    {
        ui->notesChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->notesChk->setChecked(false);
    }
    location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.backboardd.plist");
    value = PlistManager::getPlistValue(location, "BKDigitizerVisualizeTouches");
    if (value)
    {
        ui->showTouchesChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->showTouchesChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "BKHideAppleLogoOnLaunch");
    if (value)
    {
        ui->hideRespringChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->hideRespringChk->setChecked(false);
    }
    location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.CoreMotion.plist");
    value = PlistManager::getPlistValue(location, "EnableWakeGestureHaptic");
    if (value)
    {
        ui->enableWakeVibrateChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->enableWakeVibrateChk->setChecked(false);
    }
    location = QString::fromStdString(*workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.Pasteboard.plist");
    value = PlistManager::getPlistValue(location, "PlaySoundOnPaste");
    if (value)
    {
        ui->pasteSoundChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->pasteSoundChk->setChecked(false);
    }
    value = PlistManager::getPlistValue(location, "AnnounceAllPastes");
    if (value)
    {
        ui->notifyPastesChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->notifyPastesChk->setChecked(false);
    }
}

void MainWindow::on_internalOptionsEnabledChk_toggled(bool checked)
{
    ui->internalOptionsPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::InternalOptions, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_buildVersionChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "UIStatusBarShowBuildVersion", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "UIStatusBarShowBuildVersion");
    }
}

void MainWindow::on_RTLChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "NSForceRightToLeftWritingDirection", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "NSForceRightToLeftWritingDirection");
    }
}

void MainWindow::on_metalHUDChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "MetalForceHudEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "MetalForceHudEnabled");
    }
}

void MainWindow::on_accessoryChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "AccessoryDeveloperEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "AccessoryDeveloperEnabled");
    }
}

void MainWindow::on_iMessageChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "iMessageDiagnosticsEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "iMessageDiagnosticsEnabled");
    }
}

void MainWindow::on_IDSChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "IDSDiagnosticsEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "IDSDiagnosticsEnabled");
    }
}

void MainWindow::on_VCChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "VCDiagnosticsEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "VCDiagnosticsEnabled");
    }
}

void MainWindow::on_appStoreChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.AppStore.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "debugGestureEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "debugGestureEnabled");
    }
}

void MainWindow::on_notesChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.mobilenotes.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "DebugModeEnabled", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "DebugModeEnabled");
    }
}

void MainWindow::on_showTouchesChk_clicked(bool checked) {
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.backboardd.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "BKDigitizerVisualizeTouches", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "BKDigitizerVisualizeTouches");
    }
}

void MainWindow::on_hideRespringChk_clicked(bool checked) {
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.backboardd.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "BKHideAppleLogoOnLaunch", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "BKHideAppleLogoOnLaunch");
    }
}

void MainWindow::on_enableWakeVibrateChk_clicked(bool checked) {
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/com.apple.CoreMotion.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "EnableWakeGestureHaptic", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "EnableWakeGestureHaptic");
    }
}

void MainWindow::on_pasteSoundChk_clicked(bool checked) {
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.Pasteboard.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "PlaySoundOnPaste", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "PlaySoundOnPaste");
    }
}

void MainWindow::on_notifyPastesChk_clicked(bool checked) {
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = QString::fromStdString(*workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.Pasteboard.plist");
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "AnnounceAllPastes", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "AnnounceAllPastes");
    }
}

// Apply Page

void MainWindow::updateEnabledTweaks()
{
    auto labelText = std::string();
    auto tweaks = DeviceManager::getInstance().getEnabledTweaks();
    if (tweaks.empty())
    {
        labelText = "None";
    }
    else
    {
        std::ostringstream labelTextStream;
        bool firstTweak = true;
        for (auto t : tweaks) {
            if (!firstTweak) {
                labelTextStream << ", ";
            } else {
                firstTweak = false;
            }
            labelTextStream << Tweaks::getTweakData(t).description;
        }
        labelText = labelTextStream.str();
    }
    ui->enabledTweaksLbl->setText(QString::fromStdString(labelText));

    ui->themesEnabledChk->setChecked(std::find(tweaks.begin(), tweaks.end(), Tweak::Themes) != tweaks.end());
    ui->statusBarEnabledChk->setChecked(std::find(tweaks.begin(), tweaks.end(), Tweak::StatusBar) != tweaks.end());
    ui->springboardOptionsEnabledChk->setChecked(std::find(tweaks.begin(), tweaks.end(), Tweak::SpringboardOptions) != tweaks.end());
    ui->internalOptionsEnabledChk->setChecked(std::find(tweaks.begin(), tweaks.end(), Tweak::InternalOptions) != tweaks.end());
    ui->setupOptionsEnabledChk->setChecked(std::find(tweaks.begin(), tweaks.end(), Tweak::SkipSetup) != tweaks.end());
}

void MainWindow::on_applyTweaksBtn_clicked()
{
    DeviceManager::getInstance().applyTweaks(ui->statusLbl);
}

void MainWindow::on_removeTweaksBtn_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Sure?", "This will remove all tweaks previously added with Cowabunga Lite (except themed app icons, you'll need to remove them manually).\n\nAre you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        DeviceManager::getInstance().removeTweaks(ui->statusLbl, false);
    }
}

void MainWindow::on_deepCleanBtn_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Sure?", "This will remove all tweaks previously added with Cowabunga Lite (except themed app icons, you'll need to remove them manually).\n\nIt will also remove any tweaks that may have been set with older versions.\n\nAre you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        DeviceManager::getInstance().removeTweaks(ui->statusLbl, true);
    }
}

// Window

void MainWindow::on_titleBar_pressed()
{
    windowHandle()->startSystemMove();
}

void MainWindow::on_closeBtn_clicked()
{
    close();
}
