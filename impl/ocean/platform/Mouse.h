/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_MOUSE_H
#define META_OCEAN_PLATFORM_MOUSE_H

#include "ocean/platform/Platform.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Platform
{

/**
 * This class implements mouse functionalities.
 * Not available on the following platforms: Android, IOS.
 * @ingroup platform
 */
class OCEAN_PLATFORM_EXPORT Mouse
{
	public:

		/**
		 * Definition of individual mouse buttons.
		 */
		enum Button
		{
			/// No key.
			BUTTON_NONE = 0,
			/// The left mouse button.
			BUTTON_LEFT = (1 << 0),
			/// The middle mouse button.
			BUTTON_MIDDLE = (1 << 1),
			/// The right mouse button.
			BUTTON_RIGHT = (1 << 2),
			/// Last key value.
			BUTTON_LAST_UNIQUE_VALUE = (1 << 2)
		};

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
		 * Returns whether exactly one (specified button) is currently pushed (down) or not.
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
		 * Returns the current position of the mouse/cursor in screen coordinates.
		 * @param invalidPosition The invalid mouse/cursor position if currently no mouse/cursor position exists or can be determined.
		 * @return Mouse screen coordinates
		 */
		static VectorI2 screenPosition(const VectorI2& invalidPosition = VectorI2(NumericT<int>::minValue(), NumericT<int>::minValue()));

	protected:

#ifdef _WINDOWS

		/**
		 * Returns whether exactly one (specified button) is currently pushed (down) or not (specialization for Windows).
		 * @sa isButtonDown()
		 * @param button One unique button for that the state is requested, must not be a combination of several buttons
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool isButtonDownWindows(const Button button, const bool synchron = true);

		/**
		 * Returns whether exactly one (specified key) is currently pushed (down) or not.
		 * @param nVirtualKey The virtual key code for that the state is requested
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool keyStateWindows(const int nVirtualKey, const bool synchron);

		/**
		 * Returns the current position of the mouse/cursor in screen coordinates (specialization for Windows).
		 * @sa screenPosition()
		 * @param invalidPosition The invalid mouse/cursor position if currently no mouse/cursor position exists or can be determined.
		 * @return Mouse screen coordinates
		 */
		static VectorI2 screenPositionWindows(const VectorI2& invalidPosition = VectorI2(NumericT<int>::minValue(), NumericT<int>::minValue()));

#elif defined(__APPLE__)

		/**
		 * Returns whether exactly one (specified button) is currently pushed (down) or not (specialization for Apple platforms).
		 * @sa isButtonDown()
		 * @param button One unique button for that the state is requested, must not be a combination of several buttons
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool isButtonDownApple(const Button button, const bool synchron = true);

		/**
		 * Returns the current position of the mouse/cursor in screen coordinates (specialization for Apple platforms).
		 * @sa screenPosition()
		 * @param invalidPosition The invalid mouse/cursor position if currently no mouse/cursor position exists or can be determined.
		 * @return Mouse screen coordinates
		 */
		static VectorI2 screenPositionApple(const VectorI2& invalidPosition = VectorI2(NumericT<int>::minValue(), NumericT<int>::minValue()));

#endif // defined(_WINDOWS)
};

}

}

#endif // META_OCEAN_PLATFORM_MOUSE_H
