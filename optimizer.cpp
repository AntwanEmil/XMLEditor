#include <set>
#include <stack>
#include <iostream>
#include <algorithm>

#include "optimizer.h"
#include "helper.h"

Optimizer::Optimizer()
{

}

void Optimizer::printItems(std::list<Token> *tokens)
{
//    names->clear();
//    keys->clear();
//    values.clear();
//    data->clear();

//    ifstream reader;
//    reader.open("testRW.txt", ios::in | ios::binary);
//    if (!reader.is_open()) abort();

//    readFirstByte(reader);
//    readVector(reader, names, sn);
//    readVector(reader, keys, sk);
//    readVector(reader, values, sv);
//    readVector(reader, data, sd);

//    printVector(names);
//    printVector(keys);
//    printVector(values);
    //    printVector(data);
}

void Optimizer::writeFile(const string &path, std::list<Token> *tokens)
{
    clear();
    getNames(tokens);
    bool smallAC = getKeys(tokens);
    getValues(tokens);
    getData(tokens);

    ofstream writer;
    writer.open(path, ios::out | ios::binary);
    if (!writer.is_open()) abort();

    sn = bytesCount(names.size())+1;
    sk = bytesCount(keys.size())+1;
    sv = bytesCount(values.size())+1;
    sd = bytesCount(data.size())+1;

    writeFirstByte(writer);
    write(writer, smallAC, 1);
    writeVector(writer, names);
    writeVector(writer, keys);
    writeVector(writer, values);
    writeVector(writer, data);
//    writer.flush();

    for (Token &t : *tokens) {
        writeToken(writer, t, smallAC);
//        writer.flush();
    }

    write(writer, MyEOF, 1);

    writer.close();
}

std::list<Token> *Optimizer::readFile(const string &path)
{
    clear();

    ifstream reader;
    reader.open(path, ios::in | ios::binary);
    if (!reader.is_open()) abort();

    uint smallAC;

    readFirstByte(reader);
    read(reader, smallAC, 1);
    readVector(reader, names, sn);
    readVector(reader, keys, sk);
    readVector(reader, values, sv);
    readVector(reader, data, sd);

    list<Token> *tokens = new list<Token>;

    while (!isEndOf(reader, MyEOF)) {
        Token t;
        readToken(reader, t, smallAC);
        tokens->push_back(t);
    }

    reader.close();

    return tokens;
}

void Optimizer::clear()
{
    names.clear();
    keys.clear();
    values.clear();
    data.clear();
}


void Optimizer::getNames(list<Token> *tokens)
{
    std::set<string> s;
    for (Token &t : *tokens)
        s.insert(t.get_name());
    setToString(&s, names);
}

bool Optimizer::getKeys(std::list<Token> *tokens)
{
    std::set<string> s;
    uint m = 0;
    for (Token &t : *tokens) {
        uint count = 0;
        for (Attribute &a : t.get_attributes()) {
            s.insert(a.key());
            ++count;
        }
        m = max(m, count);
    }
    setToString(&s, keys);
    return m < 32;
}

void Optimizer::getValues(std::list<Token> *tokens)
{
    std::set<string> s;
    for (Token &t : *tokens)
        for (Attribute &a : t.get_attributes()) {
            s.insert(valueParser(a.value()));
        }
    setToString(&s, values);
}

void Optimizer::getData(std::list<Token> *tokens)
{
    std::set<string> s;
    for (Token &t : *tokens)
        if (!t.get_data().empty())
            s.insert(move(appendStrings(t.get_data())));
    setToString(&s, data);
}

void Optimizer::setToString(set<string> *s, vector<string> &strArr)
{
    strArr.resize(s->size());
    uint i = 0;
    for (std::set<string>::iterator itr = s->begin(); itr != s->end(); ++i, ++itr)
        strArr[i] = *itr;
}

string Optimizer::appendStrings(std::list<string> strings)
{
    string str = strings.front();
    for (list<string>::iterator itr = ++strings.begin(); itr != strings.end(); ++itr)
        str += " " + *itr;
    return str;
}

void Optimizer::printVector(vector<string> &vec)
{
    for (uint i = 0; i < vec.size(); ++i)
        cout << vec[i] << '\n';
    cout << endl;
}

void Optimizer::writeVector(ofstream &file, vector<string> &v)
{
    uint size = v.size();
    write(file, size, 1+bytesCount(size));
    for (uint i = 0; i < size; ++i) {
        writeString(file, v[i]);
    }
}

void Optimizer::readVector(ifstream &file, vector<string> &v, uint bytes)
{
    uint count = 0;
    read(file, count, bytes);
    v.resize(count);
    for (uint i = 0; i < count; ++i)
        readString(file, v[i]);
}

