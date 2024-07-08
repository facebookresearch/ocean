/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FONTS_FONTS_H
#define META_OCEAN_CV_FONTS_FONTS_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

/**
 * @ingroup cv
 * @defgroup cvfonts Ocean CV Fonts Library
 * @{
 * The Ocean CV Fonts Library provides font functionalities like drawing/rendering text.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::CV::Fonts Namespace of the CV Fonts library.<p>
 * The Namespace Ocean::CV::Fonts is used in the entire Ocean CV Fonts Library.
 */

// Defines OCEAN_CV_FONTS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_FONTS_EXPORT
		#define OCEAN_CV_FONTS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_FONTS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_FONTS_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_FONT_FONT_H
