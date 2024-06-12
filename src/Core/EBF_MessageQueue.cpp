#include "EBF_MessageQueue.h"

uint8_t EBF_MessageQueue::Init(uint8_t queueSize)
{
	this->queueSize = queueSize;

	this->mesasgesInQueue = 0;
	this->maxMessagesInQueue = 0;

	// Init queue data
	this->pQueueData = (MessageEntry*)malloc(sizeof(MessageEntry) * this->queueSize);
	memset(this->pQueueData, 0, sizeof(MessageEntry) * this->queueSize);

	this->pInLocation = this->pQueueData;
	this->pOutLocation = this->pQueueData;
	this->pEndLocation = &this->pQueueData[this->queueSize];

	return EBF_OK;
}

uint8_t EBF_MessageQueue::GetMaxMessagesNumber()
{
	return this->maxMessagesInQueue;
}

uint8_t EBF_MessageQueue::GetMessagesNumber()
{
	return this->mesasgesInQueue;
}

// Adds new message to the queue
uint8_t EBF_MessageQueue::AddMessage(MessageEntry &message)
{
    if (mesasgesInQueue == queueSize && pInLocation == pOutLocation) {
		// Buffer is full
		return EBF_NOT_ENOUGH_MEMORY;
	}

	memcpy(pInLocation, &message, sizeof(MessageEntry));
	pInLocation++;

	// Queue wrap around
    if (pInLocation >= pEndLocation) {
        pInLocation = pQueueData;
	}

	mesasgesInQueue++;

	if (mesasgesInQueue > maxMessagesInQueue) {
		maxMessagesInQueue = mesasgesInQueue;
	}

    return EBF_OK;
}

// Processes next message in the queue
uint8_t EBF_MessageQueue::GetMessage(MessageEntry &message)
{
	// Queue is empty
    if (mesasgesInQueue == 0) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	memcpy(&message, pOutLocation, sizeof(MessageEntry));

	pOutLocation++;
	mesasgesInQueue--;

	// Queue wrap around
    if (pOutLocation >= pEndLocation) {
		pOutLocation = pQueueData;
	}

    return EBF_OK;
}

