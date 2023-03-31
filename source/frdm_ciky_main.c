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
    float P = 0.3; //Pokud se na rovné čáre rozkmitává, pak "P" snížit.
    //Pokud nevytáčí i mírné zatáčky, pak "P" zvýšit.

    /* Board pin, clock, debug console init */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    CIKY_InitSensors();
    CIKY_InitMotors();
    CIKY_InitUltrasonic();


    while (1)
    {

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
