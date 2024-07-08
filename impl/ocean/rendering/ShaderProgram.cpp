/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/ShaderProgram.h"

namespace Ocean
{

namespace Rendering
{

ShaderProgram::ShaderProgram() :
	Attribute()
{
	// nothing to do here
}

ShaderProgram::~ShaderProgram()
{
	for (Textures::const_iterator i = shaderProgramTextures.begin(); i != shaderProgramTextures.end(); ++i)
	{
		unregisterThisObjectAsParent(*i);
	}
}

bool ShaderProgram::setShader(const ShaderLanguage /*shaderLanguage*/, const FilenamePairs& /*filenamePairs*/, std::string& /*errorMessage*/)
{
	throw NotSupportedException("ShaderProgram::setShader() is not supported.");
}

bool ShaderProgram::setShader(const ShaderLanguage /*shaderLanguage*/, const std::string& /*vertexShader*/, const std::string& /*fragmentShader*/, std::string& /*errorMessage*/)
{
	throw NotSupportedException("ShaderProgram::setShader() is not supported.");
}

bool ShaderProgram::setShader(const ShaderLanguage /*shaderLanguage*/, const std::vector<const char*>& /*vertexShaderCode*/, const std::vector<const char*>& /*fragmentShaderCode*/, std::string& /*errorMessage*/)
{
	throw NotSupportedException("ShaderProgram::setShader() is not supported.");
}

bool ShaderProgram::setShader(const ShaderLanguage /*shaderLanguage*/, const CodePairs& /*codePairs*/, std::string& /*errorMessage*/)
{
	throw NotSupportedException("ShaderProgram::setShader() is not supported.");
}

bool ShaderProgram::existParameter(const std::string& /*name*/) const
{
	throw NotSupportedException("ShaderProgram::existParameter() is not supported.");
}

ShaderProgram::ParameterType ShaderProgram::parameterType(const std::string& /*name*/) const
{
	throw NotSupportedException("ShaderProgram::parameterType() is not supported.");
}

unsigned int ShaderProgram::samplerNumber() const
{
	return (unsigned int)(shaderProgramTextures.size());
}

ShaderProgram::SamplerIndex ShaderProgram::samplerIndex(const TextureRef& sampler) const
{
	for (size_t n = 0; n < shaderProgramTextures.size(); ++n)
	{
		if (shaderProgramTextures[n] == sampler)
		{
			return SamplerIndex(n);
		}
	}

	return invalidSamplerIndex;
}

unsigned int ShaderProgram::parameterElements(const std::string& /*name*/) const
{
	throw NotSupportedException("ShaderProgram::parameterElements() is not supported.");
}

bool ShaderProgram::setSampler(const SamplerIndex index, const TextureRef& texture)
{
	if (index >= shaderProgramTextures.size())
	{
		shaderProgramTextures.resize(index + 1u);
	}

	if (shaderProgramTextures[index])
	{
		unregisterThisObjectAsParent(shaderProgramTextures[index]);
	}

	registerThisObjectAsParent(texture);
	shaderProgramTextures[index] = texture;

	return true;
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const double /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const float /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const float* /*value*/, const unsigned int /*elements*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const int /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const unsigned int /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const HomogenousMatrix4& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const SquareMatrix3& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const SquareMatrix4& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const Vector2& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const Vector3& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const Vector4& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const HomogenousMatrices4& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::setParameter(const std::string& /*name*/, const SquareMatrices3& /*value*/)
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

bool ShaderProgram::isCompiled() const
{
	throw NotSupportedException("ShaderProgram::setParameter() is not supported.");
}

ShaderProgram::ObjectType ShaderProgram::type() const
{
	return TYPE_SHADER_PROGRAM;
}

}

}
