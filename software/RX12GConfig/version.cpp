#include "version.h"

int Version::firmwareMajorVersion = -1;
int Version::firmwareMinorVersion = 0;


const QString Version::clientVersionString()
{
    return QString("%1.%2").arg(clientMajorVersion).arg(clientMinorVersion);
}

const QString Version::firmwareVersionString()
{
    if (firmwareMajorVersion >= 0)
        return QString("%1.%2").arg(firmwareMajorVersion).arg(firmwareMinorVersion);
    else
        return QString("Unknown");
}
