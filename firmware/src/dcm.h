/* 
 * File:   dcm.h
 * Author: bemcg
 *
 * Created on October 4, 2022, 4:51 PM
 */

#ifndef DCM_H
#define	DCM_H

#ifdef	__cplusplus
extern "C" {
#endif

  void dcmInit(void);
  void dcmUpdate(float dt, float gx, float gy, float gz, float ax, float ay, float az);
  float dcmGetRoll();
  float dcmGetPitch();
  float dcmGetYaw();

#ifdef	__cplusplus
}
#endif

#endif	/* DCM_H */

