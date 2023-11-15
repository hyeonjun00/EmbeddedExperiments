#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"

void PORT_init (void)
{
	
  PCC->P CCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTD */
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

	/*!
	 * Infinite for:
	 * ========================
	 */
	   for(;;)
	  {
		convertAdcChan(13);                   /* Convert Channel AD13 to pot on EVB 	*/
		while(adc_complete()==0){}            /* Wait for conversion complete flag 	*/
		adcResultInMv = read_adc_chx();       /* Get channel's conversion results in mv */

		if (adcResultInMv > 3584) {          
		  PTD->PSOR |= 0x04EF;			/* turn OFF Port */
		  PTD->PCOR |= 1<<0;              
		}
		else if (adcResultInMv > 3072) {    
		  PTD->PSOR |= 0x04EF;   
		  PTD->PCOR |= 1<<1;     	      
		}
		else if (adcResultInMv > 2560) {       
		  PTD->PSOR |= 0x04EF;   
		  PTD->PCOR |= 1<<2;     	    
		}
		else if (adcResultInMv > 2048) {       
		  PTD->PSOR |= 0x04EF;   
		  PTD->PCOR |= 1<<3;     	    
		}
		else if (adcResultInMv > 1536) {       
		  PTD->PSOR |= 0x04EF;   
		  PTD->PCOR |= 1<<5;     	    
		}
		else if (adcResultInMv > 1024) {       
		  PTD->PSOR |= 0x04EF;   
		  PTD->PCOR |= 1<<6;
		}
		else if (adcResultInMv > 512) {       
		  PTD->PSOR |= 0x04EF;   
		  PTD->PCOR |= 1<<7;
		}    
		else {
		  PTD->PSOR |= 0x04EF; 
		  PTD->PCOR |= 1<<10;
		}

	  }
}
