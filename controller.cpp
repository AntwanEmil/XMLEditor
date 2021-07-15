#include "controller.h"
#include "optimizer.h"

#include <fstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <stack>
#include <vector>
#include <map>

using namespace std;


list<string> *Controller::json(list<Token>* tt){
        string line;
        list<string> *json_line = new list<string>;
        int spacesCount = 1;
        int t_type,st_type,next_element;
        //list <string> tAttribute;
        list <Attribute> ::iterator attributeItr;
        list <Attribute> attr;
        int dont_close = 0,sqrebracket=0;
        string prev_name;
        list <string> tData,Data2;
        list <Token> ::iterator Itr;
        list <Token> ::iterator ItrCp;
        list <string> ::iterator strItr;
        list <Token>::iterator it2;
        vector<string> v;
        map<int, bool> square;
        string taps = "", s = "";

        json_line->push_back("{");


        for (Itr = tt-> begin(); Itr != tt->end(); Itr++){
            t_type = Itr->get_type();
            auto second_type = std::next(Itr, 1);
            if (second_type != tt->end())
                st_type = second_type->get_type();				//get the next element type
            else st_type = 1;

            if (Itr != tt->begin()){
                auto prev_type = std::prev(Itr, 1);
                prev_name = prev_type->get_name();
            }

            if (t_type == 1){
                spacesCount--;
                s= "";
                for(int i=0;i<spacesCount;i++)
                    taps+="    ";

                if(square[spacesCount]==true){
                    auto third = std::next(Itr ,1);
                    Data2 = third->get_data();

                    if(!Data2.empty()){
                    for (strItr = Data2.begin();strItr != Data2.end() ; strItr++){
                        s += *strItr +"\\n";
                        }
                    s = s.substr(0, s.size() - 2);

                    line =taps + "\"" + s + "\"";

                    json_line->push_back(line);

                    }

                  line = "",s= "",taps="";
                }

                if(spacesCount >= 0 && dont_close ==0 && sqrebracket!=10){
                    for(int i=0;i<spacesCount ;i++){
                    line+= "    ";
                        }
                    if (!st_type){
                           if(square[spacesCount] == true) {line += "]"; square[spacesCount]=false;}
                            else line += "},";
                    }
                    else{
                        if(square[spacesCount]==true){ line += "]"; square[spacesCount]=false;}
                         else line += "}";
                    }
                    json_line->push_back(line);
                    }
            line = "";
            dont_close = 0;
            if(sqrebracket==10){sqrebracket=0;spacesCount++;}
            }

            else if (t_type == 0 || (t_type == 2&& Itr->get_name() != "DOCTYPE")){		//open tag//
                if(sqrebracket==10) {spacesCount++; sqrebracket=0;}
                attr = Itr->get_attributes();
                tData = Itr->get_data();
                if (sqrebracket == 1){sqrebracket=11;}

                //check if it has repeated elements :)
                prev_name = "";


                ///// For concatination of elements with the same name //////
                if (second_type != tt->end()&&second_type->get_type()==1){
                    auto third_type = std::next(Itr, 2);
                if(third_type != tt->end() && third_type->get_type() == 0 && third_type->get_name() == Itr->get_name()){
                    sqrebracket = 1;
                    square[spacesCount] = true;
                    }
                }


                for(int i =0 ;i<spacesCount; i++){
                        line += "    ";
                        taps += "    ";
                        }
                 ///////////////////////////////////
                if(Itr->get_name() != prev_name && sqrebracket !=11){
                line+= "\"" + Itr->get_name() + "\"" ;
                spacesCount++;
                if (st_type == 0 || st_type==2 || !attr.empty()){
                        line += ": {";
                        json_line->push_back(line);
                    }
                else if(sqrebracket==1){ line += ": [";  json_line->push_back(line);}
                else line += ": ";
              }

                ItrCp = Itr;
                ItrCp++;
                int no_comma = 0;
                if (ItrCp != tt->end()){
                    auto second_type = std::next(ItrCp, 1);
                    if (second_type != tt->end()){
                        next_element = second_type->get_type();
                        if (next_element == 0)no_comma = 1;
                    }
                }
                s="";

                if(!tData.empty() && sqrebracket != 11){
                for (strItr = tData.begin();strItr != tData.end() ; strItr++){
                    s += *strItr +"\\n";
                    }
                s = s.substr(0, s.size() - 2);

                if (st_type == 0){
                     line = taps + "\"" + s + "\"" +","; dont_close = 0;
                }
                else if(sqrebracket==1){
                    line = taps + "\"" + s + "\"" +","; dont_close = 0;
                }
                else{
                        line += "\"" + s + "\"" + ",";
                        dont_close = 1; }
                if (no_comma==0){
                    line = line.substr(0, line.size() - 1);
                }

                json_line->push_back(line);

                }


                if(!attr.empty()&& sqrebracket != 11){
                     // printing the found attributes ///
                     for (attributeItr = attr.begin(); attributeItr != attr.end(); attributeItr++)
                            {
                                if (st_type == 0){
                                    if (attr.size() > 0)
                                        line = taps +"    " + "\"" + attributeItr->key() + "\": " + attributeItr->value() + ",";
                                    dont_close = 0;
                                }
                                else { line = taps+ "    " + "\"" + attributeItr->key() + "\": " + attributeItr->value() + ","; dont_close = 1; }
                                if (no_comma == 0 && st_type != 0)
                                    line = line.substr(0, line.size() - 1);
                            json_line->push_back(line);
                              }

                }
            if(t_type == 2){
                spacesCount--;
                if(st_type==0)
                    json_line->push_back(taps + "},");
                else
                    json_line->push_back(taps + "}");


                if (Itr != tt->begin()){
                    auto prev_type = std::prev(Itr, 1);
                if(prev_type->get_type() == 0)
                dont_close =0;
                }

            }       if(sqrebracket==11){ sqrebracket = 10;}
                    taps = "";
                    line = "";
            }

    }
    json_line->push_back("}");
    return json_line;
}


