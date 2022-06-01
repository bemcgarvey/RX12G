#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>
#include "QHidWatcher.h"
#include "HidUSBLink.h"
#include "RX12G.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel *connectLabel;
    QHidWatcher* hidWatcher;
    HidUSBLink usb;
    Ui::MainWindow *ui;
    uint8_t buffer[64];
    QTimer *channelsTimer;
    QProgressBar *channelBars[12];
    QTimer *sensorTimer;
    Settings settings;
    void getRxTabControls();
    void setRxTabControls();
    void getPlaneTabControls();
    void setPlaneTabControls();
    void getGyroTabControls();
    void setGyroTabControls();

private slots:
    void onUsbConnected();
    void onUsbRemoved();
    void on_loadPushButton_clicked();
    void on_connectPushButton_clicked();
    void on_savePushButton_clicked();
    void on_actionExit_triggered();
    void on_sbusEnableCheckBox_stateChanged(int arg1);
    void on_normalFailsafeRadioButton_clicked();
    void on_presetFailsafeRadioButton_clicked();
    void onChannelTimout(void);
    void on_savePresetsPushButton_clicked();
    void on_tabWidget_currentChanged(int index);
    void onSensorTimout(void);
};

#endif // MAINWINDOW_H
