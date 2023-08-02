#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DeviceManager.h"
#include "qdir.h"
#include "qstandardpaths.h"
#include "statusmanager/StatusManager.h"
#include "plistmanager.h"
#include <QDebug>
#include <QWindow>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QScrollBar>

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
        MainWindow::loadThemes();
        MainWindow::loadStatusBar();
        MainWindow::loadSpringboardOptions();
        MainWindow::loadInternalOptions();
        MainWindow::loadSetupOptions();
    }
    else
    {
        // reset all options and disable the pages
    }
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
        // shouldn't need this
        // DeviceManager::getInstance().resetCurrentDevice();
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
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white;\" href=\"#\">iOS " + QString::fromStdString(version->toString()) + " <span style=\"color: #32d74b;\">Supported!</span></a>");
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

void MainWindow::on_toolButton_3_clicked()
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.UIKit.plist";
    auto value = PlistManager::getPlistValue(location, "UIAnimationDragCoefficient");
    qDebug() << "Value: " << dynamic_cast<PList::Real *>(value)->GetValue();
}

// Themes Page

void MainWindow::on_themesEnabledChk_toggled(bool checked)
{
    ui->themesPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::Themes, checked);
    MainWindow::updateEnabledTweaks();
}

void applyMaskToImage(QImage& image)
{
    image = image.scaled(QSize(120, 120), Qt::KeepAspectRatio, Qt::SmoothTransformation);
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


// Helper function to create rounded pixmap
QPixmap MainWindow::createRoundedPixmap(const QPixmap& pixmap, double roundnessPercentage)
{
    QPixmap roundedPixmap(pixmap.size());
    roundedPixmap.fill(Qt::transparent);

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int width = pixmap.width();
    int height = pixmap.height();
    int radius = qMin(width, height) * roundnessPercentage;

    QPainterPath path;
    path.addRoundedRect(roundedPixmap.rect(), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pixmap);

    return roundedPixmap;
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
    innerLayout->setContentsMargins(0, 0, 0, 0);

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
                    iconButton->setIcon(createRoundedPixmap(userIcon, 0.25));
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
                iconButton->setIcon(createRoundedPixmap(userIcon, 0.25));
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
                    iconButton->setIcon(createRoundedPixmap(image, 0.25));
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

    // Get a list of all folder names within the directory
    QStringList folderList = themesDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

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
        QPixmap phoneIcon(QString("%1/%2/com.apple.mobilephone-large.png").arg(themesDirectoryPath, folderName));
        QPixmap safariIcon(QString("%1/%2/com.apple.mobilesafari-large.png").arg(themesDirectoryPath, folderName));
        QPixmap photosIcon(QString("%1/%2/com.apple.mobileslideshow-large.png").arg(themesDirectoryPath, folderName));
        QPixmap cameraIcon(QString("%1/%2/com.apple.camera-large.png").arg(themesDirectoryPath, folderName));

        // Create QToolButtons to display the icon images
        QToolButton* phoneButton = new QToolButton(iconContainer);
        phoneButton->setIcon(QIcon(createRoundedPixmap(phoneIcon, 0.25))); // Set the radius for rounded corners
        phoneButton->setIconSize(QSize(45, 45));
        phoneButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QToolButton* safariButton = new QToolButton(iconContainer);
        safariButton->setIcon(QIcon(createRoundedPixmap(safariIcon, 0.25))); // Set the radius for rounded corners
        safariButton->setIconSize(QSize(45, 45));
        safariButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QToolButton* photosButton = new QToolButton(iconContainer);
        photosButton->setIcon(QIcon(createRoundedPixmap(photosIcon, 0.25))); // Set the radius for rounded corners
        photosButton->setIconSize(QSize(45, 45));
        photosButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QToolButton* cameraButton = new QToolButton(iconContainer);
        cameraButton->setIcon(QIcon(createRoundedPixmap(cameraIcon, 0.25))); // Set the radius for rounded corners
        cameraButton->setIconSize(QSize(45, 45));
        cameraButton->setStyleSheet("QToolButton { min-height: 0px; background: none; padding: 0px; border: none; }");

        QLabel* name = new QLabel(widget);
        name->setText(folderName + " • " + QString::number(numIcons) + " Icons");
        name->setAlignment(Qt::AlignCenter);

        // Create a QPushButton
        QToolButton* button = new QToolButton(widget);
        button->setText("Apply");
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        // Create a QHBoxLayout to arrange the icon buttons horizontally
        QHBoxLayout* iconLayout = new QHBoxLayout(iconContainer);
        iconLayout->addWidget(phoneButton);
        iconLayout->addWidget(safariButton);
        iconLayout->addWidget(photosButton);
        iconLayout->addWidget(cameraButton);
        iconContainer->setLayout(iconLayout); // Set iconLayout as the layout for iconContainer

        // Create a QVBoxLayout for the widget and add the iconContainer and button to it
        QVBoxLayout* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 9);
        layout->addWidget(iconContainer); // Add iconContainer to the main layout
        layout->addWidget(name);
        layout->addWidget(button);
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

    MainWindow::loadIcons();
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
    // data network type
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
    // data network type
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
    auto location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.UIKit.plist";
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
    location = *workspace + "/SpringboardOptions/ConfigProfileDomain/Library/ConfigurationProfiles/SharedDeviceConfiguration.plist";
    value = PlistManager::getPlistValue(location, "LockScreenFootnote");
    if (value)
    {
        ui->footnoteTxt->setText(QString::fromStdString(dynamic_cast<PList::String *>(value)->GetValue()));
    }
    else
    {
        ui->footnoteTxt->setText("");
    }
    location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
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
    value = PlistManager::getPlistValue(location, "SBControlCenterEnabledInLockScreen");
    if (value)
    {
        ui->enableLSCCChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
    }
    else
    {
        ui->enableLSCCChk->setChecked(false);
    }
    location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.Accessibility.plist";
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
    location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.sharingd.plist";
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
    auto location = *workspace + "/SpringboardOptions/ConfigProfileDomain/Library/ConfigurationProfiles/SharedDeviceConfiguration.plist";
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
    auto location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.UIKit.plist";
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
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
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
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
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
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
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

void MainWindow::on_enableLSCCChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
    if (checked)
    {
        auto node = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "SBControlCenterEnabledInLockScreen", node);
    }
    else
    {
        PlistManager::deletePlistKey(location, "SBControlCenterEnabledInLockScreen");
    }
}

