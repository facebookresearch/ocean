/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_BLOB_BLOB_TRACKER_6DOF_H
#define META_OCEAN_TRACKING_BLOB_BLOB_TRACKER_6DOF_H

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/blob/CorrespondenceManager.h"
#include "ocean/tracking/blob/FeatureMap.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/MotionModel.h"
#include "ocean/tracking/VisualTracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * This class implements a 6DOF Blob feature tracker.
 * @ingroup trackingblob
 */
class OCEAN_TRACKING_BLOB_EXPORT BlobTracker6DOF : virtual public VisualTracker
{
	public:

		/**
		 * Creates a new feature tracker object.
		 * @param realtimePerformance True, to initialize the tracker with real-time execution state
		 */
		explicit BlobTracker6DOF(const bool realtimePerformance = true);

		/**
		 * Destructs a feature tracker object.
		 */
		~BlobTracker6DOF() override;

		/**
		 * Sets or changes the feature map for this tracker.
		 * @param featureMap Blob feature map to set
		 * @param autoUpdateMaxPositionOffset State determining whether the maximal position offset bettwen two frame will be adjusted to the feature map size automatically
		 */
		void setFeatureMap(const FeatureMap& featureMap, const bool autoUpdateMaxPositionOffset = true);

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker = nullptr) override;

		/**
		 * Returns whether the tracker is initialized for real-time performance.
		 * @return True, if so
		 */
		inline bool realtimePerformance() const;

		/**
		 * Returns the recent number of feature correspondences used for the recent frame.
		 * @return Most recent number of feature correspondences
		 */
		inline unsigned int featureCorrespondences();

		/**
		 * Returns the minimal number of features that the tracker uses.
		 * @return Minimal number of features
		 */
		inline unsigned int minimalUsedFeatures() const;

		/**
		 * Returns the maximal number of features that the tracker uses.
		 * @return Maximal number of features
		 */
		inline unsigned int maximalUsedFeatures() const;

		/**
		 * Sets the minimal and maximal number of features to use.
		 * @param minimal Number of minimal features, with range [10u, maximal)
		 * @param maximal Number of maximal features, with range (minimal, infinity)
		 * @return True, if succeeded
		 */
		bool setUsedFeatures(const unsigned int minimal, const unsigned int maximal);

		/**
		 * Resets the tracker and releases all internal correspondences or poses from previous tracking iterations.
		 * This function should be invoked if a video stream holds a new keyframe with new visual content.
		 */
		void reset();

		/**
		 * Returns the internal feature correspondence manager.
		 * @return Feature correspondence manager
		 */
		inline const CorrespondenceManager& correspondenceManager() const;

		/**
		 * Executes the tracking step for a collection of frames and corresponding cameras
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr) override;

		/**
		 * Determines the camera pose in relation to a 3D model with given 3D feature points.
		 * @param yFrame The live camera frame for which the pose will be determined, with pixel format FORMAT_Y8, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param model_T_camera The resulting transformation between camera and model
		 * @param modelFeatures The features defining the 3D model
		 * @param minimalCorrespondences The minimal number of feature correspondences so that the resulting pose counts as valid, with range [4, modelFeatures.size()]
		 * @param correspondences Optional resulting number of feature correspondences which have been used to determine the camera pose
		 * @param worker Optional worker object to distribute the computation
		 * @param faultyRate The faulty rate of features, with range [0, 1)
		 * @param usedObjectPoints Optional resulting object points which have been used for relocalization
		 * @param usedImagePoints Optional resulting image points which have been used for relocalization
		 * @param model_T_roughCamera Optional known rough camera pose to use a guided matching; An invalid pose otherwise
		 * @return True, if succeeded
		 */
		static bool determinePose(const Frame& yFrame, const AnyCamera& camera, HomogenousMatrix4& model_T_camera, CV::Detector::Blob::BlobFeatures& modelFeatures, const size_t minimalCorrespondences, size_t* correspondences = nullptr, Worker* worker = nullptr, const Scalar faultyRate = Scalar(0.2), Vectors3* usedObjectPoints = nullptr, Vectors2* usedImagePoints = nullptr, const HomogenousMatrix4& model_T_roughCamera = HomogenousMatrix4(false));

		/**
		 * Deprecated.
		 *
		 * Determines the camera pose in relation to a 3D model with given 3D feature points.
		 * @param yFrame The live camera frame for which the pose will be determined, with pixel format FORMAT_Y8, must be valid
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param model_T_camera The resulting transformation between camera and model
		 * @param modelFeatures The features defining the 3D model
		 * @param minimalCorrespondences The minimal number of feature correspondences so that the resulting pose counts as valid, with range [4, modelFeatures.size()]
		 * @param correspondences Optional resulting number of feature correspondences which have been used to determine the camera pose
		 * @param worker Optional worker object to distribute the computation
		 * @param faultyRate The faulty rate of features, with range [0, 1)
		 * @param usedObjectPoints Optional resulting object points which have been used for relocalization
		 * @param usedImagePoints Optional resulting image points which have been used for relocalization
		 * @param model_T_roughCamera Optional known rough camera pose to use a guided matching; An invalid pose otherwise
		 * @return True, if succeeded
		 */
		static inline bool determinePose(const Frame& yFrame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& model_T_camera, CV::Detector::Blob::BlobFeatures& modelFeatures, const size_t minimalCorrespondences, size_t* correspondences = nullptr, Worker* worker = nullptr, const Scalar faultyRate = Scalar(0.2), Vectors3* usedObjectPoints = nullptr, Vectors2* usedImagePoints = nullptr, const HomogenousMatrix4& model_T_roughCamera = HomogenousMatrix4(false));

	protected:

		/**
		 * Determines the 6DOF tracking for a given frame and given integral image.
		 * @param frame The frame to be used for tracking
		 * @param integralImage Integral image of the tracking frame
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param pose Resulting 6DOF pose
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePose(const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker = nullptr);

		/**
		 * Determines the pose if no previous information is given.
		 * @param features Already detected features to be used for pose determination
		 * @param frame The frame to be used for tracking
		 * @param integralImage Integral image of the tracking frame
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param pose Resulting 6DOF pose
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoseWithoutKnowledge(CV::Detector::Blob::BlobFeatures& features, const Frame& frame, const unsigned int* integralImage, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker);

		/**
		 * Determines the pose if 2D/3D features correspondences from the previous frame are given.
		 * @param features Already detected Blob features in the given frame
		 * @param frame The frame to be used fro tracking
		 * @param integralImage Integral image of the tracking frame
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param boundingBox Bounding box of the detected features
		 * @param pose Resulting 6DOF pose
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoseWithPreviousCorrespondences(CV::Detector::Blob::BlobFeatures& features, const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const Box2& boundingBox, HomogenousMatrix4& pose, Worker* worker);

		/**
		 * Determines the pose if the pose from the prevous frame is given.
		 * @param features Already detected Blob features in the given frame
		 * @param frame The frame to be used fro tracking
		 * @param integralImage Integral image of the tracking frame
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param pose Resulting 6DOF pose
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoseWithPreviousPose(CV::Detector::Blob::BlobFeatures& features, const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker);

		/**
		 * Creates the integral image of the given frame.
		 * @param frame The frame to create an integral image from
		 * @param worker Optional worker object
		 * @return Pointer to the integral image data
		 */
		const uint32_t* createIntegralImage(const Frame& frame, Worker* worker);

		/**
		 * Returns the 2D observation position of a feature point.
		 * @param feature The feature to return the 2D position for
		 * @return Resulting 2D feature position
		 */
		static inline const Vector2& feature2Vector(const CV::Detector::Blob::BlobFeature& feature);

	protected:

		/// Feature map holding all reference features.
		FeatureMap featureMap_;

		/// Feature correspondence manager.
		CorrespondenceManager correspondenceManager_;

		/// Timestamp of the most recent pose.
		Timestamp recentTimestamp_;

		/// Most recent pose.
		HomogenousMatrix4 recentPose_ = HomogenousMatrix4(false);

		/// Random generator object.
		RandomGenerator randomGenerator_;

		/// Number of currently used image features.
		unsigned int usedFeatures_ = 300u;

		/// Minimal number of features to be used.
		unsigned int minimalUsedFeatures_ = 200u;

		/// Maximal number of features to be used.
		unsigned int maximalUsedFeatures_ = 1000u;

		/// Features increase factor.
		const Scalar featuresIncreaseFactor_ = Scalar(2);

		/// Features decrease factor.
		const Scalar featuresDecreaseFactor_ = Scalar(0.7);

		/// Feature strength threshold currently used for feature detection.
		Scalar featureStrengthThreshold_ = Scalar(20);

		/// Minimal feature strength threshold for feature detection.
		Scalar minimalFeatureStrengthThreshold_ = Scalar(2.5);

		/// Maximal feature strength threshold for feature detection.
		Scalar maximalFeatureStrengthThreshold_ = Scalar(500);

		/// Feature strength threshold increase factor.
		const Scalar featureStrengthThresholdIncreaseFactor_ = Scalar(1.5);

		/// Feature strength threshold decrease factor.
		const Scalar featureStrengthThresholdDecreaseFactor_ = Scalar(0.5);

		/// Number of detected features in the current frame.
		unsigned int numberDetectedFeatures_ = 0u;

		/// Number of recently used feature correspondences.
		unsigned int recentFeatureCorrespondences_ = 0u;

		/// Real-time performance state.
		bool realtimePerformance_ = false;

		/// Tracker lock object.
		Lock lock_;

	private:

		/// Intermediate grayscale frame.
		Frame yFrame_;

		/// Integral image for the most recent frame.
		Frame integralImage_;
};

