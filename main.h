/*
 ******************************************************************************
 * File              : main.h
 * Author            : Philip Zatta, PhD, Software Consultant
 * Company           : Embedded Software & Systems LLC
 * MCU/Board         : Waveshare OpenH743 STM32H743IIT6 Development Board
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

/**************************** Macros ************************************/
#define RCC_PLLCKSELR_DIVM1_CLEAR   ( 0b111111UL << 4U )
#define RCC_PLLCKSELR_DIV_M1        (    0b000101 << 4 ) // DIVM1[5:0], Page 397
#define RCC_PLL1DIVR_DIV_N1         (     0xBFU << 0   )
#define RCC_PLL1DIVR_DIV_P1         (       1U << 9    )
#define RCC_PLL1DIVR_DIV_Q1         (       1U << 16   )
#define RCC_PLL1DIVR_DIV_R1         (       1U << 24   )
#define RCC_D1CFGR_HPRE_DIV_8       (    0x08  << 0    )
#define RCC_D1CFGR_D1CPRE_RESET     (   0b1111 << 8    )


/************************ Function prototypes ***************************/
static void MCO_Pins_Config(void)    ;
static void MCO_Select_Set(void)     ;
static void SystemClock_Config(void) ;

extern void SystemInit(void);              // ST predefined function
extern void SystemCoreClockUpdate(void);   // ST predefined function

/************************** Global Variables ***************************/
extern uint32_t SystemD1Clock;
extern uint32_t SystemD2Clock;



#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
