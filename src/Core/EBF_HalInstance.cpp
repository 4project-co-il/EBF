#include "EBF_HalInstance.h"
#include "EBF_Logic.h"
#include "EBF_Core.h"

// Empty callback will be used instead of NULL pointer for callbacks in different classes
// If you don't want a callback function to be called, pass NULL pointer to the initialization
// of EBF class instance, that will prevent some of the processing that is not needed
// EBF products classes use that function to explicitly enable that processing, in order to
// continue the needed flow
void EBF_EmptyCallback() { ; };

uint8_t EBF_HalInstance::numberOfInstances = 0;

EBF_HalInstance::EBF_HalInstance() {
	pollIntervalMs = 0;

	this->numberOfInstances++;
}

uint8_t EBF_HalInstance::Init(HAL_Type type, uint32_t id)
{
	uint8_t rc;

	this->pollIntervalMs = 0;
	this->type = type;
	this->id = id;

	rc = EBF_Logic::GetInstance()->AddHalInstance(*this);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

void EBF_HalInstance::SetPollingInterval(uint32_t ms)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	pollIntervalMs = ms;

	// Recalculation of delays is needed after polling interval change
	pLogic->Recalculate();
}

unsigned long EBF_HalInstance::micros()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->micros();
}

unsigned long EBF_HalInstance::millis()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->millis();
}
