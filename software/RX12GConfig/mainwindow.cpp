#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), usb(64, false)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectLabel = new QLabel("Not connected");
    ui->statusbar->addPermanentWidget(connectLabel);
    hidWatcher = new QHidWatcher(PID, VID, this);
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
    memset(&settings, 0, sizeof(Settings));
}

MainWindow::~MainWindow()
{
    delete ui;
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
    channelsTimer->stop();
    for (int i = 0; i < 12; ++i) {
        channelBars[i]->setValue(0);
        channelBars[i]->setEnabled(false);
    }
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
    setRxTabControls();
    setPlaneTabControls();
    setGyroTabControls();
    ui->takeoffPitchSpinBox->setValue(settings.takeoffPitch);
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
        QString version = QString("Firmware Version %1.%2").arg(buffer[1]).arg(buffer[0]);
        connectLabel->setText("Connected: " + version);
        ui->loadPushButton->setEnabled(true);
        ui->savePushButton->setEnabled(true);
        on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
    } else {
        connectLabel->setText("Not connected");
        ui->loadPushButton->setEnabled(false);
        ui->savePushButton->setEnabled(false);
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
    getRxTabControls();
    getPlaneTabControls();
    getGyroTabControls();
    settings.takeoffPitch = ui->takeoffPitchSpinBox->value();
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
    buffer[0] = GET_CHANNELS;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    uint16_t *channels = (uint16_t *)buffer;
    for (int i = 0; i < 12; ++i) {
        if (channels[i] == 0xffff) {
            channelBars[i]->setEnabled(false);
            channelBars[i]->setValue(0);
        } else {
            channelBars[i]->setEnabled(true);
            channelBars[i]->setValue(channels[i]);
        }
    }
}


void MainWindow::on_savePresetsPushButton_clicked()
{
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
    if (ui->tabWidget->tabText(index) == "Receiver") {
        channelsTimer->start(100);
    } else if (ui->tabWidget->tabText(index) == "Sensors") {
        sensorTimer->start(100);
    }
}

void MainWindow::onSensorTimout()
{
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

