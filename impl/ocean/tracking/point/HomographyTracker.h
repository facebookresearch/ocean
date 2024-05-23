/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_HOMOGRAPHY_TRACKER_H
#define META_OCEAN_TRACKING_POINT_HOMOGRAPHY_TRACKER_H

#include "ocean/tracking/point/Point.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Box2.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * This class implements a homography-based tracker for planar backgrounds.
 * The tracker can be started with a region of interest located on a planar background, while the entire surrounding area around the region may also be used for tracking.
 * @ingroup trackingpoint
 */
class OCEAN_TRACKING_POINT_EXPORT HomographyTracker
{
	protected:

		/**
		 * Definition of homography qualities.
		 */
		enum HomographyQuality
		{
			/// The homography is invalid.
			HQ_FAILED,
			/// The homography has a moderate quality, good enough for visualization, but not good enough to e.g., use the frame as key-frame.
			HQ_MODERATE,
			/// The homography has a good quality so that the corresponding frame e.g., can be used as key-frame.
			HQ_GOOD
		};

		/**
		 * Definition of a pyramid of 2D image points, mainly a vector with image points located in individual pyramid frames.
		 */
		typedef std::vector<Vectors2> Vectors2Pyramid;

		/**
		 * Definition of a pyramid of 3D object points, mainly a vector with object points visible in individual pyramid frames.
		 */
		typedef std::vector<Vectors3> Vectors3Pyramid;

		/**
		 * This class composes several parameters necessary for a key-frame.
		 * Key-frames are used for re-initialization of the tracker (whenever the tracker failed during frame-to-frame tracking).
		 */
		class KeyFrame
		{
			public:

				/**
				 * Creates a new key frame object.
				 */
				inline KeyFrame();

				/**
				 * Creates a new key frame object.
				 * @param timestamp The timestamp of the key frame, must be valid
				 * @param initialPointsPyramid The pyramid of points defined in the initial camera frame, corresponding with points defined in the key-frame, one for each key-frame point
				 * @param pointsPyramid The pyramid of points defined in the key-frame (pyramid)
				 * @param pyramid The frame pyramid of the key-frame which can be used for re-localization
				 * @param globalHomography The homography transforming points defined in the initial frame to points defined in the key-frame kHi
				 * @param cameraOrientation The orientation of the camera when creating the key frame, if known.
				 */
				inline KeyFrame(const Timestamp& timestamp, const Vectors2Pyramid& initialPointsPyramid, const Vectors2Pyramid& pointsPyramid, const CV::FramePyramid& pyramid, const SquareMatrix3& globalHomography, const Quaternion& cameraOrientation);

				/**
				 * Returns whether this object stores an actual key frame.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			public:

				/// The timestamp of the key frame.
				Timestamp timestamp_;

				/// The pyramid of points defined in the initial camera frame, corresponding with points defined in the key-frame, one for each key-frame point.
				Vectors2Pyramid initialPointsPyramid_;

				/// The pyramid of points defined in the key-frame (pyramid).
				Vectors2Pyramid pointsPyramid_;

				/// The frame pyramid of the key-frame which can be used for re-localization.
				CV::FramePyramid pyramid_;

				/// The homography transforming points defined in the initial frame to points defined in the key-frame (keyFramePoint = globalHomography_ * initialPoint) = kHi.
				SquareMatrix3 globalHomography_;

				/// The orientation of the camera when creating the key frame, if known.
				Quaternion cameraOrientation_;
		};

		static constexpr size_t numberKeyFrames_ = 2;

	public:

		/**
		 * Creates a new tracker object.
		 */
		inline HomographyTracker();

		/**
		 * Sets a new region of interest (or resets an existing region of interest).
		 * The tracker will be set to a new initial state and any resulting homography will be defined in relation to this initial state.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param region The region of interest to be set, defined in the coordinate system of the image frames used for tracking, must be valid
		 * @return True, if succeeded
		 */
		bool resetRegion(const AnyCamera& camera, const Box2& region);

