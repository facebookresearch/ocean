/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/Mouse.h"

#include <AppKit/AppKit.h>
#include <Carbon/Carbon.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

bool Mouse::allButtonsDown(const Button buttons, const bool synchron)
{
	static_assert(BUTTON_LAST_UNIQUE_VALUE <= 0x40000000, "Invalid BUTTON_LAST_UNIQUE_VALUE value");

	if (buttons == BUTTON_NONE)
		return true;

	Button value = Button(1);

	while (value <= BUTTON_LAST_UNIQUE_VALUE)
	{
		if ((buttons & value) != 0 && !isButtonDown(value, synchron))
			return false;

		value = Button(value << 1);
	}

	return true;
}

bool Mouse::oneButtonDown(const Button buttons, const bool synchron)
{
	static_assert(BUTTON_LAST_UNIQUE_VALUE <= 0x40000000, "Invalid BUTTON_LAST_UNIQUE_VALUE value");

	if (buttons == BUTTON_NONE)
		return true;

	Button value = Button(1);

	while (value <= BUTTON_LAST_UNIQUE_VALUE)
	{
		if ((buttons & value) != 0 && isButtonDown(value, synchron))
			return true;

		value = Button(value << 1);
	}

	return false;
}

bool Mouse::isButtonDown(const Button button, const bool synchron)
{
	ocean_assert(button != BUTTON_NONE);

	const unsigned int state = synchron ? GetCurrentEventButtonState() : GetCurrentButtonState();

	switch (button)
	{
		case BUTTON_LEFT:
			return (state & 0x00000001u) != 0u;

		case BUTTON_MIDDLE:
			return (state & 0x00000004u) != 0u;

		case BUTTON_RIGHT:
			return (state & 0x00000002u) != 0u;

		default:
			break;
	}

	ocean_assert(false && "Invalid mouse");
	return false;
}

Mouse::Button Mouse::currentMouseButtonState(const bool synchron)
{
	Button button = BUTTON_NONE;

	if (isButtonDown(BUTTON_LEFT, synchron))
		button = Button(button | BUTTON_LEFT);

	if (isButtonDown(BUTTON_MIDDLE, synchron))
		button = Button(button | BUTTON_MIDDLE);

	if (isButtonDown(BUTTON_RIGHT, synchron))
		button = Button(button | BUTTON_RIGHT);

	return button;
}

VectorI2 Mouse::screenPosition()
{
	NSPoint position = [NSEvent mouseLocation];

	return VectorI2(NumericD::round32(position.x), NumericD::round32(position.y));
}

}

}

}

}
