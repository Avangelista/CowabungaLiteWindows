#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DeviceManager.h"
#include "statusmanager/StatusManager.h"
#include "plistmanager.h"
#include <QDebug>

// Boilerplate

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CowabungaLite)
{
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
        // Load status bar overrides
        MainWindow::loadStatusBar();
        MainWindow::loadSpringboardOptions();
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
        DeviceManager::getInstance().resetCurrentDevice();
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

void MainWindow::on_statusBarPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::StatusBar));
}

void MainWindow::on_springboardOptionsPageBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::SpringboardOptions));
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
        ui->phoneNameLbl->setText("None");
    }
    auto version = DeviceManager::getInstance().getCurrentVersion();
    if (version)
    {
        if (DeviceManager::getInstance().isDeviceAvailable())
        {
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white;\" href=\"#\">" + QString::fromStdString(version->toString()) + " <span style=\"color: #32d74b;\">Supported!</span></a>");
        }
        else
        {
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white;\" href=\"#\">" + QString::fromStdString(version->toString()) + " <span style=\"color: #ff453a;\">Not Supported.</span></a>");
        }
    }
    else
    {
        ui->phoneVersionLbl->setText("None");
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

// Status Bar Page

void MainWindow::loadStatusBar()
{
    ui->pCarrierChk->setCheckState(Qt::CheckState(StatusManager::getInstance().isCarrierOverridden() ? Qt::Checked : Qt::Unchecked));
    ui->pCarrierTxt->setText(QString::fromStdString(StatusManager::getInstance().getCarrierOverride()));
    ui->hideBatteryChk->setCheckState(Qt::CheckState(StatusManager::getInstance().isBatteryHidden() ? Qt::Checked : Qt::Unchecked));
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

void MainWindow::on_hideCellChk_clicked(bool checked)
{
    StatusManager::getInstance().hideCell(checked);
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
    if (!workspace)
        return;
    // fix this

    auto location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.UIKit.plist";
    auto value = PlistManager::getPlistValue(location, "UIAnimationDragCoefficient");
    double speed = dynamic_cast<PList::Real *>(value)->GetValue();
    ui->UIAnimSpeedLbl->setText(QString::number(speed) + (speed == 1 ? " (Default)" : speed > 1 ? " (Slow)"
                                                                                                : " (Fast)"));
    ui->UIAnimSpeedSld->setValue(speed * 100);
    location = *workspace + "/SpringboardOptions/ConfigProfileDomain/Library/ConfigurationProfiles/SharedDeviceConfiguration.plist";
    value = PlistManager::getPlistValue(location, "LockScreenFootnote");
    ui->footnoteTxt->setText(QString::fromStdString(dynamic_cast<PList::String *>(value)->GetValue()));
    location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
    value = PlistManager::getPlistValue(location, "SBDontLockAfterCrash");
    ui->disableLockRespringChk->setCheckState(
        Qt::CheckState(dynamic_cast<PList::Boolean *>(value)->GetValue() ? Qt::Checked : Qt::Unchecked));
    value = PlistManager::getPlistValue(location, "SBDontDimOrLockOnAC");
    ui->disableDimmingChk->setCheckState(
        Qt::CheckState(dynamic_cast<PList::Boolean *>(value)->GetValue() ? Qt::Checked : Qt::Unchecked));
    value = PlistManager::getPlistValue(location, "SBHideLowPowerAlerts");
    ui->disableBatteryAlertsChk->setCheckState(
        Qt::CheckState(dynamic_cast<PList::Boolean *>(value)->GetValue() ? Qt::Checked : Qt::Unchecked));
    value = PlistManager::getPlistValue(location, "SBControlCenterEnabledInLockScreen");
    ui->enableLSCCChk->setCheckState(
        Qt::CheckState(dynamic_cast<PList::Boolean *>(value)->GetValue() ? Qt::Checked : Qt::Unchecked));
    location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.Accessibility.plist";
    value = PlistManager::getPlistValue(location, "StartupSoundEnabled");
    ui->enableShutdownSoundChk->setCheckState(
        Qt::CheckState(dynamic_cast<PList::Boolean *>(value)->GetValue() ? Qt::Checked : Qt::Unchecked));
    location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.sharingd.plist";
    value = PlistManager::getPlistValue(location, "DiscoverableMode");
    ui->allowAirDropEveryoneChk->setCheckState(
        Qt::CheckState(dynamic_cast<PList::Boolean *>(value)->GetValue() ? Qt::Checked : Qt::Unchecked));
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
    auto node = PList::String(text.toStdString());
    PlistManager::setPlistValue(location, "LockScreenFootnote", node);
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
    auto node = PList::Real(speed);
    PlistManager::setPlistValue(location, "UIAnimationDragCoefficient", node);
}

void MainWindow::on_disableLockRespringChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
    auto node = PList::Boolean(checked);
    PlistManager::setPlistValue(location, "SBDontLockAfterCrash", node);
}

void MainWindow::on_disableDimmingChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
    auto node = PList::Boolean(checked);
    PlistManager::setPlistValue(location, "SBDontDimOrLockOnAC", node);
}

void MainWindow::on_disableBatteryAlertsChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
    auto node = PList::Boolean(checked);
    PlistManager::setPlistValue(location, "SBHideLowPowerAlerts", node);
}

void MainWindow::on_enableLSCCChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.springboard.plist";
    auto node = PList::Boolean(checked);
    PlistManager::setPlistValue(location, "SBControlCenterEnabledInLockScreen", node);
}

void MainWindow::on_enableShutdownSoundChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/HomeDomain/Library/Preferences/com.apple.Accessibility.plist";
    auto node = PList::Boolean(checked);
    PlistManager::setPlistValue(location, "StartupSoundEnabled", node);
}

void MainWindow::on_allowAirDropEveryoneChk_clicked(bool checked)
{
    auto workspace = DeviceManager::getInstance().getCurrentWorkspace();
    if (!workspace)
        return;
    auto location = *workspace + "/SpringboardOptions/ManagedPreferencesDomain/mobile/com.apple.sharingd.plist";
    auto node = PList::Boolean(checked);
    PlistManager::setPlistValue(location, "DiscoverableMode", node);
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

void MainWindow::on_phoneVersionLbl_linkActivated(const QString &link)
{
    auto uuid = DeviceManager::getInstance().getCurrentUUID();
    if (uuid)
    {
        ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(*uuid) + "</a>");
    }
}
