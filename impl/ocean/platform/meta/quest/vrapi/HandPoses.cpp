// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/HandPoses.h"
#include "ocean/platform/meta/quest/vrapi/Utilities.h"

#include "ocean/base/Median.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

#if defined(OCEAN_VRAPI_VERSION) && OCEAN_VRAPI_VERSION >= 1031

HandPoses::HandPoses() :
	ovrMobile_(nullptr),
	isLeftHandTracked_(false),
	isRightHandTracked_(false),
	world_T_leftHand_(false),
	world_T_rightHand_(false)
{
	// nothing to do here
}

HandPoses::HandPoses(HandPoses&& handPoses) :
	HandPoses()
{
	*this = std::move(handPoses);
}

HandPoses::HandPoses(ovrMobile* ovr) :
	ovrMobile_(ovr),
	isLeftHandTracked_(false),
	isRightHandTracked_(false),
	world_T_leftHand_(false),
	world_T_rightHand_(false)
{
	ocean_assert(ovrMobile_ != nullptr);
}

void HandPoses::update(const double absTimeInSeconds)
{
	const ScopedLock scopedLock(lock_);

	if (ovrMobile_ == nullptr || absTimeInSeconds < 0.0)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	// Get the device indices of the left and right hands

	if (leftHandDeviceIndex_ == invalidDeviceIndex)
	{
		leftHandDeviceIndex_ = getHandControllerDeviceID(ovrMobile_, /* left */ true);
	}

	if (rightHandDeviceIndex_ == invalidDeviceIndex)
	{
		rightHandDeviceIndex_ = getHandControllerDeviceID(ovrMobile_, /* left */ false);
	}

	ocean_assert(leftHandDeviceIndex_ != rightHandDeviceIndex_ || (leftHandDeviceIndex_ == invalidDeviceIndex && rightHandDeviceIndex_ == invalidDeviceIndex));

	// Get the hand skeletons and rest poses

	for (const bool leftHand : {true, false})
	{
		Joints& handSkeleton = leftHand ? leftHandSkeleton_ : rightHandSkeleton_;

		if (handSkeleton.empty())
		{
			handSkeleton = getHandSkeleton(ovrMobile_, leftHand);

			HomogenousMatrices4& world_T_jointRestPoses = leftHand ? world_T_leftJointRestPoses_ : world_T_rightJointRestPoses_;
			ocean_assert(world_T_jointRestPoses.empty());

			// The rest pose of the hand is defined at the origin of the world coordinate system, so chain all transformations using the identity
			if (!computeChainedTransformations(handSkeleton, /* identity */ HomogenousMatrix4(true), world_T_jointRestPoses) || world_T_jointRestPoses.size() != handSkeleton.size())
			{
				ocean_assert(false && "This should never happen!");
			}
		}
	}

	// Query hand poses

	for (size_t h = 0; h < 2; ++h)
	{
		const bool isLeftHand = h == 0;

		const ovrDeviceID deviceIndex = isLeftHand ? leftHandDeviceIndex_ : rightHandDeviceIndex_;
		HomogenousMatrix4& world_T_hand = isLeftHand ? world_T_leftHand_ : world_T_rightHand_;
		Joints& handSkeleton = isLeftHand ? leftHandSkeleton_ : rightHandSkeleton_;
		bool& isHandTracked = isLeftHand ? isLeftHandTracked_ : isRightHandTracked_;

		if (deviceIndex == invalidDeviceIndex)
		{
			continue;
		}

		ovrHandPose handPose;
		handPose.Header.Version = ovrHandVersion_1;
		const ovrResult handPoseStatus = vrapi_GetHandPose(ovrMobile_, deviceIndex, absTimeInSeconds, &(handPose.Header));

		isHandTracked = handPoseStatus == ovrSuccess && handPose.Status == ovrHandTrackingStatus_Tracked && handPose.HandConfidence == ovrConfidence_HIGH;

		if (isHandTracked)
		{
			world_T_hand = Utilities::toHomogenousMatrix4<Scalar>(handPose.RootPose);
			ocean_assert(world_T_hand.isValid());

			ocean_assert(handSkeleton.size() <= size_t(ovrHand_MaxBones));
			for (size_t i = 0; i < handSkeleton.size(); ++i)
			{
				handSkeleton[i].parentJoint_T_joint().setRotation(Quaternion(Scalar(handPose.BoneRotations[i].w), Scalar(handPose.BoneRotations[i].x), Scalar(handPose.BoneRotations[i].y), Scalar(handPose.BoneRotations[i].z)));
				ocean_assert(handSkeleton[i].isValid());
			}
		}
		else
		{
			world_T_hand.toNull();
		}
	}
}

