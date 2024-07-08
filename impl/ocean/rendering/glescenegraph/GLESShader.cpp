/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESShader.h"

#ifndef GL_COMPUTE_SHADER // temporary solution until third-party has access to OpenGL ES 3.2
	#define GL_COMPUTE_SHADER 0x91B9
#endif

#ifndef GL_GEOMETRY_SHADER
	#define GL_GEOMETRY_SHADER 0x8DD9
#endif

#ifndef GL_TESS_CONTROL_SHADER
	#define GL_TESS_CONTROL_SHADER 0x8E88
#endif

#ifndef GL_TESS_EVALUATION_SHADER
	#define GL_TESS_EVALUATION_SHADER 0x8E87
#endif

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESShader::~GLESShader()
{
	if (id_ != 0u)
	{
		ocean_assert(GL_NO_ERROR == glGetError());

		GLint preValue = 0;
		glGetShaderiv(id_, GL_DELETE_STATUS, &preValue);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(preValue == GL_FALSE);

		glDeleteShader(id_);
		ocean_assert(GL_NO_ERROR == glGetError());

		id_ = 0u;
	}
}

bool GLESShader::compile(const GLenum type, const std::string& code, std::string& message)
{
	ocean_assert(type == GL_COMPUTE_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER || type == GL_TESS_CONTROL_SHADER || type == GL_TESS_EVALUATION_SHADER || type == GL_VERTEX_SHADER);

	return compile(type, {code.c_str()}, {GLint(code.length())}, message);
}

bool GLESShader::compile(const GLenum type, const std::vector<const GLchar*>& codes, const std::vector<GLint>& lengths, std::string& message)
{
	ocean_assert(type == GL_COMPUTE_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER || type == GL_TESS_CONTROL_SHADER || type == GL_TESS_EVALUATION_SHADER || type == GL_VERTEX_SHADER);

	if (codes.empty() || codes.size() != lengths.size())
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	if (id_ == 0u)
	{
		id_ = glCreateShader(type);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (id_ == 0u)
		{
			return false;
		}
	}

	glShaderSource(id_, GLsizei(codes.size()), (const GLchar**)(codes.data()), lengths.data());
	ocean_assert(GL_NO_ERROR == glGetError());

	glCompileShader(id_);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint shaderCompiled = 0;
	glGetShaderiv(id_, GL_COMPILE_STATUS, &shaderCompiled);
	ocean_assert(GL_NO_ERROR == glGetError());

	if (!shaderCompiled)
	{
		GLint infoLength = 0;
		glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &infoLength);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (infoLength > 1 && infoLength <= 4096)
		{
			message.resize(infoLength);
			glGetShaderInfoLog(id_, infoLength, nullptr, &message[0]);
			message.resize(infoLength - 1);
		}

		glDeleteShader(id_);
		id_ = 0u;

		ocean_assert(GL_NO_ERROR == glGetError());

		return false;
	}

	type_ = type;

	return true;
}

}

}

}
