/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MTL_SCANNER_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MTL_SCANNER_H

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
 * This class implements a scanner able to scan obj mtl token.
 * @ingroup scenedescriptionsdlobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT MtlScanner : public IO::Scanner
{
	public:

		/**
		 * Defines different obj mtl keywords.
		 */
		enum Keyword : uint32_t
		{
			/// Mtl keyword
			KEYWORD_BUMP,
			/// Mtl keyword
			KEYWORD_D,
			/// Mtl keyword 'illum' identifying the illumination model.
			KEYWORD_ILLUM,
			/// Mtl keyword 'Ka' identifying the ambient color.
			KEYWORD_KA,
			/// Mtl keyword 'Kd' identifying the diffuse color.
			KEYWORD_KD,
			/// Mtl keyword 'Ke' identifying the emissive color.
			KEYWORD_KE,
			/// Mtl keyword 'Ks' identifying the specular color.
			KEYWORD_KS,
			/// Mtl keyword
			KEYWORD_MAP_BUMP,
			/// Mtl keyword
			KEYWORD_MAP_D,
			/// Mtl keyword
			KEYWORD_MAP_KA,
			/// Mtl keyword
			KEYWORD_MAP_KD,
			/// Mtl keyword
			KEYWORD_MAP_KS,
			/// Mtl keyword
			KEYWORD_MAP_NS,
			/// Mtl keyword
			KEYWORD_MAP_OPACITY,
			/// Mtl keyword
			KEYWORD_MAP_REFL,
			/// Mtl keyword 'mtllib' identifying a material library.
			KEYWORD_NEWMTL,
			/// Mtl keyword
			KEYWORD_NI,
			/// Mtl keyword
			KEYWORD_NS,
			/// Mtl keyword
			KEYWORD_REFL,
			/// Mtl keyword
			KEYWORD_SHARPNESS,
			/// Mtl keyword
			KEYWORD_TF,
			/// Mtl keyword
			KEYWORD_TR
		};

	public:

		/**
		 * Creates a new mtl scanner.
		 * @param filename Name of the file to scan
		 */
		MtlScanner(const std::string& filename);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MTL_SCANNER_H
