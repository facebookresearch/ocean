/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_HAND_POSES_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_HAND_POSES_H

#include "ocean/platform/meta/quest/openxr/OpenXR.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/openxr/Session.h"
#include "ocean/platform/openxr/Utilities.h"

#include <openxr/openxr.h>

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

/**
 * This class implements a wrapper for OpenXR-based hand tracking.
 * This object uses the Android feature 'android.hardware.vr.headtracking'
 * @ingroup platformmetaquestopenxr
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT HandPoses
{
	public:

		/**
		 * Definition of an unordered set holding strings.
		 */
		typedef std::unordered_set<std::string> StringSet;

		/// The number of supported hands.
		static constexpr size_t numberHands_ = 2;

		/// The index of the left hand.
		static constexpr size_t leftHandIndex_ = 0;

		/// The index of the right hand.
		static constexpr size_t rightHandIndex_ = 1;

		/**
		 * This class holds the relevant information for an OpenXR hand mesh.
		 */
		class OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT Mesh
		{
			friend class HandPoses;

			public:

				/**
				 * Definition of a vector holding radii.
				 */
				typedef std::vector<float> Radii;

				/**
				 * Definition of a vector holding signed 16-bit indices.
				 */
				typedef std::vector<int16_t> SignedIndices16;

			public:

				/**
				 * Returns the vertices of the hand mesh.
				 * @return The hand mesh's vertices
				 */
				Vectors3 vertices() const;

				/**
				 * Returns the normals of the hand mesh.
				 * @return The hand mesh's per-vertex normals
				 */
				Vectors3 normals() const;

				/**
				 * Returns the blend indices of the individual hand joints.
				 * @return The hand joint's blend indices
				 */
				inline const Platform::OpenXR::XrVectors4sFB& blendIndices() const;

				/**
				 * Returns the blend weights of the individual hand joints.
				 * @return The hand joint's blend weights
				 */
				inline const Platform::OpenXR::XrVectors4f& blendWeights() const;

				/**
				 * Returns the vertex indices of the hand mesh triangles.
				 * @return The mesh triangle faces, three consecutive indices define one triangle
				 */
				inline const SignedIndices16& indices() const;

				/**
				 * Returns the transformations between bind joints and wrist.
				 * @return The bind joint transformations
				 * @tparam The data type of the scalar to be used either 'float' or 'double'
				 */
				template <typename T = Scalar>
				HomogenousMatricesT4<T> wrist_T_bindJoints() const;

				/**
				 * Returns whether this mesh is valid
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Protected default constructor.
				 */
				Mesh() = default;

				/**
				 * Initializes this mesh with a given tracker for one hand.
				 * @param xrHandTrackerEXT The hand tracker which will be associated with this mesh, must be valid
				 * @param xrGetHandMeshFB The OpenXR get hand mesh function, must be valid
				 * @return True, if succeeded
				 */
				bool initialize(const XrHandTrackerEXT& xrHandTrackerEXT, const PFN_xrGetHandMeshFB& xrGetHandMeshFB);

			protected:

				/// The OpenXR hand tracking mesh.
				XrHandTrackingMeshFB xrHandTrackingMeshFB_ = {XR_TYPE_HAND_TRACKING_MESH_FB};

				/// The joint bind poses.
				Platform::OpenXR::XrPosesf jointBindPoses_;

				/// The radii of the individual hand joints.
				Radii jointRadii_;

				/// The parents of the individual joints.
				Platform::OpenXR::XrHandJointsEXT jointParents_;

				/// The positions of the individual vertices.
				Platform::OpenXR::XrVectors3f vertexPositions_;

				/// The normals of the individual vertices.
				Platform::OpenXR::XrVectors3f vertexNormals_;

				/// The texture coordinates of the individual vertices.
				Platform::OpenXR::XrVectors2f vertexUVs_;

				/// The blend indices of the individual vertices.
				Platform::OpenXR::XrVectors4sFB vertexBlendIndices_;

				/// The blend weights of the individual vertices.
				Platform::OpenXR::XrVectors4f vertexBlendWeights_;

				/// The vertex indices of the hand mesh triangles, must be a multiple of 3.
				SignedIndices16 indices_;

				/// True, if the mesh is valid.
				bool isValid_ = false;
		};

		/**
		 * This class holds the relevant information for an OpenXR hand pose.
		 */
		class OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT Pose
		{
			friend class HandPoses;

			public:

				/// Default flags for valid hand locations.
				static constexpr XrSpaceLocationFlags defaultLocationFlags_ = XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;

			public:

				/**
				 * Returns the 3D positions of the hand joints in relation to the hand pose's base space.
				 * @param baseSpaceJointPositions The resulting joint locations defined in the base space, either XR_HAND_JOINT_COUNT_EXT or zero
				 * @param xrSpaceLocationFlags The location flags all hand joints must satisfy
				 * @return True, if succeeded; False, if e.g., some hand joints don't satisfy the specified location flags
				 * @tparam T The data type of the scalar to be used either 'float' or 'double'
				 */
				template <typename T = Scalar>
				bool jointPositions(VectorsT3<T>& baseSpaceJointPositions, const XrSpaceLocationFlags xrSpaceLocationFlags = defaultLocationFlags_) const;

				/**
				 * Returns the 6DOF transformations of the hand joints in relation to the hand pose's base space.
				 * The resulting transformations do not incorporate the mesh's joint bind poses.
				 * @param baseSpace_T_joints The resulting joint transformations, either XR_HAND_JOINT_COUNT_EXT or zero
				 * @param xrSpaceLocationFlags The location flags all hand joints must satisfy
				 * @return True, if succeeded; False, if e.g., some hand joints don't satisfy the specified location flags
				 * @tparam T The data type of the scalar to be used either 'float' or 'double'
				 */
				template <typename T = Scalar>
				bool jointTransformations(HomogenousMatricesT4<T>& baseSpace_T_joints, const XrSpaceLocationFlags xrSpaceLocationFlags = defaultLocationFlags_) const;

				/**
				 * Returns the 6DOF transformations of the hand joints in relation to the hand pose's base space.
				 * The resulting transformations incorporate the mesh's joint bind poses and thus can be used to e.g., render a hand mesh.
				 * @param mesh The hand mesh providing the joint bind poses, must be valid
				 * @param baseSpace_T_joints The resulting joint transformations, either XR_HAND_JOINT_COUNT_EXT or zero
				 * @param xrSpaceLocationFlags The location flags all hand joints must satisfy
				 * @return True, if succeeded; False, if e.g., some hand joints don't satisfy the specified location flags
				 * @tparam T The data type of the scalar to be used either 'float' or 'double'
				 */
				template <typename T = Scalar>
				bool jointTransformations(const Mesh& mesh, HomogenousMatricesT4<T>& baseSpace_T_joints, const XrSpaceLocationFlags xrSpaceLocationFlags = defaultLocationFlags_) const;

				/**
				 * Returns the vertices of the hand mesh in relation to the hand pose's base space.
				 * The resulting vertices incorporate the mesh's joint bind poses and thus can be used to e.g., render a hand mesh without any further transformations.
				 * @param mesh The hand mesh providing the joint bind poses, must be valid
				 * @param baseSpaceMeshVertices The resulting hand mesh vertices defined in the base space
				 * @param xrSpaceLocationFlags The location flags all hand joints must satisfy
				 * @return True, if succeeded; False, if e.g., some hand joints don't satisfy the specified location flags
				 * @tparam T The data type of the scalar to be used either 'float' or 'double'
				 */
				template <typename T = Scalar>
				bool meshVertices(const Mesh& mesh, VectorsT3<T>& baseSpaceMeshVertices, const XrSpaceLocationFlags xrSpaceLocationFlags = defaultLocationFlags_) const;

				/**
				 * Returns the OpenXR space in which relation the hand pose is determined.
				 * @return The OpenXR space
				 */
				inline XrSpace xrBaseSpace() const;

				/**
				 * Returns the XR_HAND_JOINT_COUNT_EXT hand joint locations of this pose.
				 * @return The pose' hand joint locations
				 */
				inline const XrHandJointLocationEXT* xrHandJointLocationsEXT() const;

				/**
				 * Returns the XR_HAND_JOINT_COUNT_EXT hand joint velocities of this pose.
				 * @return The pose' hand joint velocities
				 */
				inline const XrHandJointVelocityEXT* xrHandJointVelocitiesEXT() const;

				/**
				 * Returns whether this hand pose is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Protected default constructor.
				 */
				Pose() = default;

			protected:

				/// The OpenXR space in which relation the hand pose is determined.
				XrSpace xrBaseSpace_ = XR_NULL_HANDLE;

				/// The hand joint locations of this hand pose.
				XrHandJointLocationEXT xrHandJointLocationsEXT_[XR_HAND_JOINT_COUNT_EXT];

				/// The hand joint velocities of this hand pose.
				XrHandJointVelocityEXT xrHandJointVelocitiesEXT_[XR_HAND_JOINT_COUNT_EXT];
		};

	public:

		/**
		 * Default constructor
		 */
		HandPoses() = default;

		/**
		 * Move constructor.
		 * @param handPoses Object to be moved
		 */
		HandPoses(HandPoses&& handPoses);

		/**
		 * Initializes the hand tracking object.
		 * @param session The OpenxR session to be used, must be valid
		 * @return True, if succeeded
		 */
		inline bool initialize(const Platform::OpenXR::Session& session);

		/**
		 * Initializes the hand tracking object.
		 * @param xrInstance The instance for which the hand poses will be created, must be valid
		 * @param xrSession The session for which the hand poses will be created, must be valid
		 * @param xrSystemId The runtime's identifier, must be valid
		 * @return True, if succeeded
		 */
		bool initialize(const XrInstance& xrInstance, const XrSession& xrSession, const XrSystemId& xrSystemId);

		/**
		 * Explicitly releases the session and all associated resources.
		 * @see initialize().
		 */
		void release();

		/**
		 * Updates the pose of the left and right hands.
		 * @param xrBaseSpace The space in which the hands will be defined, must be valid
		 * @param predictedDisplayTime The predicted display time of the next frame
		 * @return True, if succeeded
		 */
		bool update(const XrSpace& xrBaseSpace, const XrTime& predictedDisplayTime);

		/**
		 * Returns the mesh of one hand.
		 * Lock this object's lock before calling this function.
		 * @param handIndex The index of the hand for which the mesh will be returned, with range [0, numberHands_ - 1]
		 * @return The requested mesh, may be invalid
		 * @see lock().
		 */
		inline const Mesh& mesh(const size_t handIndex) const;

		/**
		 * Returns the current pose of one hand.
		 * Lock this object's lock before calling this function.<br>
		 * Call update() to ensure that the latest hand pose is provided.
		 * @param handIndex The index of the hand for which the pose will be returned, with range [0, numberHands_ - 1]
		 * @return The requested mesh, may be invalid
		 * @see lock(), update().
		 */
		inline const Pose& pose(const size_t handIndex) const;

		/**
		 * Returns the 3D positions of the joints of one hand in relation to the hand pose's base space.
		 * The resulting positions are valid until the next 'update()' call.
		 * @param handIndex The index of the hand for which the pose will be returned, with range [0, numberHands_ - 1]
		 * @return The resulting joint locations define in the baseSpace, XR_HAND_JOINT_COUNT_EXT joint positions if succeeded
		 */
		const Vectors3& jointPositions(const size_t handIndex) const;

		/**
		 * Returns the lock of the hand poses.
		 * @return The hand poses' lock.
		 */
		inline Lock& lock() const;

		/**
		 * Returns whether this object holds initialized and valid OpenXR hand trackers.
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
		 * Returns the names of the necessary OpenXR extensions necessary for hand tracking.
		 * @return The necessary OpenXR extensions
		 */
		static const StringSet& necessaryOpenXRExtensionNames();

		/**
		 * Updates the current hand pose of one hand.
		 * @param xrHandTrackersEXT The hand tracker from which the hand pose will be extracted, must be valid
		 * @param xrLocateHandJointsEXT The OpenXR locate hand joint function, must be valid
		 * @param xrBaseSpace The OpenXR space in which relation the hand pose is determined, must be valid
		 * @param xrTime The OpenXR time for which the hand pose is requested, e.g. the predicted display time
		 * @param pose The hand pose receiving the updated information
		 * @return True, if succeeded
		 */
		static bool updateHandPose(const XrHandTrackerEXT& xrHandTrackersEXT, const PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT, const XrSpace& xrBaseSpace, const XrTime& xrTime, Pose& pose);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		HandPoses(const HandPoses&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		HandPoses& operator=(const HandPoses&) = delete;

	protected:

		/// The OpenXR instance for which hand tracking is created.
		XrInstance xrInstance_ = XR_NULL_HANDLE;

		/// The OpenXR function pointer to create a new hand tracker.
		PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT_ = nullptr;

		/// The OpenXR function pointer to destroy a hand tracker.
		PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT_ = nullptr;

		/// The OpenXR function to determine the hand joints.
		PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT_ = nullptr;

		/// The OpenXR function to determine the hand mesh.
		PFN_xrGetHandMeshFB xrGetHandMeshFB_ = nullptr;

		/// The OpenXR handles to both hand trackers with order [left, right].
		XrHandTrackerEXT xrHandTrackersEXT_[numberHands_] = {XR_NULL_HANDLE, XR_NULL_HANDLE};

		/// The meshes for both hands.
		Mesh meshes_[numberHands_];

		/// The poses for both hands.
		Pose poses_[numberHands_];

		/// The cached positions of the joints of both hands.
		mutable Vectors3 baseSpaceJointPositions_[numberHands_];

		/// An empty vector with 3D vectors.
		const Vectors3 invalidResultVectors_;

		/// The lock of this object.
		mutable Lock lock_;
};

