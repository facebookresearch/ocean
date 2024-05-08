/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/OBJScanner.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

OBJScanner::OBJScanner(const std::string& filename, float* progress, bool* cancel) :
	IO::Scanner(filename, std::string(), progress, cancel)
{
	registerKeyword("f", KEYWORD_F);
	registerKeyword("g", KEYWORD_G);
	registerKeyword("l", KEYWORD_L);
	registerKeyword("mtllib", KEYWORD_MTLLIB);
	registerKeyword("o", KEYWORD_O);
	registerKeyword("off", KEYWORD_OFF);
	registerKeyword("p", KEYWORD_P);
	registerKeyword("s", KEYWORD_S);
	registerKeyword("usemtl", KEYWORD_USEMTL);
	registerKeyword("v", KEYWORD_V);
	registerKeyword("vn", KEYWORD_VN);
	registerKeyword("vp", KEYWORD_VP);
	registerKeyword("vt", KEYWORD_VT);

	registerSymbol("/", SYMBOL_SLASH);

	registerLineRemark("#");

	registerWhiteSpaceCharacter('\\');
}

}

}

}

}
