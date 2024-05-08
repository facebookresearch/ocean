/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/ShaderProgram.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

ShaderProgram::~ShaderProgram()
{
	release();
}

bool ShaderProgram::createProgram(const std::string& vertexCode, const std::string& fragmentCode, std::string* errorMessage)
{
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	if (programId != 0u)
	{
		release();
	}

	ocean_assert(programId == 0u);
	ocean_assert(programVertexShaderId == 0u);
	ocean_assert(programFragmentShaderId == 0u);

	std::string errorMessageVertexShader;
	if (!vertexCode.empty() && !createShader(vertexCode, programVertexShaderId, GL_VERTEX_SHADER, &errorMessageVertexShader))
	{
		if (errorMessage)
		{
			*errorMessage = std::move(errorMessageVertexShader);
		}

		ocean_assert(false && "Invalid shader");
		return false;
	}

	std::string errorMessageFragmentShader;
	if (!vertexCode.empty() && !createShader(fragmentCode, programFragmentShaderId, GL_FRAGMENT_SHADER, &errorMessageFragmentShader))
	{
		if (errorMessage)
		{
			*errorMessage = std::move(errorMessageFragmentShader);
		}

		ocean_assert(false && "Invalid shader");
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	programId = associatedContext_->glCreateProgram();
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLboolean isProgram = associatedContext_->glIsProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert_and_suppress_unused(isProgram == GL_TRUE, isProgram);

	associatedContext_->glAttachShader(programId, programVertexShaderId);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glAttachShader(programId, programFragmentShaderId);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glLinkProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint programLinked = 0;
	associatedContext_->glGetProgramiv(programId, GL_LINK_STATUS, &programLinked);
	ocean_assert(GL_NO_ERROR == glGetError());
	if (!programLinked)
	{
		GLint infoLength = 0;
		associatedContext_->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);

		if (infoLength > 1 && infoLength <= 4096 && errorMessage)
		{
			std::vector<char> buffer(infoLength);
			associatedContext_->glGetProgramInfoLog(programId, infoLength, nullptr, buffer.data());

			*errorMessage = std::string(buffer.data(), infoLength - 1);

			if (!errorMessageVertexShader.empty())
			{
				*errorMessage += "\nWith vertex shader error:\n\r";
				*errorMessage += errorMessageVertexShader;
			}

			if (!errorMessageFragmentShader.empty())
			{
				*errorMessage += "\nWith fragment shader error:\n\r";
				*errorMessage += errorMessageFragmentShader;
			}
		}

		release();
		ocean_assert(false && "Invalid shader");
		return false;
	}

	return true;
}

