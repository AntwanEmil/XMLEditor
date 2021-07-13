#ifndef TOKENS_H_
#define TOKENS_H_

#include <string>
#include <list>
#include "attribute.h"

using namespace std;

enum tokenType { open, closing, selfClosing, comment };

class Token
{
private:
	string name;
	tokenType tType;
	list <Attribute> attributes;
	list <string> data;

public:
	void set_name(string s);
	string get_name();
	
	void set_type(tokenType t);
	tokenType get_type();

	void add_attribute(Attribute t);
	list <Attribute> get_attributes();

	void add_data(string t);
	list <string> get_data();
};

#endif /* TOKENS_H_ */
