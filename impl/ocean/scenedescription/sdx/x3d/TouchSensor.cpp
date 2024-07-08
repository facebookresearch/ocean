/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/TouchSensor.h"
#include "ocean/scenedescription/sdx/x3d/Inline.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TouchSensor::TouchSensor(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DSensorNode(environment),
	SDXEventNode(environment),
	X3DPointingDeviceSensorNode(environment),
	X3DTouchSensorNode(environment),
	hitTexCoordChanged_(Vector2(0, 0)),
	lastObjectPosition_(0, 0, 0)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

TouchSensor::NodeSpecification TouchSensor::specifyNode()
{
	NodeSpecification specification("TouchSensor");

	registerField(specification, "enabled", enabled_, ACCESS_GET_SET);
	registerField(specification, "hitNormal_changed", hitNormalChanged_, ACCESS_GET);
	registerField(specification, "hitPoint_changed", hitPointChanged_, ACCESS_GET);
	registerField(specification, "hitTexCoord_changed", hitTexCoordChanged_, ACCESS_GET);

	X3DTouchSensorNode::registerFields(specification);

	return specification;
}

void TouchSensor::onMouse(const ButtonType button, const ButtonEvent buttonEvent, const Vector2& /*screenPosition*/, const Vector3& objectPosition, const Rendering::ObjectId objectId, const Timestamp timestamp)
{
	if (enabled_.value() == false || button != BUTTON_LEFT)
	{
		return;
	}

	if (isActive_.value() == false)
	{
		if (buttonEvent == EVENT_PRESS && isSiblingNode(objectId))
		{
			isActive_.setValue(true, timestamp);
			forwardThatFieldHasBeenChanged("isActive", isActive_);

			isOver_.setValue(true, timestamp);
			forwardThatFieldHasBeenChanged("isOver", isOver_);

			touchTime_.setValue(timestamp, timestamp);
			forwardThatFieldHasBeenChanged("touchTime", touchTime_);

			hitPointChanged_.setValue(objectPosition, timestamp);
			forwardThatFieldHasBeenChanged("hitPoint_changed", hitPointChanged_);
			lastObjectPosition_ = objectPosition;
		}
	}
	else // isActive.value() == true
	{
		if (buttonEvent == EVENT_HOLD)
		{
			if (isSiblingNode(objectId))
			{
				if (isOver_.value() == false)
				{
					isOver_.setValue(true, timestamp);
					forwardThatFieldHasBeenChanged("isOver", isOver_);
				}

				touchTime_.setValue(timestamp, timestamp);
				forwardThatFieldHasBeenChanged("touchTime", touchTime_);

				if (lastObjectPosition_ != objectPosition)
				{
					hitPointChanged_.setValue(objectPosition, timestamp);
					forwardThatFieldHasBeenChanged("hitPoint_changed", hitPointChanged_);
					lastObjectPosition_ = objectPosition;
				}
			}
			else
			{
				if (isOver_.value())
				{
					isOver_.setValue(false, timestamp);
					forwardThatFieldHasBeenChanged("isOver", isOver_);
				}
			}
		}
		else if (buttonEvent == EVENT_RELEASE)
		{
			isActive_.setValue(false, timestamp);
			forwardThatFieldHasBeenChanged("isActive", isActive_);

			if (isOver_.value())
			{
				isOver_.setValue(false, timestamp);
				forwardThatFieldHasBeenChanged("isOver", isOver_);
			}

			touchTime_.setValue(timestamp, timestamp);
			forwardThatFieldHasBeenChanged("touchTime", touchTime_);
		}
	}
}

void TouchSensor::onKey(const int /*key*/, const ButtonEvent /*buttonEvent*/, const Rendering::ObjectId /*objectId*/, const Timestamp /*timestamp*/)
{
	// nothing to do here
}

bool TouchSensor::isSiblingNode(const Rendering::ObjectId objectId) const
{
	if (objectId == Rendering::invalidObjectId)
	{
		return false;
	}

	const NodeRefs parents(parentNodes());

	for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
	{
		const SmartObjectRef<X3DGroupingNode, Node> group(*i);

		if (group)
		{
			if (hasRenderingObject(group, objectId))
			{
				return true;
			}
		}
		else
		{
			const SmartObjectRef<Inline, Node> inlineNode(*i);

			if (inlineNode)
			{
				const SDXSceneRef inlineChildScene(inlineNode->childScene());

				if (inlineChildScene && hasRenderingObject(inlineChildScene, objectId))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool TouchSensor::hasRenderingObject(const SmartObjectRef<X3DGroupingNode, Node>& group, const Rendering::ObjectId objectId)
{
	ocean_assert(group);

	const MultiNode& children = group->field<MultiNode>("children");

	for (MultiNode::Values::const_iterator i = children.values().begin(); i != children.values().end(); ++i)
	{
		const SDXNodeRef child(*i);
		ocean_assert(child);

		if (child->renderingObject() && child->renderingObject()->id() == objectId)
		{
			return true;
		}

		const SmartObjectRef<X3DGroupingNode, Node> childGroup(*i);
		if (childGroup)
		{
			if (hasRenderingObject(childGroup, objectId))
			{
				return true;
			}
		}
		else
		{
			const SmartObjectRef<Inline, Node> inlineNode(*i);

			if (inlineNode)
			{
				const SDXSceneRef inlineChildScene(inlineNode->childScene());

				if (inlineChildScene && hasRenderingObject(inlineChildScene, objectId))
				{
					return true;
				}
			}
		}
	}

	return false;
}

size_t TouchSensor::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
