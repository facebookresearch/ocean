/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ClassicScanner.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ClassicScanner::ClassicScanner(const std::string& filename, float* progress, bool* cancel) :
	Scanner(filename, std::string(), progress, cancel)
{
	registerSymbol(".", SYMBOL_DOT);
	registerSymbol("{", SYMBOL_NODE_BEGIN);
	registerSymbol("}", SYMBOL_NODE_END);
	registerSymbol("[", SYMBOL_FIELD_BEGIN);
	registerSymbol("]", SYMBOL_FIELD_END);

	registerKeyword("DEF", KEYWORD_DEF);
	registerKeyword("FALSE", KEYWORD_FALSE);
	registerKeyword("META", KEYWORD_META);
	registerKeyword("MFBool", KEYWORD_MF_BOOL);
	registerKeyword("MFColor", KEYWORD_MF_COLOR);
	registerKeyword("MFFloat", KEYWORD_MF_FLOAT);
	registerKeyword("MFInt32", KEYWORD_MF_INT32);
	registerKeyword("MFMatrix3f", KEYWORD_MF_MATRIX_3F);
	registerKeyword("MFMatrix4f", KEYWORD_MF_MATRIX_4F);
	registerKeyword("MFNode", KEYWORD_MF_NODE);
	registerKeyword("MFRotation", KEYWORD_MF_ROTATION);
	registerKeyword("MFString", KEYWORD_MF_STRING);
	registerKeyword("MFTime", KEYWORD_MF_TIME);
	registerKeyword("MFVec2f", KEYWORD_MF_VEC_2F);
	registerKeyword("MFVec3f", KEYWORD_MF_VEC_3F);
	registerKeyword("MFVec4f", KEYWORD_MF_VEC_4F);
	registerKeyword("NULL", KEYWORD_NULL);
	registerKeyword("PROFILE", KEYWORD_PROFILE);
	registerKeyword("ROUTE", KEYWORD_ROUTE);
	registerKeyword("SFBool", KEYWORD_SF_BOOL);
	registerKeyword("SFColor", KEYWORD_SF_COLOR);
	registerKeyword("SFFloat", KEYWORD_SF_FLOAT);
	registerKeyword("SFInt32", KEYWORD_SF_INT32);
	registerKeyword("SFMatrix3f", KEYWORD_SF_MATRIX_3F);
	registerKeyword("SFMatrix4f", KEYWORD_SF_MATRIX_4F);
	registerKeyword("SFNode", KEYWORD_SF_NODE);
	registerKeyword("SFRotation", KEYWORD_SF_ROTATION);
	registerKeyword("SFString", KEYWORD_SF_STRING);
	registerKeyword("SFTime", KEYWORD_SF_TIME);
	registerKeyword("SFVec2f", KEYWORD_SF_VEC_2F);
	registerKeyword("SFVec3f", KEYWORD_SF_VEC_3F);
	registerKeyword("SFVec4f", KEYWORD_SF_VEC_4F);
	registerKeyword("TO", KEYWORD_TO);
	registerKeyword("TRUE", KEYWORD_TRUE);
	registerKeyword("USE", KEYWORD_USE);

	registerLineRemark("#");
	registerScopeRemark("#/*", "*/#");

	registerWhiteSpaceCharacter(',');

	followingCharTable_['-'] = CHAR_IDENTIFIER;
}

}

}

}

}
