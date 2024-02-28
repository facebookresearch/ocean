// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

Scalar XRPlaygroundExperience::distanceBetweenFingerBones(const bool left, const ovrHandBone bone0, const ovrHandBone bone1, Vector3* position0, Vector3* position1)
{
	if (bone0 == bone1)
	{
		return Scalar(0);
	}

	if (bone0 >= ovrHandBone_Max || bone1 >= ovrHandBone_Max)
	{
		ocean_assert(false && "Invalid bones!");
		return Numeric::maxValue();
	}

	Vectors3 world_T_jointPoints;
	if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, world_T_jointPoints))
	{
		return Numeric::maxValue();
	}

	if (world_T_jointPoints.size() < ovrHandBone_Max)
	{
		ocean_assert(false && "This should never happen!");
		return Numeric::maxValue();
	}

	if (position0 != nullptr)
	{
		*position0 = world_T_jointPoints[bone0];
	}

	if (position1 != nullptr)
	{
		*position1 = world_T_jointPoints[bone1];
	}

	return world_T_jointPoints[bone0].distance(world_T_jointPoints[bone1]);
}

bool XRPlaygroundExperience::isHandPinching(const bool left, Vector3* position, const Scalar maxDistance)
{
	ocean_assert(maxDistance >= 0);

	Vector3 thumbPosition;
	Vector3 indexPosition;
	const Scalar distance = distanceBetweenFingerBones(left, ovrHandBone_ThumbTip, ovrHandBone_IndexTip, &thumbPosition, &indexPosition);

	if (position != nullptr)
	{
		*position = (thumbPosition + indexPosition) * Scalar(0.5);
	}

	return distance <= maxDistance;
}

bool XRPlaygroundExperience::isHandTunneling(const bool left, HomogenousMatrix4& world_T_tunnelCenter, const Scalar maxTipDistance, const Scalar minTunnelDistance)
{
	ocean_assert(maxTipDistance >= 0);
	ocean_assert(minTunnelDistance >= 0);

	Vectors3 jointPointsInWorld;
	if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, jointPointsInWorld))
	{
		return false;
	}

	const Scalar distance = jointPointsInWorld[ovrHandBone_ThumbTip].distance(jointPointsInWorld[ovrHandBone_IndexTip]);

	if (distance > maxTipDistance)
	{
		return false;
	}

	Vector3 xAxis = jointPointsInWorld[ovrHandBone_Index1] - jointPointsInWorld[ovrHandBone_IndexTip]; // e.g., ovrHandBone_Index1 == index proximal phalange bone
	Vector3 yAxis = jointPointsInWorld[ovrHandBone_Index2] - jointPointsInWorld[ovrHandBone_Thumb2];

	const Scalar sqrMinTunnelDistance = Numeric::sqr(minTunnelDistance);

	if (xAxis.sqr() < sqrMinTunnelDistance || yAxis.sqr() < sqrMinTunnelDistance)
	{
		return false;
	}

	if (xAxis.isNull() || yAxis.isNull())
	{
		return false;
	}

	if (left)
	{
		// the x-axis is pointing to the user's right (regardless of whether this is the left or right hand)
		xAxis = -xAxis;
	}

	Vector3 zAxis = xAxis.cross(yAxis);
	yAxis = zAxis.cross(xAxis);

	if (!xAxis.normalize() || !yAxis.normalize() || !zAxis.normalize())
	{
		return false;
	}

	const Indices32 jointIndices =
	{
		ovrHandBone_IndexTip, ovrHandBone_Index2, ovrHandBone_Index1, ovrHandBone_Thumb2
	};

	Vector3 sumPosition(0, 0, 0);
	for (const Index32 jointIndex : jointIndices)
	{
		sumPosition += jointPointsInWorld[jointIndex];
	}

	const Vector3 translation = sumPosition / Scalar(jointIndices.size());

	world_T_tunnelCenter = HomogenousMatrix4(xAxis, yAxis, zAxis, translation);

	return true;
}

