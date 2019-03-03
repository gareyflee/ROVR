/*
 * @brief LPC40xx 16/32-bit PWM driver
 *
 * @note
 * Copyright(C) Matthew Dupree, 2019
 * All rights reserved.
 */

// See https://www.nxp.com/docs/en/user-guide/UM10562.pdf

#ifndef __PWM_DRIVER_H_
#define __PWM_DRIVER_H_

#include <stdbool.h>


#define LPC_PWM0_BASE             0x40014000
#define LPC_PWM1_BASE             0x40018000
#define LPC_PWM0                ((PWM_T *) LPC_PWM0_BASE)
#define LPC_PWM1                ((PWM_T *) LPC_PWM1_BASE)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 32-bit Standard PWM register block structure
 */
typedef struct {					/*!< PWMn Structure       */
	uint32_t IR;				/*!< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending. */
	uint32_t TCR;				/*!< PWM Control Register. The TCR is used to control the PWM Counter functions. The PWM Counter can be disabled or reset through the TCR. */
	uint32_t TC;				/*!< PWM Counter. The 32 bit TC is incremented every PR+1 cycles of PCLK. The TC is controlled through the TCR. */
	uint32_t PR;				/*!< Prescale Register. The Prescale Counter (below) is equal to this value, the next clock increments the TC and clears the PC. */
	uint32_t PC;				/*!< Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface. */
	uint32_t MCR;				/*!< Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs. */
	uint32_t MR[4];			/*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	uint32_t CCR;				/*!< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place. */
	uint32_t CR[2];			/*!< Capture Register. CR is loaded with the value of TC when there is an event on the CAPn.0 input. */
	uint32_t MR2[3];
	uint32_t RESERVED0[12];
	uint32_t CTCR;				/*!< Count Control Register. The CTCR selects between PWM and Counter mode, and in Counter mode selects the signal and edge(s) for counting. */
} PWM_T;

/** Macro to clear interrupt pending */
#define PWM_IR_CLR(n)         _BIT(n)

/** Macro for getting a PWM match interrupt bit */
#define PWM_MATCH_INT(n)      (_BIT((n) & 0x0F))
/** Macro for getting a capture event interrupt bit */
#define PWM_CAP_INT(n)        (_BIT((((n) & 0x0F) + 4)))

/** PWM/counter enable bit */
#define PWM_ENABLE            ((uint32_t) (1 << 0))
/** PWM/counter reset bit */
#define PWM_RESET             ((uint32_t) (1 << 1))

/** Bit location for interrupt on MRx match, n = 0 to 3 */
#define PWM_INT_ON_MATCH(n)   (_BIT(((n) * 3)))
/** Bit location for reset on MRx match, n = 0 to 3 */
#define PWM_RESET_ON_MATCH(n) (_BIT((((n) * 3) + 1)))
/** Bit location for stop on MRx match, n = 0 to 3 */
#define PWM_STOP_ON_MATCH(n)  (_BIT((((n) * 3) + 2)))

/** Bit location for CAP.n on CRx rising edge, n = 0 to 3 */
#define PWM_CAP_RISING(n)     (_BIT(((n) * 3)))
/** Bit location for CAP.n on CRx falling edge, n = 0 to 3 */
#define PWM_CAP_FALLING(n)    (_BIT((((n) * 3) + 1)))
/** Bit location for CAP.n on CRx interrupt enable, n = 0 to 3 */
#define PWM_INT_ON_CAP(n)     (_BIT((((n) * 3) + 2)))

/**
 * @brief	Initialize a PWM
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Nothing
 */
void Chip_PWM_Init(PWM_T *pTMR);

/**
 * @brief	Shutdown a PWM
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Nothing
 */
void Chip_PWM_DeInit(PWM_T *pTMR);

/**
 * @brief	Determine if a match interrupt is pending
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match interrupt number to check
 * @return	false if the interrupt is not pending, otherwise true
 * @note	Determine if the match interrupt for the passed PWM and match
 * counter is pending.
 */
bool Chip_PWM_MatchPending(PWM_T *pTMR, int8_t matchnum)
{
	return (bool) ((pTMR->IR & PWM_MATCH_INT(matchnum)) != 0);
}

/**
 * @brief	Determine if a capture interrupt is pending
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture interrupt number to check
 * @return	false if the interrupt is not pending, otherwise true
 * @note	Determine if the capture interrupt for the passed capture pin is
 * pending.
 */
bool Chip_PWM_CapturePending(PWM_T *pTMR, int8_t capnum)
{
	return (bool) ((pTMR->IR & PWM_CAP_INT(capnum)) != 0);
}

