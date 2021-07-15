#include "helper.h"

using namespace std;

void read(ifstream &file, uint &number, int bytesCount) {
    number = 0;
    file.read((char*) &number, bytesCount);
}

void write(ofstream &file, uint number, int bytesCount) {
    file.write((char*) &number, bytesCount);
}

void readString(ifstream &file, string &str, uint eos) {
    str = "";
    uint i = 0;
    char buffer[BUFFER_SIZE];
    uchar c;
    while (true) {
        c = file.get();
        if (c == eos) {
            buffer[i] = '\0';
            str += buffer;
            return;
        }
        else if (i == BUFFER_SIZE-2) {
            buffer[i++] = c;
            buffer[i] = '\0';
            str += buffer;
            i = 0;
        }
        else
            buffer[i++] = c;
    }
}

void writeString(ofstream &file, string &str, uint eos) {
    file.write(str.c_str(), str.size());
    write(file, eos, 1);
}

bool isEndOf(ifstream &file, uint symbol) {
    return (uchar) file.peek() == symbol;
}

void neglect(ifstream &file, uint bytesCount) {
    char c[bytesCount];
    file.read(c, bytesCount);
}