bool XRPlaygroundExperience::isHandPointing(const bool left, Line3& worldRay, const Scalar maxIndexAngle, const Scalar minAngle, const Scalar minTipDistance, const Scalar maxTipDistance)
{
	ocean_assert(maxIndexAngle >= 0 && maxIndexAngle < Numeric::pi());
	ocean_assert(minAngle >= 0 && minAngle < Numeric::pi());

	ocean_assert(minTipDistance >= 0);
	ocean_assert(maxTipDistance >= 0);

	Vectors3 jointPointsInWorld;
	if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, jointPointsInWorld))
	{
		return false;
	}

	// first, we ensure that the index finger is a straight line, and other fings are not straight

	Scalar resultMaxAngle;
	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(jointPointsInWorld, {ovrHandBone_Index1, ovrHandBone_Index2, ovrHandBone_Index3, ovrHandBone_IndexTip}, nullptr, &resultMaxAngle) || resultMaxAngle > maxIndexAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(jointPointsInWorld, {ovrHandBone_WristRoot, ovrHandBone_Middle1, ovrHandBone_Middle2, ovrHandBone_Middle3, ovrHandBone_MiddleTip}, nullptr, &resultMaxAngle) || resultMaxAngle < minAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(jointPointsInWorld, {ovrHandBone_WristRoot, ovrHandBone_Ring1, ovrHandBone_Ring2, ovrHandBone_Ring3, ovrHandBone_RingTip}, nullptr, &resultMaxAngle) || resultMaxAngle < minAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(jointPointsInWorld, {ovrHandBone_Pinky0, ovrHandBone_Pinky1, ovrHandBone_Pinky2, ovrHandBone_Pinky3, ovrHandBone_PinkyTip}, nullptr, &resultMaxAngle) || resultMaxAngle < minAngle)
	{
		return false;
	}

	const Scalar sqrMinTipDistance = Numeric::sqr(minTipDistance);
	const Scalar sqrMaxTipDistance = Numeric::sqr(maxTipDistance);

	// now, we ensure that the remaining fingers tips are close to each other

	if (jointPointsInWorld[ovrHandBone_PinkyTip].sqrDistance(jointPointsInWorld[ovrHandBone_RingTip]) > sqrMaxTipDistance
			|| jointPointsInWorld[ovrHandBone_RingTip].sqrDistance(jointPointsInWorld[ovrHandBone_MiddleTip]) > sqrMaxTipDistance
			|| jointPointsInWorld[ovrHandBone_ThumbTip].sqrDistance(jointPointsInWorld[ovrHandBone_Middle2]) > sqrMaxTipDistance)
	{
		return false;
	}

	// now, we ensure that the remaining fingers are far away from the index tip

	if (jointPointsInWorld[ovrHandBone_PinkyTip].sqrDistance(jointPointsInWorld[ovrHandBone_IndexTip]) < sqrMinTipDistance
			|| jointPointsInWorld[ovrHandBone_RingTip].sqrDistance(jointPointsInWorld[ovrHandBone_IndexTip]) < sqrMinTipDistance
			|| jointPointsInWorld[ovrHandBone_MiddleTip].sqrDistance(jointPointsInWorld[ovrHandBone_IndexTip]) < sqrMinTipDistance
			|| jointPointsInWorld[ovrHandBone_ThumbTip].sqrDistance(jointPointsInWorld[ovrHandBone_IndexTip]) < sqrMinTipDistance)
	{
		return false;
	}

	Vector3 direction = jointPointsInWorld[ovrHandBone_IndexTip] - jointPointsInWorld[ovrHandBone_Index1];

	if (!direction.normalize())
	{
		return false;
	}

	worldRay = Line3(jointPointsInWorld[ovrHandBone_Index1], direction);

	return true;
}

