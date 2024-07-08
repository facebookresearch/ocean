/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DEPTH_DEPTH_H
#define META_OCEAN_CV_DEPTH_DEPTH_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Depth
{

/**
 * @ingroup cv
 * @defgroup cvdepth Ocean CV Depth Library
 * @{
 * The Ocean CV Depth Library provides functionalities for depth estimation, stereo rectification etc.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Depth Namespace of the CV Depth library.<p>
 * The Namespace Ocean::CV::Depth is used in the entire Ocean CV Depth Library.
 */

// Defines OCEAN_CV_DEPTH_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_DEPTH_EXPORT
		#define OCEAN_CV_DEPTH_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_DEPTH_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_DEPTH_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_DEPTH_DEPTH_H
