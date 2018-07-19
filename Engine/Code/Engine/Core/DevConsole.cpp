#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/File/File.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include <stdarg.h>

static DevConsole* g_DevConsole = nullptr; 
static InputSystem* g_ConsoleInput = nullptr;
static Rgba DEFAULT_COLOR = Rgba(255, 255, 255);
static Rgba ERROR_COLOR = Rgba(255, 0, 0);
static Rgba WARNING_COLOR = Rgba::yellow;
constexpr float BASE_WIDTH = 1280;
constexpr float BASE_HEIGHT = 720;
constexpr float CELL_HEIGHT = 15;
constexpr float CURSOR_SHOW_DURATION = 0.8f;
constexpr float CURSOR_BLINK_DURATION = 0.5f;
constexpr float CURSOR_WIDTH = 2;
constexpr float TEXT_OFFSET_X = 10.f;
constexpr float CONSOLE_TEXTS_DISTANCE = 2.f;
constexpr float AUTO_SCROLL_DELAY = 1.5f;
constexpr float SCROLL_DELAY = 0.05f;

bool ConsoleMessageHandler(unsigned int wmMessageCode, size_t wParam, size_t lParam)
{
	UNUSED(lParam);

	if (g_DevConsole->IsOpen())
	{
		unsigned char keyCode = (unsigned char) wParam; 
		switch (wmMessageCode) 
		{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			break;

		case WM_KEYDOWN:
			g_ConsoleInput->OnKeyPressed(keyCode);
			break;
		case WM_KEYUP:
			g_ConsoleInput->OnKeyReleased(keyCode);
			break;
		case WM_CHAR:
			g_DevConsole->HandleCharInsert(keyCode);
			break;
		case WM_MOUSEWHEEL:
			g_DevConsole->HandleScolling(GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		};
	}

	return true;
}

KEY_CODE DevConsole::ACTIVATE_KEY = KEY_CODE::BACK_QUOTE;

DevConsole::DevConsole()
{
	Window* window = Window::GetInstance();
	if (window != nullptr)
	{
		window->RegisterHandler(ConsoleMessageHandler);
	}
	else
	{
		ASSERT_RECOVERABLE(false, "Window MUST be created before DevConsole");
	}

	//TODO: query dimensions from camera later
	m_dimensions = Vector2(BASE_WIDTH, BASE_HEIGHT);

	CommandRegister("help", DevConsole::PushHelpCommands, "Prints all registered commands to console.");
	CommandRegister("clear", DevConsole::ClearConsole, "Clears the developer console.");
	CommandRegister("echo_with_color", DevConsole::EchoWithColorCommand, "Prints the supplied text with the supplied color to the console.");
	CommandRegister("save_log", DevConsole::SaveToFile, "Saves a copy of the current console output to the file.");
}

DevConsole::~DevConsole()
{
	delete g_ConsoleInput;
}

void DevConsole::StartUp()
{
	m_uiCamera = new Camera();
	m_uiCamera->SetProjection(Matrix44::MakeOrtho(Vector2(0, 0), Vector2(m_dimensions.x, m_dimensions.y), 0, 1));

	Renderer* r = Renderer::GetInstance();
	m_uiCamera->SetColorTarget(r->GetDefaultColorTarget());
	m_uiCamera->SetDepthStencilTarget(r->GetDefaultDepthTarget());
}

void DevConsole::Update()
{
	ProfilerPush(__FUNCTION__);

	float currentTime = static_cast<float>(GetCurrentTimeSeconds());	
	//float deltaSeconds = currentTime - m_lastTime;
	m_lastTime = currentTime;

	if (g_ConsoleInput->WasKeyJustPressed(DevConsole::ACTIVATE_KEY) || m_willCloseInUpdate)
	{
		Close();
		m_willCloseInUpdate = false;
	}

	ProcessInput();

	//cursor show/blink
	if (m_nextCursorTime <= currentTime)
	{
		if (m_isCursorBlinking)
		{
			m_isCursorBlinking = false;
			m_nextCursorTime = currentTime + CURSOR_SHOW_DURATION;
		}
		else
		{
			m_isCursorBlinking = true;
			m_nextCursorTime = currentTime + CURSOR_BLINK_DURATION;
		}
	}

	//scroll up
	if (m_nextScrollTime <= currentTime && m_isConsoleTextOverflow && m_scrollDelta > 0)
	{
		m_consoleTextOffset = MinInt(m_consoleTextOffset + 1, (int) m_consoleTexts.size() - 1);
		m_nextScrollTime = currentTime + SCROLL_DELAY;
		m_nextAutoScrollTime = currentTime + AUTO_SCROLL_DELAY;
	}
	//scroll down
	if (m_consoleTextOffset > 0 && (m_scrollDelta < 0 || m_nextAutoScrollTime <= currentTime) && m_nextScrollTime <= currentTime )
	{
		if (m_scrollDelta < 0)
		{
			m_nextAutoScrollTime = currentTime + AUTO_SCROLL_DELAY;
		}

		m_consoleTextOffset = MaxInt(m_consoleTextOffset - 1, 0);
		m_nextScrollTime = currentTime + SCROLL_DELAY;
	}

	m_scrollDelta = 0;

	g_ConsoleInput->UpdateKeyboard();

	ProfilerPop();
}

void DevConsole::Render(Renderer* renderer)
{
	ProfilerPush(__FUNCTION__);

	renderer->SetCamera(m_uiCamera);
	renderer->SetShader(nullptr);
	renderer->DisableDepth();

	renderer->DrawAABB(AABB2(0, 0, m_dimensions.x, m_dimensions.y), Rgba(60, 60, 60, 150));
	renderer->DrawAABB(AABB2(0, 0, m_dimensions.x, CELL_HEIGHT), Rgba(30, 30, 30, 200));
	renderer->DrawTextInBox2D(AABB2(TEXT_OFFSET_X, 0, m_dimensions.x, CELL_HEIGHT), m_currentText, CELL_HEIGHT, Rgba::white, 1, m_currentFont);

	//draw cursor
	if (!m_isCursorBlinking)
	{
		float startX = (CELL_HEIGHT * m_cursorIndex) + TEXT_OFFSET_X; //assume the aspect is 1, so height = width
		renderer->DrawAABB(AABB2(startX, 0, startX + CURSOR_WIDTH, CELL_HEIGHT), Rgba::white);
	}

	//draw text selection
	if (IsSelectingText())
	{
		float start = TEXT_OFFSET_X + (CELL_HEIGHT * MinInt(m_cursorIndex, m_selectionIndex));
		float end = TEXT_OFFSET_X + (CELL_HEIGHT * MaxInt(m_cursorIndex, m_selectionIndex));
		renderer->DrawAABB(AABB2(start, 0, end, CELL_HEIGHT), Rgba(180, 180, 0, 100));
	}

	//draw console history
	m_isConsoleTextOverflow = false;
	float startY = CELL_HEIGHT + CONSOLE_TEXTS_DISTANCE;
	for (unsigned int i = m_consoleTextOffset; i < m_consoleTexts.size(); i++)
	{
		float stepHeight = (i - m_consoleTextOffset) * (CELL_HEIGHT + CONSOLE_TEXTS_DISTANCE);

		// so we don't overdraw
		if (stepHeight + startY + CELL_HEIGHT > m_dimensions.y)
		{
			m_isConsoleTextOverflow = true;
			break;
		}

		renderer->DrawTextInBox2D(AABB2(TEXT_OFFSET_X, startY + stepHeight, m_dimensions.x, startY + stepHeight + CELL_HEIGHT), m_consoleTexts[i].m_text, CELL_HEIGHT, m_consoleTexts[i].m_color, 1, m_currentFont);
		m_lastVisibleConsoleTextIndex = i;
	}

	//draw scroll visual
	renderer->DrawAABB(AABB2(m_dimensions.x - 15, CELL_HEIGHT, m_dimensions.x, m_dimensions.y), Rgba(60, 60, 60, 200));
	float start = CELL_HEIGHT + (((float) m_consoleTextOffset / (m_consoleTexts.size() - 1)) * m_dimensions.y);
	float end = ((float) m_lastVisibleConsoleTextIndex / (m_consoleTexts.size() - 1)) * m_dimensions.y;
	renderer->DrawAABB(AABB2(m_dimensions.x - 15, start, m_dimensions.x, end), Rgba(120, 120, 120, 200));

	ProfilerPop();
}

void DevConsole::SetCurrentFont(BitmapFont* font)
{
	m_currentFont = font;
}

void DevConsole::Open()
{
	m_isOpen = true;
	g_ConsoleInput->UpdateKeyboard();
}

void DevConsole::Close()
{
	m_isOpen = false;
	ClearText();
}

bool DevConsole::IsOpen()
{
	return m_isOpen;
}

void DevConsole::ProcessInput()
{
	if (!m_isInHistoryMode && g_ConsoleInput->WasKeyJustPressed(KEY_CODE::UP_ARROW))
	{
		EnterHistoryMode();
	}
	else if (m_isInHistoryMode)
	{
		if (g_ConsoleInput->WasKeyJustPressed(KEY_CODE::UP_ARROW))
			MoveHistoryUp();
		else if (g_ConsoleInput->WasKeyJustPressed(KEY_CODE::DOWN_ARROW))
			MoveHistoryDown();
	}

	if (g_ConsoleInput->WasKeyJustPressed(KEY_CODE::LEFT_ARROW))
	{
		MoveCursorLeft();
	}
	if (g_ConsoleInput->WasKeyJustPressed(KEY_CODE::RIGHT_ARROW))
	{
		MoveCursorRight();
	}
	if (g_ConsoleInput->WasKeyJustPressed(KEY_CODE::DEL))
	{
		DeleteCharacterRight();
	}
}

void DevConsole::HandleCharInsert(unsigned char c)
{
	if (c == (unsigned char) KEY_CODE::KEYPAD_ENTER || c == (unsigned char) KEY_CODE::RETURN)
	{
		IssueCommand();
	}
	else if (c == (unsigned char) KEY_CODE::ESC)
	{
		//cancel selection..high priority
		if (IsSelectingText())
		{
			m_selectionIndex = m_cursorIndex;
		}
		else if (m_currentText.length() > 0)
		{
			ClearText();
		}
		else
		{
			m_willCloseInUpdate = true;
		}
	}
	else if (c == (unsigned char) KEY_CODE::BACKSPACE)
	{
		DeleteCharacterLeft();
	}
	else
	{
		RemoveSelectedText();
		m_currentText.insert(m_cursorIndex, std::string(1, c));
		SetAbsoluteCursorIndex(m_cursorIndex + 1);
	}
}

void DevConsole::MoveCursorRight()
{
	if (g_ConsoleInput->IsShiftDown())
	{
		m_selectionIndex = MinInt(m_selectionIndex + 1, (int) m_currentText.length());
	}
	else
	{
		SetAbsoluteCursorIndex(MinInt(m_cursorIndex + 1, (int) m_currentText.length()));
	}
}

void DevConsole::MoveCursorLeft()
{
	if (g_ConsoleInput->IsShiftDown())
	{
		m_selectionIndex = MaxInt(m_selectionIndex - 1, 0);
	}
	else
	{
		SetAbsoluteCursorIndex(MaxInt(m_cursorIndex - 1, 0));
	}
}

void DevConsole::DeleteCharacterRight()
{
	if (IsSelectingText())
	{
		RemoveSelectedText();
	}
	else if (m_cursorIndex < m_currentText.length())
	{
		m_currentText.erase(m_cursorIndex, 1);
	}
}

void DevConsole::DeleteCharacterLeft()
{
	if (IsSelectingText())
	{
		RemoveSelectedText();
	}
	else if (m_cursorIndex > 0)
	{
		m_currentText.erase(m_cursorIndex - 1, 1);
		SetAbsoluteCursorIndex(m_cursorIndex - 1);
	}
}

void DevConsole::ClearText()
{
	m_currentText = "";
	SetAbsoluteCursorIndex(0);

	ExitHistoryMode();
}

void DevConsole::IssueCommand()
{
	if (m_currentText.length() > 0)
	{
		CommandRun(m_currentText.c_str());
	}

	ClearText();
}

void DevConsole::PushToConsoleTexts(const ConsoleTextData& data)
{
	Strings splited = Split(data.m_text, '\n');

	for each (std::string s in splited)
	{
		ConsoleTextData newData = ConsoleTextData(s, data.m_color);

		m_consoleTexts.insert(m_consoleTexts.begin(), newData);
		//the user should not have where he is looking change when scrolling
		if (m_consoleTextOffset > 0)
		{
			m_consoleTextOffset++;
			m_nextAutoScrollTime = m_lastTime + AUTO_SCROLL_DELAY;
		}
	}
}

void DevConsole::HandleScolling(short delta)
{
	m_scrollDelta = delta;
}

void DevConsole::EnterHistoryMode()
{
	m_isInHistoryMode = true;
	m_currentHistory = GetCommandHistory();
	m_indexInHistory =  m_currentHistory.size() - 1;

	m_currentText = m_currentHistory[m_indexInHistory];
	SetAbsoluteCursorIndex((int) m_currentText.size());
}

void DevConsole::ExitHistoryMode()
{
	m_isInHistoryMode = false;
}

void DevConsole::MoveHistoryUp()
{
	if (m_indexInHistory == 0)
		m_indexInHistory = m_currentHistory.size() - 1;
	else
		m_indexInHistory--;

	m_currentText = m_currentHistory[m_indexInHistory];
	SetAbsoluteCursorIndex((int) m_currentText.size());
}

void DevConsole::MoveHistoryDown()
{
	if (m_indexInHistory == m_currentHistory.size() - 1)
		m_indexInHistory = 0;
	else
		m_indexInHistory++;

	m_currentText = m_currentHistory[m_indexInHistory];
	SetAbsoluteCursorIndex((int) m_currentText.size());
}

void DevConsole::SetAbsoluteCursorIndex(int index)
{
	m_cursorIndex = index;
	m_selectionIndex = m_cursorIndex;
}

bool DevConsole::IsSelectingText()
{
	return m_cursorIndex != m_selectionIndex;
}

void DevConsole::RemoveSelectedText()
{
	if (!IsSelectingText())
		return;

	if (m_selectionIndex > m_cursorIndex)
		m_currentText.erase(m_cursorIndex, m_selectionIndex - m_cursorIndex);
	else
		m_currentText.erase(m_selectionIndex, m_cursorIndex - m_selectionIndex);

	SetAbsoluteCursorIndex(MinInt(m_cursorIndex, m_selectionIndex));
}

DevConsole* DevConsole::CreateInstance()
{
	if (g_DevConsole == nullptr) 
	{
		g_DevConsole = new DevConsole(); 
		g_ConsoleInput = new InputSystem();
	}
	return g_DevConsole; 
}

DevConsole* DevConsole::GetInstance()
{
	return g_DevConsole; 
}

void DevConsole::PushHelpCommands(Command& cmd)
{
	UNUSED(cmd);

	Strings commands = RegisteredCommandsToStrings();
	for (unsigned int i = 0; i < commands.size(); i++)
	{
		g_DevConsole->PushToConsoleTexts(ConsoleTextData(commands[i], DEFAULT_COLOR));
	}
}

void DevConsole::ClearConsole(Command& cmd)
{
	UNUSED(cmd);

	g_DevConsole->m_consoleTexts.clear();
	g_DevConsole->m_consoleTextOffset = 0;
}

void DevConsole::EchoWithColorCommand(Command& cmd)
{
	Rgba color;
	if (cmd.GetNextColor(&color))
	{
		std::string text = cmd.GetNextString();
		if (text == "")
		{
			ConsoleErrorf("Need string as second argument");
			return;
		}

		ConsolePrintf(color, text.c_str());
	}
	else
	{
		ConsoleErrorf("Need Rgba as first argument");
	}
}

void DevConsole::SaveToFile(Command & cmd)
{
	std::string name = cmd.GetNextString();
	if (name == "")
	{
		ConsoleErrorf("Need to enter file name");
		return;
	}

	DevConsole* console = DevConsole::GetInstance();
	std::string data;
	size_t totalLines = console->m_consoleTexts.size();
	for (size_t i = totalLines - 1; i > 0; i--)
	{
		data = data + console->m_consoleTexts[i].m_text + "\n";
	}
	data.erase(data.size() - 1, 1); //delete trailing "\n"

	if (WriteStringToFile(name.c_str(), data))
	{
		ConsolePrintf("save_log success!");
	}
	else
	{
		ConsoleErrorf("save_log failed");
	}
}

bool DevConsoleIsOpen()
{
	if (g_DevConsole == nullptr)
		return false;

	return g_DevConsole->IsOpen();
}

void ConsolePrintf(const Rgba& color, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	DevConsole::GetInstance()->PushToConsoleTexts(ConsoleTextData(Stringv(format, args), color));
	va_end(args);
}

void ConsolePrintf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	DevConsole::GetInstance()->PushToConsoleTexts(ConsoleTextData(Stringv(format, args), DEFAULT_COLOR));
	va_end(args);
}

void ConsoleErrorf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	DevConsole::GetInstance()->PushToConsoleTexts(ConsoleTextData(Stringv(format, args), ERROR_COLOR));
	va_end(args);
}

void ConsoleWarningf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	DevConsole::GetInstance()->PushToConsoleTexts(ConsoleTextData(Stringv(format, args), WARNING_COLOR));
	va_end(args);
}
