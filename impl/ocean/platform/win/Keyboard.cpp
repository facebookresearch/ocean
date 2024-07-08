/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Keyboard.h"

#include <winsock2.h>
#include <windows.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

bool Keyboard::translateVirtualkey(const unsigned int vKey, std::string& key)
{

#ifndef MAPVK_VK_TO_VSC
	#define MAPVK_VK_TO_VSC 0
#endif

#ifndef MAPVK_VSC_TO_VK
	#define MAPVK_VSC_TO_VK 1
#endif

#ifndef MAPVK_VK_TO_CHAR
	#define MAPVK_VK_TO_CHAR 2
#endif

#ifndef MAPVK_VSC_TO_VK_EX
	#define MAPVK_VSC_TO_VK_EX 3
#endif

	UINT cKey = MapVirtualKeyEx(UINT(vKey), MAPVK_VK_TO_CHAR, GetKeyboardLayout(0));

	if (cKey)
	{
		key = std::string(1, char(cKey & 0xFF));
		return true;
	}

	switch (vKey)
	{
		case VK_LBUTTON:
			key = "left mouse button";
			break;

		case VK_RBUTTON:
			key = "right mouse button";
			break;

		case VK_CANCEL:
			key = "cancel";
			break;

		case VK_MBUTTON:
			key = "middle mouse button";
			break;

		case VK_XBUTTON1:
			key = "x1 mouse button";
			break;

		case VK_XBUTTON2:
			key = "x2 mouse button";
			break;

		case VK_BACK:
			key = "backspace";
			break;

		case VK_TAB:
			key = "tab";
			break;

		case VK_CLEAR:
			key = "clear";
			break;

		case VK_RETURN:
			key = "return";
			break;

		case VK_SHIFT:
			key = "shift";
			break;

		case VK_CONTROL:
			key = "control";
			break;

		case VK_MENU:
			key = "alt";
			break;

		case VK_PAUSE:
			key = "pause";
			break;

		case VK_CAPITAL:
			key = "caps lock";
			break;

		case VK_HANGUL:
			key = "hangul";
			break;

		case VK_JUNJA:
			key = "junja";
			break;

		case VK_FINAL:
			key = "final";
			break;

		case VK_KANJI:
			key = "kanji";
			break;

		case VK_ESCAPE:
			key = "escape";
			break;

		case VK_CONVERT:
			key = "convert";
			break;

		case VK_NONCONVERT:
			key = "nonconvert";
			break;

		case VK_ACCEPT:
			key = "accept";
			break;

		case VK_MODECHANGE:
			key = "mode change";
			break;

		case VK_SPACE:
			key = "space";
			break;

		case VK_PRIOR:
			key = "page up";
			break;

		case VK_NEXT:
			key = "page down";
			break;

		case VK_END:
			key = "end";
			break;

		case VK_HOME:
			key = "home";
			break;

		case VK_LEFT:
			key = "left";
			break;

		case VK_UP:
			key = "up";
			break;

		case VK_RIGHT:
			key = "right";
			break;

		case VK_DOWN:
			key = "down";
			break;

		case VK_SELECT:
			key = "select";
			break;

		case VK_PRINT:
			key = "pring";
			break;

		case VK_EXECUTE:
			key = "execute";
			break;

		case VK_SNAPSHOT:
			key = "snapsho";
			break;

		case VK_INSERT:
			key = "insert";
			break;

		case VK_DELETE:
			key = "delete";
			break;

		case VK_HELP:
			key = "help";
			break;

		case VK_LWIN:
			key = "left windows key";
			break;

		case VK_RWIN:
			key = "right windows key";
			break;

		case VK_APPS:
			key = "application";
			break;

		case VK_SLEEP:
			key = "sleep";
			break;

		case VK_NUMPAD0:
			key = "keypad0";
			break;

		case VK_NUMPAD1:
			key = "keypad1";
			break;

		case VK_NUMPAD2:
			key = "keypad2";
			break;

		case VK_NUMPAD3:
			key = "keypad3";
			break;

		case VK_NUMPAD4:
			key = "keypad4";
			break;

		case VK_NUMPAD5:
			key = "keypad5";
			break;

		case VK_NUMPAD6:
			key = "keypad6";
			break;

		case VK_NUMPAD7:
			key = "keypad7";
			break;

		case VK_NUMPAD8:
			key = "keypad8";
			break;

		case VK_NUMPAD9:
			key = "keypad9";
			break;

		case VK_MULTIPLY:
			key = "multiply";
			break;

		case VK_ADD:
			key = "add";
			break;

		case VK_SEPARATOR:
			key = "separator";
			break;

		case VK_SUBTRACT:
			key = "subtract";
			break;

		case VK_DECIMAL:
			key = "decimal";
			break;

		case VK_DIVIDE:
			key = "divide";
			break;

		case VK_F1:
			key = "f1";
			break;

		case VK_F2:
			key = "f2";
			break;

		case VK_F3:
			key = "f3";
			break;

		case VK_F4:
			key = "f4";
			break;

		case VK_F5:
			key = "f5";
			break;

		case VK_F6:
			key = "f6";
			break;

		case VK_F7:
			key = "f7";
			break;

		case VK_F8:
			key = "f8";
			break;

		case VK_F9:
			key = "f9";
			break;

		case VK_F10:
			key = "f10";
			break;

		case VK_F11:
			key = "f11";
			break;

		case VK_F12:
			key = "f12";
			break;

		case VK_F13:
			key = "f13";
			break;

		case VK_F14:
			key = "f14";
			break;

		case VK_F15:
			key = "f15";
			break;

		case VK_F16:
			key = "f16";
			break;

		case VK_F17:
			key = "f17";
			break;

		case VK_F18:
			key = "f18";
			break;

		case VK_F19:
			key = "f19";
			break;

		case VK_F20:
			key = "f20";
			break;

		case VK_F21:
			key = "f21";
			break;

		case VK_F22:
			key = "f22";
			break;

		case VK_F23:
			key = "f23";
			break;

		case VK_F24:
			key = "f24";
			break;

		case VK_NUMLOCK:
			key = "num lock";
			break;

		case VK_SCROLL:
			key = "scroll lock";
			break;

		case VK_LSHIFT:
			key = "left shift";
			break;

		case VK_RSHIFT:
			key = "right shift";
			break;

		case VK_LCONTROL:
			key = "left control";
			break;

		case VK_RCONTROL:
			key = "right control";
			break;

		case VK_LMENU:
			key = "left menu";
			break;

		case VK_RMENU:
			key = "right menu";
			break;

		case VK_BROWSER_BACK:
			key = "browser back";
			break;

		case VK_BROWSER_FORWARD:
			key = "browser forward";
			break;

		case VK_BROWSER_REFRESH:
			key = "browser refresh";
			break;

		case VK_BROWSER_STOP:
			key = "browser stop";
			break;

		case VK_BROWSER_SEARCH:
			key = "browser search";
			break;

		case VK_BROWSER_FAVORITES:
			key = "browser favorites";
			break;

		case VK_BROWSER_HOME:
			key = "browser home";
			break;

		case VK_VOLUME_MUTE:
			key = "volume mute";
			break;

		case VK_VOLUME_DOWN:
			key = "volume down";
			break;

		case VK_VOLUME_UP:
			key = "volume up";
			break;

		case VK_MEDIA_NEXT_TRACK:
			key = "media next";
			break;

		case VK_MEDIA_PREV_TRACK:
			key = "media previous";
			break;

		case VK_MEDIA_STOP:
			key = "media stop";
			break;

		case VK_MEDIA_PLAY_PAUSE:
			key = "media play";
			break;

		case VK_LAUNCH_MAIL:
			key = "mail";
			break;

		case VK_LAUNCH_MEDIA_SELECT:
			key = "launch media";
			break;

		case VK_LAUNCH_APP1:
			key = "launch app1";
			break;

		case VK_LAUNCH_APP2:
			key = "launch app2";
			break;

		case VK_PLAY:
			key = "play";
			break;

		case VK_ZOOM:
			key = "zoom";
			break;

		default:
			return false;
	}

	return true;
}

