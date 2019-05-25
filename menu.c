/************************************************************************************
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: menu.с
  Назначение: Формирование меню
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#include "menu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "uart_io.h"
#include "ParseCommand.h"
#include <string.h>
#include <stdbool.h>
extern xQueueHandle stdin_queue;
extern xQueueHandle stdout_queue;

#define PAGE_NUM 10
#define PAGE_SIZE 10

//first index - puge, second - item
CommandStruct pages[PAGE_NUM][PAGE_SIZE];
uint8_t activePage = 0;

#define QUEUE_SIZE 1
xQueueHandle xButtonQueue;

typedef enum PAGE_ERR
{
	PAGE_OK,
	PAGE_IS_FULL
}PAGE_ERR;


// Выбранный пункт меню
uint8_t U_MENU_Item;

// Строка для представления времени
uint8_t U_MENU_Time_String[8];  

// Указатель на сообщение, выводимое в качестве пункта меню
char* U_MENU_Message;

// Подготовить строку для выбранного пункта меню
char* U_MENU_Prepare_Item(uint8_t item); 

void ClearPage(uint8_t page_number)
{
	uint8_t j;
	for(j = 0; j < PAGE_SIZE; j++)
		{
			if(pages[page_number][j].commandName == SCROLLSTRING)
			{
				vPortFree(pages[page_number][j].scrollParameters.paramForTask);
				vTaskDelete(pages[page_number][j].scrollParameters.scrollTaskHandle);
			}
			InitCommandStruct(&pages[page_number][j]);
		}
}

void StopScrolling()
{
	uint8_t j;
	for(j = 0; j < PAGE_SIZE; j++)
		{
			if(pages[activePage][j].commandName == SCROLLSTRING)
			{
				vPortFree(pages[activePage][j].scrollParameters.paramForTask);
				vTaskDelete(pages[activePage][j].scrollParameters.scrollTaskHandle);
			}
		}
}

// Инициализировать меню
void U_MENU_Init (void)
{
	uint8_t i; 
	//Clean pages memory
	for(i = 0; i < PAGE_NUM; i++)
	{
		ClearPage(i);
	}
	
	xButtonQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
  
}

// Задача по работе с меню
void U_MENU_Task_Function (void)
{
	/*Переменная, которая будет хранить значение счётчика квантов*/
	uint8_t data_to_send = 0;
	portTickType xLastTime_left;
	portTickType xLastTime_right;
	uint8_t flag_button_left;
	uint8_t flag_button_right;
	//Инициализация переменной, хранящей значение счётчика квантов
	xLastTime_left=xTaskGetTickCount();
	xLastTime_right=xTaskGetTickCount();
	flag_button_left=OFF;
	flag_button_right=OFF;
	while(1)
	{
		// Пауза 20мс, чтобы по-напрасну не грузить процессор
    vTaskDelay (20);  
    // Проверка на нажатие кнопки
		if ((U_BTN_Read_Button_LEFT()==ON) && (flag_button_left==OFF)&& ((xTaskGetTickCount()-xLastTime_left)>=DrebezgTime))
		{
			// Если нажата кнопка установить флаг нажатой кнопки
			flag_button_left=ON;
			xLastTime_left=xTaskGetTickCount(); // Получение текущего значения счётчика квантов	
		}	
		// Проверка на отпускание кнопки с учетом времени антидребезга
		if ((flag_button_left==ON)&&(U_BTN_Read_Button_LEFT()==OFF)&& ((xTaskGetTickCount()-xLastTime_left)>=DrebezgTime))
		{
			//LEFT BUTTON HERE
			data_to_send = 0;
			xQueueSendToBack(xButtonQueue, &data_to_send, 0); 
			
			flag_button_left=OFF;
			xLastTime_left=xTaskGetTickCount(); // Получение текущего значения счётчика квантов	
		}
		
		
		
		if ((U_BTN_Read_Button_RIGHT()==ON) && (flag_button_right==OFF)&& ((xTaskGetTickCount()-xLastTime_right)>=DrebezgTime))
		{
			// Если нажата кнопка установить флаг нажатой кнопки
			flag_button_right=ON;
			xLastTime_right=xTaskGetTickCount(); // Получение текущего значения счётчика квантов	
		}	
		// Проверка на отпускание кнопки с учетом времени антидребезга
		if ((flag_button_right==ON)&&(U_BTN_Read_Button_RIGHT()==OFF)&& ((xTaskGetTickCount()-xLastTime_right)>=DrebezgTime))
		{
			//RIGHT BUTTON HERE
			data_to_send = 1;
			xQueueSendToBack(xButtonQueue, &data_to_send, 0);
			
			flag_button_right=OFF;
			xLastTime_right=xTaskGetTickCount(); // Получение текущего значения счётчика квантов	
		}

    
		
	}
}

