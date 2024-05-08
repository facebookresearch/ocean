/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_DETECTOR_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * @ingroup cv
 * @defgroup cvdetector Ocean CV Detector Library
 * @{
 * The Ocean CV Dectector Library provides several different computer vision feature detection functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Detector Namespace of the CV Detector library.<p>
 * The Namespace Ocean::CV::Detector is used in the entire Ocean CV Detector Library.
 */

// Defines OCEAN_CV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_DETECTOR_EXPORT
		#define OCEAN_CV_DETECTOR_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_DETECTOR_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_DETECTOR_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_DETECTOR_DETECTOR_H