inline const Platform::OpenXR::XrVectors4sFB& HandPoses::Mesh::blendIndices() const
{
	return vertexBlendIndices_;
}

inline const Platform::OpenXR::XrVectors4f& HandPoses::Mesh::blendWeights() const
{
	return vertexBlendWeights_;
}

inline const HandPoses::Mesh::SignedIndices16& HandPoses::Mesh::indices() const
{
	return indices_;
}

inline bool HandPoses::Mesh::isValid() const
{
	return isValid_;
}

inline XrSpace HandPoses::Pose::xrBaseSpace() const
{
	return xrBaseSpace_;
}

inline const XrHandJointLocationEXT* HandPoses::Pose::xrHandJointLocationsEXT() const
{
	return xrHandJointLocationsEXT_;
}

inline const XrHandJointVelocityEXT* HandPoses::Pose::xrHandJointVelocitiesEXT() const
{
	return xrHandJointVelocitiesEXT_;
}

inline bool HandPoses::Pose::isValid() const
{
	return xrBaseSpace_ != XR_NULL_HANDLE;
}

inline bool HandPoses::initialize(const Platform::OpenXR::Session& session)
{
	ocean_assert(session.isValid());

	return initialize(session.xrInstance(), session, session.xrSystemId());
}

inline const HandPoses::Mesh& HandPoses::mesh(const size_t handIndex) const
{
	ocean_assert(handIndex < numberHands_);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return meshes_[handIndex];
}

inline const HandPoses::Pose& HandPoses::pose(const size_t handIndex) const
{
	ocean_assert(handIndex < numberHands_);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return poses_[handIndex];
}

inline Lock& HandPoses::lock() const
{
	return lock_;
}

inline bool HandPoses::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return xrInstance_ != XR_NULL_HANDLE;
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_HAND_POSES_H
