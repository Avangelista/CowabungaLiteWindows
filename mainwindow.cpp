#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DeviceManager.h"
#include "statusmanager/StatusManager.h"
#include <QDebug>

// Boilerplate

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CowabungaLite)
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

void MainWindow::updateInterfaceForNewDevice() {
    // Update names
    MainWindow::updatePhoneInfo();

    if (DeviceManager::getInstance().isDeviceAvailable()) {
        // Load status bar overrides
        MainWindow::loadStatusBar();
    } else {
        // reset all options and disable the pages
    }
}

// Sidebar

void MainWindow::refreshDevices() {
    // Clear existing items in the combobox
    ui->devicePicker->clear();

    // Load devices
    auto devices = DeviceManager::getInstance().loadDevices();

    if (devices.empty()) {
        ui->devicePicker->setEnabled(false);
        ui->devicePicker->addItem(QString("None"), QVariant::fromValue(NULL));
        DeviceManager::getInstance().resetCurrentDevice();
    } else {
        ui->devicePicker->setEnabled(true);
        // Populate the combobox with device names
        for (auto& device : devices) {
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

void MainWindow::updatePhoneInfo() {
    auto name = DeviceManager::getInstance().getCurrentName();
    if (name) {
        ui->phoneNameLbl->setText(QString::fromStdString(*name));
    } else {
        ui->phoneNameLbl->setText("None");
    }
    auto version = DeviceManager::getInstance().getCurrentVersion();
    if (version) {
        if (DeviceManager::getInstance().isDeviceAvailable()) {
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(version->toString()) + " (supported) </a>");
        } else {
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(version->toString()) + " (not supported) </a>");
        }

    } else {
        ui->phoneVersionLbl->setText("None");
    }
}

void MainWindow::on_toolButton_3_clicked()
{
    qDebug() << "Clicked";
}

// Status Bar Page

void MainWindow::loadStatusBar() {
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
    if (checked) {
        StatusManager::getInstance().setCarrier(ui->pCarrierTxt->text().toStdString());
    } else {
        StatusManager::getInstance().unsetCarrier();
    }

}

void MainWindow::on_pCarrierTxt_textEdited(const QString &text)
{
    if (ui->pCarrierChk->checkState()) {
        StatusManager::getInstance().setCarrier(ui->pCarrierTxt->text().toStdString());
    }
}

void MainWindow::on_pBadgeChk_clicked(bool checked)
{
    if (checked) {
        StatusManager::getInstance().setPrimaryServiceBadge(ui->pBadgeTxt->text().toStdString());
    } else {
        StatusManager::getInstance().unsetPrimaryServiceBadge();
    }

}

void MainWindow::on_pBadgeTxt_textEdited(const QString &text)
{
    if (ui->pBadgeChk->checkState()) {
        StatusManager::getInstance().setPrimaryServiceBadge(ui->pBadgeTxt->text().toStdString());
    }
}

void MainWindow::on_pStrengthChk_clicked(bool checked)
{
    if (checked) {
        StatusManager::getInstance().setGSMSignalStrengthBars(ui->pStrengthSld->value());
    } else {
        StatusManager::getInstance().unsetGSMSignalStrengthBars();
    }

}

void MainWindow::on_pStrengthSld_sliderMoved(int pos)
{
    if (ui->pStrengthChk->checkState()) {
        StatusManager::getInstance().setGSMSignalStrengthBars(ui->pStrengthSld->value());
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
    if (checked) {
        StatusManager::getInstance().setSecondaryCarrier(ui->sCarrierTxt->text().toStdString());
    } else {
        StatusManager::getInstance().unsetSecondaryCarrier();
    }

}

void MainWindow::on_sCarrierTxt_textEdited(const QString &text)
{
    if (ui->sCarrierChk->checkState()) {
        StatusManager::getInstance().setSecondaryCarrier(ui->sCarrierTxt->text().toStdString());
    }
}

void MainWindow::on_sBadgeChk_clicked(bool checked)
{
    if (checked) {
        StatusManager::getInstance().setSecondaryServiceBadge(ui->sBadgeTxt->text().toStdString());
    } else {
        StatusManager::getInstance().unsetSecondaryServiceBadge();
    }

}

void MainWindow::on_sBadgeTxt_textEdited(const QString &text)
{
    if (ui->sBadgeChk->checkState()) {
        StatusManager::getInstance().setSecondaryServiceBadge(ui->sBadgeTxt->text().toStdString());
    }
}

void MainWindow::on_sStrengthChk_clicked(bool checked)
{
    if (checked) {
        StatusManager::getInstance().setSecondaryGSMSignalStrengthBars(ui->sStrengthSld->value());
    } else {
        StatusManager::getInstance().unsetSecondaryGSMSignalStrengthBars();
    }

}

void MainWindow::on_sStrengthSld_sliderMoved(int pos)
{
    if (ui->sStrengthChk->checkState()) {
        StatusManager::getInstance().setSecondaryGSMSignalStrengthBars(ui->sStrengthSld->value());
    }
}











//void MainWindow::on_hideBatteryChk_clicked(bool checked)
//{
//    StatusManager::getInstance().hideBattery(checked);
//}

// Apply Page

void MainWindow::updateEnabledTweaks() {
    auto labelText = std::string();
    auto tweaks = DeviceManager::getInstance().getEnabledTweaks();
    if (tweaks.empty()) {
        labelText = "None";
    } else {
        for (auto t : tweaks) {
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
    if (uuid) {
        ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(*uuid) + "</a>");
    }
}

