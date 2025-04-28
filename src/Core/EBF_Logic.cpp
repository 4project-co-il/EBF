#include "EBF_Logic.h"
#include "EBF_HalInstance.h"

// EBF_Logic implementation
EBF_Logic *EBF_Logic::pStaticInstance = new EBF_Logic();

#ifdef EBF_USE_INTERRUPTS

#if defined(ARDUINO_ARCH_AVR)
#define IMPLEMENT_EBF_ISR(interrupt) \
	void EBF_ISR_Handler_ ## interrupt() { \
		EBF_Logic::GetInstance()->HandleIsr(interrupt); \
	}

#if EXTERNAL_NUM_INTERRUPTS > 8
    #error Up to 8 external interrupts are currently supported for AVR platform
#endif
#if EXTERNAL_NUM_INTERRUPTS > 7
    IMPLEMENT_EBF_ISR(7)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 6
    IMPLEMENT_EBF_ISR(6)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 5
    IMPLEMENT_EBF_ISR(5)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 4
    IMPLEMENT_EBF_ISR(4)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 3
    IMPLEMENT_EBF_ISR(3)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 2
    IMPLEMENT_EBF_ISR(2)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 1
    IMPLEMENT_EBF_ISR(1)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 0
    IMPLEMENT_EBF_ISR(0)
#endif
#elif defined(ARDUINO_ARCH_SAMD)
	// TODO: Possible optimization
	// Since we use Arduino's attachInterrupt functionality, we will have only one ISR function registered to it
	// so there is a need to loop again on the INTFLAG register to find what interrupt fired
	void EBF_ISR_Handler() {
		// Optimization: exit right away if the INTFLAG is empty
		// Might happen when more than one interrupt is waiting.
		// Arduino code will call the handler for every interrupt, while we handle all the interrupts in a single run
		if (EIC->INTFLAG.reg == 0) {
			return;
		}

		EBF_Logic *pLogic = EBF_Logic::GetInstance();

#ifdef EBF_SLEEP_IMPLEMENTATION
		pLogic->ExitSleep();
#endif

		for (uint8_t i=0; i<EXTERNAL_NUM_INTERRUPTS; i++) {
			if ((EIC->INTFLAG.reg & 1<<i) != 0) {
				pLogic->HandleIsr(i);

				// Clear the interrupt flag, so we will not handle the same interrupt in the next call of the
				// handler from the Arduino's processing functions
				EIC->INTFLAG.reg = 1<<i;
			}
		}
	}
#else
	#error Current board type is not supported
#endif

#endif

EBF_Logic::EBF_Logic()
{
	pHalInstances = NULL;
	halIndex = 0;
#ifdef EBF_USE_INTERRUPTS
	isRunFromISR = 0;
	isPostInterruptProcessing = 0;
#endif

#ifdef EBF_SLEEP_IMPLEMENTATION
	this->SleepConstructor();
#endif
}

EBF_Logic *EBF_Logic::GetInstance()
{
	return pStaticInstance;
}

uint8_t EBF_Logic::Init(uint8_t maxTimers, uint8_t queueSize)
{
	uint8_t rc;

#ifdef EBF_USE_INTERRUPTS
	for (uint8_t i=0; i<EXTERNAL_NUM_INTERRUPTS; i++) {
		pHalIsr[i] = NULL;
	}

	rc = this->msgQueue.Init(queueSize);
	if (rc != EBF_OK) {
		return rc;
	}

	rc = this->timers.Init(maxTimers, &msgQueue);
#else
	rc = this->timers.Init(maxTimers, NULL);
#endif
	if (rc != EBF_OK) {
		return rc;
	}

#ifdef EBF_SLEEP_IMPLEMENTATION
		InitSleep();
#endif

	if (EBF_HalInstance::GetNumberOfInstances() > 0) {
		pHalInstances = (EBF_HalInstance**)malloc(sizeof(EBF_HalInstance*) * EBF_HalInstance::GetNumberOfInstances());

		if (pHalInstances == NULL) {
			return EBF_NOT_ENOUGH_MEMORY;
		}
	}

	return EBF_OK;
}

uint8_t EBF_Logic::AddHalInstance(EBF_HalInstance &instance)
{
	if (halIndex >= EBF_HalInstance::GetNumberOfInstances()) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	pHalInstances[halIndex] = &instance;
	halIndex++;

	return EBF_OK;
}

