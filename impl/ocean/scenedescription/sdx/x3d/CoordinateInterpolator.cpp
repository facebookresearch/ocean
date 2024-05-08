/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/CoordinateInterpolator.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

CoordinateInterpolator::CoordinateInterpolator(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DInterpolatorNode(environment),
	interpolationValues_(0u)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

CoordinateInterpolator::NodeSpecification CoordinateInterpolator::specifyNode()
{
	NodeSpecification specification("CoordinateInterpolator");

	registerField(specification, "keyValue", keyValue_, ACCESS_GET_SET);
	registerField(specification, "value_changed", valueChanged_, ACCESS_GET);

	X3DInterpolatorNode::registerFields(specification);

	return specification;
}

void CoordinateInterpolator::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DInterpolatorNode::onInitialize(scene, timestamp);

	interpolationValues_ = (unsigned int)(keyValue_.values().size() / key_.values().size());

	if ((keyValue_.values().size() % key_.values().size()) != 0)
	{
		interpolationValues_ = 0u;
	}
}

void CoordinateInterpolator::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "key" || fieldName == "keyValue")
	{
		interpolationValues_ = (unsigned int)(keyValue_.values().size() / key_.values().size());

		if ((keyValue_.values().size() % key_.values().size()) != 0)
		{
			interpolationValues_ = 0u;
		}

		return;
	}

	X3DInterpolatorNode::onFieldChanged(fieldName);
}

void CoordinateInterpolator::onSingleValue(const size_t index, const Timestamp eventTimestamp)
{
	ocean_assert(index < keyValue_.values().size());

	valueChanged_.values().resize(interpolationValues_);

	unsigned int begin = (unsigned int)(index * interpolationValues_);
	for (unsigned int n = 0u; n < interpolationValues_; ++n)
	{
		valueChanged_.values()[n] = keyValue_.values()[begin++];
	}

	valueChanged_.setTimestamp(eventTimestamp);
	forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
}

void CoordinateInterpolator::onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp)
{
	ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);
	ocean_assert(leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size());
	ocean_assert(leftIndex + 1 == rightIndex);

	if (interpolationValues_ != 0u)
	{
		valueChanged_.values().resize(interpolationValues_);

		if (leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size())
		{
			unsigned int left = (unsigned int)(leftIndex * interpolationValues_);
			unsigned int right = (unsigned int)(rightIndex * interpolationValues_);

			for (unsigned int n = 0u; n < interpolationValues_; ++n)
			{
				valueChanged_.values()[n] = keyValue_.values()[left++] * (1 - interpolationFactor) + keyValue_.values()[right++] * interpolationFactor;
			}

			valueChanged_.setTimestamp(eventTimestamp);
			forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
		}
	}
}

size_t CoordinateInterpolator::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
