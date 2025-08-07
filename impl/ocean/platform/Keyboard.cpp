/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/Keyboard.h"

namespace Ocean
{

namespace Platform
{

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
#if defined(_WINDOWS)

  return isKeyDownWindows(key, synchron);

#elif defined(__APPLE__)

  return isKeyDownApple(key, synchron);

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(key);
	OCEAN_SUPPRESS_UNUSED_WARNING(synchron);

  ocean_assert(false && "Missing implementation!");
  return false;

#endif
}

#if defined(_WINDOWS)

bool Keyboard::isKeyDownWindows(const Key key, const bool synchron)
{
	ocean_assert(key != KEY_NONE);

	switch (key)
	{
		case KEY_SHIFT_LEFT:
			return keyStateWindows(VK_LSHIFT, synchron);

		case KEY_SHIFT_RIGHT:
			return keyStateWindows(VK_RSHIFT, synchron);

		case KEY_CONTROL_LEFT:
			return keyStateWindows(VK_LCONTROL, synchron);

		case KEY_CONTROL_RIGHT:
			return keyStateWindows(VK_RCONTROL, synchron);

		case KEY_MENU_LEFT:
			return keyStateWindows(VK_LMENU, synchron);

		case KEY_MENU_RIGHT:
			return keyStateWindows(VK_RMENU, synchron);

		case KEY_SPACE:
			return keyStateWindows(VK_SPACE, synchron);

		case KEY_NONE:
		case KEY_SHIFT:
		case KEY_CONTROL:
		case KEY_MENU:
			break;
	}

	ocean_assert(false && "Invalid key");
	return false;
}

bool Keyboard::keyStateWindows(const int nVirtualKey, const bool synchron)
{
	if (synchron)
	{
		// MSDN: If the high-order bit is 1, the key is down; otherwise, it is up
		return (GetKeyState(nVirtualKey) & 0xF0) != 0;
	}
	else
		return GetAsyncKeyState(nVirtualKey) != 0;
}

#endif // defined(_WINDOWS)

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

} // namespace Platform

} // namespace Ocean
