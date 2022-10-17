#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"
#include <QFile>
#include "version.h"
#include <QMessageBox>
#include "settingsfile.h"

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
    if (ui->rxOnlyCheckBox->isChecked()) {
        settings.rxOnly = RX_ONLY_MODE;
    } else {
        settings.rxOnly = NORMAL_RX_MODE;
    }
    if (ui->aetrRadioButton->isChecked()) {
        settings.channelOrder = CHANNEL_ORDER_AETR;
    } else {
        settings.channelOrder = CHANNEL_ORDER_TAER;
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
    if (settings.rxOnly == RX_ONLY_MODE) {
        ui->rxOnlyCheckBox->setChecked(true);
    } else {
        ui->rxOnlyCheckBox->setChecked(false);
    }
    if (settings.channelOrder == CHANNEL_ORDER_AETR) {
        ui->aetrRadioButton->setChecked(true);
    } else {
        ui->taerRadioButton->setChecked(true);
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
    } else if (ui->elevon_ARadioButton->isChecked()) {
        settings.wingType = ELEVON_A;
    } else if (ui->elevon_BRadioButton->isChecked()) {
        settings.wingType = ELEVON_B;
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
    case ELEVON_A:
        ui->elevon_ARadioButton->setChecked(true);
        break;
    case ELEVON_B:
        ui->elevon_BRadioButton->setChecked(true);
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
    settings.gyroEnabledFlags = 0;
    if (ui->enableAileronCheckBox->isChecked()) {
        settings.gyroEnabledFlags |= AILERON_MASK;
    }
    if (ui->enableElevatorCheckBox->isChecked()) {
        settings.gyroEnabledFlags |= ELEVATOR_MASK;
    }
    if (ui->enableRudderCheckBox->isChecked()) {
        settings.gyroEnabledFlags |= RUDDER_MASK;
    }
    if (ui->enableAileron2CheckBox->isChecked()) {
        settings.gyroEnabledFlags |= AILERON2_MASK;
    }
    if (ui->enableElevator2CheckBox->isChecked()) {
        settings.gyroEnabledFlags |= ELEVATOR2_MASK;
    }
    settings.gyroReverseFlags = 0;
    if (ui->reverseAileronCheckBox->isChecked()) {
        settings.gyroReverseFlags |= AILERON_MASK;
    }
    if (ui->reverseElevatorCheckBox->isChecked()) {
        settings.gyroReverseFlags |= ELEVATOR_MASK;
    }
    if (ui->reverseRudderCheckBox->isChecked()) {
        settings.gyroReverseFlags |= RUDDER_MASK;
    }
    if (ui->reverseAileron2CheckBox->isChecked()) {
        settings.gyroReverseFlags |= AILERON2_MASK;
    }
    if (ui->reverseElevator2CheckBox->isChecked()) {
        settings.gyroReverseFlags |= ELEVATOR2_MASK;
    }
    settings.deadbands[AILERON_INDEX] = ui->aileronDeadbandSpinBox->value();
    settings.deadbands[ELEVATOR_INDEX] = ui->elevatorDeadbandSpinBox->value();
    settings.deadbands[RUDDER_INDEX] = ui->rudderDeadbandSpinBox->value();
    if (ui->threeModeRadioButton->isChecked()) {
        settings.numFlightModes = 3;
    } else if (ui->sixModeRadioButton->isChecked()) {
        settings.numFlightModes = 6;
    } else {
        settings.numFlightModes = 1;
    }
    int index = ui->modeChannelComboBox->currentIndex();
    if (index > 0) {
        index += 3;
    }
    settings.flightModeChannel = index;
    settings.flightModes[0] = ui->mode1ComboBox->currentIndex();
    settings.flightModes[1] = ui->mode2ComboBox->currentIndex();
    settings.flightModes[2] = ui->mode3ComboBox->currentIndex();
    settings.flightModes[3] = ui->mode4ComboBox->currentIndex();
    settings.flightModes[4] = ui->mode5ComboBox->currentIndex();
    settings.flightModes[5] = ui->mode6ComboBox->currentIndex();
    settings.customMode1.aileronMode = ui->cm1AileronComboBox->currentIndex();
    settings.customMode1.elevatorMode = ui->cm1ElevatorComboBox->currentIndex();
    settings.customMode1.rudderMode = ui->cm1RudderComboBox->currentIndex();
    settings.customMode2.aileronMode = ui->cm2AileronComboBox->currentIndex();
    settings.customMode2.elevatorMode = ui->cm2ElevatorComboBox->currentIndex();
    settings.customMode2.rudderMode = ui->cm2RudderComboBox->currentIndex();
    index = ui->aileron2ChannelComboBox->currentIndex();
    if (index > 0) {
        index += 3;
    }
    settings.aileron2Channel = index;
    index = ui->elevator2ChannelComboBox->currentIndex();
    if (index > 0) {
        index += 3;
    }
    settings.elevator2Channel = index;
}

void MainWindow::setGyroTabControls() {
    ui->enableAileronCheckBox->setChecked(settings.gyroEnabledFlags & AILERON_MASK);
    ui->enableElevatorCheckBox->setChecked(settings.gyroEnabledFlags & ELEVATOR_MASK);
    ui->enableRudderCheckBox->setChecked(settings.gyroEnabledFlags & RUDDER_MASK);
    ui->enableAileron2CheckBox->setChecked(settings.gyroEnabledFlags & AILERON2_MASK);
    ui->enableElevator2CheckBox->setChecked(settings.gyroEnabledFlags & ELEVATOR2_MASK);
    ui->reverseAileronCheckBox->setChecked(settings.gyroReverseFlags & AILERON_MASK);
    ui->reverseElevatorCheckBox->setChecked(settings.gyroReverseFlags & ELEVATOR_MASK);
    ui->reverseRudderCheckBox->setChecked(settings.gyroReverseFlags & RUDDER_MASK);
    ui->reverseAileron2CheckBox->setChecked(settings.gyroReverseFlags & AILERON2_MASK);
    ui->reverseElevator2CheckBox->setChecked(settings.gyroReverseFlags & ELEVATOR2_MASK);
    ui->aileronDeadbandSpinBox->setValue(settings.deadbands[AILERON_INDEX]);
    ui->elevatorDeadbandSpinBox->setValue(settings.deadbands[ELEVATOR_INDEX]);
    ui->rudderDeadbandSpinBox->setValue(settings.deadbands[RUDDER_INDEX]);
    switch(settings.numFlightModes) {
    case 1: ui->oneModeRadioButton->setChecked(true);
        on_oneModeRadioButton_clicked(true);
        break;
    case 3: ui->threeModeRadioButton->setChecked(true);
        on_threeModeRadioButton_clicked(true);
        break;
    case 6: ui->sixModeRadioButton->setChecked(true);
        on_sixModeRadioButton_clicked(true);
        break;
    default: ui->oneModeRadioButton->setChecked(true);
        on_oneModeRadioButton_clicked(true);
        break;
    }
    int index = settings.flightModeChannel;
    if (index > 0) {
        index -= 3;
    }
    ui->modeChannelComboBox->setCurrentIndex(index);
    ui->mode1ComboBox->setCurrentIndex(settings.flightModes[0]);
    ui->mode2ComboBox->setCurrentIndex(settings.flightModes[1]);
    ui->mode3ComboBox->setCurrentIndex(settings.flightModes[2]);
    ui->mode4ComboBox->setCurrentIndex(settings.flightModes[3]);
    ui->mode5ComboBox->setCurrentIndex(settings.flightModes[4]);
    ui->mode6ComboBox->setCurrentIndex(settings.flightModes[5]);
    ui->cm1AileronComboBox->setCurrentIndex(settings.customMode1.aileronMode);
    ui->cm1ElevatorComboBox->setCurrentIndex(settings.customMode1.elevatorMode);
    ui->cm1RudderComboBox->setCurrentIndex(settings.customMode1.rudderMode);
    ui->cm2AileronComboBox->setCurrentIndex(settings.customMode2.aileronMode);
    ui->cm2ElevatorComboBox->setCurrentIndex(settings.customMode2.elevatorMode);
    ui->cm2RudderComboBox->setCurrentIndex(settings.customMode2.rudderMode);
    index = settings.aileron2Channel;
    if (index > 0) {
        index -= 3;
    }
    ui->aileron2ChannelComboBox->setCurrentIndex(index);
    index = settings.elevator2Channel;
    if (index > 0) {
        index -= 3;
    }
    ui->elevator2ChannelComboBox->setCurrentIndex(index);
}

void MainWindow::getLimitsTabControls() {
    settings.takeoffPitch = ui->takeoffPitchSpinBox->value();
    settings.rollLimit = ui->rollLimitSpinBox->value();
    settings.pitchLimit = ui->pitchLimitSpinBox->value();
    settings.maxRollRate = ui->maxRollSpinBox->value();
    settings.maxPitchRate = ui->maxPitchSpinBox->value();
    settings.maxYawRate = ui->maxYawSpinBox->value();
}

void MainWindow::setLimitsTabControls() {
    ui->aileronMinMaxBar->setInitialMinMax(settings.minTravelLimits[AILERON_INDEX], settings.maxTravelLimits[AILERON_INDEX]);
    ui->elevatorMinMaxBar->setInitialMinMax(settings.minTravelLimits[ELEVATOR_INDEX], settings.maxTravelLimits[ELEVATOR_INDEX]);
    ui->rudderMinMaxBar->setInitialMinMax(settings.minTravelLimits[RUDDER_INDEX], settings.maxTravelLimits[RUDDER_INDEX]);
    ui->aileron2MinMaxBar->setInitialMinMax(settings.minTravelLimits[AILERON2_INDEX], settings.maxTravelLimits[AILERON2_INDEX]);
    ui->elevator2MinMaxBar->setInitialMinMax(settings.minTravelLimits[ELEVATOR2_INDEX], settings.maxTravelLimits[ELEVATOR2_INDEX]);
    ui->takeoffPitchSpinBox->setValue(settings.takeoffPitch);
    ui->rollLimitSpinBox->setValue(settings.rollLimit);
    ui->pitchLimitSpinBox->setValue(settings.pitchLimit);
    ui->maxRollSpinBox->setValue(settings.maxRollRate);
    ui->maxPitchSpinBox->setValue(settings.maxPitchRate);
    ui->maxYawSpinBox->setValue(settings.maxYawRate);
}

void MainWindow::getTuningTabControls()
{
    bool allValid = true;
    bool ok;
    settings.rollPID._P = ui->rollPLineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.rollPID._I = ui->rollILineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.rollPID._D = ui->rollDLineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.rollPID._maxI = ui->rollMaxILineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.pitchPID._P = ui->pitchPLineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.pitchPID._I = ui->pitchILineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.pitchPID._D = ui->pitchDLineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.pitchPID._maxI = ui->pitchMaxILineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.yawPID._P = ui->yawPLineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.yawPID._I = ui->yawILineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.yawPID._D = ui->yawDLineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    settings.yawPID._maxI = ui->yawMaxILineEdit->text().toFloat(&ok);
    allValid = allValid && ok;
    if (!allValid) {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tuningTab));
        QMessageBox::warning(this, QApplication::applicationName(), "Invalid number in a PID value.  Settings may not be valid.");
    }
}

