#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <QObject>

class Bootloader : public QObject
{
    Q_OBJECT
public:
    Bootloader();
    virtual bool isConnected() = 0;
    virtual int readBootInfo();
    virtual bool setFile(QString fileName) = 0;
    virtual bool eraseFlash();
    virtual bool programFlash() = 0;
    virtual void jumpToApp() = 0;
    virtual bool verify() = 0;
    virtual void abort();
    bool isAborted() {return m_abort;}
    enum {PIC32 = 0, ARM = 1, OTHER = 2};
    void setFamily(int family) {m_family = family;}
protected:
    bool m_abort;
    int m_family;
signals:
    void finished(bool success);
    void progress(int p);
    void message(QString m);
};

#endif // BOOTLOADER_H
