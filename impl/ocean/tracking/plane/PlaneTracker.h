// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_PLANE_PLANE_TRACKER_H
#define META_OCEAN_TRACKING_PLANE_PLANE_TRACKER_H

#include "ocean/tracking/plane/Plane.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector2.h"

#include "ocean/tracking/point/HomographyTracker.h"

#include <vplib/support/LiveVerticalPlaneFinderWithIMU.h>

#include <memory>

namespace Ocean
{

namespace Tracking
{

namespace Plane
{

/**
 * This class implements a visual tracker based on planes in 3D space.
 * The normals of the planes are determined by using vanishing points while the position is tracked with a homography/perspective pose tracking approach.<br>
 * The coordinate system of the camera is defined such that the observer/camera is looking into the negative z-space, the origin is in the center of the image.<br>
 * The x-axis points to the right of the camera, and the y-axis is identical to the up vector of the camera:<br>
 * <pre>
 * ................................. (camera image top right)
 * .                               .
 * .            ^                  .
 * .          Y |                  .
 * .            |                  .
 * .            |                  .
 * .            O --------->       .
 * .           /        X          .
 * .          / Z                  .
 * .         v                     .
 * .                               .
 * ................................. (camera image bottom right)
 * (camera image bottom left)
 * </pre>
 * In case, the tracker is used in combintation with known camera orientations (e.g,. base on IMU information),<br>
 * the given orientation must transform points defined in the coordinate system of the camera into points defined in the world coordinate system (pointWorld = wTc * pointCamera).<br>
 * Further, the negative y-axis of the world coordinate system must points towards the ground (the negative y-axis must be identical with the gravity vector).
 * @ingroup trackingplane
 */
class OCEAN_TRACKING_PLANE_EXPORT PlaneTracker : protected Thread
{
	public:

		/**
		 * Definition of an id of a plane.
		 */
		typedef unsigned int PlaneId;

		/**
		 * Definition of a vector holding plane ids.
		 */
		typedef std::vector<PlaneId> PlaneIds;

		/**
		 * Definition of individual plane properties.
		 */
		enum PlaneProperty : unsigned int
		{
			/// An invalid plane.
			PP_INVALID = 0u,
			/// A horizontal floor/ground plane.
			PP_HORIZONTAL_FLOOR = (1u << 0u),
			/// A horizontal ceiling plane.
			PP_HORIZONTAL_CEILING = (1u << 1u),
			/// A vertical plane.
			PP_VERTICAL = (1u << 2u),
			/// Any horizontal plane (either floor or ceiling).
			PP_HORIZONTAL = (unsigned int)(PP_HORIZONTAL_FLOOR | PP_HORIZONTAL_CEILING),
			/// Either a horizontal or a vertical plane.
			PP_HORIZONTAL_OR_VERTICAL = (unsigned int)(PP_HORIZONTAL | PP_VERTICAL)
		};

		/**
		 * Definition of a vector holding plane properties.
		 */
		typedef std::vector<PlaneProperty> PlaneProperties;

		/**
		 * This class stores the information necessary to represent a plane in 3D space.
		 */
		class Plane
		{
			friend class PlaneTracker;

			public:

				/**
				 * Creates a new invalid plane data object.
				 */
				inline Plane();

				/**
				 * Returns the 6DOF pose of the camera, transforming points defined in the camera coordinate system to points defined in the world coordinate system (wTc).
				 * @return The camera's 6DOF pose
				 */
				inline const HomogenousMatrix4& pose() const;

				/**
				 * Returns the transformation transforming points defined in the plane coordinate system to points defined in the world coordinate system (wTp).
				 * @return The plane's transformation.
				 */
				inline const HomogenousMatrix4& object() const;

				/**
				 * Returns the property of the plane.
				 * @return The plane's property, either PP_HORIZONTAL or PP_VERTICAL, PP_INVALID if the plane is invalid
				 */
				inline PlaneProperty planeProperty() const;

				/**
				 * Returns the timestamp of the pose when determined based on visual information (not predicted based on IMU information).
				 * @return The pose's timestamp
				 */
				inline const Timestamp& accuratePoseTimestamp() const;

			protected:

				/**
				 * Creates a new plane data object.
				 * @param pose The 6DOF camera pose for which the plane has been determined, transforming points defined in the camera coordinate system to points defined in the world coordinate system (wTc), must be valid
				 * @param object The transformation transforming points defined in the plane coordinate system to points defined in the world coordinate system (wTp), must be valid
				 * @param planeProperty The property of the plane, either PP_HORIZONTAL or PP_VERTICAL
				 * @param cameraOrientation The orientation of the camera e.g., given by an IMU sensor, (pointWorld = wTc * pointCamera), if known
				 * @param timestamp The timestamp matching with the given pose
				 */
				inline Plane(const HomogenousMatrix4& pose, const HomogenousMatrix4& object, const PlaneProperty planeProperty, const Quaternion& cameraOrientation = Quaternion(), const Timestamp& timestamp = Timestamp(false));

