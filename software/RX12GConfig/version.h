#ifndef VERSION_H
#define VERSION_H

#include <QString>

class Version
{
public:
    Version() = delete;
    static const int clientMajorVersion = 1;
    static const int clientMinorVersion = 2;
    static const QString clientVersionString(void);
    static int firmwareMajorVersion;
    static int firmwareMinorVersion;
    static const QString firmwareVersionString(void);
    static const unsigned int fileVersion = 1;
};

#endif // VERSION_H
