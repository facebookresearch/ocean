/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mesh/MeshObject.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Subset.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"
#include "ocean/tracking/mesh/FrameToFrameTracker.h"
#include "ocean/tracking/mesh/Utilities.h"

#include <algorithm>
#include <random>

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

// TODO (jtprice): fix or explain hardcoded values
MeshObject::MeshObject(const Frame& yTextureFrame, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, Worker* worker) :
	featureMap_(yTextureFrame.constdata<uint8_t>(), yTextureFrame.width(), yTextureFrame.height(), yTextureFrame.paddingElements(), meshUVTextureMappingRef, Scalar(6.5), 0u, worker),
	texturePyramid_(yTextureFrame, CV::FramePyramid::idealLayers(yTextureFrame.width(), yTextureFrame.height(), 15u, 15u), true /*copyFirstLayer*/, worker),
	totalNumberTrackedPoints_(0u),
	pose_world_T_camera_(false),
	poseGuess_world_T_camera_(false),
	lastRectifiedTriangleIndex_(0u)
{
	ocean_assert(featureMap_);

	if (!featureMap_)
	{
		return; // invalid mesh specification, or the texture mapping is invalid
	}

	const unsigned int numberMeshTriangles = (unsigned int)(meshUVTextureMappingRef->triangles3().size());
	meshTriangles_.reserve(numberMeshTriangles);

	for (unsigned int triangleIndex = 0u; triangleIndex < numberMeshTriangles; ++triangleIndex)
	{
		meshTriangles_.emplace_back(texturePyramid_.layers());
	}

	// TODO (jtprice): fix hardcoded values
	for (unsigned int pyramidLevel = 0u; pyramidLevel < texturePyramid_.layers(); ++pyramidLevel)
	{
		const Scalar scale_originalTextureFromDownsampledTexture = Scalar(texturePyramid_.sizeFactor(pyramidLevel));
		const Frame& scaledImage = texturePyramid_[pyramidLevel];

		const Vectors2 keypoints = CV::Detector::FeatureDetector::determineHarrisPoints(scaledImage.constdata<uint8_t>(), scaledImage.width(), scaledImage.height(), scaledImage.paddingElements(), CV::SubRegion(), 0u, 0u, 6u, worker);

		// Exclude points that aren't contained in a texture triangle.
		for (const Vector2& keypoint : keypoints)
		{
			Vector3 point3D; // unused
			Index32 triangleIndex;
			if (meshUVTextureMappingRef->textureCoordinateTo3DCoordinate(keypoint * scale_originalTextureFromDownsampledTexture, point3D, &triangleIndex))
			{
				ocean_assert(triangleIndex < meshTriangles_.size());
				meshTriangles_[triangleIndex].addTexturePyramidReferencePoint(pyramidLevel, keypoint);
			}
		}
	}

	// Remove reference points that are too close to each other in each pyramid layer.
	const Triangles2& uvTriangles2 = meshUVTextureMappingRef->triangles2();
	for (Index32 i = 0u; i < meshTriangles_.size(); ++i)
	{
		meshTriangles_[i].distributeAndFilterTexturePyramidReferencePoints(uvTriangles2[i]);
	}

	// Generate a random ordering of triangles for rectification updates.
	std::mt19937 randomGenerator((std::random_device())());
	triangleOrderingForRectification_.reserve(meshTriangles_.size());
	for (Index32 i = 0u; i < meshTriangles_.size(); ++i)
	{
		triangleOrderingForRectification_.push_back(i);
	}
	std::shuffle(triangleOrderingForRectification_.begin(), triangleOrderingForRectification_.end(), randomGenerator);
}

void MeshObject::reset(bool keepPoseGuess)
{
	pose_world_T_camera_.toNull();

	for (const Index32 triangleIndex : trackedTriangleIndices_)
	{
		meshTriangles_[triangleIndex].clearTrackedPoints(true); // resetPreviousPyramidLevel = true
	}

#ifdef OCEAN_DEBUG
	for (const MeshTriangle& meshTriangle : meshTriangles_)
	{
		ocean_assert(meshTriangle.numberTrackedPoints() == 0);
	}
#endif // OCEAN_DEBUG

	trackedTriangleIndices_.clear();

	totalNumberTrackedPoints_ = 0u;

	projectedTrianglesSubRegion_ = CV::SubRegion();

	if (!keepPoseGuess)
	{
		poseGuess_world_T_camera_.toNull();
		poseGuessTimestamp_.toInvalid();
	}
}

