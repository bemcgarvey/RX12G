/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: tasks.h                                   //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: task creation and scheduler start  //
/////////////////////////////////////////////////////

#ifndef TASKS_H
#define	TASKS_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initQueues(void);
    void initTasks(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TASKS_H */

