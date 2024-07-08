/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/Keyboard.h"

#include <Carbon/Carbon.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

bool Keyboard::translateVirtualkey(const unsigned short vKey, std::string& key)
{
	switch (vKey)
	{
		default:
			ocean_assert(false && "Missing implementation!");
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

	const UInt32 state = synchron ? GetCurrentEventKeyModifiers() : GetCurrentKeyModifiers();

	// optionKey = 0x0100 Left Command Key down?
	// optionKey = 0x0100 Right Command Key down?

	switch (key)
	{
		case KEY_SHIFT_LEFT:
			// shiftKey = 0x0200 Shift key down?
			return (state & 0x0200u) != 0u;

		case KEY_SHIFT_RIGHT:
			// rightShiftKey = 0x0200 Right Shift Key down?
			return (state & 0x0200u) != 0u;

		case KEY_CONTROL_LEFT:
			// controlKey = 0x1000 Control key down?
			return (state & 0x1000u) != 0u;

		case KEY_CONTROL_RIGHT:
			// rightControlKey = 0x1000 Right Control Key down?
			return (state & 0x1000u) != 0u;

		case KEY_MENU_LEFT:
			// optionKey = 0x0800 Option key down?
			return (state & 0x800u) != 0u;

		case KEY_MENU_RIGHT:
			// rightOptionKey = 0x0800 Right Option Key down?
			return (state & 0x0800u) != 0u;

		case KEY_SPACE:
			return keyState(kVK_Space, synchron);

		default:
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

inline bool Keyboard::keyState(const unsigned short nVirtualKey, const bool synchron)
{
	unsigned char keyMap[16] = {0};

	GetKeys((BigEndianUInt32*)&keyMap);

	return 0 != ((keyMap[nVirtualKey >> 3u] >> (nVirtualKey & 7u)) & 1u);
}

}

}

}

}
