#include "device_registers.h"
#include "clocks_and_modes.h"
int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */


void LPIT0_init (uint32_t delay)
{
   uint32_t timeout;
   /*!
    * LPIT Clocking:
    * ==============================
    */
  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs */

  /*!
   * LPIT Initialization:
   */
  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
                                        /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                        /* SW_RST=0: SW reset does not reset timer chans, regs */
                                        /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

  timeout=delay* 40000;
  LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
  LPIT0->TMR[0].TCTRL |=  0x11;//.................LPIT_TMR_TCTRL_T_EN_MASK;
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
   while (0 == (LPIT0->MSR &  0x01/*LPIT_MSR_TIF0_MASK*/)) {} /* Wait for LPIT0 CH0 Flag */
               lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
               LPIT0->MSR |= 0x00;//............LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void PORT_init (void)
{
     /*
      * ===============PORTE SEGMENT=====================
      */
     PCC-> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
     PTD->PDDR |= 1<<1| 1<<2| 1<<3| 1<<4| 1<<5| 1<<6| 1<<7;   /* Port D1-D7: Data Direction= output (default) */
     PORTD->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
     PORTD->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
     PORTD->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
     PORTD->PCR[4] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
     PORTD->PCR[5] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
     PORTD->PCR[6] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
     PORTD->PCR[7] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */

     PTD->PDDR |=1<<10|1<<11; /* Port D10-D11: Data Direction= output (default) */
     PORTD->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO, first display*/
     PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO , second display */
     
     PTD->PDDR &= 0<<12;  /*Port D12 is connect sw1 of board, portD is configured as input*/
     PORTD->PCR[12] = PORT_PCR_MUX(1);
}

void num (int nom){
	/*
	 	   A
		ㅡㅡㅡㅡㅡ
	      ㅣ	    ㅣ
	 F ㅣ	    ㅣB
	      ㅣ    G  ㅣ
	      ㅡㅡㅡㅡㅡㅡ
	      ㅣ	    ㅣ
	  Eㅣ	    ㅣC
	      ㅣ	    ㅣ
	      ㅡㅡㅡㅡㅡㅡ    ㅇ DOT
		   D
	*/
   switch(nom){
   case 0:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PSOR |= 1<<5;//PTD5; // FND E ON
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PCOR |= 1<<7;//PTD7; // 
         break;
      case 1:
         PTD-> PCOR |= 1<<1;//PTD1; //
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PCOR |= 1<<4;//PTD4; //
         PTD-> PCOR |= 1<<5;//PTD5; //
         PTD-> PCOR |= 1<<6;//PTD6; //
         PTD-> PCOR |= 1<<7;//PTD7; //
         break;
      case 2:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PCOR |= 1<<3;//PTD3; //
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PSOR |= 1<<5;//PTD5; // FND E ON
         PTD-> PCOR |= 1<<6;//PTD6; //
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;
      case 3:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PCOR |= 1<<5;//PTD5; //
         PTD-> PCOR |= 1<<6;//PTD6; //
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;
      case 4:
         PTD-> PCOR |= 1<<1;//PTD1; //
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PCOR |= 1<<4;//PTD4; //
         PTD-> PCOR |= 1<<5;//PTD5; //
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;
      case 5:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PCOR |= 1<<2;//PTD2; // 
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PCOR |= 1<<5;//PTD5; //
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;
      case 6:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PCOR |= 1<<2;//PTD2; // 
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PSOR |= 1<<5;//PTD5; // FND E ON
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;
      case 7:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PCOR |= 1<<4;//PTD4; //
         PTD-> PCOR |= 1<<5;//PTD5; //
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PCOR |= 1<<7;//PTD7; //
         break;
      case 8:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PSOR |= 1<<5;//PTD5; // FND E ON
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;
      case 9:
         PTD-> PSOR |= 1<<1;//PTD1; // FND A ON
         PTD-> PSOR |= 1<<2;//PTD2; // FND B ON
         PTD-> PSOR |= 1<<3;//PTD3; // FND C ON
         PTD-> PSOR |= 1<<4;//PTD4; // FND D ON
         PTD-> PCOR |= 1<<5;//PTD5; //
         PTD-> PSOR |= 1<<6;//PTD6; // FND F ON
         PTD-> PSOR |= 1<<7;//PTD7; // FND G ON
         break;

   }

}

void Seg_out(int number){
   int j;
   int d10, d1;

                    d10 = number % 100/10;
                    d1 = number % 10;

                           for( j = 0 ; j < 10 ; j++){

                        //10
                        num(d10);
                        PTD-> PSOR |= 1<<10;
                        PTD-> PCOR |= 1<<11;
                        delay_ms(2);

                        // 1
                        num(d1);
                        PTD-> PCOR |= 1<<10;
                        PTD-> PSOR |= 1<<11;
                        delay_ms(2);

                        }


}


int main(void)
{
   int count = 94;
   int count1 = 00;

   PORT_init();            /* Configure ports */
   SOSC_init_8MHz();       /* Initialize system oscilator for 8 MHz xtal */
   SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
   NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */

   while(1){
        
        if(PTD->PDIR &(1<<12)){   //7 segment와 led랑 햇갈리지 말자. 항상 high 상태인 세그먼트가 sw를 누르면 bjt가 on 되면서 ground와 연결된 path가 활성화 되어 0V의 상태가 된다. 즉 sw누르지 않았을때 조건 성립
            Seg_out(count);


         /* if문의 조건을 sw를 켰을때 만족하게 하려면 
         if(PTD->PDIR ^(1<<12)){
            ~~
         }
         
         
         */
        
        }else{
            seg_out(count1);

        }

   }
}
