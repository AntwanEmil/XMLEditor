#include "Tokens.h"
#include <string>
#include <list>

using namespace std;

void Token::set_name(string s)
{
	name = s;
}

string Token::get_name()
{
	return name;
}

void Token::set_type(tokenType t)
{
	tType = t;
}

tokenType Token::get_type()
{
	return tType;
}

void Token::add_attribute(Attribute t)
{
	attributes.push_back(t);
}

list <Attribute> Token::get_attributes()
{
	return attributes;
}

void Token::add_data(string t)
{
	data.push_back(t);
}

list <string> Token::get_data()
{
	return data;
}
