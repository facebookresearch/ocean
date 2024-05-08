/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESTexture.h"

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

GLESShaderProgram::GLESShaderProgram() :
	id_(0),
	programType_(PT_UNKNOWN)
{
	// nothing to do here
}

GLESShaderProgram::~GLESShaderProgram()
{
	release();
}

bool GLESShaderProgram::link(const ProgramType programType, const GLESShaderRef& vertexShader, const GLESShaderRef& fragmentShader, std::string& message)
{
	if ((vertexShader.isNull() || vertexShader->isNull()) || (fragmentShader.isNull() || fragmentShader->isNull()))
	{
		return false;
	}

	return link(programType, {vertexShader, fragmentShader}, message);
}

bool GLESShaderProgram::link(const ProgramType programType, const std::vector<GLESShaderRef>& shaders, std::string& message)
{
	if (shaders.empty())
	{
		return false;
	}

	release();

	id_ = glCreateProgram();
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLboolean isProgram = glIsProgram(id_);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert_and_suppress_unused(isProgram == GL_TRUE, isProgram);

	for (const GLESShaderRef& shader : shaders)
	{
		if (shader.isNull() || shader->isNull())
		{
			return false;
		}

		glAttachShader(id_, shader->id());
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	glLinkProgram(id_);
	ocean_assert(GL_NO_ERROR == glGetError());

	GLint programLinked = 0;
	glGetProgramiv(id_, GL_LINK_STATUS, &programLinked);
	ocean_assert(GL_NO_ERROR == glGetError());
	if (!programLinked)
	{
		GLint infoLength = 0;
		glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &infoLength);

		if (infoLength > 1 && infoLength <= 4096)
		{
			message.resize(infoLength);
			glGetProgramInfoLog(id_, infoLength, nullptr, &message[0]);
		}

		release();
		return false;
	}

	shaders_ = shaders;

	programType_ = programType;

	return true;
}

bool GLESShaderProgram::compileAndLink(const ProgramType programType, const std::vector<const char*>& vertexShaderCode, const std::vector<const char*>& fragmentShaderCode, std::string& message)
{
	const ShaderCodePairs shaderCodePairs =
	{
		ShaderCodePair(GL_VERTEX_SHADER, vertexShaderCode),
		ShaderCodePair(GL_FRAGMENT_SHADER, fragmentShaderCode)
	};

	return compileAndLink(programType, shaderCodePairs, message);
}

bool GLESShaderProgram::compileAndLink(const ProgramType programType, const ShaderCodePairs& shaderCodePairs, std::string& message)
{
	ocean_assert(programType != PT_UNKNOWN);

	std::vector<GLint> lengths;
	lengths.reserve(8);

	std::vector<GLESShaderRef> shaders;

	for (const ShaderCodePair& shaderCodePair : shaderCodePairs)
	{
		lengths.clear();

		for (const char* vertexCode : shaderCodePair.second)
		{
			lengths.emplace_back(int(strlen(vertexCode)));
		}

		const GLenum shaderType = shaderCodePair.first;

		GLESShaderRef shader(new GLESShader());
		if (shader->compile(shaderType, shaderCodePair.second, lengths, message) == false)
		{
			Log::error() << "Failed to compile " + translateShaderType(shaderType) + ": " << message;
			return false;
		}

		shaders.emplace_back(std::move(shader));
	}

	if (link(programType, shaders, message) == false)
	{
		return false;
	}

	ocean_assert(programType_ == programType);

	return true;
}

void GLESShaderProgram::bind(const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix) const
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (id_ == 0)
	{
		return;
	}

	ocean_assert(programType_ != PT_UNKNOWN);

	ocean_assert(glIsProgram(id_));
	glUseProgram(id_);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint projectLocation = glGetUniformLocation(id_, "projectionMatrix");
	if (projectLocation != -1)
	{
		GLESObject::setUniform(projectLocation, projection);
	}

	const GLint modelViewMatrixLocation = glGetUniformLocation(id_, "modelViewMatrix");
	if (modelViewMatrixLocation != -1)
	{
		ocean_assert(camera_T_model.isValid());
		GLESObject::setUniform(modelViewMatrixLocation, camera_T_model);
	}

	const GLint viewMatrixLocation = glGetUniformLocation(id_, "viewMatrix");
	if (viewMatrixLocation != -1)
	{
		ocean_assert(camera_T_world.isValid());
		GLESObject::setUniform(viewMatrixLocation, camera_T_world);
	}

	const GLint normalMatrixLocation = glGetUniformLocation(id_, "normalMatrix");
	if (normalMatrixLocation != -1)
	{
		ocean_assert(!normalMatrix.isSingular());
		GLESObject::setUniform(normalMatrixLocation, normalMatrix);
	}

	const GLint locationLightingTwoSided = glGetUniformLocation(id_, "lightingTwoSided");
	if (locationLightingTwoSided != -1)
	{
		GLESObject::setUniform(locationLightingTwoSided, 0);
	}

	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESShaderProgram::bind(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix)
{
	bind(projection, camera_T_model, camera_T_world, normalMatrix);

	ocean_assert(GL_NO_ERROR == glGetError());

	bindAttribute(framebuffer, *this);

	ocean_assert(GL_NO_ERROR == glGetError());
}

