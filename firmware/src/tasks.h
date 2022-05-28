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
    
    enum {START_NORMAL = 0, START_USB, START_BOR, START_WDT};
    
    extern int startMode;
    
    void initQueues(void);
    void initTasks(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TASKS_H */

