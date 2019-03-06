/*
 * @brief LPC17xx/40xx 16/32-bit PWM/PWM driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#if false

#include "chip.h"
#include "pwm_driver.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Returns clock for the peripheral block */
STATIC CHIP_SYSCTL_CLOCK_T Chip_PWM_GetClockIndex(PWM_T *pTMR)
{
	CHIP_SYSCTL_CLOCK_T clkTMR;

	if (pTMR == LPC_PWM1) {
		clkTMR = SYSCTL_CLOCK_PWM1;
	}
	else {
		clkTMR = SYSCTL_CLOCK_PWM0;
	}

	return clkTMR;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize a PWM */
void Chip_PWM_Init(PWM_T *pTMR)
{
	Chip_Clock_EnablePeriphClock(Chip_PWM_GetClockIndex(pTMR));
}

/*	Shutdown a PWM */
void Chip_PWM_DeInit(PWM_T *pTMR)
{
	Chip_Clock_DisablePeriphClock(Chip_PWM_GetClockIndex(pTMR));
}

/* Resets the PWM terminal and prescale counts to 0 */
void Chip_PWM_Reset(PWM_T *pTMR)
{
	uint32_t reg;

	/* Disable PWM, set terminal count to non-0 */
	reg = pTMR->TCR;
	pTMR->TCR = 0;
	pTMR->TC = 1;

	/* Reset PWM counter */
	pTMR->TCR = PWM_RESET;

	/* Wait for terminal count to clear */
	while (pTMR->TC != 0) {}

	/* Restore PWM state */
	pTMR->TCR = reg;
}

#endif /* Disable broken code */