		/**
		 * Sets a new region of interest (or resets an existing region of interest).
		 * The tracker will be set to a new initial state and any resulting homography will be defined in relation to this initial state.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param region The region of interest to be set, defined in the coordinate system of the image frames used for tracking, must be valid
		 * @param cameraOrientation The orientation of the camera matching with the given (current) camera frame, defined w.r.t. the world coordinate system (wTc), must be valid
		 * @param planeNormal Normal the planar background at the region of interest, defined w.r.t. the world coordinate system (wN), must be valid
		 * @param pose Optional resulting 6DOF camera pose for the initial camera frame, transforming points defined in the coordinate system of the camera to points defined in the world coordinate system (wTc)
		 * @param plane Optional resulting transformation transforming points defined in the coordinate system of the plane to points defined in the coordinate system of the world coordinate system (wTp)
		 * @return True, if succeeded
		 */
		bool resetRegion(const AnyCamera& camera, const Box2& region, const Quaternion& cameraOrientation, const Vector3& planeNormal, HomogenousMatrix4* pose = nullptr, HomogenousMatrix4* plane = nullptr);

		/**
		 * Determines the homography between the current frame and the initial frame.
		 * The initial frame is the frame given after the region of interest has been (re-)set.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param yFrame The current image frame for which the homography will be determined (matching with the camera profile), must have a pixel format FORMAT_Y8, must be valid
		 * @param homography The resulting homography transforming a point defined in the initial frame to a point defined in the current frame (currentPoint = homograph * initialPoint)
		 * @param pose Optional resulting 6DOF camera pose in case the tracker has been reset with a known 3D plane normal, otherwise an invalid pose
		 * @param cameraOrientation The orientation of the camera matching with the given (current) camera frame, defined w.r.t. the world coordinate system (wTc)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool determineHomography(const AnyCamera& camera, const Frame& yFrame, SquareMatrix3& homography, HomogenousMatrix4* pose, const Quaternion& cameraOrientation = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Resets the homography tracker.
		 */
		inline void reset();

	protected:

		/**
		 * Adds new feature points to all pyramid layers (at least to all desired layers e.g., layer 0 and 2) if the layers do not contain enough feature points already.
		 * @param yFramePyramid The frame pyramid of the frame for which the new feature points will be determined, must be valid
		 * @param pointsPyramid The pyramid of already existing feature points for the given frame (individual points for individual pyramid layers)
		 * @param initialPointsPyramid The pyramid of feature points defined in the initial camera frame corresponding with the points in 'pointsPyramid'
		 * @param homography The homography transforming points defined in the initial camera frame to points defined in the given camera frame (pyramid), must be valid, (currentPoint = homography * initialPoint)
		 * @param region The region of interest defined in the initial camera frame, this region will be used prioritize feature points in this area, can be invalid if no region of interest is known
		 * @param minimalFeatures The minimal number of feature points each pyramid layer should contain, with range [1, infinity)
		 * @param maximalFeatures The maximal number of feature points each pyramid layer should contain, with range [minimalFeatures, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see addNewFeaturePointsToPyramidLayer().
		 */
		static bool addNewFeaturePointsToPyramid(const CV::FramePyramid& yFramePyramid, Vectors2Pyramid& pointsPyramid, Vectors2Pyramid& initialPointsPyramid, const Box2& region, const SquareMatrix3& homography, const unsigned int minimalFeatures = 30u, const unsigned int maximalFeatures = 60u, Worker* worker = nullptr);

