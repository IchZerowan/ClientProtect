#include <iostream>
#include "ClientProtect.h"

using namespace std;

int main()
{
    const char* filename = "example.bin";
    string str = "Hello, protected strings!";

    // Writing string to the file
    cout << "Writing string to the file" << endl;
    cp_file_write_bytes(filename, (char*) str.c_str(), str.size());
    cout << "String written to the file" << endl << endl;
    
    // Reading string from the file
    cout << "Reading string from the file:" << endl;
    int outSize = 0;
    char* result = cp_file_read_bytes(filename, outSize);

    cout << result << " | " << outSize << " bytes" << endl;
}
