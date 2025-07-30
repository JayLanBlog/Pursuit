#pragma once
#ifndef SYS_LOGGER
#define SYS_LOGGER
#include "type_def.h"


#if defined(PLATFORM_ANDROID)
#include <stdio.h>                      // Required for: FILE
#include <android/asset_manager.h>      // Required for: AAssetManager
#endif

#if defined(SUPPORT_TRACELOG)
#define TRACELOG(level, ...) TraceLog(level, __VA_ARGS__)

#if defined(SUPPORT_TRACELOG_DEBUG)
#define TRACELOGD(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#else
#define TRACELOGD(...) (void)0
#endif
#else
#define TRACELOG(level, ...) (void)0
#define TRACELOGD(...) (void)0
#endif

#if defined(SUPPORT_CONSOLE_TRACELOG)

void TraceConsoleLog(int logType, const char* text, ...);

#ifdef TRACELOG
#undef TRACELOG
#endif // TRACELOG
#define  TRACELOG(level, ...) TraceConsoleLog(level, __VA_ARGS__)
#endif


#endif