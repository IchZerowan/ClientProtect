#include <iostream>
#include "ClientProtect.h"

using namespace std;

int main(int argc, char* argv[])
{
    const char* usage = " usage: .\\ClientProtectTest [encode|decode] filename";

    if (argc < 3) {
        cout << "No arguments provided," << usage << endl;
        return -1;
    }
    char* filename = argv[2];

    if (strcmp(argv[1], "encode") == 0) {
        // Writing string to the file
        cout << "Enter the string to save: ";
        char buffer[500];
        cin.getline(buffer, 500);
        cout << "Writing string to the file" << endl;
        cp_file_write_bytes(filename, buffer, strlen(buffer));
        cout << "String written to the file" << endl << endl;
    }

    else if (strcmp(argv[1], "decode") == 0) {
        // Reading string from the file
        cout << "Reading string from the file" << endl;
        int outSize = 0;
        char* result = cp_file_read_bytes(filename, outSize);
        cout << outSize << " bytes read from file:" << endl << result << endl << endl;
    }

    else if (strcmp(argv[1], "test") == 0) {
        string test = "Hello, World!";
        cout << "Writing string to the file" << endl;
        cp_file_write_bytes(filename, (char*) test.c_str(), test.size());
        cout << "String written to the file" << endl << endl;
        cout << "Reading string from the file" << endl;
        int outSize = 0;
        char* result = cp_file_read_bytes(filename, outSize);
        cout << outSize << " bytes read from file:" << endl << result << endl << endl;
    }

    else {
        cout << "Invalid action," << usage << endl;
        return -1;
    }
    
}
