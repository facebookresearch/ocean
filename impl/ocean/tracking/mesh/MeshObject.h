// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_MESH_MESH_OBJECT_H
#define META_OCEAN_TRACKING_MESH_MESH_OBJECT_H

#include "ocean/tracking/mesh/Mesh.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"

#include "ocean/tracking/blob/Blob.h"
#include "ocean/tracking/blob/FeatureMap.h"

#include "ocean/tracking/mesh/MeshTriangle.h"

#include "ocean/tracking/uvtexturemapping/MeshUVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

/**
 * This class stores the information necessary for a single trackable mesh object.
 * @ingroup trackingmesh
 */
class OCEAN_TRACKING_MESH_EXPORT MeshObject
{
	public:

		/**
		 * Set of configurable parameters used when determining the pose of an object.
		 */
		struct MeshObjectTrackingOptions
		{
			/// Number of successfully tracked points required for an estimated pose to be counted as valid during tracking, with range [4, infinity).
			unsigned int minNumberTrackedPoints = 4u;

			/// Number of pyramid layers to use for frame-to-frame tracking.
			unsigned int numberPyramidLayersForTracking = 3u;

			/// The number of iterations to run RANSAC when attempting to verify a newly recognized object.
			unsigned int recognitionRansacIterations = 50u;

			/// Maximum amount of time in seconds to spend for rectification in a round-robin scheme among all triangles after a successful recognition.
			double recognitionMaxTimeAllowedForRectification = 0.006;

			/// Maximum amount of time in seconds to spend for rectification in a round-robin scheme among all triangles.
			double trackingMaxTimeAllowedForRectification = 0.002;
		};

	public:

		/**
		 * Creates a new invalid mesh object.
		 */
		inline MeshObject();

		/**
		 * Creates a new UV-mapped mesh tracking object.
		 * @param yTextureFrame The 8 bit grayscale frame (with Y8 pixel format, and pixel origin in the upper left corner) specifying the tracking texture, must be valid
		 * @param meshUVTextureMappingRef The mapping from the provided image into 3D coordinates
		 * @param worker Optional worker object to distribute the computation
		 * @see UVTextureMapping::MeshUVTextureMapping.
		 */
		MeshObject(const Frame& yTextureFrame, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, Worker* worker = nullptr);

		/**
		 * Returns the Blob feature map of this object.
		 * @return Blob feature map
		 */
		inline const Blob::FeatureMap& featureMap() const;

		/**
		 * Returns the frame pyramid of the texture image defining the tracking pattern.
		 * @return MeshObject Frame pyramid of the texture image
		 */
		inline const CV::FramePyramid& texturePyramid() const;

		/**
		 * Returns the previous camera pose from which this tracking pattern has been seen.
		 * @return Previous camera pose, if any
		 */
		inline const HomogenousMatrix4& pose_world_T_camera() const;

		/*
		 * Returns, for the most recently processed frame, the 2D subregion covered by the mesh when its triangles are projected into the image using the estimated image-to-object relative pose.
		 * @return Subregion covered by the projected mesh, if the pose estimate is valid
		 */
		inline const CV::SubRegion& projectedTrianglesSubRegion() const;

		/**
		 * Returns the 3D object points of this object which have been observed in the most recently processed frame.
		 * @return 3D object points lying on the object surface, each corresponding with one image point
		 * @see imagePoints().
		 */
		Vectors3 objectPoints() const;

		/**
		 * Returns the 2D image points of this object which have been observed in the most recently processed frame.
		 * @return 2D image points, each point corresponds with one object point
		 * @see objectPoints().
		 */
		Vectors2 imagePoints() const;

		/**
		 * Returns whether this object holds a valid/useful rough guess of the camera pose.
		 * @param poseGuess_world_T_camera The resulting rough pose guess, if existing
		 * @param maximalAge The maximal age of the rough pose guess in seconds, with range [0, 2]
		 * @return True, if so
		 */
		inline bool hasPoseGuess(HomogenousMatrix4& poseGuess_world_T_camera, const double maximalAge = 0.5) const;

		/**
		 * Returns a guess of the current camera pose for this object.
		 * @param timestamp Optional timestamp of the pose guess
		 * @return pose The camera pose guess, if any
		 */
		// TODO (jtprice): I think we can get rid of poseGuess_world_T_camera. Keeping for now.
		inline const HomogenousMatrix4& poseGuess_world_T_camera(Timestamp* timestamp = nullptr) const;

		/**
		 * Caches the current (or next) camera pose for this object as its "pose guess".
		 * @param timestamp The timestamp of this guess
		 */
		inline void updatePoseGuess(const Timestamp& timestamp);

		/*
		 * Cached version of projectedTrianglesSubRegion() for the most recently processed frame at which we successfully estimated a pose (if any). Returns the 2D subregion covered by the mesh when its triangles are projected into the last frame where we successfully estimated an image-to-object relative pose.
		 * @return Subregion covered by the projected mesh at the last frame for which we had a successful pose estimate, if any
		 */
		inline const CV::SubRegion& poseGuessProjectedTrianglesSubRegion() const;

		/**
		 * Resets the internal recognition states of this object while the actual feature map is untouched.
		 * @param keepPoseGuess If false, clear the pose guess for this object; otherwise, leave it unchanged
		 */
		void reset(bool keepPoseGuess = true);

