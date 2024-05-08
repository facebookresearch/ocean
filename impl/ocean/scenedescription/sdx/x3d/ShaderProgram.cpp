/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ShaderProgram.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ShaderProgram::ShaderProgram(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DUrlObject(environment),
	X3DProgrammableShaderObject(environment),
	type_("VERTEX")
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ShaderProgram::NodeSpecification ShaderProgram::specifyNode()
{
	NodeSpecification specification("ShaderProgram");

	registerField(specification, "type", type_);

	X3DNode::registerFields(specification);
	X3DUrlObject::registerFields(specification);
	X3DProgrammableShaderObject::registerFields(specification);

	return specification;
}

size_t ShaderProgram::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