uint Optimizer::bytesCount(uint value)
{
    if (value < 256) //1 byte is required
        return 0;
    else if (value < 65536) //2 bytes
        return 1;
    else if (value < 16777216)
        return 2;
    else
        return 3;
}

void Optimizer::writeFirstByte(ofstream &file)
{
    uint first = 0;
    first |= bytesCount(names.size());
    first |= bytesCount(keys.size()) << 2;
    first |= bytesCount(values.size()) << 4;
    first |= bytesCount(data.size()) << 6;
    write(file, first, 1);
}

void Optimizer::readFirstByte(ifstream &file)
{
    uint first = 0;
    read(file, first, 1);
    sn = sk = sv = sd = 0;
    sn = (first & 0x3) + 1;
    sk = ((first & 0xc) >> 2) + 1;
    sv = ((first & 0x30) >> 4) + 1;
    sd = ((first & 0xc0) >> 6) + 1;
}

void Optimizer::writeToken(ofstream &file, Token &t, bool smallAC)
{
    uint first = t.get_type(), index;
    first |= (!t.get_data().empty()) << 2;
    first |= t.get_attributes().size() << 3;
    write(file, first, 1);

    switch (t.get_type()) {
    case tokenType::open:
        index = binarySearch(names, t.get_name());
        write(file, index, sn);

        for (Attribute &a : t.get_attributes()) {
            write(file, binarySearch(keys, a.key()), sk);
            write(file, binarySearch(values, valueParser(a.value())), sv);
        }

        if (!smallAC)
            write(file, Section::keys, 1);

        if (!t.get_data().empty())
            write(file, binarySearch(data, appendStrings(t.get_data())), sd);
        break;

    case tokenType::closing:
        break;

    case tokenType::selfClosing:
        write(file, binarySearch(names, t.get_name()), sn);
        if (t.get_name() != "DOCTYPE")
            for (Attribute &a : t.get_attributes()) {
                write(file, binarySearch(keys, a.key()), sk);
                write(file, binarySearch(values, valueParser(a.value())), sv);
            }
        else
            for (Attribute &a : t.get_attributes()) {
                write(file, binarySearch(keys, a.key()), sk);
            }
        if (!smallAC)
            write(file, Section::keys, 1);
        break;

    case tokenType::comment:
        write(file, binarySearch(data, appendStrings(t.get_data())), sd);
        break;
    }
}

void Optimizer::readToken(ifstream &file, Token &t, bool smallAC)
{
    uint first = 0, index = 0, count = 0;
    read(file, first, 1);

    string key, value;

    switch (first & 0x3) {
    case tokenType::open:
        t.set_type(tokenType::open); //type
        read(file, index, sn);  //name
        t.set_name(names[index]);
        st.push(t.get_name());

        count = (first & 0xf8) >> 3; //attributes
        readAttributes(file, t, true, smallAC, count);
//        for (uint i = 0; i < count; ++i) {
//            Attribute a;
//            readAttribute(file, a);
//            t.add_attribute(a);
//        }

        if (first & 0x4) { //data
            read(file, index, sd);
            t.add_data(data[index]);
        }
        break;

    case tokenType::closing:
        t.set_type(tokenType::closing); //type
        t.set_name(st.top());
        st.pop();
        break;

    case tokenType::comment:
        t.set_type(tokenType::comment); //type
        read(file, index, sd);
        t.add_data(data[index]);
        break;

    case tokenType::selfClosing:
        t.set_type(tokenType::selfClosing); //type
        read(file, index, sn);  //name
        t.set_name(names[index]);

        count = (first & 0xf8) >> 3; //attributes
        bool hasValue = t.get_name() != "DOCTYPE";
        readAttributes(file, t, hasValue, smallAC, count);
//        for (uint i = 0; i < count; ++i) {
//            Attribute a;
//            readAttribute(file, a, hasValue);
//            t.add_attribute(a);
//        }
        break;
    }
}

void Optimizer::readAttribute(ifstream &file, Attribute &a, bool hasValue)
{
    static uint index = 0;
    read(file, index, sk);
    a.setKey(keys[index]);

    if (hasValue) {
        read(file, index, sv);
        a.setValue("\"" + values[index] + "\"");
    }
}

void Optimizer::readAttributes(ifstream &file, Token &t, bool hasValue, bool smallAC, uint count)
{
    if (smallAC) {
        for (uint i = 0; i < count; ++i) {
            Attribute a;
            readAttribute(file, a, hasValue);
            t.add_attribute(a);
        }
    } else {
        while (!isEndOf(file, Section::keys)) {
            Attribute a;
            readAttribute(file, a, hasValue);
            t.add_attribute(a);
        }
        neglect(file, 1);
    }
}