inline bool BlobTracker6DOF::realtimePerformance() const
{
	return realtimePerformance_;
}

inline unsigned int BlobTracker6DOF::featureCorrespondences()
{
	return recentFeatureCorrespondences_;
}

inline unsigned int BlobTracker6DOF::minimalUsedFeatures() const
{
	return minimalUsedFeatures_;
}

inline unsigned int BlobTracker6DOF::maximalUsedFeatures() const
{
	return maximalUsedFeatures_;
}

inline bool BlobTracker6DOF::determinePose(const Frame& yFrame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& model_T_camera, CV::Detector::Blob::BlobFeatures& modelFeatures, const size_t minimalCorrespondences, size_t* correspondences, Worker* worker, const Scalar faultyRate, Vectors3* usedObjectPoints, Vectors2* usedImagePoints, const HomogenousMatrix4& model_T_roughCamera)
{
	return determinePose(yFrame, AnyCameraPinhole(pinholeCamera), model_T_camera, modelFeatures, minimalCorrespondences, correspondences, worker, faultyRate, usedObjectPoints, usedImagePoints, model_T_roughCamera);
}

inline const Vector2& BlobTracker6DOF::feature2Vector(const CV::Detector::Blob::BlobFeature& feature)
{
	return feature.observation();
}

inline const CorrespondenceManager& BlobTracker6DOF::correspondenceManager() const
{
	return correspondenceManager_;
}

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_BLOB_TRACKER_6DOF_H
