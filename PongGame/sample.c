//PONG GAME

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "adc/adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "button_EXINT/button.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
	
#define SCREEN_WIDTH 320	//window height
#define SCREEN_HEIGHT 240	//window width

typedef struct ball_s { //ball representation
	int x, y; /* position on the screen */
	int w,h; // ball dimensions
	int dx, dy; /* movement vector */
} ball_t;

typedef struct paddle { //paddle representation
	int x,y;
	int w,h;
} paddle_t;


// Program global variables

ball_t ball;
paddle_t paddle;
paddle_t paddle_cpu;
int cpu_moving_direction;
int score= 0;
int score_cpu=0;
char score_in_char[1]="";
char score_cpu_in_char[1]="";

int isGameRunning=0; //if 1 game is running, if 0 it is in pause
int paddleSound=0; //if 1 the ball hit the paddle, else the ball hit the wall

//Program functions

//if return is 1 a collision between paddle and ball occured, else there isn't a collision.
int check_collision(ball_t a, paddle_t b) {
	
	//the function check if the pixel area which represent the ball is over the pixel area of the paddle
	
	int left_a, left_b;
	int right_a, right_b;
	int top_a, top_b;
	int bottom_a, bottom_b;

	left_a = a.x;
	right_a = a.x + a.w;
	top_a = a.y;
	bottom_a = a.y + a.h;

	left_b = b.x;
	right_b = b.x + b.w;
	top_b = b.y;
	bottom_b = b.y + b.h;

	if (left_a > right_b) {
		return 0;
	}
	if (right_a < left_b) {
		return 0;
	}
	if (top_a > bottom_b) {
		return 0;
	}
	if (bottom_a < top_b) {
		return 0;
	}
	return 1;
}

void draw_playerScore() { //draw on the screen the actual player's score
	
	int scoreX=7;
	int scoreY=120;	
	sprintf(score_in_char,"%4d",score);
	GUI_Text(scoreX,scoreY,(uint8_t *) score_in_char,White, Black);
	
}

void draw_cpuScore() { //draw on the screen the actual player's score
	
	int scoreX=190;
	int scoreY=120;	
	sprintf(score_cpu_in_char,"%4d",score_cpu);
	GUI_Text(scoreX,scoreY,(uint8_t *) score_cpu_in_char,White, Black);
	
}


void drawRect(uint16_t x0,uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){// draw on the screen a rectangle in a defined position and of a define color. It works using LCD_DrawLine(...) function
	
	int i=y0;
	for(i=y0; i<y1; i++){
		LCD_DrawLine(x0,i,x1,i, color);
	}

}

void draw_ball() { //draw on the screen the ball in the right position. It works using drawRect function
	
	int i=ball.x;
	int j=ball.y;
	drawRect(ball.x,ball.y, ball.x+ball.w, ball.y+ball.h, Green);
	
}

void goal(){
	
	if(score<5 && score_cpu<5){
		ball.x = 90;
		ball.y = 50 ;
		ball.w = 5;
		ball.h = 5;
		ball.dy = 2;
		ball.dx = 2;
	}
	else if(score==5){
		disable_timer(2);
		disable_timer(3);
		GUI_Text(80,190,"You Win!",Green, Black);
		GUI_Text(80,120,"You Lose!",Green, Black);
	}
	else if(score_cpu==5){
		disable_timer(2);
		disable_timer(3);
		GUI_Text(80,120,"You Win!",Green, Black);
		GUI_Text(80,190,"You Lose!",Green, Black);
	}
	
}

/* This routine moves each ball by its motion vector. */
 void move_ball() {

		int i;
		int hit_pos;
		int c;
		int c_cpu; 
	 
	 drawRect(ball.x,ball.y, ball.x+ball.w, ball.y+ball.h, Black);	 

	/* Move the ball by its motion vector. */
	ball.x += ball.dx;
	ball.y += ball.dy;
	 
	 c=check_collision(ball, paddle); 
	 c_cpu=check_collision(ball, paddle_cpu);
	 
	/* Change the ball movement vector whenever it touches the wall*/
	 
	if (ball.y > 300) { //if the ball goes under the paddle level, cpu gain one point and the game restart.
		score_cpu++;
		draw_cpuScore();
		goal();	 
	}

	if (ball.y < 15) {
		score++;
		draw_playerScore();
		goal();
	}
	
	if (ball.x < 9) {
		paddleSound=0;
		enable_timer(0);
		ball.dx=-ball.dx;
	}

	if (ball.x > 225) {
		paddleSound=0;
		enable_timer(0);
			ball.dx=-ball.dx;
	}

		if (c == 1) {//if a collision is detected	
										
			//change ball direction to up
			ball.dy=-ball.dy;
			
			//change ball angle depending on where it hit the paddle
			hit_pos = (paddle.x + paddle.w) - ball.x;
			
			
			if (hit_pos >= 0 && hit_pos < 5) {
				ball.dx = 4;
			}

			if (hit_pos >= 5 && hit_pos < 10) {
				ball.dx = 3;
			}
			
			if (hit_pos >= 10 && hit_pos < 15) {
				ball.dx = 2;
			}

			if (hit_pos >=15 && hit_pos < 20) {
				ball.dx = 1;
			}

			if (hit_pos >= 20 && hit_pos < 30) {
				ball.dx = 0;
			}

			if (hit_pos >= 30 && hit_pos < 35) {
				ball.dx = -1;
			}

			if (hit_pos >= 35 && hit_pos < 40) {
				ball.dx = -2;
			}

			if (hit_pos >= 40 && hit_pos < 45) {
				ball.dx = -3;
			}

			if (hit_pos >= 45 && hit_pos <= 50) {
				ball.dx = -4;
			}
				paddleSound=1;
				enable_timer(0);
		}
		
		if(c_cpu==1){
		
			//change ball direction to up
			ball.dy=-ball.dy;
			
			//change ball angle depending on where it hit the paddle
			hit_pos = (paddle_cpu.x + paddle_cpu.w) - ball.x;
			
			
			if (hit_pos >= 0 && hit_pos < 5) {
				ball.dx = 4;
			}

			if (hit_pos >= 5 && hit_pos < 10) {
				ball.dx = 3;
			}
			
			if (hit_pos >= 10 && hit_pos < 15) {
				ball.dx = 2;
			}

			if (hit_pos >=15 && hit_pos < 20) {
				ball.dx = 1;
			}

			if (hit_pos >= 20 && hit_pos < 30) {
				ball.dx = 0;
			}

			if (hit_pos >= 30 && hit_pos < 35) {
				ball.dx = -1;
			}

			if (hit_pos >= 35 && hit_pos < 40) {
				ball.dx = -2;
			}

			if (hit_pos >= 40 && hit_pos < 45) {
				ball.dx = -3;
			}

			if (hit_pos >= 45 && hit_pos <= 50) {
				ball.dx = -4;
			}
				paddleSound=1;
				enable_timer(0);
		}
		
		draw_ball();
		if(ball.x >180 && ball.y>110 && ball.x<220 && ball.y<130) 
			draw_cpuScore();

		if(ball.x >0 && ball.y>100 && ball.x<40 && ball.y<140) 
			draw_playerScore();	
		
}

