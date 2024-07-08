/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/Mouse.h"

namespace Ocean
{

namespace Platform
{

bool Mouse::allButtonsDown(const Button buttons, const bool synchron)
{
	static_assert(BUTTON_LAST_UNIQUE_VALUE <= 0x40000000, "Invalid BUTTON_LAST_UNIQUE_VALUE value");

	if (buttons == BUTTON_NONE)
	{
		return true;
	}

	Button value = Button(1);

	while (value <= BUTTON_LAST_UNIQUE_VALUE)
	{
		if ((buttons & value) != 0 && !isButtonDown(value, synchron))
		{
			return false;
		}

		value = Button(value << 1);
	}

	return true;
}

bool Mouse::oneButtonDown(const Button buttons, const bool synchron)
{
	static_assert(BUTTON_LAST_UNIQUE_VALUE <= 0x40000000, "Invalid BUTTON_LAST_UNIQUE_VALUE value");

	if (buttons == BUTTON_NONE)
	{
		return true;
	}

	Button value = Button(1);

	while (value <= BUTTON_LAST_UNIQUE_VALUE)
	{
		if ((buttons & value) != 0 && isButtonDown(value, synchron))
		{
			return true;
		}

		value = Button(value << 1);
	}

	return false;
}

bool Mouse::isButtonDown(const Button button, const bool synchron)
{
#if defined(_WINDOWS)

	return isButtonDownWindows(button, synchron);

#elif defined(__APPLE__)

	return isButtonDownApple(button, synchron);

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(button);
	OCEAN_SUPPRESS_UNUSED_WARNING(synchron);

	ocean_assert(false && "Missing implementation!");
	return false;

#endif
}

#if defined(_WINDOWS)

bool Mouse::isButtonDownWindows(const Button button, const bool synchron)
{
	ocean_assert(button != BUTTON_NONE);

	switch (button)
	{
		case BUTTON_LEFT:
			return keyStateWindows(VK_LBUTTON, synchron);

		case BUTTON_MIDDLE:
			return keyStateWindows(VK_MBUTTON, synchron);

		case BUTTON_RIGHT:
			return keyStateWindows(VK_RBUTTON, synchron);

		case BUTTON_NONE:
			break;
	}

	ocean_assert(false && "Invalid mouse");
	return false;
}

bool Mouse::keyStateWindows(const int nVirtualKey, const bool synchron)
{
	if (synchron)
	{
		// MSDN: If the high-order bit is 1, the key is down; otherwise, it is up
		return (GetKeyState(nVirtualKey) & 0xF0) != 0;
	}
	else
	{
		return GetAsyncKeyState(nVirtualKey) != 0;
	}
}

#endif // defined(_WINDOWS)

Mouse::Button Mouse::currentMouseButtonState(const bool synchron)
{
	Button button = BUTTON_NONE;

	if (isButtonDown(BUTTON_LEFT, synchron))
	{
		button = Button(button | BUTTON_LEFT);
	}

	if (isButtonDown(BUTTON_MIDDLE, synchron))
	{
		button = Button(button | BUTTON_MIDDLE);
	}

	if (isButtonDown(BUTTON_RIGHT, synchron))
	{
		button = Button(button | BUTTON_RIGHT);
	}

	return button;
}

VectorI2 Mouse::screenPosition(const VectorI2& invalidPosition)
{
#if defined(_WINDOWS)
	return screenPositionWindows(invalidPosition);
#elif defined(__APPLE__)
	return screenPositionApple(invalidPosition);
#else
	ocean_assert(false && "Not implemented!");
	return invalidPosition;
#endif
}

#if defined(_WINDOWS)

VectorI2 Mouse::screenPositionWindows(const VectorI2& /*invalidPosition*/)
{
	POINT position;
	position.x = 0;
	position.y = 0;

	const bool result = GetCursorPos(&position) == TRUE;
	ocean_assert_and_suppress_unused(result, result);

	return VectorI2(position.x, position.y);
}

#endif // defined(_WINDOWS)

} // namespace Platform

} // namespace Ocean
