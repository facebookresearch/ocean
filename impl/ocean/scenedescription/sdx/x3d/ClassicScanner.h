/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_CLASSIC_SCANNER_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_CLASSIC_SCANNER_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/io/Scanner.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a scanner for the classic x3d file format.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT ClassicScanner : public IO::Scanner
{
	public:

		/**
		 * Definition of x3d symbols.
		 */
		enum Symbol : uint32_t
		{
			/// Dot symbol: '.'.
			SYMBOL_DOT,
			/// Node begin symbol: '{'.
			SYMBOL_NODE_BEGIN,
			/// Node end symbol: '}'.
			SYMBOL_NODE_END,
			/// Multi field begin symbol: '['.
			SYMBOL_FIELD_BEGIN,
			/// Multi field end symbol: ']'.
			SYMBOL_FIELD_END
		};

		/**
		 * Definition of x3d keywords.
		 */
		enum Keyword : uint32_t
		{
			/// DEF keyword.
			KEYWORD_DEF,
			/// FALSE keyword.
			KEYWORD_FALSE,
			/// META keyword.
			KEYWORD_META,
			/// MFBool keyword.
			KEYWORD_MF_BOOL,
			/// MFColor keyword.
			KEYWORD_MF_COLOR,
			/// MFFloat keyword.
			KEYWORD_MF_FLOAT,
			/// MFInt32 keyword.
			KEYWORD_MF_INT32,
			/// MFMatrix3f keyword.
			KEYWORD_MF_MATRIX_3F,
			/// MFMatrix4f keyword.
			KEYWORD_MF_MATRIX_4F,
			/// MFNode keyword.
			KEYWORD_MF_NODE,
			/// MFRotation keyword.
			KEYWORD_MF_ROTATION,
			/// MFString keyword.
			KEYWORD_MF_STRING,
			/// MFTime keywod.
			KEYWORD_MF_TIME,
			/// MFVec2f keyword.
			KEYWORD_MF_VEC_2F,
			/// MFVec3f keyword.
			KEYWORD_MF_VEC_3F,
			/// MFVec4f keyword.
			KEYWORD_MF_VEC_4F,
			/// NULL keyword.
			KEYWORD_NULL,
			/// PROFILE keyword.
			KEYWORD_PROFILE,
			/// ROUTE keyword.
			KEYWORD_ROUTE,
			/// SFBool keyword.
			KEYWORD_SF_BOOL,
			/// SFColor keyword.
			KEYWORD_SF_COLOR,
			/// SFFloat keyword.
			KEYWORD_SF_FLOAT,
			/// SFInt32 keyword.
			KEYWORD_SF_INT32,
			/// SFMatrix3f keyword.
			KEYWORD_SF_MATRIX_3F,
			/// SFMatrix4f keyword.
			KEYWORD_SF_MATRIX_4F,
			/// SFNode keyword.
			KEYWORD_SF_NODE,
			/// SFRotation keyword.
			KEYWORD_SF_ROTATION,
			/// SFString keyword.
			KEYWORD_SF_STRING,
			/// SFTime keywod.
			KEYWORD_SF_TIME,
			/// SFVec2f keyword.
			KEYWORD_SF_VEC_2F,
			/// SFVec3f keyword.
			KEYWORD_SF_VEC_3F,
			/// SFVec4f keyword.
			KEYWORD_SF_VEC_4F,
			/// TO keyword.
			KEYWORD_TO,
			/// TRUE keyword.
			KEYWORD_TRUE,
			/// USE keyword.
			KEYWORD_USE
		};

	public:

		/**
		 * Creates a new classic scanner by a given filename.
		 * @param filename The filename of the file to scan
		 * @param progress Optional scanner progress value
		 * @param cancel Optional scanner cancel state
		 * @see Scanner::Scanner().
		 */
		explicit ClassicScanner(const std::string& filename, float* progress = nullptr, bool* cancel = nullptr);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_CLASSIC_SCANNER_H