void ReadQueue(void)
{
	uint8_t data_to_receive;
	while(1)
	{
		if(xQueueReceive(xButtonQueue, &data_to_receive, portMAX_DELAY) == pdPASS)
		{
			StopScrolling();
			switch(data_to_receive)
			{
				case 0: //LEFT
					if(activePage > 0)
						activePage--;
					break;
				case 1:	//RIGHT
					if(activePage < PAGE_NUM - 1)
						activePage++;
					break;
			}
		}
	}
}

// Подготовить строку для выбранного пункта меню
char* U_MENU_Prepare_Item (uint8_t item) 
{
	switch (item)
	{
		// Пункт 0;
		case 0:
			
			return "\xCF\xF3\xED\xEA\xF2 0";  // Пункт 0
		
		// Пункт 1;
		case 1:
			
			return "\xCF\xF3\xED\xEA\xF2 1";  // Пункт 1

		// Пункт 2;
		case 2:
			
			return "\xCF\xF3\xED\xEA\xF2 2";  // Пункт 2

		// Пункт 3;
		case 3:
		
			return "\xCF\xF3\xED\xEA\xF2 3";  // Пункт 3
		
		default:
			
			return 0;
	}
}

// Задача по выводу бегущей строки на ЖКИ
void U_MENU_Running_String_Task_Function(void* pvParameters)
{
	volatile SrollStringStruct *srollStringStruct;
		/* Преобразование типа void* к типу TaskParam* */
	srollStringStruct = (SrollStringStruct *)pvParameters;
	uint32_t k = 0;
	char* string;
	string = pvPortMalloc(strlen(srollStringStruct->string)*sizeof(char));
	strncpy(string, srollStringStruct->string, strlen(srollStringStruct->string));
	while(1)
	{
		// Пауза в тиках системного таймера. Здесь 1 тик = 1 мс
    vTaskDelay(srollStringStruct->Period);
		
		// Вывести строку c прокруткой
		// s - выводимая строка
		// 6 - номер строки на дисплее
		// k++ - смещение строки на количество символов
		U_MLT_Scroll_String (string, srollStringStruct->y, k += srollStringStruct->shift);
	}
}

PAGE_ERR SaveToPage(CommandStruct* commandStruct)
{
	PAGE_ERR ret = PAGE_IS_FULL;
	bool pageIsSet = false;
	int i = 0;
	while(pageIsSet == false && i < PAGE_SIZE)
	{
		if(pages[activePage][i].commandName == UNDEFINED)
		{
			memcpy(&pages[activePage][i], commandStruct, sizeof(CommandStruct));
			pageIsSet = true;
			ret = PAGE_OK;
		}
		i++;
	}
	
	return ret;
}

void ClearLCD()
{
	// Очистить дисплей 1 
	U_MLT_Clear_Chip (1);
	// Очистить дисплей 2
	U_MLT_Clear_Chip (2);	
}

void PUTCtoLCD(CommandStruct* commandStruct)																														//DO HERE
{
	U_MLT_Put_Char(commandStruct->stringParameters.Char, commandStruct->coordinate.x, commandStruct->coordinate.y);
}

void PUTSTRINGtoLCD(CommandStruct* commandStruct)
{
	U_MLT_Put_String(commandStruct->stringParameters.string, commandStruct->coordinate.y);
}

