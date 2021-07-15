#ifndef HELPER_H
#define HELPER_H

#include <fstream>
#include <vector>

using namespace std;

using uint = unsigned int;
using uchar = unsigned char;

const uint BUFFER_SIZE = 256;
const uint EOS = '\n';

enum Section {tags = 240, keys, values, data};

void read(ifstream &file, uint &number, int bytesCount);

void write(ofstream &file, uint number, int bytesCount);

void readString(ifstream &file, string &str, uint eos = EOS);

void writeString(ofstream &file, string &str, uint eos = EOS);

bool isEndOf(ifstream &file, uint symbol);

void neglect(ifstream &file, uint bytesCount);

template<class T>
uint binarySearch(vector<T> &v, const T &value)
{
    int i = 0, j = v.size()-1;
    while (i <= j) {
        int mid = (i+j)/2;
        if (v[mid] == value)
            return mid;
        else if (v[mid] > value)
            j = mid-1;
        else
            i = mid+1;
    }
    return -1;
}

#endif // HELPER_H
