/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CALIBRATION_H
#define META_OCEAN_CV_CALIBRATION_CALIBRATION_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * @ingroup cv
 * @defgroup cvcalibration Ocean CV Calibration Library
 * @{
 * The Ocean CV Calibration Library provides functionalities for camera calibration.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Calibration Namespace of the CV Calibration library.<p>
 * The Namespace Ocean::CV::Calibration is used in the entire Ocean CV Calibration Library.
 */

// Defines OCEAN_CV_CALIBRATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_CALIBRATION_EXPORT
		#define OCEAN_CV_CALIBRATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_CALIBRATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_CALIBRATION_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CALIBRATION_H
