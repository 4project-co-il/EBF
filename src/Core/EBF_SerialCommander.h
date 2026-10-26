#ifndef __EBF_SERIAL_COMMANDER_H__
#define __EBF_SERIAL_COMMANDER_H__

#include <Arduino.h>

#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_Serial.h"

class EBF_SerialCommander : protected EBF_Serial::Listener {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_SerialCommander");

	public:
		// Define the mapping structure
		struct CommandsMap {
			const char* commandString;
			EBF_CallbackType callbackFunc;
		};

	public:
		EBF_SerialCommander() {}

		template <size_t mapSize>
		uint8_t Init(EBF_Serial& serial, const CommandsMap (&commandsMap)[mapSize], EBF_CallbackType unknownCommandCallbackFunc = NULL, uint8_t bufferSize = 32)
		{
			return InitEx(serial, commandsMap, mapSize, unknownCommandCallbackFunc, bufferSize);
		}

		uint8_t SetParametersSeparatorCharacter(char paramSeparatorChar = ',');

		// Returns pointer to the buffer, that contains current processed command with NULL termination after the command
		char* GetCommand() { return pBuffer; }

		// Returns next parameter string
		char* GetNextParameterAsString();
		int GetNextParameterAsInt() { return light_atoi(GetNextParameterAsString()); }
		float GetNextParameterAsFloat() { return light_atof(GetNextParameterAsString()); }

	private:
		// Internal Init implementation
		uint8_t InitEx(EBF_Serial& serial, const CommandsMap* pCommandsMap, size_t mapSize, EBF_CallbackType unknownCommandCallbackFunc, uint8_t bufferSize);

		// OnDataAvailable callback from EBF_Serial::Listener
		void onDataAvailable();

		// Process the buffer to find the matching command from the CommandsMap
		uint8_t ProcessBuffer();

		// Light version of atoi and atof string to int/float conversion functions as google suggested
		// The original atoi and atof consume a lot of flash memory
		int light_atoi(const char *str);
		float light_atof(const char *str);

	private:
		EBF_Serial* pSerial;
		const CommandsMap* pCommands = NULL;
		uint8_t numOfCommands = 0;

		EBF_CallbackType unknownCommandCallbackFunc = NULL;

		char* pBuffer = NULL;
		uint8_t bufferSize = 0;
		uint8_t index = 0;
		uint8_t parameterIndex = 0;

		// Character that determines the separator between the parameters of the command
		char parametersSeparatorChar;
};

#endif
