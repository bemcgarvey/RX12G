#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RX12G.h"
#include <QFile>
#include "version.h"
#include <QMessageBox>

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
    settings.gains[AILERON_INDEX] = ui->aileronGainSpinBox->value();
    settings.gains[ELEVATOR_INDEX] = ui->elevatorGainSpinBox->value();
    settings.gains[RUDDER_INDEX] = ui->rudderGainSpinBox->value();
    settings.gainCurves[AILERON_INDEX] = ui->aileronGainCurveComboBox->currentIndex();
    settings.gainCurves[ELEVATOR_INDEX] = ui->elevatorGainCurveComboBox->currentIndex();
    settings.gainCurves[RUDDER_INDEX] = ui->rudderGainCurveComboBox->currentIndex();
    index = ui->aileronGainChannelComboBox->currentIndex();
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
    ui->aileronGainSpinBox->setValue(settings.gains[AILERON_INDEX]);
    ui->elevatorGainSpinBox->setValue(settings.gains[ELEVATOR_INDEX]);
    ui->rudderGainSpinBox->setValue(settings.gains[RUDDER_INDEX]);
    ui->aileronGainCurveComboBox->setCurrentIndex(settings.gainCurves[AILERON_INDEX]);
    ui->elevatorGainCurveComboBox->setCurrentIndex(settings.gainCurves[ELEVATOR_INDEX]);
    ui->rudderGainCurveComboBox->setCurrentIndex(settings.gainCurves[RUDDER_INDEX]);
    index = settings.gainChannels[AILERON_INDEX];
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

void MainWindow::getControls()
{
    getRxTabControls();
    getPlaneTabControls();
    getGyroTabControls();
    getLimitsTabControls();
    getTuningTabControls();
}

void MainWindow::setControls()
{
    setRxTabControls();
    setGyroTabControls();
    setLimitsTabControls();
    setTuningTabControls();
    setPlaneTabControls();
}

bool MainWindow::openFile(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        unsigned int magic;
        file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
        if (magic != Version::fileMagicNumber) {
            return false;
        }
        unsigned int version;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        file.read(reinterpret_cast<char *>(&settings), sizeof(Settings));
        file.close();
        return true;
    } else {
        return false;
    }
}

bool MainWindow::saveFile(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        unsigned int magic = Version::fileMagicNumber;
        unsigned int fileVersion = Version::fileVersion;
        file.write(reinterpret_cast<const char *>(&magic), sizeof(magic));
        file.write(reinterpret_cast<const char *>(&fileVersion), sizeof(fileVersion));
        file.write(reinterpret_cast<const char *>(&settings), sizeof(Settings));
        file.close();
        return true;
    } else {
        return false;
    }
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
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 6; ++j) {
            if (assignedChannels[i] == 0) {
                continue;
            }
            if (assignedChannels[i] == assignedChannels[j]) {
                ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->gyroTab));
                if (QMessageBox::warning(this, QApplication::applicationName(),
                    "You have multiple functions assigned to the same channel. Are you sure this is what you want?",
                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
                        return false;
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
    newOffsets[0] *= 512.0 / 1000.0;
    newOffsets[1] = ((levelSampleSums[1] / counts) * mg_PER_LSB);
    newOffsets[1] *= 512.0 / 1000.0;
    newOffsets[2] = ((levelSampleSums[2] / counts) * mg_PER_LSB);
    newOffsets[2] = 1000 - newOffsets[2];
    newOffsets[2] *= -512.0 / 1000.0;
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
