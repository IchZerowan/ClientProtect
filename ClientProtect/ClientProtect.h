#pragma once

#ifdef CLIENTPROTECT_EXPORTS
#define CLIENTPROTECT_API __declspec(dllexport)
#else
#define CLIENTPROTECT_API __declspec(dllimport)
#endif

#include <string>

typedef unsigned char byte;
typedef unsigned int stringsize;


// Writes data bytes to the specified file
extern "C" CLIENTPROTECT_API void* cp_file_write_bytes(const char* filename, char* data, int size);

// Reads data bytes from the specified file to the buffer
// If buffer size is less then the file size, only size bytes are read
extern "C" CLIENTPROTECT_API char* cp_file_read_bytes(const char* filename, int& outSize);