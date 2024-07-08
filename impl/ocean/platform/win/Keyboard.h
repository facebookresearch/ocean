/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_KEYBOARD_H
#define META_OCEAN_PLATFORM_WIN_KEYBOARD_H

#include "ocean/platform/win/Win.h"

#include "ocean/platform/Keyboard.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements keyboard functionalities.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Keyboard : Platform::Keyboard
{
	public:

		/**
		 * Translates a virtual key code to the corresponsing character key and key description.
		 * @param vKey Virtual key code to translate
		 * @param key Resulting key character or name
		 * @return True, if succeeded
		 */
		static bool translateVirtualkey(const unsigned int vKey, std::string& key);

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
		 * Returns wheater exactly one (specified key) is currently pushed (down) or not.
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

		/**
		 * Returns wheater exactly one (specified key) is currently pushed (down) or not.
		 * @param nVirtualKey The virtual key code for that the state is requested
		 * @param synchron True, to request the state synchronously with the message queue; False, to request the state in the current moment
		 * @return True, if so
		 */
		static inline bool keyState(const int nVirtualKey, const bool synchron);
};

inline bool Keyboard::keyState(const int nVirtualKey, const bool synchron)
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

#endif // META_OCEAN_PLATFORM_WIN_KEYBOARD_H