void MainWindow::setTuningTabControls()
{
    ui->rollPLineEdit->setText(QString().setNum(settings.rollPID._P));
    ui->rollILineEdit->setText(QString().setNum(settings.rollPID._I));
    ui->rollDLineEdit->setText(QString().setNum(settings.rollPID._D));
    ui->rollMaxILineEdit->setText(QString().setNum(settings.rollPID._maxI));
    ui->pitchPLineEdit->setText(QString().setNum(settings.pitchPID._P));
    ui->pitchILineEdit->setText(QString().setNum(settings.pitchPID._I));
    ui->pitchDLineEdit->setText(QString().setNum(settings.pitchPID._D));
    ui->pitchMaxILineEdit->setText(QString().setNum(settings.pitchPID._maxI));
    ui->yawPLineEdit->setText(QString().setNum(settings.yawPID._P));
    ui->yawILineEdit->setText(QString().setNum(settings.yawPID._I));
    ui->yawDLineEdit->setText(QString().setNum(settings.yawPID._D));
    ui->yawMaxILineEdit->setText(QString().setNum(settings.yawPID._maxI));
}

void MainWindow::getGainTabControls()
{
    settings.normalGains[AILERON_INDEX] = ui->rollNormalGainSpinBox->value();
    settings.levelGains[AILERON_INDEX] = ui->rollLevelGainSpinBox->value();
    settings.lockGains[AILERON_INDEX] = ui->rollLockGainSpinBox->value();
    settings.normalGains[ELEVATOR_INDEX] = ui->pitchNormalGainSpinBox->value();
    settings.levelGains[ELEVATOR_INDEX] = ui->pitchLevelGainSpinBox->value();
    settings.lockGains[ELEVATOR_INDEX] = ui->pitchLockGainSpinBox->value();
    settings.normalGains[RUDDER_INDEX] = ui->yawNormalGainSpinBox->value();
    settings.lockGains[RUDDER_INDEX] = ui->yawLockGainSpinBox->value();
    settings.gainCurves[AILERON_INDEX] = ui->aileronGainCurveComboBox->currentIndex();
    settings.gainCurves[ELEVATOR_INDEX] = ui->elevatorGainCurveComboBox->currentIndex();
    settings.gainCurves[RUDDER_INDEX] = ui->rudderGainCurveComboBox->currentIndex();
    int index = ui->aileronGainChannelComboBox->currentIndex();
    if (index > 0) {
        index += 3;
    }
    settings.gainChannels[AILERON_INDEX] = index;
    index = ui->elevatorGainChannelComboBox->currentIndex();
    if (index > 0) {
        index += 3;
    }
    settings.gainChannels[ELEVATOR_INDEX] = index;
    index = ui->rudderGainChannelComboBox->currentIndex();
    if (index > 0) {
        index += 3;
    }
    settings.gainChannels[RUDDER_INDEX] = index;
}

