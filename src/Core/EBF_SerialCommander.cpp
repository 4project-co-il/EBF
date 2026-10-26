#include "EBF_SerialCommander.h"
#include "EBF_Core.h"

uint8_t EBF_SerialCommander::InitEx(EBF_Serial& serial, const CommandsMap* pCommandsMap, size_t mapSize, EBF_CallbackType unknownCommandCallbackFunc, uint8_t bufferSize)
{
	uint8_t rc = EBF_OK;

	// We support up to 255 commands in that class
	if (mapSize > 255) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}

	this->pSerial = &serial;
	this->pCommands = pCommandsMap;
	this->numOfCommands = mapSize;

	this->unknownCommandCallbackFunc = unknownCommandCallbackFunc;

	// Default parameters delimiter will be comma ','
	parametersSeparatorChar = ',';

	this->bufferSize = bufferSize;
	pBuffer = (char*)malloc(bufferSize);
	if (pBuffer == NULL) {
		EBF_REPORT_AND_RETURN(EBF_NOT_ENOUGH_MEMORY);
	}

	memset(pBuffer, 0, bufferSize);
	index = 0;

	// Register this commander instance as the callback for serial input
	pSerial->SetCallback(this);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_SerialCommander::SetParametersSeparatorCharacter(char paramSeparatorChar)
{
	this->parametersSeparatorChar = paramSeparatorChar;

	return EBF_OK;
}

void EBF_SerialCommander::onDataAvailable()
{
	uint8_t rc;
	char ch;
	char nlCh;

	// Loop while there is data to read
	while (pSerial->available()) {
		// Read the incoming string char by char
		ch = pSerial->read();

		// One of the line end characters
		if (ch == 13 || ch == 10) {
			// Special case - we might get only "\n" or only "\r",
			// but we might also get "\r\n" or "\n\r".
			if (pSerial->available()) {
				// Checking if there is more characters to read and peek if next character is the pair of currently received
				nlCh = pSerial->peek();

				if ((ch == 13 && nlCh == 10) || (ch == 10 && nlCh == 13)) {
					// We have the "\r\n" or "\n\r" case
					// Read the next characters to discard it
					pSerial->read();
				}
			}

			// Add NULL termination to the string
			pBuffer[index++] = 0;

			// Process the incoming buffer
			rc = ProcessBuffer();
			if (rc != EBF_OK) {
				EBF_REPORT_ERROR(rc);
				return;
			}

			// Restart the index, preparing for the next command
			index = 0;
			pBuffer[0] = 0;
		} else {
			// We might get a string longer than the allocated buffer, so need to check that we're in the buffer boundaries
			if (index < bufferSize-1) {
				pBuffer[index++] = ch;
				pBuffer[index] = 0;
			}
		}
	}
}

uint8_t EBF_SerialCommander::ProcessBuffer()
{
	uint8_t i;

	// Look on all the commands
	for (i=0; i<numOfCommands; i++) {
		// Find the exact match with command length, since there might be parameters after it
		if (strncmp(pCommands[i].commandString, pBuffer, strlen(pCommands[i].commandString)) == 0) {
			// Found the command, move the parameters index to point after the command in the buffer
			parameterIndex = strlen(pCommands[i].commandString);

			// Skip the first separator if it's there
			if (pBuffer[parameterIndex] == parametersSeparatorChar) {
				pBuffer[parameterIndex] = 0;
				parameterIndex++;
			}

			// Call the callback function for that command
			if (pCommands[i].callbackFunc != NULL) {
				(pCommands[i].callbackFunc)();
			}

			break;
		}
	}

	// We've reached end of the list, no commands were found, and unknown command callback was set
	if (i == numOfCommands && unknownCommandCallbackFunc != NULL) {
		(unknownCommandCallbackFunc)();
	}

	return EBF_OK;
}

char* EBF_SerialCommander::GetNextParameterAsString()
{
	uint8_t paramStartIndex = parameterIndex;

	// There are no more parameters
	if (pBuffer[parameterIndex] == 0) {
		return NULL;
	}

	// Look for next separator or 0
	while (parameterIndex < bufferSize) {
		// That's the end of the string
		if (pBuffer[parameterIndex] == 0) {
			break;
		}

		// Found next separator
		if (pBuffer[parameterIndex] == parametersSeparatorChar) {
			// Replace it with NULL, so we could process next parameter after it
			pBuffer[parameterIndex] = 0;
			parameterIndex++;

			break;
		}

		// Advance to next character
		parameterIndex++;
	}

	return &pBuffer[paramStartIndex];
}

int EBF_SerialCommander::light_atoi(const char *str) {
    int res = 0;
    int sign = 1;

	// Skip leading spaces
	while (*str == ' ') {
		str++;
	}

    // Handle negative numbers
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Process each character
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }

    return sign * res;
}

float EBF_SerialCommander::light_atof(const char *str) {
    float res = 0.0;
    float sign = 1.0;
    float divisor = 1.0;

	// Skip leading spaces
	while (*str == ' ') {
		str++;
	}

    // Handle sign
    if (*str == '-') {
        sign = -1.0;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Integer part
    while (*str >= '0' && *str <= '9') {
        res = res * 10.0 + (*str - '0');
        str++;
    }

    // Fractional part
    if (*str == '.') {
        str++;
        while (*str >= '0' && *str <= '9') {
            res = res * 10.0 + (*str - '0');
            divisor *= 10.0;
            str++;
        }
    }

    return (sign * res) / divisor;
}
