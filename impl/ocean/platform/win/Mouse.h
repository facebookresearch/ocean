/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_MOUSE_H
#define META_OCEAN_PLATFORM_WIN_MOUSE_H

#include "ocean/platform/win/Win.h"

#include "ocean/math/Vector2.h"

#include "ocean/platform/Mouse.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements mouse functionalities.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Mouse : Platform::Mouse
{
	public:

		/**
		 * Returns whether all specified buttons of the mouse are currently pushed (down) or not.
		 * @param buttons The buttons for that the states are requested (may be a combination of several buttons)
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so; also True if no button is provided
		 */
		static bool allButtonsDown(const Button buttons, const bool synchron = true);

		/**
		 * Returns whether at least one button of the specified buttons of the mouse is currently pushed (down) or not.
		 * @param buttons The buttons for that the states are requested (may be a combination of several buttons)
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so; also True if no button is provided
		 */
		static bool oneButtonDown(const Button buttons, const bool synchron = true);

		/**
		 * Returns wheater exactly one (specified button) is currently pushed (down) or not.
		 * @param button One unique button for that the state is requested, must not be a combination of several buttons
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool isButtonDown(const Button button, const bool synchron = true);

		/**
		 * Returns the current mouse button state for the left, middle and right mouse button.
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return Current mouse button state
		 */
		static Button currentMouseButtonState(const bool synchron = true);

		/**
		 * Returns the current position of the mouse (cursor) in screen coordinates.
		 * @return Mouse screen coordinates
		 */
		static VectorI2 screenPosition();

	protected:

		/**
		 * Returns wheater exactly one (specified mouse button) is currently pushed (down) or not.
		 * @param nVirtualKey The virtual key code for that the state is requested
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static inline bool keyState(const int nVirtualKey, const bool synchron);
};

inline bool Mouse::keyState(const int nVirtualKey, const bool synchron)
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

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_MOUSE_H
