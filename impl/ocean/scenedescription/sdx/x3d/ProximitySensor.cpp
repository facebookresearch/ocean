/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ProximitySensor.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ProximitySensor::ProximitySensor(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DSensorNode(environment),
	SDXUpdateNode(environment),
	X3DEnvironmentalSensorNode(environment),
	lastLocalTranslation_(0, 0, 0)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ProximitySensor::NodeSpecification ProximitySensor::specifyNode()
{
	NodeSpecification specification("ProximitySensor");

	registerField(specification, "centerOfRotation_changed", centerOfRotationChanged_, ACCESS_GET);
	registerField(specification, "orientation_changed", orientationChanged_, ACCESS_GET);
	registerField(specification, "position_changed", positionChanged_, ACCESS_GET);

	X3DEnvironmentalSensorNode::registerFields(specification);

	return specification;
}

void ProximitySensor::onInsideBoundingBox(const Vector3& translation, const Quaternion& orientation, const Timestamp timestamp)
{
	if (translation != lastLocalTranslation_)
	{
		positionChanged_.setValue(translation, timestamp);
		forwardThatFieldHasBeenChanged("position_changed", positionChanged_);

		lastLocalTranslation_ = translation;
	}

	if (orientation != lastLocalOrientation_)
	{
		orientationChanged_.setValue(Rotation(orientation), timestamp);
		forwardThatFieldHasBeenChanged("orientation_changed", orientationChanged_);

		lastLocalOrientation_ = orientation;
	}
}

size_t ProximitySensor::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
