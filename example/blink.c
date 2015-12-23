#include <msp430g2553.h>
#include <stdlib.h>
#include "kernel.h"

//define tasks
void task1(void);
void task2(void);


int main(){

	DCOCTL = CALDCO_1MHZ;//Use DCOCLK as MCLK 1MHz
	BCSCTL1 = CALBC1_1MHZ;	
	BCSCTL2|= SELS;
	BCSCTL3|= LFXT1S_2;	
	
	TaskInit(&task1,0); //Initialize task 1 & 2
	TaskInit(&task2,1);
	P1DIR |= (1<<7);
	P1OUT &= ~(1<<7);

	RunTask(0);	// run tasks

	return 0;
}

/* user task #1 to blink LED*/
void task1(void){
	
	P1DIR |= (1<<0); //set direction of pin to output

	while(1){
	
		P1OUT ^= (1<<0); //toggle pin

	}
}
/* user task #2 if button pressed it turns on LED*/
void task2(void){
	
	//set pin to input and enable pull up resistor
	P1DIR &= ~(1<<3);
	P1REN |= (1<<3);
	P1OUT |= (1<<3);

	//set pin to output
	P1DIR |= (1<<6);
	
	while(1){
		
		
		if(!(P1IN & (0x08))){//check if button is pressed or not
			
			P1OUT &= ~(1<<6); //turn on LED
		}
		else{
			
			P1OUT |= (1<<6); //turn off LED
		}
		
	}
}
