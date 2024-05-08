/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ComposedShader.h"
#include "ocean/scenedescription/sdx/x3d/ShaderPart.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Textures.h"

#include "ocean/io/FileResolver.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ComposedShader::ComposedShader(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DProgrammableShaderObject(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DShaderNode(environment),
	SDXDynamicNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ComposedShader::NodeSpecification ComposedShader::specifyNode()
{
	NodeSpecification specification("ComposedShader");

	X3DProgrammableShaderObject::registerFields(specification);
	X3DShaderNode::registerFields(specification);

	registerField(specification, "parts", parts_, ACCESS_GET_SET);

	return specification;
}

void ComposedShader::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DShaderNode::onInitialize(scene, timestamp);
	X3DProgrammableShaderObject::onInitialize(scene, timestamp);

	if (language_.value() != "GLSL")
	{
		Log::warning() << "Up to now ComposedShader with \"GLSL\" shading language are supported.";
		return;
	}

	if (parts_.values().empty() == false)
	{
		Rendering::ShaderProgramRef shaderProgram = engine()->factory().createShaderProgram();

		if (shaderProgram)
		{
			Rendering::ShaderProgram::FilenamePairs filenamePairs;
			filenamePairs.reserve(2);

			for (const NodeRef& part : parts_.values())
			{
				const SDXNodeRef childNode(part);

				if (childNode->type() == "ShaderPart")
				{
					Rendering::ShaderProgram::FilenamePair filenamePair = childNode.force<ShaderPart>().filenamePair();

					if (!filenamePair.first.empty())
					{
						filenamePairs.emplace_back(std::move(filenamePair));
					}
				}
			}

			std::string errorMessage;
			if (shaderProgram->setShader(Rendering::ShaderProgram::SL_GLSL, filenamePairs, errorMessage))
			{
				ocean_assert(shaderProgram->isCompiled());

				for (unsigned int n = 0u; n < dynamicFields(); ++n)
				{
					applyParameter(shaderProgram, dynamicFieldName(n));
				}

				renderingObject_ = std::move(shaderProgram);
			}
			else
			{
				std::string resolvedFilesString;

				for (const Rendering::ShaderProgram::FilenamePair& filenamePair : filenamePairs)
				{
					if (resolvedFilesString.empty())
					{
						resolvedFilesString += "'" + filenamePair.first + "'";
					}
					else
					{
						resolvedFilesString += ", or '" + filenamePair.first + "'";
					}
				}

				Log::error() << "Failed to compile and link the shader code files " << resolvedFilesString << ": " << errorMessage;
			}
		}
	}
}

void ComposedShader::onFieldChanged(const std::string& fieldName)
{
	const Rendering::ShaderProgramRef renderingShaderProgram(renderingObject_);

	if (renderingShaderProgram)
	{
		if (hasDynamicField(fieldName))
		{
			applyParameter(renderingShaderProgram, fieldName);
			return;
		}
	}

	X3DShaderNode::onFieldChanged(fieldName);
	X3DProgrammableShaderObject::onFieldChanged(fieldName);
}

bool ComposedShader::applyParameter(const Rendering::ShaderProgramRef& shaderProgram, const std::string& fieldName)
{
	ocean_assert(shaderProgram);

	if (shaderProgram->existParameter(fieldName))
	{
		const Field& field = dynamicField(fieldName);

		if (field.is0D())
		{
			switch (field.type())
			{
				case Field::TYPE_BOOLEAN:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleBool>(field).value());

				case Field::TYPE_COLOR:
				{
					const RGBAColor& color = Field::cast<SingleColor>(field).value();
					return shaderProgram->setParameter(fieldName, Vector4(color.red(), color.green(), color.blue(), color.alpha()));
				}

				case Field::TYPE_FLOAT:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleFloat>(field).value());

				case Field::TYPE_INT:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleInt>(field).value());

				case Field::TYPE_MATRIX3:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleMatrix3>(field).value());

				case Field::TYPE_MATRIX4:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleMatrix4>(field).value());

				case Field::TYPE_TIME:
					return shaderProgram->setParameter(fieldName, double(Field::cast<SingleTime>(field).value()));

				case Field::TYPE_VECTOR2:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleVector2>(field).value());

				case Field::TYPE_VECTOR3:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleVector3>(field).value());

				case Field::TYPE_VECTOR4:
					return shaderProgram->setParameter(fieldName, Field::cast<SingleVector4>(field).value());

				default:
					break;
			}
		}
		else
		{
			ocean_assert(false && "Missing implementation.");
		}
	}

	Log::warning() << "Failed to apply field \"" << fieldName << "\" to shader.";

	return false;
}

size_t ComposedShader::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
