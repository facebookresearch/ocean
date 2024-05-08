/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SLAM_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_SLAM_TRACKER_6DOF_H

#include "ocean/devices/slam/SLAM.h"
#include "ocean/devices/slam/SLAMDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

/**
 * This class implements an SLAM feature based tracker.
 * @ingroup devicesslam
 */
class OCEAN_DEVICES_SLAM_EXPORT SLAMTracker6DOF :
	virtual public SLAMDevice,
	virtual public Tracker6DOF,
	virtual public VisualTracker,
	protected Thread
{
	friend class SLAMFactory;

	protected:

		/**
		 * Definition of a pair holding a camera pose and image point.
		 */
		typedef std::pair<HomogenousMatrix4, Vector2> Observation;

		/**
		 * Definition of a vector holding observations.
		 */
		typedef std::vector<Observation> Observations;

		/**
		 * Definition of a vector holding observation groups.
		 */
		typedef std::vector<Observations> ObservationGroups;

	public:

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameSLAMTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeSLAMTracker6DOF();

	private:

		/**
		 * Creates a new SLAM feature based 6DOF tracker object.
		 */
		SLAMTracker6DOF();

		/**
		 * Destructs an SLAM feature based 6DOF tracker object.
		 */
		~SLAMTracker6DOF() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Posts a new camera pose.
		 * @param pose The camera pose to post
		 * @param timestamp The timestamp of the frame to which the pose belongs
		 */
		void postPose(const HomogenousMatrix4& pose, const Timestamp& timestamp);

		/**
		 * Determines feature points in a given camera frame, scatters the feature points into individual bins while (optional) skips bins in which points are located already.
		 * @param frame The frame in which the feature points will be determined
		 * @param alreadyKnownFeaturePoints Optional already known feature points in which's bins no further/new feature points will be determined
		 * @param newFeaturePoints The resulting new feature points
		 * @param binSize The size of a bin in pixel, with range [1, infinity)
		 * @param worker Optional worker to distribute the computation
		 */
		static void determineFeaturePoints(const Frame& frame, const Vectors2& alreadyKnownFeaturePoints, Vectors2& newFeaturePoints, const unsigned int binSize = 50u, Worker* worker = nullptr);

		/**
		 * Tracks feature points from a previous frame to the current frame.
		 * @param previousFramePyramid The frame pyramid of the previous frame
		 * @param currentFramePyramid The frame pyramid of the current frame, with same layout and pixel format as the pyramid of the previous frame
		 * @param previousImagePoints The feature points located in the previous frame
		 * @param currentImagePoints The resulting tracked feature points in the current frame
		 * @param validIndices The indices of the successfully tracked feature points
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 * @tparam tSize The size of the image patches used for tracking, must be odd, with range [1, infinity)
		 */
		template <unsigned int tSize>
		static bool trackPoints(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const Vectors2& previousImagePoints, Vectors2& currentImagePoints, Indices32& validIndices, Worker* worker = nullptr);

		/**
		 * Determines the locations of initial 3D object points from two sets of corresponding image points from individual (stereo) frames.
		 * The origin of the world coordinate system will be located onto the dominant 3D plane where the principal point's viewing ray is intersecting the plane.
		 * @param pinholeCamera The camera profile defining the projection
		 * @param firstImagePoints The first set of image points, at least 5
		 * @param secondImagePoints The second set of image points, one image point for each image point in the first set
		 * @param pose The resulting camera pose of the frame in which the second image points are located (the camera pose of the first frame is expected to be the identity pose)
		 * @param objectPoints The resulting object point locations for the image points
		 * @param validImagePoints The indices of all valid image point correspondences, one index for each resulting object point
		 * @return True, if succeeded
		 */
		static bool determineInitialObjectPoints(const PinholeCamera& pinholeCamera, const Vectors2& firstImagePoints, const Vectors2& secondImagePoints, HomogenousMatrix4& pose, Vectors3& objectPoints, Indices32& validImagePoints);

		/**
		 * Combines three groups of image points to one large set of image points.
		 * @param locatedPreviousImagePoints The image points of already located 3D object points as observed in the previous frame
		 * @param unlocatedObservationGroups The groups of observation of unlocated 3D object points as observed in the previous frames
		 * @param newObservations The set of image points not jet connected with a valid camera pose
		 * @return The large set of image points
		 */
		static Vectors2 combineImagePointGroups(const Vectors2& locatedPreviousImagePoints, const ObservationGroups& unlocatedObservationGroups, const Vectors2& newObservations);

		/**
		 * Extracts the indices of the combined image points belonging to the group of image points for which a located 3D object point exists.
		 * @param numberLocatedPreviousImagePoints The number of image points from the previous frame for which a 3D object point location is known
		 * @param validIndices The indices of valid combined image points
		 * @return The indices of the combined image points
		 */
		static Indices32 extractLocatedImagePointIndices(const size_t numberLocatedPreviousImagePoints, const Indices32& validIndices);

		/**
		 * Extracts the image points of already located 3D object points from the set of combined image points.
		 * @param combinedImagePoints The set of combined image points
		 * @param numberLocatedPreviousImagePoints The number of image points from the previous frame for which a 3D object point location is known
		 * @param validIndices The indices of valid combined image points
		 * @return The image points for which the location of the 3D object point is known
		 */
		static Vectors2 extractLocatedImagePoints(const Vectors2& combinedImagePoints, const size_t numberLocatedPreviousImagePoints, const Indices32& validIndices);

		/**
		 * Extracts the image points for which no known 3D object point location has been determined already from the set of combined image points.
		 * The extracted image points will be inserted into the groups of observations.<br>
		 * Groups of observations without valid (tracked) image point will be removed
		 * @param combinedImagePoints The set of combined image points
		 * @param numberLocatedPreviousImagePoints The number of image points from the previous frame for which a 3D object point location is known
		 * @param validIndices The indices of valid combined image points
		 * @param pose The most recent camera pose, must be valid
		 * @param observationGroups The groups of observations which provide candidates for new 3D object points
		 */
		static void extractUnlocatedImagePoints(const Vectors2& combinedImagePoints, const size_t numberLocatedPreviousImagePoints, const Indices32& validIndices, const HomogenousMatrix4& pose, ObservationGroups& observationGroups);

		/**
		 * Extends the tracking database by determining the locations of 3D object points based on the observations in several individual camera frames.
		 * Observations for which a 3D object point could be determined will be removed from the set of candidates.
		 * @param pinholeCamera The camera profile defining the projection
		 * @param observationGroups The groups of observations which provide candidates for new 3D object points
		 * @param objectPoints The already known object points currently used for tracking, this set will be extended by new 3D object point locations
		 * @param imagePoints The image points currently used for tracking, one image point for each object point, this set will be extended by new image points locations
		 * @param minimalObservations The minimal number of observations an object point must have to so that the 3D object point will be determined
		 */
		static void extendTrackingDatabase(const PinholeCamera& pinholeCamera, ObservationGroups& observationGroups, Vectors3& objectPoints, Vectors2& imagePoints, const unsigned int minimalObservations = 20u);

		/**
		 * Determines the median angle between the mean viewing ray and all individual viewing rays of an object points (and the corresponding image points respectively).
		 * @param pinholeCamera The camera profile defining the projection
		 * @param observations The observations of the object point
		 * @return The median angle, in radian
		 */
		static Scalar medianObservationAngle(const PinholeCamera& pinholeCamera, const Observations& observations);

	private:

		/// Frame timestamp.
		Timestamp frameTimestamp_;

		/// Frame recent type.
		FrameType recentFrameType_;

		/// Most recent sample timestamp.
		Timestamp recentFrameTimestamp_;

		/// The camera calibration object.
		PinholeCamera camera_;

		/// The 2D image points located in the first stereo vision frame which will be used for initialization.
		Vectors2 initializationFirstImagePoints_;

		/// The 2D image points located in the recent stereo vision frame which will be used for initialization.
		Vectors2 initializationRecentImagePoints_;

		/// The number of image points determined in the first stereo vision frame.
		size_t initializationImagePointsDetermined_ = 0;

		/// The timestamp after which the first stereo vision frame may be used.
		Timestamp initializationTimestamp_;

		/// The 3D object points currently used for tracking.
		Vectors3 objectPoints_;

		/// The 2D image points currently used for tracking.
		Vectors2 imagePoints_;

		/// The frame pyramid of the current frame.
		CV::FramePyramid currentFramePyramid_;

		/// The frame pyramid of the previous frame.
		CV::FramePyramid previousFramePyramid_;

		/// The recent camera pose.
		HomogenousMatrix4 previousPose_ = HomogenousMatrix4(false);

		/// The observation groups of feature point candidates.
		ObservationGroups observationGroups_;

		/// The unique object id of this tracker.
		ObjectId uniqueObjectId_ = invalidObjectId();
};

inline std::string SLAMTracker6DOF::deviceNameSLAMTracker6DOF()
{
	return std::string("SLAM Feature Based 6DOF Tracker");
}

inline SLAMTracker6DOF::DeviceType SLAMTracker6DOF::deviceTypeSLAMTracker6DOF()
{
	return SLAMTracker6DOF::DeviceType(SLAMTracker6DOF::deviceTypeTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_SLAM_TRACKER_6DOF_H
