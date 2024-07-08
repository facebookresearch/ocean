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
#include "ocean/math/Vector2.h"

#include <array>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Barcodes
{

/**
 * @ingroup cvdetector
 * @defgroup cvdetectorbarcodes Ocean Barcode Library
 * @{
 * The Ocean Barcode Library implements a detector for barcodes.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Barcodes Namespace of the barcode code library.<p>
 * The namespace Ocean::Tracking::Barcodes is used in the entire Ocean Barcode Library.
 */

// Defines OCEAN_CV_DETECTOR_BARCODES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_DETECTOR_BARCODES_EXPORT
		#define OCEAN_CV_DETECTOR_BARCODES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_DETECTOR_BARCODES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_DETECTOR_BARCODES_EXPORT
#endif

/**
 * Definition of scan line data, i.e., a sequence of raw pixel data.
 * @ingroup cvdetectorbarcodes
 */
typedef std::vector<uint8_t> ScanlineData;

/**
 * Definition of segment data, i.e., a sequence of lengths of binary, alternating foreground and background segments (resulting from applying a threshold to raw pixel data).
 * @ingroup cvdetectorbarcodes
 */
typedef std::vector<uint32_t> SegmentData;

} // namespace Barcodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
