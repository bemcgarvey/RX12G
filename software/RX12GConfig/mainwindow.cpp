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
}

void MainWindow::on_loadPushButton_clicked()
{
    buffer[0] = GET_SETTINGS;
    usb.SendReport(buffer);
    usb.GetReport(buffer);
    Settings settings;
    memcpy(&settings, buffer, sizeof(Settings));
    if (settings.numSBusOutputs > 0) {
        ui->sbusEnableCheckBox->setChecked(true);
    } else {
        ui->sbusEnableCheckBox->setChecked(false);
    }
    ui->sbusOutputsSpinBox->setValue(settings.numSBusOutputs);
    ui->sbusPeriodSpinBox->setValue(settings.sBusPeriodMs);
    switch(settings.outputHz) {
    case 50:
        ui->servoRateComboBox->setCurrentIndex(0);
        break;
    case 100:
        ui->servoRateComboBox->setCurrentIndex(1);
        break;
    case 200:
        ui->servoRateComboBox->setCurrentIndex(2);
        break;
    }
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
    } else {
        connectLabel->setText("Not connected");
        ui->loadPushButton->setEnabled(false);
        ui->savePushButton->setEnabled(false);
    }
}


void MainWindow::on_savePushButton_clicked()
{

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

