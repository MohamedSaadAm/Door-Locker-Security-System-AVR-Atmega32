/*
 ================================================================================================
 Name        : MC2.c
 Author      : Mohamed Saad
 Description : MC2 code in Door Locker Security System project
 ================================================================================================
 */

#include "uart.h"
#include "twi.h"
#include "timer.h"
#include "buzzer.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include <util/delay.h> /* For the delay functions */
#include <avr/io.h> /* To use the SREG register */

/**************************** Global Variables ****************************/
uint8 g_tick=0, seconds;


/************************** Functions Prototypes **************************/
uint8 verifyPassword (uint8 password[] , uint8 confirmPassword[]);
void storePasswordeeprom (uint8 password[]);
void receivePassword (uint8 password[]);
void readPasswordeeprom (uint8 password []);
void wait_seconds(void);


/******************************** Main ********************************/
int main()
{
	uint8 password[5] , confirmPassword[5] , pass_confirmed=0 , operation,saved_pass[5],count=0,buzzer=0;


	/* Create configuration structure for TIMER driver with
	 * initial value = 0 & compare value = 7813
	 * which generates an interrupt every one second
	 */
	TIMER_ConfigType TIMER_Config = {TIMER1,TIMER_COMPARE_MODE,F_CPU_1024,0,7813};
	/*
	 * define Uart struct
	 * 8 bits of data,Disable parity ,One stop bit ,Baud rate=9600
	 */
	UART_ConfigType Config={UART_8_BIT_PER_FRAME,UART_DISABLE_PARITY,UART_1_STOP_BIT,9600};

	TWI_ConfigType twi_config={40000,0x01};

	SREG|=(1<<7);  //Enable I-bit

	BUZZER_Init();  //Initialize buzzer

	DcMotor_Init();  //Initialize DC Motor

	UART_init(&Config);  //Initialize UART

	TWI_init(&twi_config);  //Initialize I2C

	while(1){

		/****************************** Step 1 ******************************/
		while(pass_confirmed == 0)
		{
			receivePassword(password);

			receivePassword(confirmPassword);

			pass_confirmed = verifyPassword(password, confirmPassword);
			UART_sendByte(pass_confirmed);

			_delay_ms(130);
		}


		storePasswordeeprom(password);
		/****************************** Step 2 ******************************/
		pass_confirmed=0;
		operation= UART_recieveByte();
		/****************************** Step 3 ******************************/
		if(operation=='+')
		{
			while((!pass_confirmed)){
				receivePassword(password);
				readPasswordeeprom(saved_pass);
				pass_confirmed = verifyPassword(password, confirmPassword);
				UART_sendByte(pass_confirmed);
				if(pass_confirmed)
				{

					count=0;
					DcMotor_Rotate(CLOCKWISE);
					TIMER_setCallBack(wait_seconds);
					TIMER_Init(&TIMER_Config); // initialize timer1
					seconds=15;
					while(g_tick < seconds){};//wait 15 seconds
					g_tick=0;

					DcMotor_Rotate(STOP);
					TIMER_setCallBack(wait_seconds);
					TIMER_Init(&TIMER_Config); // initialize timer1
					seconds=3;
					while(g_tick < seconds){};//wait 3 seconds
					g_tick=0;


					DcMotor_Rotate(ANTI_CLOCKWISE);
					TIMER_setCallBack(wait_seconds);
					TIMER_Init(&TIMER_Config); // initialize timer1
					seconds=15;
					while(g_tick < seconds){};//wait 15 seconds
					g_tick=0;

					DcMotor_Rotate(STOP);
				}
				else
				{
					count=UART_recieveByte();
					if(count==3)
					{
						pass_confirmed=1;
						buzzer=UART_recieveByte();
						if(buzzer)
						{
							BUZZER_ON();
							TIMER_setCallBack(wait_seconds);
							TIMER_Init(&TIMER_Config); // initialize timer1
							seconds=60;
							while(g_tick < seconds){};//wait 60 seconds
							g_tick=0;
							BUZZER_OFF();
						}
					}
				}
			}
		}
		/****************************** Step 4 ******************************/
		else if(operation=='-')
		{
			while( (!pass_confirmed)){
				receivePassword(password);
				readPasswordeeprom(saved_pass);
				pass_confirmed = verifyPassword(password, confirmPassword);
				UART_sendByte(pass_confirmed);
				if(pass_confirmed)
				{
					count=0;
					pass_confirmed=0;
					break;

				}
				else
				{
					count=UART_recieveByte();
					if(count==3)
					{
						pass_confirmed=1;
						buzzer=UART_recieveByte();
						if(buzzer)
						{
							BUZZER_ON();
							TIMER_setCallBack(wait_seconds);
							TIMER_Init(&TIMER_Config); // initialize timer1
							seconds=60;
							while(g_tick < seconds){};//wait 60 seconds
							g_tick=0;
							BUZZER_OFF();
						}
					}
				}

			}

		}


	}
}

uint8 verifyPassword (uint8 password[] , uint8 confirmPassword[])
{
	uint8 i;

	for (i=0 ; i<5 ; ++i)
	{
		if (password[i] != confirmPassword[i])
		{
			return 0;
		}
	}
	return 1;
}
void storePasswordeeprom (uint8 password[])
{
	uint8 i;
	for (i=0 ; i<5 ; ++i)
	{
		EEPROM_writeByte ((0X0090 + i) , (password[i]));
		_delay_ms(130);
	}
}
void receivePassword (uint8 password[])
{
	uint8 i;

	for (i=0 ; i<5 ; ++i)
	{
		password [i] = UART_recieveByte();
	}
	_delay_ms (130);
}
void readPasswordeeprom (uint8 password [])
{
	uint8 i;
	for (i=0 ; i<5 ; ++i)
	{
		EEPROM_readByte ((0x0090 + i) , (password+i));
		_delay_ms(100);
	}
}
void wait_seconds(void){
	g_tick++;
	if(g_tick==seconds)
	{
		TIMER_DeInit();
	}
}