/**
 * @brief	Clears a (pending) match interrupt
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match interrupt number to clear
 * @return	Nothing
 * @note	Clears a pending PWM match interrupt.
 */
void Chip_PWM_ClearMatch(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->IR = PWM_IR_CLR(matchnum);
}

/**
 * @brief	Clears a (pending) capture interrupt
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture interrupt number to clear
 * @return	Nothing
 * @note	Clears a pending PWM capture interrupt.
 */
void Chip_PWM_ClearCapture(PWM_T *pTMR, int8_t capnum)
{
	pTMR->IR = (0x10 << capnum);
}

/**
 * @brief	Enables the PWM (starts count)
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Nothing
 * @note	Enables the PWM to start counting.
 */
void Chip_PWM_Enable(PWM_T *pTMR)
{
	pTMR->TCR |= PWM_ENABLE;
}

/**
 * @brief	Disables the PWM (stops count)
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Nothing
 * @note	Disables the PWM to stop counting.
 */
void Chip_PWM_Disable(PWM_T *pTMR)
{
	pTMR->TCR &= ~PWM_ENABLE;
}

/**
 * @brief	Returns the current PWM count
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Current PWM terminal count value
 * @note	Returns the current PWM terminal count.
 */
uint32_t Chip_PWM_ReadCount(PWM_T *pTMR)
{
	return pTMR->TC;
}

/**
 * @brief	Returns the current prescale count
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Current PWM prescale count value
 * @note	Returns the current prescale count.
 */
uint32_t Chip_PWM_ReadPrescale(PWM_T *pTMR)
{
	return pTMR->PC;
}

/**
 * @brief	Sets the prescaler value
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	prescale	: Prescale value to set the prescale register to
 * @return	Nothing
 * @note	Sets the prescale count value.
 */
void Chip_PWM_PrescaleSet(PWM_T *pTMR, uint32_t prescale)
{
	pTMR->PR = prescale;
}

/**
 * @brief	Sets a PWM match value
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM to set match count for
 * @param	matchval	: Match value for the selected match count
 * @return	Nothing
 * @note	Sets one of the PWM match values.
 */
void Chip_PWM_SetMatch(PWM_T *pTMR, int8_t matchnum, uint32_t matchval)
{
	pTMR->MR[matchnum] = matchval;
}

/**
 * @brief	Reads a capture register
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture register to read
 * @return	The selected capture register value
 * @note	Returns the selected capture register value.
 */
uint32_t Chip_PWM_ReadCapture(PWM_T *pTMR, int8_t capnum)
{
	return pTMR->CR[capnum];
}

/**
 * @brief	Resets the PWM terminal and prescale counts to 0
 * @param	pTMR	: Pointer to PWM IP register address
 * @return	Nothing
 */
void Chip_PWM_Reset(PWM_T *pTMR);

/**
 * @brief	Enables a match interrupt that fires when the terminal count
 *			matches the match counter value.
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM, 0 to 3
 * @return	Nothing
 */
void Chip_PWM_MatchEnableInt(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= PWM_INT_ON_MATCH(matchnum);
}

/**
 * @brief	Disables a match interrupt for a match counter.
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM, 0 to 3
 * @return	Nothing
 */
void Chip_PWM_MatchDisableInt(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR &= ~PWM_INT_ON_MATCH(matchnum);
}

/**
 * @brief	For the specific match counter, enables reset of the terminal count register when a match occurs
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM, 0 to 3
 * @return	Nothing
 */
void Chip_PWM_ResetOnMatchEnable(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= PWM_RESET_ON_MATCH(matchnum);
}

/**
 * @brief	For the specific match counter, disables reset of the terminal count register when a match occurs
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM, 0 to 3
 * @return	Nothing
 */
void Chip_PWM_ResetOnMatchDisable(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR &= ~PWM_RESET_ON_MATCH(matchnum);
}

/**
 * @brief	Enable a match PWM to stop the terminal count when a
 *			match count equals the terminal count.
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM, 0 to 3
 * @return	Nothing
 */
void Chip_PWM_StopOnMatchEnable(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= PWM_STOP_ON_MATCH(matchnum);
}

/**
 * @brief	Disable stop on match for a match PWM. Disables a match PWM
 *			to stop the terminal count when a match count equals the terminal count.
 * @param	pTMR		: Pointer to PWM IP register address
 * @param	matchnum	: Match PWM, 0 to 3
 * @return	Nothing
 */
void Chip_PWM_StopOnMatchDisable(PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR &= ~PWM_STOP_ON_MATCH(matchnum);
}

