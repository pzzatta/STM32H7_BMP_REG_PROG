/*
 ******************************************************************************
 * File              : system_clock_config.c
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

/*************************** Macros ************************************/

// Those macros were defined in addition to those in stm32h743xx.h header
#define RCC_PLLCKSELR_DIVM1_CLEAR   ( 0b111111UL << 4U )
#define RCC_PLLCKSELR_DIV_M1        (    0b000101 << 4 ) // DIVM1[5:0], Page 397
#define RCC_PLL1DIVR_DIV_N1         (     0xBFU << 0   )
#define RCC_PLL1DIVR_DIV_P1         (       1U << 9    )
#define RCC_PLL1DIVR_DIV_Q1         (       1U << 16   )
#define RCC_PLL1DIVR_DIV_R1         (       1U << 24   )
#define RCC_D1CFGR_HPRE_DIV_8       (    0x08  << 0    )
#define RCC_D1CFGR_D1CPRE_RESET     (   0b1111 << 8    )

void SystemClock_Config(void)
{
   /* Step 1: Set VOS1 in PWR D3 domain control register (PWR_D3CR) VOS bits
   *
   * Voltage scaling
   * Reference Manual, Page 279
   *       Run mode voltage scaling
   *       – VOS0: Scale 0 (VCORE boost)
   *       – VOS1: Scale 1 ; VOS[1:0] = 1 1
   *       – VOS2: Scale 2 ; VOS[1:0] = 1 0
   *       – VOS3: Scale 3 ; VOS[1:0] = 0 1
   */

   PWR->D3CR |= PWR_D3CR_VOS_1 | PWR_D3CR_VOS_0 ; // VOS1 ->Scale: 11

   // Wait for bits to be set
   while (! ( PWR->D3CR & PWR_D3CR_VOS_1) ) {}
   while (! ( PWR->D3CR & PWR_D3CR_VOS_0) ) {}

   /* Step 2: Set the voltage scaling level to VOS0 for maximum frequency
   * Reference Manual., Page 279
   * VOS0 activation/deactivation sequence
   * The system maximum frequency can be reached by boosting the voltage
   * scaling level to VOS0. This is done through the ODEN bit in the
   * SYSCFG_PWRCR register.
   * The sequence to activate the VOS0 is the following:
   *  1.Ensure that the system voltage scaling is set to VOS1 by checking
   *  the VOS bits in PWR D3 domain control register (PWR D3 domain control
   *  register (PWR_D3CR))
   *  2. Enable the SYSCFG clock in the RCC by setting the SYSCFGEN bit
   *  in the RCC_APB4ENR register.
   *  3. Enable the ODEN bit in the SYSCFG_PWRCR register.
   *  4. Wait for VOSRDY to be set.
   *  Once the VCORE supply has reached the required level, the system
   *  frequency can be increased.
   *
   * Reference Manual, Page 280
   * VOS0 can be enabled only when VOS1 is programmed in PWR D3 domain
   * control register (PWR_D3CR) VOS bits.
   **/

   /* Step 3: Set supply configuration update enable */
   PWR->CR3 |= (PWR_CR3_SCUEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS );

   /* Step 4: Enable the SYSCFG clock in the RCC by setting the SYSCFGEN
    * Reference Manual, Page 470
    **/
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN ;

    /* Step 5: Enable power over drive using SYSCFG power control register
    * Reference Manual, Page 560
    * Register only has 1 bit, ODEN
    **/

    /* Step 6: Enable power over drive using SYSCFG power control register
     * Reference Manual, Page 560
     * Register only has 1 bit, ODEN
     * */

   SYSCFG->PWRCR |=  SYSCFG_PWRCR_ODEN ;

   // Wait  for VOSRDY to be set (VOS to be ready)
   while(! (PWR->D3CR & PWR_D3CR_VOSRDY) ) {}

   /* Step 7: Enable HSE clock   */
   RCC->CR |= RCC_CR_HSEON;

   // Wait for HSE to be ready
   while(! (RCC->CR & RCC_CR_HSERDY) );

   /* Step 8: Select HSE temporarily using clock configuration register */
   RCC->CFGR |= RCC_CFGR_SW_HSE ;

   // Wait for HSE clock to be ready
   while(! (RCC->CFGR & RCC_CFGR_SWS_HSE )) {}

   /* Step 9: Disable HSI, PLL*/
   // Disable HSI
	RCC->CR   &= ~ RCC_CR_HSION  ;

   // Disable PLL1
   RCC->CR   &= ~ RCC_CR_PLL1ON ;

   // wait for PPL to be disabled
	while( (RCC->CR & RCC_CR_PLL1RDY) != 0 );

    /* Step 10: Configure division factor of  PLL1
    * Reference Manual, Page 397
	* Clear all bits DIVM from bit 4 to bit 9 first
	* Set division factor
	*/

	// Clearing all bits of DIVM1[5:0]. It also correspond to disabling prescaler
	RCC -> PLLCKSELR &= ~ RCC_PLLCKSELR_DIVM1_CLEAR ;

    // Set division factor

	/* Reference Manual, Page 398
	* Bits 9:4 DIVM1[5:0]: Prescaler for PLL1
    * Set and cleared by software to configure the prescaler of the PLL1.
    * The hardware does not allow any modification of this prescaler when PLL1 is
	* enabled (PLL1ON =‘1’).
	* In order to save power when PLL1 is not used, the value of DIVM1 must be
	* set to ‘0’.
	* 000000: prescaler disabled
	* 000001: division by 1 (bypass)
	* 000010: division by 2
	* 000011: division by 3
	* ...
	* 100000: division by 32 (default after reset)
	* ...
	* 111111: division by 63
	*/

    // Set division factor to 5
	RCC -> PLLCKSELR |= RCC_PLLCKSELR_DIV_M1 ; // DIVM1[5:0]

	/* Step 11: Select HSE as source of PLL1 */
	RCC -> PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSE;


	/* Step 12: Set Multiplication factor for PLL1 VCO(N)
	*               PLL1 DIVP division factor,
	*               PLL1 DIVQ division factor and
	*               PLL1 DIVR division factor
	*   Reference Manual =, Page 401 and 402
	*/

	// DIVN = 192, DIVP = 2, DIVQ = 2, DIVR = 2.
	RCC -> PLL1DIVR  |= RCC_PLL1DIVR_DIV_N1 ; // DIVN = 192,
	RCC -> PLL1DIVR  |= RCC_PLL1DIVR_DIV_P1 ; // DIVP =   2,
	RCC -> PLL1DIVR  |= RCC_PLL1DIVR_DIV_Q1 ; // DIVQ =   2,
	RCC -> PLL1DIVR  |= RCC_PLL1DIVR_DIV_R1 ; // DIVR =   2,

    /* Step 13: Disable PLL1 fractional latch
    *
	*   Reference Manual =, Page 401 and 402
	*   In order to latch the FRACN1 value into the Sigma-Delta modulator,
	*   PLL1FRACEN must be set to ‘0’, then set to ‘1’: the transition 0 to 1
	*    transfers the content of FRACN1 into the modulator.
	*/

	// Disable PLLFRACN
	RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLL1FRACEN;

	/*  Step 14: Reset PLL1 fractional divider register*/
	RCC->PLL1FRACR = 0x00;


	/*  Step 15: Select PLL1 input reference frequency range to 2 and 4 MHz
	*  Reference Manual, Page 401
	*  This bit must be written before enabling the PLL1.
	*  00: The PLL1 input (ref1_ck) clock range frequency is between 1 and 2 MHz (default after reset)
    *  01: The PLL1 input (ref1_ck) clock range frequency is between 2 and 4 MHz
	*  10: The PLL1 input (ref1_ck) clock range frequency is between 4 and 8 MHz
	*  11: The PLL1 input (ref1_ck) clock range frequency is between 8 and 16 MHz
	*/

	//RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_3;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_2  ;


	/*  Step 16: Select PLL1 output frequency range
	*  Reference Manual, Page 401
	*  This bit must be written before enabling the PLL1.
	*  0: Wide VCO range: 192 to 960 MHz (default after reset)
	*  1: Medium VCO range: 150 to 420 MHz
	*/

	RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLL1VCOSEL ;

	/*  Step 17: Enable PLL System Clock output
	*  Reference Manual, Page 401
	**/

	// Enable PLL System Clock output
	RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN;

	// Enable PLL1Q Clock output
	RCC->PLLCFGR |= RCC_PLLCFGR_DIVQ1EN;

	// Enable PLL1R  Clock output
	RCC->PLLCFGR |= RCC_PLLCFGR_DIVR1EN;

	// Enable PLL1FRACN
	RCC->PLLCFGR |= RCC_PLLCFGR_PLL1FRACEN;


	/*  Step 18: Enable the main PLL
	*  Reference Manual, Page 382
	*/

	// Enable the main PLL
	RCC->CR |= RCC_CR_PLLON;
	while(! (RCC->CR & RCC_CR_PLL1RDY));

	/* Step 19: Configure D1 domain CPU clock
    * Reference Manual, Page 394
	* HPRE[3:0]: D1 domain AHB prescaler
	* D1PPRE[2:0]: D1 domain APB3 prescaler
	* D1CPRE[3:0]: D1 domain Core prescaler
	*/

	// D1 domain AHB prescaler, 1000 for rcc_hclk3 = sys_d1cpre_ck / 2
	RCC->D1CFGR |=  RCC_D1CFGR_HPRE_DIV_8 ; // Or use RCC_D1CFGR_HPRE_3 from stm32h743xx.h

	//D1 domain Core prescaler, 0xxx for sys_ck not divided. (default after reset)
	RCC->D1CFGR &= ~ RCC_D1CFGR_D1CPRE_RESET ;

	//Select PLL1 as system clock (pll1_p_ck)
	RCC->CFGR |= RCC_CFGR_SW_PLL1;

	// Wait for PPL1 to be ready
	while(! (RCC->CFGR & RCC_CFGR_SWS_PLL1) );

	//Set D1 domain APB3 prescaler, 100: rcc_pclk3 = rcc_hclk3 / 2
	RCC->D1CFGR   |= RCC_D1CFGR_D1PPRE_2 ;

	/* Step 20: Configure D2 domain clock
	* Reference Manual, Page 395
    * D2PPRE1[2:0]: D2 domain APB1 prescaler
	* D2CPRE2[3:0]: D2 domain APB2 prescaler
	*/

	// Set D2 domain APB1 prescaler, 100 for rcc_pclk1 = rcc_hclk1 / 2
	RCC -> D2CFGR |= RCC_D2CFGR_D2PPRE1_2 ;

	//Set D2 domain APB2 prescaler, 100 for rcc_pclk2 = rcc_hclk1 / 2
	RCC -> D2CFGR |=RCC_D2CFGR_D2PPRE2_2 ;

	/* Step 21: Configure D3 domain clock
	* Reference Manual, Page 395
	*
	* D3 domain APB4 prescale
	*/

	// Set D3 domain APB4 prescaler, 100 for rcc_pclk4 = rcc_hclk4 / 2
	RCC -> D3CFGR |= RCC_D3CFGR_D3PPRE_2 ;

	 /* Step 21, Optional: Update of SystemCoreClock and SystemD2Clock
	* SystemCoreClock and SystemD2Clock will be automatically updated
	* after call of external function SystemCoreClockUpdate(void);
	* If SystemCoreClockUpdate(void) will not be called, then use
	* D1CorePrescTable to update SystemD2Clock in this section.
	*/
}
