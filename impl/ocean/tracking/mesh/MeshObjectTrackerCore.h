// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_MESH_MESH_OBJECT_TRACKER_CORE_H
#define META_OCEAN_TRACKING_MESH_MESH_OBJECT_TRACKER_CORE_H

#include "ocean/tracking/mesh/Mesh.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/VisualTracker.h"

#include "ocean/tracking/mesh/MeshObject.h"
#include "ocean/tracking/uvtexturemapping/MeshUVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

/**
 * This class implements the core of the 6DOF feature tracker for textured mesh objects.
 * This 'core' class is separated from the general MeshObjectTracker class to avoid any virtual functions.<br>
 * Virtual functions may increase the binary size as the compiler/linker may not be able to identify unused virtual functions as 'dead', and thus the linker will not be able to strip such functions.<br>
 * However, if the tracker's object-oriented capability is not needed anyway the usage of this core class is recommended (especially if the binary size matters).
 * @see MeshObjectTracker.
 * @ingroup trackingmesh
 */
class OCEAN_TRACKING_MESH_EXPORT MeshObjectTrackerCore
{
	public:

		/**
		 * ID data type for a tracked object.
		 */
		typedef unsigned int RegisteredObjectId;

		static constexpr RegisteredObjectId kInvalidRegisteredObjectId = static_cast<RegisteredObjectId>(-1);

		/**
		 * Definition of a map holding trackable textured mesh objects.
		 */
		typedef std::map<RegisteredObjectId, MeshObject> RegisteredObjectMap;

		/**
		 * Set of configurable parameters for the tracker.
		 */
		struct Options
		{
			/// The maximal number of objects that can be visible concurrently, with range [1, infinity).
			/// If equal to zero, then no limit will be used.
			unsigned int concurrentTrackedObjectLimit = 1u;

			/// Maximum number of features to extract from a given input frame during recognition.
			/// If equal to zero, then no limit will be used.
			unsigned int maxNumberFeatures = 0u;

			/// The maximal time used for feature-based recognition for each frame in seconds, with range (0, infinity).
			/// If the provided value is <= 0 when the tracker is created, then a default value will be selected.
			double maxTimeAllowedForRecognition = 0.0;

			/// Time in seconds to wait between recognition attempts when at least one object is currently being tracked.
			/// If the value is <= 0, a default value will be chosen.
			double recognitionCadenceWithTrackedObjects = 0.5;

			/// Time in seconds to wait between recognition attempts when no objects are currently being tracked.
			/// If the provided value is < 0, it is ignored and set to zero.
			double recognitionCadenceWithoutTrackedObjects = 0.0;

			/// Algorithm parameters used for determining the pose of a specific object.
			MeshObject::MeshObjectTrackingOptions meshObjectTrackingOptions;

			inline Options() noexcept;
		};

		/**
		 * Creates a new feature tracker object.
		 */
		explicit MeshObjectTrackerCore(const Options& options = Options());

		/**
		 * Destructor.
		 */
		~MeshObjectTrackerCore();

		/**
		 * Adds a new UV-mapped mesh tracking object to the tracker.
		 * @param yTextureFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking texture image, must be valid
		 * @param meshUVTextureMappingRef The mapping from the provided image into 3D coordinates
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the newly tracked object, -1 if the object could not be added
		 * @see UVTextureMapping::MeshUVTextureMapping.
		 */
		RegisteredObjectId add(const Frame& yTextureFrame, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, Worker* worker = nullptr);

		/**
		 * De-registers an object from this tracker.
		 * @param objectId The id of the object to be removed
		 * @return True, if the defined object could be removed; False, if e.g., the object id is invalid
		 * @see clear().
		 */
		bool remove(const RegisteredObjectId objectId);