GLESShaderProgram::ProgramType GLESShaderProgram::necessaryShader() const
{
	return programType_;
}

void GLESShaderProgram::bindAttribute(const GLESFramebuffer& /*framebuffer*/, GLESShaderProgram& shaderProgram)
{
	if (programType_ != PT_CUSTOM)
	{
		return;
	}

	ocean_assert_and_suppress_unused(shaderProgram.isCompiled(), shaderProgram);

	for (const std::pair<SamplerIndex, TextureRef> samplerPair : samplers_)
	{
		const SmartObjectRef<GLESTexture> texture(samplerPair.second);
		ocean_assert(texture);

		texture->bindTexture(shaderProgram, samplerPair.first);
	}

	parametersFloat_.bindValues(shaderProgram.id());
	parametersFloats_.bindValues(shaderProgram.id());
	parametersInt_.bindValues(shaderProgram.id());
	parametersUnsignedInt_.bindValues(shaderProgram.id());
	parametersHomogenousMatrix4_.bindValues(shaderProgram.id());
	parametersSquareMatrix3_.bindValues(shaderProgram.id());
	parametersSquareMatrix4_.bindValues(shaderProgram.id());
	parametersVector2_.bindValues(shaderProgram.id());
	parametersVector3_.bindValues(shaderProgram.id());
	parametersVector4_.bindValues(shaderProgram.id());
	parametersHomogenousMatrices4_.bindValues(shaderProgram.id());
	parametersSquareMatrices3_.bindValues(shaderProgram.id());
}

bool GLESShaderProgram::setShader(const ShaderLanguage shaderLanguage, const FilenamePairs& filenamePairs, std::string& errorMessage)
{
	if (shaderLanguage != SL_GLSL)
	{
		errorMessage = "Wrong shader language, needs to be GLSL";
		return false;
	}

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	for (const FilenamePair& filenamePair : filenamePairs)
	{
		if (filenamePair.second == ST_VERTEX)
		{
			if (!vertexShaderCode.empty())
			{
				errorMessage = "Cannot use two vertex shaders";
				return false;
			}

			std::ifstream fileStream(filenamePair.first);

			if (fileStream.is_open())
			{
				vertexShaderCode = std::string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
			}
			else
			{
				errorMessage = "Could not load vertex shader file '" + filenamePair.first + "'";
				return false;
			}
		}
		else if (filenamePair.second == ST_FRAGMENT)
		{
			if (!fragmentShaderCode.empty())
			{
				errorMessage = "Cannot use two fragment shaders";
				return false;
			}

			std::ifstream fileStream(filenamePair.first);

			if (fileStream.is_open())
			{
				fragmentShaderCode = std::string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
			}
			else
			{
				errorMessage = "Could not load fragment shader file '" + filenamePair.first + "'";
				return false;
			}
		}
		else
		{
			errorMessage = "Invalid shader type, needs to be either vertex or fragment shader";
			return false;
		}
	}

	if (vertexShaderCode.empty() || fragmentShaderCode.empty())
	{
		errorMessage = "Need one vertex shader and one fragment shader";
		return false;
	}

	return compileAndLink(PT_CUSTOM, {vertexShaderCode.c_str()}, {fragmentShaderCode.c_str()}, errorMessage);
}

bool GLESShaderProgram::setShader(const ShaderLanguage shaderLanguage, const std::string& vertexShaderCode, const std::string& fragmentShaderCode, std::string& errorMessage)
{
	if (shaderLanguage != SL_GLSL)
	{
		errorMessage = "Wrong shader language, needs to be GLSL";
		return false;
	}

	if (vertexShaderCode.empty() || fragmentShaderCode.empty())
	{
		errorMessage = "Need one vertex shader and one fragment shader";
		return false;
	}

	return compileAndLink(PT_CUSTOM, {vertexShaderCode.c_str()}, {fragmentShaderCode.c_str()}, errorMessage);
}

bool GLESShaderProgram::setShader(const ShaderLanguage shaderLanguage, const std::vector<const char*>& vertexShaderCode, const std::vector<const char*>& fragmentShaderCode, std::string& errorMessage)
{
	if (shaderLanguage != SL_GLSL)
	{
		errorMessage = "Wrong shader language, needs to be GLSL";
		return false;
	}

	if (vertexShaderCode.empty() || fragmentShaderCode.empty())
	{
		errorMessage = "Need one vertex shader and one fragment shader";
		return false;
	}

	return compileAndLink(PT_CUSTOM, vertexShaderCode, fragmentShaderCode, errorMessage);
}

