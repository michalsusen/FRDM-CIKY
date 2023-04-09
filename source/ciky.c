/*
 * ciky.c
 *
 *  Created on: 24. 3. 2023
 *      Author: Mike
 */

#include "ciky.h"


adc16_channel_config_t adc16ChannelConfigStruct[4];

void CIKY_InitMotors(void)
{

	ftm_config_t ftmInfo;
	uint8_t updatedDutycycle;
	ftm_chnl_pwm_signal_param_t ftmParam[2];

	/* Fill in the FTM config struct with the default settings */
	FTM_GetDefaultConfig(&ftmInfo);
	/* Calculate the clock division based on the PWM frequency to be obtained */
	ftmInfo.prescale = FTM_CalculateCounterClkDiv(MOTORS_FTM_BASEADDR, MOTORS_PWM_FREQUENCY, FTM_SOURCE_CLOCK);
	/* Initialize FTM module */
	FTM_Init(MOTORS_FTM_BASEADDR, &ftmInfo);

	/* Configure ftm params with frequency 24kHZ */
	ftmParam[0].chnlNumber            = (ftm_chnl_t)LEFT_MOTOR_FTM_CHANNEL;
	ftmParam[0].level                 = FTM_PWM_ON_LEVEL;
	ftmParam[0].dutyCyclePercent      = 0U;
	ftmParam[0].firstEdgeDelayPercent = 0U;
	ftmParam[0].enableComplementary   = false;
	ftmParam[0].enableDeadtime        = false;

	ftmParam[1].chnlNumber            = (ftm_chnl_t)RIGHT_MOTOR_FTM_CHANNEL;
	ftmParam[1].level                 = FTM_PWM_ON_LEVEL;
	ftmParam[1].dutyCyclePercent      = 0U;
	ftmParam[1].firstEdgeDelayPercent = 0U;
	ftmParam[1].enableComplementary   = false;
	ftmParam[1].enableDeadtime        = false;
	if (kStatus_Success !=
		FTM_SetupPwm(MOTORS_FTM_BASEADDR, ftmParam, 2U, kFTM_EdgeAlignedPwm, MOTORS_PWM_FREQUENCY, FTM_SOURCE_CLOCK))
	{
		return -1;
	}
	FTM_StartTimer(MOTORS_FTM_BASEADDR, kFTM_SystemClock);
}

void setMotors(uint8_t mLp,  uint8_t mRp)
{

	/* Start PWM mode with updated duty cycle */
	FTM_UpdatePwmDutycycle(MOTORS_FTM_BASEADDR, (ftm_chnl_t)LEFT_MOTOR_FTM_CHANNEL,kFTM_EdgeAlignedPwm, map(mLp, 0, 255, 0, 100));
	FTM_UpdatePwmDutycycle(MOTORS_FTM_BASEADDR, (ftm_chnl_t)RIGHT_MOTOR_FTM_CHANNEL,kFTM_EdgeAlignedPwm, map(mRp, 0, 255, 0, 100));

	/* Software trigger to update registers */
	FTM_SetSoftwareTrigger(MOTORS_FTM_BASEADDR, true);

}

void CIKY_InitServo(void)
{

	ftm_config_t ftmInfo;
	uint8_t updatedDutycycle;
	ftm_chnl_pwm_signal_param_t ftmParam;

	/* Fill in the FTM config struct with the default settings */
	FTM_GetDefaultConfig(&ftmInfo);
	/* Calculate the clock division based on the PWM frequency to be obtained */
	ftmInfo.prescale = FTM_CalculateCounterClkDiv(SERVO_FTM_BASEADDR, SERVO_PWM_FREQUENCY, FTM_SOURCE_CLOCK);
	/* Initialize FTM module */
	FTM_Init(SERVO_FTM_BASEADDR, &ftmInfo);

	/* Configure ftm params with frequency 24kHZ */
	ftmParam.chnlNumber            = (ftm_chnl_t)SERVO_FTM_CHANNEL;
	ftmParam.level                 = FTM_PWM_ON_LEVEL;
	ftmParam.dutyCyclePercent      = 0U;
	ftmParam.firstEdgeDelayPercent = 0U;
	ftmParam.enableComplementary   = false;
	ftmParam.enableDeadtime        = false;

	if (kStatus_Success !=
		FTM_SetupPwm(SERVO_FTM_BASEADDR, &ftmParam, 1U, kFTM_EdgeAlignedPwm, SERVO_PWM_FREQUENCY, FTM_SOURCE_CLOCK))
	{
		return -1;
	}
	FTM_StartTimer(SERVO_FTM_BASEADDR, kFTM_SystemClock);
}

void setServo(uint8_t angle)
{

	uint32_t cnv;
	cnv = map(angle, 0, 180, 375, 5620);

	SERVO_FTM_BASEADDR->CONTROLS[ (ftm_chnl_t)SERVO_FTM_CHANNEL].CnV = cnv;

	/* Software trigger to update registers */
	FTM_SetSoftwareTrigger(SERVO_FTM_BASEADDR, true);

}

