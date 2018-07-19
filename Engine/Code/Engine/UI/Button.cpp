#include "Engine/UI/Button.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

Button::Button(Canvas* canvas)
	: CanvasElement(canvas)
{
	ChangeButtonState(BUTTON_STATE_IDLE);
}

void Button::Render()
{
	Renderer* r = Renderer::GetInstance();

	if (m_texture != nullptr)
	{
		r->DrawTexturedAABB(m_screenBounds, *m_texture, Vector2::one, Vector2::zero, m_currentTint);
	}
	else
	{
		r->DrawAABB(m_screenBounds, m_currentTint);
	}
}

bool Button::MouseDownInteract(int screenX, int screenY)
{
	if (m_buttonState == BUTTON_STATE_DISABLE)
		return false;

	bool result = m_screenBounds.IsPointInside(screenX, screenY);

	if (result)
		ChangeButtonState(BUTTON_STATE_PRESSED);

	return result;
}

bool Button::MouseUpInteract(int screenX, int screenY)
{
	if (m_buttonState == BUTTON_STATE_DISABLE)
		return false;

	ChangeButtonState(BUTTON_STATE_IDLE);

	return m_screenBounds.IsPointInside(screenX, screenY);
}

void Button::ChangeButtonState(BUTTON_STATE newState)
{
	m_buttonState = newState;
	
	switch(m_buttonState)
	{
	case BUTTON_STATE_IDLE:
		m_currentTint = m_normalTint;
		break;
	case BUTTON_STATE_PRESSED:
		m_currentTint = m_pressedTint;
		break;
	case BUTTON_STATE_DISABLE:
		m_currentTint = m_disableTint;
		break;
	}
}

void Button::OnClicked()
{
	for each (click_cb_string var in m_onClickCB_string)
	{
		var(m_stringValue);
	}

	for each (click_cb_int var in m_onClickCB_int)
	{
		var(m_intValue);
	}
}

void Button::RegisterOnClicked(click_cb_string cb)
{
	m_onClickCB_string.push_back(cb);
}

void Button::RemoveOnClicked(click_cb_string cb)
{
	std::vector<click_cb_string>::iterator search;
	search = std::find(m_onClickCB_string.begin(), m_onClickCB_string.end(), cb);
	if (search != m_onClickCB_string.end())
	{
		m_onClickCB_string.erase(search);
	}
}

void Button::RegisterOnClicked(click_cb_int cb)
{
	m_onClickCB_int.push_back(cb);
}

void Button::RemoveOnClicked(click_cb_int cb)
{
	std::vector<click_cb_int>::iterator search;
	search = std::find(m_onClickCB_int.begin(), m_onClickCB_int.end(), cb);
	if (search != m_onClickCB_int.end())
	{
		m_onClickCB_int.erase(search);
	}
}
