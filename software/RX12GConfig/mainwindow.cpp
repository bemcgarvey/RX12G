#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"
#include <QMessageBox>
#include "version.h"
#include <QFileDialog>
#include <QSettings>
#include <math.h>
#include <synchapi.h>
#include "aboutdialog.h"

//TODO add something to icon to distinguish from RX12

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), usb(64, false)
    , ui(new Ui::MainWindow), isLeveling(false)
{
    ui->setupUi(this);
    usb.SetWaitTimesInMs(500, 500);
    connectLabel = new QLabel("Not connected");
    ui->statusbar->addPermanentWidget(connectLabel);
    hidWatcher = new QHidWatcher(PID, VID);
    connect(hidWatcher, &QHidWatcher::connected, this, &MainWindow::onUsbConnected);
    connect(hidWatcher, &QHidWatcher::removed, this, &MainWindow::onUsbRemoved);
    channelsTimer = new QTimer(this);
    connect(channelsTimer, &QTimer::timeout, this, &MainWindow::onChannelTimout);
    sensorTimer = new QTimer(this);
    connect(sensorTimer, &QTimer::timeout, this, &MainWindow::onSensorTimout);
    gainsTimer = new QTimer(this);
    connect(gainsTimer, &QTimer::timeout, this, &MainWindow::onGainsTimout);
    channelBars[0] = ui->ch1ProgressBar;
    channelBars[1] = ui->ch2ProgressBar;
    channelBars[2] = ui->ch3ProgressBar;
    channelBars[3] = ui->ch4ProgressBar;
    channelBars[4] = ui->ch5ProgressBar;
    channelBars[5] = ui->ch6ProgressBar;
    channelBars[6] = ui->ch7ProgressBar;
    channelBars[7] = ui->ch8ProgressBar;
    channelBars[8] = ui->ch9ProgressBar;
    channelBars[9] = ui->ch10ProgressBar;
    channelBars[10] = ui->ch11ProgressBar;
    channelBars[11] = ui->ch12ProgressBar;
    initSettings();
    ui->xGyroDisplay->setRange(2000);
    ui->yGyroDisplay->setRange(2000);
    ui->zGyroDisplay->setRange(2000);
    ui->yGyroDisplay->setOrientation(GyroDisplay::VERTICAL);
    ui->zGyroDisplay->setOrientation(GyroDisplay::VERTICAL);
    levelingProgressBar = new QProgressBar();
    levelingProgressBar->setRange(0, 30);
    setControls();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hidWatcher;
}

void MainWindow::onUsbConnected()
{
    on_connectPushButton_clicked();
}

void MainWindow::onUsbRemoved()
{
    if (usb.Connected()) {
        usb.Close();
    }
    connectLabel->setText("Not connected");
    ui->loadPushButton->setEnabled(false);
    ui->savePushButton->setEnabled(false);
    ui->rebootPushButton->setEnabled(false);
    ui->levelCalibratePushButton->setEnabled(false);
    ui->bindPushButton->setEnabled(false);
    channelsTimer->stop();
    for (int i = 0; i < 12; ++i) {
        channelBars[i]->setValue(0);
        channelBars[i]->setEnabled(false);
    }
    ui->connectPushButton->setEnabled(true);
}

void MainWindow::on_loadPushButton_clicked()
{
    buffer[0] = GET_SETTINGS;
    usb.SendReport(buffer);
    int bytesRemaining = sizeof(Settings);
    uint8_t *p = (uint8_t *) &settings;
    do {
        usb.GetReport(buffer);
        if (bytesRemaining >= 64) {
            memcpy(p, buffer, 64);
            p += 64;
            bytesRemaining -= 64;
        } else {
            memcpy(p, buffer, bytesRemaining);
            bytesRemaining = 0;
        }
    } while (bytesRemaining > 0);
    uint32_t crc = calculateCRC(&settings, sizeof(Settings));
    uint32_t sentCRC;
    usb.GetReport(buffer);
    sentCRC = *(uint32_t *)buffer;
    if (crc != sentCRC) {
        QMessageBox::critical(this, QApplication::applicationName(), "Error loading settings");
        return;
    }
    setControls();
    ui->statusbar->showMessage("Settings loaded.", 2000);
}


