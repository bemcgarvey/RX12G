#include "hexfile.h"
#include <QTemporaryFile>

HexRecord::HexRecord(char *asciiRecord) : m_binary{0}, m_valid(false)
{
    int pos = 0;
    if (*asciiRecord != ':') {
        m_valid = false;
        m_binary[0] = 0; //set length to 0
        return;
    }
    ++asciiRecord;
    while (*asciiRecord) {
        if (*asciiRecord == '\n' || pos >= (int)sizeof(m_binary)) {
            break;
        }
        m_binary[pos] = hexCharToInt(*asciiRecord++) << 4;
        m_binary[pos++] += hexCharToInt(*asciiRecord++);
    }
    if (*asciiRecord == '\n') {
        m_valid = true;
    }
}

uint8_t *HexRecord::toBinary()
{
    return m_binary;
}

uint8_t HexRecord::recType()
{
    if (m_valid) {
        return m_binary[3];
    } else {
        return HEX_INVALID;
    }
}

uint32_t HexRecord::address()
{
    uint32_t address = 0;
    switch(recType()) {
    case HEX_DATA:
        address = m_binary[1];
        address <<= 8;
        address += m_binary[2];
        return address;
    case HEX_LIN_ADDRESS:
        address = m_binary[4];
        address <<= 8;
        address += m_binary[5];
        address <<= 16;
        return address;
    case HEX_SEG_ADDRESS:
        address = m_binary[4];
        address <<= 8;
        address += m_binary[5];
        address <<= 4;
        return address;
    case HEX_EOF:
    default:
        return 0;
    }
}

uint8_t HexRecord::dataLength()
{
    return m_binary[0];
}

int HexRecord::recLength()
{
    return m_binary[0] + 5;
}

uint8_t *HexRecord::data()
{
    return &m_binary[4];
}

uint8_t HexRecord::hexCharToInt(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    return 0;
}

HexFile::HexFile()
{

}

std::unique_ptr<QFile> HexFile::hexToBinFile(QString hexFileName, uint32_t &startAddress, QString binFileName)
{
    std::unique_ptr<QFile> hexFile(new QFile(hexFileName));
    if (!hexFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return nullptr;
    }
    std::unique_ptr<QFile> binFile;
    if (binFileName == "") {
        binFile.reset(new QTemporaryFile());
    } else {
        binFile.reset(new QFile(binFileName));
    }
    if (!binFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return nullptr;
    }
    uint32_t currentAddress = 0;
    uint32_t recordAddress = 0;
    uint32_t linAddress = 0;
    uint32_t segAddress = 0;
    char lineBuffer[265];
    bool firstDataRecProcessed = false;
    while (hexFile->readLine(lineBuffer, 265) > 0) {
        HexRecord rec(lineBuffer);
        if (!rec.isValid()) {
            continue;
        }
        switch(rec.recType()) {
        case HexRecord::HEX_LIN_ADDRESS:
            linAddress = rec.address();
            break;
        case HexRecord::HEX_SEG_ADDRESS:
            segAddress = rec.address();
            break;
        case HexRecord::HEX_DATA:
            recordAddress = rec.address();
            recordAddress += segAddress + linAddress;
            if (!firstDataRecProcessed) {
                startAddress = recordAddress;
                firstDataRecProcessed = true;
            }
            if (recordAddress != currentAddress) {
                if (currentAddress != 0) {
                    for (uint32_t i = currentAddress; i < recordAddress; ++i) {
                        char filler = 0xff;
                        binFile->write(&filler, 1);
                    }
                }
            }
            currentAddress = recordAddress;
            binFile->write((char *)rec.data(), rec.dataLength());
            currentAddress += rec.dataLength();
            break;
        case HexRecord::HEX_EOF:
            binFile->close();
            return binFile;
        }
    }
    //Should never get here unless EOF record is missing.
    //In that case the hex file is probably invalid
    binFile->close();
    return nullptr;
}
