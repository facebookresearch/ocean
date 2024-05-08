/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_SCANNER_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_SCANNER_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"

#include "ocean/io/Scanner.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

/**
 * This class implements a scanner able to scan obj token.
 * @ingroup scenedescriptionsdlobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT OBJScanner : public IO::Scanner
{
	public:

		/**
		 * Defines different obj keywords.
		 */
		enum Keyword
		{
			/// OBJ keyword 'f' identifying a face.
			KEYWORD_F,
			/// OBJ keyword 'g' identifying a group.
			KEYWORD_G,
			/// OBJ keyword 'l' identifying a line.
			KEYWORD_L,
			/// OBJ keyword 'mtllib' identifying a material library.
			KEYWORD_MTLLIB,
			/// OBJ keyword
			KEYWORD_O,
			/// OBJ keyword
			KEYWORD_OFF,
			/// OBJ keyword 'p' identifying a point.
			KEYWORD_P,
			/// OBJ keyword 's' identifying a smoothing factor.
			KEYWORD_S,
			/// OBJ keyword 'usemtl' identifying a 'use material state'.
			KEYWORD_USEMTL,
			/// OBJ keyword 'v' identifying a vertex.
			KEYWORD_V,
			/// OBJ keyword 'vn' identifying a normal.
			KEYWORD_VN,
			/// OBJ keyword 'vp' identifiying a parameter.
			KEYWORD_VP,
			/// OBJ keyword 'vt' identifying a texture coordinate.
			KEYWORD_VT
		};

		/**
		 * Defines different obj symbols.
		 */
		enum Symbol
		{
			/// OBJ symbol
			SYMBOL_SLASH
		};

	public:

		/**
		 * Creates a new obj scanner.
		 * @param filename Name of the file to scan
		 * @param progress Optional progress state receiving the parsing progress recurrently
		 * @param cancel Optional cancel state
		 * @see Scanner::Scanner().
		 */
		explicit OBJScanner(const std::string& filename, float* progress = nullptr, bool* cancel = nullptr);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ__H
