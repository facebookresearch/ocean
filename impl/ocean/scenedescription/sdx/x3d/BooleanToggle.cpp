/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/BooleanToggle.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

BooleanToggle::BooleanToggle(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

BooleanToggle::NodeSpecification BooleanToggle::specifyNode()
{
	NodeSpecification specification("BooleanToggle");

	registerField(specification, "set_boolean", setBoolean_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "toggle", toggle_, ACCESS_GET);

	X3DChildNode::registerFields(specification);

	return specification;
}

bool BooleanToggle::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "set_boolean")
	{
		const SingleBool& singleBool = Field::cast<SingleBool>(field);

		if (singleBool.value())
		{
			toggle_.setValue(!toggle_.value(), field.timestamp());
			forwardThatFieldHasBeenChanged("toggle", toggle_);
		}

		return true;
	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

size_t BooleanToggle::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
