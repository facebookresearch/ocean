/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/PackagedShader.h"

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

PackagedShader::PackagedShader(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DProgrammableShaderObject(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DShaderNode(environment),
	X3DUrlObject(environment),
	SDXDynamicNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

PackagedShader::NodeSpecification PackagedShader::specifyNode()
{
	NodeSpecification specification("PackagedShader");

	X3DProgrammableShaderObject::registerFields(specification);
	X3DUrlObject::registerFields(specification);
	X3DShaderNode::registerFields(specification);

	return specification;
}

void PackagedShader::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DShaderNode::onInitialize(scene, timestamp);
	X3DProgrammableShaderObject::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	if (language_.value() != "CG")
	{
		Log::warning() << "Up to now PackagedShader with \"CG\" shading language are supported.";
		return;
	}

	if (url_.values().empty() == false)
	{
		const IO::Files resolvedFiles(resolveUrls());

		Rendering::ShaderProgramRef shaderProgram = engine()->factory().createShaderProgram();

		if (shaderProgram)
		{
			shaderProgram->setName(name_);
			std::string errorMessage;

			for (const IO::File& resolvedFile : resolvedFiles)
			{
				if (resolvedFile.exists())
				{
					const Rendering::ShaderProgram::FilenamePairs filenamePairs(1, std::make_pair(resolvedFile(), Rendering::ShaderProgram::ST_UNIFIED));

					if (shaderProgram->setShader(Rendering::ShaderProgram::SL_CG, filenamePairs, errorMessage))
					{
						break;
					}

					Log::error() << "Failed to compile and link the shader code file \"" << resolvedFile() << "\": " << errorMessage;
				}
			}

			if (shaderProgram->isCompiled())
			{
				for (unsigned int n = 0u; n < dynamicFields(); ++n)
				{
					applyParameter(shaderProgram, dynamicFieldName(n));
				}

				renderingObject_ = std::move(shaderProgram);
			}
		}
	}
}

void PackagedShader::onFieldChanged(const std::string& fieldName)
{
	Rendering::ShaderProgramRef renderingShaderProgram(renderingObject_);

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
	X3DUrlObject::onFieldChanged(fieldName);
}

bool PackagedShader::applyParameter(const Rendering::ShaderProgramRef& shaderProgram, const std::string& fieldName)
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

size_t PackagedShader::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
