#ifndef __EBF_MESSAGE_QUEUE_H__
#define __EBF_MESSAGE_QUEUE_H__

#include <Arduino.h>

#ifndef EBF_MESSAGE_PARAMS
#define EBF_MESSAGE_PARAMS 1
#endif

class EBF_MessageQueue {
	public:
#if EBF_MESSAGE_PARAMS == 1
		typedef struct {
			uint32_t param1;
		} MessageEntry;
#elif EBF_MESSAGE_PARAMS == 2
		typedef struct {
			uint32_t param1;
			uint32_t param2;
		} MessageEntry;
#elif EBF_MESSAGE_PARAMS == 3
		typedef struct {
			uint32_t param1;
			uint32_t param2;
			uint32_t param3;
		} MessageEntry;
#elif EBF_MESSAGE_PARAMS == 4
		typedef struct {
			uint32_t param1;
			uint32_t param2;
			uint32_t param3;
			uint32_t param4;
		} MessageEntry;
#else
	#error Unhandled number of parameters for EBF_MESSAGE_PARAMS
#endif

	public:
		uint8_t Init(uint8_t queueSize);

		uint8_t GetMaxMessagesNumber();
		uint8_t GetMessagesNumber();

		uint8_t AddMessage(MessageEntry &message);
		uint8_t GetMessage(MessageEntry &message);

	private:
		uint8_t queueSize;
		uint8_t volatile mesasgesInQueue;
		uint8_t maxMessagesInQueue;

		MessageEntry* pQueueData;
		MessageEntry* pInLocation;
		MessageEntry* pOutLocation;
		MessageEntry* pEndLocation;
};

#endif