		/**
		 * Adds new feature points to one specific pyramid layer if the layer does not contain enough feature points already.
		 * This function is supposed to be called out of 'addNewFeaturePointsToPyramid()'
		 * @param yFramePyramid The frame pyramid of the frame for which the new feature points will be determined, must be valid
		 * @param pointsPyramid The pyramid of already existing feature points (individual points for individual pyramid layers)
		 * @param layerSubRegionTriangles The triangles defined a sub-region within the specified layer in which more feature points will be determined compared to the surrounding area, no triangle to avoid the usage of a hot spot
		 * @param layer The pyramid layer for which the new feature points will be added, with range [0, yFramePyramid.size() - 1]
		 * @param featureThreshold The strength threshold each new feature point must exceed to count as new feature candidate, with range [0, 255]
		 * @param minimalFeatures The minimal number of feature points each pyramid layer should contain, with range [1, infinity)
		 * @param maximalFeatures The maximal number of feature points (the ideal number of feature points) the pyramid layer should contain, in case the layer contains more points already, no additional point is added, with range [minimalFeatures, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see addNewFeaturePointsToPyramid().
		 */
		static bool addNewFeaturePointsToPyramidLayer(const CV::FramePyramid& yFramePyramid, Vectors2Pyramid& pointsPyramid, const Triangles2& layerSubRegionTriangles, const unsigned int layer, const unsigned int featureThreshold = 25u, const unsigned int minimalFeatures = 30u, const unsigned int maximalFeatures = 60u, Worker* worker = nullptr);

		/**
		 * @param yPreviousFramePyramid The pyramid image of the previous frame with pixel format FORMAT_Y8 (1 channel, uint8_t), must be valid
		 * @param yCurrentFramePyramid The pyramid image of the current frame with same pixel format as 'yPreviousFramePyramid', must be valid
		 * @param previousPointsPyramid The image points located in the pyramid image of the previous frame, which will be tracked to the current frame, (with different points for different pyramid layers)
		 * @param currentPointsPyramid The resulting image points located in the pyramid image of the current frame, one point for each point in the corresponding previous image layer
		 * @param startLayer The layer within the pyramid image of the previous frame from which the image points will be tracked, with range [0, yPreviousFramePyramid.layers() - 1]
		 * @param maximalOffsetPercent The maximal pixel offset between corresponding image points defined in percentage in relation to the max(layerWidth, layerHeight), with range [0, 1]
		 * @param validTrackedPointIndices The resulting indices of all previous image points which could be tracked reliably
		 * @param roughHomography Optional homography providing a rough guess for the location of the points in the current frame, defined for the finest pyramid layer (finestCurrentPoint = roughHomography * finestPreviousPoint), a null matrix otherwise
		 * @param coarsestLayerRadius The search radius on the coarsest pyramid layer used for tracking, with range [1, infinity)
		 * @param subPixelIterations The number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool trackPoints(const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, Vectors2Pyramid& previousPointsPyramid, Vectors2Pyramid& currentPointsPyramid, const unsigned int startLayer, const float maximalOffsetPercent, Indices32& validTrackedPointIndices, const SquareMatrix3& roughHomography = SquareMatrix3(false), const unsigned int coarsestLayerRadius = 2u, const unsigned int subPixelIterations = 2u, Worker* worker = nullptr);

		/**
		 * Determines the homography between two consecutive camera frames based on known feature points located in the previous frame (pyramid).
		 * @param camera The camera profile defining the projection, must be valid
		 * @param plane The 3D plane on which the image points (the corresponding 3D object points) are located, must be valid
		 * @param yPreviousFramePyramid The frame pyramid of the previous camera frame, must have a pixel format FORMAT_Y8, must be valid
		 * @param yCurrentFramePyramid The frame pyramid of the current camera frame, must have a pixel format FORMAT_Y8, must be valid
		 * @param previousPointsPyramid The pyramid of known points located in the previous camera frame, these points will be used for homography estimation, points which could not be used for tracking will be removed during homography estimation, must be valid
		 * @param currentPointsPyramid The resulting pyramid of points located in the current camera frame, one point of each point in the pyramid of points for the previous camera frame 'previousPointsPyramid'
		 * @param initialPointsPyramid The pyramid of known points located in the initial camera frame, one point of each point in the pyramid of points for the previous camera frame 'previousPointsPyramid'
		 * @param previousHomography The known homography transforming points defined in the initial camera frame to points defined in the previous camera frame (defined for the finest pyramid layer), must be valid, (previousPoint = previousHomography * initialPoint)
		 * @param region The region of interest within the camera frame, must be valid
		 * @param homography The resulting homography transforming points defined in the initial camera frame to points defined in the current camera frame (defined for the finest pyramid layer), (currentPoint = previousHomography * initialPoint)
		 * @param pose Optional resulting 6DOF camera pose in case the tracker has been reset with a known 3D plane normal, otherwise an invalid pose
		 * @param predictedLocalHomography Optional known prediction of the homography transformation points defined in the previous camera frame to points defined in the current camera frame, a null matrix otherwise, (currentPoint = predictedLocalHomography * previousPoint)
		 * @param initialCameraOrientation The orientation of the camera frame for the initial frame, (wRi), if known
		 * @param currentCameraOrientation The orientation of the camera frame for the current frame, (wRc), if known
		 * @param randomGenerator Random generator to be used
		 * @param explicitMaximalOffsetPercent An explicit threshold defining the maximal offset between two corresponding feature points, defined in relation to the maximal frame size, with range [0, 1], -1 to use internal thresholds
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static HomographyQuality determineHomographyWithPyramid(const AnyCamera& camera, const Plane3& plane, const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, Vectors2Pyramid& previousPointsPyramid, Vectors2Pyramid& currentPointsPyramid, Vectors2Pyramid& initialPointsPyramid, const SquareMatrix3& previousHomography, const Box2& region, SquareMatrix3& homography, HomogenousMatrix4* pose, const SquareMatrix3& predictedLocalHomography, const Quaternion& initialCameraOrientation, const Quaternion& currentCameraOrientation, RandomGenerator& randomGenerator, const float explicitMaximalOffsetPercent = -1.0f, Worker* worker = nullptr);

		/**
		 * Returns whether the region of interest is visible based on a simple angle threshold.
		 * The function determines the angle between viewing direction (the z-axis) for the current frame and the initial frame.<br>
		 * In case, the viewing angle exceeds a specified threshold, the region is rated to be invisible.<br>
		 * If one or both provided camera orientation are unknown, the region is rated to be visible.
		 * @param wRi The rotation of the camera matching with the initial camera frame, can be invalid if unknown
		 * @param wRc The rotation of the camera matching with the current camera frame, can be invalid if unknown
		 * @param maximalAngle The maximal angle between both viewing directions, in radian, with range [0, PI_2]
		 */
		static bool isRegionVisible(const Quaternion& wRi, const Quaternion& wRc, const Scalar maximalAngle = Numeric::deg2rad(50));

