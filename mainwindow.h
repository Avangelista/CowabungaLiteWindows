#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CowabungaLite; }
QT_END_NAMESPACE

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
    void on_statusBarBtn_clicked();

    // Home
    void on_toolButton_3_clicked();

    // Status Bar
    void on_statusBarEnabledChk_toggled(bool checked);
    void on_primaryCarrierTextChk_clicked(bool checked);
    void on_primaryCarrierTextTxt_textEdited(const QString &arg1);
    void on_hideBatteryChk_clicked(bool checked);

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
};
#endif // MAINWINDOW_H
