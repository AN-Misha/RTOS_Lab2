/************************************************************************************
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: button.h 
  Назначение: Управление кнопкой   
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#ifndef __U_BUTTON
 #define __U_BUTTON

#include "common.h"

// Порт кнопки
#define U_BTN_PORT_LEFT MDR_PORTE
#define U_BTN_PORT_RIGHT MDR_PORTB
// Линия для кнопки
#define U_BTN_PIN_LEFT PORT_Pin_3
#define U_BTN_PIN_RIGHT PORT_Pin_6

// Инициализировать кнопку
void U_BTN_Init (void);

// Получить состояние кнопки
uint8_t U_BTN_Read_Button_LEFT (void);
uint8_t U_BTN_Read_Button_RIGHT (void);

#endif 
