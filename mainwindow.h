#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CowabungaLite; }
QT_END_NAMESPACE

enum class Page {
    Home = 0,
    StatusBar = 1,
    Apply = 2
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

    // Apply
    void on_applyTweaksBtn_clicked();

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

    // Apply
    void updateEnabledTweaks();
};
#endif // MAINWINDOW_H