list<Token>* Controller::parsing(list<string>* FileInfo)
{
    list<Token>* tokensList = new list<Token>;
    int charIndex = 0;
    int qoutes = 0;
    string tempStr = "";
    string line;

    list <string> ::iterator lineItr;
    for (lineItr = FileInfo->begin(); lineItr != FileInfo->end(); lineItr++)
    {
        line = *lineItr;

        charIndex = 0;
        tempStr = "";

        while (line[charIndex] != '\0')
        {
            Token t;
            Attribute atrTemp;
            while (line[charIndex] != '<')
            {
                 charIndex++;
                 if (line[charIndex] == '\0')
                 {
                     lineItr++;
                     line = *lineItr;
                     charIndex = 0;
                 }
            }
            charIndex++;


            /* parsing of comment tags */
            if (line[charIndex] == '!' && line[charIndex+1] == '-' && line[charIndex+2] == '-')
            {
                t.set_type(comment);
                charIndex += 3;

                while(1)
                {
                    while (line[charIndex] == ' ' || line[charIndex] == '\t')
                        charIndex++;

                    if (line[charIndex] == '\0')
                    {
                        lineItr++;
                        line = *lineItr;
                        charIndex = 0;
                    }
                    else
                        break;
                }

                while (!(line[charIndex] == '-' && line[charIndex+1] == '-' && line[charIndex+2] == '>'))
                {
                    tempStr += line[charIndex];
                    charIndex++;

                    if (line[charIndex] == '\0')
                    {
                        lineItr++;
                        line = *lineItr;
                        charIndex = 0;
                    }
                }
                t.add_data(tempStr);
                tempStr = "";
                charIndex += 3;
            }

            else
            {
                /* Parsing of selfClosing tags. */
                if (line[charIndex] == '?')
                {
                    charIndex++;
                    while (line[charIndex] != ' ' && line[charIndex] != '\t')
                    {
                        tempStr += line[charIndex];
                        charIndex++;
                    }

                    t.set_type(selfClosing);
                    t.set_name(tempStr);
                    tempStr = "";

                    while(1)
                    {
                        while (line[charIndex] == ' ' || line[charIndex] == '\t')
                            charIndex++;

                        if (line[charIndex] == '\0')
                        {
                            lineItr++;
                            line = *lineItr;
                            charIndex = 0;
                        }
                        else
                            break;
                    }

                    while (line[charIndex] != '?')
                    {
                        while (qoutes < 2)
                        {
                            while(1)
                            {
                                if(qoutes<1)
                                {
                                    while (line[charIndex] == ' ' || line[charIndex] == '\t')
                                        charIndex++;
                                }

                                if (line[charIndex] == '\0')
                                {
                                    lineItr++;
                                    line = *lineItr;
                                    charIndex = 0;
                                }
                                else
                                    break;
                            }

                            if (line[charIndex] == '=')
                            {
                                atrTemp.setKey(tempStr);
                                tempStr = "";
                                charIndex++;

                                while(1)
                                {
                                    while (line[charIndex] == ' ' || line[charIndex] == '\t')
                                        charIndex++;

                                    if (line[charIndex] == '\0')
                                    {
                                        lineItr++;
                                        line = *lineItr;
                                        charIndex = 0;
                                    }
                                    else
                                        break;
                                }
                            }
                            tempStr += line[charIndex];
                            if (line[charIndex] == '"')
                                qoutes++;
                            charIndex++;

                        }
                        qoutes = 0;
                        atrTemp.setValue(tempStr);
                        t.add_attribute(atrTemp);
                        tempStr = "";

                    }
                    charIndex++;
                    charIndex++;
                }

                /* Parsing of closing tags. */
                else if (line[charIndex] == '/')
                {
                    charIndex++;
                    while (line[charIndex] != '>')
                    {
                        tempStr += line[charIndex];
                        charIndex++;
                    }
                    charIndex++;
                    t.set_type(closing);
                    t.set_name(tempStr);
                    tempStr = "";
                }

                /* Parsing of DOCTYPE tags. */
                else if (line[charIndex] == '!' && line[charIndex+1] == 'D' && line[charIndex+2] == 'O' && line[charIndex+3] == 'C')
                {
                    t.set_type(selfClosing);
                    t.set_name("DOCTYPE");
                    charIndex += 9;

                    while(1)
                    {
                        while (!(line[charIndex] == ' ' || line[charIndex] == '\t' || line[charIndex] == '>'))
                        {
                            if (line[charIndex] == '\0')
                            {
                                lineItr++;
                                line = *lineItr;
                                charIndex = 0;
                            }
                            tempStr += line[charIndex];
                            charIndex++;
                        }
                        atrTemp.setKey(tempStr);
                        tempStr = "";
                        atrTemp.setValue("");
                        t.add_attribute(atrTemp);

                        if(line[charIndex] == '>')
                        {
                            charIndex++;
                            break;
                        }
                    }
                }

                /* Parsing of open tags */
                else
                {
                    while (line[charIndex] != ' ' && line[charIndex] != '\t' && line[charIndex] != '>')
                    {
                        if (line[charIndex] == '\0')
                        {
                            lineItr++;
                            line = *lineItr;
                            charIndex = 0;
                        }

                        tempStr += line[charIndex];
                        charIndex++;
                    }
                    t.set_type(open);
                    t.set_name(tempStr);
                    tempStr = "";

                    while (line[charIndex] != '>')
                    {
                        while(1)
                        {
                            while (line[charIndex] == ' ' || line[charIndex] == '\t')
                                charIndex++;

                            if (line[charIndex] == '\0')
                            {
                                lineItr++;
                                line = *lineItr;
                                charIndex = 0;
                            }
                            else
                                break;
                        }

                        if (line[charIndex] != '>')
                        {
                            if (line[charIndex] == '/')
                            {
                                t.set_type(selfClosing);
                                charIndex++;
                            }
                            else
                            {
                                while (qoutes < 2)
                                {
                                    while(1)
                                    {
                                        if(qoutes<1)
                                        {
                                            while (line[charIndex] == ' ' || line[charIndex] == '\t')
                                                charIndex++;
                                        }

                                        if (line[charIndex] == '\0')
                                        {
                                            lineItr++;
                                            line = *lineItr;
                                            charIndex = 0;
                                        }
                                        else
                                            break;
                                    }

                                    if (line[charIndex] == '=')
                                    {
                                        atrTemp.setKey(tempStr);
                                        tempStr = "";
                                        charIndex++;
                                        while(1)
                                        {
                                            while (line[charIndex] == ' ' || line[charIndex] == '\t')
                                                charIndex++;

                                            if (line[charIndex] == '\0')
                                            {
                                                lineItr++;
                                                line = *lineItr;
                                                charIndex = 0;
                                            }
                                            else
                                                break;
                                        }
                                    }
                                    tempStr += line[charIndex];
                                    if (line[charIndex] == '"')
                                        qoutes++;
                                    charIndex++;

                                }
                                qoutes = 0;
                                atrTemp.setValue(tempStr);
                                t.add_attribute(atrTemp);
                                tempStr = "";
                            }
                        }
                    }
                    charIndex++;

                    while(1)
                    {
                        while (line[charIndex] == ' ' || line[charIndex] == '\t')
                            charIndex++;

                        if (line[charIndex] == '\0')
                        {
                            lineItr++;
                            line = *lineItr;
                            charIndex = 0;
                        }
                        else
                            break;
                    }
                    while (line[charIndex] != '<')
                    {
                        if((line[charIndex] == ' ' && (line[charIndex+1] == ' ' || line[charIndex+1] == '\0' || line[charIndex+1] == '\t' || line[charIndex+1] == '<' ))
                                || line[charIndex] == '\t' || line[charIndex] == '\0')
                        {}
                        else
                        {
                            tempStr += line[charIndex];
                        }

                        if (line[charIndex] == '\0')
                        {
                            lineItr++;
                            line = *lineItr;
                            charIndex = 0;
                        }
                        charIndex++;
                    }
                    if (tempStr != "")
                    {
                        t.add_data(tempStr);
                        tempStr = "";
                    }
                }
            }
            tokensList->push_back(t);
        }
    }
    return tokensList;
}