uint8_t EBF_Logic::Process()
{
	uint8_t i;
	uint32_t delayWanted = EBF_NO_POLLING;
	EBF_HalInstance *pHal;
	unsigned long ms;

	// Start counting time before the execution of the callbacks, that might take some time
	uint32_t start = this->micros();

	// Process timers
	delayWanted = timers.Process(start);

//	SerialUSB.print("Timers wanted: ");
//	SerialUSB.println(delayWanted);

	// Process HALs
	for (i=0; i<halIndex; i++) {
		pHal = pHalInstances[i];

//		SerialUSB.print("HAL ");
//		SerialUSB.print(pHal->GetType());
//		SerialUSB.print(":");
//		SerialUSB.print(pHal->GetId());
//		SerialUSB.print(" want ");
//		SerialUSB.println(pHal->GetPollingInterval());

		if (pHal->GetPollingInterval() == EBF_NO_POLLING) {
			continue;
		}

		ms = this->millis();

		if (ms - pHal->GetLastPollMillis() > pHal->GetPollingInterval()) {
			pHal->SetLastPollMillis(ms);
			pHal->Process();

			if (pHal->GetPollingInterval() < delayWanted) {
				delayWanted = pHal->GetPollingInterval();
			}
		} else {
			uint16_t pollWanted = pHal->GetPollingInterval() - (ms - pHal->GetLastPollMillis());

			if (pollWanted < delayWanted) {
				delayWanted = pollWanted;
			}
		}
	}

	// Should give other things some CPU time
	if (delayWanted == 0) {
		delayWanted = 1;
	}

//	SerialUSB.print("Wanted delay: ");
//	SerialUSB.println(delayWanted);

#ifdef EBF_SLEEP_IMPLEMENTATION
	// Try to power down the CPU for some time...
	if (delayWanted > 1 && sleepMode != EBF_SleepMode::EBF_NO_SLEEP && msgQueue.GetMessagesNumber() == 0) {
		// Enter sleep mode for delayWanted timer
		EnterSleep(delayWanted);

#ifdef EBF_USE_INTERRUPTS
		uint8_t rc;

		// Messages are used to pass information from interrupts to normal run
		while (msgQueue.GetMessagesNumber() > 0) {
			rc = msgQueue.GetMessage(lastMessage);

			if (rc == EBF_OK) {
				isPostInterruptProcessing = 1;
				lastMessage.pHalInstance->Process();
				isPostInterruptProcessing = 0;
			}
		}

		return EBF_OK;
#endif
	}
#endif

	// Implementing our own delay loop, so we could check the messages from interrupts in the loop
	while (delayWanted > 0) {
		yield();

#ifdef EBF_USE_INTERRUPTS
		uint8_t rc;

		// Messages are used to pass information from interrupts to normal run
		while (msgQueue.GetMessagesNumber() > 0) {
			rc = msgQueue.GetMessage(lastMessage);

			if (rc == EBF_OK) {
				isPostInterruptProcessing = 1;
				lastMessage.pHalInstance->Process();
				isPostInterruptProcessing = 0;
			}
		}
#endif
		while ( delayWanted > 0 && (this->micros() - start) >= 1000) {
			delayWanted--;
			start += 1000;
		}
	}

	return EBF_OK;
}