void MainWindow::on_connectPushButton_clicked()
{
    if (usb.Connected()) {
        usb.Close();
    }
    usb.Open(PID, VID);
    if (usb.Connected()) {
        buffer[0] = GET_VERSION;
        usb.SendReport(buffer);
        usb.GetReport(buffer);
        Version::firmwareMajorVersion = buffer[1];
        Version::firmwareMinorVersion = buffer[0];
        connectLabel->setText("Connected: Firmware version " + Version::firmwareVersionString());
        ui->loadPushButton->setEnabled(true);
        ui->savePushButton->setEnabled(true);
        ui->rebootPushButton->setEnabled(true);
        ui->levelCalibratePushButton->setEnabled(true);
        ui->bindPushButton->setEnabled(true);
        on_loadPushButton_clicked();
        on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
        ui->connectPushButton->setEnabled(false);
    } else {
        connectLabel->setText("Not connected");
        ui->loadPushButton->setEnabled(false);
        ui->savePushButton->setEnabled(false);
        ui->rebootPushButton->setEnabled(false);
        ui->levelCalibratePushButton->setEnabled(false);
        ui->bindPushButton->setEnabled(false);
        channelsTimer->stop();
        sensorTimer->stop();
        for (int i = 0; i < 12; ++i) {
            channelBars[i]->setValue(0);
            channelBars[i]->setEnabled(false);
        }
    }
}


void MainWindow::on_savePushButton_clicked()
{
    if (controlsValid()) {
        getControls();
        buffer[0] = SAVE_SETTINGS;
        usb.SendReport(buffer);
        int bytesRemaining = sizeof(Settings);
        uint8_t *p = (uint8_t *) &settings;
        do {
            if (bytesRemaining >= 64) {
                memcpy(buffer, p, 64);
                p += 64;
                bytesRemaining -= 64;
            } else {
                memcpy(buffer, p, bytesRemaining);
                bytesRemaining = 0;
            }
            usb.SendReport(buffer);
        } while (bytesRemaining > 0);
        uint32_t crc = calculateCRC(&settings, sizeof(Settings));
        *(uint32_t *)buffer = crc;
        usb.SendReport(buffer);
        usb.GetReport(buffer);
        if (buffer[0] == CMD_ACK) {
            ui->statusbar->showMessage("Settings saved", 2000);
        } else {
            QMessageBox::critical(this, QApplication::applicationName(), "Unable to save settings");
        }
    }
}


void MainWindow::on_actionExit_triggered()
{
    close();
}


void MainWindow::on_sbusEnableCheckBox_stateChanged(int arg1)
{
    if (arg1) {
        ui->sbusOutputsSpinBox->setEnabled(true);
        ui->sbusPeriodSpinBox->setEnabled(true);
    } else {
        ui->sbusOutputsSpinBox->setEnabled(false);
        ui->sbusPeriodSpinBox->setEnabled(false);
    }
}


void MainWindow::on_normalFailsafeRadioButton_clicked()
{
    ui->savePresetsPushButton->setEnabled(!ui->normalFailsafeRadioButton->isChecked());
}


void MainWindow::on_presetFailsafeRadioButton_clicked()
{
    ui->savePresetsPushButton->setEnabled(!ui->normalFailsafeRadioButton->isChecked());
}

