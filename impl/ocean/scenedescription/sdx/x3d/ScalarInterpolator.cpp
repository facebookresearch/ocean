/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ScalarInterpolator.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ScalarInterpolator::ScalarInterpolator(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DInterpolatorNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ScalarInterpolator::NodeSpecification ScalarInterpolator::specifyNode()
{
	NodeSpecification specification("ScalarInterpolator");

	registerField(specification, "keyValue", keyValue_, ACCESS_GET_SET);
	registerField(specification, "value_changed", valueChanged_, ACCESS_GET);

	X3DInterpolatorNode::registerFields(specification);

	return specification;
}

void ScalarInterpolator::onSingleValue(const size_t index, const Timestamp eventTimestamp)
{
	const ScopedLock lock(lock_);

	if (index < keyValue_.values().size())
	{
		valueChanged_.setValue(keyValue_.values()[index], eventTimestamp);
		forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
	}
	else
	{
		if (name_.empty())
		{
			Log::warning() << "ScalarInterpolator holds too less key values";
		}
		else
		{
			Log::warning() << "ScalarInterpolator \"" << name_ << "\"holds too less key values";
		}
	}
}

void ScalarInterpolator::onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp)
{
	ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);
	const ScopedLock lock(lock_);

	if (leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size())
	{
		ocean_assert(leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size());
		ocean_assert(leftIndex + 1 == rightIndex);

		valueChanged_.setValue(keyValue_.values()[leftIndex] * (1 - interpolationFactor) + keyValue_.values()[rightIndex] * interpolationFactor, eventTimestamp);
		forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
	}
	else
	{
		if (name_.empty())
		{
			Log::warning() << "ScalarInterpolator holds too less key values";
		}
		else
		{
			Log::warning() << "ScalarInterpolator \"" << name_ << "\"holds too less key values";
		}
	}
}

size_t ScalarInterpolator::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
