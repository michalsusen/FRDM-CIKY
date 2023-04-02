/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "ciky.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/



/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef enum {
  LOW=0,
  HIGH=1
} states;

volatile uint32_t g_systickCounter;
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/


/*!
 * @brief Main function
 */
int main(void)
{


    float err;
    int kor, mL, mR;
    int v = 180;
    float P = 0.9; //Pokud se na rovné čáre rozkmitává, pak "P" snížit.
    //Pokud nevytáčí i mírné zatáčky, pak "P" zvýšit.

    long odezva, vzdalenost;

    /* Board pin, clock, debug console init */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    CIKY_InitSensors();
    CIKY_InitMotors();

    /* Set systick reload value to generate 1us interrupt */
	if (SysTick_Config(SystemCoreClock/1000000))
	{
		while (1)
		{
		}
	}


    while (1)
    {


    	GPIO_PinWrite(BOARD_INITPINS_pTrig_GPIO, BOARD_INITPINS_pTrig_PIN, 0U);
		SysTick_DelayTicks(2U);
		GPIO_PinWrite(BOARD_INITPINS_pTrig_GPIO, BOARD_INITPINS_pTrig_PIN, 1U);
		SysTick_DelayTicks(10U);
		GPIO_PinWrite(BOARD_INITPINS_pTrig_GPIO, BOARD_INITPINS_pTrig_PIN, 0U);

		odezva = pulseIn(BOARD_INITPINS_pEcho_GPIO,BOARD_INITPINS_pEcho_PIN,HIGH,5000); // délku pulzu v mikrosekundách (us)
		vzdalenost = (int)(odezva / 58.31);    // přepočet na cm

		//PRINTF("\r\nCapture value C(n)V=%d cm\r\n", vzdalenost);

		if ((vzdalenost > 0) && (vzdalenost < 15)) {
			 setMotors(0, 0);
		}else {

		   // P - regulace
		   // Výpočet Error-u, s jednoduchou normalizací čidel.
			err = (0.17 * (analogRead(A2) - 161.0)) - (0.17 * (analogRead(A1) - 87.0));
		   kor = (int)(P * err);
		   mL = v + kor;
		   mR = v - kor;
		   if (mL > 255)
			 mL = 255;
		   if (mL < 0)
			 mL = 0;
		   if (mR > 255)
			 mR = 255;
		   if (mR < 0)
			 mR = 0;

		   setMotors(mL, mR);

		   //Ošetření nespojitých dějů.
		   if (analogRead(A0) > 500) {
			 setMotors(0, v);
			 while (analogRead(A2) < 500);
			 setMotors(v, 0);
			 while (analogRead(A1) < 500);
			 setMotors(0, 0);
		   }
		   if (analogRead(A3) > 500) {
			  setMotors(v, 0);
			 while (analogRead(A1) < 500);
			 setMotors(0, v);
			 while (analogRead(A2) < 500);
			 setMotors(0,0);
		   }
		}

    }
}
