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
 * Date              : May 23, 2023
 ******************************************************************************
 *
 * Comments/Explanations:
 *
 * Bare metal programming of ARM Cortex M7, ST STM32H743 Waveshare OpenH743I-C
 * development board
 *
 * System clock setting at 480 MHz
 * Output Microcontroller clock on MCO1 and MCO2
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
 *
 *
 *
 * User LEDs
 *  LED1  --------------> PB6
 *  LED2                  PB7
 *  LED3                  PH4
 *  LED4                  PI8
 *
 *
 * Programming steps:
 *   1- Turn on clock access to GPIO   using RCC_AHB1ENR  register
 *   2- Configure Pin   as output using MODER Register (01 for output mode)
 *   3- Create a simply delay using a for loop with a volatile variable
 *   4- Turn LEDS on and off using  ^ with Output Data Register
 *
 ******************************************************************************
 */




#include "stm32h7xx.h"
#include "main.h"


/*Enable clock access to GPIOB, GPIOH and GPIOI using  RCC_AHB4ENR register*/
#define RCC_GPIOAEN            (    1U << 0    )    /* Enable clock access to GPIOA in RCC_AHB4ENR*/
#define RCC_GPIOBEN            (    1U << 1    )    /* Enable clock access to GPIOB in RCC_AHB4ENR*/
#define RCC_GPIOCEN            (    1U << 2    )    /* Enable clock access to GPIOC in RCC_AHB4ENR*/
#define RCC_GPIOHEN            (    1U << 7    )    /* Enable clock access to GPIOB in RCC_AHB4ENR*/
#define RCC_GPIOIEN            (    1U << 8    )    /* Enable clock access to GPIOB in RCC_AHB4ENR*/

/* Set PA8 and PC9 in alternate mode using GPIOx_MODER register
 * 10: Alternate function mode */
#define PA8_MODER1             (    1U << 17   )     /* Set bit    */
#define PA8_MODER0             (    1U << 16   )     /* Clear bit  */

#define PC9_MODER1             (    1U << 19   )     /* Set bit    */
#define PC9_MODER0             (    1U << 18   )     /* Clear bit  */

/* Set alternate function of type system for PA8 and PC9 using GPIOx_AFRH register
 * 0000: AF0- System */
 #define PA8_AFR8_3            (    1U << 3   )      /* Clear bit  */
 #define PA8_AFR8_2            (    1U << 2   )      /* Clear bit  */
 #define PA8_AFR8_1            (    1U << 2   )      /* Clear bit  */
 #define PA8_AFR8_0            (    1U << 0   )      /* Clear bit  */

 #define PC9_AFR9_3            (    1U << 7   )      /* Clear bit  */
 #define PC9_AFR9_2            (    1U << 6   )      /* Clear bit  */
 #define PC9_AFR9_1            (    1U << 5   )      /* Clear bit  */
 #define PC9_AFR9_0            (    1U << 4   )      /* Clear bit  */

// PA8 and PC9 output speed
#define PC9_OSPEEDR_1          (    1U << 19  )    //PC9
#define PC9_OSPEEDR_0          (    1U << 18  )
#define PA8_OSPEEDR_1          (    1U << 17  )    // PA8
#define PA8_OSPEEDR_0          (    1U << 16  )

// PA8 and PC9 output type
#define PC9_OTYPER             (    1U << 9  )
#define PA8_OTYPER             (    1U << 8  )

/* Select PLL1 as MCO1 output on PA8*/
#define MCO1_2                 (    1U << 24  )      /* must set 011 */
#define MCO1_1                 (    1U << 23  )
#define MCO1_0                 (    1U << 22  )

/* Select system clock as MCO2 output on PC9 */
#define MCO2_2                 (    1U << 31  )      /* must set 000 */
#define MCO2_1                 (    1U << 30  )
#define MCO2_0                 (    1U << 29  )

/* MCO1 prescaler */
#define MCO1_PRE_3             (    1U << 21  )     /* 0010: division by 2 */
#define MCO1_PRE_2             (    1U << 20  )     /* 0011: division by 3 */
#define MCO1_PRE_1             (    1U << 19  )     /* 0100: division by 4 */
#define MCO1_PRE_0             (    1U << 18  )     /* 1111: division by 15*/

/* MCO2 prescaler */
#define MCO2_PRE_3             (    1U << 28  )     /* 0010: division by 2 */
#define MCO2_PRE_2             (    1U << 27  )     /* 0011: division by 3 */
#define MCO2_PRE_1             (    1U << 26  )     /* 0100: division by 4 */
#define MCO2_PRE_0             (    1U << 25  )     /* 1111: division by 15*/



/* Set PB6, PB7, PH4 and PI8 in output mode using GPIOx_MODER register
 * 01: General purpose output mode */
