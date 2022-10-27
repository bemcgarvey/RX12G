/* 
 * File:   sbus.h
 * Author: bemcg
 *
 * Created on May 24, 2022, 4:36 PM
 */

#ifndef SBUS_H
#define	SBUS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SBUS_HEADER     0x0f
#define SBUS_FOOTER     0x00
#define SBUS_FAILSAFE   0x08
    
    void initSBus(void);
    void processSBusPacket(uint8_t *buffer);

#ifdef	__cplusplus
}
#endif

#endif	/* SBUS_H */

