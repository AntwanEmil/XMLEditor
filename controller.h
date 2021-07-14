#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <fstream>
#include <list>

#include "Tokens.h"

using namespace std;

class Controller
{
    unsigned int m_bufferSize;
public:
    //initialize the controller with buffer size = infinity (0xffffffff)
    Controller(unsigned int bufferSize = -1) : m_bufferSize(bufferSize) {}

    //get and set the buffer size
    void setBufferSize(unsigned int size) {m_bufferSize = size;}
    unsigned int bufferSize() {return m_bufferSize;}

    /*
    get the full text of the file given its path.
    the file is read as a list of strings, each string representing a line.
    the number of lines to be read <= controllers buffer size

    parameters
    path: path of the file

    return value
    pointer to list of strings, each string representing a line.
    */
    list<string>* getText(const string &path);


    /* save the text into a file given its path.
     * you can choose whether or not a new line is inserted between each string.
     *
     * parameters:
     * path: path of the file to be save
     * text: list of strings to be written into the file
     * insertNewLine: true => a new line is inserted between strings
     *                false => strings are written side by side
     *
     * return value:
     * bool: true if the file was opened successfully for reading, false otherwise
     * */
    bool saveText(const string &path, list<string> *text, bool insertNewLine = true);


    /*
     * append a list of strings into one long string.
     * the number of lines to be appended <= controllers buffer size
     *
     * paramters:
     * text: list of string to be appended
     * insertNewLine: do you want a new line between each string or not?
     *
     * return value:
     * pointer to the string created
     *
     * */
    string* listToString(list<string> *text, bool insertNewLine = true)
        {return listToString(text, m_bufferSize, insertNewLine);}


    /*
     * append a list of strings into one long string.
     * the number of lines to be appended <= buffer size specified by the user
     *
     * paramters:
     * text: list of string to be appended
     * bufferSize: a custom buffer size specified by the user
     * insertNewLine: do you want a new line between each string or not?
     *
     * return value:
     * pointer to the string created
     *
     * */
    string* listToString(list<string> *text, unsigned int bufferSize, bool insertNewLine = true);


    /*
     * remove whitespaces and tabs from each string.
     *
     * paramters:
     * text: list of token.
     *
     * return values:
     * list of strings after minifying
     *
     * */
    list<string> * minifyText(list<Token>* tokens);


    /*
     * solve some inconsistencies that might be found in a file.
     * 1: if open tag, data and missing closing tag, add it
     * 2: if open tag, tags, and missing tag, add it
     * 3: if open tag, tags, and unmatched closing tag, change the closing to make it matching
     *
     * */
    bool solveIncon(list<Token>* tokens, list<int> *&added);

    /*
     * Parse the XML file strings into tokens to deal with it.
     *
     * */
    list<Token>* parsing(list<string>* FileInfo);

    /*
     * Check Balance.
     *
     * */

    bool checkBalance(list<Token> *Nodess);

    /*
     * Beautify the XML file.
     *
     * */
    list<string>* beautifyXML(list<Token>* tokens);

    /*
     * change format xml->json
     *
     * */
    list<string>* json(list<Token>* tt);
    
        /*
     * save compressed file
     * */
    void saveCompressed(const string &path, list<Token>* tokens);

    /*
     * open compressed file
     * */
    list<Token>* openCompressed(const string &path);
};

#endif // CONTROLLER_H