			protected:

				/// 6DOF pose of the pinholeCamera, transforming points defined in the camera coordinate system to points defined in the world coordinate system (wTc), may be a predicted pose based on IMU information.
				HomogenousMatrix4 pose_;

				/// Transformation transforming points defined in the plane coordinate system to points defined in the world coordinate system (wTp).
				HomogenousMatrix4 object_;

				/// The property of the plane, either PP_HORIZONTAL or PP_VERTICAL, PP_INVALID if the plane is invalid.
				PlaneProperty planeProperty_;

				/// 6DOF pose of the pinholeCamera, transforming points defined in the camera coordinate system to points defined in the world coordinate system (wTc), must be based on a valid visual tracking iteration.
				HomogenousMatrix4 lastAccuratePose_;

				/// The orientation of the camera when the last accurate pose was determined.
				Quaternion lastAccuratePoseCameraOrientation_;

				/// The timestamp of the last accurate pose.
				Timestamp lastAccuratePoseTimestamp_;
		};

		/**
		 * Definition of a map mapping plane ids to planes.
		 */
		typedef std::map<PlaneId, Plane> PlaneMap;

	protected:

		/**
		 * Definition of a map mapping plane ids to homography trackers.
		 */
		typedef std::map<PlaneId, std::shared_ptr<Point::HomographyTracker>> HomographyTrackerMap;

		/**
		 * This class contains the necessary data the vanishing point detector needs to process a frame.
		 * The implementation itself
		 */
		class PlaneFinderAsynchronousData
		{
			public:

				/**
				 * Creates a new data object not holding any data.
				 */
				inline PlaneFinderAsynchronousData() noexcept;

				/**
				 * Move constructor.
				 * @param data The data object to be moved
				 */
				inline PlaneFinderAsynchronousData(PlaneFinderAsynchronousData&& data) noexcept;

				/**
				 * Creates a new data object with the information necessary to determine a plane.
				 * @param yFrame The frame in which the plane will be determined, will be moved, must be valid
				 * @param pinholeCamera The pinhole camera profile defining the project, must be valid
				 * @param cameraOrientationIF Optional known orientation of the camera, inverted and flipped
				 */
				inline PlaneFinderAsynchronousData(Frame&& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& cameraOrientationIF) noexcept;

				/**
				 * Returns the frame of this object.
				 * @return The object's frame, may be invalid
				 */
				inline const Frame& yFrame() const;

				/**
				 * Returns the camera of this object.
				 * @return The object's camera, may be invalid
				 */
				inline const PinholeCamera& camera() const;

				/**
				 * Returns the camera orientation of this object.
				 * @return The object's orientation, may be invalid
				 */
				inline const Quaternion& cameraOrientationIF() const;

				/**
				 * Move operator.
				 * @param data The data object to be moved
				 * @return Reference to this object
				 */
				inline PlaneFinderAsynchronousData& operator=(PlaneFinderAsynchronousData&& data) noexcept;

			protected:

				/// The image in which the plane will be determined.
				Frame yFrame_;

				/// The pinhole camera profile matching with the frame.
				PinholeCamera pinholeCamera_;

				/// Optional known orientation of the pinholeCamera, inverted and flipped.
				Quaternion cameraOrientationIF_;
		};

	public:

		/**
		 * Creates a new plane tracker.
		 */
		PlaneTracker();

		/**
		 * Destructs the tracker.
		 */
		~PlaneTracker() override;

		/**
		 * Add new planes to this tracker by specifying the locations of the new planes within the current camera image.
		 * @param pinholeCamera The pinhole camera profile for the given image, must be valid
		 * @param yFrame The frame in which the new planes will be added, must have pixel format FORMAT_Y8, with valid timestamp, must be valid
		 * @param planeLocations The individual locations within the given camera frame, at least one
		 * @param cameraOrientation Optional known orientation of the camera w.r.t. the world coordinate system (pointWorld = wTc * pointCamera), with negative y-axis pointing towards the ground, an invalid orientation otherwise
		 * @param planeProperties The properties of each individual plane to be added, an empty vector to allow planes with arbitrary properties, otherwise one property for each given location
		 * @param worker Optional worker object to distrubute the computation
		 * @return The individual ids for the new planes, one for each specified location, an invalid id if a plane could not be determined at the specified location
		 */
		PlaneIds addPlanes(const PinholeCamera& pinholeCamera, const Frame& yFrame, const Vectors2& planeLocations, const Quaternion& cameraOrientation = Quaternion(false), const PlaneProperties& planeProperties = PlaneProperties(), Worker* worker = nullptr);

