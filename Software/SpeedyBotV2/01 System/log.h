/*
 * log.h
 *
 * Created: 19.09.2019 08:17:37
 *  Author: schreiberm
 */ 


#ifndef LOG_H_
#define LOG_H_

#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#define EN_LOGGING 1


void _log_Debug(const char *format, ...);
void _log_Info(const char *format, ...);
void _log_Warning(const char *format, ...);
void _log_Error(const char *format, ...);
void _log_Fatal(const char *format, ...);

#define LOG_DEBUG(_format, ...)	_log_Debug(PSTR(_format), ## __VA_ARGS__)
#define LOG_INFO(_format, ...)	_log_Info(PSTR(_format), ## __VA_ARGS__)
#define LOG_WARNING(_format, ...)	_log_Warning(PSTR(_format), ## __VA_ARGS__)
#define LOG_ERROR(_format, ...)	_log_Error(PSTR(_format), ## __VA_ARGS__)
#define LOG_FATAL(_format, ...)	_log_Fatal(PSTR(_format), ## __VA_ARGS__)

#endif /* LOG_H_ */