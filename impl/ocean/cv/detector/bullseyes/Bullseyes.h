/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_H
#define OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_H

#include "ocean/cv/detector/Detector.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * @ingroup cvdetector
 * @defgroup cvdetectorbullseyes Ocean Bullseye Library
 * @{
 * The Ocean Bullseyes Library implements a detector for bullseye patterns.
 * @}
 */

/**
 * @namespace Ocean::CV::Detector::Bullseyes Namespace of the bullseye code library.<p>
 * The namespace Ocean::CV::Detector::Bullseyes is used in the entire Ocean Bullseye Library.
 */

// Defines OCEAN_CV_DETECTOR_BULLSEYES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_DETECTOR_BULLSEYES_EXPORT
		#define OCEAN_CV_DETECTOR_BULLSEYES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_DETECTOR_BULLSEYES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_DETECTOR_BULLSEYES_EXPORT
#endif

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYES_H