		/**
		 * Removes all objects from this tracker.
		 * @return True, if all existing objects could be removed
		 * @see remove().
		 */
		bool clear();

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * Beware: The frame type of the input image must not change between successive calls, reset the tracker in case the image resolution changes.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the project, with same dimension as the given frame, must be valid
		 * @param transformations Resulting 6DOF poses combined with the tracking ids
		 * @param quaternion_world_T_currentFrame Optional absolute orientation of the device in the moment the frame was taken, defined in a coordinate system not related with the tracking objects, an invalid object otherwise
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoses(bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, VisualTracker::TransformationSamples& transformations, const Quaternion& quaternion_world_T_currentFrame = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * Beware: The frame type of the input image must not change between successive calls, reset the tracker in case the image resolution changes.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the project, with same dimension as the given frame, must be valid
		 * @param transformations Resulting 6DOF poses combined with the tracking ids
		 * @param quaternion_world_T_currentFrame Optional absolute orientation of the device in the moment the frame was taken, defined in a coordinate system not related with the tracking objects, an invalid object otherwise
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		inline bool determinePoses(const Frame& yFrame, const PinholeCamera& pinholeCamera, VisualTracker::TransformationSamples& transformations, const Quaternion& quaternion_world_T_currentFrame = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Returns the set of tracked 3D object points in the most recently processed frame, for an object with the given id.
		 * @param objectId Id of the registered objects
		 * @return Copy of the object points array, or an empty array if no such object exists (or if the object wasn't successfully tracked in the last frame)
		 */
		inline Vectors3 trackedObjectPoints(RegisteredObjectId objectId) const;

		/**
		 * Returns the set of extracted 2D points corresponding to tracked 3D object points in the most recently processed frame, for an object with the given id.
		 * @param objectId Id of the registered objects
		 * @return Copy of the image points array, or an empty array if no such object exists (or if the object wasn't successfully tracked in the last frame)
		 */
		inline Vectors2 trackedImagePoints(RegisteredObjectId objectId) const;

		/**
		 * Returns the number of registered/added objects.
		 * @return The number of existing objects, with range [0, infinity)
		 * @see add(), setConcurrentTrackedObjectLimit().
		 */
		inline unsigned int numRegisteredObjects() const;

		/**
		 * Returns the maximum number of objects that are allowed to be tracked concurrently within one frame.
		 * @return The number of objects that are allowed to be tracked concurrently, with range [0, numRegisteredObjects()], 0 to track as much as possible, 1 by default
		 * @see setConcurrentTrackedObjectLimit(), numRegisteredObjects().
		 */
		inline unsigned int concurrentTrackedObjectLimit() const;

		/**
		 * Sets the maximum number of objects that are allowed to be tracked concurrently within one frame.
		 * @param concurrentTrackedObjectLimit The number of objects that can be tracked concurrently, with range [0, infinity), 0 to track as much as possible
		 * @see concurrentTrackedObjectLimit(), numRegisteredObjects().
		 */
		inline void setConcurrentTrackedObjectLimit(const unsigned int concurrentTrackedObjectLimit);

		/**
		 * Resets the tracker's states but keeps all registered objects.
		 * This function should be used, e.g., whenever the resolution of the input image changes.<br>
		 * The tracker is simply reset to a state before the first call of determinePoses().
		 */
		void untrackAll();

	protected:

		/**
		 * Determines the 6DOF tracking for a given frame.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the projection, with same dimension as the given frame
		 * @param relativeOrientation Optional relative orientation between the previous frame and the current frame (roughCameraPose = previousCameraPose * relativeOrientation), if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoses(bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& relativeOrientation = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the 6DOF tracking for a given frame which has been downsampled.
		 * @param allowRecognition If false, skip feature extraction and matching for this frame
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object defining the projection, with same dimension as the given frame
		 * @param relativeOrientation Optional relative orientation between the previous frame and the current frame (roughCameraPose = previousCameraPose * relativeOrientation), if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePosesWithDownsampledResolution(bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& relativeOrientation = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Determines the 6DOF poses for registered, but untracked, objects without any a priori information.
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param yFrame The current camera frame with grayscale pixel format (Y8), must be valid
		 * @param currentFramePyramid The frame pyramid of the current frame
		 * @param relativeOrientation Optional relative orientation between the previous frame and the current frame (roughCameraPose = previousCameraPose * relativeOrientation), if known
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePosesForUntrackedObjects(const PinholeCamera& pinholeCamera, const Frame& yFrame, const CV::FramePyramid& currentFramePyramid, const Quaternion& relativeOrientation = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Creates the integral image of the given frame.
		 * @param yFrame The 8 bit grayscale frame (with Y8 pixel format) from which the integral frame will be created, must be valid
		 * @return Pointer to the integral image data
		 */
		const unsigned int* updateIntegralImageBuffer(const Frame& yFrame);

		/**
		 * Counts the number of currently visible objects.
		 * @return The number of objects having a current estimated pose
		 */
		inline unsigned int numCurrentlyTrackedObjects() const;

		/**
		 * Returns the maximum number of objects that can be tracked concurrently within one frame.
		 * This function is an internal helper function providing the number of objects that are allowed to be visible concurrently (if specified), or alternatively the number of objects that are currently registered, whichever is lower.
		 * @return The number of objects that can be tracked concurrently, with range [1, numRegisteredObjects()]
		 */
		inline unsigned int maxNumConcurrentlyTrackedObjects() const;

		/**
		 * Computes the maximum allowed time between recognition attempts, which may depend on whether or not any targets are currently being tracked.
		 * @return Maximum allowed duration in seconds, with range [0, infinity)
		 */
		inline double maximumDurationBetweenRecognitionAttempts() const;

	protected:

		/// Set of options for this tracker.
		Options options_;

		/// Frame pyramid of the current tracking frame.
		CV::FramePyramid currentFramePyramid_;

		/// Frame pyramid of the previous tracking frame.
		CV::FramePyramid previousFramePyramid_;

		/// The map holding all currently registered meshes.
		RegisteredObjectMap registeredObjects_;

		/// Optional absolute orientation for the previous camera frame (as provided from outside this tracker, e.g., via an IMU sensor).
		Quaternion quaternion_world_T_previousFrame_;

		/// A counter providing unique tracked object ids.
		RegisteredObjectId nextRegisteredObjectId_;

		/// Tracker lock object.
		mutable Lock trackerLock_;

		/// Integral image for the most recent frame (used to avoid frame buffer re-allocations).
		Frame integralImage_;

		/// The timestamp of the most recent frame in which a new object has been sought.
		Timestamp timestampOfLastCheckForNewObjects_;

		/// The timestamp of the last frame that was processed.
		Timestamp timestampOfPreviousFrame_;

		/// The id of the last object that we tried to recognize using a feature-matching approach.
		/// In order to maintain FPS, we use a round-robin scheme for recognition.
		RegisteredObjectId lastObjectIdTestedForRecognition_;
};

inline MeshObjectTrackerCore::Options::Options() noexcept
{
	// nothing to do here
}

inline bool MeshObjectTrackerCore::determinePoses(const Frame& yFrame, const PinholeCamera& pinholeCamera, VisualTracker::TransformationSamples& transformations, const Quaternion& quaternion_world_T_currentFrame, Worker* worker)
{
	return determinePoses(/* allowRecognition */ true, yFrame, pinholeCamera, transformations, quaternion_world_T_currentFrame, worker);
}

inline Vectors3 MeshObjectTrackerCore::trackedObjectPoints(RegisteredObjectId objectId) const
{
	const ScopedLock scopedLock(trackerLock_);

	const RegisteredObjectMap::const_iterator iter = registeredObjects_.find(objectId);
	return (iter != registeredObjects_.end()) ? iter->second.objectPoints() : Vectors3();
}

inline Vectors2 MeshObjectTrackerCore::trackedImagePoints(RegisteredObjectId objectId) const
{
	const ScopedLock scopedLock(trackerLock_);

	const RegisteredObjectMap::const_iterator iter = registeredObjects_.find(objectId);
	return (iter != registeredObjects_.end()) ? iter->second.imagePoints() : Vectors2();
}

inline unsigned int MeshObjectTrackerCore::numRegisteredObjects() const
{
	const ScopedLock scopedLock(trackerLock_);

	return (unsigned int)registeredObjects_.size();
}

inline unsigned int MeshObjectTrackerCore::concurrentTrackedObjectLimit() const
{
	const ScopedLock scopedLock(trackerLock_);

	return options_.concurrentTrackedObjectLimit;
}

inline void MeshObjectTrackerCore::setConcurrentTrackedObjectLimit(const unsigned int concurrentTrackedObjectLimit)
{
	const ScopedLock scopedLock(trackerLock_);

	options_.concurrentTrackedObjectLimit = concurrentTrackedObjectLimit;
}

inline unsigned int MeshObjectTrackerCore::numCurrentlyTrackedObjects() const
{
	unsigned int count = 0u;

	for (RegisteredObjectMap::const_iterator i = registeredObjects_.begin(); i != registeredObjects_.end(); ++i)
	{
		if (i->second.pose_world_T_camera().isValid())
		{
			++count;
		}
	}

	return count;
}

inline unsigned int MeshObjectTrackerCore::maxNumConcurrentlyTrackedObjects() const
{
	if (options_.concurrentTrackedObjectLimit == 0u)
	{
		return static_cast<unsigned int>(registeredObjects_.size());
	}
	else
	{
		return min(options_.concurrentTrackedObjectLimit, static_cast<unsigned int>(registeredObjects_.size()));
	}
}

inline double MeshObjectTrackerCore::maximumDurationBetweenRecognitionAttempts() const
{
	return (numCurrentlyTrackedObjects() == 0) ? options_.recognitionCadenceWithoutTrackedObjects : options_.recognitionCadenceWithTrackedObjects;
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_MESH_OBJECT_TRACKER_CORE_H