Vectors3 MeshObject::objectPoints() const
{
#ifdef OCEAN_DEBUG
	unsigned int currentTotalNumberPoints = 0u;
	for (const MeshTriangle& meshTriangle : meshTriangles_)
	{
		currentTotalNumberPoints += meshTriangle.numberTrackedPoints();
	}
	ocean_assert(totalNumberTrackedPoints_ == currentTotalNumberPoints);
#endif // OCEAN_DEBUG

	Vectors3 objectPoints;
	objectPoints.reserve(totalNumberTrackedPoints_);

	for (const Index32 triangleIndex : trackedTriangleIndices_)
	{
		const Vectors3& points = meshTriangles_[triangleIndex].objectPoints();
		objectPoints.insert(objectPoints.end(), points.cbegin(), points.cend());
	}

	ocean_assert(objectPoints.size() == totalNumberTrackedPoints_);

	return objectPoints;
}

Vectors2 MeshObject::imagePoints() const
{
#ifdef OCEAN_DEBUG
	unsigned int currentTotalNumberPoints = 0u;
	for (const MeshTriangle& meshTriangle : meshTriangles_)
	{
		currentTotalNumberPoints += meshTriangle.numberTrackedPoints();
	}
	ocean_assert(totalNumberTrackedPoints_ == currentTotalNumberPoints);
#endif // OCEAN_DEBUG

	Vectors2 imagePoints;
	imagePoints.reserve(totalNumberTrackedPoints_);

	for (const Index32 triangleIndex : trackedTriangleIndices_)
	{
		const Vectors2& points = meshTriangles_[triangleIndex].imagePoints();
		imagePoints.insert(imagePoints.end(), points.cbegin(), points.cend());
	}

	ocean_assert(imagePoints.size() == totalNumberTrackedPoints_);

	return imagePoints;
}

bool MeshObject::determinePose(const MeshObjectTrackingOptions& options, const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const Quaternion& quaternion_previousFrame_T_currentFrame, Worker* worker)
{
	// Check whether we have 3D object points (and corresponding 2D image points) from the previous
	// frame so that we can find the new image points in the current frame.

	constexpr size_t kMinimumNumberPreviouslyTrackedPoints = 16u;

	if (totalNumberTrackedPoints_ < kMinimumNumberPreviouslyTrackedPoints)
	{
		reset();
		return false;
	}

	ocean_assert(pose_world_T_camera_.isValid());

	// Try to determine the current pose by application of the previous correspondences.
	// If this fails, the new values will be invalid, but the pose guess will still be valid.
	HomogenousMatrix4 pose_world_T_currentFrame;
	Vectors3 currentObjectPoints;
	Vectors2 currentImagePoints;
	Indices32 validIndices;
	FrameToFrameTracker::track(pinholeCamera, previousFramePyramid, currentFramePyramid, quaternion_previousFrame_T_currentFrame, pose_world_T_camera_, objectPoints(), imagePoints(), pose_world_T_currentFrame, currentObjectPoints, currentImagePoints, validIndices, worker);

	ocean_assert(validIndices.size() <= totalNumberTrackedPoints_);
	ocean_assert(std::is_sorted(validIndices.begin(), validIndices.end()));

	if (validIndices.empty() || !pose_world_T_currentFrame.isValid())
	{
		reset();
		return false;
	}

	pose_world_T_camera_ = std::move(pose_world_T_currentFrame);

	// Distribute the flattened array of updated points back to their original triangles.

	Index32 startIndex = 0u; // current index among the flattened list of all previous points
	Index32 currentPointIndex = 0u; // current index among the flattened list of all current points

	IndexSet32::iterator triangleIndexIterator = trackedTriangleIndices_.begin();
	while (triangleIndexIterator != trackedTriangleIndices_.end())
	{
		const Index32 triangleIndex = *triangleIndexIterator;
		MeshTriangle& meshTriangle = meshTriangles_[triangleIndex];
		const unsigned int numberTrackedPoints = meshTriangle.numberTrackedPoints();

		ocean_assert(numberTrackedPoints <= totalNumberTrackedPoints_);

		const Index32 endIndex = startIndex + numberTrackedPoints; // (one greater than the index of the) last point associated with this triangle in the flattened list of all previous points

		Indices32 validCurrentIndicesForTriangle;
		validCurrentIndicesForTriangle.reserve(numberTrackedPoints);

		while (currentPointIndex < validIndices.size() && validIndices[currentPointIndex] < endIndex)
		{
			validCurrentIndicesForTriangle.push_back(currentPointIndex++);
		}

		startIndex = endIndex;

		ocean_assert(validCurrentIndicesForTriangle.size() <= numberTrackedPoints);

		if (validCurrentIndicesForTriangle.empty())
		{
			totalNumberTrackedPoints_ -= numberTrackedPoints;
			meshTriangle.clearTrackedPoints(true);

			triangleIndexIterator = trackedTriangleIndices_.erase(triangleIndexIterator);
		}
		else
		{
			ocean_assert(validCurrentIndicesForTriangle.size() <= numberTrackedPoints);
			const unsigned int numberInvalidPoints = numberTrackedPoints - (unsigned int)(validCurrentIndicesForTriangle.size());

			ocean_assert(numberInvalidPoints <= totalNumberTrackedPoints_);

			totalNumberTrackedPoints_ -= numberInvalidPoints;
			meshTriangle.setTrackedPoints(Subset::subset(currentObjectPoints, validCurrentIndicesForTriangle), Subset::subset(currentImagePoints, validCurrentIndicesForTriangle));

			++triangleIndexIterator;
		}
	}

	if (!optimizePoseByRectification(options, pinholeCamera, currentFramePyramid, false, worker))
	{
		return false;
	}

	ocean_assert(pose_world_T_camera_.isValid());
	ocean_assert(totalNumberTrackedPoints_ > 0u);

	return true;
}