		/**
		 * Returns whether the region of interest is visible based on the known homography for the current frame.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param globalHomography The homography transforming initial image points to current image points: currentImagePoint = globalHomography_ * initialImagePoint, (globalHomography_ = cHi)
		 * @param initialRegion The initial region of interest, must be valid
		 * @return True, if so
		 */
		static bool isRegionVisible(const AnyCamera& camera, const SquareMatrix3& globalHomography, const Box2& initialRegion);

		/**
		 * Returns whether a given homography is plausible in the context of a previous homography.
		 * Based on a region of interest, the homography is rated based on the angular difference within the transformed region of interest.<br>
		 * In case, the angle between two neighboring edges of the ROI exceeds a given threshold, the homography is rated is invalid.
		 * @param pHi The homography transforming points defined in the initial camera frame to points defined in the previous camera frame (previousPoint = pHi * initialPoint), must be valid
		 * @param cHi The homography transforming points defined in the initial camera frame to points defined in the current camera frame (currentPoint = cHi * initialPoint), must be valid
		 * @param initialRegion The region of interest defined in the initial camera frame, must be valid
		 * @param maximalAngleChange The maximal allowed change of inner angles of the ROI so that the homography counts as plausible, in radian, with range [0, PI_4]
		 * @return True, if so
		 */
		static bool isHomographyPlausible(const SquareMatrix3& pHi, const SquareMatrix3& cHi, const Box2& initialRegion, const Scalar maximalAngleChange = Numeric::deg2rad(5));