void MainWindow::onChannelTimout()
{
    if (!usb.Connected()) {
        return;
    }
    buffer[0] = GET_CHANNELS;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    uint16_t *channels = (uint16_t *)buffer;
    if (ui->tabWidget->tabText(ui->tabWidget->currentIndex()) == "Receiver") {
        for (int i = 0; i < 8; ++i) {
            if (channels[i] == 0xffff) {
                channelBars[i]->setEnabled(false);
                channelBars[i]->setValue(0);
            } else {
                channelBars[i]->setEnabled(true);
                channelBars[i]->setValue(channels[i]);
            }
        }
        if (ui->show13RadioButton->isChecked()) {
            for (int i = 12; i < 16; ++i) {
                if (channels[i] == 0xffff) {
                    channelBars[i - 4]->setEnabled(false);
                    channelBars[i - 4]->setValue(0);
                } else {
                    channelBars[i - 4]->setEnabled(true);
                    channelBars[i - 4]->setValue(channels[i]);
                }
            }
        } else if (ui->show17RadioButton->isChecked()) {
            for (int i = 16; i < 20; ++i) {
                if (channels[i] == 0xffff) {
                    channelBars[i - 8]->setEnabled(false);
                    channelBars[i - 8]->setValue(0);
                } else {
                    channelBars[i - 8]->setEnabled(true);
                    channelBars[i - 8]->setValue(channels[i]);
                }
            }
        } else {
            for (int i = 8; i < 12; ++i) {
                if (channels[i] == 0xffff) {
                    channelBars[i]->setEnabled(false);
                    channelBars[i]->setValue(0);
                } else {
                    channelBars[i]->setEnabled(true);
                    channelBars[i]->setValue(channels[i]);
                }
            }
        }
    } else if (ui->tabWidget->tabText(ui->tabWidget->currentIndex()) == "Limits") {
        if (settings.channelOrder == CHANNEL_ORDER_TAER) {
            ui->aileronMinMaxBar->setValue(channels[1]);
            ui->elevatorMinMaxBar->setValue(channels[2]);
            ui->rudderMinMaxBar->setValue(channels[3]);
        }
        if (settings.channelOrder == CHANNEL_ORDER_AETR) {
            ui->aileronMinMaxBar->setValue(channels[0]);
            ui->elevatorMinMaxBar->setValue(channels[1]);
            ui->rudderMinMaxBar->setValue(channels[3]);
        }
        if (settings.gyroEnabledFlags & AILERON2_MASK) {
            ui->aileron2MinMaxBar->setValue(channels[settings.aileron2Channel]);
        }
        if (settings.gyroEnabledFlags & ELEVATOR2_MASK) {
            ui->elevator2MinMaxBar->setValue(channels[settings.elevator2Channel]);
        }
    }
}


void MainWindow::on_savePresetsPushButton_clicked()
{
    if (!usb.Connected()) {
        return;
    }
    buffer[0] = SET_PRESETS;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    if (buffer[0] == CMD_ACK) {
        ui->statusbar->showMessage("Presets saved", 2000);
    } else {
        QMessageBox::critical(this, QApplication::applicationName(), "Unable to save presets");
    }
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    channelsTimer->stop();
    sensorTimer->stop();
    gainsTimer->stop();
    if (usb.Connected()) {
        if (ui->tabWidget->tabText(index) == "Receiver"
                || ui->tabWidget->tabText(index) == "Limits") {
            ui->horizonWidget->setEnabled(false);
            ui->xGyroDisplay->setEnabled(false);
            ui->yGyroDisplay->setEnabled(false);
            ui->zGyroDisplay->setEnabled(false);
            channelsTimer->start(100);
        } else if (ui->tabWidget->tabText(index) == "Sensors") {
            ui->horizonWidget->setEnabled(true);
            ui->xGyroDisplay->setEnabled(true);
            ui->yGyroDisplay->setEnabled(true);
            ui->zGyroDisplay->setEnabled(true);
            sensorTimer->start(100);
        } else if (ui->tabWidget->tabText(index) == "Gains") {
            gainsTimer->start(100);
        }
    }
}

