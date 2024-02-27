#pragma once

#include <map>
#include "core.hh"

enum MouseButton
{
	left = 1,
	middle,
	right
};

struct MousePos
{
	i32 x;
	i32 y;
	i32 relativeX;
	i32 relativeY;
};

class Input
{
public:
	static bool KeyboardCheck(int key);
	static bool KeyboardCheckPressed(int key);
	static bool KeyboardCheckPressedRepeat(int key);
	static bool KeyboardCheckReleased(int key);
	static MousePos GetMouseMovement();
	static bool GetMouseButton(MouseButton button);

	static void SetKeyDown(int key);
	static void SetKeyDownRepeat(int key);
	static void SetKeyUp(int key);
	static void SetMouseMovement(MousePos);
	static void SetMouseButton(MouseButton button, bool state);

	static void Update();
private:
	static std::map<int, bool> keyPress;
	static std::map<int, bool> keyPressRepeat;
	static std::map<int, bool> keyHeld;
	static std::map<int, bool> keyRelease;

	static bool leftMouseButtonState;
	static bool rightMouseButtonState;
	static bool middleMouseButtonState;

	static MousePos mousePos;
};
