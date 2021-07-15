#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stack>
#include <list>
#include <set>
#include <string>
#include <vector>

#include "Tokens.h"
#include "helper.h"

const int MyEOF = 250;

class Optimizer
{
public:
    Optimizer();

    void printItems(std::list<Token> *tokens);

    void writeFile(const string &path, std::list<Token> *tokens);
    std::list<Token> * readFile(const string &path);

private:
    vector<string> names, keys, values, data;
    uint sn, sk, sv, sd;

    void clear();

    void getNames(std::list<Token> *tokens);
    bool getKeys(std::list<Token> *tokens);
    void getValues(std::list<Token> *tokens);
    void getData(std::list<Token> *tokens);

    void setToString(std::set<string> *s, vector<string> &strArr);
    std::string appendStrings(std::list<std::string> strings);
    string valueParser(string value) {
        value.erase(value.begin(), value.begin()+1);
        value.erase(value.begin()+value.size()-1, value.begin()+value.size());
        return value;
    }

    void printVector(vector<string> &);
    void writeVector(ofstream &file, vector<string> &v);
    void readVector(ifstream &file, vector<string> &v, uint bytes);

    uint bytesCount(uint value);

    void writeFirstByte(ofstream &file);

    void readFirstByte(ifstream &file);

    void writeToken(ofstream &file, Token &t, bool smallAC = true);
    void readToken(ifstream &file, Token &t, bool smallAC = true);

    void readAttribute(ifstream &file, Attribute &a, bool hasValue = true);
    void readAttributes(ifstream &file, Token &t, bool hasValue, bool smallAC, uint count);

    stack<string> st;
};

#endif // OPTIMIZER_H