void MainWindow::onSensorTimout()
{
    if (!usb.Connected()) {
        return;
    }
    buffer[0] = GET_SENSORS;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    int16_t *data = (int16_t *)buffer;
    if (isLeveling) {
        ++levelSampleCount;
        levelingProgressBar->setValue(levelSampleCount);
        for (int i = 0; i < 3; ++i) {
            levelSampleSums[i] += data[i + 3];
        }
        if (levelSampleCount == 30) {
            ui->statusbar->removeWidget(levelingProgressBar);
            if (calculateLevelOffsets()) {
                isLeveling = false;
                QMessageBox::information(this, QApplication::applicationName(), "Level calibration complete.  Save settings and reboot to apply.");
            } else {
                QMessageBox::warning(this, QApplication::applicationName(), "Level offsets are out of range.  Check the orientation of the receiver.  Offsets have not been updated");
            }
            ui->levelCalibratePushButton->setEnabled(true);
        }
    }
    ui->xAccel->setText(QString().setNum(data[3]));
    ui->yAccel->setText(QString().setNum(data[4]));
    ui->zAccel->setText(QString().setNum(data[5]));
    ui->xGyro->setText(QString().setNum(data[0]));
    ui->yGyro->setText(QString().setNum(data[1]));
    ui->zGyro->setText(QString().setNum(data[2]));
    buffer[0] = GET_ATTITUDE;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    AttitudeData *pa = (AttitudeData *)buffer;
    ui->horizonWidget->updatePitch(pa->ypr.pitch);
    ui->horizonWidget->updateRoll(pa->ypr.roll);
    ui->xGyroDisplay->setValue(pa->gyroRatesDeg.x);
    ui->yGyroDisplay->setValue(pa->gyroRatesDeg.y);
    ui->zGyroDisplay->setValue(pa->gyroRatesDeg.z);
    ui->rollLabel->setText(QString("%1°").arg(pa->ypr.roll, 0, 'f', 1));
    ui->pitchLabel->setText(QString("%1°").arg(pa->ypr.pitch, 0, 'f', 1));
    ui->yawLabel->setText(QString("%1°").arg(pa->ypr.yaw, 0, 'f', 1));
}

void MainWindow::onGainsTimout() {
    if (!usb.Connected()) {
        return;
    }
    buffer[0] = GET_GAINS;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    float *fp = reinterpret_cast<float *>(buffer);
    int value = round(fp[0] * 100);
    ui->normalRollLabel->setText(QString("%1%").arg(value));
    value = round(fp[1] * 100);
    ui->normalPitchLabel->setText(QString("%1%").arg(value));
    value = round(fp[2] * 100);
    ui->normalYawLabel->setText(QString("%1%").arg(value));
    value = round(fp[3] * 100);
    ui->levelRollLabel->setText(QString("%1%").arg(value));
    value = round(fp[4] * 100);
    ui->levelPitchLabel->setText(QString("%1%").arg(value));
    value = round(fp[6] * 100);
    ui->lockRollLabel->setText(QString("%1%").arg(value));
    value = round(fp[7] * 100);
    ui->lockPitchLabel->setText(QString("%1%").arg(value));
    value = round(fp[8] * 100);
    ui->lockYawLabel->setText(QString("%1%").arg(value));
}

void MainWindow::on_oneModeRadioButton_clicked(bool checked)
{
    if (checked) {
        ui->mode2ComboBox->setEnabled(false);
        ui->mode3ComboBox->setEnabled(false);
        ui->mode4ComboBox->setEnabled(false);
        ui->mode5ComboBox->setEnabled(false);
        ui->mode6ComboBox->setEnabled(false);
        ui->modeChannelComboBox->setCurrentIndex(0);
        ui->modeChannelComboBox->setEnabled(false);
    }
}


void MainWindow::on_threeModeRadioButton_clicked(bool checked)
{
    if (checked) {
        ui->mode2ComboBox->setEnabled(true);
        ui->mode3ComboBox->setEnabled(true);
        ui->mode4ComboBox->setEnabled(false);
        ui->mode5ComboBox->setEnabled(false);
        ui->mode6ComboBox->setEnabled(false);
        ui->modeChannelComboBox->setEnabled(true);
    }
}


void MainWindow::on_sixModeRadioButton_clicked(bool checked)
{
    if (checked) {
        ui->mode2ComboBox->setEnabled(true);
        ui->mode3ComboBox->setEnabled(true);
        ui->mode4ComboBox->setEnabled(true);
        ui->mode5ComboBox->setEnabled(true);
        ui->mode6ComboBox->setEnabled(true);
        ui->modeChannelComboBox->setEnabled(true);
    }
}


void MainWindow::on_clearMinMaxPushButton_clicked()
{
    ui->aileronMinMaxBar->reset();
    ui->elevatorMinMaxBar->reset();
    ui->rudderMinMaxBar->reset();
    ui->aileron2MinMaxBar->reset();
    ui->elevator2MinMaxBar->reset();
}


