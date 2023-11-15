#include "device_registers.h"
#include "clocks_and_modes.h"

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
unsigned int FND_DATA[10]={0x7E, 0x0C,0xB6,0x9E,0xCC,0xDA,0xFA,0x4E,0xFE,0xCE};
unsigned int FND_SEL[4]={0x0100,0x0200,0x0400,0x0800};
unsigned int j=0; /*FND select pin index */
unsigned int num=0,num0=0,num1=0,num2=0,num3=0;
/*num is Counting value, num0 is '1', num2 is '10', num2 is '100', num3 is '1000'*/
unsigned int External_PIN=0; /* External_PIN:SW External input Assignment */
unsigned int Dtime = 0; /* Delay Time Setting Variable*/

void PORT_init (void)
{
		//PORTC,D Data Clock Set
	  	PCC->PCCn[PCC_PORTC_INDEX]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTC */
		PCC->PCCn[PCC_PORTD_INDEX]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTD */

	  //PortC,D Data Direction Set
		PTC->PDDR &= ~(1<<11);		/* Port C11 Port Input set, value '0'*/
        PTC->PDDR &= ~(1<<12);
        PTC->PDDR &= ~(1<<13);
        PTC->PDDR &= ~(1<<16);
        PTC->PDDR &= ~(1<<17);

		PTD->PDDR |= 1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7 		/* Port D1~7:  FND Data Direction = output */
				     |1<<8|1<<9|1<<10|1<<11; /*FND Select Direction */

		// PORTC_11 pin GPIO and Falling-edge Set
		PORTC->PCR[11] |= PORT_PCR_MUX(1); // Port C11 mux = GPIO
		PORTC->PCR[11] |=(10<<16); // Port C11 IRQC : interrupt on Falling-edge

        PORTC->PCR[12] |= PORT_PCR_MUX(1); // Port C12 mux = GPIO
		PORTC->PCR[12] |=(10<<16); // Port C12 IRQC : interrupt on Falling-edge

        PORTC->PCR[13] |= PORT_PCR_MUX(1); 
		PORTC->PCR[13] |=(10<<16); 
       
        PORTC->PCR[16] |= PORT_PCR_MUX(1);
		PORTC->PCR[16] |=(10<<16); 
       
        PORTC->PCR[17] |= PORT_PCR_MUX(1); 
		PORTC->PCR[17] |=(10<<16); 
       
 

		//PORTD FND Control Pin PCR Set, PTD1~PTD7:FND_DATA Control , PTD8~PTD11:FND_Select Control
	  PORTD->PCR[1]  = PORT_PCR_MUX(1);	/* Port D1: MUX = GPIO  */
	  PORTD->PCR[2]  = PORT_PCR_MUX(1);	/* Port D2: MUX = GPIO  */
	  PORTD->PCR[3]  = PORT_PCR_MUX(1);	/* Port D3: MUX = GPIO  */
	  PORTD->PCR[4]  = PORT_PCR_MUX(1);	/* Port D4: MUX = GPIO  */
	  PORTD->PCR[5]  = PORT_PCR_MUX(1);	/* Port D5: MUX = GPIO  */
	  PORTD->PCR[6]  = PORT_PCR_MUX(1);	/* Port D6: MUX = GPIO  */
	  PORTD->PCR[7] = PORT_PCR_MUX(1); 	/* Port D7: MUX = GPIO */

	  PORTD->PCR[8] = PORT_PCR_MUX(1); 	/* Port D8: MUX = GPIO */
	  PORTD->PCR[9] = PORT_PCR_MUX(1); 	/* Port D9: MUX = GPIO */
	  PORTD->PCR[10] = PORT_PCR_MUX(1); 	/* Port D10: MUX = GPIO */
	  PORTD->PCR[11] = PORT_PCR_MUX(1); 	/* Port D11: MUX = GPIO */
}
void WDOG_disable (void)
{
  WDOG->CNT=0xD928C520;     /* Unlock watchdog 		*/
  WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value 	*/
  WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
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

  timeout=delay* 40000;
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

void delay_ms (volatile int ms){
   LPIT0_init(ms);           /* Initialize PIT0 for 1 second timeout  */
   while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} /* Wait for LPIT0 CH0 Flag */
               lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
               LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void NVIC_init_IRQs(void){
	S32_NVIC->ICPR[1] |= 1<<(61%32); // Clear any pending IRQ61
	S32_NVIC->ISER[1] |= 1<<(61%32); // Enable IRQ61
	S32_NVIC->IP[61] =0xB; //Priority 11 of 15
}

void PORTC_IRQHandler(void){

	PORTC->PCR[11] &= ~(0x01000000); // Port Control Register ISF bit '0' set
    PORTC->PCR[12] &= ~(0x01000000);
    PORTC->PCR[13] &= ~(0x01000000);
    PORTC->PCR[16] &= ~(0x01000000);
    PORTC->PCR[17] &= ~(0x01000000);

	//PORTC_Interrupt State Flag Register Read
	if((PORTC->ISFR & (1<<11)) != 0){
		External_PIN=1;
	}
    if(PORTC->ISFR & (1<<12)!=0){
        External_PIN=2;
    }
    if(PORTC->ISFR & (1<<13)!=0){
        External_PIN=3;
    }
    if(PORTC->ISFR & (1<<16)!=0){
        External_PIN=4;
    }
    if(PORTC->ISFR & (1<<17)!=0){
        External_PIN=5;
    }
    


	// External input Check Behavior Assignment
	switch (External_PIN){
		case 1:
			num += 1;
			External_PIN=0;
			break;
        case 2:
            num += 2;
            External_PIN=0;
            break;
        case 3:
            num += 3;
            External_PIN=0;
            break;
        case 4:
            num = num*2;
            External_PIN=0;
            break;
        case 5:
            num = num/2;
            External_PIN=0;
            break;
		default:
			break;
	}

	PORTC->PCR[11] |= 0x01000000; // Port Control Register ISF bit '1' set
    PORTC->PCR[12] |= 0x01000000;
    PORTC->PCR[13] |= 0x01000000;
    PORTC->PCR[16] |= 0x01000000;
    PORTC->PCR[17] |= 0x01000000;

}





int main(void)
{

	WDOG_disable();/* Disable Watchdog in case it is not done in startup code */
	PORT_init();            /* Configure ports */
	SOSC_init_8MHz();        /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	NVIC_init_IRQs(); /*Interrupt Pending, Endable, Priority Set*/

	Dtime = 500; // Delay Reset Value

	num=0000; // FND Reset Value

	while(1){ /* Loop Start*/

		num3=(num/1000)%10;
		num2=(num/100)%10;
		num1=(num/10)%10;
		num0= num%10;

		PTD->PSOR = FND_SEL[j];
		PTD->PCOR =0xff;
		PTD->PSOR = FND_DATA[num3];
	    delay_ms(Dtime/300);
		PTD->PCOR = 0xfff;
		j++;

		PTD->PSOR = FND_SEL[j];
		PTD->PCOR =0xff;
		PTD->PSOR = FND_DATA[num2];
	    delay_ms(Dtime/300);
   	    PTD->PCOR = 0xfff;
		j++;

		PTD->PSOR = FND_SEL[j];
		PTD->PCOR =0xff;
		PTD->PSOR = FND_DATA[num1];
	    delay_ms(Dtime/300);
	    PTD->PCOR = 0xfff;
		j++;

		PTD->PSOR = FND_SEL[j];
		PTD->PCOR =0xff;
		PTD->PSOR = FND_DATA[num0];
	    delay_ms(Dtime/300);
  	    PTD->PCOR = 0xfff;
		j=0;
		}
}