#define PB6_MODER1             (    1U << 13   )     /* Clear bit   */
#define PB6_MODER0             (    1U << 12   )     /* Set bit     */
#define PB7_MODER1             (    1U << 15   )     /* Clear bit   */
#define PB7_MODER0             (    1U << 14   )     /* Set bit     */
#define PH4_MODER1             (    1U <<  9   )     /* Clear bit   */
#define PH4_MODER0             (    1U <<  8   )     /* Set bit     */

#define PI8_MODER1             (    1U <<  17  )     /* Clear bit   */
#define PI8_MODER0             (    1U <<  16  )     /* Set bit     */


#define LED1_ON                (    1U << 6    )    /* LED1 on using ODR*/
#define LED2_ON                (    1U << 7    )    /* LED2 on using ODR*/
#define LED3_ON                (    1U << 4    )    /* LED3 on using ODR*/
#define LED4_ON                (    1U << 8    )    /* LED4 on using ODR*/


/************************ Function prototypes ***************************/

static void MCO_Pins_Config(void)    ;
static void MCO_Select_Set(void)     ;
static void GPIO_LEDs_Config(void)   ;
static void Turn_LEDs_OFF(void)      ;
static void Turn_LEDs_ON(void)      ;

void Delay(void);



/************************ Global Variables ***************************/

extern void SystemInit(void);
extern uint32_t SystemD1Clock;
extern uint32_t SystemD2Clock;
extern void SystemCoreClockUpdate(void);



int main(void)
{
	SystemInit();


	MCO_Pins_Config()      ;
	/* Configure the system clock */
	//SystemClock_Config()   ;
	SystemCoreClockUpdate();
	MCO_Select_Set()       ;
	GPIO_LEDs_Config()     ;
	Turn_LEDs_ON()         ;
	Delay()                ;
	Delay()                ;
	Turn_LEDs_OFF()         ;
	Delay()                ;
	Turn_LEDs_ON()         ;

	//GPIOB->ODR |= LED1_ON ; // This turns LED1 on

	while (1)
	{
		Delay();
		GPIOB->ODR ^=  LED1_ON ;  // Toggle LED1
		Delay();
		GPIOB->ODR ^=  LED2_ON ;  // Toggle LED2
		Delay();
		GPIOH->ODR ^=  LED3_ON ;  // Toggle LED3
		Delay();
		GPIOI->ODR ^=  LED4_ON ;  // Toggle LED4
		Delay();
		Turn_LEDs_OFF() ;
		Delay();
		Delay();
		Delay();
	}
}


void Delay(void)
{
	for (int i = 0; i < 2000000; i++)
	{

	}
}

static void MCO_Pins_Config(void)
{
	 /* ******************************
	  *
	  * MCO1  --------------> PA8
	  * MCO2                  PC9
	  *******************************/

	// Enable clock access to GPIOB and GPIOC
	RCC->AHB4ENR |= RCC_GPIOAEN | RCC_GPIOCEN ;

	// Set GPIOA Pin PA8 and GPIOC Pin PC9 in alternate mode, MODER[1:0] = 1 0
	GPIOA->MODER |=   PA8_MODER1 ;          /* Set bit    */
	GPIOA->MODER &= ~ PA8_MODER0 ;          /* Clear bit  */

	GPIOC->MODER |=   PC9_MODER1 ;          /* Set bit    */
	GPIOC->MODER &= ~ PC9_MODER0 ;          /* Clear bit  */

	// Set Alternate function type as system for pins PA8 and PC9
  /* GPIOA->AFR[1] &= ~ PA8_AFR8_3 ;            Clear bit
   GPIOA->AFR[1] &= ~ PA8_AFR8_2 ;
   GPIOA->AFR[1] &= ~ PA8_AFR8_1 ;
   GPIOA->AFR[1] &= ~ PA8_AFR8_0 ;*/

   GPIOA->AFR[1] &= ~( PA8_AFR8_3 | PA8_AFR8_2 | PA8_AFR8_1 | PA8_AFR8_0 );

  /* GPIOC->AFR[1] &= ~PC9_AFR9_3     ;   Clear bit
   GPIOC->AFR[1] &= ~PC9_AFR9_2     ;
   GPIOC->AFR[1] &= ~PC9_AFR9_1     ;
   GPIOC->AFR[1] &= ~PC9_AFR9_0     ;

*/
   GPIOC->AFR[1] &= ~ ( PC9_AFR9_3 | PC9_AFR9_2 | PC9_AFR9_1 | PC9_AFR9_0 );
   // Set GPIOA and GPIOC pins as ultra-high speed

   /* GPIO port output type register (GPIOx_OTYPER)
  	  * PA8 and PC9 in push-pull mode
  	  * Reference Manual, RM0433 Rev 8, Page 541
  	  */
  	 GPIOA->OTYPER &= ~ PA8_OTYPER;
  	 GPIOC->OTYPER &= ~ PC9_OTYPER;

  	 /* GPIO port output speed register (GPIOx_OSPEEDR)
  		  * PC9 in fast speed mode
  		  * Reference Manual, RM0433 Rev 8, Page 540, Page
  		  */
  	GPIOC->OSPEEDR |= PC9_OSPEEDR_1  | PC9_OSPEEDR_0 ;
  	GPIOA->OSPEEDR |= PA8_OSPEEDR_1  | PA8_OSPEEDR_0 ;
}