bool MeshObject::determinePoseFromFrameFeatures(const MeshObjectTrackingOptions& options, const PinholeCamera& pinholeCamera, const CV::Detector::Blob::BlobFeatures& features, const CV::FramePyramid& currentFramePyramid, Worker* worker)
{
	// TODO (jtprice): explore whether there's a performance hit for not filtering the features based
	// on the pose guess

	// Apply a brute-force feature matching to determine candidates.
	// TODO (jtprice): hardcoded values
	Blob::Correspondences::CorrespondencePairs correspondenceCandidates(Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(features, featureMap_.features(), features.size(), Scalar(0.1), Scalar(0.7), worker));

	constexpr size_t kMinimumNumberInitialCorrespondenceCandidates = 12u;

	if (correspondenceCandidates.size() < kMinimumNumberInitialCorrespondenceCandidates)
	{
		return false;
	}

	Geometry::ImagePoints imagePoints;
	Geometry::ObjectPoints objectPoints;
	Blob::Correspondences::extractCorrespondingPoints(features, featureMap_.features(), correspondenceCandidates, imagePoints, objectPoints);
	ocean_assert(objectPoints.size() == imagePoints.size());

	HomogenousMatrix4 pose_world_T_camera;
	RandomGenerator randomGenerator; // TODO (jtprice): is it better to preallocate this?

	// Run P3P RANSAC to determine an initial pose.
	// TODO (jtprice): hardcoded values
	if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, pose_world_T_camera, 10u, true, options.recognitionRansacIterations, Scalar(15 * 15)))
	{
		return false;
	}

	// Apply another iteration of feature matching, now guided with the known pose. This often
	// significantly increases the number of feature correspondences.
	// TODO (jtprice): hardcoded values
	correspondenceCandidates = Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithPose(AnyCameraPinhole(pinholeCamera), pose_world_T_camera, features, featureMap_.features(), features.size(), Scalar(10), Scalar(0.1), Scalar(0.7));

	imagePoints.clear();
	objectPoints.clear();
	Blob::Correspondences::extractCorrespondingPoints(features, featureMap_.features(), correspondenceCandidates, imagePoints, objectPoints);
	ocean_assert(objectPoints.size() == imagePoints.size());

	// Run a second P3P to refine the pose using the guided matches.
	// TODO (jtprice): hardcoded values
	Indices32 resultingValidCorrespondences;
	if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, pose_world_T_camera, 10u, true, options.recognitionRansacIterations, Scalar(5 * 5), &resultingValidCorrespondences))
	{
		return false;
	}

	constexpr size_t kMinimumNumberRefinedCorrespondenceCandidates = 10u;

	if (resultingValidCorrespondences.size() < kMinimumNumberRefinedCorrespondenceCandidates)
	{
		return false;
	}

	ocean_assert(pose_world_T_camera.isValid());
	pose_world_T_camera_ = pose_world_T_camera;

	// Register the matched feature points with their associated mesh triangles.
	trackedTriangleIndices_.clear();
	totalNumberTrackedPoints_ = 0u;

#ifdef OCEAN_DEBUG
	for (const MeshTriangle& meshTriangle : meshTriangles_)
	{
		ocean_assert(meshTriangle.numberTrackedPoints() == 0);
	}
