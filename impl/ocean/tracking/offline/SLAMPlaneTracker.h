/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_SLAM_TRACKER_PLANAR_H
#define META_OCEAN_TRACKING_OFFLINE_SLAM_TRACKER_PLANAR_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/FrameTracker.h"
#include "ocean/tracking/offline/PlaneTracker.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/SubRegion.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/geometry/NonLinearOptimizationPlane.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class SLAMPlaneTracker;

/**
 * Definition of an object reference holding a SLAMPlaneTracker object.
 * @see SLAMPlaneTracker.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<SLAMPlaneTracker, OfflineTracker> SLAMPlaneTrackerRef;

/**
 * This class implements a SLAM tracker.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT SLAMPlaneTracker :
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

		/**
		 * This class implements a homography tracker component that determines an accurate homography for the planar tracking sub-region between successive frames.
		 * Further, this component determines an initial 3D plane that is extracted from the tracked homographies.<br>
		 * Commonly, the accuracy of the plane's normal is in a range of a few degrees.<br>
		 */
		class HomographyTrackerComponent : public FramePyramidTrackerComponent
		{
			protected:

				/**
				 * Definition of a pair of normals.
				 */
				typedef std::pair<Vector3, Vector3> NormalPair;

				/**
				 * Definition of a shift vector holding normal pairs.
				 */
				typedef ShiftVector<NormalPair> NormalPairs;

			public:

				/**
				 * Creates a new homography tracking component object.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be used for homography tracking
				 * @param resultingPlane The initial 3D plane that has been extracted from the homographies
				 * @param maxPlaneAngle Maximal angle between plane normals so that they count as similar
				 * @param optimizedCamera Optional resulting optimized camera profile
				 */
				HomographyTrackerComponent(SLAMPlaneTracker& parent, const PinholeCamera& pinholeCamera, Plane3& resultingPlane, const Scalar maxPlaneAngle = Numeric::deg2rad(5), PinholeCamera* optimizedCamera = nullptr);

				/**
				 * Destructor
				 */
				inline ~HomographyTrackerComponent() override;

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
				 * Determines an initial homography for a planar sub-region between two frames.
				 * @param previousFramePyramid Frame pyramid of the previous frame
				 * @param currentFramePyramid Frame pyramid of the current frame
				 * @param previousSubRegion Sub-region that specifies the planar area in the previous frame
				 * @param homography Resulting homography that has been determined
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				bool frame2frameHomography(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const CV::SubRegion& previousSubRegion, SquareMatrix3& homography, Worker* worker = nullptr);

				/**
				 * Optimizes the homography between two frames.
				 * @param previousFramePyramid Frame pyramid of the previous frame
				 * @param currentFrame Current frame
				 * @param previousSubRegion Sub region defining the planar area in the previous frame
				 * @param homography Already known homography between the previous and the current frame, the homography transforms points in the previous frame to points in the current frame
				 * @param optimizedHomography Resulting optimized homography
				 * @param previousImagePoints Resulting image points in the previous frame that have been used to optimized the homography
				 * @param currentImagePoints Resulting image points in the current frame that have been used to optimized the homography, each point corresponds to one point in the previous frame
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				bool optimizeHomography(const CV::FramePyramid& previousFramePyramid, const Frame& currentFrame, const CV::SubRegion& previousSubRegion, const SquareMatrix3& homography, SquareMatrix3& optimizedHomography, Vectors2& previousImagePoints, Vectors2& currentImagePoints, Worker* worker = nullptr);

				/**
				 * Determines the two best matching plane candidates from the set of plane normals pairs that have been found during the homography factorizations.
				 * @param candidatePair Resulting pair of plane normal candidates
				 * @param maxAngle Maximal angle between two normals so that they count as similar, in radian
				 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
				 * @param initialFrameIndex Index of the frame at that the tracking process will start
				 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
				 * @return True, if succeeded
				 */
				bool determinePlaneCandidates(NormalPair& candidatePair, const Scalar maxAngle, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

				/**
				 * Optimizes the camera profile that has been used to track the homography and to create the initial plane.
				 * This function takes a subset of all determined homographies and minimizes the global distances between image points in the initial and the remaining camera frames.<br>
				 * @param optimizedCamera Resulting optimized camera
				 * @param numberFrames Number of frames (and thus homographies) that are used to optimized the camera profile
				 * @return True, if succeeded
				 */
				bool optimizeCamera(PinholeCamera& optimizedCamera, const unsigned int numberFrames = 10u);

			protected:

				/// The parent tracker invoking this component.
				SLAMPlaneTracker& componentParent_;

				/// The camera profile that is applied in this component.
				const PinholeCamera componentCamera_;

				/// The maximal angle between two plane normals so that these normals count as similar.
				const Scalar componentMaxPlaneAngle_;

				/// Random number generator.
				RandomGenerator componentRandomGenerator_;

				/// The set of pairs of image points that have been used to determine the individual homographies.
				ImagePointsPairs componentImagePointsPairs_;

				/// The determined homographies of this component.
				Homographies componentHomographies_;

				/// The determined pairs of plane normals, one pair for each detected homography.
				NormalPairs componentNormalPairs_;

				/// Resulting plane.
				Plane3& componentResultingPlane_;

				/// Optional resulting optimized camera profile.
				PinholeCamera* componentResultingOptimizedCamera_;

				/// Frame pyramid of the initial frame.
				CV::FramePyramid componentInitialFramePyramid_;

				/// Intermediate frame pyramid.
				CV::FramePyramid componentIntermediateFramePyramid_;

				/// Intermediate homography frame.
				Frame componentIntermediateHomographyFrame_;

				/// The progress event stack layer of this component.
				ScopedEventStackLayer componentScopedEventLayer_;
		};

		/**
		 * This class implements a plane verifier component that selects the unique valid plane from a set of two plane candidates.
		 * Homography provides two planes for points correspondences, the correct plane can be determined with this component.<br>
		 */
		class ValidPlaneIdentifyComponent : public FramePyramidTrackerComponent
		{
			public:

				/**
				 * Creates a new component object.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that will be used for validation
				 * @param initialPose Initial pose that is connected with the first frame index and is valid for both plane candidates
				 * @param planeCandidates Two plane candidates, only one is correct for the given frame sequence
				 * @param initialSubRegion Sub-region defining the planar area in the initial frame
				 * @param resultingPlane Resulting plane that is correct for the frame sequence (this plane is one of the given plane candidates)
				 * @param maximalSqrDistance Maximal square distance between a projected plane point and the visible point in the camera frame to count as inlier, in pixel
				 */
				ValidPlaneIdentifyComponent(SLAMPlaneTracker& parent, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Plane3 planeCandidates[2], const CV::SubRegion& initialSubRegion, Plane3& resultingPlane, const Scalar maximalSqrDistance = Scalar(3 * 3));

				/**
				 * Destructor
				 */
				inline ~ValidPlaneIdentifyComponent() override;

			protected:

				/**
				 * Component decreasing start event function.
				 * This function is executed before the decreasing component iterations are applied.
				 * @return True, if the start has succeeded
				 */
				bool onStartDecreasing() override;

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

				/// Parent tracker that invokes this component.
				SLAMPlaneTracker& componentParent_;

				/// The camera profile that is applied.
				const PinholeCamera componentCamera_;

				/// Initial camera pose for the frame index for that the tracking starts.
				const HomogenousMatrix4& componentInitialPose_;

				/// Two previous camera poses, individual for each plane candidate.
				HomogenousMatrix4 componentPreviousPoses_[2];

				/// The number of valid point correspondences, individual for each plane candidate.
				unsigned int componentValidCorrespondences_[2];

				/// Two plane candidates, one is correct.
				Plane3 componentPlaneCandidates_[2];

				/// 3D plane object points, individual for each plane candidate.
				Vectors3 componentObjectPoints_[2];

				/// Initial sub-region defining the planar area in the initial frame.
				const CV::SubRegion& componentInitialSubRegion_;

				/// Initial image points lying within the planar area in the initial frame.
				Vectors2 componentInitialImagePoints_;

				/// Initial object points lying on the 3D plane, individual for each plane candidate.
				Vectors3 componentInitialObjectPoints_[2];

				/// Maximal square distance between a projected 3D plane object point and a visible camera image point.
				const Scalar componentMaximalSqrDistance_;

				/// Resulting valid plane.
				Plane3& componentResultingPlane_;

				/// The image points of the previous frame.
				Vectors2 componentPreviousImagePoints_;

				/// The image points of the current frame.
				Vectors2 componentCurrentImagePoints_;
		};

		/**
		 * This class implements a plane tracker component that is able to track a known plane and further is able to optimize the plane orientation.
		 */
		class PlaneTrackerComponent : public FramePyramidTrackerComponent
		{
			public:

				/**
				 * Creates a new component object.
				 * @param parent The parent tracker object that invokes this component
				 * @param pinholeCamera The pinhole camera profile that is applied
				 * @param initialPose Initial pose that will be used for the initial tracking frame
				 * @param plane Known plane that will be tracked
				 * @param createStateEvents True, to create state events for the individual poses
				 * @param poses Optional resulting poses for each frame between lower and upper tracking boundary
				 * @param optimizedPlane Optional resulting optimized plane
				 */
				PlaneTrackerComponent(SLAMPlaneTracker& parent, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Plane3& plane, const bool createStateEvents, OfflinePoses* poses, Plane3* optimizedPlane);

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
				 * Optimizes the pose for a given frame.
				 * @param previousFramePyramid Frame pyramid of the previous frame
				 * @param currentFrame Current frame
				 * @param previousSubRegion Sub region defining the planar area in the previous frame
				 * @param previousPose Known pose for the previous frame
				 * @param currentPose The known pose for the current frame
				 * @param optimizedPose Resulting optimized pose for the given frame pair
				 * @param previousImagePoints Resulting image points in the previous frame that have been used to optimized the pose
				 * @param currentImagePoints Resulting image points in the current frame that have been used to optimized the pose, each point corresponds to one point in the previous frame
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				bool optimizePose(const CV::FramePyramid& previousFramePyramid, const Frame& currentFrame, const CV::SubRegion& previousSubRegion, const HomogenousMatrix4& previousPose, const HomogenousMatrix4& currentPose, HomogenousMatrix4& optimizedPose, Vectors2& previousImagePoints, Vectors2& currentImagePoints, Worker* worker = nullptr);

				/**
				 * Optimizes the plane.
				 * @param optimizedPlane Resulting optimized plane
				 * @param numberFrames Number for frames that are used to optimize the plane
				 * @return True, if succeeded
				 */
				bool optimizePlane(Plane3& optimizedPlane, const unsigned int numberFrames = 10u);

			protected:

				/// Parent tracker that invokes this component.
				SLAMPlaneTracker& componentParent_;

				/// The camera profile that is applied for tracking.
				const PinholeCamera& componentCamera_;

				/// The initial pose that is used for the initial frame.
				const HomogenousMatrix4& componentInitialPose_;

				/// Plane that is be used for tracking.
				const Plane3& componentPlane_;

				/// Statement whether this component creates state events for the individual poses.
				const bool componentCreateStateEvents_;

				/// Optional resulting optimized plane.
				Plane3* componentResultingPlane_;

				/// Optional resulting poses.
				OfflinePoses* componentResultingPoses_;

				/// Homographies that are detected during tracking, each homography is defined in relation to the initial tracking frame.
				Homographies componentHomographies_;

				/// The camera poses that are detected during tracking, one pose for each frame
				OfflinePoses componentPoses_;

				/// The set of pairs of image points that have been used to determine the individual poses.
				ImagePointsPairs componentImagePointsPairs_;

				/// Frame pyramid of the initial frame.
				CV::FramePyramid componentInitialFramePyramid_;

				/// Intermediate frame pyramid.
				CV::FramePyramid componentIntermediateFramePyramid_;

				/// Intermediate homography frame.
				Frame componentIntermediateHomographyFrame_;
		};

	public:

		/**
		 * Creates a new tracker object.
		 */
		SLAMPlaneTracker();

		/**
		 * Destructs a tracker object.
		 */
		~SLAMPlaneTracker() override;

		/**
		 * Sets an initial tracking area defined by a set of 2D triangles located in the initial camera frame.
		 * Beware: Set the tracking area before the tracker has been started.<br>
		 * @param triangles The triangles defining the initial tracking area
		 * @return True, if succeed
		 */
		bool setInitialTrackingArea(const Triangles2& triangles);

		/**
		 * Sets an initial tracking area defined by a set of 2D triangles located in the initial camera frame.
		 * Beware: Set the tracking area before the tracker has been started.<br>
		 * @param yMask The 8 bit binary mask defining the sub-region, pixel values not equal to 0xFF define the sub-region, must be valid
		 * @return True, if succeed
		 */
		bool setInitialTrackingArea(const Frame& yMask);

	protected:

		/**
		 * Frame tracker run function.
		 * @see OfflineFrameTracker::applyFrameTracking().
		 */
		bool applyFrameTracking(const FrameType& frameType) override;

		/**
		 * Determines an initial plane.
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @return True, if succeeded
		 */
		bool determinePlane(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

		/**
		 * Determines an initial plane.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @param optimizedPlane Resulting optimized plane
		 * @param optimizedCamera Optional resulting optimized camera
		 * @return True, if succeeded
		 */
		bool determineInitialPlane(const PinholeCamera& pinholeCamera, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, Plane3& optimizedPlane, PinholeCamera* optimizedCamera = nullptr);

		/**
		 * Optimizes the initial plane.
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @return True, if succeeded
		 */
		bool optimizePlane(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex);

		/**
		 * Tracks a known plane and can determines the camera poses or/and optimize the plane.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param plane 3D plane to be tracked
		 * @param lowerFrameIndex Index of the frame defining the lower (including) tracking boundary
		 * @param initialFrameIndex Index of the frame at that the tracking process will start
		 * @param upperFrameIndex Index of the frame defining the upper (including) tracking boundary
		 * @param createStateEvents True, to create state events for the individual poses
		 * @param optimizedPlane Optional resulting optimized plane
		 * @param poses Optional resulting camera poses
		 * @return True, if succeeded
		 */
		bool trackPlane(const PinholeCamera& pinholeCamera, const Plane3& plane, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, const bool createStateEvents, Plane3* optimizedPlane = nullptr, OfflinePoses* poses = nullptr);

		/**
		 * Updates the plane of this tracker and invokes the corresponding state event(s).
		 * @see PlaneTracker::updatePlane().
		 */
		void updatePlane(const Plane3& plane) override;

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

	protected:

		/// The initial pose that is defined for the first frame index, this pose is the default pose: locking towards the negative z-axis with y-axis as up vector at the position (0, 0, 0).
		const HomogenousMatrix4 initialPose_;

		/// The user-defined sub-region for the initial tracking frame.
		CV::SubRegion initialSubRegion_;
};

inline SLAMPlaneTracker::HomographyTrackerComponent::~HomographyTrackerComponent()
{
	// Nothing else to do.
}

inline SLAMPlaneTracker::ValidPlaneIdentifyComponent::~ValidPlaneIdentifyComponent()
{
	// Nothing else to do.
}

inline SLAMPlaneTracker::PlaneTrackerComponent::~PlaneTrackerComponent()
{
	// Nothing else to do.
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_SLAM_TRACKER_PLANAR_H