bool XRPlaygroundExperience::isHandSpreading(const bool left, HomogenousMatrix4& world_T_hand, const Scalar maxAngle, const Scalar minTipDistance)
{
	ocean_assert(maxAngle >= 0 && maxAngle < Numeric::pi());

	ocean_assert(minTipDistance >= 0);

	Vectors3 world_T_jointPoints;
	if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, world_T_jointPoints))
	{
		return false;
	}

	// first, we ensure that all fingers are straight lines

	Scalar resultMaxAngle;
	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Index1, ovrHandBone_Index2, ovrHandBone_Index3}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Middle1, ovrHandBone_Middle2, ovrHandBone_Middle3}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Ring1, ovrHandBone_Ring2, ovrHandBone_Ring3}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Pinky1, ovrHandBone_Pinky2, ovrHandBone_Pinky3}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	const Scalar sqrMinTipDistance = Numeric::sqr(minTipDistance);

	// now, we ensure that all fingers tips are far away from each other

	if (world_T_jointPoints[ovrHandBone_ThumbTip].sqrDistance(world_T_jointPoints[ovrHandBone_IndexTip]) < sqrMinTipDistance
			|| world_T_jointPoints[ovrHandBone_IndexTip].sqrDistance(world_T_jointPoints[ovrHandBone_MiddleTip]) < sqrMinTipDistance
			|| world_T_jointPoints[ovrHandBone_MiddleTip].sqrDistance(world_T_jointPoints[ovrHandBone_RingTip]) < sqrMinTipDistance
			|| world_T_jointPoints[ovrHandBone_RingTip].sqrDistance(world_T_jointPoints[ovrHandBone_PinkyTip]) < sqrMinTipDistance)
	{
		return false;
	}

	const Vector3 position = (world_T_jointPoints[ovrHandBone_WristRoot] + world_T_jointPoints[ovrHandBone_Middle1]) * Scalar(0.5);

	Vector3 xAxis = world_T_jointPoints[ovrHandBone_Pinky1] - world_T_jointPoints[ovrHandBone_Index1];

	if (left)
	{
		// the x-axis is pointing to the user's right when the palms are away from the user and to the left when the palms are towards the user (regardless of whether this is the left or right hand)
		xAxis = -xAxis;
	}

	Vector3 yAxis = world_T_jointPoints[ovrHandBone_Middle1] - world_T_jointPoints[ovrHandBone_WristRoot];

	Vector3 zAxis = xAxis.cross(yAxis);

	if (!zAxis.normalize() || !yAxis.normalize())
	{
		return false;
	}

	xAxis = yAxis.cross(zAxis);

	constexpr Scalar additionalRotationAngle = Numeric::deg2rad(10); // just some custom rotation to ensure that z-axis is pointing perfectly away from the hand

	const Quaternion additionalRotation = Quaternion(Vector3(0, 1, 0), left ? additionalRotationAngle : -additionalRotationAngle) * Quaternion(Vector3(1, 0, 0), additionalRotationAngle);

	world_T_hand = HomogenousMatrix4(xAxis, yAxis, zAxis, position) * additionalRotation;

	return true;
}

bool XRPlaygroundExperience::isHandGrabbing(const bool left, HomogenousMatrix4& world_T_hand, const Scalar minAngle, const Scalar maxAngle, const Scalar minTipDistance)
{
	ocean_assert(minAngle >= 0 && minAngle < Numeric::pi());
	ocean_assert(minAngle < maxAngle && maxAngle < Numeric::pi());

	ocean_assert(minTipDistance >= 0);

	Vectors3 world_T_jointPoints;
	if (!PlatformSpecific::get().handPoses().getHandJointPositions(left, world_T_jointPoints))
	{
		return false;
	}

	// first, we ensure that all fingers are curved, but not closed

	Scalar resultMedianAngle;
	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Index1, ovrHandBone_Index2, ovrHandBone_Index3}, nullptr, nullptr, &resultMedianAngle) || resultMedianAngle < minAngle || resultMedianAngle > maxAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Middle1, ovrHandBone_Middle2, ovrHandBone_Middle3}, nullptr, nullptr, &resultMedianAngle) || resultMedianAngle < minAngle || resultMedianAngle > maxAngle)
	{
		return false;
	}

	if (!Platform::Meta::Quest::VrApi::HandPoses::determineBoneAngles(world_T_jointPoints, {ovrHandBone_Ring1, ovrHandBone_Ring2, ovrHandBone_Ring3}, nullptr, nullptr, &resultMedianAngle) || resultMedianAngle < minAngle || resultMedianAngle > maxAngle)
	{
		return false;
	}

	const Scalar sqrMinTipDistance = Numeric::sqr(minTipDistance);

	// now, we ensure that all fingers tips are far away from each other

	if (world_T_jointPoints[ovrHandBone_ThumbTip].sqrDistance(world_T_jointPoints[ovrHandBone_IndexTip]) < sqrMinTipDistance
			|| world_T_jointPoints[ovrHandBone_ThumbTip].sqrDistance(world_T_jointPoints[ovrHandBone_MiddleTip]) < sqrMinTipDistance
			|| world_T_jointPoints[ovrHandBone_ThumbTip].sqrDistance(world_T_jointPoints[ovrHandBone_RingTip]) < sqrMinTipDistance
			|| world_T_jointPoints[ovrHandBone_ThumbTip].sqrDistance(world_T_jointPoints[ovrHandBone_PinkyTip]) < sqrMinTipDistance)
	{
		return false;
	}

	const Vector3 position = (world_T_jointPoints[ovrHandBone_ThumbTip] + world_T_jointPoints[ovrHandBone_IndexTip] + world_T_jointPoints[ovrHandBone_PinkyTip]) / Scalar(3);

	Vector3 xAxis = world_T_jointPoints[ovrHandBone_Pinky1] - world_T_jointPoints[ovrHandBone_Index1];

	if (left)
	{
		// the x-axis is pointing to the user's right when the palms are away from the user and to the left when the palms are towards the user (regardless of whether this is the left or right hand)
		xAxis = -xAxis;
	}

	Vector3 yAxis = world_T_jointPoints[ovrHandBone_Middle1] - world_T_jointPoints[ovrHandBone_WristRoot];

	Vector3 zAxis = xAxis.cross(yAxis);

	if (!zAxis.normalize() || !yAxis.normalize())
	{
		return false;
	}

	xAxis = yAxis.cross(zAxis);

	constexpr Scalar additionalRotationAngle = Numeric::deg2rad(20); // just some custom rotation to ensure that z-axis is pointing perfectly away from the hand

	const Quaternion additionalRotation = Quaternion(Vector3(0, 1, 0), left ? additionalRotationAngle : -additionalRotationAngle) * Quaternion(Vector3(1, 0, 0), additionalRotationAngle);

	world_T_hand = HomogenousMatrix4(xAxis, yAxis, zAxis, position) * additionalRotation;

	return true;
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

