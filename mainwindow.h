#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CowabungaLite; }
QT_END_NAMESPACE

enum class Page {
    Home = 0,
    StatusBar = 1,
    ControlCenter = 2,
    SpringboardOptions = 3,
    InternalOptions = 4,
    SetupOptions = 5,
    Apply = 6
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Sidebar
    void on_refreshBtn_clicked();
    void on_devicePicker_activated(int index);
    void on_homePageBtn_clicked();
    void on_statusBarPageBtn_clicked();
    void on_controlCenterPageBtn_clicked();
    void on_springboardOptionsPageBtn_clicked();
    void on_internalOptionsPageBtn_clicked();
    void on_setupOptionsPageBtn_clicked();
    void on_applyPageBtn_clicked();

    // Home
    void on_toolButton_3_clicked();
    void on_phoneVersionLbl_linkActivated(const QString &link);

    // Status Bar
    void on_statusBarEnabledChk_toggled(bool checked);
    void on_pDefaultRdo_clicked();
    void on_pShowRdo_clicked();
    void on_pHideRdo_clicked();
    void on_pCarrierChk_clicked(bool checked);
    void on_pCarrierTxt_textEdited(const QString &text);
    void on_pBadgeChk_clicked(bool checked);
    void on_pBadgeTxt_textEdited(const QString &text);
    void on_pStrengthChk_clicked(bool checked);
    void on_pStrengthSld_sliderMoved(int pos);
    void on_sDefaultRdo_clicked();
    void on_sShowRdo_clicked();
    void on_sHideRdo_clicked();
    void on_sCarrierChk_clicked(bool checked);
    void on_sCarrierTxt_textEdited(const QString &text);
    void on_sBadgeChk_clicked(bool checked);
    void on_sBadgeTxt_textEdited(const QString &text);
    void on_sStrengthChk_clicked(bool checked);
    void on_sStrengthSld_sliderMoved(int pos);
    void on_timeChk_clicked(bool checked);
    void on_timeTxt_textEdited(const QString &text);
    void on_breadcrumbChk_clicked(bool checked);
    void on_breadcrumbTxt_textEdited(const QString &text);
    void on_batteryDetailChk_clicked(bool checked);
    void on_batteryDetailTxt_textEdited(const QString &text);
    void on_batteryCapacityChk_clicked(bool checked);
    void on_batteryCapacitySld_sliderMoved(int pos);
    void on_wifiStrengthChk_clicked(bool checked);
    void on_wifiStrengthSld_sliderMoved(int pos);
    void on_numericWifiChk_clicked(bool checked);
    void on_numericCellChk_clicked(bool checked);
    void on_hideDNDChk_clicked(bool checked);
    void on_hideAirplaneChk_clicked(bool checked);
    void on_hideCellChk_clicked(bool checked);
    void on_hideWifiChk_clicked(bool checked);
    void on_hideBatteryChk_clicked(bool checked);
    void on_hideBluetoothChk_clicked(bool checked);
    void on_hideAlarmChk_clicked(bool checked);
    void on_hideLocationChk_clicked(bool checked);
    void on_hideRotationChk_clicked(bool checked);
    void on_hideAirPlayChk_clicked(bool checked);
    void on_hideCarPlayChk_clicked(bool checked);
    void on_hideVPNChk_clicked(bool checked);

    // Springboard Options
    void on_springboardOptionsEnabledChk_toggled(bool checked);
    void on_UIAnimSpeedSld_sliderMoved(int pos);
    void on_footnoteTxt_textEdited(const QString &text);
    void on_disableLockRespringChk_clicked(bool checked);
    void on_disableDimmingChk_clicked(bool checked);
    void on_disableBatteryAlertsChk_clicked(bool checked);
    void on_enableLSCCChk_clicked(bool checked);
    void on_enableShutdownSoundChk_clicked(bool checked);
    void on_allowAirDropEveryoneChk_clicked(bool checked);

    // Setup Options

    void on_setupOptionsEnabledChk_toggled(bool checked);
    void on_skipSetupChk_clicked(bool checked);
    void on_disableUpdatesChk_clicked(bool checked);
    void on_enableSupervisionChk_clicked(bool checked);
    void on_organizationNameTxt_textEdited(const QString &text);

    // Apply
    void on_applyTweaksBtn_clicked();

    // Window
    void on_titleBar_pressed();
    void on_closeBtn_clicked();

private:
    Ui::CowabungaLite *ui;

    // Utilities
    void updateInterfaceForNewDevice();

    // Sidebar
    void refreshDevices();

    // Home
    void updatePhoneInfo();

    // Status Bar
    void loadStatusBar();

    // Springboard Options
    void loadSpringboardOptions();

    // Setup Options
    void loadSetupOptions();

    // Apply
    void updateEnabledTweaks();
};
#endif // MAINWINDOW_H
