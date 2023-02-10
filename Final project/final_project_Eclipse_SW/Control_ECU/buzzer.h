 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the BUZZER driver
 *
 * Author: Mohamed Saad
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID         PORTB_ID
#define BUZZER_PIN_ID          PIN0_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * The Function responsible for setup the direction for the buzzer
 * pin through the GPIO driver.
 */
void BUZZER_Init(void);

/*
 * Description :
 * The Function responsible for turning the buzzer on.
 */
void BUZZER_ON(void);

/*
 * Description :
 * The Function responsible for turning the buzzer off.
 */
void BUZZER_OFF(void);

#endif /* BUZZER_H_ */
