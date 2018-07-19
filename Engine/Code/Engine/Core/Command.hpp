#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>

// A command is a single submitted command...NOT the definition (hidden internally)
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" ); 
class Command
{
public:
	Command(const char* str); 
	~Command(); 

	std::string GetName(); // would return "echo_with_color"

	// Gets the next string in the argument list.
	// Breaks on whitespace.  Quoted strings should be treated as a single return 
	std::string GetNextString();

	bool GetNextColor(Rgba* out_val);
	bool GetNextInt(int* out_val);
	bool GetNextVector2(Vector2* out_val);

private:
	void ParseToStrings(const char* str);

public:
	std::string m_rawText = ""; //for history

private:
	Strings m_strings;
	std::string m_name;
	int m_stringsIndex = 0;

};

// Command callbacks take a Command.
typedef void (*command_cb) (Command& cmd); 

// Allows for setting up the system and cleaning up.
// Optional, but may help with some tasks. 
void CommandStartup();
void CommandShutdown(); 

// Registers a command with the system
// Example, say we had a global function named...
//    void Help( Command &cmd ) { /* ... */ }  
// 
// We then, during some startup, call
//    CommandRegister( "help", Help ); 
void CommandRegister(const char* name, command_cb cb, std::string helpText = ""); 

// Will construct a Command object locally, and if 
// a callback is associated with its name, will call it and 
// return true, otherwise returns false.
// Name is case-insensitive
bool CommandRun(const char* command); 

// Get all the registered commands to display in text
Strings RegisteredCommandsToStrings();

// Returns a list containing the last entered
// commands ordered by how recently they were used.
// If a command is entered twice, it should not appear
// twice, but instead just just move to the front. 
// History length is up to you, but 32 or 64 is good.
std::vector<std::string> GetCommandHistory(); 
void AddToHistory(std::string data);

// Runs a "script", or multiple lines of commands (each line
// is treated as a command). 
// Leading whitespace is ignored, and 
// lines starting with '#' should be ignored. 
void CommandRunScript(const char* script); 
void CommandRunScriptFile(const char* filename); 
