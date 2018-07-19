#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Command.hpp"
#include <string>
#include <vector>

class BitmapFont;
class Renderer;
class Camera;

struct ConsoleTextData
{
	ConsoleTextData(std::string text, Rgba color)
		: m_text(text), m_color(color) {}

	std::string m_text;
	Rgba m_color;
};

// Will be a singleton
class DevConsole 
{
public:
	DevConsole(); 
	~DevConsole(); 

	void StartUp();

	// Handles all input
	void Update();

	// Renders the display
	void Render(Renderer* renderer); 
	void SetCurrentFont(BitmapFont* font);

	void Open(); 
	void Close(); 
	bool IsOpen(); 

	void ProcessInput();
	void HandleCharInsert(unsigned char c);
	void MoveCursorRight();
	void MoveCursorLeft();
	void DeleteCharacterRight();
	void DeleteCharacterLeft();
	void ClearText();
	void IssueCommand();
	void PushToConsoleTexts(const ConsoleTextData& data);

	void HandleScolling(short delta);

	void EnterHistoryMode();
	void ExitHistoryMode();
	void MoveHistoryUp();
	void MoveHistoryDown();

	void SetAbsoluteCursorIndex(int index);
	bool IsSelectingText();
	void RemoveSelectedText();

private: 
	bool m_isOpen = false;
	bool m_willCloseInUpdate = false; //a hack so that we close after update call, help apps know when devconsole is just closed
	std::string m_currentText = "";
	std::vector<ConsoleTextData> m_consoleTexts;
	int m_cursorIndex = 0;
	int m_selectionIndex = 0;
	Vector2 m_dimensions;
	float m_lastTime = 0;
	float m_nextCursorTime = 0;
	bool m_isCursorBlinking = false;
	BitmapFont* m_currentFont = nullptr;

	int m_scrollDelta = 0;
	int m_consoleTextOffset = 0;
	bool m_isConsoleTextOverflow = false;
	float m_nextScrollTime = 0;
	float m_nextAutoScrollTime = 0;
	int m_lastVisibleConsoleTextIndex = 0;

	bool m_isInHistoryMode =  false;
	std::vector<std::string> m_currentHistory;
	size_t m_indexInHistory;

	Camera* m_uiCamera = nullptr;

public:
	static DevConsole* CreateInstance();
	static DevConsole* GetInstance(); 

	static void PushHelpCommands(Command& cmd);
	static void ClearConsole(Command& cmd);
	static void EchoWithColorCommand(Command& cmd);
	static void SaveToFile(Command& cmd);

public:
	static KEY_CODE ACTIVATE_KEY;
};


// Global functions

// I like this as a C function or a static 
// method so that it is easy for systems to check if the DevConsole is open.  
bool DevConsoleIsOpen(); 

// Should add a line of colored text to the output 
void ConsolePrintf(const Rgba& color, const char* format, ...); 

// Same as previous, be defaults to a color visible easily on your console
void ConsolePrintf(const char* format, ...);

void ConsoleErrorf(const char* format, ...);

void ConsoleWarningf(const char* format, ...);
