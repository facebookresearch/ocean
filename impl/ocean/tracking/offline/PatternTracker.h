/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_PATTERN_TRACKER_H
#define META_OCEAN_TRACKING_OFFLINE_PATTERN_TRACKER_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/FrameTracker.h"
#include "ocean/tracking/offline/PlaneTracker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/geometry/Estimator.h"

#include "ocean/tracking/pattern/PatternTracker6DOF.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class PatternTracker;

/**
 * Definition of a smart object reference holding a PatternTracker object.
 * @see PatternTracker.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<PatternTracker, OfflineTracker> PatternTrackerRef;

/**
 * This class implements an offline tracker able to detect and track a previously known 2D pattern.
 * The tracker does not provide real-time performance but creates tracking results with high accuracy.<br>
 * The given video stream is passed through several times to increase the tracking quality.
 * @see PatternTrackerRef.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT PatternTracker :
	virtual public FrameTracker,
	virtual public PlaneTracker
{
	protected:

		/**
		 * This class implements the base class for all tracking components providing a rough pose.
		 * The component does not apply frame pyramids but standard frames.<br>
		 */
		class RoughPoseBaseComponent : public FrameTrackerComponent
		{
			protected:

				/**
				 * Creates a new rough tracking component.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be applied
				 */
				RoughPoseBaseComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera);

				/**
				 * Destructor
				 */
				inline ~RoughPoseBaseComponent() override;

			protected:

				/// The parent tracker invoking this component.
				PatternTracker& parent_;

				/// The camera profile that will be applied.
				const PinholeCamera camera_;

				/// The tracker determining the rough poses.
				Pattern::PatternTracker6DOF subTracker_;

				/// The id of the pattern as received by the tracker.
				unsigned int patternId_ = (unsigned int)(-1);
		};

		/**
		 * This class implements a tracking component that determines a rough guess of the horizontal field of view of the camera.
		 * The component takes the first 'n' tracking frames and extracts 2D / 3D point correspondences.<br>
		 * The camera profile with minimal projection error defines the initial guess.<br>
		 */
		class RoughCameraFovComponent : public RoughPoseBaseComponent
		{
			public:

				/**
				 * Creates a new component object.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be applied
				 * @param resultingFovX Resulting horizontal field of view in radian
				 * @param frameNumber Number of frames that are used to guess the field of view, typically a few frame of about 10 are sufficient
				 */
				RoughCameraFovComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera, Scalar& resultingFovX, const unsigned int frameNumber);

				/**
				 * Destructor
				 */
				inline ~RoughCameraFovComponent() override;

			protected:

				/**
				 * Applies one component step.
				 * @see TrackerComponent::onFrame().
				 */
				IterationResult onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations) override;

				/**
				 * Component start event function.
				 * @see TrackerComponent::onStop().
				 */
				bool onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

			protected:

				/// The resulting horizontal field of view.
				Scalar& resultingFovX_;

				/// Number of frames that will be used to guess the field of view.
				unsigned int frameNumber_ = 0u;

				/// The groups of image points that are extracted in the first frames.
				Geometry::ImagePointGroups imagePointGroups_;

				/// The groups of object points that are extracted in the first frames.
				Geometry::ObjectPointGroups objectPointGroups_;

				/// The extracted camera poses.
				HomogenousMatrices4 poses_;
		};

		/**
		 * This class implements a tracking component that determines rough camera poses by application of 2D / 3D point correspondences.
		 * In each frame 2D image point and 3D object points correspondences are determined and a equivalent camera pose is determined.<br>
		 */
		class RoughPoseComponent : public RoughPoseBaseComponent
		{
			public:

				/**
				 * Creates a new tracking component object.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be applied
				 * @param resultingPoses The resulting rough poses that will be determined
				 */
				RoughPoseComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera, OfflinePoses& resultingPoses);

				/**
				 * Destructor
				 */
				inline ~RoughPoseComponent() override;

			protected:

				/**
				 * Component start event function.
				 * @see TrackerComponent::onStart().
				 */
				bool onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

				/**
				 * Applies one component step.
				 * @see TrackerComponent::onFrame().
				 */
				IterationResult onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations) override;

				/**
				 * Component stop event function.
				 * @see TrackerComponent::onStop().
				 */
				bool onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

			protected:

				/// The resulting rough poses that will be determined.
				OfflinePoses& resultingPoses_;

				/// The internal rough poses that will be determined.
				OfflinePoses poses_;
		};

		/**
		 * This class implements a tracking component that allows to determine an accurate pose and that further allows to optimized the camera parameters.
		 */
		class FineTrackingComponent : public FrameTrackerComponent
		{
			public:

				/**
				 * Creates a new fine tracking component.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be applied
				 * @param poses Already determined poses for the tracking sequence
				 * @param iterations Number of optimization iterations
				 * @param detectionBorder Border in the tracking pattern in which no feature points will be tracked
				 * @param horizontalBinSize Size of each horizontal bins (the bin widths) in pixel
				 * @param verticalBinSize Size of each vertical bins (the bin heights) in pixel
				 * @param optimizedPoses Resulting optimized poses
				 * @param optimizedCamera Optional resulting optimized camera
				 */
				FineTrackingComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera, const OfflinePoses& poses, const unsigned int iterations, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, OfflinePoses& optimizedPoses, PinholeCamera* optimizedCamera);

				/**
				 * Destructor
				 */
				inline ~FineTrackingComponent() override;

			protected:

				/**
				 * Component start event function.
				 * @see TrackerComponent::onStart().
				 */
				bool onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

				/**
				 * Applies one component step.
				 * @see TrackerComponent::onFrame().
				 */
				IterationResult onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations) override;

				/**
				 * Component stop event function.
				 * @see TrackerComponent::onStop().
				 */
				bool onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

				/**
				 * Optimizes a pose due to accurate 2D/3D point correspondences.
				 * @param pinholeCamera The pinhole camera profile to be applied
				 * @param pose Initial pose that will be optimized
				 * @param frame The frame that matches with the initial pose
				 * @param detectionBorder Border in the tracking pattern in which no feature points will be tracked
				 * @param horizontalBinSize Size of each horizontal bins (the bin widths) in pixel
				 * @param verticalBinSize Size of each vertical bins (the bin heights) in pixel
				 * @param imagePoints Resulting imagePoints that were used to improve the pose
				 * @param objectPoints Resulting objectPoints that ware used to improve the pose, each point corresponds to one image point
				 * @param optimizedPose Resulting optimized pose
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				bool optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Frame& frame, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, ImagePoints& imagePoints, ObjectPoints& objectPoints, HomogenousMatrix4& optimizedPose, Worker* worker = nullptr);

				/**
				 * Optimizes a pose due to accurate 2D/3D point correspondences.
				 * @param pinholeCamera The pinhole camera profile to be applied
				 * @param pose Initial pose that will be optimized
				 * @param frame The frame that matches with the initial pose
				 * @param patternLayer Ideal pyramid layer of the tracking pattern best matching with the projected area of the visible patten in the frame
				 * @param detectionBorder Border in the tracking pattern in which no feature points will be tracked
				 * @param horizontalBinSize Size of each horizontal bins (the bin widths) in pixel
				 * @param verticalBinSize Size of each vertical bins (the bin heights) in pixel
				 * @param imagePoints Resulting imagePoints that were used to improve the pose
				 * @param objectPoints Resulting objectPoints that ware used to improve the pose, each point corresponds to one image point
				 * @param optimizedPose Resulting optimized pose
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				bool optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Frame& frame, const Frame& patternLayer, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, ImagePoints& imagePoints, ObjectPoints& objectPoints, HomogenousMatrix4& optimizedPose, Worker* worker = nullptr);

				/**
				 * Optimizes the camera profile due to the determined 2D/3D point correspondences.
				 * @param pinholeCamera The initial pinhole camera profile to be used
				 * @param numberFrames Number of individual frames that will be used to optimized the camera profile
				 * @param iterations Number of improvement iterations
				 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
				 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
				 * @param optimizedCamera Resulting optimized camera profile
				 * @return True, if succeeded
				 */
				bool optimizeCamera(const PinholeCamera& pinholeCamera, const unsigned int numberFrames, const unsigned int iterations, const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, PinholeCamera& optimizedCamera);

			protected:

				/// The parent tracker invoking this component.
				PatternTracker& parent_;

				/// The camera profile that will be applied.
				const PinholeCamera camera_;

				/// The frame pyramid of the pattern image.
				CV::FramePyramid patternFramePyramid_;

				/// Already known poses for each frame.
				const OfflinePoses& poses_;

				/// Number of optimization iterations that will applied in this component.
				const unsigned int iterations_ = 0u;

				/// The border in the pattern frame in which no feature will be tracked, in pixel
				const unsigned int detectionBorder_ = 0u;

				/// Size of each horizontal bins (the bin widths) in pixel
				const unsigned int horizontalBinSize_ = 0u;

				/// Size of each vertical bins (the bin heights) in pixel
				const unsigned int verticalBinSize_ = 0u;

				/// Resulting optimized poses for each frame.
				OfflinePoses& optimizedPoses_;

				/// Optional resulting optimized camera profile.
				PinholeCamera* optimizedCamera_ = nullptr;

				/// The set of image points that are used for pose determination, one set for each frame.
				ShiftVector<ImagePoints> imagePointsSet_;

				/// The set of object points that are used for pose determination, one set for each frame.
				ShiftVector<ObjectPoints> objectPointsSet_;
		};

	public:

		/**
		 * Creates a new pattern tracker object.
		 */
		PatternTracker();

		/**
		 * Returns the current dimension of the tracking pattern.
		 * @return Current tracking pattern dimension
		 */
		inline Vector2 patternDimension() const;

		/**
		 * Sets the tracking pattern that will be tracked in the video stream.
		 * @param frame The frame pattern
		 * @param dimension The dimension of the given pattern in meter
		 * @return True, if succeeded
		 */
		bool setPattern(const Frame& frame, const Vector2& dimension);

		/**
		 * Sets a camera object that will be used by this tracker.
		 * @see OfflineTracker::setCamera().
		 */
		bool setCamera(const PinholeCamera& pinholeCamera, const Scalar cameraPrecision, const bool optimizeCamera) override;

	protected:

		/**
		 * Frame tracker run function.
		 * @see OfflineFrameTracker::applyFrameTracking().
		 */
		bool applyFrameTracking(const FrameType& frameType) override;

		/**
		 * Guesses the rough field of view of the camera.
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @param cameraFovDeterminationFrames Number of initial frames that are used to determine a rough approximation of the camera's field of view, 0 to avoid the determination
		 * @return True, if succeeded
		 */
		bool determineRoughCameraFov(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, const unsigned int cameraFovDeterminationFrames = 10u);

		/**
		 * Determines the initial rough poses for the entire video stream.
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @return True, if succeeded
		 */
		bool determineRoughPoses(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

		/**
		 * Closes the tracking gaps (break-downs) for the provided video stream.
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @param minimalCorrespondences Minimal number of correspondences that need to be exceeded so that a pose counts as valid (and not as gap)
		 * @return True, if succeeded
		 */
		bool closeGaps(const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, const unsigned int minimalCorrespondences = 50u);

		/**
		 * Applies a fine adjustment of the camera poses.
		 * @param iterations Number of adjustment iterations for each frame
		 * @param detectionBorder Border in the tracking pattern in which no feature points will be tracked
		 * @param horizontalBinSize Size of each horizontal bins (the bin widths) in pixel
		 * @param verticalBinSize Size of each vertical bins (the bin heights) in pixel
		 * @param optimizeCamera True to apply a camera profile optimization after the fine tracking has been applied
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 */
		bool fineAdjustment(const unsigned iterations, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, const bool optimizeCamera, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

		/**
		 * Calculates the projection area of the defined 3D object pattern.
		 * @param pose The camera pose
		 * @param pinholeCamera The pinhole camera object
		 * @return Resulting projection area
		 */
		Scalar projectedPatternArea(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera) const;

		/**
		 * Returns projected 2D triangles that cover the area of the 3D tracking pattern.
		 * @param pose The camera pose
		 * @param pinholeCamera The pinhole camera object
		 * @return Set of 2D triangles
		 */
		Triangles2 projectedPatternTriangles(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera) const;

		/**
		 * Updates the pattern dimension of this tracker and invokes the corresponding state event(s).
		 * @param dimension The dimension of the given pattern in meter
		 * @return True, if succeeded
		 */
		bool updatePatternDimension(const Vector2& dimension);

	protected:

		/// The frame pattern that is tracked during the video stream.
		Frame patternFrame_;

		/// Dimension of the frame pattern in meter.
		Vector2 patternDimension_ = Vector2(0, 0);

		/// The four corners of the tracking pattern.
		Vector3 patternCorners_[4];
};

inline PatternTracker::RoughPoseBaseComponent::~RoughPoseBaseComponent()
{
	// Nothing else to do.
}

inline PatternTracker::RoughCameraFovComponent::~RoughCameraFovComponent()
{
	// Nothing else to do.
}

inline PatternTracker::RoughPoseComponent::~RoughPoseComponent()
{
	// Nothing else to do.
}

inline PatternTracker::FineTrackingComponent::~FineTrackingComponent()
{
	// Nothing else to do.
}

inline Vector2 PatternTracker::patternDimension() const
{
	const ScopedLock scopedLock(lock_);

	const Vector2 dimension(patternDimension_);
	return dimension;
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_PATTERN_TRACKER_H
