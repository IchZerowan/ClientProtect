// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h"
#include "ClientProtect.h"

#include <fstream>
#include <sstream>

// AES library from https://github.com/SergeyBel/AES/ 
#include "AES.h"


using namespace std;

unsigned char* getKey() {
	return (unsigned char*) "t7w!z%C*F-JaNdRgUjXn2r5u8x/A?D(G";
}

CLIENTPROTECT_API void cp_file_write_bytes(const char* filename, char* data, int size)
{
	// Encrypt data
	AES aes(256);
	unsigned int outLen = 0;
	unsigned char* bytes = aes.EncryptECB((unsigned char*)data, size, getKey(), outLen);

	// Write to file
	ofstream file;
	file.open(filename);
	file << size;
	file.write((char*)bytes, outLen);
	file.close();

	// Clean up
	delete[] bytes;
}

CLIENTPROTECT_API char* cp_file_read_bytes(const char* filename, int& outSize)
{
	// Read from file
	ifstream file;
	file.open(filename);
	std::stringstream inbuff;
	file >> outSize;
	inbuff << file.rdbuf();
	file.close();
	string str = inbuff.str();

	// Decrypt data
	AES aes(256);
	unsigned char* result = aes.DecryptECB((unsigned char*)str.c_str(), str.length(), getKey());
	return (char*)result;
}