bool GLESShaderProgram::setShader(const ShaderLanguage shaderLanguage, const CodePairs& codePairs, std::string& errorMessage)
{
	if (shaderLanguage != SL_GLSL)
	{
		errorMessage = "Wrong shader language, needs to be GLSL";
		return false;
	}

	if (codePairs.empty())
	{
		errorMessage = "Need at least one shader code";
		return false;
	}

	ShaderCodePairs shaderCodePairs;
	shaderCodePairs.reserve(codePairs.size());

	for (const CodePair& codePair : codePairs)
	{
		GLenum shaderType = 0u;

		switch (codePair.second)
		{
			case ST_COMPUTE:
				shaderType = GL_COMPUTE_SHADER;
				break;

			case ST_FRAGMENT:
				shaderType = GL_FRAGMENT_SHADER;
				break;

			case ST_GEOMETRY:
				shaderType = GL_GEOMETRY_SHADER;
				break;

			case ST_TESSELLATION_CONTROL:
				shaderType = GL_TESS_CONTROL_SHADER;
				break;

			case ST_TESSELLATION_EVALUATION:
				shaderType = GL_TESS_EVALUATION_SHADER;
				break;

			case ST_VERTEX:
				shaderType = GL_VERTEX_SHADER;
				break;

			case ST_UNIFIED:
				errorMessage = "OpenGL does not support a unified shader";
				return false;

			case ST_INVALID:
				break;
		}

		if (shaderType == 0u)
		{
			errorMessage = "Invalid shader type";
			return false;
		}

		shaderCodePairs.emplace_back(shaderType, codePair.first);
	}

	return compileAndLink(PT_CUSTOM, shaderCodePairs, errorMessage);
}

bool GLESShaderProgram::existParameter(const std::string& name) const
{
	ocean_assert(isCompiled());

	const GLint location = glGetUniformLocation(id(), name.c_str());

	return location != -1;
}

GLESShaderProgram::ParameterType GLESShaderProgram::parameterType(const std::string& /*name*/) const
{
	ocean_assert(false && "**TODO**");
	return TYPE_INVALID;
}

unsigned int GLESShaderProgram::samplerNumber() const
{
	return (unsigned int)(samplers_.size());
}

GLESShaderProgram::SamplerIndex GLESShaderProgram::samplerIndex(const TextureRef& sampler) const
{
	for (const std::pair<SamplerIndex, TextureRef> samplerPair : samplers_)
	{
		if (samplerPair.second == sampler)
		{
			return samplerPair.first;
		}
	}

	return (unsigned int)(-1);
}

unsigned int GLESShaderProgram::parameterElements(const std::string& /*name*/) const
{
	ocean_assert(false && "**TODO**");
	return 0u;
}

bool GLESShaderProgram::setSampler(const SamplerIndex index, const TextureRef& texture)
{
	samplers_[index] = texture;

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const double value)
{
	parametersFloat_.setValue(name, float(value));

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const float value)
{
	parametersFloat_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const float* value, const unsigned int elements)
{
	std::vector<float> vectorValue = std::vector<float>(size_t(elements));
	memcpy(vectorValue.data(), value, sizeof(float) * elements);

	parametersFloats_.setValue(name, std::move(vectorValue));

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const int value)
{
	parametersInt_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const unsigned int value)
{
	parametersUnsignedInt_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const HomogenousMatrix4& value)
{
	parametersHomogenousMatrix4_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const SquareMatrix3& value)
{
	parametersSquareMatrix3_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const SquareMatrix4& value)
{
	parametersSquareMatrix4_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const Vector2& value)
{
	parametersVector2_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const Vector3& value)
{
	parametersVector3_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const Vector4& value)
{
	parametersVector4_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const HomogenousMatrices4& value)
{
	parametersHomogenousMatrices4_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::setParameter(const std::string& name, const SquareMatrices3& value)
{
	parametersSquareMatrices3_.setValue(name, value);

	return true;
}

bool GLESShaderProgram::isCompiled() const
{
	return !shaders_.empty();
}

std::string GLESShaderProgram::translateShaderType(const GLenum shaderType)
{
	switch (shaderType)
	{
		case GL_COMPUTE_SHADER:
			return std::string("Compute Shader");

		case GL_FRAGMENT_SHADER:
			return std::string("Fragment Shader");

		case GL_GEOMETRY_SHADER:
			return std::string("Geometry Shader");

		case GL_TESS_CONTROL_SHADER:
			return std::string("Tessellation Control Shader");

		case GL_TESS_EVALUATION_SHADER:
			return std::string("Tessellation Evaluation Shader");

		case GL_VERTEX_SHADER:
			return std::string("Vertex Shader");

		default:
			break;
	}

	ocean_assert(false && "Invalid shader type!");
	return std::string("Unknown");
}

void GLESShaderProgram::release()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (id_ != 0)
	{
		const GLboolean isProgram = glIsProgram(id_);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert_and_suppress_unused(isProgram == GL_TRUE, isProgram);

		for (GLESShaderRef& shader : shaders_)
		{
			ocean_assert(shader);

			glDetachShader(id_, shader->id());
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		shaders_.clear();
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	if (id_ != 0)
	{
		ocean_assert(GL_NO_ERROR == glGetError());

		GLint preValue = 0;
		glGetProgramiv(id_, GL_DELETE_STATUS, &preValue);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(preValue == GL_FALSE);

		glDeleteProgram(id_);
		ocean_assert(GL_NO_ERROR == glGetError());

		id_ = 0;
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	programType_ = PT_UNKNOWN;
}

}

}

}
