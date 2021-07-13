#include "attribute.h"

Attribute::Attribute()
{

}

string Attribute::key() const
{
    return m_key;
}

void Attribute::setKey(const string &key)
{
    m_key = key;
}

string Attribute::value() const
{
    return m_value;
}

void Attribute::setValue(const string &value)
{
    m_value = value;
}
