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

    // Load status bar overrides
    MainWindow::loadStatusBar();
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
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(*version) + " (supported) </a>");
        } else {
            ui->phoneVersionLbl->setText("<a style=\"text-decoration:none; color: white\" href=\"#\">" + QString::fromStdString(*version) + " (not supported) </a>");
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
    ui->primaryCarrierTextChk->setCheckState(Qt::CheckState(StatusManager::getInstance().isCarrierOverridden() ? Qt::Checked : Qt::Unchecked));
    ui->primaryCarrierTextTxt->setText(QString::fromStdString(StatusManager::getInstance().getCarrierOverride()));
    ui->hideBatteryChk->setCheckState(Qt::CheckState(StatusManager::getInstance().isBatteryHidden() ? Qt::Checked : Qt::Unchecked));
}

void MainWindow::on_statusBarEnabledChk_toggled(bool checked)
{
    ui->statusBarPageContent->setDisabled(!checked);
    DeviceManager::getInstance().setTweakEnabled(Tweak::StatusBar, checked);
    MainWindow::updateEnabledTweaks();
}

void MainWindow::on_primaryCarrierTextChk_clicked(bool checked)
{
    if (checked) {
        StatusManager::getInstance().setCarrier(ui->primaryCarrierTextTxt->text().toStdString());
    } else {
        StatusManager::getInstance().unsetCarrier();
    }

}

void MainWindow::on_primaryCarrierTextTxt_textEdited(const QString &arg1)
{
    if (ui->primaryCarrierTextChk->checkState()) {
        StatusManager::getInstance().setCarrier(ui->primaryCarrierTextTxt->text().toStdString());
    }
}

void MainWindow::on_hideBatteryChk_clicked(bool checked)
{
    StatusManager::getInstance().hideBattery(checked);
}

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

