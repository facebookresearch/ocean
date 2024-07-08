/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_ORB_FEATURE_TRACKER_6DOF_H
#define META_OCEAN_TRACKING_ORB_FEATURE_TRACKER_6DOF_H

#include "ocean/tracking/orb/ORB.h"
#include "ocean/tracking/orb/FeatureMap.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/VisualTracker.h"

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

/**
 * This class implements a 6DOF ORB feature tracker.
 * @ingroup trackingorb
 */
class OCEAN_TRACKING_ORB_EXPORT FeatureTracker6DOF : virtual public VisualTracker
{
	public:

		/**
		 * Creates a new feature tracker object.
		 */
		explicit FeatureTracker6DOF() = default;

		/**
		 * Destructs a feature tracker object.
		 */
		~FeatureTracker6DOF() override;

		/**
		 * Sets or changes the feature map for this tracker.
		 * @param featureMap ORB feature map to set, must be created with the same feature detector as the tracker
		 * @param autoUpdateMaxPositionOffset State determining whether the maximal position offset between two frames will be adjusted to the feature map size automatically
		 * @return True, if succeeded
		 */
		bool setFeatureMap(const FeatureMap& featureMap, const bool autoUpdateMaxPositionOffset = true);

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr) override;

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
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param pose Resulting 6DOF pose
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		bool determinePoseWithoutKnowledge(CV::Detector::ORBFeatures& features, const Frame& frame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& pose, Worker* worker);

		/**
		 * Determines the pose if the pose from the previous frame is given.
		 * @param features Already detected ORB features in the given frame
		 * @param frame The frame to be used fro tracking
		 * @param pinholeCamera The pinhole camera object associated with the frame
		 * @param pose Resulting 6DOF pose
		 * @return True, if succeeded
		 */
		bool determinePoseWithPreviousPose(CV::Detector::ORBFeatures& features, const Frame& frame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& pose);

		/**
		 * Creates the lined integral image of the given frame.
		 * @param frame The frame for which the lined integral image will be created, must be valid
		 * @param worker Optional worker object
		 * @return Pointer to the integral image data
		 */
		const uint32_t* createLinedIntegralImage(const Frame& frame, Worker* worker);

	protected:

		/// Feature map holding all reference features.
		FeatureMap featureMap_;

		/// Timestamp of the most recent pose.
		Timestamp recentTimestamp_;

		/// Most recent pose (object_T_camera).
		HomogenousMatrix4 recentPose_ = HomogenousMatrix4(false);

		/// Random generator object.
		RandomGenerator randomGenerator_;

		/// Feature strength threshold currently used for feature detection.
		unsigned int featureStrengthThreshold_ = 35u;

		// The percentage (in relation to the number of descriptor bits) of the maximal hamming distance so that two descriptors count as similar, with range [0, 1]
		float matchingThreshold_ = 0.2f;

		/// Number of detected features in the current frame.
		size_t numberDetectedFeatures_ = 0u;

		/// True, if projected 3D features are used for determine pose.
		bool useProjectedFeatures_ = false;

		// True, to use Harris corners; False, to use FAST features.
		bool usingHarrisFeatures_ = false;

	private:

		/// Internal grayscale frame with format FORMAT_Y8.
		Frame yFrame_;

		/// Internal lined integral image for the most recent frame.
		Frame linedIntegralImage_;
};

}

}

}

#endif // META_OCEAN_TRACKING_ORB_FEATURE_TRACKER_6DOF_H
