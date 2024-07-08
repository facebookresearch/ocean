/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DShaderNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DShaderNode::X3DShaderNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	activate_(false),
	isSelected_(false),
	isValid_(false),
	language_()
{
	// nothing to do here
}

void X3DShaderNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "activate", activate_);
	registerField(specification, "isSelected", isSelected_);
	registerField(specification, "isValid", isValid_);
	registerField(specification, "language", language_);

	X3DAppearanceChildNode::registerFields(specification);
}

}

}

}

}
