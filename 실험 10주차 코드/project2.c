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

void NVIC_init_IRQs(void)
{
	S32_NVIC->ICPR[1] = 1 << (48 % 32);
	S32_NVIC->ISER[1] = 1 << (48 % 32);
	S32_NVIC->IP[48] = 0x01;
	S32_NVIC->ICPR[1] = 1 << (49 % 32);
	S32_NVIC->ISER[1] = 1 << (49 % 32);
	S32_NVIC->IP[49] = 0x02;
	S32_NVIC->ICPR[1] = 1 << (50 % 32);
	S32_NVIC->ISER[1] = 1 << (50 % 32);
	S32_NVIC->IP[50] = 0x03;
}
void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
	PTD->PDDR |= 0b1111111111110; //1,2,3,4,5,6,7,8,9,10,11,12

	PORTD->PCR[1] = PORT_PCR_MUX(1);
	PORTD->PCR[2] = PORT_PCR_MUX(1);
	PORTD->PCR[3] = PORT_PCR_MUX(1);
	PORTD->PCR[4] = PORT_PCR_MUX(1);
	PORTD->PCR[5] = PORT_PCR_MUX(1);
	PORTD->PCR[6] = PORT_PCR_MUX(1);
	PORTD->PCR[7] = PORT_PCR_MUX(1); 
	PORTD->PCR[8] = PORT_PCR_MUX(1);
	PORTD->PCR[9] = PORT_PCR_MUX(1);
	PORTD->PCR[10] = PORT_PCR_MUX(1);
	PORTD->PCR[11] = PORT_PCR_MUX(1);
	PORTD->PCR[12] = PORT_PCR_MUX(1);
}
void LPIT0_init(void)
{
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;
	LPIT0->MCR = 0x00000001;

	LPIT0->MIER = 0b0111; /* TIE0=1: Timer Interrupt Enabled for Chan 0,1,2 */
	LPIT0->TMR[0].TVAL = 20000000;  // 0.5s
	LPIT0->TMR[0].TCTRL = 0x00000001;
	LPIT0->TMR[1].TVAL = 40000000;  //1s
	LPIT0->TMR[1].TCTRL = 0x00000001;
	LPIT0->TMR[2].TVAL = 1 * 40000;  //40000/40M   ->0.001s
	LPIT0->TMR[2].TCTRL = 0x00000001;
}
void WDOG_disable(void)
{
	WDOG->CNT = 0xD928C520;
	WDOG->TOVAL = 0x0000FFFF;
	WDOG->CS = 0x00002100;
}
void LPIT0_Ch0_IRQHandler(void)  //0.5s의 SEG_CLOCK control
{
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK;
	lpit0_ch0_flag_counter++;
	toggle=!toggle;
}
void LPIT0_Ch1_IRQHandler(void)  //1s의 segment data contorl
{
	LPIT0->MSR |= LPIT_MSR_TIF1_MASK;
	lpit0_ch1_flag_counter++;
	num++;
}
void LPIT0_Ch2_IRQHandler(void) //0.001s의 dynamic 7segment display control 
{
	LPIT0->MSR |= LPIT_MSR_TIF2_MASK;
	lpit0_ch2_flag_counter++;
	j++;
}
void Seg_out2(int toggle){
	if (toggle)
		PTD->PSOR |= 1 << 12;
	else
		PTD->PCOR |= 1 << 12;

}

void Seg_out(int number)
{

	Delaytime = 1;

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
		j=0;
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
		Seg_out(num);
		Seg_out2(toggle);
		
	}
}