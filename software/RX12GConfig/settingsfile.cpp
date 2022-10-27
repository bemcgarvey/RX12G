#include "settingsfile.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "version.h"

SettingsFile::SettingsFile(Settings *settings)
    : m_settings(settings)
{

}

bool SettingsFile::save(QString fileName)
{
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    QJsonObject json;
    json["fileVersion"] = static_cast<double>(Version::fileVersion);
    json["outputHz"] = m_settings->outputHz;
    json["sBusPeriodMs"] = m_settings->sBusPeriodMs;
    json["numSBusOutputs"] = m_settings->numSBusOutputs;
    json["failsafeType"] = m_settings->failsafeType;
    json["rxOnly"] = m_settings->rxOnly;
    json["channelOrder"] = m_settings->channelOrder;
    json["satType"] = m_settings->satType;
    json["gyroOrientation"] = m_settings->gyroOrientation;
    json["wingType"] = m_settings->wingType;
    json["tailType"] = m_settings->tailType;
    json["gyroEnabledFlags"] = m_settings->gyroEnabledFlags;
    json["gyroReverseFlags"] = m_settings->gyroReverseFlags;
    json["numFlightModes"] = m_settings->numFlightModes;
    json["flightModeChannel"] = m_settings->flightModeChannel;
    QJsonArray modes;
    for (int i = 0; i < 6; ++i) {
        modes.push_back(m_settings->flightModes[i]);
    }
    json["flightModes"] = modes;
    QJsonArray dbs;
    for (int i = 0; i < 3; ++i) {
        dbs.push_back(m_settings->deadbands[i]);
    }
    json["deadbands"] = dbs;
    QJsonArray cm1;
    cm1.push_back(m_settings->customMode1.aileronMode);
    cm1.push_back(m_settings->customMode1.elevatorMode);
    cm1.push_back(m_settings->customMode1.rudderMode);
    json["customMode1"] = cm1;
    QJsonArray cm2;
    cm2.push_back(m_settings->customMode2.aileronMode);
    cm2.push_back(m_settings->customMode2.elevatorMode);
    cm2.push_back(m_settings->customMode2.rudderMode);
    json["customMode2"] = cm2;
    QJsonArray ng;
    for (int i = 0; i < 3; ++i) {
        ng.push_back(m_settings->normalGains[i]);
    }
    json["normalGains"] = ng;
    QJsonArray lg;
    for (int i = 0; i < 2; ++i) {
        lg.push_back(m_settings->levelGains[i]);
    }
    json["levelGains"] = lg;
    QJsonArray lkg;
    for (int i = 0; i < 3; ++i) {
        lkg.push_back(m_settings->lockGains[i]);
    }
    json["lockGains"] = lkg;
    QJsonArray gc;
    for (int i = 0; i < 3; ++i) {
        gc.push_back(m_settings->gainChannels[i]);
    }
    json["gainChannels"] = gc;
    QJsonArray gcv;
    for (int i = 0; i < 3; ++i) {
        gcv.push_back(m_settings->gainCurves[i]);
    }
    json["gainCurves"] = gcv;
    json["rollLimit"] = m_settings->rollLimit;
    json["pitchLimit"] = m_settings->pitchLimit;
    json["takeoffPitch"] = m_settings->takeoffPitch;
    QJsonArray lo;
    for (int i = 0; i < 3; ++i) {
        lo.push_back(m_settings->levelOffsets[i]);
    }
    json["levelOffsets"] = lo;
    json["aileron2Channel"] = m_settings->aileron2Channel;
    json["elevator2Channel"] = m_settings->elevator2Channel;
    QJsonArray minTL;
    QJsonArray maxTL;
    for (int i = 0; i < 5; ++i) {
        minTL.push_back(m_settings->minTravelLimits[i]);
        maxTL.push_back(m_settings->maxTravelLimits[i]);
    }
    json["minTravelLimits"] = minTL;
    json["maxTravelLimits"] = maxTL;
    json["maxRollRate"] = m_settings->maxRollRate;
    json["maxPitchRate"] = m_settings->maxPitchRate;
    json["maxYawRate"] = m_settings->maxYawRate;
    QJsonObject pid;
    pid["P"] = m_settings->rollPID._P;
    pid["I"] = m_settings->rollPID._I;
    pid["D"] = m_settings->rollPID._D;
    pid["maxI"] = m_settings->rollPID._maxI;
    json["rollPID"] = pid;
    pid["P"] = m_settings->pitchPID._P;
    pid["I"] = m_settings->pitchPID._I;
    pid["D"] = m_settings->pitchPID._D;
    pid["maxI"] = m_settings->pitchPID._maxI;
    json["pitchPID"] = pid;
    pid["P"] = m_settings->yawPID._P;
    pid["I"] = m_settings->yawPID._I;
    pid["D"] = m_settings->yawPID._D;
    pid["maxI"] = m_settings->yawPID._maxI;
    json["yawPID"] = pid;
    saveFile.write(QJsonDocument(json).toJson());
    return true;
}

