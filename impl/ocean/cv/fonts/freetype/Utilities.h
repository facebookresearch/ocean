/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FONTS_FREETYPE_UTILITIES_H
#define META_OCEAN_CV_FONTS_FREETYPE_UTILITIES_H

#include "ocean/cv/fonts/freetype/FreeType.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

/**
 * This class implements utility functions for the FreeType font library.
 * @ingroup cvfonts
 */
class OCEAN_CV_FONTS_EXPORT Utilities
{
	public:

		/**
		 * Translates the a FreeType error code to a readable string.
		 * @param error The error code to translate
		 * @return The readable string
		 */
		static std::string translateErrorCode(const FT_Error error);
};

}

}

}

#endif // META_OCEAN_CV_FONTS_FREETYPE_UTILITIES_H