void MainWindow::on_enableShutdownSoundChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.Accessibility.plist";
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
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.sharingd.plist";
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
    auto location = *workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist";
    auto value = PlistManager::getPlistValue(location, "CloudConfigurationUIComplete");
    if (value)
    {
        ui->skipSetupChk->setChecked(
            dynamic_cast<PList::Boolean *>(value)->GetValue());
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
    location = *workspace + "/SkipSetup/ManagedPreferencesDomain/mobile/com.apple.MobileAsset.plist";
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
    auto location = *workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist";
    if (checked)
    {
        auto node1 = PList::Boolean(checked);
        PlistManager::setPlistValue(location, "CloudConfigurationUIComplete", node1);

        auto strings = {
            "Diagnostics",
            "WiFi",
            "AppleID",
            "Siri",
            "Restore",
            "SoftwareUpdate",
            "Welcome",
            "Appearance",
            "Privacy",
            "SIMSetup",
            "OnBoarding",
            "Zoom",
            "Biometric",
            "ScreenTime",
            "Payment",
            "Passcode",
            "Display"};

        auto node2 = PList::Array();
        for (const auto &str : strings)
        {
            node2.Append(new PList::String(str));
        }

        PlistManager::setPlistValue(location, "SkipSetup", node2);
    }
    else
    {
        PlistManager::deletePlistKey(location, "CloudConfigurationUIComplete");
        PlistManager::deletePlistKey(location, "SkipSetup");
    }
}

void MainWindow::on_disableUpdatesChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SkipSetup/ManagedPreferencesDomain/mobile/com.apple.MobileAsset.plist";
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
    auto location = *workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist";
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
    auto location = *workspace + "/SkipSetup/ConfigProfileDomain/Library/ConfigurationProfiles/CloudConfigurationDetails.plist";
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
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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
    location = *workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.AppStore.plist";
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
    location = *workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.mobilenotes.plist";
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
}

void MainWindow::on_internalOptionsEnabledChk_toggled(bool checked)
{
    ui->internalOptionsPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::InternalOptions, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_buildVersionChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_RTLChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_metalHUDChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_accessoryChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_iMessageChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_IDSChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_VCChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/ManagedPreferencesDomain/mobile/hiddendotGlobalPreferences.plist";
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

void MainWindow::on_appStoreChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.AppStore.plist";
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

void MainWindow::on_notesChk_toggled(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/InternalOptions/HomeDomain/Library/Preferences/com.apple.mobilenotes.plist";
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
        for (auto t : tweaks)
        {
            labelText += "  • " + Tweaks::getTweakData(t).description + "\n";
        }
    }
    ui->enabledTweaksLbl->setText(QString::fromStdString(labelText));
}

void MainWindow::on_applyTweaksBtn_clicked()
{
    DeviceManager::getInstance().applyTweaks();
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
