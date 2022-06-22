#ifndef HIDBOOTLOADER_H
#define HIDBOOTLOADER_H

#include "bootloaderusblink.h"
#include "bootloader.h"
#include <QList>
#include <QFile>

typedef struct {
    uint32_t startAddress;
    uint32_t length;
    uint16_t crc;
} FlashRegion;

class HidBootloader : public Bootloader
{
public:
    HidBootloader(uint16_t vid, uint16_t pid);
    virtual bool isConnected() override;
    virtual bool setFile(QString fileName) override;
    virtual int readBootInfo() override;
    virtual bool eraseFlash() override;
    virtual bool programFlash() override;
    virtual void jumpToApp() override;
    virtual bool verify() override;
private:
    enum {READ_BOOT_INFO = 1, ERASE_FLASH, PROGRAM_FLASH, READ_CRC, JMP_TO_APP};
    enum {SOH = 0x01, EOT = 0x04, DLE = 0x10};
    int processOutput(void);
    int processInput(void);
    uint8_t m_transferBuffer[512];
    uint8_t m_processedBuffer[512];
    int m_bufferLen;
    std::unique_ptr<BootLoaderUSBLink> m_link;
    std::unique_ptr<QFile> m_hexFile;
    uint16_t readCRC(uint32_t address, uint32_t len);
    uint16_t calculateCRC(uint8_t *data, uint32_t len, uint16_t crc = 0);
    uint16_t m_sectionCRC;
    uint32_t m_sectionStartAddress;
    uint32_t m_currentAddress;
    uint32_t m_linAddress;
    uint32_t m_segAddress;
    QList<FlashRegion> m_regionList;
};

#endif // HIDBOOTLOADER_H
