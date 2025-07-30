#include "sys_file.h"
#include <logger/logger.h>

namespace System {


	// Get pointer to extension for a filename string (includes the dot: .png)
	const char* GetFileExtension(const char* fileName)
	{
		const char* dot = strrchr(fileName, '.');

		if (!dot || dot == fileName) return NULL;

		return dot;
	}

	// Check file extension
	 bool IsFileExtension(const char* fileName, const char* ext)
	{
		bool result = false;
		const char* fileExt;

		if ((fileExt = strrchr(fileName, '.')) != NULL)
		{
			if (strcmp(fileExt, ext) == 0) result = true;
		}

		return result;
	}
#pragma warning (disable: 4996)  
	// Load data from file into a buffer
	unsigned char* LoadFileData(const char* fileName, int* dataSize) {
		unsigned char* data = NULL;
		*dataSize = 0;
		if (fileName != NULL) {
			if (loadFileData)
			{
				data = loadFileData(fileName, dataSize);
				return data;
			}
#if defined(SUPPORT_STANDARD_FILEIO)
		FILE* file = fopen(fileName, "rb");
		if (file != NULL) {
			fseek(file, 0, SEEK_END);
			int size = ftell(file);   
			fseek(file, 0, SEEK_SET);
			if (size > 0) {
				data = (unsigned char*)MALLOC(size * sizeof(unsigned char));
				if (data != NULL) {
					size_t count = fread(data, sizeof(unsigned char), size, file);
					if (count > 2147483647)
					{
						TRACELOG(LOG_WARNING, "FILEIO: [%s] File is bigger than 2147483647 bytes, avoid using LoadFileData()", fileName);

						FREE(data);
						data = NULL;
					}
					else
					{
						*dataSize = (int)count;
						if ((*dataSize) != size) TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially loaded (%i bytes out of %i)", fileName, dataSize, count);
						else TRACELOG(LOG_INFO, "FILEIO: [%s] File loaded successfully", fileName);
					}
				}
				else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", fileName);
			}
			else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read file", fileName);

			fclose(file);
		}
		else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
#else
		TRACELOG(LOG_WARNING, "FILEIO: Standard file io not supported, use custom file callback");
#endif
	}
	else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

	return data;
	}

	// Unload file data allocated by LoadFileData()
	void UnloadFileData(unsigned char* data) {
		FREE(data);
	}

	// Save data to file from buffer
	bool SaveFileData(const char* fileName, void* data, int dataSize) {
		bool success = false;

		if (fileName != NULL)
		{
			if (saveFileData)
			{
				return saveFileData(fileName, data, dataSize);
			}
#if defined(SUPPORT_STANDARD_FILEIO)
			FILE* file = fopen(fileName, "wb");

			if (file != NULL)
			{
				// WARNING: fwrite() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
				// and expects a size_t input value but as dataSize is limited to INT_MAX (2147483647 bytes), there shouldn't be a problem
				int count = (int)fwrite(data, sizeof(unsigned char), dataSize, file);

				if (count == 0) TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to write file", fileName);
				else if (count != dataSize) TRACELOG(LOG_WARNING, "FILEIO: [%s] File partially written", fileName);
				else TRACELOG(LOG_INFO, "FILEIO: [%s] File saved successfully", fileName);

				int result = fclose(file);
				if (result == 0) success = true;
			}
			else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
#else
			TRACELOG(LOG_WARNING, "FILEIO: Standard file io not supported, use custom file callback");
#endif
		}
		else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

		return success;
	}

	// Internal memory free
	void MemFree(void* ptr) {
		FREE(ptr);
	}

	// Internal memory reallocator
	void* MemRealloc(void* ptr, unsigned int size) {
		void* ret = REALLOC(ptr, size);
		return ret;
	}
	void UnloadFileText(char* text) {
		FREE(text);
	}

	// String pointer reverse break: returns right-most occurrence of charset in s
	static const char* strprbrk(const char* s, const char* charset)
	{
		const char* latestMatch = NULL;

		for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) {}

		return latestMatch;
	}

	const char* GetDirectoryPath(const char* filePath) {
		const char* lastSlash = NULL;
		static char dirPath[MAX_FILEPATH_LENGTH] = { 0 };
		memset(dirPath, 0, MAX_FILEPATH_LENGTH);
		// In case provided path does not contain a root drive letter (C:\, D:\) nor leading path separator (\, /),
// we add the current directory path to dirPath
		if ((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/'))
		{
			// For security, we set starting path to current directory,
			// obtained path will be concatenated to this
			dirPath[0] = '.';
			dirPath[1] = '/';
		}

		lastSlash = strprbrk(filePath, "\\/");
		if (lastSlash)
		{
			if (lastSlash == filePath)
			{
				// The last and only slash is the leading one: path is in a root directory
				dirPath[0] = filePath[0];
				dirPath[1] = '\0';
			}
			else
			{
				// NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
				char* dirPathPtr = dirPath;
				if ((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/')) dirPathPtr += 2;     // Skip drive letter, "C:"
				memcpy(dirPathPtr, filePath, strlen(filePath) - (strlen(lastSlash) - 1));
				dirPath[strlen(filePath) - strlen(lastSlash) + (((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/')) ? 2 : 0)] = '\0';  // Add '\0' manually
			}
		}

		return dirPath;
	}

	const char* GetWorkingDirectory(void) {
		static char currentDir[MAX_FILEPATH_LENGTH] = { 0 };
		memset(currentDir, 0, MAX_FILEPATH_LENGTH);
		char* path = GETCWD(currentDir, MAX_FILEPATH_LENGTH - 1);
		return path;
	}

	char* LoadFileText(const char* fileName) {
		char* text = NULL;

		if (fileName != NULL)
		{
			if (loadFileText)
			{
				text = loadFileText(fileName);
				return text;
			}
			FILE* file = fopen(fileName, "rt");
			if (file != NULL)
			{
				// WARNING: When reading a file as 'text' file,
				// text mode causes carriage return-linefeed translation...
				// ...but using fseek() should return correct byte-offset
				fseek(file, 0, SEEK_END);
				unsigned int size = (unsigned int)ftell(file);
				fseek(file, 0, SEEK_SET);
				if (size > 0)
				{
					text = (char*)MALLOC((size + 1) * sizeof(char));

					if (text != NULL)
					{
						unsigned int count = (unsigned int)fread(text, sizeof(char), size, file);

						// WARNING: \r\n is converted to \n on reading, so,
						// read bytes count gets reduced by the number of lines
						if (count < size) text = (char* )REALLOC(text, count + 1);

						// Zero-terminate the string
						text[count] = '\0';

						TRACELOG(LOG_INFO, "FILEIO: [%s] Text file loaded successfully", fileName);
					}
					else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", fileName);
				}
				else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to read text file", fileName);

				fclose(file);
			}
			else TRACELOG(LOG_WARNING, "FILEIO: [%s] Failed to open text file", fileName);
		}
		else TRACELOG(LOG_WARNING, "FILEIO: File name provided is not valid");

		return text;
	}

	// Internal memory allocator
	// NOTE: Initializes to zero by default
	void* MemAlloc(unsigned int size) {
		void* ptr = CALLOC(size, 1);
		return ptr;
	}

}