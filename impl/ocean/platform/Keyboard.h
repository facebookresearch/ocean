/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_KEYBOARD_H
#define META_OCEAN_PLATFORM_KEYBOARD_H

#include "ocean/platform/Platform.h"

namespace Ocean
{

namespace Platform
{

/**
 * This class implements keyboard functionalities.
 * Not available on the following platforms: Android, IOS.
 * @ingroup platform
 */
class OCEAN_PLATFORM_EXPORT Keyboard
{
	public:

		/**
		 * Definition of individual keyboard keys.
		 */
		enum Key
		{
			/// No key.
			KEY_NONE = 0,
			/// The left shift key.
			KEY_SHIFT_LEFT = (1 << 0),
			/// The right shift key.
			KEY_SHIFT_RIGHT = (1 << 1),
			/// The left control key.
			KEY_CONTROL_LEFT = (1 << 2),
			/// The right control key.
			KEY_CONTROL_RIGHT = (1 << 3),
			/// The left menu key.
			KEY_MENU_LEFT = (1 << 4),
			/// The right menu key.
			KEY_MENU_RIGHT = (1 << 5),
			/// The space key.
			KEY_SPACE = (1 << 6),
			/// Last key value.
			KEY_LAST_UNIQUE_VALUE = (1 << 6),
			/// Both shift keys (left and right), beware: is not a unique (single) key.
			KEY_SHIFT = KEY_SHIFT_LEFT | KEY_SHIFT_RIGHT,
			/// Both control keys (left and right), beware: is not a unique (single) key.
			KEY_CONTROL = KEY_CONTROL_LEFT | KEY_CONTROL_RIGHT,
			/// Both menu keys (left and right), beware: is not a unique (single) key.
			KEY_MENU = KEY_MENU_LEFT | KEY_MENU_RIGHT,
		};

	public:

		/**
		 * Returns whether all specified keys of the keyboard are currently pushed (down) or not.
		 * @param keys The keys for that the states are requested (may be a combination of several keys)
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so; also True if no key is provided
		 */
		static bool allKeysDown(const Key keys, const bool synchron = true);

		/**
		 * Returns whether at least one key of the specified keys of the keyboard is currently pushed (down) or not.
		 * @param keys The keys for that the states are requested (may be a combination of several keys)
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so; also True if no key is provided
		 */
		static bool oneKeyDown(const Key keys, const bool synchron = true);

		/**
		 * Returns whether exactly one (specified key) is currently pushed (down) or not.
		 * @param key One unique key for that the state is requested, must not be a combination of several keys
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool isKeyDown(const Key key, const bool synchron = true);

		/**
		 * Returns the current key state for the shift, control and menu keys.
		 * This function does not distinguish between the left and a right key version.
		 * @return Current key state
		 */
		static Key currentKeyState(const bool synchron = true);

	protected:

#if defined(_WINDOWS)

		/**
		 * Returns whether exactly one (specified key) is currently pushed (down) or not (specialization for Windows)
		 * @sa isKeyDown()
		 * @param key One unique key for that the state is requested, must not be a combination of several keys
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool isKeyDownWindows(const Key key, const bool synchron = true);

		/**
		 * Returns whether exactly one (specified key) is currently pushed (down) or not.
		 * @param nVirtualKey The virtual key code for that the state is requested
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool keyStateWindows(const int nVirtualKey, const bool synchron);

#elif defined(__APPLE__)

		/**
		 * Returns whether exactly one (specified key) is currently pushed (down) or not (specialization for Apple platforms)
		 * @sa isKeyDown()
		 * @param key One unique key for that the state is requested, must not be a combination of several keys
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool isKeyDownApple(const Key key, const bool synchron = true);

		/**
		 * Returns whether exactly one (specified key) is currently pushed (down) or not.
		 * @param nVirtualKey The virtual key code for that the state is requested
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static bool keyStateApple(const unsigned short nVirtualKey, const bool synchron);

#endif

};

}

}

#endif // META_OCEAN_PLATFORM_KEYBOARD_H
