/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_SYNTHESIS_H
#define META_OCEAN_CV_SYNTHESIS_SYNTHESIS_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * @ingroup cv
 * @defgroup cvsynthesis Ocean CV Synthesis Library
 * @{
 * The Ocean CV Synthesis Library provides all computer vision synthesis functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Synthesis Namespace of the CV Synthesis library.<p>
 * The Namespace Ocean::CV::Synthesis is used in the entire Ocean CV Synthesis Library.
 */

// Defines OCEAN_CV_SYNTHESIS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_SYNTHESIS_EXPORT
		#define OCEAN_CV_SYNTHESIS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_SYNTHESIS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_SYNTHESIS_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_SYNTHESIS_H
