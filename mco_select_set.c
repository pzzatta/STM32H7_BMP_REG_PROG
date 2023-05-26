/*
 ******************************************************************************
 * File              : mco_select_set.c
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
 * Date              : May 23, 2023
 ******************************************************************************/

#include "stm32h7xx.h"

void MCO_Select_Set(void)
{
	/*****************************************************************
	 *  RCC clock configuration register  RCC_CFGR
	 *  000: HSI clock selected (hsi_ck) (default after reset)
	 *  001: LSE oscillator clock selected (lse_ck)
	 *  010: HSE clock selected (hse_ck)
	 *  011: PLL1 clock selected (pll1_q_ck)
	 *  100: HSI48 clock selected (hsi48_ck)
	 **************************************************************/
	 RCC->CFGR &= ~ (RCC_CFGR_MCO1_2| RCC_CFGR_MCO1_1 | RCC_CFGR_MCO1_0 )  ; // MCO1: HSI clock selected

	/****************************************************************
	 * 000: System clock selected (sys_ck) (default after reset)
	 * 001: PLL2 oscillator clock selected (pll2_p_ck)
	 * 010: HSE clock selected (hse_ck)
	 * 011: PLL1 clock selected (pll1_p_ck)
	 * 100: CSI clock selected (csi_ck)
	 * 101:LSI clock selected (lsi_ck)
	 * *************************************************************/

	 RCC->CFGR &= ~ RCC_CFGR_MCO2_2   ; // MCO2 :  011: PLL1 clock
	 RCC->CFGR |=   RCC_CFGR_MCO2_1   ;
	 RCC->CFGR |=   RCC_CFGR_MCO2_0   ;

	 /****************************************************
	  * MCO1 prescaler, RM0433 Rev 8, Page 391
	  * 0000: prescaler disabled (default after reset)
	  * 0001: division by 1 (bypass)
	  * 0010: division by 2
	  * 0011: division by 3
	  * 0100: division by 4
	  *...
	  * 1111: division by 15
	  ***************************************************/

	  // MCO1: No division or prescaler disabled.
	  RCC->CFGR &= ~ ( RCC_CFGR_MCO1PRE_3 | RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_0);

	  /****************************************************
	  * MCO2 prescaler, RM0433 Rev 8, Page 390
	  * 0000: prescaler disabled (default after reset)
	  * 0001: division by 1 (bypass)
	  * 0010: division by 2
	  * 0011: division by 3
	  * 0100: division by 4
	  * ...
	  * 1111: division by 15
	  ***************************************************/

	  RCC->CFGR &= ~ RCC_CFGR_MCO2PRE_3       ;  // Division by 5 = 0 1 0 1
	  RCC->CFGR |=   RCC_CFGR_MCO2PRE_2       ;
	  RCC->CFGR &= ~ RCC_CFGR_MCO2PRE_1       ;
	  RCC->CFGR |=   RCC_CFGR_MCO2PRE_0       ;
}
