#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

std::string Stringv(const char* format, va_list args)
{
	//create buffer
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];

	vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, format, args);

	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	std::string output(messageLiteral);

	return output;
}

Strings Split(const std::string & text, char delim)
{
	Strings result;
	size_t start = 0;
	size_t end = text.find(delim, start);

	while (end != -1) 
	{
		result.push_back(text.substr(start, end - start));
		start = end + 1;
		end = text.find(delim, start);
	}
	result.push_back(text.substr(start));

	return result;
}

std::string MergeStrings(Strings myStrings)
{
	std::string output;
	for (unsigned int i = 0; i < myStrings.size(); i++)
	{
		output.append(myStrings[i]);
	}
	return output;
}

std::string ReplaceAllStrings(std::string str, const std::string& from, const std::string& to) 
{
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) 
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}
