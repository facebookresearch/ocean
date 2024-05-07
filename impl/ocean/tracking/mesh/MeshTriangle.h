// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_MESH_MESH_TRIANGLE_H
#define META_OCEAN_TRACKING_MESH_MESH_TRIANGLE_H

#include "ocean/tracking/mesh/Mesh.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/uvtexturemapping/MeshUVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

// Forward declaration.
class MeshTriangle;

/**
 * Definition of a vector holding MeshTriangle objects.
 * @see MeshTriangle
 * @ingroup trackingmesh
 */
typedef std::vector<MeshTriangle> MeshTriangles;

/**
 * This class stores the tracking information for a single triangle of a mesh.
 * @ingroup trackingmesh
 */
class OCEAN_TRACKING_MESH_EXPORT MeshTriangle
{
	public:

		/// Value that is used to indicate that a given triangle has not previously been matched for a
		/// given texture image pyramid level.
		static constexpr unsigned int kInvalidPyramidLevel = (unsigned int)(-1);

	public:

		/**
		 * Copy constructor.
		 * @param other Object to be copied
		 */
		MeshTriangle(const MeshTriangle& other) = default;

		/**
		 * Move constructor.
		 * @param other Object to move to this object
		 */
		MeshTriangle(MeshTriangle&& other) = default;

		/**
		 * Creates a new mesh triangle instance.
		 * @param numberPyramidLevels Number of pyramid levels to associate with the reference texture points for this triangle, with range (0, infinity)
		 */
		explicit MeshTriangle(const unsigned int numberPyramidLevels);

		/**
		 * Adds a reference point for this triangle at the specified pyramid level.
		 * Reference points are predetermined 2D locations in the texture space for this triangle and are used for efficient rectified alignment between the texture and the current image for a given triangle.
		 * @param pyramidLevel Texture-image pyramid level, with range [0, texturePyramidReferencePoints_.size())
		 * @param referencePoint 2D reference point to add, at the resolution of the specified pyramid level
		 */
		void addTexturePyramidReferencePoint(const Index32 pyramidLevel, const Vector2& referencePoint);

		/**
		 * Distributes the set of registered reference points for each pyramid layer.
		 * @param triangle2 Original 2D texture triangle in the texture at its finest resolution, expected to contain all registered reference points
		 */
		void distributeAndFilterTexturePyramidReferencePoints(const Triangle2& triangle2);

		/**
		 * Maps a live image frame into the texture space for this triangle based on an estimated 6DOF object pose, then identifies texture <-> live image correspondences for triangle's reference texture points.
		 * The alignment may be skipped if certain tracking criteria are not met, e.g., if the triangle is too small in the live image.
		 * If this function returns true, the internally managed set of object and image points will be updated to respectively contain the matched 3D reference features and their associated 2D counterparts; otherwise, these points will remain unchanged.
		 * @param pinholeCamera The pinhole camera associated with the live frame; must be valid
		 * @param yFrame Grayscale live frame; must be valid
		 * @param texturePyramid Image pyramid for the object texture, into which the live image will be mapped; must be valid
		 * @param pose_cameraFlipped_T_world Estimated object-to-camera relative pose that, with the known camera intrinsics, allows us to project the current 3D triangle into the image; must be valid
		 * @param meshUVTextureMappingRef Texture mapping object defining the 2D <-> 3D relationship between triangles in the texture space and 3D triangles of the mesh; must be valid
		 * @param triangleIndex Index of the triangle in the mesh, with range [0, meshUVTextureMappingRef->triangles3().size())
		 * @param worker Optional worker to use for the operation
		 * @return True if the alignment was determined to be necessary and was also successful; otherwise, false
		 */
		bool rectifyTriangleAndIdentifyReferenceCorrespondences(const PinholeCamera& pinholeCamera, const Frame& yFrame, const CV::FramePyramid& texturePyramid, const HomogenousMatrix4& pose_cameraFlipped_T_world, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, const Index32 triangleIndex, Worker* worker = nullptr);