bool XRPlaygroundExperience::preLoad(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp /*timestamp*/, const std::string& /*properties*/)
{
	Log::debug() << "XRPlaygroundExperience::preLoad()";

	ocean_assert(engine);

	renderingExperienceEngine_ = engine;

	renderingExperienceScene_ = engine->factory().createScene();
	ocean_assert(renderingExperienceScene_);

	engine->framebuffers().front()->addScene(renderingExperienceScene_);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	PlatformSpecific::get().vrControllerVisualizer().pushState();
	PlatformSpecific::get().vrHandVisualizer().pushState();
	PlatformSpecific::get().mrPassthroughVisualizer().pushState();

#endif

	return true;
}

bool XRPlaygroundExperience::postUnload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp /*timestamp*/)
{
	Log::debug() << "XRPlaygroundExperience::postUnload()";

	ocean_assert(engine);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	PlatformSpecific::get().mrPassthroughVisualizer().popState();
	PlatformSpecific::get().vrHandVisualizer().popState();
	PlatformSpecific::get().vrControllerVisualizer().popState();

#endif

	ocean_assert(&*engine == &*renderingExperienceEngine_ && "The engine should never change!");

	if (renderingExperienceScene_)
	{
		engine->framebuffers().front()->removeScene(renderingExperienceScene_);
	}

	renderingExperienceScene_.release();
	renderingExperienceEngine_.release();

	return true;
}

void XRPlaygroundExperience::showMessage(const std::string& message, const HomogenousMatrix4& world_T_message)
{
	ocean_assert(!message.empty());
	ocean_assert(world_T_message.isValid());

	ocean_assert(experienceScene());
	ocean_assert(renderingExperienceEngine_);

	const Rendering::TransformRef transform = Rendering::Utilities::createText(*renderingExperienceEngine_, message, RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
	transform->setTransformation(world_T_message);

	experienceScene()->addChild(transform);
}

void XRPlaygroundExperience::showMessage(const MessageType messageType, const HomogenousMatrix4& world_T_message)
{
	std::string message;

	switch (messageType)
	{
		case MT_INVALID:
			break;

		case MT_CAMERA_ACCESS_FAILED:
			message = " Failed to access all cameras \n see https://fburl.com/access_cameras ";
			break;

		case MT_COLOR_CAMERA_NEEDED:
			message = " The device does not have a color camera ";
			break;
	}

	if (message.empty())
	{
		ocean_assert(false && "Invalid message");
		return;
	}

	showMessage(message, world_T_message);
}

}

}
