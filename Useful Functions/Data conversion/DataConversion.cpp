/*
This file will contain useful conversion function
*/

#include <Windows.h>
//#include <string>
#include <sstream>

#include DataConversion.h

/*
Convert a DWORD into a string
*/
std::string ConvertDWORDToString(DWORD dwValueToConvert)
{
	std::ostringstream stream;
	stream << dwValueToConvert;
	return stream.str();
}