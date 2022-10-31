/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: trainer.h                                 //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: trainer mode                       //
/////////////////////////////////////////////////////

#ifndef TRAINER_H
#define	TRAINER_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initTrainerMode(void);
    void trainerModeCalculate(int axes);


#ifdef	__cplusplus
}
#endif

#endif	/* TRAINER_H */