		/**
		 * Add new planes to this tracker by specifying the locations of the new planes within the current camera image.
		 * @param pinholeCamera The pinhole camera profile for the given image, must be valid
		 * @param timestamp timestamp in seconds
		 * @param planeLocations The individual locations within the given camera frame, at least one
		 * @param cameraOrientation Optional known orientation of the camera w.r.t. the world coordinate system (pointWorld = wTc * pointCamera), with negative y-axis pointing towards the ground, an invalid orientation otherwise
		 * @param planeProperties The properties of each individual plane to be added, an empty vector to allow planes with arbitrary properties, otherwise one property for each given location
		 * @param worker Optional worker object to distrubute the computation
		 * @return The individual ids for the new planes, one for each specified location, an invalid id if a plane could not be determined at the specified location
		 */
		PlaneIds addPlanes(const PinholeCamera& pinholeCamera, const Timestamp& timestamp, const Vectors2& planeLocations, const Quaternion& cameraOrientation, const PlaneProperties& planeProperties, Worker* worker = nullptr);

		/**
		 * Moves the origin of an existing plane along the plane.
		 * @param planeId The id of the plane for which the origin will be moved, must be valid
		 * @param pinholeCamera The pinhole camera profile defining the projection
		 * @param newPlaneLocation The location of the new plane within the camera frame, so that the origin will be placed at the intersecting of the viewing ray and the original plane
		 * @return true, if succeeded
		 */
		bool movePlane(const PlaneId planeId, const PinholeCamera& pinholeCamera, const Vector2& newPlaneLocation);

		/**
		 * Removes a plane from this tracker.
		 * @param planeId The id of the plane to be removed, must be valid
		 * @return True, if the plane could be removed
		 */
		bool removePlane(const PlaneId planeId);

		/**
		 * Removes all planes from this tracker.
		 */
		void removePlanes();

		/**
		 * Tracks all planes from the previous frame to a given new frame.
		 * @param pinholeCamera The pinhole camera profile for the given image, must be valid
		 * @param yFrame The new frame in which the plane will be tracked, must have pixel format FORMAT_Y8, with valid timestamp, must be valid
		 * @param cameraOrientation Optional known orientation of the camera w.r.t. the world coordinate system (pointWorld = wTc * pointCamera), with negative y-axis pointing towards the ground, an invalid orientation otherwise
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool trackPlanes(const PinholeCamera& pinholeCamera, const Frame& yFrame, const Quaternion& cameraOrientation = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Returns one specific plane of this tracker.
		 * @param planeId The id of the plane to return
		 * @return The requested plane
		 */
		inline Plane plane(const PlaneId planeId) const;

		/**
		 * Returns all planes currently tracked in this tracker.
		 * @return The tracker's planes
		 */
		inline PlaneMap planes() const;

		/**
		 * Returns an id to denote invalid planes.
		 * @return The invalid plane id
		 */
		constexpr PlaneId invalidPlaneId();

	protected:

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Creates a new instance of the vertical plane finder (using IMU).
		 * @param maxTimeToLastProcessedSeconds Maximal interval between two consecutive plane detection (frame processing) calls, in seconds, with range (0, infinity)
		 * @return The new vertical plane finder object
		 */
		static std::unique_ptr<facebook::coreai::vplib::LiveVerticalPlaneFinderWithIMU> createVerticalPlaneFinder(const double maxTimeToLastProcessedSeconds);

	protected:

		/// The actual implementation of the plane detector.
		std::unique_ptr<facebook::coreai::vplib::LiveVerticalPlaneFinderWithIMU> liveVerticalPlaneFinderWithIMU_;

	 	/// The asynchronous data used by the plane finder.
		std::unique_ptr<PlaneFinderAsynchronousData> planeFinderAsynchronousData_;

		/// The desired interval between two consecutive plane detection (frame processing) calls, in seconds, with range (0, infinity)
		double planeDetectionInterval_;

		/// The lock for the asynchronous data used by the plane finder.
		Lock asynchronousDataLock_;

		/// The map mapping plane ids to information representing the planes.
		PlaneMap planeMap_;

		/// The map mapping plane ids to homography trackers.
		HomographyTrackerMap homographyTrackerMap_;

		/// The timestamp of the previous VP processFrame call.
		Timestamp previousProcessFrameTimestamp_;

		/// The maximal interval between an accurate pose (determined based on a visual tracking) and a predicted pose (based on pure IMU-based information only), in seconds.
		double maximalIntervalForPredictedPose_;

		/// The camera orientation matching with the previous camera frame, if known ((pointWorld = wTc * pointCamera).
		Quaternion previousCameraOrientation_;