void MainWindow::on_aileronLimitsSetPushButton_clicked()
{
    int min = ui->aileronMinMaxBar->getMinValue();
    int max = ui->aileronMinMaxBar->getMaxValue();
    ui->aileronMinMaxBar->setInitialMinMax(min, max);
    settings.minTravelLimits[AILERON_INDEX] = min;
    settings.maxTravelLimits[AILERON_INDEX] = max;
}


void MainWindow::on_elevatorLimitsSetPushButton_clicked()
{
    int min = ui->elevatorMinMaxBar->getMinValue();
    int max = ui->elevatorMinMaxBar->getMaxValue();
    ui->elevatorMinMaxBar->setInitialMinMax(min, max);
    settings.minTravelLimits[ELEVATOR_INDEX] = min;
    settings.maxTravelLimits[ELEVATOR_INDEX] = max;
}


void MainWindow::on_rudderLimitsSetPushButton_clicked()
{
    int min = ui->rudderMinMaxBar->getMinValue();
    int max = ui->rudderMinMaxBar->getMaxValue();
    ui->rudderMinMaxBar->setInitialMinMax(min, max);
    settings.minTravelLimits[RUDDER_INDEX] = min;
    settings.maxTravelLimits[RUDDER_INDEX] = max;
}


void MainWindow::on_aileron2LimitsSetPushButton_clicked()
{
    int min = ui->aileron2MinMaxBar->getMinValue();
    int max = ui->aileron2MinMaxBar->getMaxValue();
    ui->aileron2MinMaxBar->setInitialMinMax(min, max);
    settings.minTravelLimits[AILERON2_INDEX] = min;
    settings.maxTravelLimits[AILERON2_INDEX] = max;
}


void MainWindow::on_elevator2LimitsSetPushButton_clicked()
{
    int min = ui->elevator2MinMaxBar->getMinValue();
    int max = ui->elevator2MinMaxBar->getMaxValue();
    ui->elevator2MinMaxBar->setInitialMinMax(min, max);
    settings.minTravelLimits[ELEVATOR2_INDEX] = min;
    settings.maxTravelLimits[ELEVATOR2_INDEX] = max;
}


void MainWindow::on_actionOpen_Configuration_triggered()
{
    QString fileName;
    QSettings settings;
    QString dir = settings.value("last folder", "").toString();
    fileName = QFileDialog::getOpenFileName(this, "Open Configuration", dir, "RX12G Files (*.RX12G)");
    if (fileName != "") {
        if (openFile(fileName)) {
            setWindowTitle(windowTitle() + " - " + fileName);
            setControls();
        } else {
            QMessageBox::critical(this, QApplication::applicationName(), "Unable to open file.");
        }
        QFileInfo info(fileName);
        settings.setValue("last folder", info.path());
    }
}


void MainWindow::on_actionSave_Configuration_triggered()
{
    QString fileName;
    QSettings settings;
    QString dir = settings.value("last folder", "").toString();
    fileName = QFileDialog::getSaveFileName(this, "Save Configuration", dir, "RX12G Files (*.RX12G)");
    if (fileName != "") {
        getControls();
        if (saveFile(fileName)) {
            setWindowTitle(windowTitle() + " - " + fileName);
        } else {
            QMessageBox::critical(this, QApplication::applicationName(), "Unable to save file.");
        }
        QFileInfo info(fileName);
        settings.setValue("last folder", info.path());
    }
}


void MainWindow::on_rebootPushButton_clicked()
{
    buffer[0] = REBOOT;
    usb.SendReport(buffer);
}


void MainWindow::on_oneAileronRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->aileron2LimitsSetPushButton->setEnabled(false);
        ui->aileron2MinMaxBar->setEnabled(false);
        ui->enableAileron2CheckBox->setEnabled(false);
        ui->enableAileron2CheckBox->setChecked(false);
        ui->reverseAileron2CheckBox->setChecked(false);
        ui->reverseAileron2CheckBox->setEnabled(false);
        ui->aileron2ChannelComboBox->setEnabled(false);
        ui->aileron2ChannelComboBox->setCurrentIndex(0);
    }
}


