/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ShaderPart.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ShaderPart::ShaderPart(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DUrlObject(environment),
	type_("VERTEX")
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ShaderPart::NodeSpecification ShaderPart::specifyNode()
{
	NodeSpecification specification("ShaderPart");

	X3DNode::registerFields(specification);
	X3DUrlObject::registerFields(specification);

	registerField(specification, "type", type_, ACCESS_NONE);

	return specification;
}

void ShaderPart::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DNode::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	if (type_.value() != "VERTEX" && type_.value() != "FRAGMENT")
	{
		Log::warning() << "The type of a ShaderPart must be \"VERTEX\" or \"FRAGMENT\".";
		return;
	}
}

size_t ShaderPart::objectAddress() const
{
	return size_t(this);
}

Rendering::ShaderProgram::FilenamePair ShaderPart::filenamePair() const
{
	Rendering::ShaderProgram::ShaderType shaderType = Rendering::ShaderProgram::ST_INVALID;

	if (type_.value() == "VERTEX")
	{
		shaderType = Rendering::ShaderProgram::ST_VERTEX;
	}
	else if (type_.value() == "FRAGMENT")
	{
		shaderType = Rendering::ShaderProgram::ST_FRAGMENT;
	}

	if (shaderType != Rendering::ShaderProgram::ST_INVALID)
	{
		const IO::Files resolvedFiles(resolveUrls());

		for (const IO::File& resolvedFile : resolvedFiles)
		{
			if (resolvedFile.exists())
			{
				return Rendering::ShaderProgram::FilenamePair(resolvedFile(), shaderType);
			}
		}
	}

	return Rendering::ShaderProgram::FilenamePair();
}

}

}

}

}
