/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/HandPoses.h"

namespace Ocean
{

namespace Platform
{

using namespace OpenXR;

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

Vectors3 HandPoses::Mesh::vertices() const
{
	ocean_assert(isValid());

	Vectors3 result;
	result.reserve(vertexPositions_.size());

	for (const XrVector3f& vertexPosition : vertexPositions_)
	{
		result.emplace_back(Scalar(vertexPosition.x), Scalar(vertexPosition.y), Scalar(vertexPosition.z));
	}

	return result;
}

Vectors3 HandPoses::Mesh::normals() const
{
	ocean_assert(isValid());

	Vectors3 result;
	result.reserve(vertexNormals_.size());

	for (const XrVector3f& vertexNormal : vertexNormals_)
	{
		result.emplace_back(Scalar(vertexNormal.x), Scalar(vertexNormal.y), Scalar(vertexNormal.z));
	}

	return result;
}

template <typename T>
HomogenousMatricesT4<T> HandPoses::Mesh::wrist_T_bindJoints() const
{
	ocean_assert(isValid());

	HomogenousMatricesT4<T> result;
	result.reserve(jointBindPoses_.size());

	for (const XrPosef& jointBindPose : jointBindPoses_)
	{
		result.emplace_back(Utilities::toHomogenousMatrix4<T>(jointBindPose));
	}

	return result;
}

template HomogenousMatricesT4<float> OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Mesh::wrist_T_bindJoints() const;
template HomogenousMatricesT4<double> OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Mesh::wrist_T_bindJoints() const;

bool HandPoses::Mesh::initialize(const XrHandTrackerEXT& xrHandTrackerEXT, const PFN_xrGetHandMeshFB& xrGetHandMeshFB)
{
	ocean_assert(xrHandTrackerEXT != XR_NULL_HANDLE);
	ocean_assert(xrGetHandMeshFB != nullptr);

	if (xrHandTrackerEXT == XR_NULL_HANDLE || xrGetHandMeshFB == nullptr)
	{
		return false;
	}

	// first, we determine the necessary memory

	XrResult xrResult = xrGetHandMeshFB(xrHandTrackerEXT, &xrHandTrackingMeshFB_);
	ocean_assert_and_suppress_unused(xrResult == XR_SUCCESS, xrResult);

	// joints

	const uint32_t numberJoints = xrHandTrackingMeshFB_.jointCountOutput;

	xrHandTrackingMeshFB_.jointCapacityInput = numberJoints;

	jointBindPoses_.resize(numberJoints);
	xrHandTrackingMeshFB_.jointBindPoses = jointBindPoses_.data();

	jointRadii_.resize(numberJoints);
	xrHandTrackingMeshFB_.jointRadii = jointRadii_.data();

	jointParents_.resize(numberJoints);
	xrHandTrackingMeshFB_.jointParents = jointParents_.data();


	// vertices

	const uint32_t numberVertices = xrHandTrackingMeshFB_.vertexCountOutput;

	if (numberVertices == 0u)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	xrHandTrackingMeshFB_.vertexCapacityInput = numberVertices;

	vertexPositions_.resize(numberVertices);
	xrHandTrackingMeshFB_.vertexPositions = vertexPositions_.data();

	vertexNormals_.resize(numberVertices);
	xrHandTrackingMeshFB_.vertexNormals = vertexNormals_.data();

	vertexUVs_.resize(numberVertices);
	xrHandTrackingMeshFB_.vertexUVs = vertexUVs_.data();

	vertexBlendIndices_.resize(numberVertices);
	xrHandTrackingMeshFB_.vertexBlendIndices = vertexBlendIndices_.data();

	vertexBlendWeights_.resize(numberVertices);
	xrHandTrackingMeshFB_.vertexBlendWeights = vertexBlendWeights_.data();


	// indices

	const uint32_t numberIndices = xrHandTrackingMeshFB_.indexCountOutput;

	if (numberIndices == 0u || numberIndices % 3u != 0u)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	xrHandTrackingMeshFB_.indexCapacityInput = numberIndices;

	indices_.resize(numberIndices);
	xrHandTrackingMeshFB_.indices = indices_.data();

	// now, we fill the memory

	XrHandTrackingCapsulesStateFB xrHandTrackingCapsulesStateFB{XR_TYPE_HAND_TRACKING_CAPSULES_STATE_FB};
	xrHandTrackingMeshFB_.next = &xrHandTrackingCapsulesStateFB;

	xrResult = xrGetHandMeshFB(xrHandTrackerEXT, &xrHandTrackingMeshFB_);

	if (xrResult != XR_SUCCESS)
	{
		return false;
	}

	isValid_ = true;

	return true;
}

template <typename T>
bool HandPoses::Pose::jointPositions(VectorsT3<T>& baseSpaceJointPositions, const XrSpaceLocationFlags xrSpaceLocationFlags) const
{
	if (xrBaseSpace_ == XR_NULL_HANDLE)
	{
		return false;
	}

	baseSpaceJointPositions.reserve(XR_HAND_JOINT_COUNT_EXT);

	for (size_t jointIndex = 0; jointIndex < XR_HAND_JOINT_COUNT_EXT; ++jointIndex)
	{
		const XrHandJointLocationEXT& xrHandJointLocationEXT = xrHandJointLocationsEXT_[jointIndex];

		if ((xrHandJointLocationEXT.locationFlags & xrSpaceLocationFlags) == xrSpaceLocationFlags)
		{
			baseSpaceJointPositions.emplace_back(Utilities::toVector3<T>(xrHandJointLocationEXT.pose.position));
		}
		else
		{
			return false;
		}
	}

	return true;
}

template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::jointPositions(VectorsT3<float>&, const XrSpaceLocationFlags) const;
template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::jointPositions(VectorsT3<double>&, const XrSpaceLocationFlags) const;

template <typename T>
bool HandPoses::Pose::jointTransformations(HomogenousMatricesT4<T>& baseSpace_T_joints, const XrSpaceLocationFlags xrSpaceLocationFlags) const
{
	if (xrBaseSpace_ == XR_NULL_HANDLE)
	{
		return false;
	}

	baseSpace_T_joints.reserve(XR_HAND_JOINT_COUNT_EXT);

	for (size_t jointIndex = 0; jointIndex < XR_HAND_JOINT_COUNT_EXT; ++jointIndex)
	{
		const XrHandJointLocationEXT& xrHandJointLocationEXT = xrHandJointLocationsEXT_[jointIndex];

		if ((xrHandJointLocationEXT.locationFlags & xrSpaceLocationFlags) == xrSpaceLocationFlags)
		{
			baseSpace_T_joints.emplace_back(Utilities::toHomogenousMatrix4<T>(xrHandJointLocationEXT.pose));
		}
		else
		{
			return false;
		}
	}

	return true;
}

template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::jointTransformations(HomogenousMatricesT4<float>&, const XrSpaceLocationFlags) const;
template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::jointTransformations(HomogenousMatricesT4<double>&, const XrSpaceLocationFlags) const;

template <typename T>
bool HandPoses::Pose::jointTransformations(const Mesh& mesh, HomogenousMatricesT4<T>& baseSpace_T_joints, const XrSpaceLocationFlags xrSpaceLocationFlags) const
{
	if (xrBaseSpace_ == XR_NULL_HANDLE || !mesh.isValid())
	{
		return false;
	}

	baseSpace_T_joints.reserve(XR_HAND_JOINT_COUNT_EXT);

	ocean_assert(mesh.jointBindPoses_.size() == XR_HAND_JOINT_COUNT_EXT);

	for (size_t jointIndex = 0; jointIndex < XR_HAND_JOINT_COUNT_EXT; ++jointIndex)
	{
		const XrHandJointLocationEXT& xrHandJointLocationEXT = xrHandJointLocationsEXT_[jointIndex];

		if ((xrHandJointLocationEXT.locationFlags & xrSpaceLocationFlags) == xrSpaceLocationFlags)
		{
			const HomogenousMatrixT4<T> baseSpace_T_joint(Utilities::toHomogenousMatrix4<T>(xrHandJointLocationEXT.pose));
			const HomogenousMatrixT4<T> wrist_T_bindJoint(Utilities::toHomogenousMatrix4<T>(mesh.jointBindPoses_[jointIndex]));

			baseSpace_T_joints.emplace_back(baseSpace_T_joint * wrist_T_bindJoint.inverted());
		}
		else
		{
			return false;
		}
	}

	return true;
}

template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::jointTransformations<float>(const Mesh&, HomogenousMatricesT4<float>&, const XrSpaceLocationFlags) const;
template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::jointTransformations<double>(const Mesh&, HomogenousMatricesT4<double>&, const XrSpaceLocationFlags) const;

template <typename T>
bool HandPoses::Pose::meshVertices(const Mesh& mesh, VectorsT3<T>& baseSpaceMeshVertices, const XrSpaceLocationFlags xrSpaceLocationFlags) const
{
	ocean_assert(mesh.isValid());

	if (!mesh.isValid())
	{
		return false;
	}

	ocean_assert(!mesh.vertexPositions_.empty());
	ocean_assert(mesh.vertexPositions_.size() == mesh.vertexBlendIndices_.size());

	HomogenousMatricesF4 baseSpace_T_joints;
	if (!jointTransformations<float>(mesh, baseSpace_T_joints, xrSpaceLocationFlags))
	{
		return false;
	}

	ocean_assert(baseSpace_T_joints.size() == XR_HAND_JOINT_COUNT_EXT);

	baseSpaceMeshVertices.reserve(mesh.vertexPositions_.size());

	for (size_t vertexIndex = 0; vertexIndex < mesh.vertexPositions_.size(); ++vertexIndex)
	{
		const VectorF3 vertexPosition(Utilities::toVector3<float>(mesh.vertexPositions_[vertexIndex]));

		const XrVector4f& vertexBlendWeight = mesh.vertexBlendWeights_[vertexIndex];
		const XrVector4sFB& vertexBlendIndex = mesh.vertexBlendIndices_[vertexIndex];

		if (vertexBlendIndex.x >= XR_HAND_JOINT_COUNT_EXT || vertexBlendIndex.y >= XR_HAND_JOINT_COUNT_EXT || vertexBlendIndex.z >= XR_HAND_JOINT_COUNT_EXT || vertexBlendIndex.w >= XR_HAND_JOINT_COUNT_EXT)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const VectorF3 position0 = baseSpace_T_joints[vertexBlendIndex.x] * vertexPosition;
		const VectorF3 position1 = baseSpace_T_joints[vertexBlendIndex.y] * vertexPosition;
		const VectorF3 position2 = baseSpace_T_joints[vertexBlendIndex.z] * vertexPosition;
		const VectorF3 position3 = baseSpace_T_joints[vertexBlendIndex.w] * vertexPosition;

		baseSpaceMeshVertices.emplace_back(position0 * vertexBlendWeight.x + position1 * vertexBlendWeight.y + position2 * vertexBlendWeight.z + position3 * vertexBlendWeight.w);
	}

	return true;
}

template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::meshVertices(const Mesh&, VectorsT3<float>&, const XrSpaceLocationFlags) const;
template bool OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses::Pose::meshVertices(const Mesh&, VectorsT3<double>&, const XrSpaceLocationFlags) const;

HandPoses::HandPoses(HandPoses&& handPoses) :
	HandPoses()
{
	*this = std::move(handPoses);
}

bool HandPoses::initialize(const XrInstance& xrInstance, const XrSession& xrSession, const XrSystemId& xrSystemId)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSystemId != XR_NULL_SYSTEM_ID);

