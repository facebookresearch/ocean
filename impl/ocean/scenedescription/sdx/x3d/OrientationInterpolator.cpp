/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/OrientationInterpolator.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

OrientationInterpolator::OrientationInterpolator(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DInterpolatorNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

OrientationInterpolator::NodeSpecification OrientationInterpolator::specifyNode()
{
	NodeSpecification specification("OrientationInterpolator");

	registerField(specification, "keyValue", keyValue_, ACCESS_GET_SET);
	registerField(specification, "value_changed", valueChanged_, ACCESS_GET);

	X3DInterpolatorNode::registerFields(specification);

	return specification;
}

void OrientationInterpolator::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DInterpolatorNode::onInitialize(scene, timestamp);

	quaternionRotations_.clear();
	quaternionRotations_.reserve(keyValue_.values().size());

	for (MultiRotation::Values::const_iterator i = keyValue_.values().begin(); i != keyValue_.values().end(); ++i)
	{
		quaternionRotations_.push_back(Quaternion(*i));
	}
}

void OrientationInterpolator::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "keyValue")
	{
		quaternionRotations_.clear();
		quaternionRotations_.reserve(keyValue_.values().size());

		for (MultiRotation::Values::const_iterator i = keyValue_.values().begin(); i != keyValue_.values().end(); ++i)
		{
			quaternionRotations_.push_back(Quaternion(*i));
		}

		return;
	}

	X3DInterpolatorNode::onFieldChanged(fieldName);
}

void OrientationInterpolator::onSingleValue(const size_t index, const Timestamp eventTimestamp)
{
	ocean_assert(index < keyValue_.values().size());

	valueChanged_.setValue(keyValue_.values()[index], eventTimestamp);
	forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
}

void OrientationInterpolator::onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp)
{
	ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);
	ocean_assert(leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size());
	ocean_assert(leftIndex + 1 == rightIndex);

	if (leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size())
	{
		ocean_assert(keyValue_.values().size() == quaternionRotations_.size());

		valueChanged_.setValue(Rotation(quaternionRotations_[leftIndex].slerp(quaternionRotations_[rightIndex], float(interpolationFactor))), eventTimestamp);
		forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
	}
}

size_t OrientationInterpolator::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