void draw_background() { //draw the background on the screen, characterized by a black background with the red walls 

	LCD_Clear(Black);
	//draw red walls
	drawRect(0,0,5,320, Red);
	drawRect(235,0,240,320, Red);
	
}

void draw_paddle(void) { //draw the paddle on the screen, in the right position. It works using drawRect function

drawRect(paddle.x, paddle.y, paddle.x +paddle.w, paddle.y+paddle.h, White);
		
}

void draw_cpu_paddle(void){

	drawRect(paddle_cpu.x, paddle_cpu.y, paddle_cpu.x +paddle_cpu.w, paddle_cpu.y+paddle.h, White);
	
}

void move_paddle(uint16_t newX) { //change the paddle position and draw it in the new position, deleting the previous one
	
	if (newX>0 && newX<190){
	drawRect(paddle.x,paddle.y,paddle.x+paddle.w,paddle.y+paddle.h, Black); 
	paddle.x=newX;
	draw_paddle();
	}
	
}

void move_paddle_cpu() { //change the paddle position and draw it in the new position, deleting the previous one
	
		if( paddle_cpu.x>175)
		cpu_moving_direction=-1;
		
		if( paddle_cpu.x<15)
			cpu_moving_direction=1;
	
	if(cpu_moving_direction==1){
		drawRect(paddle_cpu.x,paddle_cpu.y,paddle_cpu.x+paddle_cpu.w,paddle_cpu.y+paddle_cpu.h, Black); 
		paddle_cpu.x += 8;
		draw_cpu_paddle();
	}
	else if(cpu_moving_direction==-1){
		drawRect(paddle_cpu.x,paddle_cpu.y,paddle_cpu.x+paddle_cpu.w,paddle_cpu.y+paddle_cpu.h, Black); 
		paddle_cpu.x -= 8;
		draw_cpu_paddle();
	}
}

 void init_game(void) {//start the game, defining the initial position  of the ball and the paddle. There's also the drawing of the graphic components amd the initialization of timer2, which is responsible of ball's movement.
	
	ball.x = 90;
	ball.y = 50 ;
	ball.w = 5;
	ball.h = 5;
	ball.dy = 2;
	ball.dx = 2;
	
	paddle.x = 120-25;
	paddle.y = 320-42 ;
	paddle.w = 50;
	paddle.h = 10;
	 
	paddle_cpu.x = 120-25;
	paddle_cpu.y = 32 ;
	paddle_cpu.w = 50;
	paddle_cpu.h = 10; 
	
	score=0;
	score_cpu=0;
	isGameRunning=1;
	cpu_moving_direction=1;
	draw_background();
	draw_paddle();
	draw_cpu_paddle();
	draw_ball();
	draw_cpuScore();
	draw_playerScore();
	init_timer(2, 0xAB40);
	init_timer(3, 0xAAB40);

}

void start_game(void){

	isGameRunning=1;
	enable_timer(2);
	enable_timer(3);
	
}

void pause_game(void){ //this function pause and start the game. It is related to key2 button pression  
	
	if(isGameRunning==1){
	disable_timer(2);
	disable_timer(3);
	isGameRunning=0;
	}
	else{
	enable_timer(2);
	enable_timer(3);
	isGameRunning=1;
	}
}

void reset_game(void){//the function reset the game to initial settings. It is related to Int0 button pression.

	score_cpu=0;
	score=0;
	init_game();
	isGameRunning=0;
	
}

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
  LCD_Initialization();
	init_timer(0, 0xC8 );									/* 8us * 25MHz = 200 ~= 0xC8 */ 
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	BUTTON_init();
	enable_RIT();													/* RIT enabled	*/
	ADC_init();														/* ADC Initialization									*/	

	//GAME OPERATIONS
	init_game(); 
	isGameRunning=0;
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
