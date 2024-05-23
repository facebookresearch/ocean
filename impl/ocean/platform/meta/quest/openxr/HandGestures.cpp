/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/HandGestures.h"

#include "ocean/base/Median.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

bool HandGestures::determineBoneAngles(const Vectors3& jointPoints, const Indices32& jointIndices, Scalar* minAngle, Scalar* maxAngle, Scalar* medianAngle)
{
	ocean_assert(!jointPoints.empty());
	ocean_assert(jointIndices.size() >= 3);

	if (jointIndices.size() < 3)
	{
		return false;
	}

	Scalar internalMinAngle = Numeric::pi();
	Scalar internalMaxAngle = 0;

	if (jointIndices[0] >= jointPoints.size() || jointIndices[1] >= jointPoints.size())
	{
		return false;
	}

	ocean_assert(jointIndices[0] != jointIndices[1]);

	Vector3 previousBone = jointPoints[jointIndices[1]] - jointPoints[jointIndices[0]];

	Scalars angles;

	if (medianAngle != nullptr)
	{
		angles.reserve(jointIndices.size());
	}

	for (size_t n = 2; n < jointIndices.size(); ++n)
	{
		if (jointIndices[n] >= jointPoints.size())
		{
			return false;
		}

		ocean_assert(jointIndices[n] != jointIndices[n - 1]);

		const Vector3 newBone = jointPoints[jointIndices[n]] - jointPoints[jointIndices[n - 1]];

		const Scalar angle = previousBone.angle(newBone);

		internalMinAngle = std::min(angle, internalMinAngle);
		internalMaxAngle = std::max(angle, internalMaxAngle);

		previousBone = newBone;

		if (medianAngle != nullptr)
		{
			angles.emplace_back(angle);
		}
	}

	if (minAngle != nullptr)
	{
		*minAngle = internalMinAngle;
	}

	if (maxAngle != nullptr)
	{
		*maxAngle = internalMaxAngle;
	}

	if (medianAngle != nullptr)
	{
		*medianAngle = Median::median(angles.data(), angles.size());
	}

	return true;
}

Scalar HandGestures::distanceBetweenFingerJoints(const Vectors3& jointPoints, const Index32 joint0, const Index32 joint1, Vector3* position0, Vector3* position1)
{
	if (joint0 == joint1)
	{
		return Scalar(0);
	}

	if (joint0 >= jointPoints.size() || joint1 >= jointPoints.size())
	{
		ocean_assert(false && "Invalid bones!");
		return Numeric::maxValue();
	}

	if (position0 != nullptr)
	{
		*position0 = jointPoints[joint0];
	}

	if (position1 != nullptr)
	{
		*position1 = jointPoints[joint1];
	}

	return jointPoints[joint0].distance(jointPoints[joint1]);
}

bool HandGestures::isHandPinching(const Vectors3& jointPositions, Vector3* position, const Scalar maxDistance)
{
	if (jointPositions.size() != XR_HAND_JOINT_COUNT_EXT)
	{
		return false;
	}

	ocean_assert(maxDistance >= Scalar(0));

	Vector3 thumbPosition;
	Vector3 indexPosition;
	const Scalar distance = distanceBetweenFingerJoints(jointPositions, XR_HAND_JOINT_THUMB_TIP_EXT, XR_HAND_JOINT_INDEX_TIP_EXT, &thumbPosition, &indexPosition);

	if (position != nullptr)
	{
		*position = (thumbPosition + indexPosition) * Scalar(0.5);
	}

	return distance <= maxDistance;
}

bool HandGestures::isHandTunneling(const Vectors3& jointPositions, const bool isLeft, HomogenousMatrix4& world_T_tunnelCenter, const Scalar maxTipDistance, const Scalar minTunnelDistance)
{
	if (jointPositions.size() != XR_HAND_JOINT_COUNT_EXT)
	{
		return false;
	}

	ocean_assert(maxTipDistance >= 0);
	ocean_assert(minTunnelDistance >= 0);

	const Scalar distance = distanceBetweenFingerJoints(jointPositions, XR_HAND_JOINT_THUMB_TIP_EXT, XR_HAND_JOINT_INDEX_TIP_EXT);

	if (distance > maxTipDistance)
	{
		return false;
	}

	Vector3 xAxis = jointPositions[XR_HAND_JOINT_INDEX_PROXIMAL_EXT] - jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]; // e.g., ovrHandBone_Index1 == index proximal phalange bone
	Vector3 yAxis = jointPositions[XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT] - jointPositions[XR_HAND_JOINT_THUMB_PROXIMAL_EXT];

	const Scalar sqrMinTunnelDistance = Numeric::sqr(minTunnelDistance);

	if (xAxis.sqr() < sqrMinTunnelDistance || yAxis.sqr() < sqrMinTunnelDistance)
	{
		return false;
	}

	if (xAxis.isNull() || yAxis.isNull())
	{
		return false;
	}

	if (isLeft)
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
		XR_HAND_JOINT_INDEX_TIP_EXT, XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT, XR_HAND_JOINT_INDEX_PROXIMAL_EXT, XR_HAND_JOINT_THUMB_PROXIMAL_EXT
	};

	Vector3 sumPosition(0, 0, 0);
	for (const Index32 jointIndex : jointIndices)
	{
		sumPosition += jointPositions[jointIndex];
	}

	const Vector3 translation = sumPosition / Scalar(jointIndices.size());

	world_T_tunnelCenter = HomogenousMatrix4(xAxis, yAxis, zAxis, translation);

	return true;
}

