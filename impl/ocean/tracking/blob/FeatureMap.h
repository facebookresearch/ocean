/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_BLOB_FEATURE_MAP_H
#define META_OCEAN_TRACKING_BLOB_FEATURE_MAP_H

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/uvtexturemapping/ConeUVTextureMapping.h"
#include "ocean/tracking/uvtexturemapping/CylinderUVTextureMapping.h"
#include "ocean/tracking/uvtexturemapping/MeshUVTextureMapping.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/math/Box2.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Cone3.h"
#include "ocean/math/Cylinder3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Lookup2.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/SquareMatrix2.h"
#include "ocean/math/Triangle3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * This class implements a feature map for Blob features.
 * @ingroup trackingblob
 */
class OCEAN_TRACKING_BLOB_EXPORT FeatureMap
{
	public:
		/**
		 * Creates an empty feature map.
		 */
		FeatureMap();

		/**
		 * Creates a new feature map by given 3D object features.
		 * @param features Features to build the feature map
		 */
		FeatureMap(const CV::Detector::Blob::BlobFeatures& features);

		/**
		 * Creates a new feature map by a given image frame.
		 * The frame defines the x-z-plane and the y-axis stays perpendicular on the frame.<br>
		 * The coordinate origin will be defined at the top-left corner of the frame.
		 * @param frame The frame to create a feature map from, this frame will be converted internally if the pixel format is not FORMAT_Y8, must be valid
		 * @param dimension Width and (optional height) of the frame in reality in meter, if the height is zero it will be defined by the frame's aspect ratio
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param maxFeatures Number of maximal features to be stored (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 */
		FeatureMap(const Frame& frame, const Vector2& dimension, const Scalar threshold = Scalar(6.5), const bool frameIsUndistorted = true, const unsigned int maxFeatures = 0u, Worker* worker = nullptr);