list<string>* Controller::beautifyXML(list<Token>* tokens)
{
    int tabCount = 0;
    list<string>* beautifyStr = new list<string>;
    string tempStr = "";

    list <Token> ::iterator tokenItr;
    for (tokenItr = tokens->begin(); tokenItr != tokens->end(); tokenItr++)
    {
        list<string>::iterator infoItr;
        list<Attribute>::iterator attributeItr;
        list<string> info;
        list<Attribute> attr;
        int tokenType = tokenItr->get_type();

        if (tokenType == open)
        {
            for (int i = 0; i < tabCount; i++)
                tempStr += '\t';
            tempStr = tempStr + '<' + tokenItr->get_name();

            attr = tokenItr->get_attributes();
            for (attributeItr = attr.begin(); attributeItr != attr.end(); attributeItr++)
            {
                tempStr = tempStr + " " + attributeItr->key() + "=" + attributeItr->value();
            }
            tempStr += '>';

            beautifyStr->push_back(tempStr);
            tempStr = "";

            tabCount++;

            info = tokenItr->get_data();
            for (infoItr = info.begin(); infoItr != info.end(); infoItr++)
            {
                for (int i = 0; i < tabCount; i++)
                    tempStr += '\t';
                tempStr += *infoItr;
                beautifyStr->push_back(tempStr);
                tempStr = "";
            }
        }
        else if (tokenType == closing)
        {
            tabCount--;
            for (int i = 0; i < tabCount; i++)
                tempStr += '\t';
            tempStr = tempStr + "</" + tokenItr->get_name() + '>';
            beautifyStr->push_back(tempStr);
            tempStr = "";
        }
        else if (tokenType == comment)
        {
            int once = 1;
            for (int i = 0; i < tabCount; i++)
                tempStr += '\t';
            tempStr = tempStr + "<!-- ";
            info = tokenItr->get_data();
            for (infoItr = info.begin(); infoItr != info.end(); infoItr++)
            {
                if (once)
                {
                    once = 0;
                }
                else
                {
                    beautifyStr->push_back(tempStr);
                    tempStr = "";
                    for (int i = 0; i < tabCount; i++)
                        tempStr += '\t';
                    tempStr += "        ";
                }
                tempStr += *infoItr;
            }
            tempStr += "-->";
            beautifyStr->push_back(tempStr);
            tempStr = "";
        }
        else if (tokenType == selfClosing)
        {
            string name = tokenItr->get_name();

            for (int i = 0; i < tabCount; i++)
                tempStr += '\t';

            tempStr += '<';
            if (name[0] == 'x' && name[1] == 'm' && name[2] == 'l')
                tempStr = tempStr + '?';

            else if(name == "DOCTYPE")
            {
                tempStr = tempStr + '!';
            }

            tempStr += name;

            attr = tokenItr->get_attributes();
            for (attributeItr = attr.begin(); attributeItr != attr.end(); attributeItr++)
            {
                if(name == "DOCTYPE")
                     tempStr = tempStr + " " + attributeItr->key();
                else
                    tempStr = tempStr + " " + attributeItr->key() + "=" + attributeItr->value();
            }

            if (name[0] == 'x' && name[1] == 'm' && name[2] == 'l')
                tempStr = tempStr + '?';
            else if (name == "DOCTYPE")
            {}
            else
                tempStr = tempStr + '/';
            tempStr += '>';

            beautifyStr->push_back(tempStr);
            tempStr = "";
        }
    }

    return beautifyStr;
}

