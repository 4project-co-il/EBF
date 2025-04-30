#ifndef __EBF_MESSAGE_QUEUE_H__
#define __EBF_MESSAGE_QUEUE_H__

#include <Arduino.h>
#include "EBF_Global.h"

class EBF_HalInstance;

class EBF_MessageQueue {
	public:
		typedef struct {
			EBF_HalInstance *pHalInstance;
			uint32_t param1;
		} MessageEntry;

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