		/**
		 * Unregisters all tracked 2D image points and 3D object points for this triangle.
		 * @param resetPreviousPyramidLevel If true, completely reset the tracking state by invalidating the previous pyramid level evaluated in rectifyTriangleAndIdentifyReferenceCorrespondences
		 */
		inline void clearTrackedPoints(bool resetPreviousPyramidLevel = false);

		/**
		 * Add a new 2D image points <-> 3D object point correspondence to this triangle.
		 * @param objectPoint 3D object point to add
		 * @param imagePoint 2D live-image points to add
		 */
		inline void addTrackedPoint(const Vector3& objectPoint, const Vector2& imagePoint);

		/**
		 * Update the set of 2D image points and 3D object points registered as being tracked for this triangle.
		 * @param objectPoints 3D object points associated that are now being tracked, with the same length as imagePoints
		 * @param imagePoints 2D live-image points associated that are now being tracked, with the same length as objectPoints
		 */
		inline void setTrackedPoints(Vectors3&& objectPoints, Vectors2&& imagePoints);

		/**
		 * Returns the current set of object points currently being tracked for this triangle.
		 * @return Tracked 3D object points
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the most recent set of tracked live-image points for this triangle.
		 * @return Tracked 2D live-image points
		 */
		inline const Vectors2& imagePoints() const;

		/**
		 * Returns the number of tracked points for this triangle.
		 */
		inline unsigned int numberTrackedPoints() const;

		/**
		 * Copy assignment operator.
		 * @param other Object to copy
		 * @return Reference to this object
		 */
		MeshTriangle& operator=(const MeshTriangle& other) = default;

		/**
		 * Move assignment operator.
		 * @param other Object to move
		 * @return Reference to this object
		 */
		MeshTriangle& operator=(MeshTriangle&& other) = default;

	private:

		/// For each pyramid level of the texture image, the set of initially extracted reference keypoints
		/// associated with this triangle.
		std::vector<Vectors2> texturePyramidReferencePoints_;

		/// The observed 3D object points associated with this triangle for the most recently processed frame.
		Vectors3 objectPoints_;

		/// The observed 2D image points associated with this triangle for the most recently processed frame.
		Vectors2 imagePoints_;

		/// When performing rectified matching for a given triangle, we compute the "best match" reference
		/// pyramid level in terms of scale. This vector keeps track of the most recently computed pyramid level.
		Index32 previousTexturePyramidLevel_;

		/// The last timestamp at which rectification was successfully performed.
		Timestamp lastRectificationTimestamp_;
};

inline void MeshTriangle::clearTrackedPoints(bool resetPreviousPyramidLevel)
{
	objectPoints_.clear();
	imagePoints_.clear();

	if (resetPreviousPyramidLevel)
	{
		previousTexturePyramidLevel_ = kInvalidPyramidLevel;
	}
}

inline void MeshTriangle::addTrackedPoint(const Vector3& objectPoint, const Vector2& imagePoint)
{
	objectPoints_.push_back(objectPoint);
	imagePoints_.push_back(imagePoint);
}

inline void MeshTriangle::setTrackedPoints(Vectors3&& objectPoints, Vectors2&& imagePoints)
{
	ocean_assert(objectPoints.size() == imagePoints.size());
	objectPoints_ = std::move(objectPoints);
	imagePoints_ = std::move(imagePoints);
}

inline const Vectors3& MeshTriangle::objectPoints() const
{
	return objectPoints_;
}

inline const Vectors2& MeshTriangle::imagePoints() const
{
	return imagePoints_;
}

inline unsigned int MeshTriangle::numberTrackedPoints() const
{
	ocean_assert(objectPoints_.size() == imagePoints_.size());

	return (unsigned int)(imagePoints_.size());
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_MESH_TRIANGLE_H