bool HandPoses::getHandRoot(const bool left, HomogenousMatrix4& world_T_hand) const
{
	const ScopedLock scopedLock(lock_);

	if (left)
	{
		if (!world_T_leftHand_.isValid())
		{
			return false;
		}

		world_T_hand = world_T_leftHand_;
		return true;
	}
	else
	{
		if (!world_T_rightHand_.isValid())
		{
			return false;
		}

		world_T_hand = world_T_rightHand_;
		return true;
	}
}


bool HandPoses::getHandPose(const bool left, HomogenousMatrices4& world_T_joints) const
{
	const ScopedLock scopedLock(lock_);

	const HomogenousMatrix4& world_T_hand = left ? world_T_leftHand_ : world_T_rightHand_;
	const Joints& handSkeletonJoints = left ? leftHandSkeleton_ : rightHandSkeleton_;
	const bool& isHandTracked = left ? isLeftHandTracked_ : isRightHandTracked_;

	if (handSkeletonJoints.empty() || isHandTracked == false)
	{
		return false;
	}

	return computeChainedTransformations(handSkeletonJoints, world_T_hand, world_T_joints);
}

bool HandPoses::getHandJointPositions(const bool left, Vectors3& world_T_jointPoints) const
{
	const ScopedLock scopedLock(lock_);

	HomogenousMatrices4 world_T_joints;
	if (!getHandPose(left, world_T_joints))
	{
		return false;
	}

	world_T_jointPoints.reserve(world_T_joints.size());

	for (const HomogenousMatrix4& world_T_joint : world_T_joints)
	{
		world_T_jointPoints.emplace_back(world_T_joint.translation());
	}

	return !world_T_jointPoints.empty();
}

bool HandPoses::determineBoneAngles(const Vectors3& jointPoints, const Indices32& jointIndices, Scalar* minAngle, Scalar* maxAngle, Scalar* medianAngle)
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

HandPoses& HandPoses::operator=(HandPoses&& handPoses)
{
	if (this != &handPoses)
	{
		ovrMobile_ = handPoses.ovrMobile_;
		handPoses.ovrMobile_ = nullptr;

		leftHandDeviceIndex_ = handPoses.leftHandDeviceIndex_;
		rightHandDeviceIndex_ = handPoses.rightHandDeviceIndex_;
		handPoses.leftHandDeviceIndex_ = invalidDeviceIndex;
		handPoses.rightHandDeviceIndex_ = invalidDeviceIndex;

		isLeftHandTracked_ = handPoses.isLeftHandTracked_;
		isRightHandTracked_ = handPoses.isRightHandTracked_;
		handPoses.isLeftHandTracked_ = false;
		handPoses.isRightHandTracked_ = false;

		world_T_leftHand_ = handPoses.world_T_leftHand_;
		world_T_rightHand_ = handPoses.world_T_rightHand_;
		handPoses.world_T_leftHand_.toNull();
		handPoses.world_T_rightHand_.toNull();

		leftHandSkeleton_ = std::move(handPoses.leftHandSkeleton_);
		rightHandSkeleton_ = std::move(handPoses.rightHandSkeleton_);

		world_T_leftJointRestPoses_ = std::move(handPoses.world_T_leftJointRestPoses_);
		world_T_rightJointRestPoses_ = std::move(handPoses.world_T_rightJointRestPoses_);
	}

	return *this;
}

