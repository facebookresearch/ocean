/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_SEGMENTATION_H
#define META_OCEAN_CV_SEGMENTATION_SEGMENTATION_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * @ingroup cv
 * @defgroup cvsegmentation Ocean CV Segmentation Library
 * @{
 * The Ocean CV Segmentation Library provides all computer vision segmentation, contour and masking functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Segmentation Namespace of the CV Segmentation library.<p>
 * The Namespace Ocean::CV::Segmentation is used in the entire Ocean CV Segmentation Library.
 */

// Defines OCEAN_CV_SEGMENTATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_SEGMENTATION_EXPORT
		#define OCEAN_CV_SEGMENTATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_SEGMENTATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_SEGMENTATION_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_SEGMENTATION_H
