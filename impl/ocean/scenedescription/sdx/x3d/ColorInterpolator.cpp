/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ColorInterpolator.h"

#include "ocean/math/HSVAColor.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ColorInterpolator::ColorInterpolator(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DInterpolatorNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ColorInterpolator::NodeSpecification ColorInterpolator::specifyNode()
{
	NodeSpecification specification("ColorInterpolator");

	registerField(specification, "keyValue", keyValue_, ACCESS_GET_SET);
	registerField(specification, "value_changed", valueChanged_, ACCESS_GET);

	X3DInterpolatorNode::registerFields(specification);

	return specification;
}

void ColorInterpolator::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DInterpolatorNode::onInitialize(scene, timestamp);

	hsvaColors_.clear();
	hsvaColors_.reserve(keyValue_.values().size());

	for (MultiColor::Values::const_iterator i = keyValue_.values().begin(); i != keyValue_.values().end(); ++i)
	{
		hsvaColors_.push_back(HSVAColor(*i));
	}
}

void ColorInterpolator::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "keyValue")
	{
		hsvaColors_.clear();
		hsvaColors_.reserve(keyValue_.values().size());

		for (MultiColor::Values::const_iterator i = keyValue_.values().begin(); i != keyValue_.values().end(); ++i)
		{
			hsvaColors_.push_back(HSVAColor(*i));
		}

		return;
	}

	X3DInterpolatorNode::onFieldChanged(fieldName);
}

void ColorInterpolator::onSingleValue(const size_t index, const Timestamp eventTimestamp)
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
			Log::warning() << "ColorInterpolator holds too less key values";
		}
		else
		{
			Log::warning() << "ColorInterpolator \"" << name_ << "\"holds too less key values";
		}
	}
}

void ColorInterpolator::onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp)
{
	ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);

	const ScopedLock lock(lock_);

	if (leftIndex < keyValue_.values().size() && rightIndex < keyValue_.values().size())
	{
		ocean_assert(keyValue_.values().size() == hsvaColors_.size());

		valueChanged_.setValue(RGBAColor(hsvaColors_[leftIndex].interpolate(hsvaColors_[rightIndex], float(interpolationFactor))), eventTimestamp);
		forwardThatFieldHasBeenChanged("value_changed", valueChanged_);
	}
	else
	{
		if (name_.empty())
		{
			Log::warning() << "ColorInterpolator holds too less key values";
		}
		else
		{
			Log::warning() << "ColorInterpolator \"" << name_ << "\"holds too less key values";
		}
	}
}

size_t ColorInterpolator::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
