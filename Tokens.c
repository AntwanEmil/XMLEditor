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