		/**
		 * Returns whether a given 6DOF camera pose if plausible based on e.g., an IMU-based camera orientation.
		 * @param currentPose The current 6DOF camera pose to be checked, must be valid
		 * @param initialCameraOrientation The orientation of the initial camera frame, (wTi), if known
		 * @param currentCameraOrientation The orientation of the current camera frame, (wTc), if known
		 * @param maximalAngle The maximal angle between the orientation of the given 6DOF camera pose and the IMU-based camera orientation so that the pose counts as plausible, in radian, with range [0, PI_2]
		 * @return True, if so
		 */
		static bool isPosePlausible(const HomogenousMatrix4& currentPose, const Quaternion& initialCameraOrientation, const Quaternion& currentCameraOrientation, const Scalar maximalAngle = Numeric::deg2rad(3));

	protected:

		/// The frame pyramid of the previous frame.
		CV::FramePyramid previousFramePyramid_;

		/// The frame pyramid of the current frame.
		CV::FramePyramid currentFramePyramid_;

		/// The random generator object.
		RandomGenerator randomGenerator_;

		/// The image points located in the coordinate system of the initial frame (the frame in which the tracking region has been defined).
		Vectors2Pyramid initialPointsPyramid_;

		/// The image points located in 'previousFramePyramid_'.
		Vectors2Pyramid previousPointsPyramid_;

		/// The orientation of the initial camera frame, (wRi), if known.
		Quaternion initialCameraOrientation_;

		/// The orientation of the camera matching with the previous camera frame, defined w.r.t. the world coordinate system (wTp).
		Quaternion previousCameraOrientation_;

		/// The homography transforming initial image points to current image points: currentImagePoint = globalHomography_ * initialImagePoint, (globalHomography_ = cHi).
		SquareMatrix3 globalHomography_;

		/// The orientation of the camera frame for the last valid homography 'globalHomography_' (wTl), if known.
		Quaternion globalCameraOrientation_;

		/// The tracking region (the region of interest) located on the plane, defined in the coordinate system of the initial frame.
		Box2 region_;

		/// The 3D plane on which all feature points will be located, if known.
		Plane3 plane_;

		/// The two key frames which will be used during re-initialization.
		KeyFrame keyFrames_[numberKeyFrames_];

		/// True, if the tracker needs to be re-initialized.
		bool needsReInitialization_ = false;
};

inline HomographyTracker::KeyFrame::KeyFrame() :
	timestamp_(false),
	globalHomography_(false),
	cameraOrientation_(false)
{
	// nothing to do here
}

inline HomographyTracker::KeyFrame::KeyFrame(const Timestamp& timestamp, const Vectors2Pyramid& initialPointsPyramid, const Vectors2Pyramid& pointsPyramid, const CV::FramePyramid& pyramid, const SquareMatrix3& globalHomography, const Quaternion& cameraOrientation) :
	timestamp_(timestamp),
	initialPointsPyramid_(initialPointsPyramid),
	pointsPyramid_(pointsPyramid),
	pyramid_(pyramid, true /*copyData*/),
	globalHomography_(globalHomography),
	cameraOrientation_(cameraOrientation)
{
	// nothing to do here
}

inline HomographyTracker::KeyFrame::operator bool() const
{
	ocean_assert(timestamp_.isValid() == pyramid_.isValid());

	return timestamp_.isValid();
}

inline HomographyTracker::HomographyTracker() :
	initialCameraOrientation_(false),
	previousCameraOrientation_(false),
	globalHomography_(false),
	globalCameraOrientation_(false),
	needsReInitialization_(false)
{
	// nothing to do here
}

inline void HomographyTracker::reset()
{
	currentFramePyramid_.clear();
	previousFramePyramid_.clear();

	initialPointsPyramid_.clear();
	previousPointsPyramid_.clear();

	initialCameraOrientation_ = Quaternion(false);

	previousCameraOrientation_ = Quaternion(false);

	globalHomography_.toNull();
	globalCameraOrientation_ = Quaternion(false);

	region_ = Box2();
	plane_ = Plane3();

	for (size_t n = 0; n < numberKeyFrames_; ++n)
	{
		keyFrames_[n] = KeyFrame();
	}

	needsReInitialization_ = false;
}

}

}

}

#endif // META_OCEAN_TRACKING_POINT_HOMOGRAPHY_TRACKER_H
