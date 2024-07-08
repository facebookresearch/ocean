/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESAttribute::GLESAttribute() :
	GLESObject(),
	Attribute()
{
	// nothing to do here
}

GLESAttribute::~GLESAttribute()
{
	// nothing to do here
}

void GLESAttribute::unbindAttribute()
{
	// nothing to do here
}

GLESAttribute::ProgramType GLESAttribute::necessaryShader() const
{
	return PT_UNKNOWN;
}

std::string GLESAttribute::translateProgramType(const GLESAttribute::ProgramType programType)
{
	if (programType == PT_UNKNOWN)
	{
		return "PT_UNKNOWN";
	}

	std::string result;

	if ((programType & PT_STATIC_COLOR) != 0)
	{
		result += "PT_STATIC_COLOR | ";
	}

	if ((programType & PT_POINTS) != 0)
	{
		result += "PT_POINTS | ";
	}

	if ((programType & PI_DEBUG_GRAY) != 0)
	{
		result += "PI_DEBUG_GRAY | ";
	}

	if ((programType & PT_MATERIAL) != 0)
	{
		result += "PT_MATERIAL | ";
	}

	if ((programType & PT_LIGHT) != 0)
	{
		result += "PT_LIGHT | ";
	}

	if ((programType & PT_TEXTURE_LOWER_LEFT) != 0)
	{
		result += "PT_TEXTURE_LOWER_LEFT | ";
	}

	if ((programType & PT_TEXTURE_UPPER_LEFT) != 0)
	{
		result += "PT_TEXTURE_UPPER_LEFT | ";
	}

	if ((programType & PT_TEXTURE_RGBA) != 0)
	{
		result += "PT_TEXTURE_RGBA | ";
	}

	if ((programType & PT_TEXTURE_BGRA) != 0)
	{
		result += "PT_TEXTURE_BGRA | ";
	}

	if ((programType & PT_TEXTURE_YUV24) != 0)
	{
		result += "PT_TEXTURE_YUV24 | ";
	}

	if ((programType & PT_TEXTURE_YVU24) != 0)
	{
		result += "PT_TEXTURE_YVU24 | ";
	}

	if ((programType & PT_TEXTURE_Y_VU12) != 0)
	{
		result += "PT_TEXTURE_Y_VU12 | ";
	}

	if ((programType & PT_TEXTURE_Y_U_V12) != 0)
	{
		result += "PT_TEXTURE_Y_U_V12 | ";
	}

	if ((programType & PT_TEXTURE_Y_UV12) != 0)
	{
		result += "PT_TEXTURE_Y_UV12 | ";
	}

	if ((programType & PT_TEXTURES) != 0)
	{
		result += "PT_TEXTURES | ";
	}

	if ((programType & PT_PHANTOM_VIDEO_FAST) != 0)
	{
		result += "PT_PHANTOM_VIDEO_FAST | ";
	}

	if ((programType & PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST) != 0)
	{
		result += "PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | ";
	}

	if ((programType & PT_TEXT_TRANSPARENT_Y) != 0)
	{
		result += "PT_TEXT_TRANSPARENT_Y | ";
	}

	if ((programType & PT_TEXT_OPAQUE_Y) != 0)
	{
		result += "PT_TEXT_OPAQUE_Y | ";
	}

	if ((programType & PT_COLOR_ID) != 0)
	{
		result += "PT_COLOR_ID | ";
	}

	if ((programType & PT_PENDING) != 0)
	{
		result += "PT_PENDING | ";
	}

	ocean_assert(result.size() >= 3);
	if (result.size() > 3)
	{
		ocean_assert(result[result.size() - 1] == ' ' && result[result.size() - 2] == '|' && result[result.size() - 3] == ' ');
		result.resize(result.size() - 3);
	}

	return result;
}

}

}

}
