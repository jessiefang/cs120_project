/*	Author: Yunjie Fang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#include "scheduler.h"
#include "nokia5110.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//unsigned char input = ~PINA & 0x07;

#define input (~PINA & 0x07) 
unsigned char pos_x = 0;
unsigned char pos_y = 0;
enum MenuStates {MenuStart, Wait, Release, Up, Down, MenuRestart};
int ButtonTick(int state){
	switch(state){
		case MenuStart:
			state = Wait;
			break;
		case Wait:
			if(input == 0x01){
				state = Down;
			}else if(input == 0x02){
				state = Up;
			}else if(input == 0x08){
				state = MenuRestart;
			}
			else{
				state = MenuStart;
			}
			break;
		case Down:
			state = Release;
			break;
		case Up:
			state = Release;
			break;
		case Release:
			if(input == 0x00){
				state = Wait;
			}else{
				state = Release;
			}
			break;
		case MenuRestart:
			state = MenuStart;
			break;
		default:
			state = MenuStart;
			break;
	}
	switch(state){
		case MenuStart:
			break;
		case Wait:
			break;
		case Down:
			pos_y++;
			break;
		case Up:
			pos_y--;
			break;
		case Release:
			break;
		case MenuRestart:
			break;
		default:
			break;
	}
	return state;
}


enum LCDStates {Start, LCD, Restart};
//unsigned char text[] = "' ',' ',' ',' ','#',' ',' ',' ',' ','#'";
//unsigned char text[] = "          #                #               #                    #";
//unsigned char lose[] = "game over";
unsigned char text[84]="'#'";
unsigned char thing = '#';
unsigned char space[] = "' '";
unsigned char i = 0x00;
unsigned char j = 0x00;
int LCDTick(int state){
	switch(state){
		case Start:
			if(pos_y == 1 && input == 0x04){
				state = LCD;
			}else{
				state = Start;
			}
			break;
		case LCD:
				state = LCD;
			break;
		case Restart:
			if (input == 0x08){
				state = Start;
			}
			break;
		default:
			state = Start;
			break;
	}
	switch(state){
		case Start:
			nokia_lcd_clear();
                        nokia_lcd_write_string("Menu",1);
                        nokia_lcd_set_cursor(0,10);
                        nokia_lcd_write_string("Start",1);
			nokia_lcd_set_cursor(0,20);
			nokia_lcd_write_string("Record",1);
			nokia_lcd_render();
			break;
		case LCD:
			nokia_lcd_clear();
			for(int i = 79; i>=0; i--) {
			  nokia_lcd_clear();
			  if(i%79 == 0){
				  nokia_lcd_set_cursor(79, 0);
				  nokia_lcd_write_char(thing,1);
			  }
			  nokia_lcd_set_cursor(i,0);
			  nokia_lcd_write_char(thing, 1);
			  nokia_lcd_render();
			}

			nokia_lcd_set_cursor(pos_x,pos_y);
			break;
		case Restart:
			break;
		default:
			break;
	}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    //DDRB = 0xFF; PORTB = 0x00;
   // DDRC = 0xFF; PORTC = 0x00;
    //DDRD = 0xFF; PORTD = 0x00;
    
    /* Insert your solution below */
    static task task1, task2;
    task *tasks[] = { &task1, &task2 };
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;
    /* Task 1 */
    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &ButtonTick;

    task2.state = start;
    task2.period = 20;
    task2.elapsedTime = task2.period;
    task2.TickFct = &LCDTick;

    TimerSet(10);
    TimerOn();
    nokia_lcd_init();
    unsigned short i;
    while (1) {
	for (i = 0; i < numTasks; i++ ) {
		if (tasks[i]->elapsedTime == tasks[i]->period) {
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
