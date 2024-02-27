#include "input.hh"

std::map<int, bool> Input::keyPress;
std::map<int, bool> Input::keyPressRepeat;
std::map<int, bool> Input::keyHeld;
std::map<int, bool> Input::keyRelease;

bool Input::leftMouseButtonState = false;
bool Input::rightMouseButtonState = false;
bool Input::middleMouseButtonState = false;

MousePos Input::mousePos = MousePos {0,0,0,0};

bool Input::KeyboardCheck(int key)
{
	if (keyHeld.find(key) == keyHeld.end())
	{
		// key not held
		return false;
	}
	else
	{
		// key is held
		return true;
	}
}

bool Input::KeyboardCheckPressed(int key)
{
	if (keyPress.find(key) == keyPress.end())
	{
		// key not held
		return false;
	}
	else
	{
		// key is held
		return true;
	}
}

bool Input::KeyboardCheckPressedRepeat(int key)
{
	if (keyPressRepeat.find(key) == keyPressRepeat.end())
	{
		// key not held
		return false;
	}
	else
	{
		// key is held
		return true;
	}
}

bool Input::KeyboardCheckReleased(int key)
{
	if (keyRelease.find(key) == keyRelease.end())
	{
		// key not held
		return false;
	}
	else
	{
		// key is held
		return true;
	}
}

MousePos Input::GetMouseMovement()
{
	return mousePos;
}

bool Input::GetMouseButton(MouseButton button)
{
	switch (button)
	{
	case MouseButton::left:
		return leftMouseButtonState;
		break;
	case MouseButton::middle:
		return middleMouseButtonState;
		break;
	case MouseButton::right:
		return rightMouseButtonState;
		break;
	}
	return false;
}

void Input::SetKeyDown(int key)
{
	keyPress[key] = true;
	keyPressRepeat[key] = true;
	keyHeld[key] = true;
	//Logger::Log("Keydown");
}

void Input::SetKeyDownRepeat(int key)
{
	keyPressRepeat[key] = true;
	keyHeld[key] = true;
	//Logger::Log("Keydown Repeat");
}

void Input::SetKeyUp(int key)
{
	keyRelease[key] = true;
	keyHeld.erase(key);
	//Logger::Log("Keyup");
}

void Input::SetMouseMovement(MousePos pos)
{
	mousePos = pos;
}

void Input::SetMouseButton(MouseButton button, bool state)
{
	switch (button)
	{
	case MouseButton::left:
		leftMouseButtonState = state;
		break;
	case MouseButton::middle:
		middleMouseButtonState = state;
		break;
	case MouseButton::right:
		rightMouseButtonState = state;
		break;
	}
}

void Input::Update()
{
	keyPress.clear();
	keyPressRepeat.clear();
	keyRelease.clear();
	mousePos.relativeX = 0;
	mousePos.relativeY = 0;
}