list<string> *Controller::getText(const string &path)
{
    ifstream file;
    file.open(path);
    if (file.is_open()) {
        string line;
        list<string> *text = new list<string>;

        unsigned int linesCount = 0;

        while (linesCount < bufferSize() && getline(file, line)) {
            text->push_back(line);
            linesCount++;
        }
        file.close();
        return text;
    } else
        return nullptr;
}

bool Controller::saveText(const string &path, list<string> *text, bool insertNewLine)
{
    ofstream file;
    file.open(path);
    if (file.is_open()) {
        for (auto line : *text) {
            file << line;
            if (insertNewLine)
                file << '\n';
        }
        file.close();
        return true;
    }
    else
        return false;
}

string *Controller::listToString(list<string> *text, unsigned int bufferSize, bool insertNewLine)
{
    string *str = new string;
    unsigned int linesCount = 0, size = 0;
    for (string line : *text) {
        if (linesCount == bufferSize)
            break;
        size += line.size() + 1;
        linesCount++;
    }

    str->resize(size+5, '\0');
    linesCount = 0;
    unsigned int i = 0;
    for (string line : *text) {
        if (linesCount == bufferSize)
            break;
        std::copy(line.begin(), line.end(), str->begin()+i);
        i += line.size();
        if (insertNewLine) {
            (*str)[i] = '\n';
            ++i;
        }
        ++linesCount;
    }

    return str;
}

