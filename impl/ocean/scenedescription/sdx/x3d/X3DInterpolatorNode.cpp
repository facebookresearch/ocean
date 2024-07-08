/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DInterpolatorNode.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DInterpolatorNode::X3DInterpolatorNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment)
{
	// nothing to do here
}

void X3DInterpolatorNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "set_fraction", setFraction_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "key", key_, ACCESS_GET_SET);

	X3DChildNode::registerFields(specification);
}

void X3DInterpolatorNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DChildNode::onInitialize(scene, timestamp);

	keyMap_.clear();
	for (size_t n = 0; n < key_.values().size(); ++n)
	{
		const Scalar value = key_.values()[n];

		if (value < 0 || value > 1)
		{
			Log::error() << "Invalid interpolation key value: " << value << " the interpolator will be disabled.";
			keyMap_.clear();
			break;
		}

		// inserting the new key connected with the index of the key value
		// if the key does exist already the highest key value index is stored as second index
		KeyMap::iterator i = keyMap_.insert(std::make_pair(key_.values()[n], std::make_pair(n, size_t(-1)))).first;
		i->second.second = n;
	}
}

bool X3DInterpolatorNode::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "set_fraction")
	{
		const SingleFloat& singleFloat = Field::cast<SingleFloat>(field);

		if (singleFloat.value() < 0 || singleFloat.value() > 1)
		{
			Log::warning() << "Invalid set_fraction value which must be inside the range [0, 1]!";
		}
		else
		{
			const Scalar fraction = singleFloat.value();

			ocean_assert(fraction >= 0 && fraction <= 1);

			if (keyMap_.size() >= 2)
			{
				KeyMap::const_iterator left = keyMap_.lower_bound(fraction);

				if (left == keyMap_.begin())
				{
					ocean_assert(fraction <= left->first);
					onSingleValue(0, field.timestamp());
				}
				else if (left == keyMap_.end())
				{
					ocean_assert(fraction > (--keyMap_.end())->first);
					onSingleValue(keyMap_.size() - 1, field.timestamp());
				}
				else if (Numeric::isEqual(left->first, fraction))
				{
					onSingleValue(left->second.first, field.timestamp());
				}
				else
				{
					KeyMap::const_iterator right = left;
					--left;


					ocean_assert(left->first < fraction && right->first >= fraction);

					const Scalar range = right->first - left->first;
					ocean_assert(range > 0);

					Scalar interpolationFactor = (fraction - left->first) / range;
					ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);

					onInterpolate(left->second.second, right->second.first, interpolationFactor, field.timestamp());
				}
			}
		}

		return true;
	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

}

}

}

}
