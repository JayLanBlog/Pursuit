#pragma once
#include <stdarg.h>  
#include <crtdbg.h>
#include <corecrt_malloc.h>
// Allow custom memory allocators
// NOTE: Require recompiling raylib sources
#ifndef MALLOC
#define MALLOC(sz)       malloc(sz)
#endif


#ifndef CALLOC
#define CALLOC(n,sz)     calloc(n,sz)
#endif
#ifndef REALLOC
#define REALLOC(ptr,sz)  realloc(ptr,sz)
#endif
#ifndef FREE
#define FREE(ptr)        free(ptr)
#endif


// Unload file data allocated by LoadFileData()
void UnloadData(unsigned char* data);