#endif

	const Indices32& triangleIndexPerFeature = featureMap_.triangleIndexPerFeature();

	for (const Index32 correspondenceIndex : resultingValidCorrespondences)
	{
		const Index32 featurePointIndex = correspondenceCandidates[correspondenceIndex].second;
		const Index32 triangleIndex = triangleIndexPerFeature[featurePointIndex];
		ocean_assert(triangleIndex < meshTriangles_.size());

		if (triangleIndex < meshTriangles_.size())
		{
			meshTriangles_[triangleIndex].addTrackedPoint(objectPoints[correspondenceIndex], imagePoints[correspondenceIndex]);
			++totalNumberTrackedPoints_;
			trackedTriangleIndices_.insert(triangleIndex);
		}
	}

	return optimizePoseByRectification(options, pinholeCamera, currentFramePyramid, true, worker);
}

bool MeshObject::optimizePoseByRectification(const MeshObjectTrackingOptions& options, const PinholeCamera& pinholeCamera, const CV::FramePyramid& currentFramePyramid, bool runningRecognition, Worker* worker)
{
	ocean_assert(texturePyramid_.layers() >= options.numberPyramidLayersForTracking);

	if (texturePyramid_.layers() < options.numberPyramidLayersForTracking)
	{
		return false;
	}

	ocean_assert(pose_world_T_camera_.isValid());
	ocean_assert(currentFramePyramid.layers() > 0u);

	const Frame& yFrame = currentFramePyramid.finestLayer();
	ocean_assert(yFrame.channels() == 1u);

	HomogenousMatrix4 pose_cameraFlipped_T_world(PinholeCamera::standard2InvertedFlipped(pose_world_T_camera_));

	// Go through all mesh triangles and apply rectification-based matching; if the triangle is
	// updated, add it to trackedTriangleIndices_.
	IndexSet32 updatedTriangleIndices; // subset of trackedTriangleIndices_ that have been updated

	const Timestamp startTimestamp(true);
	const unsigned int initialTriangleIndex = lastRectifiedTriangleIndex_; // to avoid duplicate work

	const double maximumTimeAllowedForRectification = runningRecognition ? options.recognitionMaxTimeAllowedForRectification : options.trackingMaxTimeAllowedForRectification;

	while (double(Timestamp(true) - startTimestamp) < maximumTimeAllowedForRectification && ++lastRectifiedTriangleIndex_ != initialTriangleIndex)
	{
		if (lastRectifiedTriangleIndex_ >= meshTriangles_.size())
		{
			lastRectifiedTriangleIndex_ = 0u;
		}

		ocean_assert(lastRectifiedTriangleIndex_ < triangleOrderingForRectification_.size());
		const Index32 triangleIndex = triangleOrderingForRectification_[lastRectifiedTriangleIndex_];
		MeshTriangle& meshTriangle = meshTriangles_[triangleIndex];

		const unsigned int previousNumberTrackedPoints = meshTriangle.numberTrackedPoints();
		ocean_assert(previousNumberTrackedPoints <= totalNumberTrackedPoints_);

		if (meshTriangle.rectifyTriangleAndIdentifyReferenceCorrespondences(pinholeCamera, yFrame, texturePyramid_, pose_cameraFlipped_T_world, featureMap_.meshUVTextureMapping(), triangleIndex, worker))
		{
			ocean_assert(meshTriangle.numberTrackedPoints() > 0);
			totalNumberTrackedPoints_ -= previousNumberTrackedPoints;
			totalNumberTrackedPoints_ += meshTriangle.numberTrackedPoints();

			trackedTriangleIndices_.insert(triangleIndex);
			updatedTriangleIndices.insert(triangleIndex);
		}
	}

	// If no triangles were updated, we don't need to perform any additional optimization.
	if (updatedTriangleIndices.empty())
	{
		return true;
	}

	// Optimize the given rough pose by application of the new 2D/3D correspondences.

	Scalar initError, finalError;
	// TODO (jtprice): hardcoded values
	if (totalNumberTrackedPoints_ < options.minNumberTrackedPoints ||
		!Geometry::NonLinearOptimizationPose::optimizePose(
			pinholeCamera,
			HomogenousMatrix4(pose_world_T_camera_),
			ConstArrayAccessor<Vector3>(objectPoints()),
			ConstArrayAccessor<Vector2>(imagePoints()),
			pinholeCamera.hasDistortionParameters(),
			pose_world_T_camera_,
			20u,
			Geometry::Estimator::ET_HUBER,
			Scalar(0.001),
			Scalar(5.0),
			&initError,
			&finalError))
	{
		reset();
		return false;
	}

	ocean_assert(!trackedTriangleIndices_.empty());

	return true;
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean
