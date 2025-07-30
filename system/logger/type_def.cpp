#include "type_def.h"

// Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
void TraceLog(int logType, const char* text, ...)
{
#if defined(SUPPORT_TRACELOG)
    va_list args;
    va_start(args, text);

    if (traceLog)
    {
        traceLog(logType, text, args);
        va_end(args);
        return;
    }
#if defined(PLATFORM_ANDROID)

#else
    char buffer[MAX_TRACELOG_MSG_LENGTH] = { 0 };

    switch (logType)
    {
    case LOG_TRACE: strcpy(buffer, "TRACE: "); break;
    case LOG_DEBUG: strcpy(buffer, "DEBUG: "); break;
    case LOG_INFO: strcpy(buffer, "INFO: "); break;
    case LOG_WARNING: strcpy(buffer, "WARNING: "); break;
    case LOG_ERROR: strcpy(buffer, "ERROR: "); break;
    case LOG_FATAL: strcpy(buffer, "FATAL: "); break;
    default: break;
    }

    unsigned int textSize = (unsigned int)strlen(text);
    memcpy(buffer + strlen(buffer), text, (textSize < (MAX_TRACELOG_MSG_LENGTH - 12)) ? textSize : (MAX_TRACELOG_MSG_LENGTH - 12));
    strcat(buffer, "\n");
    vprintf(buffer, args);
    fflush(stdout);
#endif

    va_end(args);

    if (logType == LOG_FATAL) exit(EXIT_FAILURE);  // If fatal logging, exit program

#endif  // SUPPORT_TRACELOG
}
