#include "controller.h"

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


