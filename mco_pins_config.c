/*
 ******************************************************************************
 * File              : mco_pins_config.c
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

void MCO_Pins_Config(void)
{
   /* ******************************
	* MCO1  --------------> PA8
	* MCO2                  PC9
	*******************************/

	/* Step 1:  Enable clock access to GPIOB and GPIOC */
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOCEN ;

	/* Step 2: Set GPIOA Pin PA8 and GPIOC Pin PC9 in alternate mode, MODER[1:0] = 1 0 */
	GPIOA->MODER |=   GPIO_MODER_MODE8_1 ;         /* Set bit    */
	GPIOA->MODER &= ~ GPIO_MODER_MODE8_0 ;         /* Clear bit  */

	GPIOC->MODER |=   GPIO_MODER_MODE9_1 ;          /* Set bit    */
	GPIOC->MODER &= ~ GPIO_MODER_MODE9_0 ;  ;          /* Clear bit  */

	/* Step 3: Set Alternate function type as system  (AF = 0000 ) for pins PA8 and PC9 */
    // Pin PA8
   GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8_3  | GPIO_AFRH_AFSEL8_2  | GPIO_AFRH_AFSEL8_1  | GPIO_AFRH_AFSEL8_0 );

   // Pin PC9
   GPIOC->AFR[1] &= ~ (GPIO_AFRH_AFSEL9_3  | GPIO_AFRH_AFSEL9_2  | GPIO_AFRH_AFSEL9_1  | GPIO_AFRH_AFSEL9_0 );

   /* Step 4: Set GPIOA and GPIOC pins PA8 and PC9 in push-pull mode */

   /* GPIO port output type register (GPIOx_OTYPER)
  	* PA8 and PC9 in push-pull mode, clear bit
  	* Reference Manual, RM0433 Rev 8, Page 541
  	*/
   GPIOA->OTYPER &= ~ GPIO_OTYPER_OT8;
   GPIOC->OTYPER &= ~ GPIO_OTYPER_OT9;

   /* Step 5: Set GPIOA and GPIOC pins PA8 and PC9 as ultra-high speed
    * GPIO port output speed register (GPIOx_OSPEEDR)
    * PC9 in fast speed mode
    * Reference Manual, RM0433 Rev 8, Page 540, Page
  	*/
   GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1  |  GPIO_OSPEEDR_OSPEED9_0;
   GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1  |  GPIO_OSPEEDR_OSPEED8_0;
}
