#ifndef QHIDWATCHER_H
#define QHIDWATCHER_H

#include <QWidget>
#include <QString>
#include <QThread>
#include <stdint.h>


#define MY_VID             0x4d63

class QHidWatcher : public QWidget
{
    Q_OBJECT
public:
    explicit QHidWatcher(uint16_t pid, uint16_t vid = MY_VID);
    ~QHidWatcher(void);
signals:
    void connected(void);
    void removed(void);

protected:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    QString pid;
    QString vid;
    void *DeviceNotificationHandle;
};

#endif // QHIDWATCHER_H
