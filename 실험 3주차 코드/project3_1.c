#include "device_registers.h"
#include "clocks_and_modes.h"
int lpit0_ch0_flag_counter = 0; 

void PORT_init (void)
{	  
          PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK;	
	  PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTD */
	  

          PTC->PDDR &= 0xFF07;   		/* 1111 1111 0000 0111, Port C3,4,5,6,7:  Data Direction = input*/  
	  PORTC->PCR[3]  = PORT_PCR_MUX(1);	/* Port C3: MUX = GPIO  */
	  PORTC->PCR[4]  = PORT_PCR_MUX(1);	
	  PORTC->PCR[5]  = PORT_PCR_MUX(1);
          PORTC->PCR[6]  = PORT_PCR_MUX(1);
	  PORTC->PCR[7]  = PORT_PCR_MUX(1);



 
          PTD->PDDR |= 0x001F;			/* Port D0,1,2,3,5:  Data Direction = output */
	  PORTD->PCR[0]  = PORT_PCR_MUX(1);	/* Port D0: MUX = GPIO  */
	  PORTD->PCR[1]  = PORT_PCR_MUX(1);	
	  PORTD->PCR[2]  = PORT_PCR_MUX(1);
          PORTD->PCR[3]  = PORT_PCR_MUX(1);
	  PORTD->PCR[5]  = PORT_PCR_MUX(1);
		

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
		while(1) {
				PTD->PSOR|=0x001F; //every pin setting
				if(PTC->PDIR & (1 << 3)) {
					PTD->PCOR |= 1 << 0;
				} else if(PTC->PDIR & (1 << 4)) { 
						PTD->PCOR |= 1 << 1; 
				} else if(PTC->PDIR & (1 << 5)) { 
						PTD->PCOR |= 1 << 2;
				} else if(PTC->PDIR & (1 << 6)) { 
						PTD->PCOR |= 1 << 3; 
				} else if(PTC->PDIR & (1 << 7)) { 
						PTD->PCOR |= 1 << 5; 
				} else{ PTD->PSOR|=0x001F; 			//every pin setting
				
				}
		}
}

