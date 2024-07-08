/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/BooleanFilter.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

BooleanFilter::BooleanFilter(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

BooleanFilter::NodeSpecification BooleanFilter::specifyNode()
{
	NodeSpecification specification("BooleanFilter");

	registerField(specification, "set_boolean", setBoolean_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "inputFalse", inputFalse_, ACCESS_GET);
	registerField(specification, "inputNegate", inputNegate_, ACCESS_GET);
	registerField(specification, "inputTrue", inputTrue_, ACCESS_GET);

	X3DChildNode::registerFields(specification);

	return specification;
}

bool BooleanFilter::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "set_boolean")
	{
		const SingleBool& singleBool = Field::cast<SingleBool>(field);

		if (singleBool.value())
		{
			inputTrue_.setValue(true, field.timestamp());
			forwardThatFieldHasBeenChanged("inputTrue", inputTrue_);
		}
		else
		{
			inputFalse_.setValue(true, field.timestamp());
			forwardThatFieldHasBeenChanged("inputFalse", inputFalse_);
		}

		inputNegate_.setValue(!singleBool.value(), field.timestamp());
		forwardThatFieldHasBeenChanged("inputNegate", inputNegate_);

		return true;
	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

size_t BooleanFilter::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
