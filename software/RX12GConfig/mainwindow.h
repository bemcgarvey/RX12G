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
    void getLimitsTabControls();
    void setLimitsTabControls();
    void getTuningTabControls();
    void setTuningTabControls();
    void getControls();
    void setControls();
    bool openFile(QString fileName);
    bool saveFile(QString fileName);
    void initSettings();
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
    void on_oneModeRadioButton_clicked(bool checked);
    void on_threeModeRadioButton_clicked(bool checked);
    void on_sixModeRadioButton_clicked(bool checked);
    void on_clearMinMaxPushButton_clicked();
    void on_aileronLimitsSetPushButton_clicked();
    void on_elevatorLimitsSetPushButton_clicked();
    void on_rudderLimitsSetPushButton_clicked();
    void on_aileron2LimitsSetPushButton_clicked();
    void on_elevator2LimitsSetPushButton_clicked();
    void on_actionOpen_Configuration_triggered();
    void on_actionSave_Configuration_triggered();
};

#endif // MAINWINDOW_H
