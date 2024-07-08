/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_ORB_FEATURE_MAP_H
#define META_OCEAN_TRACKING_ORB_FEATURE_MAP_H

#include "ocean/tracking/orb/ORB.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/ORBFeature.h"

#include "ocean/math/Box2.h"
#include "ocean/math/Box3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

/**
 * This class implements a feature map for ORB features.
 * @ingroup trackingorb
 */
class OCEAN_TRACKING_ORB_EXPORT FeatureMap
{
	public:

		/**
		 * Creates an empty feature map.
		 */
		FeatureMap();

		/**
		 * Creates a new feature map by a given image frame.
		 * The frame defines the x-z-plane and the y-axis stays perpendicular on the frame.<br>
		 * The coordinate origin will be defined at the top-left corner of the frame.
		 * @param frame The frame to create a feature map from
		 * @param dimension Width and (optional height) of the frame in reality in meter, if the height is zero it will be defined by the frame's aspect ratio
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param maxFeatures Number of maximal features to be stored (sorted by their strength), 0 to use all detected features
		 * @param useHarrisFeatures True, to use Harris corners; False, to use FAST features
		 * @param worker Optional worker object to speed up the process
		 */
		FeatureMap(const Frame& frame, const Vector2& dimension, const Scalar threshold = Scalar(6.5), const bool frameIsUndistorted = true, const unsigned int maxFeatures = 0u, const bool useHarrisFeatures = false, Worker* worker = nullptr);

		/**
		 * Projects the bounding box of the feature map into an image plane.
		 * @param pose The camera pose which is the extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera to be used representing the intrinsic camera parameter
		 * @return 2D bounding box of the projected 3D bounding box
		 */
		Box2 projectToImagePlane(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera);

		/**
		 * Projects the bounding box of the feature map into an image plane.
		 * The resulting 2D bounding box is increased by an extra pixel boundary.
		 * @param pose The camera pose which is the extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera to be used representing the intrinsic camera parameter
		 * @param boundary Extra boundary in pixel
		 * @param left Horizontal start position of the bounding box in pixel
		 * @param top Vertical start position of the bounding box in pixelResulting top position in pixel
		 * @param width Resulting width of the bounding box in pixel
		 * @param height Resulting height of the bounding box in pixel
		 * @param leftClamped Horizontal start position of the bounding box (clamped to the frame dimension)
		 * @param topClamped Horizontal start position of the bounding box (clamped to the frame dimension)
		 * @param widthClamped Horizontal start position of the bounding box (clamped to the frame dimension)
		 * @param heightClamped Horizontal start position of the bounding box (clamped to the frame dimension)
		 * @return True, if succeeded
		 */
		bool projectToImagePlane(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar boundary, int& left, int& top, unsigned int& width, unsigned int& height, unsigned int& leftClamped, unsigned int& topClamped, unsigned int& widthClamped, unsigned int& heightClamped);

		/**
		 * Projects the bounding box of the feature map into an image plane.
		 * @param iFlippedPose Inverted and flipped extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera to be used representing the intrinsic camera parameter
		 * @return 2D bounding box of the projected 3D bounding box
		 */
		Box2 projectToImagePlaneIF(const HomogenousMatrix4& iFlippedPose, const PinholeCamera& pinholeCamera);

		/**
		 * Projects the bounding box of the feature map into an image plane.
		 * The resulting 2D bounding box is increased by an extra pixel boundary and clamped to the frame dimension.
		 * @param iFlippedPose Inverted and flipped extrinsic camera matrix
		 * @param pinholeCamera The pinhole camera to be used representing the intrinsic camera parameter
		 * @param boundary Extra boundary in pixel
		 * @param left Horizontal start position of the bounding box in pixel
		 * @param top Vertical start position of the bounding box in pixelResulting top position in pixel
		 * @param width Resulting width of the bounding box in pixel
		 * @param height Resulting height of the bounding box in pixel
		 * @return True, if succeeded
		 */
		bool projectToImagePlaneIF(const HomogenousMatrix4& iFlippedPose, const PinholeCamera& pinholeCamera, const Scalar boundary, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height);

		/**
		 * Returns all features determined during creation of the feature map.
		 * @return Map features
		 */
		inline const CV::Detector::ORBFeatures& features() const;

		/**
		 * Returns the bounding box of this feature map.
		 * @return Bounding box
		 */
		inline const Box3& boundingBox() const;

		/**
		 * Returns the plane of this feature map.
		 * Beware: This plane may be invalid for e.g. 3D feature maps.
		 * @return Plane holding all planar features
		 * @see isPlanar().
		 */
		inline const Plane3& plane() const;

		/**
		 * Returns whether the feature map is planar.
		 * @return True, if so
		 */
		inline bool isPlanar() const;

		/**
		 * Returns whether the feature map is using Harris corners or FAST features.
		 * @return True, to use Harris corners; False, to use FAST features
		 */
		inline bool isUsingHarrisFeatures() const;

		/**
		 * Returns whether this map holds no features.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether this map holds at least one feature.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Returns features that were projected from the camera plane to the 3D plane of the feature map.
		 * Beware: the size of the features could be zero
		 * @return Projected features.
		 */
		inline const CV::Detector::ORBFeatures& projectedFeatures() const;

		/**
		 * Sets additional feature points that were projected from the camera plane to the 3D plane of the feature map.
		 * This will not override the features determined during creation of the map.
		 * @param features Features to set
		 */
		inline void setProjectedFeatures(CV::Detector::ORBFeatures& features);

	private:

		/// All object ORB features corresponding to this map.
		CV::Detector::ORBFeatures features_;

		/// Optional projected ORB features
		CV::Detector::ORBFeatures projectedFeatures_;

		/// Bounding box of the feature map enclosing all object features.
		Box3 boundingBox_;

		/// Plane of the feature map if all feature points are planar.
		Plane3 plane_;

		// True, to use Harris corners; False, to use FAST features.
		bool usingHarrisFeatures_;
};

inline const CV::Detector::ORBFeatures& FeatureMap::projectedFeatures() const
{
	return projectedFeatures_;
}

inline void FeatureMap::setProjectedFeatures(CV::Detector::ORBFeatures& features)
{
	projectedFeatures_ = features;
}

inline const CV::Detector::ORBFeatures& FeatureMap::features() const
{
	return features_;
}

inline const Box3& FeatureMap::boundingBox() const
{
	return boundingBox_;
}

inline bool FeatureMap::isNull() const
{
	return features_.empty();
}

inline const Plane3& FeatureMap::plane() const
{
	return plane_;
}

inline bool FeatureMap::isPlanar() const
{
	return plane_.isValid();
}

inline bool FeatureMap::isUsingHarrisFeatures() const
{
	return usingHarrisFeatures_;
}

inline FeatureMap::operator bool() const
{
	return !features_.empty();
}

}

}

}

#endif // META_OCEAN_TRACKING_ORB_FEATURE_MAP_H
