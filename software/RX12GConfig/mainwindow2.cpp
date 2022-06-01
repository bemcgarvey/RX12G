#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"

void MainWindow::getRxTabControls() {
    if (ui->sbusEnableCheckBox->isChecked()) {
        settings.numSBusOutputs = ui->sbusOutputsSpinBox->value();
        settings.sBusPeriodMs = ui->sbusPeriodSpinBox->value();
    } else {
        settings.numSBusOutputs = 0;
        settings.sBusPeriodMs = 7;
    }
    switch (ui->servoRateComboBox->currentIndex()) {
        case 0: settings.outputHz = 50;
        break;
    case 1: settings.outputHz = 100;
        break;
    case 2: settings.outputHz = 200;
        break;
    default: settings.outputHz = 50;
        break;
    }
    if (ui->normalFailsafeRadioButton->isChecked()) {
        settings.failsafeType = NORMAL_FAILSAFE;
    } else {
        settings.failsafeType = PRESET_FAILSAFE;
    }
}

void MainWindow::setRxTabControls() {
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
    if (settings.failsafeType == NORMAL_FAILSAFE) {
        ui->normalFailsafeRadioButton->setChecked(true);
        ui->presetFailsafeRadioButton->setChecked(false);
        ui->savePresetsPushButton->setEnabled(false);
    } else {
        ui->normalFailsafeRadioButton->setChecked(false);
        ui->presetFailsafeRadioButton->setChecked(true);
        ui->savePresetsPushButton->setEnabled(true);
    }
}

void MainWindow::getPlaneTabControls() {
    if (ui->flatOrientationRadioButton->isChecked()) {
        settings.gyroOrientation = FLAT_ORIENTATION;
    } else if (ui->invertedOrientationRadioButton->isChecked()) {
        settings.gyroOrientation = INVERTED_ORIENTATION;
    } else if (ui->leftOrientationRadioButton->isChecked()) {
        settings.gyroOrientation = LEFT_DOWN_ORIENTATION;
    } else if (ui->rightOrientationRadioButton->isChecked()) {
            settings.gyroOrientation = RIGHT_DOWN_ORIENTATION;
    }
    if (ui->oneAileronRadioButton->isChecked()) {
        settings.wingType = ONE_AILERON;
    } else if (ui->twoAileronsRadioButton->isChecked()) {
        settings.wingType = TWO_AILERON;
    } else if (ui->elevonsRadioButton->isChecked()) {
        settings.wingType = ELEVON;
    }
    if (ui->oneElevatorRadioButton->isChecked()) {
        settings.tailType = ONE_ELEVATOR;
    } else if (ui->twoElevatorRadioButton->isChecked()) {
        settings.tailType = TWO_ELEVATOR;
    } else if (ui->vtailRadioButton->isChecked()) {
        settings.tailType = V_TAIL;
    }
}

void MainWindow::setPlaneTabControls() {
    switch(settings.gyroOrientation) {
    case FLAT_ORIENTATION:
        ui->flatOrientationRadioButton->setChecked(true);
        break;
    case INVERTED_ORIENTATION:
        ui->invertedOrientationRadioButton->setChecked(true);
        break;
    case LEFT_DOWN_ORIENTATION:
        ui->leftOrientationRadioButton->setChecked(true);
        break;
    case RIGHT_DOWN_ORIENTATION:
        ui->rightOrientationRadioButton->setChecked(true);
    }
    switch(settings.wingType) {
    case ONE_AILERON:
        ui->oneAileronRadioButton->setChecked(true);
        break;
    case TWO_AILERON:
        ui->twoAileronsRadioButton->setChecked(true);
        break;
    case ELEVON:
        ui->elevonsRadioButton->setChecked(true);
        break;
    }
    switch (settings.tailType) {
    case ONE_ELEVATOR:
        ui->oneElevatorRadioButton->setChecked(true);
        break;
    case TWO_ELEVATOR:
        ui->twoElevatorRadioButton->setChecked(true);
        break;
    case V_TAIL:
        ui->vtailRadioButton->setChecked(true);
        break;
    }
}

void MainWindow::getGyroTabControls() {

}

void MainWindow::setGyroTabControls() {

}