EBF_HalInstance *EBF_Logic::GetHalInstance(EBF_HalInstance::HAL_Type type, uint8_t id)
{
	uint8_t i;

	for (i=0; i<halIndex; i++) {
		if (pHalInstances[i]->GetType() == type && pHalInstances[i]->GetId() == id) {
			return pHalInstances[i];
		}
	}

	return NULL;
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_Logic::AttachInterrupt(uint8_t interruptNumber, EBF_HalInstance *pHalInstance, uint8_t mode)
{
	// Use interrupt number as the hint
	return AttachInterrupt(interruptNumber, pHalInstance, mode, interruptNumber);
}

uint8_t EBF_Logic::AttachInterrupt(uint8_t interruptNumber, EBF_HalInstance *pHalInstance, uint8_t mode, uint32_t hint)
{
#if defined(ARDUINO_ARCH_SAMD)
	uint8_t pinNumber = digitalPinToInterrupt(interruptNumber);

// For SAMD there is a converstion table
#if ARDUINO_SAMD_VARIANT_COMPLIANCE >= 10606
	interruptNumber = g_APinDescription[interruptNumber].ulExtInt;
#endif
#endif

	if (interruptNumber > EXTERNAL_NUM_INTERRUPTS) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	pHalIsr[interruptNumber] = pHalInstance;
	isrHint[interruptNumber] = hint;

#if defined(ARDUINO_ARCH_AVR)
	switch (interruptNumber)
	{
#if EXTERNAL_NUM_INTERRUPTS > 8
    #error Up to 8 external interrupts are currently supported for AVR platform
#endif
#if EXTERNAL_NUM_INTERRUPTS > 7
	case 7:
		attachInterrupt(7, EBF_ISR_Handler_7, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 6
	case 6:
		attachInterrupt(6, EBF_ISR_Handler_6, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 5
	case 5:
		attachInterrupt(5, EBF_ISR_Handler_5, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 4
	case 4:
		attachInterrupt(4, EBF_ISR_Handler_4, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 3
	case 3:
		attachInterrupt(3, EBF_ISR_Handler_3, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 2
	case 2:
		attachInterrupt(2, EBF_ISR_Handler_2, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 1
	case 1:
		attachInterrupt(1, EBF_ISR_Handler_1, mode);
		break;
#endif
#if EXTERNAL_NUM_INTERRUPTS > 0
	case 0:
		attachInterrupt(0, EBF_ISR_Handler_0, mode);
		break;
#endif

	default:
		break;
	}
#elif defined(ARDUINO_ARCH_SAMD)
	// TODO: Possible optimization
	// We will use Arduino's attachInterrupt for now
	// PROs: All the initialization is done there (EIC clock, WAKEUP flag, mode)
	// CONs: There will be one more function call on the way and the loop in the ISR to determine what interrupt is processed now
	attachInterrupt(pinNumber, EBF_ISR_Handler, (PinStatus)mode);
#else
	#error Current board type is not supported
#endif

	return EBF_OK;
}

void EBF_Logic::HandleIsr(uint8_t interruptNumber)
{
	if (pHalIsr[interruptNumber] != NULL) {
		// Next process call is done from ISR
		isRunFromISR = 1;
		interruptHint = isrHint[interruptNumber];

		pHalIsr[interruptNumber]->ProcessInterrupt();

		isRunFromISR = 0;
	}
}

uint8_t EBF_Logic::ProcessInterrupt(EBF_HalInstance *pHalInstance)
{
	EBF_MessageQueue::MessageEntry msg;

	memset(&msg, 0, sizeof(EBF_MessageQueue::MessageEntry));
	msg.pHalInstance = pHalInstance;
	msg.param1 = 0;

	return msgQueue.AddMessage(msg);
}

uint8_t EBF_Logic::ProcessInterrupt(EBF_HalInstance *pHalInstance, uint32_t param1)
{
	EBF_MessageQueue::MessageEntry msg;

	memset(&msg, 0, sizeof(EBF_MessageQueue::MessageEntry));
	msg.pHalInstance = pHalInstance;
	msg.param1 = param1;

	return msgQueue.AddMessage(msg);
}
#endif

#ifdef EBF_SLEEP_IMPLEMENTATION

#if defined(ARDUINO_ARCH_SAMD)
void EBF_Logic::SleepConstructor()
{
	sleepMode = EBF_SleepMode::EBF_NO_SLEEP;
	microsAddition = 0;

	// Disable all pins (input, no pullup, no input buffer)
	for (uint32_t ulPin = 0 ; ulPin < NUM_DIGITAL_PINS ; ulPin++) {
		PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg = (uint8_t)(PORT_PINCFG_RESETVALUE);
		PORT->Group[g_APinDescription[ulPin].ulPort].DIRCLR.reg = (uint32_t)(1<<g_APinDescription[ulPin].ulPin);
	}

	// reset_gclks, leave only clock ID(0), the 48MHz
	for (uint8_t i = 1; i < GCLK_GEN_NUM; i++) {
		GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(i);
		while (GCLK->STATUS.bit.SYNCBUSY);
	}

	// connect all peripherial to a dead clock
	for (byte i = 1; i < 37; i++) {
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(i) | GCLK_CLKCTRL_GEN(4) | GCLK_CLKCTRL_CLKEN;
		while (GCLK->STATUS.bit.SYNCBUSY);
	}
}

void RTC_Handler(void)
{
	// Disable RTC
	RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// And set the counter to 0, in case it moved on
	RTC->MODE0.COUNT.reg = 0;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// must clear flag at end
	RTC->MODE0.INTFLAG.reg |= (RTC_MODE0_INTFLAG_CMP0 | RTC_MODE0_INTFLAG_OVF);
}

uint8_t EBF_Logic::InitSleep()
{
	// Based on ArduinoLowPower library, configGCLK6() function
	// https://github.com/arduino-libraries/ArduinoLowPower/blob/master/src/samd/ArduinoLowPower.cpp
	// And on ArduinoLowPower library, "Added support for Mode 0 and Mode 1" PR
	// https://github.com/arduino-libraries/RTCZero/pull/58

	// turn on digital interface clock
	PM->APBAMASK.reg |= PM_APBAMASK_RTC | PM_APBAMASK_EIC;

	// Generic clock 2 will have divider = 4 (/32), with 32K crystal it gives tick about every mSec
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4);
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Using UltraLowPower 32K internal crystal for generic clock 2
	GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_RUNSTDBY);
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Connect RTC to generic clock 2
	GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID(GCM_RTC)));
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Connect EIC to generic clock 2
	GCLK->CLKCTRL.reg = (uint16_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID(GCM_EIC)));
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Disable RTC after initialization
	RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// SW reset
	RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_SWRST;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// Configure MODE0 with prescaler=1, tick per about 1 mSec
	RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE_COUNT32 | RTC_MODE0_CTRL_PRESCALER_DIV1;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// We will use overflow interrupt to wake the CPU
	RTC->MODE0.INTENSET.reg |= RTC_MODE0_INTENSET_OVF;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// Enable RTC interrupt in interrupt controller
	NVIC_EnableIRQ(RTC_IRQn);
	// Enable EIC interrupt in interrupt controller
	NVIC_EnableIRQ(EIC_IRQn);

	// Clear SW reset
	RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_SWRST;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	/* Errata: Make sure that the Flash does not power all the way down
     	* when in sleep mode. */

	NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;

	// TODO: Check if other modules can be disabled to save more power

	return EBF_OK;
}

// Based on ArduinoLowPower
// https://github.com/arduino-libraries/ArduinoLowPower/blob/master/src/samd/ArduinoLowPower.cpp
uint8_t EBF_Logic::EnterSleep(uint32_t msSleep)
{
	uint32_t timerCnt;

	// No sleep needed
	if (sleepMode == EBF_SleepMode::EBF_NO_SLEEP) {
		return EBF_OK;
	}

	// Move IEC (External Interrupt Controller) to GCLK2, which is running on low power internal 32K oscilator
	// Something else in the Arduino code touches that register (attachInterrupt?), so have to do it on every sleep entry
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID(GCM_EIC));
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Count up to the overflow (-1)
	RTC->MODE0.COUNT.reg = (uint32_t)(-1) - msSleep;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// Save the sleep time for micros/millis correction
	this->sleepMs = msSleep;

	// Enable the RTC
	RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	switch (sleepMode)
	{
	case EBF_SleepMode::EBF_SLEEP_LIGHT:
		SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
		PM->SLEEP.reg = 2;
		__DSB();
		__WFI();
		break;

	case EBF_SleepMode::EBF_SLEEP_DEEP:
	    // Turn down anything that will not be used during deep sleep
		SYSCTRL->OSC8M.bit.ENABLE = 0;
    	SYSCTRL->BOD33.bit.ENABLE = 0;

		// Save currently running modules setup
		apbBMask = PM->APBBMASK.reg;
		apbCMask = PM->APBCMASK.reg;

		PM->APBBMASK.bit.DMAC_ = 0;
		PM->APBBMASK.bit.USB_ = 0;
		PM->APBCMASK.reg = 0;

		if (SERIAL_PORT_USBVIRTUAL) {
			USBDevice.standby();
			restoreUSBDevice = 0;
		} else {
			USBDevice.detach();
			restoreUSBDevice = 1;
		}

		// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

		__DSB();
		__WFI();

		noInterrupts();
		ExitSleep();
		interrupts();

		break;

	default:
		break;
	}

	// We're after the sleep

	// Move IEC (External Interrupt Controller) back to the main GCLK0 (48MHz)
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_EIC));
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Disable the RTC
	RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

	// Get RTC counter
	RTC->MODE0.READREQ.reg = RTC_READREQ_RREQ;
	while (RTC->MODE0.STATUS.bit.SYNCBUSY);

  	timerCnt = RTC->MODE0.COUNT.reg;
	if (timerCnt == 0) {
		// RTC timer overflow, the ISR stops and zeroes the counter, so we can be sure it will be 0 in that case
		// We slept the whole period we wanted, advance the additions counter
		microsAddition += sleepMs*1000;
	} else {
		// RTC didn't overflow yet, some other reason woke the CPU from sleep
		// Advance the additions counter with the delta
		microsAddition += (sleepMs - ((uint32_t)(-1) - timerCnt)) * 1000;
	}

	// Back to normal CPU oreration
	return EBF_OK;
}

void EBF_Logic::ExitSleep()
{
	// We're in deep sleep, restore the registers
	if (PM->APBCMASK.reg == 0) {
		// Enable systick interrupt
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
		if (restoreUSBDevice) {
			USBDevice.attach();
		}

		// Restore currently running modules
		PM->APBCMASK.reg = apbCMask;
		PM->APBBMASK.reg = apbBMask;
	}
}
#else
	#error Current board type is not supported
#endif

#endif