bool SettingsFile::load(QString fileName)
{
    QFile loadFile(fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    memset(m_settings, 0, sizeof(Settings));
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject json = loadDoc.object();
    int fileVersion = json["fileVersion"].toInt();
    Q_UNUSED(fileVersion);
    m_settings->outputHz = json["outputHz"].toInt(50);
    m_settings->sBusPeriodMs = json["sBusPeriodMs"].toInt(7);
    m_settings->numSBusOutputs = json["numSBusOutputs"].toInt();
    m_settings->failsafeType = json["failsafeType"].toInt();
    m_settings->rxOnly = json["rxOnly"].toInt();
    m_settings->channelOrder = json["channelOrder"].toInt(CHANNEL_ORDER_TAER);
    m_settings->satType = json["satType"].toInt(SAT_TYPE_DSMX);
    m_settings->gyroOrientation = json["gyroOrientation"].toInt();
    m_settings->wingType = json["wingType"].toInt();
    m_settings->tailType = json["tailType"].toInt();
    m_settings->gyroEnabledFlags = json["gyroEnabledFlags"].toInt();
    m_settings->gyroReverseFlags = json["gyroReverseFlags"].toInt();
    m_settings->numFlightModes = json["numFlightModes"].toInt();
    m_settings->flightModeChannel = json["flightModeChannel"].toInt();
    QJsonArray arr;
    arr = json["flightModes"].toArray();
    for (int i = 0; i < 6; ++i) {
        m_settings->flightModes[i] = arr[i].toInt();
    }
    arr = json["deadbands"].toArray();
    for (int i = 0; i < 3; ++i) {
        m_settings->deadbands[i] = arr[i].toInt();
    }
    arr = json["customMode1"].toArray();
    m_settings->customMode1.aileronMode = arr[0].toInt();
    m_settings->customMode1.elevatorMode = arr[1].toInt();
    m_settings->customMode1.rudderMode = arr[2].toInt();
    arr = json["customMode2"].toArray();
    m_settings->customMode2.aileronMode = arr[0].toInt();
    m_settings->customMode2.elevatorMode = arr[1].toInt();
    m_settings->customMode2.rudderMode = arr[2].toInt();
    arr = json["normalGains"].toArray();
    for (int i = 0; i < 3; ++i) {
        m_settings->normalGains[i] = arr[i].toInt();
    }
    arr = json["levelGains"].toArray();
    for (int i = 0; i < 2; ++i) {
        m_settings->levelGains[i] = arr[i].toInt();
    }
    arr = json["lockGains"].toArray();
    for (int i = 0; i < 3; ++i) {
        m_settings->lockGains[i] = arr[i].toInt();
    }
    arr = json["gainChannels"].toArray();
    for (int i = 0; i < 3; ++i) {
        m_settings->gainChannels[i] = arr[i].toInt();
    }
    arr = json["gainCurves"].toArray();
    for (int i = 0; i < 3; ++i) {
        m_settings->gainCurves[i] = arr[i].toInt();
    }
    m_settings->rollLimit = json["rollLimit"].toInt(90);
    m_settings->pitchLimit = json["pitchLimit"].toInt(90);
    m_settings->takeoffPitch = json["takeoffPitch"].toInt();
    arr = json["levelOffsets"].toArray();
    for (int i = 0; i < 3; ++i) {
        m_settings->levelOffsets[i] = arr[i].toInt();
    }
    m_settings->aileron2Channel = json["aileron2Channel"].toInt();
    m_settings->elevator2Channel = json["elevator2Channel"].toInt();
    arr = json["minTravelLimits"].toArray();
    for (int i = 0; i < 5; ++i) {
        m_settings->minTravelLimits[i] = arr[i].toInt();
    }
    arr = json["maxTravelLimits"].toArray();
    for (int i = 0; i < 5; ++i) {
        m_settings->maxTravelLimits[i] = arr[i].toInt(2047);
    }
    m_settings->maxRollRate = json["maxRollRate"].toInt(90);
    m_settings->maxPitchRate = json["maxPitchRate"].toInt(90);
    m_settings->maxYawRate = json["maxYawRate"].toInt(90);
    QJsonObject pid;
    pid = json["rollPID"].toObject();
    m_settings->rollPID._P = pid["P"].toDouble();
    m_settings->rollPID._I = pid["I"].toDouble();
    m_settings->rollPID._D = pid["D"].toDouble();
    m_settings->rollPID._maxI = pid["maxI"].toDouble();
    pid = json["pitchPID"].toObject();
    m_settings->pitchPID._P = pid["P"].toDouble();
    m_settings->pitchPID._I = pid["I"].toDouble();
    m_settings->pitchPID._D = pid["D"].toDouble();
    m_settings->pitchPID._maxI = pid["maxI"].toDouble();
    pid = json["yawPID"].toObject();
    m_settings->yawPID._P = pid["P"].toDouble();
    m_settings->yawPID._I = pid["I"].toDouble();
    m_settings->yawPID._D = pid["D"].toDouble();
    m_settings->yawPID._maxI = pid["maxI"].toDouble();
    return true;
}
