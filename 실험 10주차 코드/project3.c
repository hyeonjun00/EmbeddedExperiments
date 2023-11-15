#include "device_registers.h"
#include "clocks_and_modes.h"
int lpit0_ch0_flag_counter = 0;
int lpit0_ch1_flag_counter = 0;
int lpit0_ch2_flag_counter = 0;

unsigned int num, num0, num1, num2, num3 = 0;
unsigned int j = 0;
unsigned int toggle = 0;
unsigned int FND_DATA[10] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xCE};
unsigned int Delaytime = 0;
unsigned int FND_SEL[4] = {0x0100, 0x0200, 0x0400, 0x0800};

unsigned int External_PIN=0;

void NVIC_init_IRQs(void)
{
	S32_NVIC->ICPR[1] = 1 << (48 % 32);
	S32_NVIC->ISER[1] = 1 << (48 % 32);
	S32_NVIC->IP[48] = 0x0A;
	S32_NVIC->ICPR[1] = 1 << (49 % 32);
	S32_NVIC->ISER[1] = 1 << (49 % 32);
	S32_NVIC->IP[49] = 0x0B;
	S32_NVIC->ICPR[1] = 1 << (50 % 32);
	S32_NVIC->ISER[1] = 1 << (50 % 32);
	S32_NVIC->IP[50] = 0x0C;


}
void PORT_init(void)
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


	  PTD->PDDR |= 1<<8|1<<9|1<<10|1<<11<12;
	  PORTD->PCR[8] = PORT_PCR_MUX(1); /* Port D8: MUX = GPIO */
	  PORTD->PCR[9] = PORT_PCR_MUX(1); /* Port D9: MUX = GPIO */
	  PORTD->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
	  PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */
      PORTD->PCR[12] = PORT_PCR_MUX(1); /*port D12*/

	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
	PTC->PDDR &= ~(1 << 11);
	PORTC->PCR[11] |= PORT_PCR_MUX(1);
	
}
void LPIT0_init(void)
{
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;
	LPIT0->MCR = 0x00000001;

	LPIT0->MIER = 0b0111;
	LPIT0->TMR[0].TVAL = 20000000;
	LPIT0->TMR[0].TCTRL = 0x00000001;
	LPIT0->TMR[1].TVAL = 20000000;
	LPIT0->TMR[1].TCTRL = 0x00000001;
	LPIT0->TMR[2].TVAL = 1 * 40000;
	LPIT0->TMR[2].TCTRL = 0x00000001;
}
void WDOG_disable(void)
{
	WDOG->CNT = 0xD928C520;
	WDOG->TOVAL = 0x0000FFFF;
	WDOG->CS = 0x00002100;
}
void LPIT0_Ch0_IRQHandler(void)
{
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK;
	lpit0_ch0_flag_counter++;
	toggle = !toggle;
}
void LPIT0_Ch1_IRQHandler(void)
{
	LPIT0->MSR |= LPIT_MSR_TIF1_MASK;
	lpit0_ch1_flag_counter++;
	
	if ((PTC->PDIR)&(1<<11)==0){
	num++;
	}
}
void LPIT0_Ch2_IRQHandler(void)
{
	LPIT0->MSR |= LPIT_MSR_TIF2_MASK;
	lpit0_ch2_flag_counter++;
	j++;
}

void Seg_out2(int toggle)
{
	if (toggle)
		PTD->PSOR |= 1 << 12;
	else
		PTD->PCOR |= 1 << 12;
}
void Seg_out(int number)
{


	num3 = (number / 1000) % 10;
	num2 = (number / 100) % 10;
	num1 = (number / 10) % 10;
	num0 = number % 10;

	switch (j)
	{
	case 0:
		// 1000자리수 출력
		PTD->PCOR = 0xfff;
		PTD->PSOR = FND_SEL[j];
		PTD->PCOR = 0x7f;
		PTD->PSOR = FND_DATA[num3];
		// delay_ms();
		break;

	case 1:
		// 100자리수 출력
		PTD->PCOR = 0xfff;
		PTD->PSOR = FND_SEL[j];
		PTD->PCOR = 0x7f;
		PTD->PSOR = FND_DATA[num2];
		// delay_ms();
		break;

	case 2:
		// 10자리수 출력
		PTD->PCOR = 0xfff;
		PTD->PSOR = FND_SEL[j];
		PTD->PCOR = 0x7f;
		PTD->PSOR = FND_DATA[num1];
		// delay_ms();
		break;

	case 3:
		// 1자리수 출력
		PTD->PCOR = 0xfff;
		PTD->PSOR = FND_SEL[j];
		PTD->PCOR = 0x7f;
		PTD->PSOR = FND_DATA[num0];
		// delay_ms();
		break;

	default:
		j = 0;
		break;
	}
}
int main(void)
{
	WDOG_disable();
	PORT_init();
	SOSC_init_8MHz();
	SPLL_init_160MHz();
	NormalRUNmode_80MHz();
	NVIC_init_IRQs();
	LPIT0_init();
	for (;;)
	{

		num = num % 16;
		Seg_out(num);
		Seg_out2(toggle);		
	}
}