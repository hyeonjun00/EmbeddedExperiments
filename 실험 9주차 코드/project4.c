#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"
#include <math.h>


int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */

void LPIT0_init(uint32_t delay)
{
	uint32_t timeout;

	/*!
	 * LPIT Clocking:
	 * ==============================
	 */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);	/* Clock Src = 6 (SPLL2_DIV2_CLK)*/
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

	/*!
	 * LPIT Initialization:
	 */
	LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* DBG_EN-0: Timer chans stop in Debug mode */
									   /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
									   /* SW_RST=0: SW reset does not reset timer chans, regs */
									   /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

	timeout = delay * 40;
	LPIT0->TMR[0].TVAL = timeout; /* Chan 0 Timeout period: 40M clocks */
	LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
	/* T_EN=1: Timer channel is enabled */
	/* CHAIN=0: channel chaining is disabled */
	/* MODE=0: 32 periodic counter mode */
	/* TSOT=0: Timer decrements immediately based on restart */
	/* TSOI=0: Timer does not stop after timeout */
	/* TROT=0 Timer will not reload on trigger */
	/* TRG_SRC=0: External trigger soruce */
	/* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void delay_us(volatile int us)
{
	LPIT0_init(us); /* Initialize PIT0 for 1 second timeout  */
	while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK))
	{
	}								  /* Wait for LPIT0 CH0 Flag */
	lpit0_ch0_flag_counter++;		  /* Increment LPIT0 timeout counter */
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void FTM_init(void)
{
	PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(0b010) | PCC_PCCn_CGC_MASK;

	FTM0->SC = FTM_SC_PS(0) | FTM_SC_PWMEN2_MASK | FTM_SC_PWMEN3_MASK;

	FTM0->MOD = 8000 - 1;
	FTM0->CNTIN = FTM_CNTIN_INIT(0);

	FTM0->COMBINE |= FTM_COMBINE_SYNCEN1_MASK | FTM_COMBINE_COMP1_MASK | FTM_COMBINE_DTEN1_MASK;

	FTM0->CONTROLS[2].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[2].CnSC |= FTM_CnSC_ELSA_MASK;
	FTM0->CONTROLS[3].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[3].CnSC |= FTM_CnSC_ELSA_MASK;
}

void FTM0_CH23_PWM(int i)
{
	FTM0->CONTROLS[2].CnV = i;
	FTM0->CONTROLS[3].CnV = i;
	FTM0->SC |= FTM_SC_CLKS(3);
}

void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[1] |= PORT_PCR_MUX(2);				 /* Port D16: MUX = ALT2, FTM0CH1 */
	PORTD->PCR[0] |= PORT_PCR_MUX(2);				 /* Port D16: MUX = ALT2, FTM0CH1 */
}

int main(void)
{
	uint32_t adcResultInMv = 0;
	SOSC_init_8MHz();	   /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz();	   /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Init clocks: 80 MHz SPLL & core, 40 MHz bus, 20 MHz flash */
	FTM_init();
	ADC_init();

	PORT_init(); /* Configure ports */

	int D = 0;

	while (1)
	{
		convertAdcChan(13);
		while (adc_complete() == 0)
		{
		}
		adcResultInMv = read_adc_chx();

		D = (int)((float)adcResultInMv * 1.6);

		FTM0_CH23_PWM(D);
	
	}
}