void SCROLLSTRINGtoLCD(CommandStruct* commandStruct)
{
	SrollStringStruct params;
	/*
	commandStruct->scrollParameters.paramForTask.Period = commandStruct->stringParameters.Period;
	//commandStruct->scrollParameters.paramForTask.string = commandStruct->stringParameters.string;
	strcpy(commandStruct->scrollParameters.paramForTask.string, commandStruct->stringParameters.string);
	
	commandStruct->scrollParameters.paramForTask.y = commandStruct->coordinate.y;
	switch(commandStruct->stringParameters.stringDirection)
	{
		case LEFT:
			commandStruct->scrollParameters.paramForTask.shift = -1;
			break;
		case RIGHT:
			commandStruct->scrollParameters.paramForTask.shift = 1;
			break;
	}
	*/
	
	params.Period = commandStruct->stringParameters.Period;
	params.y = commandStruct->coordinate.y;
	strcpy(params.string, commandStruct->stringParameters.string);
	SrollStringStruct* paramPointer = pvPortMalloc(sizeof(SrollStringStruct));
	memcpy(paramPointer, &params, sizeof(params));
	commandStruct->scrollParameters.paramForTask = paramPointer;
	
	switch(commandStruct->stringParameters.stringDirection)
	{
		case LEFT:
			params.shift = -1;
			break;
		case RIGHT:
			params.shift = 1;
			break;
	}
	
	
	xTaskCreate(U_MENU_Running_String_Task_Function, "ScrollingStr", configMINIMAL_STACK_SIZE, paramPointer, tskIDLE_PRIORITY + 2, commandStruct->scrollParameters.scrollTaskHandle);
}

void PageToLCD()
{
	ClearLCD();
	int i;
	for(i = 0; i < PAGE_SIZE; i++)
	{
		if(pages[activePage][i].commandName != UNDEFINED)
		{
			switch(pages[activePage][i].commandName)
			{
				case PUTC:
					PUTCtoLCD(&pages[activePage][i]);
					break;
				case PUTSTRING:
					PUTSTRINGtoLCD(&pages[activePage][i]);
					break;
				case SCROLLSTRING:
					SCROLLSTRINGtoLCD(&pages[activePage][i]);
					break;
			}
			//print item to the mlt																																													
		}
	}
}

//Задача по выводу на ЖКИ информации полученной с терминала по UART
void U_MENU_Output (void)
{ 
	 /* Буфер для полученного сообщения по UART */
  char Message_UART[60 + 1]; 
	while(1)
	{	
	 /* Ожидание ввода сообщения по UART */
		scanf("%60s", Message_UART);
		CommandStruct commandStruct = getCommandStruct(Message_UART);
		SaveToPage(&commandStruct);
		PageToLCD();
		/*
		printf("CMD: %d, X: %d, Y: %d, Chr: %c, Period: %d, Str: %s, Dir: %d\n", commandStruct.commandName, commandStruct.coordinate.x, commandStruct.coordinate.y, 
		commandStruct.stringParameters.Char, commandStruct.stringParameters.Period, 
		commandStruct.stringParameters.string, commandStruct.stringParameters.stringDirection);
		*/
		
		
		// Передача полученного ссобщения на дисплей ЖКИ в 4-ю строку
	 //U_MLT_Put_String (Message_UART, 3); 
	 // Передача в стандартный поток вывода, что сообщение успешно передано на ЖКИ
	 //printf("Transmit Message To LCD - Success\r\n");
	}
}
// Задача - приветствие
void U_Privet(void)
{
	// Отправка сообщения приветствия
	printf("Work with Queues\r\n");
	// Удаление задачи
	vTaskDelete( NULL );
}
// Задача для работы с выводом по UART
void Task_output(void)
{
	  static uint8_t buffer;
  while(1)
	{
		if (xQueueReceive(stdout_queue, &buffer, portMAX_DELAY) == pdPASS)
    {
				  /* Ожидание флага освобождения буфера передачи (TXFE) */
					while (UART_GetFlagStatus(UART_IO, UART_FLAG_TXFE) != SET);
					// Отправка одного символа по UART
					UART_SendData(UART_IO, buffer);
		}
				
  }
}

