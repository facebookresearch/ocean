/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/Mouse.h"

#ifndef __APPLE__
    #error This files contains Apple specific functions only, see Mouse.cpp
#endif

#if (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR == 1) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1)
	#error Platform::Mouse is not available on iOS platforms!
#endif

#include <AppKit/AppKit.h>
#include <Carbon/Carbon.h>

namespace Ocean
{

namespace Platform
{

bool Mouse::isButtonDownApple(const Button button, const bool synchron)
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

VectorI2 Mouse::screenPositionApple(const VectorI2& invalidPosition)
{
  NSPoint position = [NSEvent mouseLocation];
  return VectorI2(NumericD::round32(position.x), NumericD::round32(position.y));
}

} // namespace Platform

} // namespace Ocean
