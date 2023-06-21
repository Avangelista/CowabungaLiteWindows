#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DeviceManager.h"
#include "statusmanager/StatusManager.h"
#include <QDebug>

enum class Page {
    Home = 0,
    StatusBar = 1
};

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
    std::vector<DeviceInfo> devices = DeviceManager::getInstance().loadDevices();

    if (devices.empty()) {
        ui->devicePicker->setEnabled(false);
        ui->devicePicker->addItem(QString("None"), QVariant::fromValue(NULL));
        DeviceManager::getInstance().resetCurrentDevice();
    } else {
        ui->devicePicker->setEnabled(true);
        // Populate the combobox with device names
        for (const DeviceInfo& device : devices) {
            QString deviceName = QString("%1 (%2)").arg(QString::fromStdString(device.Name), QString::fromStdString(device.Version));
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

void MainWindow::on_statusBarBtn_clicked()
{
    ui->pages->setCurrentIndex(static_cast<int>(Page::StatusBar));
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
    std::optional name = DeviceManager::getInstance().getCurrentName();
    if (name) {
        ui->phoneNameLbl->setText(QString::fromStdString(name.value()));
    } else {
        ui->phoneNameLbl->setText("None");
    }
    std::optional version = DeviceManager::getInstance().getCurrentVersion();
    if (version) {
        if (DeviceManager::getInstance().isDeviceAvailable()) {
            ui->phoneVersionLbl->setText(QString::fromStdString(version.value()) + " (supported)");
        } else {
            ui->phoneVersionLbl->setText(QString::fromStdString(version.value()) + " (not supported)");
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