bool HandPoses::computeChainedTransformations(const Joints& joints, const HomogenousMatrix4& world_T_rootJoint, HomogenousMatrices4& world_T_joints)
{
	if (joints.empty() || !world_T_rootJoint.isValid())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	ocean_assert(joints.size() <= size_t(ovrHand_MaxBones));
	world_T_joints.resize(joints.size());

	ocean_assert(joints[0].isRoot());
	world_T_joints[0] = world_T_rootJoint * joints[0].parentJoint_T_joint();

	for (size_t i = 1; i < joints.size(); ++i)
	{
		ocean_assert(i > joints[i].parentIndex() && "Index of current joint must be larger than index of its parent joint");
		world_T_joints[i] = world_T_joints[joints[i].parentIndex()] * joints[i].parentJoint_T_joint();
	}

	return !world_T_joints.empty();
}

ovrDeviceID HandPoses::getHandControllerDeviceID(ovrMobile* ovr, const bool left)
{
	ocean_assert(ovr != nullptr);

	const uint32_t handCapabilities = left ? ovrHandCaps_LeftHand : ovrHandCaps_RightHand;
	uint32_t deviceIndex = 0u;
	ovrInputCapabilityHeader inputCapabilityHeader;

	while (vrapi_EnumerateInputDevices(ovr, deviceIndex, &inputCapabilityHeader) == ovrSuccess)
	{
		if (inputCapabilityHeader.Type == ovrControllerType_Hand)
		{
			ovrInputHandCapabilities inputHandCapabilities;
			inputHandCapabilities.Header = inputCapabilityHeader;

			if (vrapi_GetInputDeviceCapabilities(ovr, &(inputHandCapabilities.Header)) == ovrSuccess)
			{
				if (inputHandCapabilities.HandCapabilities == handCapabilities)
				{
					return inputCapabilityHeader.DeviceID;
				}
			}
		}

		++deviceIndex;
	}

	return invalidDeviceIndex;
}

HandPoses::Joints HandPoses::getHandSkeleton(ovrMobile* ovr, const bool left)
{
	if (ovr == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return Joints();
	}

	const ovrHandedness handedness = left ? VRAPI_HAND_LEFT : VRAPI_HAND_RIGHT;

	ovrHandSkeleton handSkeleton;
	handSkeleton.Header.Version = ovrHandVersion_1;
	const ovrResult haveSkeleton = vrapi_GetHandSkeleton(ovr, handedness, &(handSkeleton.Header));

	if (haveSkeleton != ovrSuccess)
	{
		return Joints();
	}

	ocean_assert(handSkeleton.NumBones != 0u);
	ocean_assert(handSkeleton.NumBones <= ovrHand_MaxBones);

	Joints skeletonJoints;
	skeletonJoints.reserve(size_t(handSkeleton.NumBones));

	for (uint32_t boneIndex = 0u; boneIndex < handSkeleton.NumBones; ++boneIndex)
	{
		const ovrHandBoneIndex ovrParentBoneIndex = handSkeleton.BoneParentIndices[boneIndex];
		ocean_assert(((boneIndex == 0u && ovrParentBoneIndex < ovrHandBoneIndex(0)) || uint32_t(ovrParentBoneIndex) < boneIndex) && "Index of current joint must be larger than index of its parent joint");

		const HomogenousMatrix4 parentBone_T_bone = Utilities::toHomogenousMatrix4<Scalar>(handSkeleton.BonePoses[boneIndex]);

		skeletonJoints.emplace_back(parentBone_T_bone, ovrParentBoneIndex < 0 ? Index32(-1) : Index32(ovrParentBoneIndex));
	}

	return skeletonJoints;
}

#endif // defined(OCEAN_VRAPI_VERSION) && OCEAN_VRAPI_VERSION >= 1031

} // namespace VrApi

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
