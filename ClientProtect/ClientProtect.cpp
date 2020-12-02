#include "pch.h"
#include "ClientProtect.h"

#include <fstream>
#include <sstream>
#include <combaseapi.h>
#include <iomanip>
#include <time.h>
#include <algorithm>

// AES library from https://github.com/SergeyBel/AES/ 
#include "AES.h"


using namespace std;


// Some global value
byte value = 183;


// Should return 32 bytes of unique data
byte* getKey() {
	INT64 otherValue = 39453834233245;
	byte* result = new byte[32];
	std::fill(result, result + 32, 0);

	// Fill array with random staff
	result[22] = sizeof(result); // 22
	result[27] = value;          // 27
	result[value] = value * 152; // 11

	otherValue /= 3;

	memcpy(result + 14, "Hello, World!", 13); // 15-26

	result[18] = value ^ -5;

	char buff[25];
	sprintf_s(buff, "%d%p", 958, (void*) (result[19])); 
	memcpy(result, buff, 9); // 0-9


	// With some system-specific info
	char buff2[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = 0;
	GetComputerNameA(buff2, &size);
	memcpy(result + 28, buff2, 4); // 28-31
	memcpy(result + 9, &size, 2); // 9-10

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	*((DWORD*) result + 3) = siSysInfo.wProcessorRevision; // 12-13

	int eaxdata = 0;
	__asm
	{
		mov eaxdata, eax
	}

	*((DWORD*)result + 5) = eaxdata;
	

	return result;
}


// Some basic transformations
byte* transform1(byte* result) {
	for (int i = 0; i < 32; i++) {
		result[i] -= (32 + i);
		result[i] *= i - 23;
	}
	return result;
}


// Some fake basic transformations
byte* transform2(byte* result) {
	for (int i = 0; i < 32; i++) {
		result[i] = -5 * result[i];
		result[i] += 1;
	}
	result[13]--;
}


// Some basic transformations
byte* transform3(byte* result) {
	result[10] = result[4];
	result[4] = result[22];
	result[22] = result[0];
	return result;
}


// Some basic transformations
byte* transform4(byte* result) {
	sort(&result[0], &result[32]);
	reverse(&result[3], &result[27]);
	return result;
}


// Some basic transformations
byte* transform5(byte* result) {
	rotate(result, &result[32] - 4, &result[32]);
	for (int i = 0; i < 32; i++) {
		result[i] = ~result[i];
	}
	return result;
}


// Some complex transformation with 2 keys
byte* mix(byte* result1, byte* result2) {
	for (int i = 0; i < 32; i++) {
		result1[i] = result1[i] ^ result2[i] | (65 * i);
	}
	memcpy(result2, result1, 16);
	return result1;
}


// Prints generated key
void printArray(byte* arr, int size) {
	cout << hex;
	for (int i = 0; i < size; i++) {
		cout << setw(2) << setfill('0') << (int) arr[i] << " ";
	}
	cout << dec << endl;
}


CLIENTPROTECT_API void cp_file_write_bytes(const char* filename, char* data, int size)
{
	byte* r1 = transform1(transform4(getKey()));
	byte* r2 = mix(transform1(getKey()), r1);
	byte* r3 = transform3(transform5(r1));
	r2 = mix(r2, r3);
	
	// Encrypt data
	AES aes(256);
	unsigned int outLen = 0;
	unsigned char* bytes = aes.EncryptECB((unsigned char*)data, size, r3, outLen);

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
	byte* r1 = transform1(transform4(getKey()));
	byte* r2 = mix(transform1(getKey()), r1);
	byte* r3 = transform3(transform5(r1));
	r2 = mix(r2, r3);

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
	unsigned char* result = aes.DecryptECB((unsigned char*)str.c_str(), str.length(), r3);
	return (char*)result;
}