void CIKY_InitSensors(void)
{

	adc16_config_t adc16ConfigStruct[2];


	ADC16_GetDefaultConfig(&adc16ConfigStruct[0]);
	ADC16_Init(ADC16_BASE0, &adc16ConfigStruct[0]);
	ADC16_EnableHardwareTrigger(ADC16_BASE0, false); /* Make sure the software trigger is used. */
	ADC16_DoAutoCalibration(ADC16_BASE0);

	ADC16_GetDefaultConfig(&adc16ConfigStruct[1]);
	ADC16_Init(ADC16_BASE1, &adc16ConfigStruct[1]);
	ADC16_EnableHardwareTrigger(ADC16_BASE1, false); /* Make sure the software trigger is used. */
	ADC16_DoAutoCalibration(ADC16_BASE1);

	adc16ChannelConfigStruct[0].channelNumber                        = ADC16_A3;
	adc16ChannelConfigStruct[0].enableInterruptOnConversionCompleted = false;
	adc16ChannelConfigStruct[0].enableDifferentialConversion = false;

	adc16ChannelConfigStruct[1].channelNumber                        = ADC16_A2;
   	adc16ChannelConfigStruct[1].enableInterruptOnConversionCompleted = false;
   	adc16ChannelConfigStruct[1].enableDifferentialConversion = false;

   	adc16ChannelConfigStruct[2].channelNumber                        = ADC16_A1;
   	adc16ChannelConfigStruct[2].enableInterruptOnConversionCompleted = false;
   	adc16ChannelConfigStruct[2].enableDifferentialConversion = false;

   	adc16ChannelConfigStruct[3].channelNumber                        = ADC16_A0;
   	adc16ChannelConfigStruct[3].enableInterruptOnConversionCompleted = false;
   	adc16ChannelConfigStruct[3].enableDifferentialConversion = false;
}


int analogRead(sensors sensor)
{
	if(sensor == A0){
		ADC16_SetChannelConfig(ADC16_BASE1, ADC16_CHANNEL_GROUP0, &adc16ChannelConfigStruct[3]);
	    while (0U == (kADC16_ChannelConversionDoneFlag & ADC16_GetChannelStatusFlags(ADC16_BASE1, ADC16_CHANNEL_GROUP0)));
	    return map(ADC16_GetChannelConversionValue(ADC16_BASE1, ADC16_CHANNEL_GROUP0),0,4095,0,1023);
	}
	else if(sensor == A1){
		ADC16_SetChannelConfig(ADC16_BASE1, ADC16_CHANNEL_GROUP0, &adc16ChannelConfigStruct[2]);
	    while (0U == (kADC16_ChannelConversionDoneFlag & ADC16_GetChannelStatusFlags(ADC16_BASE1, ADC16_CHANNEL_GROUP0)));
	    return map(ADC16_GetChannelConversionValue(ADC16_BASE1, ADC16_CHANNEL_GROUP0),0,4095,0,1023);
	}
	else if(sensor == A2){
		ADC16_SetChannelConfig(ADC16_BASE0, ADC16_CHANNEL_GROUP0, &adc16ChannelConfigStruct[1]);
		while (0U == (kADC16_ChannelConversionDoneFlag & ADC16_GetChannelStatusFlags(ADC16_BASE0, ADC16_CHANNEL_GROUP0)));
		return  map(ADC16_GetChannelConversionValue(ADC16_BASE0, ADC16_CHANNEL_GROUP0),0,4095,0,1023);
	}
	else if(sensor == A3){
		ADC16_SetChannelConfig(ADC16_BASE0, ADC16_CHANNEL_GROUP0, &adc16ChannelConfigStruct[0]);
		while (0U == (kADC16_ChannelConversionDoneFlag &ADC16_GetChannelStatusFlags(ADC16_BASE0, ADC16_CHANNEL_GROUP0)));
		return map(ADC16_GetChannelConversionValue(ADC16_BASE0, ADC16_CHANNEL_GROUP0),0,4095,0,1023);
	}
	else
		return -1;

}


unsigned long pulseIn(GPIO_Type *base, uint32_t pin, bool state, uint32_t timeout)
{

	unsigned long width = 0;


	unsigned long numloops = 0;
	unsigned long maxloops = timeout<<1;

	// wait for any previous pulse to end
	while (GPIO_PinRead(base,pin) == state)
		if (numloops++ == maxloops)
			return 0;

	// wait for the pulse to start
	while (GPIO_PinRead(base,pin) != state)
		if (numloops++ == maxloops)
			return 0;

	// wait for the pulse to stop
	while (GPIO_PinRead(base,pin) == state) {
		if (numloops++ == maxloops)
			return 0;
		width++;
	}


	return width>>1; //not accurate
}



long map(long x, long in_min, long in_max, long out_min, long out_max) {

	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
