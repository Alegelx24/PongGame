/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include "../button_EXINT/button.h"
#include "../timer/timer.h"
#include "../TouchPanel/TouchPanel.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern uint8_t button;

int down_INT0=0;
int down_KEY1=0;
int down_KEY2=0;

void RIT_IRQHandler (void)
{						
	static int down=0;	
	down++;
	
	/* ADC management */
		ADC_start_conversion();		
	
	// k2 -> GPIO2 1<<12; eint2; PINSEL 1<<24
	// k1 -> GPIO2 1<<11; eint1; PINSEL 1<<22
	// int0 -> GPIO2  1<<10; eint0; PINSEL 1<<20
	
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0 || (LPC_GPIO2->FIOPIN & (1<<11)) == 0 || (LPC_GPIO2->FIOPIN & (1<<10)) == 0){
		reset_RIT();
		switch(down){ 
			case 1:
				switch(button){//key1
					case 1:
							reset_RIT();
							LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
					start_game();
					break;
					
					case 2://key2
						reset_RIT();
						LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */ 
						pause_game();
					
					break;
						
					case 3: //eint0
						reset_RIT();
						LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
						reset_game();
					
					break;
					default:
					break;
				}
			break;
			default:
			break;
		 }
	}
	else {	/* button released */
		down=0;			
		disable_RIT();
		reset_RIT();
		NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
		NVIC_EnableIRQ(EINT0_IRQn);
		NVIC_EnableIRQ(EINT2_IRQn);
		LPC_PINCON->PINSEL4    |= (1 << 20);
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		LPC_PINCON->PINSEL4    |= (1 << 24);
	}

	enable_RIT();	
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
