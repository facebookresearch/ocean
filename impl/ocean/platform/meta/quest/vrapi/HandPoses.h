// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_HAND_POSES_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_HAND_POSES_H

#include "ocean/platform/meta/quest/vrapi/VrApi.h"

#include "ocean/math/HomogenousMatrix4.h"

#include <VrApi_Input.h>
#include <VrApi_Version.h>

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

/**
 * This class implements a lookup for most recent hand poses.
 * The class uses the VrAPI function vrapi_GetHandPose() and vrapi_GetHandSkeleton() to determine the most recent hand tracking samples.
 * @ingroup platformmetaquestvrapi
 */
class HandPoses
{
	public:

		/**
		 * Definition of a joint element of the hand skeleton
		 */
		class Joint
		{
			public:

				/**
				 * Default constructor that creates an invalid joint object
				 */
				inline Joint();

				/**
				 * Constructor to create a joint object
				 * @param parentJoint_T_joint The rigid body transformation that describes the pose of the joint, must be valid
				 * @param parentIndex The index of the joint that is the parent of this joint, range: [0, infinity)
				 */
				inline Joint(const HomogenousMatrix4& parentJoint_T_joint, const Index32 parentIndex);

				/**
				 * Returns true if the joint is valid
				 * @return True if the joint has a valid transformation, otherwise false
				 */
				inline bool isValid() const;

				/**
				 * Returns a const reference of the transformation of this joint
				 * @return The const reference of the transformation
				 */
				inline const HomogenousMatrix4& parentJoint_T_joint() const;

				/**
				 * Returns a reference of the transformation of this joint
				 * @return The reference of the transformation
				 */
				inline HomogenousMatrix4& parentJoint_T_joint();

				/**
				 * Returns the index of the parent joint.
				 * @return The index of the parent joint, -1 if the joint is the root joint.
				 */
				inline Index32 parentIndex() const;

				/**
				 * Returns whether this joint is the root joint.
				 * @return True, if so
				 */
				inline bool isRoot() const;

			protected:

				/// Pose of this joint in relation to parent joint.
				HomogenousMatrix4 parentJoint_T_joint_;

				/// Index of parent joint
				Index32 parentIndex_;
		};

		/// Vector of hand joints
		typedef std::vector<Joint> Joints;

	protected:

		/// An invalid device index
		static constexpr ovrDeviceID invalidDeviceIndex = 0u;

	public:

		/**
		 * Default constructor
		 */
		HandPoses();

		/**
		 * Movey constructor.
		 * @param handPoses Object to be moved
		 */
		HandPoses(HandPoses&& handPoses);

		/**
		 * Disabled copy constructor.
		 * @param handPoses Object to be copied
		 */
		HandPoses(const HandPoses& handPoses) = delete;

		/**
		 * Creates a new object with given OVR SDK object
		 * @param ovr The OVR SDK object, must be valid
		 */
		HandPoses(ovrMobile* ovr);

		/**
		 * Updates the pose of the left and right hands to that of the specifed time stamp
		 * @param absTimeInSeconds The time stamp for which the pose of the hand will be updated to, range: [0, infinity) (according to the VrAPi the value 0 will grab the most recent pose)
		 */
		void update(const double absTimeInSeconds = 0.0);

		/**
		 * Returns the hand's root defined in world.
		 * Note: you need to call `update()` one per frame before calling this function
		 * @param left True, to return the root of the left hand; False, to return the root of the right hand
		 * @param world_T_hand The root transformation of the hands defined in world
		 * @return True, if the transformation is available
		 */
		bool getHandRoot(const bool left, HomogenousMatrix4& world_T_hand) const;

		/**
		 * Determines the pose of a hand in world coordinates
		 * Note: you need to call `update()` one per frame before calling this function
		 * @param left Indicates the left or right hand
		 * @param world_T_joints The rigid body transformations of the joints of the selected hand (will have at most `ovrHand_MaxBones` elements), each transformation transforms the individual joint to world
		 * @return True if the pose is available, otherwise false
		 */
		bool getHandPose(const bool left, HomogenousMatrices4& world_T_joints) const;

		/**
		 * Determines the joint positions of a hand in world coordinates.
		 * Note: you need to call `update()` one per frame before calling this function
		 * @param left Indicates the left (`true`) or right hand (`false`)
		 * @param world_T_jointPoints The 3D positions of the joints of the selected hand (will have at most `ovrHand_MaxBones` elements)
		 * @return True if the pose is available, otherwise false
		 */
		bool getHandJointPositions(const bool left, Vectors3& world_T_jointPoints) const;

		/**
		 * Returns the rest poses of the left or right hand joints
		 * The rest pose of a hand is defined by the mesh that is returned by the VrApi. Its hand root is centered at the world origin.
		 * Each matrix maps from the coordinate system of the corresponding joint to the world, i.e., `world_T_jointRestPose[i]`.
		 * Note: you need to call `update()` before calling this function
		 * @param left Indicates the left or right hand
		 * @return A constant reference to the pose matrices
		 */
		inline const HomogenousMatrices4& getHandRestPose(const bool left) const;

		/**
		 * Returns whether this object has been initialized with a valid OVR SDK object and thus can be used.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Move operator.
		 * @param handPoses The object to be moved
		 * @return Reference to this object
		 */
		HandPoses& operator=(HandPoses&& handPoses);

