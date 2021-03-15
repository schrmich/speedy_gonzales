#include "log.h"
#include <avr/pgmspace.h>
#include <stdlib.h>


/** Dieser Typ definiert die Typen der Log-Ausgaben. */
typedef enum {
	LOG_TYPE_DEBUG = 0,	/**< Allgemeines Debugging */
	LOG_TYPE_INFO,		/**< Allgemeine Informationen */
	LOG_TYPE_ERROR,		/**< Fehler aufgetreten */
	LOG_TYPE_FATAL,		/**< Fataler Fehler */
}logType_t;

#define LOG_TERMINIG '\n'

/* Log-Typen als String, auf MCU im Flash */
static const char debug_str[] PROGMEM = "-DEBUG- "; /**< Log Typ Debug */
static const char info_str[] PROGMEM = "-INFO- "; /**< Log Typ Info */
static const char warn_str[] PROGMEM = "-WARNING- "; /**< Log Typ Warnung */
static const char error_str[] PROGMEM = "-ERROR- "; /**< Log Typ Fehler */
static const char fatal_str[] PROGMEM = "-FATAL- "; /**< Log Typ fataler Fehler */



void _log_Debug(const char *format, ...)
{
	#ifdef EN_LOGGING
	uart_puts_p(debug_str);
	
	va_list argPtr;
	va_start(argPtr, format);
	vfprintf_P(stdout, format, argPtr);
	va_end(argPtr);
	
	uart_putc(LOG_TERMINIG);
	#endif
}

void _log_Info(const char *format, ...)
{
	#ifdef EN_LOGGING
	uart_puts_p(info_str);
	
	va_list argPtr;
	va_start(argPtr, format);
	vfprintf_P(stdout, format, argPtr);
	va_end(argPtr);
	
	uart_putc(LOG_TERMINIG);
	#endif
	
}

void _log_Warning(const char *format, ...)
{
	#ifdef EN_LOGGING
	uart_puts_p(warn_str);
	
	va_list argPtr;
	va_start(argPtr, format);
	vfprintf_P(stdout, format, argPtr);
	va_end(argPtr);
	
	uart_putc(LOG_TERMINIG);
	#endif
	
}

void _log_Error(const char *format, ...)
{
	#ifdef EN_LOGGING
	uart_puts_p(error_str);
	
	va_list argPtr;
	va_start(argPtr, format);
	vfprintf_P(stdout, format, argPtr);
	va_end(argPtr);
	
	uart_putc(LOG_TERMINIG);
	#endif
	
}

void _log_Fatal(const char *format, ...)
{
	#ifdef EN_LOGGING
	uart_puts_p(fatal_str);
	
	va_list argPtr;
	va_start(argPtr, format);
	vfprintf_P(stdout, format, argPtr);
	va_end(argPtr);
	
	uart_putc(LOG_TERMINIG);
	#endif
	
}