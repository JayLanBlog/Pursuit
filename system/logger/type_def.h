#pragma once
#include <config/platform_def.h>
#include <cstdarg>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// Trace log level
// NOTE: Organized by priority level
typedef enum {
    LOG_ALL = 0,        // Display all logs
    LOG_TRACE,          // Trace logging, intended for internal use only
    LOG_DEBUG,          // Debug logging, used for internal debugging, it should be disabled on release builds
    LOG_INFO,           // Info logging, used for program execution info
    LOG_WARNING,        // Warning logging, used on recoverable failures
    LOG_ERROR,          // Error logging, used on unrecoverable failures
    LOG_FATAL,          // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    LOG_NONE            // Disable logging
} TraceLogLevel;

// Callbacks to hook some internal functions
// WARNING: These callbacks are intended for advanced users
typedef void (*TraceLogCallback)(int logLevel, const char* text, va_list args);  // Logging: Redirect trace log messages
typedef unsigned char* (*LoadFileDataCallback)(const char* fileName, int* dataSize);    // FileIO: Load binary data
typedef bool (*SaveFileDataCallback)(const char* fileName, void* data, int dataSize);   // FileIO: Save binary data
typedef char* (*LoadFileTextCallback)(const char* fileName);            // FileIO: Load text data
typedef bool (*SaveFileTextCallback)(const char* fileName, const char* text); // FileIO: Save text data

static TraceLogCallback traceLog = NULL;            // TraceLog callback function pointer
static SaveFileDataCallback saveFileData = NULL;    // SaveFileText callback function pointer
static LoadFileTextCallback loadFileText = NULL;    // LoadFileText callback function pointer
static SaveFileTextCallback saveFileText = NULL;    // SaveFileText callback function pointer

// Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
void TraceLog(int logType, const char* text, ...);