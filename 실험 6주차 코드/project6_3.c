#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
unsigned int FND_DATA[10]={0x7E, 0x0C,0xB6,0x9E,0xCC,0xDA,0xFA,0x4E,0xFE,0xCE};
unsigned int FND_SEL[4]={0x0100,0x0200,0x0400,0x0800};  
unsigned int j=0; /*FND select pin index */
unsigned int num,num0,num1,num2,num3 =0;
/*num is Counting value, num0 is '1', num2 is '10', num2 is '100', num3 is '1000'*/
unsigned int Dtime = 0; /* Delay Time Setting Variable*/

void PORT_init (void)
{
	
 	PCC-> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
    PTD->PDDR |= (1<<1);   /* Port D1: Data Direction= input (default) */
	PTD->PDDR |= (1<<2);   /* Port D2: Data Direction= input (default) */
	PTD->PDDR |= (1<<3);   /* Port D3: Data Direction= input (default) */
	PTD->PDDR |= (1<<4);   /* Port D4: Data Direction= input (default) */
	PTD->PDDR |= (1<<5);   /* Port D5: Data Direction= input (default) */
	PTD->PDDR |= (1<<6);   /* Port D6: Data Direction= input (default) */
	PTD->PDDR |= (1<<7);   /* Port D7: Data Direction= input (default) */


	PORTD->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
	PORTD->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
	PORTD->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
	PORTD->PCR[4] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
	PORTD->PCR[5] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
	PORTD->PCR[6] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
	PORTD->PCR[7] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */


	PTD->PDDR |= 1<<8|1<<9|1<<10|1<<11;
	PORTD->PCR[8] = PORT_PCR_MUX(1); /* Port D8: MUX = GPIO */
	PORTD->PCR[9] = PORT_PCR_MUX(1); /* Port D9: MUX = GPIO */
	PORTD->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
	PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */
}

void LPIT0_init (uint32_t delay)
{
   uint32_t timeout;
   /*!
    * LPIT Clocking:
    * ==============================
    */
  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

  /*!
   * LPIT Initialization:
   */
  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
                                        /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                        /* SW_RST=0: SW reset does not reset timer chans, regs */
                                        /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

  timeout=delay* 40;            //ms setting*40000 us setting *40
  LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
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
void LPIT0_init (uint32_t delay)
{
   uint32_t timeout;
   /*!
    * LPIT Clocking:
    * ==============================
    */
  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

  /*!
   * LPIT Initialization:
   */
  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
                                        /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                        /* SW_RST=0: SW reset does not reset timer chans, regs */
                                        /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

  timeout=delay* 40;            //ms setting*40000 us setting *40
  LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
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

void delay_us (volatile int us){
   LPIT0_init(us);           /* Initialize PIT0 for 1 second timeout  */
   while (0 == (LPIT0->MSR &  0x01/*LPIT_MSR_TIF0_MASK*/)) {} /* Wait for LPIT0 CH0 Flag */
               lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
               LPIT0->MSR |= 0x00;//............LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}



void Seg_out(int number){

	 Dtime = 1000;

	num3=(number/1000)%10;
	num2=(number/100)%10;
	num1=(number/10)%10;
	num0= number%10;


	// 1000자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num3];
    delay_us(Dtime);
	PTD->PCOR = 0xfff;
	j++;

	// 100자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num2];
    delay_us(Dtime);
	PTD->PCOR = 0xfff;
	j++;

	// 10자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num1];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
	j++;

	// 1자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num0];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
	j=0;

}

void WDOG_disable (void)
{
  WDOG->CNT=0xD928C520;     /* Unlock watchdog 		*/
  WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value 	*/
  WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
}







int main(void)
{
  uint32_t adcResultInMv=0;	/*< ADC0 Result in miliVolts */

	/*!
	 * Initialization:
	 * =======================
	 */
  WDOG_disable();        /* Disable WDOG												*/
  SOSC_init_8MHz();      /* Initialize system oscillator for 8 MHz xtal 				*/
  SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC 				*/
  NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash*/
  PORT_init();		     /* Init  port clocks and gpio outputs 						*/
  ADC_init();            /* Init ADC resolution 12 bit									*/

	
	  for(;;)
	  {
		convertAdcChan(13);                   /* Convert Channel AD13 to pot on EVB 	*/
		while(adc_complete()==0){}            /* Wait for conversion complete flag 	*/
		adcResultInMv = read_adc_chx();       /* Get channel's conversion results in mv */

		Seg_out(adcResultInMv);

	  }
}
