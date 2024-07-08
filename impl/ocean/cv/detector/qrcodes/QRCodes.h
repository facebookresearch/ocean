/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * @ingroup cvdetector
 * @defgroup cvdetectorqrcodes Ocean QR Code Library
 * @{
 * The Ocean QR Code Library implements detectors and generators for QR codes.
 * @}
 */

/**
 * @namespace Ocean::Tracking::QRCodes Namespace of the Ocean QR code Library.<p>
 * The Namespace Ocean::Tracking::QRCodes is used in the entire Ocean QR code Library.
 */

// Defines OCEAN_CV_DETECTOR_QRCODES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_DETECTOR_QRCODES_EXPORT
		#define OCEAN_CV_DETECTOR_QRCODES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_DETECTOR_QRCODES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_DETECTOR_QRCODES_EXPORT
#endif

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
