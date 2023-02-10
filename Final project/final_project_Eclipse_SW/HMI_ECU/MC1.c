/******************************************************************************
 *
 *
 * File Name: MC1.c
 *
 * Description: source file for the main of mc1
 *
 * Author: Mohamed Saad
 *
 *******************************************************************************/
#include "Keypad.h"
#include "lcd.h"
#include "uart.h"
#include "timer.h"
#include "avr/io.h"
#include "util/delay.h"
#include "avr/interrupt.h"


/**************************** Global Variables ****************************/
uint8 g_tick=0,seconds=0;

/************************** Functions Prototypes **************************/
void getPassword (uint8 password[]);
void sendPassword (uint8 password[]);
void wait_seconds(void);

/******************************** Main ********************************/
int main()
{
	uint8 password[5] , pass_confirmed = 0 , operation ,count=0;

	/*define Uart struct
	 * 8 bits of data,Disable parity ,One stop bit ,Baud rate=9600
	 */
	UART_ConfigType uart_Config={UART_8_BIT_PER_FRAME,UART_DISABLE_PARITY,UART_1_STOP_BIT,9600};
	/* Create configuration structure for TIMER driver with
	 * initial value = 0 & compare value = 7813
	 * which generates an interrupt every one second
	 */
	TIMER_ConfigType TIMER_Config = {TIMER1,TIMER_COMPARE_MODE,F_CPU_1024,0,7813};
	SREG|=(1<<7);   //Enable I-bit
	LCD_init();     //initialize the LCD
	UART_init(&uart_Config);   //initialize the UART

	while(1){


		/****************************** Step 1 ******************************/
		while(pass_confirmed==0)
		{
			LCD_clearScreen();
			LCD_displayString("Plz enter pass");
			LCD_moveCursor(1, 0);
			getPassword(password);
			_delay_ms(50);
			sendPassword(password);
			LCD_clearScreen();
			LCD_displayString("Plz re-enter the");
			LCD_moveCursor(1, 0);
			LCD_displayString("same pass:");
			LCD_moveCursor(1, 10);
			getPassword(password);
			_delay_ms(50);
			sendPassword(password);
			_delay_ms(1000);
			pass_confirmed=UART_recieveByte();
			_delay_ms(50);
		}
		/****************************** Step 2 ******************************/
		pass_confirmed=0;
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "- :Change Pass");
		LCD_displayStringRowColumn(1, 0, "+ :Open Door");
		operation = KEYPAD_getPressedKey();
		/****************************** Step 3 ******************************/
		if(operation=='+')
		{
			UART_sendByte(operation);
			while((count!=3)&& (!pass_confirmed))
			{
				_delay_ms(50);
				LCD_clearScreen();
				LCD_displayString("Plz enter pass");
				LCD_moveCursor(1, 0);
				getPassword(password);
				_delay_ms(50);
				sendPassword(password);
				pass_confirmed=UART_recieveByte();
				if(pass_confirmed)
				{
					count=0;
					LCD_clearScreen();
					LCD_displayString("Door Unlocking.");
					TIMER_setCallBack(wait_seconds);
					TIMER_Init(&TIMER_Config); // initialize timer1
					seconds=15;
					while(g_tick < seconds){};//wait 15 seconds
					g_tick=0;


					LCD_clearScreen();
					LCD_displayString("Door Opened");

					TIMER_setCallBack(wait_seconds);
					TIMER_Init(&TIMER_Config); // initialize timer1
					seconds=3;
					while(g_tick < seconds){};//wait 3 seconds
					g_tick=0;

					LCD_clearScreen();
					LCD_displayString("Door is Locking.");
					TIMER_setCallBack(wait_seconds);
					TIMER_Init(&TIMER_Config); // initialize timer1
					seconds=15;
					while(g_tick < seconds){};//wait 15 seconds
					g_tick=0;				}
				else
				{
					count++;
					UART_sendByte(count);
					if(count==3)
					{
						pass_confirmed=1;
						UART_sendByte(1);
						LCD_clearScreen();
						LCD_displayString("error pass xx");
						TIMER_setCallBack(wait_seconds);
						TIMER_Init(&TIMER_Config); // initialize timer1
						seconds=60;
						while(g_tick < seconds){};//wait 60 seconds
						g_tick=0;

					}
				}

			}

		}
		/****************************** Step 4 ******************************/
		else if(operation=='-')
		{
			UART_sendByte(operation);
			while((count!=3)&& (!pass_confirmed))
			{
				LCD_clearScreen();
				LCD_displayString("Plz enter old");
				LCD_moveCursor(1, 0);
				LCD_displayString("pass:");
				LCD_moveCursor(1, 6);
				getPassword(password);
				_delay_ms(50);
				sendPassword(password);
				pass_confirmed=UART_recieveByte();
				if(pass_confirmed){
					count=0;
					pass_confirmed=0;
					break;
				}
				else
				{
					count++;
					UART_sendByte(count);
					if(count==3)
					{
						pass_confirmed=1;
						UART_sendByte(1);
						LCD_clearScreen();
						LCD_displayString("error pass xxx");
						TIMER_setCallBack(wait_seconds);
						TIMER_Init(&TIMER_Config); // initialize timer1
						seconds=60;
						while(g_tick < seconds){};//wait 60 seconds
						g_tick=0;

					}
				}

			}


		}

	}
}
/*
 * description: this function will include store the 5 letters that will be stored in password plus confirm letter
 */
void getPassword (uint8 password[])
{
	uint8 i=0,flag=0;

	for (i=0 ; i<5 ;i++)
	{
		_delay_ms (500);
		password[i] = KEYPAD_getPressedKey();

		LCD_displayCharacter('*');
	}
	_delay_ms(500);
	LCD_clearScreen();
	LCD_displayString("enter = as ok");

	while(!flag)
	{
		flag =KEYPAD_getPressedKey();
		if(flag =='=')
			flag=1;
		else
			flag =0;
		_delay_ms(500);
	}
}
/*
 * description send the password byte after byte to the Control_ECU MC to be checked
 */
void sendPassword (uint8 password[])
{
	uint8 i;

	for (i=0 ; i<5 ; ++i)
	{
		UART_sendByte (password[i]);
	}
	_delay_ms (130);
}
void wait_seconds(void){
	g_tick++;
	if(g_tick==seconds)
	{
		TIMER_DeInit();
	}
}
