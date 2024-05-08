/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DEnvironmentalSensorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DEnvironmentalSensorNode::X3DEnvironmentalSensorNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DSensorNode(environment),
	SDXUpdateNode(environment),
	center_(Vector3(0, 0, 0)),
	size_(Vector3(0, 0, 0)),
	sensorEntered_(false)
{
	// nothing to do here
}

void X3DEnvironmentalSensorNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "center", center_, ACCESS_GET_SET);
	registerField(specification, "size", size_, ACCESS_GET_SET);
	registerField(specification, "enterTime", enterTime_, ACCESS_GET);
	registerField(specification, "exitTime", exitTime_, ACCESS_GET);
	registerField(specification, "isActive", isActive_, ACCESS_GET);

	X3DSensorNode::registerFields(specification);
}

void X3DEnvironmentalSensorNode::onUpdate(const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(view);
	ocean_assert(timestamp.isValid());

	if (enabled_.value() == false)
	{
		return;
	}

	const Vector3 boxSize = size_.value() * Scalar(0.5);

	if (boxSize.x() <= 0 || boxSize.y() <= 0 || boxSize.z() <= 0)
	{
		return;
	}

	const HomogenousMatrices4 world_T_sensors(sensorTransformations());

	bool isInside = false;
	for (const HomogenousMatrix4& world_T_sensor : world_T_sensors)
	{
		HomogenousMatrix4 sensor_T_world;
		if (!world_T_sensor.invert(sensor_T_world))
		{
			ocean_assert(false && "Invalid transformation!");
			continue;
		}

		const HomogenousMatrix4 sensor_T_view(sensor_T_world * view->transformation());
		const Vector3 translationInSensor(sensor_T_view.translation());

		if (translationInSensor.x() >= center_.value().x() - boxSize.x() && translationInSensor.x() <= center_.value().x() + boxSize.x()
			&& translationInSensor.y() >= center_.value().y() - boxSize.y() && translationInSensor.y() <= center_.value().y() + boxSize.y()
			&& translationInSensor.z() >= center_.value().z() - boxSize.z() && translationInSensor.z() <= center_.value().z() + boxSize.z())
		{
			if (sensorEntered_ == false)
			{
				enterTime_.setValue(timestamp, timestamp);
				isActive_.setValue(true, timestamp);

				forwardThatFieldHasBeenChanged("enterTime", enterTime_);
				forwardThatFieldHasBeenChanged("isActive", isActive_);

				sensorEntered_ = true;
			}

			const Quaternion rotationInSensor = sensor_T_view.rotation();

			onInsideBoundingBox(translationInSensor, rotationInSensor, timestamp);
			isInside = true;

			// we use the first valid transformation only
			break;
		}
	}

	if (sensorEntered_ && isInside == false)
	{
		exitTime_.setValue(timestamp, timestamp);
		isActive_.setValue(false, timestamp);

		forwardThatFieldHasBeenChanged("exitTime", exitTime_);
		forwardThatFieldHasBeenChanged("isActive", isActive_);

		sensorEntered_ = false;
	}
}

HomogenousMatrices4 X3DEnvironmentalSensorNode::sensorTransformations() const
{
	const ScopedLock scopedLock(lock_);

	HomogenousMatrices4 transformations;
	const NodeRefs parents(parentNodes());

	for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
	{
		const SDXNodeRef parent(*i);
		ocean_assert(parent);

		const Rendering::NodeRef parentNode(parent->renderingObject());
		if (parentNode)
		{
			const HomogenousMatrices4 parentTransformations(parentNode->worldTransformations());
			transformations.insert(transformations.end(), parentTransformations.begin(), parentTransformations.end());
		}
	}

	if (transformations.empty())
	{
		transformations.emplace_back(true /* identity */);
	}

	return transformations;
}

}

}

}

}