		/**
		 * Creates a new feature map by a given image frame.
		 * The frame defines the x-z-plane and the y-axis stays perpendicular on the frame.<br>
		 * The coordinate origin will be defined at the top-left corner of the frame.
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param dimension Width and (optional height) of the frame in reality in meter, if the height is zero it will be defined by the frame's aspect ratio
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param maxFeatures Number of maximal features to be stored (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 */
		FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, const Scalar threshold = Scalar(6.5), const bool frameIsUndistorted = true, const unsigned int maxFeatures = 0u, Worker* worker = nullptr);

		/**
		 * Creates a 3D feature map based on an unwrapped cylinder.
		 * We unwrap the cylinder by making a straight cut down its side and flattening:
		 * \code
		 *
		 *          wrap-around for
		 *        /  edge features  \
		 *  <-  ->                   <-  ->
		 *        <- pattern width ->
		 *  ------A--------B--------C------                   __AC___
		 *  |     |                 |     |                 /        \
		 *  |     |                 |     |            ^   |          |
		 *  |     |                 |     |            |   \          /
		 *  |     |     cylinder    |     |            |   |\___B___/ |
		 *  |     |     surface     |     | <=> height |   |          |      .__X
		 *  |     |                 |     |            |   |  __DF___ |     /|
		 *  |     |                 |     |            |   |/        \|    Y Z
		 *  |     |                 |     |            |   |          |
		 *  |     |                 |     |            V   \          /
		 *  ------D--------E--------F------                 \___E___/
		 *        ^
		 *        pattern x-axis offset
		 *
		 * \endcode
		 * Cuts have been made at A=C and D=F. Since features on the edge require context
		 * across the cut, the pattern may be repeated on the left and right.
		 *
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixels
		 * @param height The height of the given frame in pixels
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param cylinderUVTextureMapping The mapping from the provided image into 3D coordinates
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param maxFeatures Optional maximal number of feature points to detect (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 * @see UVTextureMapping::CylinderUVTextureMapping
		 */
		FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::CylinderUVTextureMapping& cylinderUVTextureMapping, const Scalar threshold = Scalar(12), const unsigned int maxFeatures = 0u, Worker* worker = nullptr);

		/**
		 * Creates a 3D feature map based on an unwrapped cone. The cone may be a
		 * complete cone, or it may be truncated before the point. A complete cone
		 * has a point; for example, a paper ice cream cone is a complete cone.
		 * A truncated cone has horizontal slices at the larger and smaller
		 * diameters; for example, a coffee cup is a truncated cone with a larger
		 * diameter at the opening and a smaller diameter at the bottom.
		 *
		 * Consider the following (very poor) 3D representation of a truncated cone:
		 * \code
		 *
		 *    .------.      <-larger diameter
		 *   /        \
		 *  |          |           .__X
		 *  '.        .'          /|
		 *    \______/           Y Z
		 *
		 *      .--.        <- smaller diameter
		 *      \__/
		 *
		 *       .          <- tip of cone
		 *
		 * \endcode
		 * We choose the origin of the cone to be in the middle of the 3D solid,
		 * with the z-axis going along the axis of the cone and the y-axis
		 * intersecting through (for example) some central logo on the cone's
		 * surface.
		 *
		 * We unwrap the cone by making a straight cut down its side and flattening:
		 * \code
		 *
		 *  ---------------------                ^     .--AC--.      <- larger diameter
		 *  |                   |                |    /        \
		 *  |    _.---B---._    |        segment |   |          |           .__X
		 *  |  /             \  |         height |   '.        .'          /|
		 *  | A     cone      C |                |     \__B___/           Y Z
		 *  |  \   surface   /  |  <=>           |
		 *  |   \           /   |                v       .DF.        <- smaller diameter >= 0
		 *  |    \   .E.   /    |                        \E_/
		 *  |     D./   \.F     |
		 *  |                   |                         .          <- tip of cone
		 *  ---------------------
		 *
		 * \endcode
		 * Cuts have been made at A=C and D=F. The user should specify the points C and F,
		 * as well as the larger and smaller diameters and the height of the cone segment.
		 *
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixels
		 * @param height The height of the given frame in pixels
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param coneUVTextureMapping The mapping from the provided image into 3D coordinates
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param maxFeatures Optional maximal number of feature points to detect (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 * @see UVTextureMapping::ConeUVTextureMapping
		 */
		FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::ConeUVTextureMapping& coneUVTextureMapping, const Scalar threshold = Scalar(12), const unsigned int maxFeatures = 0u, Worker* worker = nullptr);

		/**
		 * Creates a 3D feature map based on a UV-mapped mesh.
		 * @param yFrame The 8 bit grayscale texture image (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixels
		 * @param height The height of the given frame in pixels
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param meshUVTextureMapping The mapping from the provided image (which is the texture for the mesh) into 3D coordinates
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param maxFeatures Optional maximal number of feature points to detect (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 * @see UVTextureMapping::MeshUVTextureMapping
		 */
		FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMapping, const Scalar threshold = Scalar(12), const unsigned int maxFeatures = 0u, Worker* worker = nullptr);

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
		 * Returns all object Blob features of this map.
		 * @return Map features
		 */
		inline const CV::Detector::Blob::BlobFeatures& features() const;

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
		 * Returns the cone of this feature map, which will be invalid if the feature map is not a cone.
		 * @return 3D cone upon which 3D points in this feature map lie
		 * @see isConical().
		 */
		inline const Cone3& cone() const;

		/**
		 * Returns the cylinder of this feature map, which will be invalid if the feature map is not a cylinder.
		 * @return 3D cylinder upon which 3D points in this feature map lie
		 * @see isCylindrical().
		 */
		inline const Cylinder3& cylinder() const;

		/**
		 * Returns the 3D triangles forming (or approximating) the underlying object for this feature map.
		 * @return Set of triangles, currently only valid if the shape is a cone
		 */
		inline const Triangles3& triangles3() const;

		/**
		 * For cylinder patterns, returns the associated 2D-to-3D texture mapping object for the pattern.
		 * @return Valid or invalid texture mapping, depending on whether the pattern is cylindrical or not
		 */
		inline const UVTextureMapping::CylinderUVTextureMapping& cylinderUVTextureMapping() const;

		/**
		 * For conical patterns, returns the associated 2D-to-3D texture mapping object for the pattern.
		 * @return Valid or invalid texture mapping, depending on whether the pattern is conical or not
		 */
		inline const UVTextureMapping::ConeUVTextureMapping& coneUVTextureMapping() const;

		/**
		 * For mesh patterns, returns the associated 2D-to-3D texture mapping object for the pattern.
		 * @return Valid or invalid texture mapping, depending on whether the pattern is a generic mesh or not
		 */
		inline const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMapping() const;

		/**
		 * For mesh patterns, returns the triangle index associated with each element in features().
		 * @return Vector of indices with the same length as the return value of features(), or an empty vector if the pattern is not a mesh
		 */
		inline const Indices32& triangleIndexPerFeature() const;

		/**
		 * Returns whether the feature map is planar.
		 * @return True, if so
		 */
		inline bool isPlanar() const;

		/**
		 * Returns whether the feature map represents points on a cylinder.
		 * @return True, if so
		 */
		inline bool isCylindrical() const;

		/**
		 * Returns whether the feature map represents points on a cone.
		 * @return True, if so
		 */
		inline bool isConical() const;

		/**
		 * Returns whether the feature map represents points on a generic mesh.
		 * @return True, if so
		 */
		inline bool isMesh() const;

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
		 * Creates features for a 3D feature map based on a cube map providing the six faces of cube to be tracked.
		 * The layout of the six faces is as follows:
		 * <pre>
		 *  -----------------------------
		 * |         |  back   |         |
		 *  -----------------------------
		 * |  left   |   top   |  right  |
		 *  -----------------------------
		 * |         |  front  |         |
		 *  -----------------------------
		 * |         | bottom  |         |
		 *  -----------------------------
		 * </pre>
		 * The width of the provided cube map image must be a multiple of three.<br>
		 * The height of the provided cube map image must be a multiple of four.<br>
		 * Further, width * 4 == height * 3 must hold.<br>
		 * The origin of the coordinate system of the cube will be in the center of the cube.<br>
		 * With x-axis pointing to the right (to the right face),
		 * with y-axis pointing upwards (to the top face),
		 * and with z-axis pointing to the front (to the front face):
		 * <pre>
		 *       ..........................
		 *      ..                       ..
		 *     . .                      . .
		 *    .  .      top            .  .
		 *   .   .                    .   .
		 *  .    .                   .    .
		 * ..........................     .
		 * .     .      Y           .     .
		 * .     .      ^           .     .   right
		 * .     .......|..................
		 * .    .       O ----> X   .    .
		 * .   .       /            .   .
		 * .  .       v             .  .
		 * . .       Z              . .
		 * ..                       ..
		 * ..........................
		 * </pre>
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the 3D feature points will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [3, infinity), with 'width' * 4 / 3 == 'height'
		 * @param height The height of the given frame in pixel, with range [4, infinity), with 'height' * 3 / 4 == 'width'
		 * @param yFramePaddingelements The number of padding elements at the end of each frame row, in element, with range [0, infinity)
		 * @param cubeSize The size of the cube in reality in meter (the edge size), with range (0, infinity)
		 * @param blobFeatures The resulting feature points located on the 3D cube
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param maxFeaturesPerFace Optional maximal number of feature points per cube face (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 * @return True, if succeeded
		 */
		static bool createCubeFeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingelements, const Scalar cubeSize, CV::Detector::Blob::BlobFeatures& blobFeatures, const Scalar threshold = Scalar(12), const unsigned int maxFeaturesPerFace = 0u, Worker* worker = nullptr);

	private:

		/**
		 * Creates a new feature map by a given image frame.
		 * The frame defines the x-z-plane and the y-axis stays perpendicular on the frame.<br>
		 * The coordinate origin will be defined at the top-left corner of the frame.
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param dimension Width and height of the frame in reality in meter, with range (0, infinity)x(0, infinity)
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param maxFeatures Number of maximal features to be stored (sorted by their strength), 0 to use all detected features
		 * @param worker Optional worker object to speed up the process
		 * @return True, if succeeded
		 */
		bool create(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, const Scalar threshold = Scalar(6.5), const bool frameIsUndistorted = true, const unsigned int maxFeatures = 0u, Worker* worker = nullptr);

		/**
		 * Determines 2D feature points in a given frame.
		 * The observations of the feature points are equivalent with the sub-pixel position within the provided frame.<br>
		 * Feature point do not receive a 3D position, feature points are !not! sorted w.r.t. their strength.
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param blobFeatures The resulting feature points located in the 2D frame
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param reusableIntegralFrame Memory buffer for the lined integral frame which will be used internally to avoid subsequent allocation and deallocation, must be valid and continuous
		 * @param worker Optional worker object to speed up the process
		 */
		static void determineAndDescribeFeatures(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, CV::Detector::Blob::BlobFeatures& blobFeatures, const Scalar threshold, const bool frameIsUndistorted, uint32_t* const reusableIntegralFrame, Worker* worker = nullptr);

		/**
		 * Determines 2D feature points in a given frame while the frame is rendered from a specified angle (around the x-axis and y-axis) (and not simply 'observed' from the top).
		 * The general purpose of this function is to determine feature points (and their descriptors) for a perspective view of the tracking object.<br>
		 * The observations of the feature points are equivalent with the sub-pixel position within the provided frame.<br>
		 * Feature point do not receive a 3D position, feature points are !not! sorted w.r.t. their strength.
		 * @param yAngle The angle of the rotation around the y-axis which is applied before the rotation around the x-axis is applied, should be usually 0, PI/2, PI, 3/4 PI, in radian, with range [0, 2PI)
		 * @param xAngle The angle of the rotation around the x-axis which is applied as second transformation, should be usually PI/4, in radian, with range [0, PI_2)
		 * @param dimension Width and (optional height) of the frame in reality in meter, if the height is zero it will be defined by the frame's aspect ratio
		 * @param yFrame The 8 bit grayscale frame (with pixel format Y8) from which the feature map will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param blobFeatures The resulting feature points located in the 2D frame (defined in the domain of the normal (not-warped) frame
		 * @param threshold Threshold of feature strength values to be accepted for the feature map
		 * @param resuableWarpedFrame Memory buffer for the warped frame (same size as the orginal frame), which will be used internally to avoid subsequent allocation and deallocation, must be valid and continuous
		 * @param reusableIntegralFrame Memory buffer for the lined integral frame which will be used internally to avoid subsequent allocation and deallocation, must be valid and continuous
		 * @param worker Optional worker object to speed up the process
		 * @return True, if succeeded
		 */
		static bool determineAndDescribeFeaturesForPerspective(const Scalar yAngle, const Scalar xAngle, const Vector2& dimension, const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, CV::Detector::Blob::BlobFeatures& blobFeatures, const Scalar threshold, uint8_t* resuableWarpedFrame, uint32_t* reusableIntegralFrame, Worker* worker = nullptr);

	private:

		/// All object Blob features corresponding to this map.
		CV::Detector::Blob::BlobFeatures mapFeatures;

		/// Bounding box of the feature map enclosing all object features.
		Box3 mapBoundingBox;

		/// Plane of the feature map if all feature points are planar.
		Plane3 mapPlane;

		/// Cone shape of the feature map if all feature points are conical.
		Cone3 mapCone;

		/// Cylinder shape of the feature map, if applicable.
		Cylinder3 mapCylinder;

		/// For cylinder-type patterns, retain a reference to the texture mapping.
		UVTextureMapping::CylinderUVTextureMapping cylinderUVTextureMapping_;

		/// For cone-type patterns, retain a reference to the texture mapping.
		UVTextureMapping::ConeUVTextureMapping coneUVTextureMapping_;

		/// For mesh-type patterns, retain a reference to the texture mapping.
		UVTextureMapping::MeshUVTextureMappingRef meshUVTextureMapping_;

		/// Triangles forming (or approximating) the underlying shape of this feature map.
		/// If the underlying object is a mesh, these triangles are instead stored in
		/// meshUVTextureMapping_->triangles3().
		Triangles3 triangles3_;

		/// If the underlying object is a mesh, stores the precomputed mesh triangle index associated
		/// with each feature in mapFeatures.
		Indices32 triangleIndexPerFeature_;
};

