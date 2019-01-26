#include "stdUtils.h"

#include <string>
#include <sstream>

std::string hexToString(unsigned int i)
{
	std::stringstream s;
	s << std::hex << i;
	return s.str();
}
