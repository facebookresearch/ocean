/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGS_H
#define META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGS_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Base.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"

#include <unordered_map>

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

/**
 * @ingroup tracking
 * @defgroup trackingoculustags Ocean Oculus Tag Library
 * @{
 * The Ocean Oculus Tag Tracker Library implements a 6DOF Oculus Tag detector and tracker, e.g. for Oculus head-mounted devices (HMD).
 * @}
 */

/**
 * @namespace Ocean::Tracking::OculusTags Namespace of the Oculus Tag Tracker library.<p>
 * The Namespace Ocean::Tracking::OculusTags is used in the entire Ocean Oculus Tag Tracker Library.
 */

// Defines OCEAN_TRACKING_OCULUSTAGS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_OCULUSTAGS_EXPORT
		#define OCEAN_TRACKING_OCULUSTAGS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_OCULUSTAGS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_OCULUSTAGS_EXPORT
#endif

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGS_H
