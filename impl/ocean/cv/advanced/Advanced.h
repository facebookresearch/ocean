/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * @ingroup cv
 * @defgroup cvadvanced Ocean CV Advanced Library
 * @{
 * The Ocean CV Advanced Library provides several different computer vision feature detection functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Advanced Namespace of the CV Advanced library.<p>
 * The Namespace Ocean::CV::Advanced is used in the entire Ocean CV Advanced Library.
 */

// Defines OCEAN_CV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_ADVANCED_EXPORT
		#define OCEAN_CV_ADVANCED_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_ADVANCED_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_ADVANCED_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_H
