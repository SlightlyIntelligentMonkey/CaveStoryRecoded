#include "stdUtils.h"

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

string hexToString(unsigned int i)
{
	stringstream s;
	s << std::hex << i;
	return s.str();
}

string octToString(unsigned int i)
{
    stringstream s;
    s << std::oct << i;
    return s.str();
}
