#include "stdUtils.h"

#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::hex;

string hexToString(unsigned int i)
{
	stringstream s;
	s << hex << i;
	return s.str();
}
