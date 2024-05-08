/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/Keyboard.h"

#ifndef __APPLE__
	#error This files contains Apple specific functions only, see Keyboard.cpp
#endif

#if (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR == 1) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1)
	#error Platform::Keyboard is not available on iOS platforms!
#endif

#include <Carbon/Carbon.h>

namespace Ocean
{

namespace Platform
{

bool Keyboard::isKeyDownApple(const Key key, const bool synchron)
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
      return keyStateApple(kVK_Space, synchron);

    default:
      break;
  }

  ocean_assert(false && "Invalid key");
  return false;
}

bool Keyboard::keyStateApple(const unsigned short nVirtualKey, const bool synchron)
{
  unsigned char keyMap[16] = {0};

  GetKeys((BigEndianUInt32*)&keyMap);

  return 0 != ((keyMap[nVirtualKey >> 3u] >> (nVirtualKey & 7u)) & 1u);
}

} // namespace Platform

} // namespace Ocean
