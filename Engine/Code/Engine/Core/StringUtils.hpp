#pragma once
//-----------------------------------------------------------------------------------------------
#include "Engine/Core/EngineCommon.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

std::string Stringv(const char* format, va_list args);
Strings Split(const std::string& text, char delim);
std::string MergeStrings(Strings myStrings);
std::string ReplaceAllStrings(std::string str, const std::string& from, const std::string& to);