void MainWindow::setGainTabControls()
{
    ui->rollNormalGainSpinBox->setValue(settings.normalGains[AILERON_INDEX]);
    ui->pitchNormalGainSpinBox->setValue(settings.normalGains[ELEVATOR_INDEX]);
    ui->yawNormalGainSpinBox->setValue(settings.normalGains[RUDDER_INDEX]);
    ui->rollLevelGainSpinBox->setValue(settings.levelGains[AILERON_INDEX]);
    ui->pitchLevelGainSpinBox->setValue(settings.levelGains[ELEVATOR_INDEX]);
    ui->rollLockGainSpinBox->setValue(settings.lockGains[AILERON_INDEX]);
    ui->pitchLockGainSpinBox->setValue(settings.lockGains[ELEVATOR_INDEX]);
    ui->yawLockGainSpinBox->setValue(settings.lockGains[RUDDER_INDEX]);
    ui->aileronGainCurveComboBox->setCurrentIndex(settings.gainCurves[AILERON_INDEX]);
    ui->elevatorGainCurveComboBox->setCurrentIndex(settings.gainCurves[ELEVATOR_INDEX]);
    ui->rudderGainCurveComboBox->setCurrentIndex(settings.gainCurves[RUDDER_INDEX]);
    int index = settings.gainChannels[AILERON_INDEX];
    if (index > 0) {
        index -= 3;
    }
    ui->aileronGainChannelComboBox->setCurrentIndex(index);
    index = settings.gainChannels[ELEVATOR_INDEX];
    if (index > 0) {
        index -= 3;
    }
    ui->elevatorGainChannelComboBox->setCurrentIndex(index);
    index = settings.gainChannels[RUDDER_INDEX];
    if (index > 0) {
        index -= 3;
    }
    ui->rudderGainChannelComboBox->setCurrentIndex(index);
}