bool HandGestures::isHandPointing(const Vectors3& jointPositions, Line3& worldRay, const Scalar maxIndexAngle, const Scalar minAngle, const Scalar minTipDistance, const Scalar maxTipDistance)
{
	if (jointPositions.size() != XR_HAND_JOINT_COUNT_EXT)
	{
		return false;
	}

	ocean_assert(maxIndexAngle >= 0 && maxIndexAngle < Numeric::pi());
	ocean_assert(minAngle >= 0 && minAngle < Numeric::pi());

	ocean_assert(minTipDistance >= 0);
	ocean_assert(maxTipDistance >= 0);

	// first, we ensure that the index finger is a straight line, and other fingers are not straight

	Scalar resultMaxAngle;
	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_INDEX_PROXIMAL_EXT, XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT, XR_HAND_JOINT_INDEX_DISTAL_EXT, XR_HAND_JOINT_INDEX_TIP_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle > maxIndexAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_WRIST_EXT, XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT, XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT, XR_HAND_JOINT_MIDDLE_DISTAL_EXT, XR_HAND_JOINT_MIDDLE_TIP_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle < minAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_WRIST_EXT, XR_HAND_JOINT_RING_PROXIMAL_EXT, XR_HAND_JOINT_RING_INTERMEDIATE_EXT, XR_HAND_JOINT_RING_DISTAL_EXT, XR_HAND_JOINT_RING_TIP_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle < minAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_LITTLE_METACARPAL_EXT, XR_HAND_JOINT_LITTLE_PROXIMAL_EXT, XR_HAND_JOINT_LITTLE_INTERMEDIATE_EXT, XR_HAND_JOINT_LITTLE_DISTAL_EXT, XR_HAND_JOINT_LITTLE_TIP_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle < minAngle)
	{
		return false;
	}

	const Scalar sqrMinTipDistance = Numeric::sqr(minTipDistance);
	const Scalar sqrMaxTipDistance = Numeric::sqr(maxTipDistance);

	// now, we ensure that the remaining fingers tips are close to each other

	if (jointPositions[XR_HAND_JOINT_LITTLE_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_RING_TIP_EXT]) > sqrMaxTipDistance
			|| jointPositions[XR_HAND_JOINT_RING_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_MIDDLE_TIP_EXT]) > sqrMaxTipDistance
			|| jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT]) > sqrMaxTipDistance)
	{
		return false;
	}

	// now, we ensure that the remaining fingers are far away from the index tip

	if (jointPositions[XR_HAND_JOINT_LITTLE_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_RING_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_MIDDLE_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]) < sqrMinTipDistance)
	{
		return false;
	}

	Vector3 direction = jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT] - jointPositions[XR_HAND_JOINT_INDEX_PROXIMAL_EXT];

	if (!direction.normalize())
	{
		return false;
	}

	worldRay = Line3(jointPositions[XR_HAND_JOINT_INDEX_PROXIMAL_EXT], direction);

	return true;
}

bool HandGestures::isHandSpreading(const Vectors3& jointPositions, const bool isLeft, HomogenousMatrix4& world_T_hand, const Scalar maxAngle, const Scalar minTipDistance)
{
	if (jointPositions.size() != XR_HAND_JOINT_COUNT_EXT)
	{
		return false;
	}

	ocean_assert(maxAngle >= 0 && maxAngle < Numeric::pi());

	ocean_assert(minTipDistance >= 0);

	// first, we ensure that all fingers are straight lines

	Scalar resultMaxAngle;
	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_INDEX_PROXIMAL_EXT, XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT, XR_HAND_JOINT_INDEX_DISTAL_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT, XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT, XR_HAND_JOINT_MIDDLE_DISTAL_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_RING_PROXIMAL_EXT, XR_HAND_JOINT_RING_INTERMEDIATE_EXT, XR_HAND_JOINT_RING_DISTAL_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_LITTLE_PROXIMAL_EXT, XR_HAND_JOINT_LITTLE_INTERMEDIATE_EXT, XR_HAND_JOINT_LITTLE_DISTAL_EXT}, nullptr, &resultMaxAngle) || resultMaxAngle > maxAngle)
	{
		return false;
	}

	const Scalar sqrMinTipDistance = Numeric::sqr(minTipDistance);

	// now, we ensure that all fingers tips are far away from each other

	if (jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_MIDDLE_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_MIDDLE_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_RING_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_RING_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_LITTLE_TIP_EXT]) < sqrMinTipDistance)
	{
		return false;
	}

	const Vector3 position = (jointPositions[XR_HAND_JOINT_WRIST_EXT] + jointPositions[XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT]) * Scalar(0.5);

	Vector3 xAxis = jointPositions[XR_HAND_JOINT_LITTLE_PROXIMAL_EXT] - jointPositions[XR_HAND_JOINT_INDEX_PROXIMAL_EXT];

	if (isLeft)
	{
		// the x-axis is pointing to the user's right when the palms are away from the user and to the left when the palms are towards the user (regardless of whether this is the left or right hand)
		xAxis = -xAxis;
	}

	Vector3 yAxis = jointPositions[XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT] - jointPositions[XR_HAND_JOINT_WRIST_EXT];

	Vector3 zAxis = xAxis.cross(yAxis);

	if (!zAxis.normalize() || !yAxis.normalize())
	{
		return false;
	}

	xAxis = yAxis.cross(zAxis);

	constexpr Scalar additionalRotationAngle = Numeric::deg2rad(10); // just some custom rotation to ensure that z-axis is pointing perfectly away from the hand

	const Quaternion additionalRotation = Quaternion(Vector3(0, 1, 0), isLeft ? additionalRotationAngle : -additionalRotationAngle) * Quaternion(Vector3(1, 0, 0), additionalRotationAngle);

	world_T_hand = HomogenousMatrix4(xAxis, yAxis, zAxis, position) * additionalRotation;

	return true;
}

