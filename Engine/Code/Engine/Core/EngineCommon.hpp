#pragma once

#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <vector>
#include <string>

#define UNUSED(x) (void)(x);
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))

#pragma region TODO Macro

// Source from http://www.flipcode.com/archives/FIXME_TODO_Notes_As_Warnings_In_Compiler_Output.shtml

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// THE IMPORANT BITS
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
        " --------------------------------------------------------------------------------------\n" \
        "|  TODO :   " ##x "\n" \
        " --------------------------------------------------------------------------------------\n" )

#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" ); ASSERT_RECOVERABLE(false, "");

#pragma endregion


typedef std::vector<std::string> Strings;
typedef std::vector<int> Ints;
typedef unsigned int uint;

extern Blackboard g_gameConfigBlackboard; // declared in EngineCommon.hpp, defined in .cpp