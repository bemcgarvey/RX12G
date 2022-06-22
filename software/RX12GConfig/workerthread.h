#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include "bootloader.h"

class WorkerThread : public QThread
{
public:
    explicit WorkerThread(Bootloader *boot);

protected:
    virtual void run() override;
private:
    Bootloader *bootloader;
};

#endif // WORKERTHREAD_H