inline const CV::Detector::Blob::BlobFeatures& FeatureMap::features() const
{
	return mapFeatures;
}

inline const Box3& FeatureMap::boundingBox() const
{
	return mapBoundingBox;
}

inline bool FeatureMap::isNull() const
{
	return mapFeatures.empty();
}

inline const Plane3& FeatureMap::plane() const
{
	return mapPlane;
}

inline const Cone3& FeatureMap::cone() const
{
	return mapCone;
}

inline const Cylinder3& FeatureMap::cylinder() const
{
	return mapCylinder;
}

inline const Triangles3& FeatureMap::triangles3() const
{
	if (isMesh())
	{
		return meshUVTextureMapping_->triangles3();
	}

	return triangles3_;
}

inline const UVTextureMapping::CylinderUVTextureMapping& FeatureMap::cylinderUVTextureMapping() const
{
	return cylinderUVTextureMapping_;
}

inline const UVTextureMapping::ConeUVTextureMapping& FeatureMap::coneUVTextureMapping() const
{
	return coneUVTextureMapping_;
}

inline const UVTextureMapping::MeshUVTextureMappingRef& FeatureMap::meshUVTextureMapping() const
{
	return meshUVTextureMapping_;
}

inline const Indices32& FeatureMap::triangleIndexPerFeature() const
{
	ocean_assert(isMesh());
	ocean_assert(triangleIndexPerFeature_.size() == mapFeatures.size());
	return triangleIndexPerFeature_;
}

inline bool FeatureMap::isPlanar() const
{
	return bool(mapPlane);
}

inline bool FeatureMap::isCylindrical() const
{
	return mapCylinder.isValid();
}

inline bool FeatureMap::isConical() const
{
	return mapCone.isValid();
}

inline bool FeatureMap::isMesh() const
{
#ifdef OCEAN_DEBUG
	if (meshUVTextureMapping_)
	{
		ocean_assert(meshUVTextureMapping_->isValid());
	}
#endif // OCEAN_DEBUG
	return static_cast<bool>(meshUVTextureMapping_) && meshUVTextureMapping_->isValid();
}

inline FeatureMap::operator bool() const
{
	return !mapFeatures.empty();
}

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_FEATURE_MAP_H
