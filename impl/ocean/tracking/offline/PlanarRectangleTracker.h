/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_PLANAR_RECTANGLE_TRACKER_PLANAR_H
#define META_OCEAN_TRACKING_OFFLINE_PLANAR_RECTANGLE_TRACKER_PLANAR_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/FrameTracker.h"
#include "ocean/tracking/offline/Frame2FrameTracker.h"
#include "ocean/tracking/offline/PlaneTracker.h"

#include "ocean/cv/SubRegion.h"

#include "ocean/geometry/NonLinearOptimizationPlane.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class PlanarRectangleTracker;

/**
 * Definition of an object reference holding a PlanarRectangleTracker object.
 * @see PlanarRectangleTracker.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<PlanarRectangleTracker, OfflineTracker> PlanarRectangleTrackerRef;

/**
 * This class implements a tracker that is able to track an rectangle located one a 3D plane.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT PlanarRectangleTracker :
	virtual public FrameTracker,
	virtual public PlaneTracker
{
	protected:

		/**
		 * Definition of a shift vector holding homographies.
		 */
		typedef ShiftVector<SquareMatrix3> Homographies;

		/**
		 * Definition of a shift vector holding image point pairs.
		 */
		typedef ShiftVector<Geometry::NonLinearOptimizationPlane::ImagePointsPair> ImagePointsPairs;

		// Forward declaration of an optimization data object for camera profiles.
		class CameraPosesData;

		// Forward declaration of an optimization data object for camera profiles changing the distortion parameters only.
		class DistortionCameraPosesData;

		/**
		 * This component implements a plane tracker that uses an already known plane to create camera poses.
		 * Further, this component is able to optimized the camera profile.
		 */
		class PlaneTrackerComponent : public FramePyramidTrackerComponent
		{
			public:

				/**
				 * Creates a new plane tracking component object.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be used for homography tracking
				 * @param initialPose Initial pose that corresponds with the camera frame in that the tracking starts
				 * @param plane Already known plane that will be used for pose tracking
				 * @param resultingPoses Resulting camera poses, one for each frame
				 * @param resultingOptimizedCamera Optional resulting optimized camera profile
				 */
				PlaneTrackerComponent(PlanarRectangleTracker& parent, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Plane3& plane, OfflinePoses& resultingPoses, PinholeCamera* resultingOptimizedCamera);

				/**
				 * Destructor
				 */
				inline ~PlaneTrackerComponent() override;

			protected:

				/**
				 * Applies one component step.
				 * @see TrackerComponent::onFrame().
				 */
				IterationResult onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations) override;

				/**
				 * Component start event function.
				 * @see TrackerComponent::onStart().
				 */
				bool onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

				/**
				 * Component start event function.
				 * @see TrackerComponent::onStop().
				 */
				bool onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex) override;

				/**
				 * Tracks the camera pose between two successive frames.
				 * Only points lying on a 3D plane are used for pose determination.<br>
				 * @param previousFramePyramid Frame pyramid of the previous frame
				 * @param currentFramePyramid Frame pyramid of the current frame
				 * @param previousSubRegion Sub-region that specifies the planar area in the previous frame
				 * @param previousPose Pose that corresponds with the previous frame
				 * @param currentPose Resulting pose for the current frame
				 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
				 * @param horizontalBins Optional horizontal bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
				 * @param verticalBins Optional vertical bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
				 * @param strength Minimal strength parameter of the tracked feature points
				 * @param worker Optional worker object to distribute the computation
				 * @param trackingLayers Number of pyramid layers on which points are tracked, with range [1, pyramidLayers]
				 * @param previousImagePoints Optional resulting image points of the previous image that have been used for tracking
				 * @param currentImagePoints Optional resulting image points of the current image that have been used for tracking, each point corresponds to one point in the previous image
				 * @return The number of feature points that have been used for pose determination
				 */
				inline size_t frame2framePose(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const CV::SubRegion& previousSubRegion, const HomogenousMatrix4& previousPose, HomogenousMatrix4& currentPose, const Scalar maximalSqrError, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, Worker* worker = nullptr, const unsigned int trackingLayers = 1u, Vectors2* previousImagePoints = nullptr, Vectors2* currentImagePoints = nullptr);

				/**
				 * Optimizes an already known pose for the current frame by creating two rectified frames of the tracking plane.
				 * @param currentFrame The current camera frame
				 * @param currentPose The already known (rough) pose for the current frame
				 * @param optimizedPose Resulting optimized pose
				 * @param worker Optional worker object to distribute the computation
				 * @param validInitialImagePoints Optional resulting points in the initial camera frame that have been used for pose optimization
				 * @param validCurrentImagePoints Optional resulting points in the current camera frame that have been used for pose optimization, each point corresponds to one point of the set of initial image points
				 * @param reliableImagePoints Optional resulting statement whether the resulting image points correspondences are reliable and may be used for further optimization (e.g. of the camera profile); only False will be set, never True
				 * @return True, if succeeded
				 */
				bool optimizePose(const Frame& currentFrame, const HomogenousMatrix4& currentPose, HomogenousMatrix4& optimizedPose, Worker* worker, ImagePoints* validInitialImagePoints, ImagePoints* validCurrentImagePoints, bool* reliableImagePoints = nullptr);

				/**
				 * Optimizes the camera profile for the point correspondences that have been determined during the tracking phase of this component.
				 * @param optimizedCamera Resulting optimized camera profile
				 * @param numberFrames The number of frames that are used to optimized the camera profile
				 * @return True, if the camera could be optimized with plausible parameters
				 */
				bool optimizeCamera(PinholeCamera& optimizedCamera, const unsigned int numberFrames);

			protected:

				/// The parent tracker invoking this component.
				PlanarRectangleTracker& parent_;

				/// The camera profile that is applied in this component.
				const PinholeCamera camera_;

				/// Initial camera pose for the frame index for that the tracking starts.
				const HomogenousMatrix4& initialPose_;

				/// The plane that is applied in this component.
				const Plane3 plane_;

				/// The camera poses that are detected during tracking, one pose for each frame.
				OfflinePoses poses_;

				/// The resulting poses that are tracked in this component.
				OfflinePoses& resultingPoses_;

				/// The frame pyramid of the rectified initial frame.
				CV::FramePyramid initialRectifiedFramePyramid_;

				/// The camera profile that is used to create the initial rectified frame.
				PinholeCamera initialRectifiedCamera_;

				/// The camera pose that is used to create the initial rectified frame.
				HomogenousMatrix4 initialRectifiedPose_;

				/// The intermediate rectified frame.
				Frame intermediateRectifiedFrame_;

				/// The frame pyramid of the intermediate rectified frame.
				CV::FramePyramid intermediateRectifiedFramePyramid_;

				/// Pairs of image points that are used to determine the camera pose.
				ImagePointsPairs imagePointsPairs_;

				/// Optional resulting optimized camera profile.
				PinholeCamera* resultingOptimizedCamera_ = nullptr;

				/// Frame pyramid of the initial frame.
				CV::FramePyramid initialFramePyramid_;
		};

	public:

		/**
		 * Creates a new tracker object.
		 */
		PlanarRectangleTracker();

		/**
		 * Destructs a tracker object.
		 */
		~PlanarRectangleTracker() override;

		/**
		 * Sets the four points of a planar rectangle that is visible in the initial camera frame.
		 * Beware: Set the rectangle before the tracker has been started.<br>
		 * @param rectangleCorners The four corners of the rectangle.
		 * @return True, if succeed
		 */
		bool setInitialRectangle(const Vector2 rectangleCorners[4]);

	protected:

		/**
		 * Frame tracker run function.
		 * @see OfflineFrameTracker::applyFrameTracking().
		 */
		bool applyFrameTracking(const FrameType& frameType) override;

		/**
		 * Tracks a known plane and can determines the camera poses or/and optimize the camera profile.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param plane 3D plane to be tracked
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @param createStateEvents True, to create state events for the individual poses
		 * @param poses Resulting camera poses
		 * @param optimizedCamera Optional resulting optimized camera profile
		 * @return True, if succeeded
		 */
		bool trackPlane(const PinholeCamera& pinholeCamera, const Plane3& plane, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, const bool createStateEvents, OfflinePoses& poses, PinholeCamera* optimizedCamera);

		/**
		 * Updates the plane of this tracker and invokes the corresponding state event(s).
		 * @see PlaneTracker::updatePlane().
		 */
		void updatePlane(const Plane3& plane)override;

		/**
		 * Updates the camera of this tracker and invokes the corresponding state event(s).
		 * @see FrameTracker::updateCamera().
		 */
		void updateCamera(const PinholeCamera& pinholeCamera) override;

		/**
		 * Updates the tracker object transformation using the current camera profile and plane of this tracker.
		 * @return True, if succeeded
		 */
		bool updateObjectTransformation();

		/**
		 * Determines the angle difference between a perfect 90 degree rectangular angle and the angle of the parallelogram defined by the four points given by the user.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param plane The plane to be applied
		 * @return The absolute angle difference between a 90 degree angle and the parallelogram angles in radian
		 */
		Scalar parallelogramAngle(const PinholeCamera& pinholeCamera, const Plane3& plane);

		/**
		 * Determines the plane from four corners of a visible planar rectangle.
		 * @param pinholeCamera The pinhole camera profile that is applied
		 * @param rectangleCorners The four corners of the planar rectangle
		 * @param plane Resulting plane, if a valid plane can be estimated
		 * @return True, if succeeded
		 */
		static bool determinePlane(const PinholeCamera& pinholeCamera, const Vector2 rectangleCorners[4], Plane3& plane);

		/**
		 * Calculates the transformation (camera pose and camera profile) for the rectangle that the users has selected.
		 * The camera will be located in top of the plane so that the rectangle perfectly fits into the camera frame.
		 * @param pinholeCamera The pinhole camera profile of the camera frames
		 * @param initialPose The initial pose for the camera frame in that the user has selected the four corners of the rectangle
		 * @param rectangleCorners The four corners of the rectangle that lies in the 3D plane
		 * @param plane The 3D plane in which the four corners lie
		 * @param extraBorderPercent The distance between tracking plane and camera can be increased so that an extra border around the rectangle will be visible, with range [0, 1], (e.g. a value of 0.1 will calculate a pose that shows a border around the tracking rectangle with 10% of the frame's dimension)
		 * @param lookAtCamera Resulting camera profile that create a rectified frame of the tracking rectangle
		 * @param lookAtPose Resulting camera prose that create a rectified frame of the tracking rectangle
		 * @return True, if succeeded
		 */
		static bool lookAtTransformation(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Vector2 rectangleCorners[4], const Plane3& plane, const Scalar extraBorderPercent, PinholeCamera& lookAtCamera, HomogenousMatrix4& lookAtPose);

	protected:

		/// The initial pose that is defined for the first frame index, this pose is the default pose: locking towards the negative z-axis with y-axis as up vector at the position (0, 0, 0).
		const HomogenousMatrix4 initialPose_;

		/// The user-defined sub-region for the initial tracking frame.
		Vector2 initialRectangleCorners_[4];

		/// The sub-region that represents the initial rectangle corners as two triangles.
		CV::SubRegion initialRectangleSubRegion_;
};

inline PlanarRectangleTracker::PlaneTrackerComponent::~PlaneTrackerComponent()
{
	// Nothing else to do.
}

inline size_t PlanarRectangleTracker::PlaneTrackerComponent::frame2framePose(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const CV::SubRegion& previousSubRegion, const HomogenousMatrix4& previousPose, HomogenousMatrix4& currentPose, const Scalar maximalSqrError, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, Worker* worker, const unsigned int trackingLayers, Vectors2* previousImagePoints, Vectors2* currentImagePoints)
{
	ocean_assert(previousFramePyramid && currentFramePyramid);
	ocean_assert(previousFramePyramid.layers() == currentFramePyramid.layers());

	return Frame2FrameTracker::trackPlanarObject<15u>(camera_, previousFramePyramid, currentFramePyramid, 2u, previousPose, plane_, previousSubRegion, currentPose, maximalSqrError, horizontalBins, verticalBins, strength, worker, trackingLayers, previousImagePoints, currentImagePoints);
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_PLANAR_RECTANGLE_TRACKER_PLANAR_H
