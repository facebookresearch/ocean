// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_KEYBOARD_KEYBOARD_H
#define META_OCEAN_TRACKING_KEYBOARD_KEYBOARD_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace Keyboard
{

/**
 * @ingroup tracking
 * @defgroup trackingkeyboard Ocean Keyboard Tracker Library
 * @{
 * The Ocean Keyboard Tracker Library implements a 6DOF keyboard detector and tracker for Quest.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Keyboard Namespace of the Keyboard Tracker library.<p>
 * The Namespace Ocean::Tracking::Keyboard is used in the entire Ocean Keyboard Tracker Library.
 */

// Defines OCEAN_TRACKING_KEYBOARD_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_KEYBOARD_EXPORT
		#define OCEAN_TRACKING_KEYBOARD_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_KEYBOARD_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_KEYBOARD_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_KEYBOARD_KEYBOARD_H
