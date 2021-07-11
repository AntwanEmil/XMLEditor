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
};

#endif /* TOKENS_H_ */
