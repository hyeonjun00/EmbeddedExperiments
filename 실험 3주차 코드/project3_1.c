PORTC_PCR_BASE EQU 0x4004B000
PORTD_PCR_BASE EQU 0x4004C000
PCR_C12 EQU 0x4004B000+0x30 ;12*4
PCR_D0 EQU 0x4004C000
	
GPIOA EQU 0x400FF000
GPIOB EQU 0x400FF040
GPIOC EQU 0x400FF080
GPIOD EQU 0x400FF0C0
PDDR_C EQU 0x400FF080+0x14
PDDR_D EQU 0x400FF0C0+0x14
PIDR_C EQU 0x400FF080+0x18
	
PCC_BASE EQU 0x40065000
PCC_PORTC EQU 0x40065000+0x12C
PCC_PORTD EQU 0x40065000+0x130
CLOCK_EN EQU 0x40000000 ; In S32K14x, GPIO can only be accessed by the core through the cross bar interface at 0x400F_F00
	
	AREA mycode, CODE
	ENTRY
	EXPORT __main
__main
	LDR r0,=PDDR_C ; GPIO Controller, Port C Port Data Diretion Setting , PDDR offset 14h , PortC Base 400FFF080, Portc Diretion 
	LDR r1,=PCC_PORTC ; PCC_PORTC Registor= 12C, PCC Base address Portc Clock = 4006_5000h
	LDR r2,=CLOCK_EN ;  In S32K14x, GPIO can only be accessed by the core through the cross bar interface at 0x400F_F00
	
	STR r2,[r1,#0x00] ; PCC Clock Setting C, r2? [r1? ??? ?? + #0x00]? ?? ??? ??
	STR r2,[r1,#0x04] ; PCC Clock Setting D r2? [r1? ??? ?? + #0x04]? ?? ??? ??
	
	LDR           r1,[r0,#0x00] ; r1= contents of [PDDR_C]
	BIC           r1,r1,#0x1000 ;C12 is configured as input  
	STR           r1,[r0,#0x00] ; PDDR_C <-- 0x1000 D0 is input
	
	LDR			  r1,=PCR_C12 ; Pin control Registor, PORTC multiplexinh contol base 0x4004B000,  /* Port C12: MUX = GPIO, input filter enabled */
	MOV           r2,#0x110 ; r2<--0x110 
	STR           r2,[r1,#0x00] ; PCR_C12 is configured as GPIO&PFE
	
	LDR           r2,[r0,#0x40] ; r2= contents of [PDDR_D] 
	ORR           r2,r2,#0x01 ; r2= 0x01
	STR           r2,[r0,#0x40] ;PDDR_D0 is configured as output
	
	MOV           r2,#0x100 ; r2=0x100
	LDR			  r1,=PCR_D0
	STR           r2,[r1,#0x00] ; PCR_D0 is configured as GPIO
	B             NOT_PUSH ; NOT PUSH Function?? ??
ON_PUSH
	LDR           r1,[r0,#0x34] ; r1 = contents of [PDDR_C + 0x34 = PCOR_D]
	ORR			  r1,r1,#0x01 ; Clear D0
	STR           r1,[r0,#0x34] ;
NOT_PUSH
	LDR           r1,[r0,#-0x04] ; r1 = contents of [PDDR_C - 0x04 = PDIR_C], Read C12 DATA
	LSLS          r1,r1,#19 ; Move C12 value to MSB
	BMI           ON_PUSH ; MSB = 1?
	LDR           r1,[r0,#0x30] ; r1 = contets of [PDDR_C + 0x30 = PSOR_D]
	ORR           r1,r1,#0x01 ; Set D0 '1'
	STR           r1,[r0,#0x30] ; 
	B             NOT_PUSH
	END
