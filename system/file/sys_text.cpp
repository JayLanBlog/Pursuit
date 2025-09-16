#include "sys_text.h"
#include <vcruntime_string.h>
#include <stdio.h>
#include <string.h>

namespace Text {
#pragma warning(disable: 4996)
	// Formatting of text with variables to 'embed'
// WARNING: String returned will expire after this function is called MAX_TEXTFORMAT_BUFFERS times
	const char* TextFormat(const char* text, ...) {
#ifndef MAX_TEXTFORMAT_BUFFERS
#define MAX_TEXTFORMAT_BUFFERS 4        // Maximum number of static buffers for text formatting
#endif

        // We create an array of buffers so strings don't expire until MAX_TEXTFORMAT_BUFFERS invocations
        static char buffers[MAX_TEXTFORMAT_BUFFERS][MAX_TEXT_BUFFER_LENGTH] = { 0 };
        static int index = 0;

        char* currentBuffer = buffers[index];
        memset(currentBuffer, 0, MAX_TEXT_BUFFER_LENGTH);   // Clear buffer before using

        va_list args;
        va_start(args, text);
        int requiredByteCount = vsnprintf(currentBuffer, MAX_TEXT_BUFFER_LENGTH, text, args);
        va_end(args);

        // If requiredByteCount is larger than the MAX_TEXT_BUFFER_LENGTH, then overflow occured
        if (requiredByteCount >= MAX_TEXT_BUFFER_LENGTH)
        {
            // Inserting "..." at the end of the string to mark as truncated
            char* truncBuffer = buffers[index] + MAX_TEXT_BUFFER_LENGTH - 4; // Adding 4 bytes = "...\0"
            sprintf(truncBuffer, "...");
        }

        index += 1;     // Move to next buffer for next function call
        if (index >= MAX_TEXTFORMAT_BUFFERS) index = 0;

        return currentBuffer;

	}

    //----------------------------------------------------------------------------------
// Text strings management functions
//----------------------------------------------------------------------------------
// Get text length in bytes, check for \0 character
    unsigned int TextLength(const char* text)
    {
        unsigned int length = 0;

        if (text != NULL)
        {
            // NOTE: Alternative: use strlen(text)

            while (*text++) length++;
        }

        return length;
    }

    bool TextIsEqual(const char* text1, const char* text2) {
        bool result = false;

        if ((text1 != NULL) && (text2 != NULL))
        {
            if (strcmp(text1, text2) == 0) result = true;
        }

        return result;
    }

    char* TextToLower(const char* text) {
        static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
        memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

        if (text != NULL)
        {
            for (int i = 0; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[i] != '\0'); i++)
            {
                if ((text[i] >= 'A') && (text[i] <= 'Z')) buffer[i] = text[i] + 32;
                else buffer[i] = text[i];
            }
        }

        return buffer;
    }

    const char* TextSubtext(const char* text, int position, int length)
    {
        static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
        memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

        int textLength = TextLength(text);

        if (position >= textLength)
        {
            return buffer; //First char is already '\0' by memset
        }

        int maxLength = textLength - position;
        if (length > maxLength) length = maxLength;
        if (length >= MAX_TEXT_BUFFER_LENGTH) length = MAX_TEXT_BUFFER_LENGTH - 1;

        // NOTE: Alternative: memcpy(buffer, text + position, length)

        for (int c = 0; c < length; c++)
        {
            buffer[c] = text[position + c];
        }

        buffer[length] = '\0';

        return buffer;
    }

}