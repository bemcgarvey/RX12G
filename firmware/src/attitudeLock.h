/* 
 * File:   attitudeLock.h
 * Author: bemcg
 *
 * Created on June 24, 2022, 5:26 PM
 */

#ifndef ATTITUDELOCK_H
#define	ATTITUDELOCK_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initAttitudeLock(void);
    void attitudeLockCalculate(int axes);


#ifdef	__cplusplus
}
#endif

#endif	/* ATTITUDELOCK_H */

