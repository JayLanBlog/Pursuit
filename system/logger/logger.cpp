#include "logger.h"

#include "menu/console.h"
#include <helper/obf.h>


#if defined(SUPPORT_CONSOLE_TRACELOG)
void TraceConsoleLog(int logType, const char* text, ...) {
    char buffer[MAX_TRACELOG_MSG_LENGTH] = { 0 };
    va_list args;
    va_start(args, text);

    unsigned int textSize = (unsigned int)strlen(text);
    memcpy(buffer + strlen(buffer), text, (textSize < (MAX_TRACELOG_MSG_LENGTH - 12)) ? textSize : (MAX_TRACELOG_MSG_LENGTH - 12));
    strcat(buffer, "\n");
    vsnprintf(buffer, sizeof(buffer), text, args);
    //sprintf(resBuffer, buffer, args);
    va_end(args);
    switch (logType)
    {
    case LOG_TRACE:
        Console::i().logInfo(buffer);
        break;
    case LOG_DEBUG:
        Console::i().logInfo(buffer);
        break;
    case LOG_INFO:
        Console::i().logInfo(buffer);
        break;
    case LOG_WARNING:
        Console::i().logInfo(buffer);
        break;
    case LOG_ERROR:
        Console::i().logError(buffer);
        break;
    case LOG_FATAL:
        Console::i().logError(buffer);
        break;
    default: break;
    }

}
#endif