bool ShaderProgram::bind()
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	if (programId == 0u)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool ShaderProgram::setUniform(const std::string& attributeName, const int value)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(glGetError() == GL_NO_ERROR);
	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(location != -1);
	if (location == -1)
	{
		return false;
	}

	associatedContext_->glUniform1i(location, value);
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool ShaderProgram::setUniform(const std::string& attributeName, const float value)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(glGetError() == GL_NO_ERROR);
	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(location != -1);
	if (location == -1)
	{
		return false;
	}

	associatedContext_->glUniform1fv(location, 1, &value);
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool ShaderProgram::setUniform(const std::string& attributeName, const VectorF2& value)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(glGetError() == GL_NO_ERROR);
	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(location != -1);
	if (location == -1)
	{
		return false;
	}

	associatedContext_->glUniform2fv(location, 1, value.data());
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool ShaderProgram::setUniform(const std::string& attributeName, const VectorF3& value)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(glGetError() == GL_NO_ERROR);
	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(location != -1);
	if (location == -1)
	{
		return false;
	}

	associatedContext_->glUniform3fv(location, 1, value.data());
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool ShaderProgram::setUniform(const std::string& attributeName, const VectorF4& value)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(glGetError() == GL_NO_ERROR);
	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(location != -1);
	if (location == -1)
	{
		return false;
	}

	associatedContext_->glUniform4fv(location, 1, value.data());
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool ShaderProgram::setUniform(const std::string& attributeName, const SquareMatrixF4& value)
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(associatedContext_->glIsProgram(programId));
	associatedContext_->glUseProgram(programId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(glGetError() == GL_NO_ERROR);
	const GLint location = associatedContext_->glGetUniformLocation(programId, attributeName.c_str());
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(location != -1);
	if (location == -1)
	{
		return false;
	}

	associatedContext_->glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

bool ShaderProgram::release()
{
	if (programVertexShaderId != 0u)
	{
		ocean_assert(associatedContext_);
		if (!associatedContext_)
		{
			return false;
		}

		ocean_assert(GL_NO_ERROR == glGetError());

		const GLboolean isProgram = associatedContext_->glIsProgram(programId);
		ocean_assert_and_suppress_unused(isProgram == GL_TRUE, isProgram);
		ocean_assert(GL_NO_ERROR == glGetError());

		associatedContext_->glDetachShader(programId, programVertexShaderId);
		ocean_assert(GL_NO_ERROR == glGetError());

		GLint preValue = 0;
		associatedContext_->glGetShaderiv(programVertexShaderId, GL_DELETE_STATUS, &preValue);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(preValue == GL_FALSE);

		associatedContext_->glDeleteShader(programVertexShaderId);
		ocean_assert(GL_NO_ERROR == glGetError());

		programVertexShaderId = 0u;
	}

	if (programFragmentShaderId != 0u)
	{
		ocean_assert(associatedContext_);
		if (!associatedContext_)
		{
			return false;
		}

		ocean_assert(GL_NO_ERROR == glGetError());

		const GLboolean isProgram = associatedContext_->glIsProgram(programId);
		ocean_assert_and_suppress_unused(isProgram == GL_TRUE, isProgram);
		ocean_assert(GL_NO_ERROR == glGetError());

		associatedContext_->glDetachShader(programId, programFragmentShaderId);
		ocean_assert(GL_NO_ERROR == glGetError());

		GLint preValue = 0;
		associatedContext_->glGetShaderiv(programFragmentShaderId, GL_DELETE_STATUS, &preValue);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(preValue == GL_FALSE);

		associatedContext_->glDeleteShader(programFragmentShaderId);
		ocean_assert(GL_NO_ERROR == glGetError());

		programFragmentShaderId = 0u;
	}

	if (programId != 0u)
	{
		ocean_assert(associatedContext_);
		if (!associatedContext_)
		{
			return false;
		}

		ocean_assert(GL_NO_ERROR == glGetError());

		GLint preValue = 0;
		associatedContext_->glGetProgramiv(programId, GL_DELETE_STATUS, &preValue);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(preValue == GL_FALSE);

		associatedContext_->glDeleteProgram(programId);
		ocean_assert(GL_NO_ERROR == glGetError());

		programId = 0u;
	}

	return true;
}

bool ShaderProgram::createShader(const std::string& code, GLuint& shaderId, const GLenum shaderType, std::string* errorMessage)
{
	if (!associatedContext_)
	{
		return false;
	}

	ocean_assert(shaderId == 0u);
	ocean_assert(GL_NO_ERROR == glGetError());

	shaderId = associatedContext_->glCreateShader(shaderType);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLchar* codeString = code.c_str();
	const GLint codeLength = GLint(code.length());

	associatedContext_->glShaderSource(shaderId, 1, &codeString, &codeLength);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glCompileShader(shaderId);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint shaderCompiled = 0;
	associatedContext_->glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderCompiled);
	ocean_assert(GL_NO_ERROR == glGetError());

	if (!shaderCompiled)
	{
		GLint infoLength = 0;
		associatedContext_->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLength);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (errorMessage && infoLength > 1 && infoLength <= 4096)
		{
			std::vector<char> buffer(infoLength);
			associatedContext_->glGetShaderInfoLog(shaderId, infoLength, nullptr, buffer.data());

			*errorMessage = std::string(buffer.data(), infoLength - 1);
		}

		associatedContext_->glDeleteShader(shaderId);
		shaderId = 0u;
		ocean_assert(GL_NO_ERROR == glGetError());
		return false;
	}

	return true;
}

}

}

}
