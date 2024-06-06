#ifndef __EBF_H__
#define __EBF_H__

#include <Arduino.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "Core/EBF_Core.h"
#include "Core/EBF_Logic.h"
#include "Core/EBF_Timers.h"
#include "Core/EBF_MessageQueue.h"
#include "Core/EBF_HalInstance.h"

#include "Core/EBF_DigitalInput.h"
#include "Core/EBF_DigitalOutput.h"
#include "Core/EBF_PwmOutput.h"
#include "Core/EBF_AnalogInput.h"
#include "Core/EBF_Serial.h"

#endif
