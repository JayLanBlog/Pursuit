#pragma once
#include <file/sys_file.h>

// rtext: Configuration values
//------------------------------------------------------------------------------------
#define MAX_TEXT_BUFFER_LENGTH       1024       // Size of internal static buffers used on some functions:
												// TextFormat(), TextSubtext(), TextToUpper(), TextToLower(), TextToPascal(), TextSplit()
#define MAX_TEXTSPLIT_COUNT           128       // Maximum number of substrings to split: TextSplit()



namespace Text {

	// Formatting of text with variables to 'embed'
// WARNING: String returned will expire after this function is called MAX_TEXTFORMAT_BUFFERS times
	const char* TextFormat(const char* text, ...);
}