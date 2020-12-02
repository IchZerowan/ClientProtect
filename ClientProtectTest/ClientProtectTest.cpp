#include <iostream>
#include "ClientProtect.h"

using namespace std;

int main()
{
    const char* filename = "example.bin";
    string str = "Hello, protected string!";

    // Writing string to the file
    cout << "Writing string to the file" << endl;
    cp_file_write_bytes(filename, (char*) str.c_str(), str.size());
    cout << "String written to the file" << endl << endl;
    
    // Reading string from the file
    cout << "Reading string from the file:" << endl;
    char* buff = new char[50];
    unsigned int stringSize = cp_file_read_bytes(filename, buff, 50);

    string fromFile(buff, stringSize);

    cout << fromFile << endl;
}
