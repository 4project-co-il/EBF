#ifndef __EBF_GLOBAL_H__
#define __EBF_GLOBAL_H__

#define EBF_NO_POLLING 0xFFFFFFFF

// EBF error codes
enum EBF_ERROR_CODE : uint8_t {
	EBF_OK = 0,

	EBF_NOT_ENOUGH_MEMORY,
	EBF_INDEX_OUT_OF_BOUNDS,
	EBF_RESOURCE_IS_IN_USE,
	EBF_NOT_INITIALIZED,
	EBF_INVALID_STATE,
	EBF_COMMUNICATION_PROBLEM,
};

// EBF sleep modes
enum EBF_SleepMode : uint8_t {
	EBF_NO_SLEEP = 0,
	EBF_SLEEP_LIGHT,
	EBF_SLEEP_DEEP = 255
};

// Generic callback function type
typedef void (*EBF_CallbackType)();

// Error reporting callback function type
typedef void (*EBF_ErrorHandlerType)(const char *module, uint32_t line, EBF_ERROR_CODE error);

#ifndef EBF_REMOVE_DEBUG_CODE
// Error reporting macro. Should be called from a class that has a "EBF_DEBUG_MODULE_NAME" defined
#define EBF_REPORT_ERROR_INT(error) (ReportError(moduleName, __LINE__, (EBF_ERROR_CODE)error))
#define EBF_REPORT_ERROR(error) (EBF_Core::ReportError(moduleName, __LINE__, (EBF_ERROR_CODE)error))
#define EBF_DEBUG_MODULE_NAME(name) const char* moduleName=name;
#else
#define EBF_REPORT_ERROR_INT(error) ({;})
#define EBF_REPORT_ERROR(error) ({;})
#define EBF_DEBUG_MODULE_NAME(name)
#endif

#endif
