/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/tracking/Tracking.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector3D.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Vector2.h"

#include <functional>

namespace Ocean
{

namespace Tracking
{

namespace QRCodes
{

/**
 * @ingroup tracking
 * @defgroup trackingqrcodes Ocean QR Code Tracking Library
 * @{
 * The Ocean QR Code Tracking Library implements a 6DOF tracker for QR codes.
 * @}
 */

/**
 * @namespace Ocean::Tracking::QRCodes Namespace of the Ocean QR code tracking library.<p>
 * The Namespace Ocean::Tracking::QRCodes is used in the entire Ocean QR code Library.
 */

// Defines OCEAN_TRACKING_QRCODES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_QRCODES_EXPORT
		#define OCEAN_TRACKING_QRCODES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_QRCODES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_QRCODES_EXPORT
#endif

} // namespace QRCodes

} // namespace Tracking

} // namespace Ocean
