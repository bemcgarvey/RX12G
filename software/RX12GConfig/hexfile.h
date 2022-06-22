#ifndef HEXFILE_H
#define HEXFILE_H

#include <QString>
#include <QFile>

class HexRecord
{
public:
    HexRecord(char *asciiRecord);
    uint8_t* toBinary();
    uint8_t recType();
    uint32_t address();
    uint8_t dataLength();
    int recLength();
    uint8_t* data();
    bool isValid() {return m_valid;}
    enum {HEX_DATA = 0, HEX_EOF = 1, HEX_SEG_ADDRESS = 2, HEX_LIN_ADDRESS = 4, HEX_INVALID = 0xff};
private:
    uint8_t m_binary[260];
    bool m_valid;
    static uint8_t hexCharToInt(char c);
};

class HexFile
{
public:
    HexFile();
    static std::unique_ptr<QFile> hexToBinFile(QString hexFileName, uint32_t &startAddress, QString binFileName = "");
};

#endif // HEXFILE_H