		/**
		 * Disabled copy operator.
		 * @param handPoses The object to be copied
		 * @return Reference to this object
		 */
		HandPoses& operator=(const HandPoses& handPoses) = delete;

		/**
		 * Determines the minimal and maximal angle between consecutive bones.
		 * @param jointPoints The 3D locations of the hand joints, at least 3
		 * @param jointIndices The indices of consecutive hand joints (e.g., of a finger), two indices define a bone, at least 3
		 * @param minAngle Optional resulting minimal angle between all consecutive bones, in radian, with range [0, maxAngle]
		 * @param maxAngle Optional resulting maximal angle between all consecutive bones, in radian, with range [minAngle, PI]
		 * @param medianAngle Optional resulting median angle between all consecutive bones, in radian, with range [minAngle, maxAngle]
		 * @return True, if succeeded
		 */
		static bool determineBoneAngles(const Vectors3& jointPoints, const Indices32& jointIndices, Scalar* minAngle = nullptr, Scalar* maxAngle = nullptr, Scalar* medianAngle = nullptr);

	protected:

		/**
		 * Determines the pose of a hand in world coordinates
		 * This functions positions the root joint of the hand at `world_T_rootJoint` and chains the remaining `parentJoint_T_joint`-transformations to map from the joint coordinate system to world coordinate system, i.e., `world_T_joint`
		 * @param joints The joints of the hand skeleton, must be valid
		 * @param world_T_rootJoint The pose of the hand (root), must be valid
		 * @param world_T_joints The resulting transformations that convert local joint coordinates into world coordinates, only valid if the return value is true
		 * @return True, if the joint poses were computed successfully
		 */
		static bool computeChainedTransformations(const Joints& joints, const HomogenousMatrix4& world_T_rootJoint, HomogenousMatrices4& world_T_joints);

		/**
		 * Determines the device index of the hand controllers
		 * @param ovr The OVR SDK object, must be valid
		 * @param left Indicates the left (`true`) or right hand (`false`)
		 * @return The device index of the selected hand controller, will be `invalidDeviceIndex` in case of an error
		 */
		static ovrDeviceID getHandControllerDeviceID(ovrMobile* ovr, const bool left);

		/**
		 * Determines the skeletons of the a hand
		 * @param ovr The OVR SDK object, must be valid
		 * @param left Indicates the left (`true`) or right hand (`false`)
		 * @return The skeleton of the selected hand, will be empty in case of an error
		 */
		static Joints getHandSkeleton(ovrMobile* ovr, const bool left);

	protected:

		/// The OVR SDK object
		ovrMobile* ovrMobile_;

		/// The ID of the left hand device
		ovrDeviceID leftHandDeviceIndex_ = invalidDeviceIndex;

		/// The ID of the left hand device
		ovrDeviceID rightHandDeviceIndex_ = invalidDeviceIndex;

		/// Indicator for whether the left hand is currently tracked or not
		bool isLeftHandTracked_;

		/// Indicator for whether the right hand is currently tracked or not
		bool isRightHandTracked_;

		/// The pose of root element of the left hand
		HomogenousMatrix4 world_T_leftHand_;

		/// The pose of the root element of the right hand
		HomogenousMatrix4 world_T_rightHand_;

		/// The joints that form the skeleton of the left hand
		Joints leftHandSkeleton_;

		/// The joints that form the skeleton of the right hand
		Joints rightHandSkeleton_;

		/// The rest poses of the left hand (also known as bind poses in skeletal animation)
		HomogenousMatrices4 world_T_leftJointRestPoses_;

		/// The rest poses of the right hand (also known as bind poses in skeletal animation)
		HomogenousMatrices4 world_T_rightJointRestPoses_;

		/// The lock of this object.
		mutable Lock lock_;
};

inline HandPoses::Joint::Joint() :
	parentJoint_T_joint_(false),
	parentIndex_(Index32(-1))
{
	// Nothing else to do.
}

inline HandPoses::Joint::Joint(const HomogenousMatrix4& parentJoint_T_joint, const Index32 parentIndex) :
	parentJoint_T_joint_(parentJoint_T_joint),
	parentIndex_(parentIndex)
{
	// Nothing else to do.
}

inline bool HandPoses::Joint::isValid() const
{
	return parentJoint_T_joint_.isValid();
}

inline const HomogenousMatrix4& HandPoses::Joint::parentJoint_T_joint() const
{
	return parentJoint_T_joint_;
}

inline HomogenousMatrix4& HandPoses::Joint::parentJoint_T_joint()
{
	return parentJoint_T_joint_;
}

inline Index32 HandPoses::Joint::parentIndex() const
{
	return parentIndex_;
}

inline bool HandPoses::Joint::isRoot() const
{
	return parentIndex_ == Index32(-1);
}

inline const HomogenousMatrices4& HandPoses::getHandRestPose(const bool left) const
{
	const ScopedLock scopedLock(lock_);

	return left ? world_T_leftJointRestPoses_ : world_T_rightJointRestPoses_;
}

inline bool HandPoses::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return ovrMobile_ != nullptr;
}

#endif // defined(OCEAN_VRAPI_VERSION) && OCEAN_VRAPI_VERSION >= 1031

} // VrApi

} // Quest

} // Meta

} // Platform

} // Ocean

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_HAND_POSES_H