		/// The counter managing the ids of planes.
		PlaneId planeIdCounter_;

		/// The lock for this tracker.
		mutable Lock trackerLock_;

		/// The maximal angle between gravity vector and viewing vector so that the gravity vector is forced to be used instead of any vp normal, in radian, with range [0, maximalAngleForUsingGravityVector_)
		Scalar maximalAngleForForcingGravityVector_;

		/// The maximal angle between gravity vector and viewing vector so that the gravity vector is used as backup if the vp normal is invalid, in radian, with range (maximalAngleForForcingGravityVector_, PI_2)
		Scalar maximalAngleForUsingGravityVectorAsBackup_;

		/// The maximal angle between gravity vector and viewing vector so that the gravity vector can be used, in radian, with range (maximalAngleForUsingGravityVectorAsBackup_, PI_2)
		Scalar maximalAngleForUsingGravityVector_;
};

inline PlaneTracker::Plane::Plane() :
	pose_(false),
	object_(false),
	planeProperty_(PP_INVALID)
{
	// nothing to do here
}

inline PlaneTracker::Plane::Plane(const HomogenousMatrix4& pose, const HomogenousMatrix4& object, const PlaneProperty planeProperty, const Quaternion& cameraOrientation, const Timestamp& timestamp) :
	pose_(pose),
	object_(object),
	planeProperty_(planeProperty),
	lastAccuratePose_(pose),
	lastAccuratePoseCameraOrientation_(cameraOrientation),
	lastAccuratePoseTimestamp_(timestamp)
{
	// nothing to do here
}

inline const HomogenousMatrix4& PlaneTracker::Plane::pose() const
{
	return pose_;
}

inline const HomogenousMatrix4& PlaneTracker::Plane::object() const
{
	return object_;
}

inline PlaneTracker::PlaneProperty PlaneTracker::Plane::planeProperty() const
{
	return planeProperty_;
}

inline const Timestamp& PlaneTracker::Plane::accuratePoseTimestamp() const
{
	return lastAccuratePoseTimestamp_;
}

inline PlaneTracker::PlaneFinderAsynchronousData::PlaneFinderAsynchronousData() noexcept :
	yFrame_(),
	pinholeCamera_(),
	cameraOrientationIF_(false)
{
	// nothing to do here
}

inline PlaneTracker::PlaneFinderAsynchronousData::PlaneFinderAsynchronousData(PlaneFinderAsynchronousData&& data) noexcept :
	yFrame_(std::move(data.yFrame_)),
	pinholeCamera_(data.pinholeCamera_),
	cameraOrientationIF_(data.cameraOrientationIF_)
{
	// nothing to do here
}

inline PlaneTracker::PlaneFinderAsynchronousData::PlaneFinderAsynchronousData(Frame&& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& cameraOrientationIF) noexcept :
	yFrame_(std::move(yFrame)),
	pinholeCamera_(pinholeCamera),
	cameraOrientationIF_(cameraOrientationIF)
{
	// nothing to do here
}

inline const Frame& PlaneTracker::PlaneFinderAsynchronousData::yFrame() const
{
	return yFrame_;
}

inline const PinholeCamera& PlaneTracker::PlaneFinderAsynchronousData::camera() const
{
	return pinholeCamera_;
}

inline const Quaternion& PlaneTracker::PlaneFinderAsynchronousData::cameraOrientationIF() const
{
	return cameraOrientationIF_;
}

inline PlaneTracker::PlaneFinderAsynchronousData& PlaneTracker::PlaneFinderAsynchronousData::operator=(PlaneFinderAsynchronousData&& data) noexcept
{
	if (this != &data)
	{
		yFrame_ = std::move(data.yFrame_);

		pinholeCamera_ = data.pinholeCamera_;
		data.pinholeCamera_ = PinholeCamera();

		cameraOrientationIF_ = data.cameraOrientationIF_;
		data.cameraOrientationIF_ = Quaternion(false);
	}

	return *this;
}

inline PlaneTracker::Plane PlaneTracker::plane(const PlaneId planeId) const
{
	const ScopedLock scopedLock(trackerLock_);

	const PlaneMap::const_iterator i = planeMap_.find(planeId);
	ocean_assert(i != planeMap_.cend());

	if (i == planeMap_.cend())
	{
		return Plane();
	}

	return i->second;
}

inline PlaneTracker::PlaneMap PlaneTracker::planes() const
{
	const ScopedLock scopedLock(trackerLock_);

	return PlaneMap(planeMap_);
}

constexpr PlaneTracker::PlaneId PlaneTracker::invalidPlaneId()
{
	return PlaneId(-1);
}

}

}

}

#endif // META_OCEAN_TRACKING_PLANE_PLANE_TRACKER_H
