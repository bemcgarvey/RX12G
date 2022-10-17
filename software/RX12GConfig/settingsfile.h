#ifndef SETTINGSFILE_H
#define SETTINGSFILE_H

#include <QString>
#include "RX12G.h"

class SettingsFile
{
public:
    SettingsFile(Settings *settings);
    bool save(QString fileName);
    bool load(QString fileName);
private:
    Settings *m_settings;
};

#endif // SETTINGSFILE_H
