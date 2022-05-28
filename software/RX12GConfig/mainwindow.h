#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "QHidWatcher.h"
#include "HidUSBLink.h"

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

private slots:
    void onUsbConnected();
    void onUsbRemoved();
    void on_loadPushButton_clicked();
    void on_connectPushButton_clicked();
    void on_savePushButton_clicked();
    void on_actionExit_triggered();
    void on_sbusEnableCheckBox_stateChanged(int arg1);
};

#endif // MAINWINDOW_H
