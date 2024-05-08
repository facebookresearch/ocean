/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Mouse.h"

#include <winsock2.h>
#include <windows.h>

namespace Ocean
{

namespace Platform
{

namespace Win
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
	ocean_assert(button != BUTTON_NONE);

	switch (button)
	{
		case BUTTON_LEFT:
			return keyState(VK_LBUTTON, synchron);

		case BUTTON_MIDDLE:
			return keyState(VK_MBUTTON, synchron);

		case BUTTON_RIGHT:
			return keyState(VK_RBUTTON, synchron);

		case BUTTON_NONE:
			break;
	}

	ocean_assert(false && "Invalid mouse");
	return false;
}

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

VectorI2 Mouse::screenPosition()
{
	POINT position;
	position.x = 0;
	position.y = 0;

	const bool result = GetCursorPos(&position) == TRUE;
	ocean_assert_and_suppress_unused(result, result);

	return VectorI2(position.x, position.y);
}

}

}

}
