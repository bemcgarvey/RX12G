#include "bootloader.h"

Bootloader::Bootloader() : m_abort(false)
{

}

int Bootloader::readBootInfo()
{
    return 0;
}

bool Bootloader::eraseFlash()
{
    //Override this if flash needs to be erased before programming.
    //If flash is erased while programming pages leave as is.
    return true;
}

void Bootloader::abort()
{
    m_abort = true;
}

