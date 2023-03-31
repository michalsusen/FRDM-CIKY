/*
 * ciky.h
 *
 *  Created on: 24. 3. 2023
 *      Author: Mike
 */

#ifndef CIKY_H_
#define CIKY_H_

#include "fsl_ftm.h"
#include "fsl_adc16.h"

typedef enum {
  A0,
  A1,
  A2,
  A3
} sensors;

/* The Flextimer instance/channel used for board */
#define BOARD_FTM_BASEADDR       FTM0
#define LEFT_FTM_CHANNEL  0U
#define RIGHT_FTM_CHANNEL 1U
/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#ifndef FTM_PWM_ON_LEVEL
#define FTM_PWM_ON_LEVEL kFTM_HighTrue
#endif
#ifndef PWM_FREQUENCY
#define PWM_FREQUENCY (500U)
#endif


#define ADC16_BASE0          ADC0
#define ADC16_BASE1          ADC1
#define ADC16_CHANNEL_GROUP0 0U
#define ADC16_A3  12
#define ADC16_A2  13
#define ADC16_A1  14
#define ADC16_A0  15


void CIKY_InitSensors(void);
void CIKY_InitMotors(void);
void CIKY_InitUltrasonic(void);


int analogRead(sensors sensor);



long map(long x, long in_min, long in_max, long out_min, long out_max);
void setMotors(uint8_t mLp,  uint8_t mRp);




#endif /* CIKY_H_ */
