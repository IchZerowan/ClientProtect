// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h"
#include "ClientProtect.h"

#include <fstream>


using namespace std;

CLIENTPROTECT_API void cp_file_write_bytes(const char* filename, char* data, int size)
{
	ofstream file;
	file.open(filename);
	file.write((char*) data, size);
	file.close();
}

CLIENTPROTECT_API stringsize cp_file_read_bytes(const char* filename, char* buffer, int size)
{
	ifstream file;
	file.open(filename);
	file.read(buffer, size);
	stringsize stringSize = file.gcount();
	file.close();
	return stringSize;
}