/**
 * @brief	Enables capture on on rising edge of selected CAP signal for the
 *			selected capture register, enables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a rising edge.
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
void Chip_PWM_CaptureRisingEdgeEnable(PWM_T *pTMR, int8_t capnum)
{
	pTMR->CCR |= PWM_CAP_RISING(capnum);
}

/**
 * @brief	Disables capture on on rising edge of selected CAP signal. For the
 *			selected capture register, disables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a rising edge.
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
void Chip_PWM_CaptureRisingEdgeDisable(PWM_T *pTMR, int8_t capnum)
{
	pTMR->CCR &= ~PWM_CAP_RISING(capnum);
}

/**
 * @brief	Enables capture on on falling edge of selected CAP signal. For the
 *			selected capture register, enables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a falling edge.
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
void Chip_PWM_CaptureFallingEdgeEnable(PWM_T *pTMR, int8_t capnum)
{
	pTMR->CCR |= PWM_CAP_FALLING(capnum);
}

/**
 * @brief	Disables capture on on falling edge of selected CAP signal. For the
 *			selected capture register, disables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a falling edge.
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
void Chip_PWM_CaptureFallingEdgeDisable(PWM_T *pTMR, int8_t capnum)
{
	pTMR->CCR &= ~PWM_CAP_FALLING(capnum);
}

/**
 * @brief	Enables interrupt on capture of selected CAP signal. For the
 *			selected capture register, an interrupt will be generated when the enabled
 *			rising or falling edge on CAPn.capnum is detected.
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
void Chip_PWM_CaptureEnableInt(PWM_T *pTMR, int8_t capnum)
{
	pTMR->CCR |= PWM_INT_ON_CAP(capnum);
}

/**
 * @brief	Disables interrupt on capture of selected CAP signal
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
void Chip_PWM_CaptureDisableInt(PWM_T *pTMR, int8_t capnum)
{
	pTMR->CCR &= ~PWM_INT_ON_CAP(capnum);
}

/**
 * @brief Standard PWM initial match pin state and change state
 */
typedef enum IP_PWM_PIN_MATCH_STATE {
	PWM_EXTMATCH_DO_NOTHING = 0,	/*!< PWM match state does nothing on match pin */
	PWM_EXTMATCH_CLEAR      = 1,	/*!< PWM match state sets match pin low */
	PWM_EXTMATCH_SET        = 2,	/*!< PWM match state sets match pin high */
	PWM_EXTMATCH_TOGGLE     = 3	/*!< PWM match state toggles match pin */
} PWM_PIN_MATCH_STATE_T;

/**
 * @brief	Sets external match control (MATn.matchnum) pin control. For the pin
 *          selected with matchnum, sets the function of the pin that occurs on
 *          a terminal count match for the match count.
 * @param	pTMR			: Pointer to PWM IP register address
 * @param	initial_state	: Initial state of the pin, high(1) or low(0)
 * @param	matchState		: Selects the match state for the pin
 * @param	matchnum		: MATn.matchnum signal to use
 * @return	Nothing
 * @note	For the pin selected with matchnum, sets the function of the pin that occurs on
 * a terminal count match for the match count.
 */
void Chip_PWM_ExtMatchControlSet(PWM_T *pTMR, int8_t initial_state,
												 PWM_PIN_MATCH_STATE_T matchState, int8_t matchnum);

/**
 * @brief Standard PWM clock and edge for count source
 */
typedef enum IP_PWM_CAP_SRC_STATE {
	PWM_CAPSRC_RISING_PCLK  = 0,	/*!< PWM ticks on PCLK rising edge */
	PWM_CAPSRC_RISING_CAPN  = 1,	/*!< PWM ticks on CAPn.x rising edge */
	PWM_CAPSRC_FALLING_CAPN = 2,	/*!< PWM ticks on CAPn.x falling edge */
	PWM_CAPSRC_BOTH_CAPN    = 3	/*!< PWM ticks on CAPn.x both edges */
} PWM_CAP_SRC_STATE_T;

/**
 * @brief	Sets PWM count source and edge with the selected passed from CapSrc.
 *          If CapSrc selected a CAPn pin, select the specific CAPn pin with the capnum value.
 * @param	pTMR	: Pointer to PWM IP register address
 * @param	capSrc	: PWM clock source and edge
 * @param	capnum	: CAPn.capnum pin to use (if used)
 * @return	Nothing
 * @note	If CapSrc selected a CAPn pin, select the specific CAPn pin with the capnum value.
 */
void Chip_PWM_PWM_SetCountClockSrc(PWM_T *pTMR,
										PWM_CAP_SRC_STATE_T capSrc,
													 int8_t capnum)
{
	pTMR->CTCR = (uint32_t) capSrc | ((uint32_t) capnum) << 2;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __PWM_DRIVER_H_ */
