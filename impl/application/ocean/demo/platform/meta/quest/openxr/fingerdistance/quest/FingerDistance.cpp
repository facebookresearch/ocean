/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/fingerdistance/quest/FingerDistance.h"

#include "ocean/base/String.h"

#include "ocean/rendering/Utilities.h"

using namespace Platform::Meta;

FingerDistance::FingerDistance(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	// nothing to do here
}

void FingerDistance::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	// we create a new scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();

	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	// let's create a group in which we place all rendering objects (invisible by default)
	renderingGroup_ = engine_->factory().createGroup();
	renderingGroup_->setVisible(false);
	scene->addChild(renderingGroup_);

	// let's create a red sphere for the left finger tip
	renderingTransformFingerTipLeft_ = Rendering::Utilities::createSphere(engine_, Scalar(0.01), RGBAColor(1.0f, 0.0f, 0.0f));
	renderingGroup_->addChild(renderingTransformFingerTipLeft_);

	// let's create a green sphere for the right finger tip
	renderingTransformFingerTipRight_ = Rendering::Utilities::createSphere(engine_, Scalar(0.01), RGBAColor(0.0f, 1.0f, 0.0f));
	renderingGroup_->addChild(renderingTransformFingerTipRight_);

	// let's create a transparent cylinder which will be shown between both finger tips
	renderingTransformCylinder_ = Rendering::Utilities::createCylinder(engine_, Scalar(0.005), Scalar(1), RGBAColor(1.0f, 1.0f, 1.0f, 0.7f));
	renderingGroup_->addChild(renderingTransformCylinder_);

	renderingTransformTextDistance_ = Rendering::Utilities::createText(*engine_, "<will be changed>", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02) /*fixedLineHeight*/, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextDistance_);
	renderingGroup_->addChild(renderingTransformTextDistance_);
}

void FingerDistance::onFramebufferReleasing()
{
	renderingTransformFingerTipLeft_.release();
	renderingTransformFingerTipRight_.release();

	renderingTransformCylinder_.release();
	renderingTransformTextDistance_.release();
	renderingTextDistance_.release();

	renderingGroup_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void FingerDistance::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	ocean_assert(renderingTransformFingerTipLeft_ && renderingTransformFingerTipRight_ && renderingTransformCylinder_);
	ocean_assert(renderingTransformTextDistance_ && renderingTextDistance_);

	Vectors3 indexPositions;

	if (handPoses_.isValid())
	{
		Vectors3 baseSpace_T_jointPositions;

		for (const size_t handIndex : {0, 1})
		{
			const Quest::OpenXR::HandPoses::Pose& pose = handPoses_.pose(handIndex);

			if (pose.isValid())
			{
				baseSpace_T_jointPositions.clear();
				if (pose.jointPositions(baseSpace_T_jointPositions))
				{
					ocean_assert(XR_HAND_JOINT_INDEX_TIP_EXT < baseSpace_T_jointPositions.size());
					const Vector3 indexPosition = baseSpace_T_jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT];

					indexPositions.push_back(indexPosition);
				}
			}
		}
	}

	bool showGroup = false;

	ocean_assert(indexPositions.size() <= 2);
	if (indexPositions.size() == 2)
	{
		const Vector3& leftIndexPosition = indexPositions[0];
		const Vector3& rightIndexPosition = indexPositions[1];

		renderingTransformFingerTipLeft_->setTransformation(HomogenousMatrix4(leftIndexPosition));
		renderingTransformFingerTipRight_->setTransformation(HomogenousMatrix4(rightIndexPosition));

		const Scalar distance = leftIndexPosition.distance(rightIndexPosition);

		renderingTextDistance_->setText(distanceString(distance));

		Log::info() << "Distance is " << distance << " meter";

		const HomogenousMatrix4 world_T_center = centerTransformation(leftIndexPosition, rightIndexPosition);

		if (world_T_center.isValid())
		{
			// the cylinder is define along the y-axis, however we need the cylinder to be define along the x-axis, so rotating the cylinder by 90deg
			static const HomogenousMatrix4 center_T_rotatedCylinder(Quaternion(Vector3(0, 0, 1), Numeric::pi_2()));

			// by default, the cylinder has a height of 1, therefore scaling the cylinder by 90% of the distance (in the x-direction)
			const HomogenousMatrix4 rotatedCylinder_T_scaledCylinder(Vector3(0, 0, 0), Vector3(1, Scalar(distance * Scalar(0.9)), 1));

			renderingTransformCylinder_->setTransformation(world_T_center * center_T_rotatedCylinder * rotatedCylinder_T_scaledCylinder);

			// shifting the text 3cm obove the cylinder
			const HomogenousMatrix4 world_T_text = world_T_center * HomogenousMatrix4(Vector3(0, Scalar(0.03), 0));

			renderingTransformTextDistance_->setTransformation(world_T_text);

			showGroup = true;
		}

		// let's change the transparency of the hands based on the distance

		const Scalar transparency = std::min(distance, Scalar(0.85)); // at most 85% transparency

		vrHandVisualizer_.setTransparency(transparency);
	}
	else
	{
		Log::info() << "Could not detect both fingers";
	}

	renderingGroup_->setVisible(showGroup);
}

HomogenousMatrix4 FingerDistance::centerTransformation(const Vector3& leftFingerTip, const Vector3& rightFingerTip)
{
	const Vector3 direction = rightFingerTip - leftFingerTip;
	const Vector3 centerPoint = leftFingerTip + direction * Scalar(0.5);

	Vector3 xAxis = direction;

	if (!xAxis.normalize())
	{
		return HomogenousMatrix4(false);
	}

	Vector3 yAxis = Vector3(0, 1, 0);
	Vector3 zAxis = xAxis.cross(yAxis);

	if (zAxis.isNull())
	{
		return HomogenousMatrix4(false);
	}

	// removing any screw
	yAxis = zAxis.cross(xAxis);
	if (!yAxis.normalize() || !zAxis.normalize())
	{
		return HomogenousMatrix4(false);
	}

	HomogenousMatrix4 world_T_center(xAxis, yAxis, zAxis, centerPoint);
	ocean_assert(world_T_center.isValid());

	return world_T_center;
}

std::string FingerDistance::distanceString(const Scalar distance)
{
	if (distance > Scalar(1)) // 1 meter
	{
		return " Distance: " + String::toAString(distance, 1u) + "m ";
	}

	if (distance > Scalar(0.2)) // 20 cm
	{
		return " Distance: " + String::toAString(distance * Scalar(100), 1u) + "cm ";
	}

	return " Distance: " + String::toAString(distance * Scalar(1000), 1u) + "mm ";
}
