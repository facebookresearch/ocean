/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/MtlScanner.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

MtlScanner::MtlScanner(const std::string& filename) :
	Scanner(filename, std::string(), nullptr, nullptr)
{
	setKeywordProperty(false);

	registerKeyword("bump", KEYWORD_BUMP);
	registerKeyword("d", KEYWORD_D);
	registerKeyword("illum", KEYWORD_ILLUM);
	registerKeyword("Ka", KEYWORD_KA);
	registerKeyword("Kd", KEYWORD_KD);
	registerKeyword("Ke", KEYWORD_KE);
	registerKeyword("Ks", KEYWORD_KS);
	registerKeyword("map_bump", KEYWORD_MAP_BUMP);
	registerKeyword("map_d", KEYWORD_MAP_D);
	registerKeyword("map_Ka", KEYWORD_MAP_KA);
	registerKeyword("map_Kd", KEYWORD_MAP_KD);
	registerKeyword("map_Ks", KEYWORD_MAP_KS);
	registerKeyword("map_Ns", KEYWORD_MAP_NS);
	registerKeyword("map_opacity", KEYWORD_MAP_OPACITY);
	registerKeyword("map_refl", KEYWORD_MAP_REFL);
	registerKeyword("newmtl", KEYWORD_NEWMTL);
	registerKeyword("Ni", KEYWORD_NI);
	registerKeyword("Ns", KEYWORD_NS);
	registerKeyword("refl", KEYWORD_REFL);
	registerKeyword("sharpness", KEYWORD_SHARPNESS);
	registerKeyword("Tf", KEYWORD_TF);
	registerKeyword("Tr", KEYWORD_TR);

	registerLineRemark("#");

	registerWhiteSpaceCharacter('\\');
}

}

}

}

}