void MainWindow::on_twoAileronsRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->aileron2LimitsSetPushButton->setEnabled(true);
        ui->aileron2MinMaxBar->setEnabled(true);
        ui->enableAileron2CheckBox->setEnabled(true);
        ui->reverseAileron2CheckBox->setEnabled(true);
        ui->aileron2ChannelComboBox->setEnabled(true);
    }
}

void MainWindow::on_oneElevatorRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->elevator2ChannelComboBox->setEnabled(false);
        ui->elevator2LimitsSetPushButton->setEnabled(false);
        ui->enableElevator2CheckBox->setEnabled(false);
        ui->enableElevator2CheckBox->setChecked(false);
        ui->reverseElevator2CheckBox->setEnabled(false);
        ui->reverseElevator2CheckBox->setChecked(false);
        ui->elevator2MinMaxBar->setEnabled(false);
        ui->elevator2ChannelComboBox->setCurrentIndex(0);
    }
}


void MainWindow::on_twoElevatorRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->elevator2ChannelComboBox->setEnabled(true);
        ui->elevator2LimitsSetPushButton->setEnabled(true);
        ui->enableElevator2CheckBox->setEnabled(true);
        ui->reverseElevator2CheckBox->setEnabled(true);
        ui->elevator2MinMaxBar->setEnabled(true);
    }
}


void MainWindow::on_levelCalibratePushButton_clicked()
{
    if (!usb.Connected()) {
        return;
    }
    ui->levelCalibratePushButton->setEnabled(false);
    buffer[0] = DISABLE_OFFSETS;
    usb.SendReport(buffer);
    levelSampleCount = 0;
    for (int i = 0; i < 3; ++i) {
        levelSampleSums[i] = 0;
    }
    isLeveling = true;
    levelingProgressBar->setValue(0);
    levelingProgressBar->setVisible(true);
    ui->statusbar->addWidget(levelingProgressBar);
}


void MainWindow::on_defaultLevelPushButton_clicked()
{
    for (int i = 0; i < 3; ++i) {
        settings.levelOffsets[i] = 0;
    }
    QMessageBox::information(this, QApplication::applicationName(),
            "Level offsets have been zeroed. Save settings and reboot to apply");
}


void MainWindow::on_elevon_ARadioButton_toggled(bool checked)
{
    if (checked) {
        ui->aileron2LimitsSetPushButton->setEnabled(false);
        ui->aileron2MinMaxBar->setEnabled(false);
        ui->enableAileron2CheckBox->setChecked(false);
        ui->reverseAileron2CheckBox->setChecked(false);
        ui->enableAileron2CheckBox->setEnabled(false);
        ui->reverseAileron2CheckBox->setEnabled(false);
        ui->aileron2ChannelComboBox->setEnabled(false);
        ui->aileron2ChannelComboBox->setCurrentIndex(0);
    }
}


void MainWindow::on_elevon_BRadioButton_toggled(bool checked)
{
    on_elevon_ARadioButton_toggled(checked);
}


void MainWindow::on_connectBootloaderPushButton_clicked()
{
    if (usb.Connected()) {
        buffer[0] = BOOTLOAD;
        usb.SendReport(buffer);
        Sleep(500);
    }
    bootloader.reset(new HidBootloader(0x4d63, 0x2000));
    if (!bootloader->isConnected()) {
        QMessageBox::critical(this, QApplication::applicationName(),
                              "Can't connect to bootloader.  Restart the RX12G in bootload mode and try again.");
    } else {
        ui->browsePushButton->setEnabled(true);
        ui->cancelUpdatePushButton->setEnabled(true);
        ui->firmwareFileLabel->setText("Please select a file");
        ui->updateProgressBar->setValue(0);
        uint16_t version = bootloader->readBootInfo();
        ui->statusbar->showMessage(QString("Connected to bootloader version %1.%2")
                              .arg(version >> 8).arg(version & 0xff), 0);
        bootloader->setFamily(Bootloader::PIC32);
        ui->firmwareFileLabel->setText("Please select a file");
        connect(bootloader.get(), &Bootloader::message, this, &MainWindow::onBtlMessage);
        connect(bootloader.get(), &Bootloader::progress, this, &MainWindow::onBtlProgress);
        connect(bootloader.get(), &Bootloader::finished, this, &MainWindow::onBtlFinished);
    }
}


