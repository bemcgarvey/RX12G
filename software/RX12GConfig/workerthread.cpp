#include "workerthread.h"

WorkerThread::WorkerThread(Bootloader *boot) : QThread(), bootloader(boot)
{

}


void WorkerThread::run()
{
    bool success;
    success = bootloader->eraseFlash();
    if (!success || bootloader->isAborted()) {
        return;
    }
    success = bootloader->programFlash();
    if (!success || bootloader->isAborted()) {
        return;
    }
    success = bootloader->verify();
    if (success) {
        bootloader->jumpToApp();
    }
}
