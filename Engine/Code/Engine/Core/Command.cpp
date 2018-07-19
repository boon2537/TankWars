#include "Engine/Core/Command.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/File.hpp"
#include <map>

class CommandDefinition
{
public:
	CommandDefinition(std::string name, command_cb cb, std::string helpText = "")
		: m_name(name), m_callBack(cb), m_helpText(helpText) {}

	std::string ToString()
	{
		return m_name + ": " + m_helpText;
	}

	std::string m_name;
	command_cb m_callBack;
	std::string m_helpText;
};

constexpr int MAX_HISTORY = 32;
static std::map<std::string, CommandDefinition*> g_Definitions;
static std::vector<std::string> commandHistory;

Command::Command(const char* str)
{
	m_rawText = std::string(str);
	ParseToStrings(str);
	m_name = m_strings[0];
}

Command::~Command()
{
}

std::string Command::GetName()
{
	return m_name;
}

std::string Command::GetNextString()
{
	m_stringsIndex++;
	if (m_stringsIndex < m_strings.size())
		return m_strings[m_stringsIndex];

	return "";
}

bool Command::GetNextColor(Rgba* out_val)
{
	m_stringsIndex++;
	if (m_stringsIndex < m_strings.size())
	{
		out_val->SetFromText(m_strings[m_stringsIndex].c_str());
		return true;
	}

	return false;
}

bool Command::GetNextInt(int* out_val)
{
	m_stringsIndex++;
	if (m_stringsIndex < m_strings.size())
	{
		*out_val = atoi(m_strings[m_stringsIndex].c_str());
		return true;
	}

	return false;
}

bool Command::GetNextVector2(Vector2* out_val)
{
	m_stringsIndex++;
	if (m_stringsIndex < m_strings.size())
	{
		out_val->SetFromText(m_strings[m_stringsIndex].c_str());
		return true;
	}

	return false;
}

void Command::ParseToStrings(const char* str)
{
	std::string text = std::string(str);

	//first element is always a command
	size_t start = 0;
	size_t end = text.find(' ', start);

	while (end != -1)
	{
		if (end - start > 0)
		{
			m_strings.push_back(text.substr(start, end - start));
		}
		start = end + 1;

		if (start >= text.size())
		{
			break;
		}

		if (text[start] == '(')
		{
			end = text.find(')', start + 1);
			start++;
		}
		else if (text[start] == '\"')
		{
			end = text.find("\" ", start + 1);
			if (end == -1)
			{
				end = text.size() - 1;
			}
			start++;
		}
		else
		{
			end = text.find(' ', start);
		}
	}

	m_strings.push_back(text.substr(start));
}

void CommandStartup()
{
}

void CommandShutdown()
{
}

void CommandRegister(const char* name, command_cb cb, std::string helpText)
{
	CommandDefinition* def = new CommandDefinition(name, cb, helpText); 
	g_Definitions[name] = def;
}

bool CommandRun(const char* command)
{
	Command constructedCommand = Command(command);
	std::map<std::string, CommandDefinition*>::iterator search = g_Definitions.find(constructedCommand.GetName());
	if (search != g_Definitions.end())
	{
		search->second->m_callBack(constructedCommand);
		AddToHistory(constructedCommand.m_rawText);
		return true;
	}

	std::string errorMessage = "Invalid Command: '" + constructedCommand.GetName() + "'";
	ConsoleErrorf(errorMessage.c_str());
	return false;
}

Strings RegisteredCommandsToStrings()
{
	Strings output;
	for (std::map<std::string, CommandDefinition*>::iterator it = g_Definitions.begin(); it != g_Definitions.end(); ++it)
	{
		output.push_back(it->second->ToString());
	}

	return output;
}

std::vector<std::string> GetCommandHistory()
{
	return commandHistory;
}

void AddToHistory(std::string data)
{
	if (commandHistory.size() == MAX_HISTORY)
	{
		commandHistory.erase(commandHistory.begin());
	}

	commandHistory.push_back(data);
}

void CommandRunScript(const char* script)
{
	Strings commands = Split(std::string(script), '\n');
	
	for (unsigned int i = 0; i < commands.size(); i++)
	{
		if (commands[i].length() > 0)
		{
			CommandRun(commands[i].c_str());
		}
	}
}

void CommandRunScriptFile(const char* filename)
{
	char* buffer = (char*) FileReadToNewBuffer(filename);
	ASSERT_RECOVERABLE(buffer != nullptr, "reading file failed");

	CommandRunScript(buffer);
}
