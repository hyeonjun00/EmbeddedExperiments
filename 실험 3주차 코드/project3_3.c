#include "device_registers.h"
#include "clocks_and_modes.h"
int lpit0_ch0_flag_counter = 0; 

void PORT_init (void)
{
	  PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTD */
	  PTD->PDDR |= 0x04EF;		/* 0000 0100 1110 1111, Port D0~D10:  Data Direction = output */ 
	  PORTD->PCR[0]  = PORT_PCR_MUX(1);	/* Port D0: MUX = GPIO  */
	  PORTD->PCR[1]  = PORT_PCR_MUX(1);	
	  PORTD->PCR[2]  = PORT_PCR_MUX(1);	
          PORTD->PCR[3]  = PORT_PCR_MUX(1);
	  PORTD->PCR[5]  = PORT_PCR_MUX(1);
	  PORTD->PCR[6]  = PORT_PCR_MUX(1);
	  PORTD->PCR[7]  = PORT_PCR_MUX(1);
	  PORTD->PCR[10]  = PORT_PCR_MUX(1);


}
void WDOG_disable (void)
{
  WDOG->CNT=0xD928C520;     /* Unlock watchdog */
  WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value */
  WDOG->CS = 0x00002100;    /* Disable watchdog*/
}

void LPIT0_init (uint32_t delay)
{
   uint32_t timeout;

  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs */

 
  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; 
  timeout=delay* 40000;
  LPIT0->TMR[0].TVAL = timeout;      
  LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
                                   
}

void delay_ms (volatile int ms){
   LPIT0_init(ms);           /* Initialize PIT0 for 1 second timeout  */
   while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} /* Wait for LPIT0 CH0 Flag */
               lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
               LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

int main(void)
{

	WDOG_disable();/* Disable Watchdog in case it is not done in startup code */
	PORT_init();            /* Configure ports */
	SOSC_init_8MHz();       /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	  int Dtime = 0;
	  Dtime = 500;
		int arr[] = {0,1,2,3,5,6,7,10,0};
		while(1) {
			for(int i = 0;i < 8; i++){
				PTD->PSOR |= 1<<arr[i];			/* turn OFF Port */
				PTD->PCOR |= 1<<arr[i+1];		/* turn ON Port */
				delay_ms(Dtime);
			}
			
	}
}