// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_CV_DETECTOR_BLOB_BLOB_H
#define META_OCEAN_CV_DETECTOR_BLOB_BLOB_H

#include "ocean/cv/detector/Detector.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Blob
{

/**
 * @ingroup cvdetector
 * @defgroup cvdetectorblob Ocean CV Detector Blob Library
 * @{
 * The Ocean CV Detector Blob Library provides all computer vision and tracking functionalities for Blob features.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Detector::Blob Namespace of the CV Detector Blob library.<p>
 * The Namespace Ocean::CV::Detector::Blob is used in the entire Ocean CV Detector Blob Library.
 */

// Defines OCEAN_CV_DETECTOR_BLOB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_DETECTOR_BLOB_EXPORT
		#define OCEAN_CV_DETECTOR_BLOB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_DETECTOR_BLOB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_DETECTOR_BLOB_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_BLOB_BLOB_H
