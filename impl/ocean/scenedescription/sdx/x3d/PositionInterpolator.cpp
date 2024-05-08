/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/PositionInterpolator.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

PositionInterpolator::PositionInterpolator(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DInterpolatorNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

PositionInterpolator::NodeSpecification PositionInterpolator::specifyNode()
{
	NodeSpecification specification("PositionInterpolator");

	registerField(specification, "keyValue", keyValue_, ACCESS_GET_SET);
	registerField(specification, "value_changed", valueChanged_, ACCESS_GET);

	X3DInterpolatorNode::registerFields(specification);

	return specification;
}

void PositionInterpolator::onSingleValue(const size_t index, const Timestamp eventTimestamp)
{
	ocean_assert(index < keyValue_.values().size());

	valueChanged_.setValue(keyValue_.values()[index], eventTimestamp);
	forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
}

void PositionInterpolator::onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp)
{
	ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);
	ocean_assert(leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size());
	ocean_assert(leftIndex + 1 == rightIndex);

	if (leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size())
	{
		valueChanged_.setValue(keyValue_.values()[leftIndex] * (1 - interpolationFactor) + keyValue_.values()[rightIndex] * interpolationFactor, eventTimestamp);
		forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
	}
}

size_t PositionInterpolator::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
