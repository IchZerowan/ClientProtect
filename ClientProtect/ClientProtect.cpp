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

// obfy library from https://github.com/fritzone/obfy
#include "instr.h"

using namespace std;


// Some global value
byte value = N(183);


// Prints generated key
void printArray(byte* arr, int size) {
	cout << hex;
	for (int i = 0; i < size; i++) {
		cout << setw(2) << setfill('0') << (int)arr[i] << " ";
	}
	cout << dec << endl;
}


// Should return 32 bytes of unique data
byte* getKey() {
	OBF_BEGIN
	INT64 otherValue = 39453834233245;
	byte* result = new byte[32];
	std::fill(result, result + 32, 0);

	// Fill array with random staff
	result[22] = sizeof(result); // 22
	result[27] = value;          // 27
	result[value] = value * N(152); // 11

	otherValue /= N(3);

	const char* constant = "Hello, World!";
	IF(V(otherValue) % N(2) == N(0))
		constant = "Íållî, Wîrld!";
	ENDIF

	memcpy(result + N(14), constant, N(13)); // 15-26
	

	result[18] = value ^ -5;

	char buff[25];
	sprintf_s(buff, "%d%p", N(958), (void*) (result[19])); 
	memcpy(result, buff, 9); // 0-9


	// With some system-specific info
	TCHAR buff2[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(buff2) / sizeof(buff2[0]);
	GetComputerName(buff2, &size);
	memcpy(result + 28, buff2, 4); // 28-31
	memcpy(result + N(9), &size, 2); // 9-10

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	*((DWORD*) result + 3) = siSysInfo.wProcessorRevision; // 12-13

	int eaxdata = 0;
	__asm
	{
		mov eaxdata, eax
	}

	*((DWORD*)result + 5) = eaxdata;

	RETURN(result);
	OBF_END
}


// Some basic transformations
byte* transform1(byte* result) {
	OBF_BEGIN
	for (int i = 0; i < 32; i++) {
		result[i] -= (32 + i);
		result[i] *= i - N(23);
	}
	RETURN(result);
	OBF_END
}


// Some fake basic transformations
byte* transform2(byte* result) {
	OBF_BEGIN
	for (int i = 0; i < 32; i++) {
		result[i] = -5 * result[i];
		result[i] += 1;
	}
	result[13]--;
	RETURN(result);
	OBF_END
}


// Some basic transformations
byte* transform3(byte* result) {
	OBF_BEGIN
	result[10] = result[4];
	result[4] = result[22];
	result[22] = result[0];
	RETURN(result);
	OBF_END
}


// Some basic transformations
byte* transform4(byte* result) {
	OBF_BEGIN
	sort(&result[0], &result[32]);
	reverse(&result[3], &result[N(27)]);
	RETURN(result);
	OBF_END
}


// Some basic transformations
byte* transform5(byte* result) {
	OBF_BEGIN
	rotate(result, &result[32] - 4, &result[32]);
	for (int i = 0; i < 32; i++) {
		result[i] = ~result[i];
	}
	RETURN(result);
	OBF_END
}


// Some complex transformation with 2 keys
byte* mix(byte* result1, byte* result2) {
	OBF_BEGIN
	int i;
	FOR(V(i) = N(0), V(i) < N(32), V(i) += 1) 
		result1[V(i)] = result1[V(i)] ^ result2[V(i)] | (65 * V(i));
	ENDFOR
	memcpy(result2, result1, N(16));
	RETURN(result1);
	OBF_END
}


CLIENTPROTECT_API void* cp_file_write_bytes(const char* filename, char* data, int size)
{
	OBF_BEGIN
	byte* r1 = transform1(transform4(getKey()));
	byte* r2 = mix(transform1(getKey()), r1);
	byte* r3 = transform3(transform5(r1));
	r2 = mix(r2, r3);
	
	// Encrypt data
	AES aes(256);
	unsigned int outLen = N(0);
	unsigned char* bytes = aes.EncryptECB((unsigned char*)data, size, r2, outLen);

	// Write to file
	ofstream file;
	file.open(filename);
	file << size;
	file.write((char*)bytes, outLen);
	file.close();

	// Clean up
	delete[] bytes;
	RETURN(NULL)
	OBF_END
}


CLIENTPROTECT_API char* cp_file_read_bytes(const char* filename, int& outSize)
{
	OBF_BEGIN
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
	unsigned char* result = aes.DecryptECB((unsigned char*)str.c_str(), str.length(), r2);
	RETURN((char*)result);
	OBF_END
}
