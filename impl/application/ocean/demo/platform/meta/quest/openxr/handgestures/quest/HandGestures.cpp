/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/handgestures/quest/HandGestures.h"

#include "ocean/platform/meta/quest/openxr/HandGestures.h"

#include "ocean/rendering/Utilities.h"

using namespace Platform::Meta;

HandGestures::HandGestures(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	// nothing to do here
}

void HandGestures::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	// we create a new scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();

	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	renderingTransformHandLeft_ = Rendering::Utilities::createText(*engine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextHandLeft_);
	scene->addChild(renderingTransformHandLeft_);

	renderingTransformHandRight_ = Rendering::Utilities::createText(*engine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextHandRight_);
	scene->addChild(std::move(renderingTransformHandRight_));

	renderingGroupHandJoints_ = engine_->factory().createGroup();
	scene->addChild(renderingGroupHandJoints_);

	renderingGroupHandGestures_ = engine_->factory().createGroup();
	scene->addChild(renderingGroupHandGestures_);
}

void HandGestures::onFramebufferReleasing()
{
	renderingTextHandLeft_.release();
	renderingTextHandRight_.release();

	renderingTransformHandLeft_.release();
	renderingTransformHandRight_.release();

	renderingGroupHandJoints_.release();
	renderingGroupHandGestures_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void HandGestures::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	ocean_assert(renderingGroupHandJoints_);
	ocean_assert(renderingGroupHandGestures_);

	renderingGroupHandJoints_->clear();
	renderingGroupHandGestures_->clear();

	std::string textLeft;
	std::string textRight;

	const HomogenousMatrix4 world_T_device = locateSpace(xrSpaceView_.object(), xrPredictedDisplayTime);

	if (world_T_device.isValid())
	{
		for (const size_t handIndex : {0, 1}) // 0: left, 1: right
		{
			const bool isLeft = handIndex == 0;

			const Vectors3& worldJointPoints = handPoses_.jointPositions(handIndex);

			if (worldJointPoints.empty())
			{
				continue;
			}

			const Quest::OpenXR::HandPoses::Pose& pose = handPoses_.pose(handIndex);

			HomogenousMatrices4 world_T_joints;

			if (pose.isValid() && pose.jointTransformations(world_T_joints))
			{
				ocean_assert(worldJointPoints.size() == world_T_joints.size());

				const Rendering::TransformRef& renderingTransformHand = isLeft ? renderingTransformHandLeft_ : renderingTransformHandRight_;

				Vector3 handDirection = worldJointPoints[XR_HAND_JOINT_WRIST_EXT] - world_T_device.translation();
				handDirection.normalize();
				handDirection *= Scalar(0.2); // 20 cm behind and above the hands
				handDirection.y() = Scalar(0.2);

				renderingTransformHand->setTransformation(HomogenousMatrix4(worldJointPoints[XR_HAND_JOINT_WRIST_EXT] + handDirection, world_T_device.rotation()));

				if (renderingGroupHandJoints_->visible())
				{
					const Quaternion additionalRotation = Quaternion(Vector3(0, 1, 0), Numeric::pi_2()) * Quaternion(Vector3(-1, 0, 0), Numeric::pi_2());

					for (size_t n = 0; n < world_T_joints.size(); ++n)
					{
						Rendering::TransformRef transform = Rendering::Utilities::createText(*engine_, String::toAString(n), RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "");
						transform->setTransformation(world_T_joints[n] * additionalRotation);

						renderingGroupHandJoints_->addChild(std::move(transform));
					}
				}
			}

			std::string& text = isLeft ? textLeft : textRight;

			Vector3 position;
			if (Quest::OpenXR::HandGestures::isHandPinching(worldJointPoints, &position))
			{
				Rendering::TransformRef transform = Rendering::Utilities::createSphere(engine_, Scalar(0.005), RGBAColor(1.0f, 0.0f, 0.0f));
				transform->setTransformation(HomogenousMatrix4(position));
				renderingGroupHandGestures_->addChild(transform);

				text += " Pinching \n";
			}

			HomogenousMatrix4 world_T_tunnelCenter;
			if (Quest::OpenXR::HandGestures::isHandTunneling(worldJointPoints, isLeft, world_T_tunnelCenter))
			{
				Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine_, Scalar(0.05), Scalar(0.005), Scalar(0.002));
				transform->setTransformation(world_T_tunnelCenter);
				renderingGroupHandGestures_->addChild(transform);

				text += " Tunneling \n";
			}

			Line3 ray;
			if (Quest::OpenXR::HandGestures::isHandPointing(worldJointPoints, ray))
			{
				Rendering::TransformRef transform = Rendering::Utilities::createArrow(engine_, Scalar(0.05), Scalar(0.005), Scalar(0.002), RGBAColor(0.0f, 1.0f, 0.0f));
				transform->setTransformation(HomogenousMatrix4(ray.point(), Rotation(Vector3(0, 1, 0), ray.direction())));
				renderingGroupHandGestures_->addChild(transform);

				text += " Pointing \n";
			}

			HomogenousMatrix4 world_T_hand;
			if (Quest::OpenXR::HandGestures::isHandSpreading(worldJointPoints, isLeft, world_T_hand))
			{
				Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine_, Scalar(0.05), Scalar(0.005), Scalar(0.002));
				transform->setTransformation(world_T_hand);
				renderingGroupHandGestures_->addChild(transform);

				text += " Spreading \n";
			}

			if (Quest::OpenXR::HandGestures::isHandGrabbing(worldJointPoints, isLeft, world_T_hand))
			{
				Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine_, Scalar(0.05), Scalar(0.005), Scalar(0.002));
				transform->setTransformation(world_T_hand);
				renderingGroupHandGestures_->addChild(transform);

				text += " Grabbing \n";
			}

			if (text.empty())
			{
				text = " No hand gesture detected \n ";
			}
		}
	}

	renderingTextHandLeft_->setText(textLeft);
	renderingTextHandRight_->setText(textRight);
}

void HandGestures::onButtonPressed(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp)
{
	if (renderingGroupHandJoints_)
	{
		renderingGroupHandJoints_->setVisible(!renderingGroupHandJoints_->visible());
	}
}