static void MCO_Select_Set(void)
{
	   /*****************************************************************
	    *  Select  PLL1 clock selected (pll1_p_ck) as MCO1 output using
	    *  RCC clock configuration register  RCC_CFGR
	    *  000: HSI clock selected (hsi_ck) (default after reset)
	    *  001: LSE oscillator clock selected (lse_ck)
	    *  010: HSE clock selected (hse_ck)
	    *  011: PLL1 clock selected (pll1_q_ck)
	    *  100: HSI48 clock selected (hsi48_ck)
	    *  *************************************************************/
	   RCC->CFGR &= ~ MCO1_2            ; // MCO1: HSE clock selected
	   RCC->CFGR |=   MCO1_1            ;
	   RCC->CFGR |=   MCO1_0            ;

	   /****************************************************************
	    * 000: System clock selected (sys_ck) (default after reset)
	    * 001: PLL2 oscillator clock selected (pll2_p_ck)
	    * 010: HSE clock selected (hse_ck)
	    * 011: PLL1 clock selected (pll1_p_ck)
	    * 100: CSI clock selected (csi_ck)
	    * 101:LSI clock selected (lsi_ck)
	    * *************************************************************/

	   RCC->CFGR &= ~ MCO2_2            ; // MCO2 :  011: PLL1 clock
	   RCC->CFGR |=   MCO2_1            ;
	   RCC->CFGR |=   MCO2_0            ;

	   /****************************************************
	    * MCO1 prescaler, RM0433 Rev 8, Page 391
	    * 0000: prescaler disabled (default after reset)
	    * 0001: division by 1 (bypass)
	    * 0010: division by 2
	    * 0011: division by 3
	    * 0100: division by 4
	    * ...
	    * 1111: division by 15
	    ***************************************************/

	   RCC->CFGR |=   MCO1_PRE_3       ;  // Division by 8 = 1 0 0 0
	   RCC->CFGR &= ~ MCO1_PRE_2       ;
	   RCC->CFGR &= ~ MCO1_PRE_1       ;
	   RCC->CFGR &= ~ MCO1_PRE_0       ;

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

	   RCC->CFGR |=   MCO2_PRE_3       ;  // Division by 8 = 1 0 0 0
	   RCC->CFGR &= ~ MCO2_PRE_2       ;
	   RCC->CFGR &= ~ MCO2_PRE_1       ;
	   RCC->CFGR &= ~ MCO2_PRE_0       ;
}



static void GPIO_LEDs_Config(void)
{

	/* **************************************
	 *  User LEDs
     *  LED1  --------------> PB6  (GPIOB)
     *  LED2                  PB7  (GPIOB)
     *  LED3                  PH4  (GPIOH)
     *  LED4                  PI8  (GPIOI)
     *
     * *************************************/

	// Enable clock access to GPIOB, GPIOH and GPIOI
	RCC->AHB4ENR |= RCC_GPIOBEN | RCC_GPIOHEN | RCC_GPIOIEN ;

	// Set Pins PB6, PB7, PH4 and PI8 in output mode
	GPIOB->MODER &= ~ PB6_MODER1 ;
	GPIOB->MODER |=   PB6_MODER0 ;

	GPIOB->MODER &= ~ PB7_MODER1 ;
	GPIOB->MODER |=   PB7_MODER0 ;

	GPIOH->MODER &= ~ PH4_MODER1 ;
	GPIOH->MODER |=   PH4_MODER0 ;

	GPIOI->MODER &= ~ PI8_MODER1 ;
	GPIOI->MODER |=   PI8_MODER0 ;
}

static void Turn_LEDs_OFF(void)
{
	// Turn LED1, LED2, LED3 and LED4 off
	GPIOB->ODR &= ~ LED1_ON ;
	GPIOB->ODR &= ~ LED2_ON ;
	GPIOH->ODR &= ~ LED3_ON ;
	GPIOI->ODR &= ~ LED4_ON ;
}

static void Turn_LEDs_ON(void)
{
	// Turn LED1, LED2, LED3 and LED4 off
	GPIOB->ODR |=  LED1_ON ;
	GPIOB->ODR |=  LED2_ON ;
	GPIOH->ODR |=  LED3_ON ;
	GPIOI->ODR |=  LED4_ON ;
}
