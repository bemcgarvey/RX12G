#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"
#include <QMessageBox>
#include "version.h"
#include <QFileDialog>
#include <QSettings>

//TODO add something to icon to distinguish from RX12

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), usb(64, false)
    , ui(new Ui::MainWindow)
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
        on_loadPushButton_clicked();
        on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
        ui->connectPushButton->setEnabled(false);
    } else {
        connectLabel->setText("Not connected");
        ui->loadPushButton->setEnabled(false);
        ui->savePushButton->setEnabled(false);
        ui->rebootPushButton->setEnabled(false);
        ui->levelCalibratePushButton->setEnabled(false);
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
        for (int i = 0; i < 12; ++i) {
            if (channels[i] == 0xffff) {
                channelBars[i]->setEnabled(false);
                channelBars[i]->setValue(0);
            } else {
                channelBars[i]->setEnabled(true);
                channelBars[i]->setValue(channels[i]);
            }
        }
    } else if (ui->tabWidget->tabText(ui->tabWidget->currentIndex()) == "Limits") {
        ui->aileronMinMaxBar->setValue(channels[1]);
        ui->elevatorMinMaxBar->setValue(channels[2]);
        ui->rudderMinMaxBar->setValue(channels[3]);
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
    if (ui->tabWidget->tabText(index) == "Receiver"
            || ui->tabWidget->tabText(index) == "Limits") {
        channelsTimer->start(100);
    } else if (ui->tabWidget->tabText(index) == "Sensors") {
        sensorTimer->start(100);
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
    ui->xAccel->setText(QString().setNum(data[3]));
    ui->yAccel->setText(QString().setNum(data[4]));
    ui->zAccel->setText(QString().setNum(data[5]));
    ui->xGyro->setText(QString().setNum(data[0]));
    ui->yGyro->setText(QString().setNum(data[1]));
    ui->zGyro->setText(QString().setNum(data[2]));
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


void MainWindow::on_elevonsRadioButton_toggled(bool checked)
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