bool HandGestures::isHandGrabbing(const Vectors3& jointPositions, const bool isLeft, HomogenousMatrix4& world_T_hand, const Scalar minAngle, const Scalar maxAngle, const Scalar minTipDistance)
{
	if (jointPositions.size() != XR_HAND_JOINT_COUNT_EXT)
	{
		return false;
	}

	ocean_assert(minAngle >= 0 && minAngle < Numeric::pi());
	ocean_assert(minAngle < maxAngle && maxAngle < Numeric::pi());

	ocean_assert(minTipDistance >= 0);

	// first, we ensure that all fingers are curved, but not closed

	Scalar resultMedianAngle;
	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_INDEX_PROXIMAL_EXT, XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT, XR_HAND_JOINT_INDEX_DISTAL_EXT}, nullptr, nullptr, &resultMedianAngle) || resultMedianAngle < minAngle || resultMedianAngle > maxAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT, XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT, XR_HAND_JOINT_MIDDLE_DISTAL_EXT}, nullptr, nullptr, &resultMedianAngle) || resultMedianAngle < minAngle || resultMedianAngle > maxAngle)
	{
		return false;
	}

	if (!determineBoneAngles(jointPositions, {XR_HAND_JOINT_RING_PROXIMAL_EXT, XR_HAND_JOINT_RING_INTERMEDIATE_EXT, XR_HAND_JOINT_RING_DISTAL_EXT}, nullptr, nullptr, &resultMedianAngle) || resultMedianAngle < minAngle || resultMedianAngle > maxAngle)
	{
		return false;
	}

	const Scalar sqrMinTipDistance = Numeric::sqr(minTipDistance);

	// now, we ensure that all fingers tips are far away from each other

	if (jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_MIDDLE_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_RING_TIP_EXT]) < sqrMinTipDistance
			|| jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT].sqrDistance(jointPositions[XR_HAND_JOINT_LITTLE_TIP_EXT]) < sqrMinTipDistance)
	{
		return false;
	}

	const Vector3 position = (jointPositions[XR_HAND_JOINT_THUMB_TIP_EXT] + jointPositions[XR_HAND_JOINT_INDEX_TIP_EXT] + jointPositions[XR_HAND_JOINT_LITTLE_TIP_EXT]) / Scalar(3);

	Vector3 xAxis = jointPositions[XR_HAND_JOINT_LITTLE_PROXIMAL_EXT] - jointPositions[XR_HAND_JOINT_INDEX_PROXIMAL_EXT];

	if (isLeft)
	{
		// the x-axis is pointing to the user's right when the palms are away from the user and to the left when the palms are towards the user (regardless of whether this is the left or right hand)
		xAxis = -xAxis;
	}

	Vector3 yAxis = jointPositions[XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT] - jointPositions[XR_HAND_JOINT_WRIST_EXT];

	Vector3 zAxis = xAxis.cross(yAxis);

	if (!zAxis.normalize() || !yAxis.normalize())
	{
		return false;
	}

	xAxis = yAxis.cross(zAxis);

	constexpr Scalar additionalRotationAngle = Numeric::deg2rad(20); // just some custom rotation to ensure that z-axis is pointing perfectly away from the hand

	const Quaternion additionalRotation = Quaternion(Vector3(0, 1, 0), isLeft ? additionalRotationAngle : -additionalRotationAngle) * Quaternion(Vector3(1, 0, 0), additionalRotationAngle);

	world_T_hand = HomogenousMatrix4(xAxis, yAxis, zAxis, position) * additionalRotation;

	return true;
}

}

}

}

}

}