void MainWindow::on_browsePushButton_clicked()
{
    QString fileName;
    QSettings settings;
    QString dir = settings.value("last firmware file", "").toString();
    if (!bootloader->isConnected()) {
        return;
    }
    fileName = QFileDialog::getOpenFileName(this, "Open firmware file", dir, "Hex files (*.hex)");
    if (fileName != "") {
        if (bootloader->setFile(fileName)) {
            ui->firmwareFileLabel->setText(fileName);
            ui->updateFirmwarePushButton->setEnabled(true);
        } else {
            ui->updateFirmwarePushButton->setEnabled(false);
            ui->firmwareFileLabel->setText("Not Selected");
            QMessageBox::critical(this, QApplication::applicationName(), "Unable to open file.");
        }
        settings.setValue("last firmware file", fileName);
    }
}


void MainWindow::on_updateFirmwarePushButton_clicked()
{
    if (!bootloader->isConnected()) {
        return;
    }
    ui->updateFirmwarePushButton->setEnabled(false);
    ui->cancelUpdatePushButton->setEnabled(false);
    ui->browsePushButton->setEnabled(false);
    ui->updateProgressBar->setValue(0);
    ui->connectBootloaderPushButton->setEnabled(false);
    worker.reset(new WorkerThread(bootloader.get()));
    worker->start();
}

void MainWindow::onBtlMessage(QString msg)
{
    ui->statusbar->showMessage(msg, 0);
}

void MainWindow::onBtlProgress(int progress)
{
    ui->updateProgressBar->setValue(progress);
}

void MainWindow::onBtlFinished(bool success)
{
    if (worker) {
        worker->wait();
        worker = nullptr;
    }
    if (success) {
        QMessageBox::information(this, QApplication::applicationName(),
                                 "Success! Firmware updated");
    } else {
        QMessageBox::warning(this, QApplication::applicationName(),
                             "Firmware update failed");
    }
    ui->connectBootloaderPushButton->setEnabled(true);
    ui->statusbar->clearMessage();
}


void MainWindow::on_cancelUpdatePushButton_clicked()
{
    if (!bootloader->isConnected()) {
        return;
    }
    ui->updateFirmwarePushButton->setEnabled(false);
    ui->cancelUpdatePushButton->setEnabled(false);
    ui->browsePushButton->setEnabled(false);
    bootloader->jumpToApp();
}


void MainWindow::on_rxOnlyCheckBox_stateChanged(int arg1)
{
    if (arg1) {
        settings.rxOnly = RX_ONLY_MODE;
        for (int i = 1; i <= 6; ++i) {
            ui->tabWidget->setTabEnabled(i, false);
        }
    } else {
        settings.rxOnly = NORMAL_RX_MODE;
        for (int i = 1; i <= 6; ++i) {
            ui->tabWidget->setTabEnabled(i, true);
        }
    }
}


void MainWindow::on_taerRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->ch1Label->setText("CH 1 (Thr)");
        ui->ch2Label->setText("CH 2 (Ail)");
        ui->ch3Label->setText("CH 3 (Ele)");
    }
}


void MainWindow::on_aetrRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->ch1Label->setText("CH 1 (Ail)");
        ui->ch2Label->setText("CH 2 (Ele)");
        ui->ch3Label->setText("CH 3 (Thr)");
    }
}


void MainWindow::on_bindPushButton_clicked()
{
    if (!usb.Connected()) {
        return;
    }
    buffer[0] = BIND;
    usb.SendReport(buffer);
}


void MainWindow::on_show9RadioButton_clicked()
{
    updateChannelDisplay();
}


void MainWindow::on_show13RadioButton_clicked()
{
    updateChannelDisplay();
}


void MainWindow::on_show17RadioButton_clicked()
{
    updateChannelDisplay();
}


void MainWindow::on_actionAbout_RX12G_Configure_triggered()
{
    auto dlg = std::make_unique<AboutDialog>(this);
    dlg->exec();
}

