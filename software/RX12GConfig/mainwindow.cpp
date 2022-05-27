#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), usb(64, false)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectLabel = new QLabel("Not connected");
    ui->statusbar->addWidget(connectLabel);
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
}


void MainWindow::on_loadPushButton_clicked()
{
    uint8_t buffer[64];
    buffer[0] = 0x80;
    usb.SendReport(buffer);
}


void MainWindow::on_connectPushButton_clicked()
{
    if (usb.Connected()) {
        usb.Close();
    }
    usb.Open(PID, VID);
    if (usb.Connected()) {
        connectLabel->setText("Connected");
    } else {
        connectLabel->setText("Not connected");
    }
}


void MainWindow::on_savePushButton_clicked()
{
    uint8_t buffer[64];
    buffer[0] = 0x81;
    usb.SendReport(buffer);
}