list<string> *Controller::minifyText(list<Token> *tokens)
{
//    list<string> *newText = new list<string>;
//    for (string line : *text) {
////        std::remove(line.begin(), line.end(), ' ');
////        std::remove(line.begin(), line.end(), '\t');
////        std::remove(line.begin(), line.end(), '\r');
////        while (isspace(line[0]))
////            line.erase(line.begin(), line.begin()+1);
//        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
//        newText->push_back(line);
//    }
//    return newText;

    list<string>* minifiedStr = new list<string>;
    string tempStr = "";

    list <Token>::iterator tokenItr;
    for (tokenItr = tokens->begin(); tokenItr != tokens->end(); tokenItr++)
    {
        list<string>::iterator infoItr;
        list<Attribute>::iterator attributeItr;
        list<string> info;
        list<Attribute> attr;
        int tokenType = tokenItr->get_type();
        tempStr = "";

        if (tokenType == open)
        {
            tempStr = tempStr + '<' + tokenItr->get_name();

            attr = tokenItr->get_attributes();
            for (attributeItr = attr.begin(); attributeItr != attr.end(); attributeItr++)
            {
                tempStr = tempStr + " " + attributeItr->key() + "=" + attributeItr->value();
            }
            tempStr += '>';

            minifiedStr->push_back(tempStr);
            tempStr = "";

            info = tokenItr->get_data();
            if (info.size() > 0)
                tempStr = *info.begin();
            for (infoItr = ++info.begin(); infoItr != info.end(); infoItr++)
                tempStr += " " + *infoItr;
            minifiedStr->push_back(tempStr);
        }
        else if (tokenType == closing)
        {
            tempStr = tempStr + "</" + tokenItr->get_name() + '>';
            minifiedStr->push_back(tempStr);
        }
        else if (tokenType == comment)
        {
//            int once = 1;
            tempStr = "<!-- ";
            info = tokenItr->get_data();
            tempStr += *info.begin();
            for (infoItr = ++info.begin(); infoItr != info.end(); infoItr++) {
//                if (once)
//                    once = 0;
//                else
//                {
//                    minifiedStr->push_back(tempStr);
//                    tempStr = "";
//                    tempStr += "     ";
//                }
                tempStr += " " + *infoItr;
            }
            tempStr += "-->";
            minifiedStr->push_back(tempStr);
        }
        else if (tokenType == selfClosing)
        {
            string name = tokenItr->get_name();
            tempStr += '<';
            if (name[0] == 'x' && name[1] == 'm' && name[2] == 'l')
                tempStr = tempStr + '?';

            else if(name == "DOCTYPE")
            {
                tempStr = tempStr + '!';
            }

            tempStr += name;

            attr = tokenItr->get_attributes();
            for (attributeItr = attr.begin(); attributeItr != attr.end(); attributeItr++)
            {
                if(name == "DOCTYPE")
                     tempStr = tempStr + " " + attributeItr->key();
                else
                    tempStr = tempStr + " " + attributeItr->key() + "=" + attributeItr->value();
            }

            if (name[0] == 'x' && name[1] == 'm' && name[2] == 'l')
                tempStr = tempStr + '?';
            else if (name == "DOCTYPE")
            {}
            else
                tempStr = tempStr + '/';
            tempStr += '>';

            minifiedStr->push_back(tempStr);
        }
    }

    return minifiedStr;
}