void MainWindow::getControls()
{
    getRxTabControls();
    getPlaneTabControls();
    getGyroTabControls();
    getLimitsTabControls();
    getTuningTabControls();
    getGainTabControls();
}

void MainWindow::setControls()
{
    setRxTabControls();
    setGyroTabControls();
    setLimitsTabControls();
    setTuningTabControls();
    setPlaneTabControls();
    setGainTabControls();
}

bool MainWindow::openFile(QString fileName)
{
    QFile file(fileName);
    SettingsFile settingsFile(&settings);
    return settingsFile.load(fileName);
}

bool MainWindow::saveFile(QString fileName)
{
    SettingsFile settingsFile(&settings);
    return settingsFile.save(fileName);
}

void MainWindow::initSettings()
{
    memset(&settings, 0, sizeof(Settings));
}

bool MainWindow::controlsValid()
{
    if (ui->twoAileronsRadioButton->isChecked()) {
        if (ui->aileron2ChannelComboBox->currentIndex() == 0) {
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gyroTab));
            QMessageBox::critical(this, QApplication::applicationName(), "You must select a channel for Aileron2.");
            return false;
        }ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gyroTab));
    }
    if (ui->twoElevatorRadioButton->isChecked()) {
        if (ui->elevator2ChannelComboBox->currentIndex() == 0) {
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gyroTab));
            QMessageBox::critical(this, QApplication::applicationName(), "You must select a channel for Elevator2.");
            return false;
        }
    }
    if (!ui->oneModeRadioButton->isChecked()) {
        if (ui->modeChannelComboBox->currentIndex() == 0) {
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gyroTab));
            QMessageBox::critical(this, QApplication::applicationName(), "You have selected multiple flight modes. You must select a flight mode channel.");
            return false;
        }
    }
    int assignedChannels[6];
    assignedChannels[0] = ui->aileron2ChannelComboBox->currentIndex();
    assignedChannels[1] = ui->elevator2ChannelComboBox->currentIndex();
    assignedChannels[2] = ui->modeChannelComboBox->currentIndex();
    assignedChannels[3] = ui->aileronGainChannelComboBox->currentIndex();
    assignedChannels[4] = ui->elevatorGainChannelComboBox->currentIndex();
    assignedChannels[5] = ui->rudderGainChannelComboBox->currentIndex();
    for (int i = 0; i < 2; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            if (assignedChannels[i] == 0) {
                continue;
            }
            if (assignedChannels[i] == assignedChannels[j]) {
                ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gyroTab));
                QMessageBox::critical(this, QApplication::applicationName(),
                  "You have multiple control surfaces assigned to the same channel.");
                    return false;
                }
            }
        }
    for (int i = 0; i < 3; ++i) {
        for (int j = 3; j < 6; ++j) {
            if (assignedChannels[i] == 0) {
                continue;
            }
            if (assignedChannels[i] == assignedChannels[j]) {
                ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gainTab));
                if (QMessageBox::critical(this, QApplication::applicationName(),
                  "You have a control and a gain on the same channel.  Are you sure this is what you want?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    }
    return true;
}

bool MainWindow::calculateLevelOffsets()
{
    double newOffsets[3];
    double counts = levelSampleCount;
    newOffsets[0] = ((levelSampleSums[0] / counts) * mg_PER_LSB);
    newOffsets[0] *= -512.0 / 1000.0;
    newOffsets[1] = ((levelSampleSums[1] / counts) * mg_PER_LSB);
    newOffsets[1] *= -512.0 / 1000.0;
    newOffsets[2] = ((levelSampleSums[2] / counts) * mg_PER_LSB);
    newOffsets[2] = 1000 - newOffsets[2];
    newOffsets[2] *= 512.0 / 1000.0;
    int16_t temp;
    if (ui->flatOrientationRadioButton->isChecked()) {
        //Don't need to do anything
    } else if (ui->invertedOrientationRadioButton->isChecked()) {
        newOffsets[1] = -newOffsets[1];
        newOffsets[2] = -newOffsets[2];
    } else if (ui->leftOrientationRadioButton->isChecked()) {
        temp = newOffsets[1];
        newOffsets[1] = newOffsets[2];
        newOffsets[2] = temp;
    } else if (ui->rightOrientationRadioButton->isChecked()) {
        temp = newOffsets[1];
        newOffsets[1] = -newOffsets[2];
        newOffsets[2] = -temp;
    }
    for (int i = 0; i < 3; ++i) {
        if (round(newOffsets[i]) > 127 || round(newOffsets[i]) < -127) {
            return false;
        }
    }
    for (int i = 0; i < 3; ++i) {
        settings.levelOffsets[i] = (uint8_t)round(newOffsets[i]);
    }
    return true;
}

void MainWindow::updateChannelDisplay()
{
    if (ui->show13RadioButton->isChecked()) {
        ui->ch9label->setText("CH 13");
        ui->ch10label->setText("CH 14");
        ui->ch11label->setText("CH 15");
        ui->ch12label->setText("CH 16");
    } else if (ui->show17RadioButton->isChecked()) {
        ui->ch9label->setText("CH 17");
        ui->ch10label->setText("CH 18");
        ui->ch11label->setText("CH 19");
        ui->ch12label->setText("CH 20");
    } else {
        ui->ch9label->setText("CH 9");
        ui->ch10label->setText("CH 10");
        ui->ch11label->setText("CH 11");
        ui->ch12label->setText("CH 12");
    }
}
