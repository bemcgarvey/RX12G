#ifndef _BOOTLOADERUSB_LINK
#define _BOOTLOADERUSB_LINK

#include <stdint.h>
#include <QString>

#define MY_VID             0x4d63

class BootLoaderUSBLink
{
public:
    BootLoaderUSBLink();
    BootLoaderUSBLink(const BootLoaderUSBLink &obj) = delete;
    ~BootLoaderUSBLink();
    BootLoaderUSBLink& operator=(const BootLoaderUSBLink &obj) = delete;
    void Open(uint16_t pid, uint16_t vid = MY_VID);
    bool WriteDevice(uint8_t *buffer, int len, int wait_ms = 200);
    bool ReadDevice(uint8_t *buffer, int wait_ms = 200);
	bool Connected(void);
	void Close(void);
    QString getDevicePath() const;
private:
    void *handle;
    void closeHandles(void);
    QString devicePath;
};

#endif
