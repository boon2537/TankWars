#pragma once

#include "Engine/UI/CanvasElement.hpp"
#include "Engine/Core/Rgba.hpp"
#include <string>
#include <vector>

class Texture;

enum BUTTON_STATE
{
	BUTTON_STATE_IDLE,
	BUTTON_STATE_PRESSED,
	BUTTON_STATE_DISABLE
};

typedef void (*click_cb_string) (std::string& value);
typedef void (*click_cb_int) (int value);

class Button : public CanvasElement
{
public:
	~Button() {};
	Button(Canvas* canvas);

	virtual void Render() override;
	virtual bool MouseDownInteract(int screenX, int screenY) override;
	virtual bool MouseUpInteract(int screenX, int screenY) override;
	virtual void OnClicked() override;

	void ChangeButtonState(BUTTON_STATE newState);
	
	void RegisterOnClicked(click_cb_string cb);
	void RemoveOnClicked(click_cb_string cb);
	void RegisterOnClicked(click_cb_int cb);
	void RemoveOnClicked(click_cb_int cb);

public:
	Texture* m_texture;
	std::string m_stringValue = "";
	int m_intValue = 0;

	bool m_isDisable = false;
	BUTTON_STATE m_buttonState = BUTTON_STATE::BUTTON_STATE_IDLE;
	Rgba m_currentTint;

	Rgba m_normalTint = Rgba::white;
	Rgba m_pressedTint = Rgba(150, 150, 150, 255);
	Rgba m_disableTint = Rgba(150, 150, 150, 150);

	std::vector<click_cb_string> m_onClickCB_string;
	std::vector<click_cb_int> m_onClickCB_int;
};