	if (xrInstance == XR_NULL_HANDLE || xrSystemId == XR_NULL_SYSTEM_ID)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (xrInstance_ != XR_NULL_HANDLE)
	{
		ocean_assert(false && "Hand tracking has already been initialized");
		return true;
	}

	XrSystemHandTrackingPropertiesEXT xrSystemHandTrackingPropertiesEXT{XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT};
	XrSystemProperties xrSystemProperties{XR_TYPE_SYSTEM_PROPERTIES, &xrSystemHandTrackingPropertiesEXT};

	XrResult xrResult = xrGetSystemProperties(xrInstance, xrSystemId, &xrSystemProperties);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine system properties: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	if (!xrSystemHandTrackingPropertiesEXT.supportsHandTracking)
	{
		Log::error() << "OpenXR HandPoses: Hand tracking is not supported";
		return false;
	}

	xrResult = xrGetInstanceProcAddr(xrInstance, "xrCreateHandTrackerEXT", (PFN_xrVoidFunction*)(&xrCreateHandTrackerEXT_));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine create hand tracker function: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	xrResult = xrGetInstanceProcAddr(xrInstance, "xrDestroyHandTrackerEXT", (PFN_xrVoidFunction*)(&xrDestroyHandTrackerEXT_));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine destroy hand tracker function: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	xrResult = xrGetInstanceProcAddr(xrInstance, "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)(&xrLocateHandJointsEXT_));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine local hand joints function: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	xrResult = xrGetInstanceProcAddr(xrInstance, "xrGetHandMeshFB", (PFN_xrVoidFunction*)(&xrGetHandMeshFB_));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine get hand mesh function: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	static_assert(leftHandIndex_ < numberHands_ && rightHandIndex_ < numberHands_, "Invalid indices");

	XrHandTrackerEXT& xrHandTrackerLeft = xrHandTrackersEXT_[leftHandIndex_];
	XrHandTrackerEXT& xrHandTrackerRight = xrHandTrackersEXT_[rightHandIndex_];

	XrHandTrackerCreateInfoEXT xrHandTrackerCreateInfoEXT{XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT};
    xrHandTrackerCreateInfoEXT.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
    xrHandTrackerCreateInfoEXT.hand = XR_HAND_LEFT_EXT;

	ocean_assert(xrHandTrackerLeft == XR_NULL_HANDLE);
	xrResult = xrCreateHandTrackerEXT_(xrSession, &xrHandTrackerCreateInfoEXT, &xrHandTrackerLeft);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine left hand tracker: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	xrHandTrackerCreateInfoEXT.hand = XR_HAND_RIGHT_EXT;

	ocean_assert(xrHandTrackerRight == XR_NULL_HANDLE);
	xrResult = xrCreateHandTrackerEXT_(xrSession, &xrHandTrackerCreateInfoEXT, &xrHandTrackerRight);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR HandPoses: Failed to determine right hand tracker: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	for (size_t handIndex = 0; handIndex < numberHands_; ++handIndex)
	{
		if (!meshes_[handIndex].initialize(xrHandTrackersEXT_[handIndex], xrGetHandMeshFB_))
		{
			Log::error() << "OpenXR: Failed to determine hand mesh";
			return false;
		}
	}

	xrInstance_ = xrInstance;

	return true;
}

void HandPoses::release()
{
	const ScopedLock scopedLock(lock_);

	if (xrDestroyHandTrackerEXT_ != nullptr)
	{
		for (XrHandTrackerEXT& xrHandTrackerEXT : xrHandTrackersEXT_)
		{
			if (xrHandTrackerEXT != XR_NULL_HANDLE)
			{
				xrDestroyHandTrackerEXT_(xrHandTrackerEXT);
				xrHandTrackerEXT = XR_NULL_HANDLE;
			}
		}
	}

	xrCreateHandTrackerEXT_ = nullptr;
	xrDestroyHandTrackerEXT_ = nullptr;
	xrLocateHandJointsEXT_ = nullptr;
	xrGetHandMeshFB_ = nullptr;

	xrInstance_ = XR_NULL_HANDLE;
}

bool HandPoses::update(const XrSpace& xrBaseSpace, const XrTime& predictedDisplayTime)
{
	ocean_assert(xrBaseSpace != XR_NULL_HANDLE);

	const ScopedLock scopedLock(lock_);

	if (xrInstance_ == XR_NULL_HANDLE)
	{
		return false;
	}

	for (size_t handIndex = 0; handIndex < numberHands_; ++handIndex)
	{
		if (!updateHandPose(xrHandTrackersEXT_[handIndex], xrLocateHandJointsEXT_, xrBaseSpace, predictedDisplayTime, poses_[handIndex]))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		baseSpaceJointPositions_[handIndex].clear();
	}

	return true;
}

const Vectors3& HandPoses::jointPositions(const size_t handIndex) const
{
	ocean_assert(handIndex < numberHands_);
	if (handIndex >= numberHands_)
	{
		return invalidResultVectors_;
	}

	const ScopedLock scopedLock(lock_);

	Vectors3& baseSpaceJointPositions = baseSpaceJointPositions_[handIndex];

	if (baseSpaceJointPositions.empty())
	{
		pose(handIndex).jointPositions(baseSpaceJointPositions);
	}

	return baseSpaceJointPositions;
}

HandPoses& HandPoses::operator=(HandPoses&& handPoses)
{
	if (this != &handPoses)
	{
		release();

		xrInstance_ = handPoses.xrInstance_;
		handPoses.xrInstance_ = XR_NULL_HANDLE;

		xrCreateHandTrackerEXT_ = handPoses.xrCreateHandTrackerEXT_;
		handPoses.xrCreateHandTrackerEXT_ = nullptr;

		xrDestroyHandTrackerEXT_ = handPoses.xrDestroyHandTrackerEXT_;
		handPoses.xrDestroyHandTrackerEXT_ = nullptr;

		xrLocateHandJointsEXT_ = handPoses.xrLocateHandJointsEXT_;
		handPoses.xrLocateHandJointsEXT_ = nullptr;

		xrGetHandMeshFB_ = handPoses.xrGetHandMeshFB_;
		handPoses.xrGetHandMeshFB_ = nullptr;

		for (size_t handIndex = 0; handIndex < numberHands_; ++handIndex)
		{
			xrHandTrackersEXT_[handIndex] = handPoses.xrHandTrackersEXT_[handIndex];
			handPoses.xrHandTrackersEXT_[handIndex] = {XR_NULL_HANDLE};

			meshes_[handIndex] = handPoses.meshes_[handIndex];
			handPoses.meshes_[handIndex] = Mesh();
		}
	}

	return *this;
}

const HandPoses::StringSet& HandPoses::necessaryOpenXRExtensionNames()
{
	static const StringSet extensionNames =
	{
		XR_EXT_HAND_TRACKING_EXTENSION_NAME,
		XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME,
		XR_FB_HAND_TRACKING_AIM_EXTENSION_NAME,
		XR_FB_HAND_TRACKING_CAPSULES_EXTENSION_NAME
	};

	return extensionNames;
}

bool HandPoses::updateHandPose(const XrHandTrackerEXT& xrHandTrackersEXT, const PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT, const XrSpace& xrBaseSpace, const XrTime& xrTime, Pose& pose)
{
	ocean_assert(xrHandTrackersEXT != XR_NULL_HANDLE);
	ocean_assert(xrLocateHandJointsEXT != nullptr);
	ocean_assert(xrBaseSpace != XR_NULL_HANDLE);

	pose.xrBaseSpace_ = XR_NULL_HANDLE;

	XrHandTrackingScaleFB xrHandTrackingScaleFB{XR_TYPE_HAND_TRACKING_SCALE_FB};
	xrHandTrackingScaleFB.sensorOutput = 1.0f;
	xrHandTrackingScaleFB.currentOutput = 1.0f;
	xrHandTrackingScaleFB.overrideValueInput = 1.00f;
	xrHandTrackingScaleFB.overrideHandScale = XR_FALSE;

	XrHandTrackingCapsulesStateFB xrHandTrackingCapsulesStateFB{XR_TYPE_HAND_TRACKING_CAPSULES_STATE_FB};
	xrHandTrackingCapsulesStateFB.next = &xrHandTrackingScaleFB;

	XrHandTrackingAimStateFB xrHandTrackingAimStateFB{XR_TYPE_HAND_TRACKING_AIM_STATE_FB};
	xrHandTrackingAimStateFB.next = &xrHandTrackingCapsulesStateFB;

	XrHandJointVelocitiesEXT xrHandJointVelocitiesEXT{XR_TYPE_HAND_JOINT_VELOCITIES_EXT};
	xrHandJointVelocitiesEXT.next = &xrHandTrackingAimStateFB;
	xrHandJointVelocitiesEXT.jointCount = XR_HAND_JOINT_COUNT_EXT;
	xrHandJointVelocitiesEXT.jointVelocities = pose.xrHandJointVelocitiesEXT_;

	XrHandJointLocationsEXT xrHandJointLocationsEXT{XR_TYPE_HAND_JOINT_LOCATIONS_EXT};
	xrHandJointLocationsEXT.next = &xrHandJointVelocitiesEXT;
	xrHandJointLocationsEXT.jointCount = XR_HAND_JOINT_COUNT_EXT;
	xrHandJointLocationsEXT.jointLocations = pose.xrHandJointLocationsEXT_;

	XrHandJointsLocateInfoEXT xrHandJointsLocateInfoEXT{XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT};
	xrHandJointsLocateInfoEXT.baseSpace = xrBaseSpace;
	xrHandJointsLocateInfoEXT.time = xrTime;

	if (xrLocateHandJointsEXT(xrHandTrackersEXT, &xrHandJointsLocateInfoEXT, &xrHandJointLocationsEXT) != XR_SUCCESS)
	{
		return false;
	}

	pose.xrBaseSpace_ = xrBaseSpace;

	return true;
}

}

}

}

}

}
