#pragma once

#ifndef SYS_FILE
#define SYS_FILE
#include <helper/mem_help.h>
#include <section/martrix/sys_def.h>

namespace System {
	/*
		User Set OwnFile CallBack
	*/
	typedef unsigned char* (*LoadFileDataCallback)(const char* fileName, int* dataSize);    // FileIO: Load binary data
	static LoadFileDataCallback loadFileData = NULL;

	// Load data from file into a buffer
	unsigned char* LoadFileData(const char* fileName, int* dataSize);

	// Unload file data allocated by LoadFileData()
	void UnloadFileData(unsigned char* data);

	// Save data to file from buffer
	bool SaveFileData(const char* fileName, void* data, int dataSize);

	// Internal memory free
	void MemFree(void* ptr);

	// Internal memory reallocator
	void* MemRealloc(void* ptr, unsigned int size);

	// Internal memory allocator
// NOTE: Initializes to zero by default
	void* MemAlloc(unsigned int size);

	// Check file extension
	 bool IsFileExtension(const char* fileName, const char* ext);

	const char* GetFileExtension(const char* fileName);

	char* LoadFileText(const char* fileName);

	void UnloadFileText(char* text);

	const char* GetDirectoryPath(const char* filePath);

	const char* GetWorkingDirectory(void);
}

#endif // !SYS_FILE