		/**
		 * Attempts to determine the object pose from the previous pose estimate using frame-to-frame tracking and, if needed, an additional optimization via image-to-pattern rectification.
		 * @param options Structure defining algorithm parameters for determining object pose
		 * @param pinholeCamera The pinhole camera parameters defining the projection into the image
		 * @param previousFramePyramid Image pyramid for the last processed frame
		 * @param currentFramePyramid Image pyramid for the current frame
		 * @param quaternion_previousFrame_T_currentFrame Optional measurement of the rotation from the current frame into the previous frame
		 * @param worker Optional worker to use when performing the pose estimation
		 * @return True, if the pose could be successfully computed
		 */
		bool determinePose(const MeshObjectTrackingOptions& options, const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const Quaternion& quaternion_previousFrame_T_currentFrame, Worker* worker = nullptr);

		/**
		 * Attempts to determine the object pose solely from image features in the current frame, without relying on previous pose estimates.
		 * @param options Structure defining algorithm parameters for determining object pose
		 * @param pinholeCamera The pinhole camera parameters defining the projection into the image
		 * @param features Set of eligible Blob features for the current frame
		 * @param currentFramePyramid Image pyramid for the current frame
		 * @param worker Optional worker to use when performing the pose estimation
		 * @return True, if the pose could be successfully computed
		 */
		bool determinePoseFromFrameFeatures(const MeshObjectTrackingOptions& options, const PinholeCamera& pinholeCamera, const CV::Detector::Blob::BlobFeatures& features, const CV::FramePyramid& currentFramePyramid, Worker* worker = nullptr);

	protected:

		/**
		 * Attempts to refine the current pose estimate by warping the current image into the texture image and finding/refining the 2D point correspondences there.
		 * @param options Structure defining algorithm parameters for determining object pose
		 * @param pinholeCamera The pinhole camera parameters defining the projection into the image
		 * @param currentFramePyramid Image pyramid for the current frame
		 * @param runningRecognition If true, denotes that the optimization is running for a newly recognized object; if false, the object is being tracked from the previous frame
		 * @param worker Optional worker to use when performing the pose estimation
		 * @return True, unless an incorrect input is provided; the pose may or may not be actually updated
		 */
		bool optimizePoseByRectification(const MeshObjectTrackingOptions& options, const PinholeCamera& pinholeCamera, const CV::FramePyramid& currentFramePyramid, bool runningRecognition, Worker* worker);

	protected:

		/// The Blob feature map of this mesh object.
		Blob::FeatureMap featureMap_;

		/// The frame pyramid of the underlying texture image for the mesh.
		CV::FramePyramid texturePyramid_;

		/// Vector of all trackable mesh triangles. Each element stores reference points and tracked
		/// points associated with that triangle.
		MeshTriangles meshTriangles_;

		/// Set of indices in meshTriangles_ indicating which triangles currently have at least one tracked point.
		IndexSet32 trackedTriangleIndices_;

		/// Total number of tracked points across all mesh triangles.
		unsigned int totalNumberTrackedPoints_;

		/// The estimated camera pose for the tracked object in the most recently processed frame, if any.
		HomogenousMatrix4 pose_world_T_camera_;

		/// A rough guess of the camera pose for this object, if any.
		HomogenousMatrix4 poseGuess_world_T_camera_;

		/// Subregion covered by the set of visible triangles at the most recently processed frame.
		CV::SubRegion projectedTrianglesSubRegion_;

		/// Subregion covered by the most recent set of visible triangles available at poseGuess_world_T_camera_.
		CV::SubRegion poseGuessProjectedTrianglesSubRegion_;

		/// The timestamp of the rough camera pose.
		Timestamp poseGuessTimestamp_;

		/// The index of the last mesh triangle that was rectified in a round-robin scheduling scheme.
		unsigned int lastRectifiedTriangleIndex_;

		/// Random triangle ordering for the round-robin scheme for rectification.
		Indices32 triangleOrderingForRectification_;
};

inline MeshObject::MeshObject() :
	pose_world_T_camera_(false),
	poseGuess_world_T_camera_(false)
{
	// nothing to do here
}

inline const Blob::FeatureMap& MeshObject::featureMap() const
{
	return featureMap_;
}

inline const CV::FramePyramid& MeshObject::texturePyramid() const
{
	return texturePyramid_;
}

inline const HomogenousMatrix4& MeshObject::pose_world_T_camera() const
{
	return pose_world_T_camera_;
}

inline const CV::SubRegion& MeshObject::projectedTrianglesSubRegion() const
{
	return projectedTrianglesSubRegion_;
}

inline bool MeshObject::hasPoseGuess(HomogenousMatrix4& poseGuess_world_T_camera, const double maximalAge) const
{
	ocean_assert(maximalAge >= 0.0 && maximalAge <= 2.0);

	if (poseGuess_world_T_camera_.isValid() && NumericD::abs(double(Timestamp(true) - poseGuessTimestamp_)) <= maximalAge)
	{
		poseGuess_world_T_camera = poseGuess_world_T_camera_;
		return true;
	}

	return false;
}

inline const HomogenousMatrix4& MeshObject::poseGuess_world_T_camera(Timestamp* timestamp) const
{
	if (timestamp)
	{
		*timestamp = poseGuessTimestamp_;
	}

	return poseGuess_world_T_camera_;
}

inline void MeshObject::updatePoseGuess(const Timestamp& timestamp)
{
	poseGuess_world_T_camera_ = pose_world_T_camera_;
	poseGuessProjectedTrianglesSubRegion_ = projectedTrianglesSubRegion_;
	poseGuessTimestamp_ = timestamp;
}

inline const CV::SubRegion& MeshObject::poseGuessProjectedTrianglesSubRegion() const
{
	return poseGuessProjectedTrianglesSubRegion_;
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_MESH_OBJECT_H