bool Controller::solveIncon(list<Token> *tokens, list<int> *&added)
{
    std::stack<std::string> st;
    string name;
    added = new list<int>;

    int i = 0;
    for (list<Token>::iterator itr = tokens->begin(), temp; itr != tokens->end(); ++itr, ++i) {
        switch (itr->get_type()) {
        case tokenType::open:
            name = itr->get_name();
            if (!st.empty() && name == st.top()) {
                tokens->insert(itr, Token (name, tokenType::closing));
                st.pop();
                added->push_back(i++);
            }
            if (itr->get_data().size() > 0) {
                temp = itr;
                ++temp;
                if (temp != tokens->end() && temp->get_type() != tokenType::closing) {
                    tokens->insert(temp, Token(name, closing));
                    added->push_back(i+1);
                }
            }
            st.push(name);
            break;

        case tokenType::closing:
            if (!st.empty() && st.top() != itr->get_name()) {
                itr->set_name(st.top());
                added->push_back(i);
            }
            if (st.empty())
                return false;
            st.pop();
            break;

        default: break;
        }
    }
    while (!st.empty()) {
        tokens->push_back(Token(st.top(), tokenType::closing));
        added->push_back(i++);
        st.pop();
    }
    return true;
}



bool Controller::checkBalance(list<Token> *Nodess)
{
    list<Token>::iterator t;
    stack<string> s;
//    list<Token> Nodess = Nodes;
    for (t = Nodess->begin(); t != Nodess->end(); ++t)
    {
//        Token t = Nodess.front();
        if (t->get_type() == open)
        {
            s.push(t->get_name());
        }
        else if (t->get_type() == closing)
        {
            if (s.empty())
            {
                return 0;
            }
            if (t->get_name() == s.top())
            {
                s.pop();
            }
            else if (t->get_name() != s.top())
            {
                return 0;
            }
        }
//        Nodess.pop_front();
    }
    if (!s.empty())
    {
        return 0;
    }
    return 1;
}


void Controller::saveCompressed(const string &path, list<Token> *tokens)
{
    Optimizer opt;
    opt.writeFile(path, tokens);
}

list<Token> *Controller::openCompressed(const string &path)
{
    Optimizer opt;
    return opt.readFile(path);
}
