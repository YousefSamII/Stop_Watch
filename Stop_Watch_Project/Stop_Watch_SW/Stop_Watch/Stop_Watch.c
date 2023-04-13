/*
 * Stop_Watch_ES.c
 *
 *  Created on: Apr 11, 2023
 *      Author: YOUSEF SAMY
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
typedef unsigned char uint8;
/*Prototypes :)*/
void Stop_Watch(void);
void Timer1(void);
void Interrupt0(void);
void Interrupt1(void);
void Interrupt2(void);
/*Array depends on digit changes */
uint8 digit[6];
uint8 i;
/*flags*/
uint8 g_timer_flag=0;
uint8 g_interrupt_flag=0;
/*ISR*/
ISR(TIMER1_COMPA_vect)
{
	g_timer_flag=1;
}
ISR(INT0_vect)
{
	g_interrupt_flag=1;
}
ISR(INT1_vect)
{
	TIMSK&=~(1<<OCIE1A);
}
ISR(INT2_vect)
{
	TIMSK|=(1<<OCIE1A);
}
/*Main Function*/
int main(void)
{
	DDRA|=0x3F;               		//Output PIN PA0-->PA5
	DDRC|=0x0F;						//Output PIN PC0-->PC3
	PORTC &=~0x0F; 					//7-segment displays 0 as initial value
	PORTA |=0x3F;					//All Transistor on
	g_timer_flag= 0;
	g_interrupt_flag=0;
	Timer1();
	Interrupt0();
	Interrupt1();
	Interrupt2();
	while(1)
	{
		for(i=0;i<6;i++)
		{
			PORTA = (PORTA & 0xC0) | (1<<i);
			PORTC=(PORTC&0xF0) |(digit[i] & 0x0F);
			_delay_ms(2);
		}

		if(g_timer_flag==1)
		{
			Stop_Watch();
			g_timer_flag=0;
		}
		if(g_interrupt_flag==1)
		{
			for(i=0;i<6;i++)
			{
				digit[i]=0;
			}
			g_interrupt_flag=0;
		}

	}
}
/*Start Run for Watch Function*/
void Stop_Watch(void)
{
	digit[0]++;
	if(digit[0]>9)
	{
		digit[0]=0;
		digit[1]++;
		if(digit[1]==6)
		{
			digit[2]++;
			digit[1]=0;
		}
	}
	if(digit[2]>9)
	{
		digit[3]++;
		digit[2]=0;
		if(digit[3]==6)
		{
			digit[3]=0;
			digit[4]++;
		}
	}
	if(digit[4]>9)
	{
		digit[5]++;
		digit[4]=0;
		if(digit[5]==9)
		{
			for(i=0;i<6;i++)
			{
				digit[i]=0;
			}
		}
	}
}
/*use timer1 CTC mode*/
void Timer1(void)
{
	TCCR1B|=(1<<WGM12)|(1<<CS12)|(1<<CS10);
	TCNT1=0;
	OCR1A=977;
	TIMSK|=(1<<OCIE1A);
	SREG|=(1<<7);

}
void Interrupt0(void)
{
	DDRD&=~(1<<PD2); 					//PD2 Input PIN
	PORTD|=(1<<PD2);            		//Active internal pull up
	/*Enable Falling edge for interrupt 0*/
	MCUCR|=(1<<ISC01);
	MCUCR&=~(1<<ISC00);
	/*Enable Extrnal INT0 Request*/
	GICR|=(1<<INT0);
	/*Enable I-bit*/
	SREG|=(1<<7);
}
void Interrupt1(void)
{
	DDRD&=~(1<<PD3);					//Make I/p pins PD3
	/*Enable Raising Edge for Interrupt1*/
	MCUCR|=(1<<ISC11)|(1<<ISC10);
	/*Enable Extrnal INT1 Request*/
	GICR|=(1<<INT1);
	/*Enable I-bit*/
	SREG|=(1<<7);
}
void Interrupt2(void)
{
	DDRB&=~(1<<PB2);				//PB2 input pin
	PORTB|=(1<<PB2);				//Active internal Pullup
	/*Falling edge for INT2*/
	MCUCSR&=~(1<<ISC2);
	/*Enable Extrnal INT2 Request*/
	GICR|=(1<<INT2);
	/*Enable I-bit*/
	SREG|=(1<<7);
}