bool Keyboard::allKeysDown(const Key keys, const bool synchron)
{
	static_assert(KEY_LAST_UNIQUE_VALUE <= 0x40000000, "Invalid KEY_LAST_UNIQUE_VALUE value");

	if (keys == KEY_NONE)
		return true;

	Key value = Key(1);

	while (value <= KEY_LAST_UNIQUE_VALUE)
	{
		if ((keys & value) != 0 && !isKeyDown(value, synchron))
			return false;

		value = Key(value << 1);
	}

	return true;
}

bool Keyboard::oneKeyDown(const Key keys, const bool synchron)
{
	static_assert(KEY_LAST_UNIQUE_VALUE <= 0x40000000, "Invalid KEY_LAST_UNIQUE_VALUE value");

	if (keys == KEY_NONE)
		return true;

	Key value = Key(1);

	while (value <= KEY_LAST_UNIQUE_VALUE)
	{
		if ((keys & value) != 0 && isKeyDown(value, synchron))
			return true;

		value = Key(value << 1);
	}

	return false;
}

bool Keyboard::isKeyDown(const Key key, const bool synchron)
{
	ocean_assert(key != KEY_NONE);

	switch (key)
	{
		case KEY_SHIFT_LEFT:
			return keyState(VK_LSHIFT, synchron);

		case KEY_SHIFT_RIGHT:
			return keyState(VK_RSHIFT, synchron);

		case KEY_CONTROL_LEFT:
			return keyState(VK_LCONTROL, synchron);

		case KEY_CONTROL_RIGHT:
			return keyState(VK_RCONTROL, synchron);

		case KEY_MENU_LEFT:
			return keyState(VK_LMENU, synchron);

		case KEY_MENU_RIGHT:
			return keyState(VK_RMENU, synchron);

		case KEY_SPACE:
			return keyState(VK_SPACE, synchron);

		case KEY_NONE:
		case KEY_SHIFT:
		case KEY_CONTROL:
		case KEY_MENU:
			break;
	}

	ocean_assert(false && "Invalid key");
	return false;
}

Keyboard::Key Keyboard::currentKeyState(const bool synchron)
{
	Key key = KEY_NONE;

	if (oneKeyDown(KEY_SHIFT, synchron))
		key = Key(key | KEY_SHIFT);

	if (oneKeyDown(KEY_CONTROL, synchron))
		key = Key(key | KEY_CONTROL);

	if (oneKeyDown(KEY_MENU, synchron))
		key = Key(key | KEY_MENU);

	if (oneKeyDown(KEY_SPACE, synchron))
		key = Key(key | KEY_SPACE);

	return key;
}

}

}

}
