#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>
#include "QHidWatcher.h"
#include "HidUSBLink.h"
#include "RX12G.h"
#include "hidbootloader.h"
#include "workerthread.h"

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
    QTimer *gainsTimer;
    Settings settings;
    bool isLeveling;
    int levelSampleCount;
    int levelSampleSums[3];
    QProgressBar *levelingProgressBar;
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
    void getGainTabControls();
    void setGainTabControls();
    void getControls();
    void setControls();
    bool openFile(QString fileName);
    bool saveFile(QString fileName);
    void initSettings();
    bool controlsValid();
    bool calculateLevelOffsets();
    std::unique_ptr<HidBootloader> bootloader;
    std::unique_ptr<WorkerThread> worker;
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
    void onChannelTimout();
    void on_savePresetsPushButton_clicked();
    void on_tabWidget_currentChanged(int index);
    void onSensorTimout();
    void onGainsTimout();
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
    void on_rebootPushButton_clicked();
    void on_oneAileronRadioButton_toggled(bool checked);
    void on_twoAileronsRadioButton_toggled(bool checked);
    void on_oneElevatorRadioButton_toggled(bool checked);
    void on_twoElevatorRadioButton_toggled(bool checked);
    void on_levelCalibratePushButton_clicked();
    void on_defaultLevelPushButton_clicked();
    void on_elevon_ARadioButton_toggled(bool checked);
    void on_elevon_BRadioButton_toggled(bool checked);
    void on_connectBootloaderPushButton_clicked();
    void on_browsePushButton_clicked();
    void on_updateFirmwarePushButton_clicked();
    void on_cancelUpdatePushButton_clicked();

    void on_rxOnlyCheckBox_stateChanged(int arg1);

    void on_taerRadioButton_toggled(bool checked);

    void on_aetrRadioButton_toggled(bool checked);

    void on_bindPushButton_clicked();

public slots:
    void onBtlMessage(QString msg);
    void onBtlProgress(int progress);
    void onBtlFinished(bool success);
};

#endif // MAINWINDOW_H
