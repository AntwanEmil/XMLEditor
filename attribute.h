#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>

using std::string;

class Attribute
{
public:
    Attribute();

    string key() const;
    void setKey(const string &key);

    string value() const;
    void setValue(const string &value);

private:
    string m_key, m_value;
};

#endif // ATTRIBUTE_H
