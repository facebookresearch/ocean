// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/HandGesturesExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

bool HandGesturesExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	renderingTransformHandLeft_ = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextHandLeft_);
	experienceScene()->addChild(renderingTransformHandLeft_);

	renderingTransformHandRight_ = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextHandRight_);
	experienceScene()->addChild(std::move(renderingTransformHandRight_));

	renderingGroupHandJoints_ = engine->factory().createGroup();
	experienceScene()->addChild(renderingGroupHandJoints_);

	renderingGroupHandGestures_ = engine->factory().createGroup();
	experienceScene()->addChild(renderingGroupHandGestures_);

	return true;
}

bool HandGesturesExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingGroupHandGestures_.release();
	renderingGroupHandJoints_.release();

	renderingTransformHandLeft_.release();
	renderingTransformHandRight_.release();

	renderingTextHandLeft_.release();
	renderingTextHandRight_.release();

	return true;
}

Timestamp HandGesturesExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	renderingGroupHandJoints_->clear();
	renderingGroupHandGestures_->clear();

	std::string textLeft;
	std::string textRight;

	for (const bool left : {false, true})
	{
		Vectors3 world_T_jointPoints;
		if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, world_T_jointPoints))
		{
			continue;
		}

		HomogenousMatrices4 world_T_joints;
		if (!PlatformSpecific::get().handPoses().getHandPose(left, world_T_joints))
		{
			continue;
		}

		ocean_assert(world_T_jointPoints.size() == world_T_joints.size());

		const Rendering::TransformRef& renderingTransformHand = left ? renderingTransformHandLeft_ : renderingTransformHandRight_;

		Vector3 handDirection = world_T_jointPoints[ovrHandBone_WristRoot] - view->transformation().translation();
		handDirection.normalize();
		handDirection *= Scalar(0.2); // 20 cm behind and above the hands
		handDirection.y() = Scalar(0.2);

		renderingTransformHand->setTransformation(HomogenousMatrix4(world_T_jointPoints[ovrHandBone_WristRoot] + handDirection, view->transformation().rotation()));

		if (renderingGroupHandJoints_->visible())
		{
			const Quaternion additionalRotation = Quaternion(Vector3(0, 1, 0), Numeric::pi_2()) * Quaternion(Vector3(left ? 1 : -1, 0, 0), Numeric::pi_2());

			for (size_t n = 0; n < world_T_joints.size(); ++n)
			{
				Rendering::TransformRef transform = Rendering::Utilities::createText(*engine, String::toAString(n), RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "");
				transform->setTransformation(world_T_joints[n] * additionalRotation);

				renderingGroupHandJoints_->addChild(std::move(transform));
			}
		}

		std::string& text = left ? textLeft : textRight;

		Vector3 position;
		if (isHandPinching(left, &position))
		{
			Rendering::TransformRef transform = Rendering::Utilities::createSphere(engine, Scalar(0.005), RGBAColor(1.0f, 0.0f, 0.0f));
			transform->setTransformation(HomogenousMatrix4(position));
			renderingGroupHandGestures_->addChild(transform);

			text += " Pinching \n";
		}

		HomogenousMatrix4 world_T_tunnelCenter;
		if (isHandTunneling(left, world_T_tunnelCenter))
		{
			Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine, Scalar(0.05), Scalar(0.005), Scalar(0.002));
			transform->setTransformation(world_T_tunnelCenter);
			renderingGroupHandGestures_->addChild(transform);

			text += " Tunneling \n";
		}

		Line3 ray;
		if (isHandPointing(left, ray))
		{
			Rendering::TransformRef transform = Rendering::Utilities::createArrow(engine, Scalar(0.05), Scalar(0.005), Scalar(0.002), RGBAColor(0.0f, 1.0f, 0.0f));
			transform->setTransformation(HomogenousMatrix4(ray.point(), Rotation(Vector3(0, 1, 0), ray.direction())));
			renderingGroupHandGestures_->addChild(transform);

			text += " Pointing \n";
		}

		HomogenousMatrix4 world_T_hand;
		if (isHandSpreading(left, world_T_hand))
		{
			Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine, Scalar(0.05), Scalar(0.005), Scalar(0.002));
			transform->setTransformation(world_T_hand);
			renderingGroupHandGestures_->addChild(transform);

			text += " Spreading \n";
		}

		if (isHandGrabbing(left, world_T_hand))
		{
			Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine, Scalar(0.05), Scalar(0.005), Scalar(0.002));
			transform->setTransformation(world_T_hand);
			renderingGroupHandGestures_->addChild(transform);

			text += " Grabbing \n";
		}

		if (text.empty())
		{
			text = " No hand gesture detected \n ";
		}
	}

	renderingTextHandLeft_->setText(textLeft);
	renderingTextHandRight_->setText(textRight);

	return timestamp;
}

void HandGesturesExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*key*/, const Timestamp /*timestamp*/)
{
	renderingGroupHandJoints_->setVisible(!renderingGroupHandJoints_->visible());
}

std::unique_ptr<XRPlaygroundExperience> HandGesturesExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new HandGesturesExperience());
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

std::unique_ptr<XRPlaygroundExperience> HandGesturesExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new HandGesturesExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}
