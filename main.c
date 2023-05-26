/*
 ******************************************************************************
 * File              : main.c
 * Author            : Philip Zatta, PhD, Software Consultant
 * Company           : Embedded Software & Systems LLC
 * MCU/Board         : Waveshare OpenH743-C STM32H743IIT6 Development Board
 * Description       : System clock setup to max frequency of 480 MHz
 * Datasheet         : DS12110 Rev March 2023  STM32H742xI/G STM32H743xI/G
 * Reference Manual  : RM0433 Reference Manual Rev 8, January 2023
 *
 * Key Features      : 32-bit Arm® Cortex®-M7 core with double precision
 *                     FPU and L1 cache: 16 Kbytes of data
 *                     and 16 Kbytes of instruction cache; frequency
 *                     up to 480 MHz, MPU, 1027 DMIPS/
 *                     2.14 DMIPS/MHz (Dhrystone 2.1), and DSP instructions
 *
 * Author            : Philip Zatta, PhD, Software Consultant
 * Company           : Emebedded Software & Systems LLC
 *                     Embedded Software & LabView
 * IDE               : STM32CUBE Version 1.12
 * Windows OS        : WIndows 11
 * Date              : May 25, 2023
 ******************************************************************************
 *
 * Comments/Explanations: Bare Metal Programming & Register Programming
 *                        using Datasheet and Reference Manual only
 *
 * After setting system clock setting at 480 MHz,
 *    - Output HSI clock onto MCO1  (No division)
 *    - Divide main clock by 5 output on MCO2
 *
 * MCO1  --------------> PA8
 * MCO2                  PC9
 *
 * Reference Manual RM0433 Rev 8, Page 344
 *
 * 8.5.4 Clock output generation (MCO1/MCO2)
 *   Two micro-controller clock output (MCO) pins, MCO1 and MCO2, are available. A clock
 *   source can be selected for each output.The selected clock can be divided thanks to
 *   configurable prescaler (refer to Figure 45 for additional information on signal selection).
 *   MCO1 and MCO2 outputs are controlled via MCO1PRE[3:0], MCO1[2:0], MCO2PRE[3:0]
 *   and MCO2[2:0] located in the RCC clock configuration register (RCC_CFGR).
 *   The GPIO port corresponding to each MCO pin, has to be programmed in alternate function
 *   mode.
 *   The clock provided to the MCOs outputs must not exceed the maximum pin speed (refer to
 *   the product datasheet for information on the supported pin speed).
 *
 ******************************************************************************
 */


#include "stm32h7xx.h"
#include "main.h"


int main(void)
{
	/* Initialize MCU */
	SystemInit();

	/* Configure MCO pins as system pins */
	MCO_Pins_Config()      ;

	/* Configure the system clock */
	SystemClock_Config()   ;

	/* Update system clock and D2 clock */
	SystemCoreClockUpdate();

	/*Select clocks to output on pins PA8 and PC9 with prescaler values
	 * Maximum frequency to output on PA8 and PC9 is 100 MHz
	 * */
	MCO_Select_Set()       ;

	while (1)
	{

	}
}
