#include "ParseCommand.h"
#include <stdbool.h>
#include "slre.h"
#include <string.h>
#include <stdlib.h>

//const char* patterns[4];

const char *patternPUTC = "PUTC=\"(\\s|\\S)\",(\\d+),(\\d+)";
const char *patternPUTSTRING = "PUTSTRING=\"(\\s+|\\S+)\",(\\d)";
const char *patternCLEAR = "(CLEAR)";
const char *patternSCROLLSTRING = "SCROLLSTRING=\"(\\s+|\\S+)\",(\\d+),(\\d+),(LEFT|RIGHT)";




void InitCommandStruct(CommandStruct* commandStruct)
{
	commandStruct->commandName = UNDEFINED;
	commandStruct->coordinate.x = 0xFF;
	commandStruct->coordinate.y = 0xFF;
	commandStruct->stringParameters.Char = 0xFF;
	commandStruct->stringParameters.Period = 0xFF;
	//commandStruct->stringParameters.string
	memset(&commandStruct->stringParameters.string, '\0', STR_SIZE); 
	commandStruct->stringParameters.stringDirection = RIGHT;
}
//Return Name, Char, X, Y
bool parsePUTC(char* string, CommandStruct* commandStruct)
{
	bool result = false;
	struct slre_cap caps[3];
  int matchRes = slre_match(patternPUTC, string, strlen(string), caps, 3, 0);
	if(matchRes >= 0)
	{
		result = true;
		memcpy(&commandStruct->stringParameters.Char, caps[0].ptr, sizeof(char));
		commandStruct->coordinate.x = atoi(caps[1].ptr);
		commandStruct->coordinate.y = atoi(caps[2].ptr);
		commandStruct->commandName = PUTC;
		
	}
	return result;
}
//Return Name, String, Y
bool parsePUTSTRING(char* string, CommandStruct* commandStruct)
{
	bool result = false;
	struct slre_cap caps[2];
  int matchRes = slre_match(patternPUTSTRING, string, strlen(string), caps, 2, 0);
	if(matchRes >= 0)
	{
		result = true;
		
		strncpy(&commandStruct->stringParameters.string[0], caps[0].ptr, caps[0].len);
		commandStruct->stringParameters.string[caps[0].len] = '\0';
		commandStruct->coordinate.y = atoi(caps[1].ptr);
		commandStruct->commandName = PUTSTRING;
	}
	return result;
}

//Return Name
bool parseCLEAR(char* string, CommandStruct* commandStruct)
{
	bool result = false;
	struct slre_cap caps[1];
  int matchRes = slre_match(patternCLEAR, string, strlen(string), caps, 1, 0);
	if(matchRes >= 0)
	{
		result = true;
		commandStruct->commandName = CLEAR;
	}
	return result;
}

//Return Name, String, y, Period, Direction 
bool parseSCROLLSTRING(char* string, CommandStruct* commandStruct)
{
	bool result = false;
	struct slre_cap caps[4];
  int matchRes = slre_match(patternSCROLLSTRING, string, strlen(string), caps, 4, 0);
	if(matchRes >= 0)
	{
		result = true;
		
		strncpy(&commandStruct->stringParameters.string[0], caps[0].ptr, caps[0].len);
		commandStruct->stringParameters.string[caps[0].len] = '\0';
		commandStruct->coordinate.y = atoi(caps[1].ptr);
		commandStruct->commandName = SCROLLSTRING;
		commandStruct->stringParameters.Period = atoi(caps[2].ptr);
		if(!strncmp(caps[3].ptr,"LEFT",caps[2].len))
			commandStruct->stringParameters.stringDirection = LEFT;
		else
			commandStruct->stringParameters.stringDirection = RIGHT;
	}
	return result;
}

struct CommandStruct getCommandStruct(char* string)
{
	struct CommandStruct commandStruct;
	InitCommandStruct(&commandStruct);
	bool CommandWasFound = false;
	CommandWasFound = parsePUTC(string, &commandStruct);
	if(CommandWasFound == false)
		CommandWasFound = parsePUTSTRING(string, &commandStruct);
	if(CommandWasFound == false)
		CommandWasFound = parseCLEAR(string, &commandStruct);
	if(CommandWasFound == false)
		CommandWasFound = parseSCROLLSTRING(string, &commandStruct);
	
	return commandStruct;
}
