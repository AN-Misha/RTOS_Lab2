#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define STR_SIZE 16+1

typedef enum CommandName
{
	PUTC = 0,
	PUTSTRING,
	CLEAR,
	SCROLLSTRING,
	UNDEFINED
}CommandName;

typedef enum StringDirection
{
	LEFT,
	RIGHT
}StringDirection;

typedef struct Coordinate
{
	uint8_t x;
	uint8_t y;
}Coordinate;

typedef struct StringParameters
{
	char Char;
	char string[STR_SIZE];
	StringDirection stringDirection;
	uint16_t Period;
}StringParameters;

typedef struct SrollStringStruct
{
	uint16_t Period;
	int shift;
	char string[STR_SIZE];
	uint8_t y;
}SrollStringStruct;

typedef struct ScrollStringParameters
{
	SrollStringStruct* paramForTask;
	TaskHandle_t scrollTaskHandle;
}ScrollStringParameters;

typedef struct CommandStruct
{
	CommandName commandName;
	Coordinate coordinate;
	StringParameters stringParameters;
	ScrollStringParameters scrollParameters;
}CommandStruct;



void InitCommandStruct(CommandStruct* commandStruct);

struct CommandStruct getCommandStruct(char* string);
