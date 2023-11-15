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

	FTM0->SC = FTM_SC_PS(0) | FTM_SC_PWMEN0_MASK | FTM_SC_PWMEN1_MASK | FTM_SC_PWMEN2_MASK;

	FTM0->MOD = 8000 - 1;
	FTM0->CNTIN = FTM_CNTIN_INIT(0);

	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_ELSA_MASK;
	FTM0->CONTROLS[1].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[1].CnSC |= FTM_CnSC_ELSA_MASK;
	FTM0->CONTROLS[2].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[2].CnSC |= FTM_CnSC_ELSA_MASK;
}

void FTM0_CH0_PWM(int i)
{
	FTM0->CONTROLS[0].CnV = i;
	FTM0->SC |= FTM_SC_CLKS(3);
}
void FTM0_CH1_PWM(int i)
{
	FTM0->CONTROLS[1].CnV = i;
	FTM0->SC |= FTM_SC_CLKS(3);
}
void FTM0_CH2_PWM(int i)
{
	FTM0->CONTROLS[2].CnV = i;
	FTM0->SC |= FTM_SC_CLKS(3);
}

void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[16] |= PORT_PCR_MUX(2);				 
	PORTD->PCR[15] |= PORT_PCR_MUX(2);				 
	PORTD->PCR[0] |= PORT_PCR_MUX(2);				 
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

	double Lotate;
	double H;
	double S;
	double I;
	int R1;
	int G1;
	int B1;
	double r;
	double g;
	double b;
	double Pi;
	Pi = 3.14159265358979323846;

	while (1)
	{	convertAdcChan(12);
		while (adc_complete() == 0)
		{
		}
		adcResultInMv = read_adc_chx();

		Lotate = adcResultInMv / 13.88888889;
		H = Lotate /45;
		S = 1;	 // 0~1
		I = 0.5; // 0~1
		if (S > 1)
			S = 1;
		if (I > 1)
			I = 1;
		if (S == 0)
			r = g = b = I;

			
		else
		{
			if ((H >= 0) && (H < 2 * Pi / 3)) // H 0~120
			{
				b = (1 - S) / 3;
				r = (1 + S * cos(H) / cos(Pi / 3 - H)) / 3;
				g = 1 - r - b;
			}
			else if ((H >= 2 * Pi / 3) && (H < 4 * Pi / 3)) // H120~240
			{
				H = H - 2 * Pi / 3;
				r = (1 - S) / 3;
				g = (1 + S * cos(H) / cos(Pi / 3 - H)) / 3;
				b = 1 - r - g;
			}
			else if ((H >= 4 * Pi / 3) && (H < 2 * Pi)) // H240~360
			{
				H = H - 4 * Pi / 3;
				g = (1 - S) / 3;
				b = (1 + S * cos(H) / cos(Pi / 3 - H)) / 3;
				r = 1 - b - g;
			}
			if (r < 0)
				r = 0;
			if (g < 0)
				g = 0;
			if (b < 0)
				b = 0;
		}
		R1 = r * 8000 * I;
		G1 = g * 8000 * I;
		B1 = b * 8000 * I;

	

		FTM0_CH0_PWM(R1);
		FTM0_CH1_PWM(G1);
		FTM0_CH2_PWM(B1);
	
	}
}
