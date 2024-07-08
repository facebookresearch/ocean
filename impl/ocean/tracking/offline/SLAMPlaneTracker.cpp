/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/SLAMPlaneTracker.h"
#include "ocean/tracking/offline/Frame2FrameTracker.h"

#include "ocean/base/Subset.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Box2.h"
#include "ocean/math/ExponentialMap.h"
#include "ocean/math/Pose.h"
#include "ocean/math/SuccessionSubset.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

SLAMPlaneTracker::HomographyTrackerComponent::HomographyTrackerComponent(SLAMPlaneTracker& parent, const PinholeCamera& pinholeCamera, Plane3& resultingPlane, const Scalar maxPlaneAngle, PinholeCamera* optimizedCamera) :
	FramePyramidTrackerComponent(parent),
	componentParent_(parent),
	componentCamera_(pinholeCamera),
	componentMaxPlaneAngle_(maxPlaneAngle),
	componentResultingPlane_(resultingPlane),
	componentResultingOptimizedCamera_(optimizedCamera),
	componentScopedEventLayer_(parent, Scalar(0.0), Scalar(0.5))
{
	// nothing to do here
}

bool SLAMPlaneTracker::HomographyTrackerComponent::onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStart(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	const size_t frames = upperFrameIndex - lowerFrameIndex + 1;

	componentImagePointsPairs_ = ImagePointsPairs(lowerFrameIndex, frames);
	componentHomographies_ = Homographies(lowerFrameIndex, frames, SquareMatrix3(false));
	componentNormalPairs_ = NormalPairs(lowerFrameIndex, frames, NormalPair(Vector3(0, 0, 0), Vector3(0, 0, 0)));

	return true;
}

SLAMPlaneTracker::FramePyramidTrackerComponent::IterationResult SLAMPlaneTracker::HomographyTrackerComponent::onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations)
{
	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	if (!componentInitialFramePyramid_)
	{
		ocean_assert(!previousFramePyramid_);

		ocean_assert(previousIndex == (unsigned int)(-1));
		ocean_assert(componentHomographies_.isValidIndex(currentIndex));
		componentHomographies_[currentIndex] = SquareMatrix3(true);

		componentInitialFramePyramid_ = CV::FramePyramid(currentFramePyramid_, true /*copyData*/);
	}
	else if (previousFramePyramid_)
	{
		ocean_assert(componentHomographies_.isValidIndex(previousIndex));
		ocean_assert(!componentHomographies_[previousIndex].isNull());

		// adjust the sub-region from the initial frame so that it matches with the previous frame
		const CV::SubRegion previousSubRegion(componentParent_.initialSubRegion_ * componentHomographies_[previousIndex]);

		// determine the homography between the previous and the current frame
		SquareMatrix3 relativeHomography;
		if (!frame2frameHomography(previousFramePyramid_, currentFramePyramid_, previousSubRegion, relativeHomography, scopedWorker()))
		{
			return IR_FAILED;
		}

		// calculate the concatenated homography that transforms the initial frame into the current frame
		SquareMatrix3 absoluteHomography = relativeHomography * componentHomographies_[previousIndex];

		Vectors2 initialImagePoints, currentImagePoints;

		// now optimized the concatenated homography
		for (unsigned int n = 0u; n < 2u; ++n)
		{
			SquareMatrix3 optimizedAbsoluteHomography;
			if (!optimizeHomography(componentInitialFramePyramid_, currentFramePyramid_.finestLayer(), componentParent_.initialSubRegion_, absoluteHomography, optimizedAbsoluteHomography, initialImagePoints, currentImagePoints, scopedWorker()))
			{
				// the tracking region could not been tracked from the previous frame to the current frame
				// thus, we expect the tracking region to be invisible from now on and we can finish here

				ocean_assert(!componentHomographies_[previousIndex].isNull());

				return IR_FINISHED;
			}

			absoluteHomography = optimizedAbsoluteHomography;
		}

		// now factorize the homography to extract two plane normal candidates
		HomogenousMatrix4 transformations[2];
		Vector3 normals[2] =
		{
			Vector3(0, 0, 0),
			Vector3(0, 0, 0)
		};

		if (!Geometry::Homography::factorizeHomographyMatrix(absoluteHomography, componentCamera_, componentCamera_, initialImagePoints.data(), currentImagePoints.data(), initialImagePoints.size(), transformations, normals))
		{
			ocean_assert(false && "This should never happen!");
			return IR_FAILED;
		}

		// store the information that has been found in this iteration
		componentImagePointsPairs_[currentIndex] = std::make_pair(std::move(initialImagePoints), std::move(currentImagePoints));
		componentNormalPairs_[currentIndex] = NormalPair(normals[0], normals[1]);
		componentHomographies_[currentIndex] = Geometry::Homography::normalizedHomography(absoluteHomography);
	}

	ocean_assert(maximalIterations != 0u);
	componentParent_.eventCallbacks_(AnalysisProgressEvent(componentParent_.id(), (iteration + 1u) * 100u / maximalIterations));

	return IR_SUCCEEDED;
}

bool SLAMPlaneTracker::HomographyTrackerComponent::onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	componentScopedEventLayer_.modify(Scalar(0.5), Scalar(1.0));

	// determine the two best matching plane normals
	NormalPair normalCandidates;
	if (!determinePlaneCandidates(normalCandidates, componentMaxPlaneAngle_, lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	{
		const AnalysisComponentEvent startEvent(componentParent_.id(), ComponentEvent::CS_STARTED);
		AnalysisComponentEvent breakEvent(componentParent_.id(), ComponentEvent::CS_FAILED);
		AnalysisComponentEvent finishedEvent(componentParent_.id(), ComponentEvent::CS_FINISHED);

		const Plane3 planeCandidates[2] = {Plane3(normalCandidates.first, -1), Plane3(normalCandidates.second, -1)};
		Plane3 uniquePlane;

		if (!ValidPlaneIdentifyComponent(componentParent_, componentCamera_, componentParent_.initialPose_, planeCandidates, componentParent_.initialSubRegion_, uniquePlane, Scalar(3 * 3)).invoke(startEvent, breakEvent, finishedEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex))
		{
			return false;
		}

		ocean_assert(uniquePlane.isValid());

		// now we have the initial plane that could be extracted in the previous frames
		componentResultingPlane_ = uniquePlane;
	}

	// **TODO** here or later?
	if (componentResultingOptimizedCamera_ && !componentParent_.shouldThreadStop() && !optimizeCamera(*componentResultingOptimizedCamera_, 20u))
	{
		// the optimization of the camera profile failed, however, than we use the normal profile
		*componentResultingOptimizedCamera_ = componentCamera_;
	}

	return true;
}

bool SLAMPlaneTracker::HomographyTrackerComponent::frame2frameHomography(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const CV::SubRegion& previousSubRegion, SquareMatrix3& homography, Worker* worker)
{
	ocean_assert(previousFramePyramid && currentFramePyramid);
	ocean_assert(previousFramePyramid.layers() == currentFramePyramid.layers());

	Vectors2 previousPoints, currentPoints;

	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, 2u, previousPoints, currentPoints, Scalar(0.9 * 0.9), previousSubRegion, 20u, 20u, 30u, worker, 3u) || previousPoints.size() < 45)
	{
		previousPoints.clear();
		currentPoints.clear();

		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, 2u, previousPoints, currentPoints, Scalar(1.5 * 1.5), previousSubRegion, 25u, 25u, 10u, worker, 3u) || previousPoints.size() < 25)
		{
			previousPoints.clear();
			currentPoints.clear();

			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, 2u, previousPoints, currentPoints, Scalar(1.9 * 1.9), previousSubRegion, 0u, 0u, 5u, worker, 3u) || previousPoints.size() < 25)
			{
				return false;
			}
		}
	}

	ocean_assert(previousPoints.size() == currentPoints.size());

	return Geometry::RANSAC::homographyMatrix(previousPoints.data(), currentPoints.data(), previousPoints.size(), componentRandomGenerator_, homography, 8u, true, 200u, Scalar(2.5 * 2.5), nullptr, worker);
}

bool SLAMPlaneTracker::HomographyTrackerComponent::optimizeHomography(const CV::FramePyramid& previousFramePyramid, const Frame& currentFrame, const CV::SubRegion& previousSubRegion, const SquareMatrix3& homography, SquareMatrix3& optimizedHomography, Vectors2& previousImagePoints, Vectors2& currentImagePoints, Worker* worker)
{
	ocean_assert(previousFramePyramid && currentFrame);

	// transform the current frame into the coordinate system of the previous frame
	if (!componentIntermediateHomographyFrame_.set(currentFrame.frameType(), true, true))
	{
		return false;
	}

	if (!CV::FrameInterpolatorBilinear::Comfort::homographyWithCamera(componentCamera_, componentCamera_, currentFrame, componentIntermediateHomographyFrame_, homography, componentCamera_.hasDistortionParameters(), nullptr, worker))
	{
		return false;
	}

	// create a frame pyramid of the transformed current frame
	if (!componentIntermediateFramePyramid_.replace8BitPerChannel11(componentIntermediateHomographyFrame_, previousFramePyramid.layers(), true /*copyFirstLayer*/, worker))
	{
		return false;
	}
	ocean_assert(previousFramePyramid.layers() == componentIntermediateFramePyramid_.layers());

	// track points from the previous frame to the (transformed) current frame, we should determine a tiny offset as the current frame matches almost with the previous frame
	previousImagePoints.clear();
	currentImagePoints.clear();
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, componentIntermediateFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), previousSubRegion, 20u, 20u, 30u, worker, 3u) || previousImagePoints.size() < 45)
	{
		previousImagePoints.clear();
		currentImagePoints.clear();
		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, componentIntermediateFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(1.5 * 1.5), previousSubRegion, 25u, 25u, 10u, worker, 3u) || previousImagePoints.size() < 25)
		{
			previousImagePoints.clear();
			currentImagePoints.clear();
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, componentIntermediateFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(1.9 * 1.9), previousSubRegion, 0u, 0u, 5u, worker, 3u) || previousImagePoints.size() < 25)
			{
				return false;
			}
		}
	}

	// now correct the current point positions due to the determined homography
	const SquareMatrix3 cameraFreeHomography(Geometry::Homography::normalizedHomography(componentCamera_.invertedIntrinsic() * homography * componentCamera_.intrinsic()));
	for (Vectors2::iterator i = currentImagePoints.begin(); i != currentImagePoints.end(); ++i)
	{
		const Vector2 normalizedImagePoint(componentCamera_.imagePoint2normalizedImagePoint<true>(*i, componentCamera_.hasDistortionParameters()));
		*i = componentCamera_.normalizedImagePoint2imagePoint<true>(cameraFreeHomography * normalizedImagePoint, componentCamera_.hasDistortionParameters());
	}

	ocean_assert(previousImagePoints.size() == currentImagePoints.size());

	// optimize the homography due to the new image points
	return Geometry::NonLinearOptimizationHomography::optimizeHomography(componentCamera_, homography, previousImagePoints.data(), currentImagePoints.data(), previousImagePoints.size(), optimizedHomography, 40u, Geometry::Estimator::ET_SQUARE);
}

bool SLAMPlaneTracker::HomographyTrackerComponent::determinePlaneCandidates(NormalPair& candidatePair, const Scalar maxAngle, const unsigned int lowerFrameIndex, const unsigned int /*initialFrameIndex*/, const unsigned int upperFrameIndex)
{
	unsigned int maximalInliers = 0u;
	size_t maximalIndex = size_t(-1);
	Scalar maxCosValue = 0;

	const Scalar minCosValue = Numeric::cos(maxAngle);

	for (size_t a = lowerFrameIndex; a <= upperFrameIndex; ++a)
	{
		const Vector3& normalFirst = componentNormalPairs_[a].first;
		const Vector3& normalSecond = componentNormalPairs_[a].second;

		// check whether the tracking area was invisible in frame 'a'
		if (normalFirst.isNull() || normalSecond.isNull())
		{
			ocean_assert(normalFirst.isNull() && normalSecond.isNull());
			continue;
		}

		unsigned int inlierFirst = 0u;
		unsigned int inlierSecond = 0u;

		Scalar cosValueFirst = 0;
		Scalar cosValueSecond = 0;

		// **TODO** check whether b can start at a + 1
		for (size_t b = lowerFrameIndex; b <= upperFrameIndex; ++b)
		{
			if (a != b)
			{
				const Vector3& candidateFirst = componentNormalPairs_[b].first;
				const Vector3& candidateSecond = componentNormalPairs_[b].second;

				// check whether the tracking area was invisible in frame 'b'
				if (candidateFirst.isNull() || candidateSecond.isNull())
				{
					ocean_assert(candidateFirst.isNull() && candidateSecond.isNull());
					continue;
				}

				if (normalFirst * candidateFirst >= minCosValue)
				{
					cosValueFirst += normalFirst * candidateFirst;
					inlierFirst++;
				}
				else if (normalFirst * candidateSecond >= minCosValue)
				{
					cosValueFirst += normalFirst * candidateSecond;
					inlierFirst++;
				}

				if (normalSecond * candidateFirst >= minCosValue)
				{
					cosValueSecond += normalSecond * candidateFirst;
					inlierSecond++;
				}
				else if (normalSecond * candidateSecond >= minCosValue)
				{
					cosValueSecond += normalSecond * candidateSecond;
					inlierSecond++;
				}
			}
		}

		if (inlierFirst > maximalInliers || (inlierFirst == maximalInliers && cosValueFirst > maxCosValue))
		{
			maxCosValue = cosValueFirst;
			maximalInliers = inlierFirst;
			maximalIndex = a;
		}

		if (inlierSecond > maximalInliers || (inlierSecond == maximalInliers && cosValueSecond > maxCosValue))
		{
			maxCosValue = cosValueSecond;
			maximalInliers = inlierSecond;
			maximalIndex = a;
		}
	}

	if (maximalIndex == (size_t)-1)
	{
		return false;
	}

	// now determine the averaged best normals
	const Vector3& normalFirst = componentNormalPairs_[maximalIndex].first;
	const Vector3& normalSecond = componentNormalPairs_[maximalIndex].second;

	Vector3 sumVectorFirst(0, 0, 0);
	Vector3 sumVectorSecond(0, 0, 0);

	for (size_t b = lowerFrameIndex; b <= upperFrameIndex; ++b)
	{
		const Vector3& candidateFirst = componentNormalPairs_[b].first;
		const Vector3& candidateSecond = componentNormalPairs_[b].second;

		if (normalFirst * candidateFirst >= minCosValue)
		{
			sumVectorFirst += candidateFirst;
		}
		else if (normalFirst * candidateSecond >= minCosValue)
		{
			sumVectorFirst += candidateSecond;
		}

		if (normalSecond * candidateFirst >= minCosValue)
		{
			sumVectorSecond += candidateFirst;
		}
		else if (normalSecond * candidateSecond >= minCosValue)
		{
			sumVectorSecond += candidateSecond;
		}
	}

	candidatePair = NormalPair(sumVectorFirst.normalized(), sumVectorSecond.normalized());

	return true;
}

bool SLAMPlaneTracker::HomographyTrackerComponent::optimizeCamera(PinholeCamera& optimizedCamera, const unsigned int numberFrames)
{
	ocean_assert(numberFrames >= 10u);
	ocean_assert(componentCamera_.isValid());

	if (componentHomographies_.isEmpty())
	{
		return false;
	}

	ocean_assert(componentImagePointsPairs_.size() == componentHomographies_.size());

	Indices32 poseMapping;
	poseMapping.reserve(componentHomographies_.size());

	// find the threshold for feature point correspondences
	Indices32 pointCorrespondences;
	pointCorrespondences.reserve(componentHomographies_.size());

	for (OfflinePoses::Index i = componentHomographies_.firstIndex(); i < componentHomographies_.endIndex(); ++i)
	{
		if (!componentHomographies_[i].isNull())
		{
			pointCorrespondences.push_back((unsigned int)componentImagePointsPairs_[i].first.size());
		}
	}

	const unsigned int qualityThreshold = Median::median(pointCorrespondences.data(), pointCorrespondences.size());

	// find a subset of all homographies best representing the entire sequence
	SquareMatrices3 homographies;
	homographies.reserve(componentHomographies_.size());

	for (OfflinePoses::Index i = componentHomographies_.firstIndex(); i < componentHomographies_.endIndex(); ++i)
	{
		if (!componentHomographies_[i].isNull() && componentImagePointsPairs_[i].first.size() >= qualityThreshold)
		{
			// homographies is not a shift vector so we have to adjust the index
			homographies.push_back(componentHomographies_[i]);
			poseMapping.push_back((unsigned int)i);
		}
	}

	ocean_assert(sizeof(SuccessionSubset<Scalar, 9>::Object) == sizeof(SquareMatrix3));
	SuccessionSubset<Scalar, 9> successionSubset((SuccessionSubset<Scalar, 9>::Object*)homographies.data(), homographies.size());

	Geometry::NonLinearOptimizationPlane::ImagePointsPairs bestImagePointsPairs;
	SquareMatrices3 bestHomographies;

	bestImagePointsPairs.reserve(numberFrames);
	bestHomographies.reserve(numberFrames);

	while (bestImagePointsPairs.size() < numberFrames)
	{
		// find the best next pose
		const size_t indexZeroBased = successionSubset.incrementSubset();
		if (indexZeroBased == size_t(-1))
		{
			break;
		}

		// the subset index might not be identical with the frame index of the tracking sequence
		const unsigned int frameIndex = poseMapping[indexZeroBased];

		// check whether the pose of the frame is invalid
		ocean_assert(!componentHomographies_[frameIndex].isNull());

		bestHomographies.push_back(componentHomographies_[frameIndex]);
		bestImagePointsPairs.push_back(componentImagePointsPairs_[frameIndex]);
	}

	ocean_assert(bestHomographies.size() == bestImagePointsPairs.size());

	if (bestHomographies.size() < 10)
	{
		return false;
	}

	SquareMatrices3 optimizedSubsetHomographies;

	// first we try to optimized the entire camera model including, focal length, principal point and camera distortion
	if (!Geometry::NonLinearOptimizationHomography::optimizeCameraHomographies(componentCamera_, bestHomographies, bestImagePointsPairs, optimizedCamera, optimizedSubsetHomographies, 100u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5)))
	{
		return false;
	}

	// check whether the optimized camera is plausible
	if (isPlausibleCamera(optimizedCamera, CM_MEDIUM_QUALITY)) // **TODO** CM_MEDIUM_QUALITY
	{
		return true;
	}

	// now we try to optimize the distortion parameters only
	if (!Geometry::NonLinearOptimizationHomography::optimizeDistortionCameraHomographies(componentCamera_, bestHomographies, bestImagePointsPairs, optimizedCamera, optimizedSubsetHomographies, 100u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5)))
	{
		return false;
	}

	// check whether the optimized camera is plausible
	return isPlausibleCamera(optimizedCamera, CM_MEDIUM_QUALITY);
}

SLAMPlaneTracker::ValidPlaneIdentifyComponent::ValidPlaneIdentifyComponent(SLAMPlaneTracker& parent, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Plane3 planeCandidates[2], const CV::SubRegion& initialSubRegion, Plane3& resultingPlane, const Scalar maximalSqrDistance) :
	FramePyramidTrackerComponent(parent),
	componentParent_(parent),
	componentCamera_(pinholeCamera),
	componentInitialPose_(initialPose),
	componentInitialSubRegion_(initialSubRegion),
	componentMaximalSqrDistance_(maximalSqrDistance),
	componentResultingPlane_(resultingPlane)
{
	componentPlaneCandidates_[0] = planeCandidates[0];
	componentPlaneCandidates_[1] = planeCandidates[1];

	ocean_assert(componentPlaneCandidates_[0].isValid());
	ocean_assert(componentPlaneCandidates_[1].isValid());

	componentPreviousPoses_[0] = componentInitialPose_;
	componentPreviousPoses_[1] = componentInitialPose_;

	componentValidCorrespondences_[0] = 0u;
	componentValidCorrespondences_[1] = 0u;
}

bool SLAMPlaneTracker::ValidPlaneIdentifyComponent::onStartDecreasing()
{
	if (!FramePyramidTrackerComponent::onStartDecreasing())
	{
		return false;
	}

	ocean_assert(!componentInitialImagePoints_.empty());

	// we assign the initial image points to the previous points of this component as they will be swapped before the are used for the first time
	componentPreviousImagePoints_ = componentInitialImagePoints_;
	componentCurrentImagePoints_.clear();

	componentObjectPoints_[0] = componentInitialObjectPoints_[0];
	componentObjectPoints_[1] = componentInitialObjectPoints_[1];

	return true;
}

SLAMPlaneTracker::FramePyramidTrackerComponent::IterationResult SLAMPlaneTracker::ValidPlaneIdentifyComponent::onFrame(const unsigned int /*previousIndex*/, const unsigned int /*currentIndex*/, const unsigned int iteration, const unsigned int maximalIterations)
{
	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	if (iteration == 0u)
	{
		ocean_assert(componentPreviousImagePoints_.empty());
		ocean_assert(componentInitialImagePoints_.empty());
		ocean_assert(componentObjectPoints_[0].empty() && componentObjectPoints_[1].empty());

		const Box2 boundingBox(componentInitialSubRegion_.boundingBox());
		componentInitialImagePoints_ = CV::Detector::FeatureDetector::determineHarrisPoints(currentFramePyramid_.finestLayer(), componentInitialSubRegion_, 0u, 0u, 20u, scopedWorker());
		componentInitialImagePoints_ = Geometry::SpatialDistribution::distributeAndFilter(componentInitialImagePoints_.data(), componentInitialImagePoints_.size(), boundingBox.left(), boundingBox.top(), boundingBox.width(), boundingBox.height(), 20u, 20u);

		for (unsigned int p = 0u; p < 2u; ++p)
		{
			componentObjectPoints_[p] = Geometry::Utilities::backProjectImagePoints(componentCamera_, componentInitialPose_, componentPlaneCandidates_[p], componentInitialImagePoints_.data(), componentInitialImagePoints_.size(), componentCamera_.hasDistortionParameters());
			componentInitialObjectPoints_[p] = componentObjectPoints_[p];
		}

		// we assign the initial image points to the previous points of this component as they will be swapped before the are used for the first time
		componentPreviousImagePoints_ = componentInitialImagePoints_;
	}

	// swap the image points and the frame pyramid from the previous iteration
	std::swap(componentPreviousImagePoints_, componentCurrentImagePoints_);

	if (previousFramePyramid_)
	{
		// clear the current image points as they will be filled in the bidirectional point tracking function
		componentCurrentImagePoints_.clear();

		Indices32 validIndices;
		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid_, currentFramePyramid_, 2u, componentPreviousImagePoints_, componentCurrentImagePoints_, Scalar(0.9 * 0.9), scopedWorker(), &validIndices))
		{
			return IR_FAILED;
		}

		if (validIndices.size() < 25)
		{
			// the tracking region could not been tracked from the previous frame to the current frame
			// thus, we expect the tracking region to be invisible from now on and we can finish here

			return IR_FINISHED;
		}

		// reduce the current image points (the previous points are not used anymore)
		if (validIndices.size() != componentPreviousImagePoints_.size())
		{
			componentCurrentImagePoints_ = Subset::subset(componentCurrentImagePoints_, validIndices);

			for (unsigned int p = 0u; p < 2u; ++p)
			{
				componentObjectPoints_[p] = Subset::subset(componentObjectPoints_[p], validIndices);
			}
		}

		for (unsigned int p = 0u; p < 2u; ++p)
		{
			ocean_assert(componentPreviousPoses_[p].isValid());

			HomogenousMatrix4 optimizedPose;
			if (!Geometry::NonLinearOptimizationPose::optimizePose(componentCamera_, componentPreviousPoses_[p], ConstArrayAccessor<Vector3>(componentObjectPoints_[p]), ConstArrayAccessor<Vector2>(componentCurrentImagePoints_), componentCamera_.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5)))
			{
				ocean_assert(false && "This should never happen!");
				return IR_FAILED;
			}

			componentPreviousPoses_[p] = optimizedPose;

			Vectors2 projectedObjectPoints(componentObjectPoints_[p].size());
			componentCamera_.projectToImage<true>(optimizedPose, componentObjectPoints_[p].data(), componentObjectPoints_[p].size(), componentCamera_.hasDistortionParameters(), projectedObjectPoints.data());

			for (unsigned int n = 0u; n < projectedObjectPoints.size(); ++n)
			{
				if (projectedObjectPoints[n].sqrDistance(componentCurrentImagePoints_[n]) <= componentMaximalSqrDistance_)
				{
					componentValidCorrespondences_[p]++;
				}
			}
		}
	}

	// check whether the result is almost clear and we can break here
	if (iteration > 50u && ((componentValidCorrespondences_[0] > componentValidCorrespondences_[1] && componentValidCorrespondences_[0] * 80u / 100u > componentValidCorrespondences_[1])
			|| (componentValidCorrespondences_[1] > componentValidCorrespondences_[0] && componentValidCorrespondences_[1] * 80u / 100u > componentValidCorrespondences_[0])))
	{
		return IR_FINISHED;
	}

	componentParent_.eventCallbacks_(AnalysisProgressEvent(componentParent_.id(), (iteration + 1u) * 100u / maximalIterations));
	return IR_SUCCEEDED;
}

bool SLAMPlaneTracker::ValidPlaneIdentifyComponent::onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	// check whether there is a unique difference the two planes
	if (componentValidCorrespondences_[0] > componentValidCorrespondences_[1])
	{
		if (componentValidCorrespondences_[0] * 80u / 100u > componentValidCorrespondences_[1])
		{
			componentResultingPlane_ = componentPlaneCandidates_[0];
			return true;
		}
	}
	else
	{
		if (componentValidCorrespondences_[1] * 80u / 100u > componentValidCorrespondences_[0])
		{
			componentResultingPlane_ = componentPlaneCandidates_[1];
			return true;
		}
	}

	// if there is not a unique difference, than we take the plane more perpendicular to the viewing direction
	const Vector3 viewingDirection(componentInitialPose_.rotationMatrix(Vector3(0, 0, 1)));

	if (componentPlaneCandidates_[0].normal() * viewingDirection < componentPlaneCandidates_[1].normal() * viewingDirection)
	{
		componentResultingPlane_ = componentPlaneCandidates_[1];
		return true;
	}

	componentResultingPlane_ = componentPlaneCandidates_[0];
	return true;
}

SLAMPlaneTracker::PlaneTrackerComponent::PlaneTrackerComponent(SLAMPlaneTracker& parent, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& initialPose, const Plane3& plane, const bool createStateEvents, OfflinePoses* poses, Plane3* optimizedPlane) :
	FramePyramidTrackerComponent(parent),
	componentParent_(parent),
	componentCamera_(pinholeCamera),
	componentInitialPose_(initialPose),
	componentPlane_(plane),
	componentCreateStateEvents_(createStateEvents),
	componentResultingPlane_(optimizedPlane),
	componentResultingPoses_(poses)
{
	// nothing to do here
}

bool SLAMPlaneTracker::PlaneTrackerComponent::onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStart(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	const size_t frames = upperFrameIndex - lowerFrameIndex + 1;

	componentImagePointsPairs_ = ImagePointsPairs(lowerFrameIndex, frames);
	componentHomographies_ = Homographies(lowerFrameIndex, frames, SquareMatrix3(false));
	componentPoses_ = OfflinePoses(lowerFrameIndex, frames, OfflinePose());

	return true;
}

SLAMPlaneTracker::FramePyramidTrackerComponent::IterationResult SLAMPlaneTracker::PlaneTrackerComponent::onFrame(const unsigned int previousIndex, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations)
{
	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	if (!componentInitialFramePyramid_)
	{
		ocean_assert(previousIndex == (unsigned int)(-1));
		ocean_assert(componentHomographies_.isValidIndex(currentIndex));
		componentHomographies_[currentIndex] = SquareMatrix3(true);
		componentPoses_[currentIndex] = OfflinePose(currentIndex, componentInitialPose_);

		if (componentCreateStateEvents_)
		{
			componentParent_.eventCallbacks_(TrackerPoseStateEvent(componentParent_.id(), componentPoses_[currentIndex]));
		}

		componentInitialFramePyramid_ = CV::FramePyramid(currentFramePyramid_, true /*copyData*/);
	}
	else if (previousFramePyramid_)
	{
		ocean_assert(componentHomographies_.isValidIndex(previousIndex));
		ocean_assert(!componentHomographies_[previousIndex].isNull());

		ocean_assert(componentHomographies_.size() == componentPoses_.size());

		// adjust the sub-region from the initial frame so that it matches with the previous frame
		const CV::SubRegion previousSubRegion(componentParent_.initialSubRegion_ * componentHomographies_[previousIndex]);

		Vectors2 previousImagePoints, currentImagePoints;
		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid_, currentFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(0.9 * 0.9), previousSubRegion, 20u, 20u, 30u, scopedWorker()) || previousImagePoints.size() < 45)
		{
			previousImagePoints.clear();
			currentImagePoints.clear();

			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid_, currentFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(1.5 * 1.5), previousSubRegion, 25u, 25u, 10u, scopedWorker()) || previousImagePoints.size() < 25)
			{
				previousImagePoints.clear();
				currentImagePoints.clear();

				if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid_, currentFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(1.9 * 1.9), previousSubRegion, 0u, 0u, 5u, scopedWorker()) || previousImagePoints.size() < 25)
				{
					// the tracking region could not been tracked from the previous frame to the current frame
					// thus, we expect the tracking region to be invisible from now on and we can finish here

					ocean_assert(!componentPoses_[currentIndex].isValid());

					return IR_FINISHED;
				}
			}
		}

		const HomogenousMatrix4& previousPose = componentPoses_[previousIndex].transformation();
		ocean_assert(previousPose.isValid());

		HomogenousMatrix4 currentPose;
		if (!Frame2FrameTracker::trackPlanarObject(componentCamera_, previousPose, componentPlane_, previousImagePoints.data(), currentImagePoints.data(), previousImagePoints.size(), currentPose))
		{
			// the tracking region could not been tracked from the previous frame to the current frame
			// thus, we expect the tracking region to be invisible from now on and we can finish here

			ocean_assert(!componentPoses_[currentIndex].isValid());

			return IR_FINISHED;
		}

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			HomogenousMatrix4 optimizedPose;
			if (!optimizePose(componentInitialFramePyramid_, currentFramePyramid_.finestLayer(), componentParent_.initialSubRegion_, componentInitialPose_, currentPose, optimizedPose, previousImagePoints, currentImagePoints, scopedWorker()))
			{
				// the tracking region could not been tracked from the previous frame to the current frame
				// thus, we expect the tracking region to be invisible from now on and we can finish here

				ocean_assert(!componentPoses_[currentIndex].isValid());

				return IR_FINISHED;
			}

			const ExponentialMap orientationOffset(currentPose.rotation().inverted() * optimizedPose.rotation());

			currentPose = optimizedPose;

			// check whether the optimized pose is almost identical to the current pose
			if (orientationOffset.angle() < Numeric::weakEps())
			{
				break;
			}
		}

		const SquareMatrix3 currentHomography(Geometry::Homography::homographyMatrix(componentInitialPose_, currentPose, componentCamera_, componentCamera_, componentPlane_));

		// store the information that has been found in this iteration
		componentHomographies_[currentIndex] = currentHomography;
		componentPoses_[currentIndex] = OfflinePose(currentIndex, currentPose);
		componentImagePointsPairs_[currentIndex] = std::make_pair(std::move(previousImagePoints), std::move(currentImagePoints));

		if (componentCreateStateEvents_)
		{
			componentParent_.eventCallbacks_(TrackerPoseStateEvent(componentParent_.id(), componentPoses_[currentIndex]));
		}
	}

	componentParent_.eventCallbacks_(TrackingProgressEvent(componentParent_.id(), (iteration + 1u) * 100u / maximalIterations));
	return IR_SUCCEEDED;
}

bool SLAMPlaneTracker::PlaneTrackerComponent::onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FramePyramidTrackerComponent::onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	ocean_assert(componentImagePointsPairs_.size() == componentPoses_.size());

	if (componentResultingPoses_)
	{
		*componentResultingPoses_ = componentPoses_;
	}

	if (componentResultingPlane_ && !componentParent_.shouldThreadStop() && !optimizePlane(*componentResultingPlane_))
	{
		// the optimization of the plane failed, however, than we use the standard plane
		*componentResultingPlane_ = componentPlane_;
	}

	return true;
}

bool SLAMPlaneTracker::PlaneTrackerComponent::optimizePose(const CV::FramePyramid& previousFramePyramid, const Frame& currentFrame, const CV::SubRegion& previousSubRegion, const HomogenousMatrix4& previousPose, const HomogenousMatrix4& currentPose, HomogenousMatrix4& optimizedPose, Vectors2& previousImagePoints, Vectors2& currentImagePoints, Worker* worker)
{
	ocean_assert(previousFramePyramid && currentFrame);

	const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(previousPose, currentPose, componentCamera_, componentCamera_, componentPlane_));
	const SquareMatrix3 cameraFreeHomography(Geometry::Homography::normalizedHomography(componentCamera_.invertedIntrinsic() * homography * componentCamera_.intrinsic()));

	if (!componentIntermediateHomographyFrame_.set(currentFrame.frameType(), true, true))
	{
		return false;
	}

	if (!CV::FrameInterpolatorBilinear::Comfort::homographyWithCamera(componentCamera_, componentCamera_, currentFrame, componentIntermediateHomographyFrame_, homography, componentCamera_.hasDistortionParameters(), nullptr, worker))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// create a frame pyramid of the transformed current frame
	if (!componentIntermediateFramePyramid_.replace8BitPerChannel11(componentIntermediateHomographyFrame_, 2u, true /*copyFirstLayer*/, worker))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	previousImagePoints.clear();
	currentImagePoints.clear();
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, componentIntermediateFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(0.9 *  0.9), previousSubRegion, 20u, 20u, 30u, worker, 2u) || previousImagePoints.size() < 45)
	{
		previousImagePoints.clear();
		currentImagePoints.clear();
		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, componentIntermediateFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(1.5 *  1.5), previousSubRegion, 25u, 25u, 10u, worker, 2u) || previousImagePoints.size() < 25)
		{
			previousImagePoints.clear();
			currentImagePoints.clear();
			if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, componentIntermediateFramePyramid_, 2u, previousImagePoints, currentImagePoints, Scalar(1.9 *  1.9), previousSubRegion, 0u, 0u, 5u, worker, 2u) || previousImagePoints.size() < 25)
			{
				return false;
			}
		}
	}

	for (Vectors2::iterator i = currentImagePoints.begin(); i != currentImagePoints.end(); ++i)
	{
		const Vector2 normalizedImagePoint(componentCamera_.imagePoint2normalizedImagePoint<true>(*i, componentCamera_.hasDistortionParameters()));
		*i = componentCamera_.normalizedImagePoint2imagePoint<true>(cameraFreeHomography * normalizedImagePoint, componentCamera_.hasDistortionParameters());
	}

	const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(componentCamera_, previousPose, componentPlane_, previousImagePoints.data(), previousImagePoints.size(), componentCamera_.hasDistortionParameters()));

	// optimize the pose due to the new image points
	return Geometry::NonLinearOptimizationPose::optimizePose(componentCamera_, currentPose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), componentCamera_.hasDistortionParameters(), optimizedPose, 40u, Geometry::Estimator::ET_HUBER);
}

bool SLAMPlaneTracker::PlaneTrackerComponent::optimizePlane(Plane3& optimizedPlane, const unsigned int numberFrames)
{
	ocean_assert(numberFrames >= 10u);
	ocean_assert(componentPlane_.isValid());

	Indices32 poseMapping;
	poseMapping.reserve(componentPoses_.size());

	// find the threshold for feature point correspondences
	Indices32 pointCorrespondences;
	pointCorrespondences.reserve(componentPoses_.size());

	for (OfflinePoses::Index i = componentPoses_.firstIndex(); i < componentPoses_.endIndex(); ++i)
	{
		if (componentPoses_[i].isValid())
		{
			pointCorrespondences.push_back((unsigned int)componentImagePointsPairs_[i].first.size());
		}
	}

	const unsigned int qualityThreshold = Median::median(pointCorrespondences.data(), pointCorrespondences.size());

	// find a subset of all poses best representing the entire tracking sequence
	Poses poses;
	poses.reserve(componentPoses_.size());

	for (OfflinePoses::Index i = componentPoses_.firstIndex(); i < componentPoses_.endIndex(); ++i)
	{
		if (componentPoses_[i].isValid() && componentImagePointsPairs_[i].first.size() >= qualityThreshold)
		{
			// poses is not a shift vector so we have to adjust the index
			poses.push_back(Pose(componentPoses_[i].transformation()));
			poseMapping.push_back((unsigned int)i);
		}
	}

	ocean_assert(sizeof(SuccessionSubset<Scalar, 6>::Object) == sizeof(Pose));
	SuccessionSubset<Scalar, 6> successionSubset((SuccessionSubset<Scalar, 6>::Object*)poses.data(), poses.size());

	HomogenousMatrices4 bestPoses;
	Geometry::NonLinearOptimizationPlane::ImagePointsPairs bestImagePointsPairs;

	bestPoses.reserve(numberFrames);
	bestImagePointsPairs.reserve(numberFrames);

	while (bestPoses.size() < numberFrames)
	{
		// find the best next pose
		const size_t indexZeroBased = successionSubset.incrementSubset();
		if (indexZeroBased == size_t(-1))
		{
			break;
		}

		// the subset index might not be identical with the frame index of the tracking sequence
		const unsigned int frameIndex = poseMapping[indexZeroBased];

		ocean_assert(componentPoses_[frameIndex].isValid());

		bestPoses.push_back(componentPoses_[frameIndex].transformation());
		bestImagePointsPairs.push_back(componentImagePointsPairs_[frameIndex]);
	}

	ocean_assert(bestPoses.size() == bestImagePointsPairs.size());

	if (bestPoses.size() < 10)
	{
		return false;
	}

	HomogenousMatrices4 optimizedPoses;
	return Geometry::NonLinearOptimizationPlane::optimizePosesPlane(componentCamera_, componentInitialPose_, bestPoses, bestImagePointsPairs, componentPlane_, componentCamera_.hasDistortionParameters(), optimizedPoses, optimizedPlane, 40u, Geometry::Estimator::ET_SQUARE);
}

SLAMPlaneTracker::SLAMPlaneTracker() :
	initialPose_(Vector3(0, 0, 0))
{
	// nothing to do here
}

SLAMPlaneTracker::~SLAMPlaneTracker()
{
	stopThreadExplicitly();
}

bool SLAMPlaneTracker::setInitialTrackingArea(const Triangles2& triangles)
{
	if (triangles.empty())
	{
		ocean_assert(false && "Not triangles defined!");
		return false;
	}

	if (isThreadActive() || isThreadInvokedToStart())
	{
		return false;
	}

	initialSubRegion_ = CV::SubRegion(triangles);
	return true;
}

bool SLAMPlaneTracker::setInitialTrackingArea(const Frame& yMask)
{
	if (!yMask.isValid() || !yMask.isPixelFormatCompatible(FrameType::FORMAT_Y8) || yMask.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Invalid mask!");
		return false;
	}

	if (isThreadActive() || isThreadInvokedToStart())
	{
		return false;
	}

	const CV::PixelBoundingBox boundingBox(CV::MaskAnalyzer::detectBoundingBox(yMask.constdata<uint8_t>(), yMask.width(), yMask.height(), 0xFFu, yMask.paddingElements()));

	if (boundingBox.isValid())
	{
		initialSubRegion_ = CV::SubRegion(yMask, boundingBox);
	}
	else
	{
		initialSubRegion_ = CV::SubRegion();
	}

	return true;
}

bool SLAMPlaneTracker::applyFrameTracking(const FrameType& frameType)
{
	// check whether the initial camera object matches with the current frame type
	if (camera_.isValid() && (camera_.width() != frameType.width() || camera_.height() != frameType.height()))
	{
		Log::warning() << "This given camera object does not match the frame type!";
		camera_ = PinholeCamera();
	}

	// use a default camera object when no camera object is given
	if (!camera_.isValid())
	{
		ocean_assert(optimizeCamera_);
		camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(65));
	}

	// at this position a valid camera object must be defined, invoke the camera event
	ocean_assert(camera_);
	eventCallbacks_(CameraCalibrationStateEvent(id_, camera_));

	const unsigned int startFrameIndex = startFrameIndex_ == (unsigned int)(-1) ? lowerFrameIndex_ : startFrameIndex_;

	Plane3 initialPlane;
	PinholeCamera firstOptimizedCamera;

	ScopedEventStackLayer scopedEventLayer(*this, Scalar(0), Scalar(0.50));

	if (!determinePlane(lowerFrameIndex_, startFrameIndex, upperFrameIndex_) || shouldThreadStop())
	{
		return false;
	}

	scopedEventLayer.modify(Scalar(0.50), Scalar(1.00));

	if (!optimizePlane(lowerFrameIndex_, startFrameIndex, upperFrameIndex_) || shouldThreadStop())
	{
		return false;
	}

	removeIrregularPoses(Scalar(5));
	extrapolatePoses(15u, 25u);
	removeIrregularPoses(Scalar(5));

	// update the poses and provide an event
	updatePoses(offlinePoses_);

	return true;
}

bool SLAMPlaneTracker::determinePlane(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	ocean_assert(camera_.isValid());
	ocean_assert(!plane_.isValid());

	ocean_assert(lowerFrameIndex <= initialFrameIndex && initialFrameIndex <= upperFrameIndex);

	ScopedEventStackLayer scopedEventLayer(*this, Scalar(0), Scalar(0.5));

	Plane3 firstPlane;
	PinholeCamera firstOptimizedCamera;
	if (!determineInitialPlane(camera_, lowerFrameIndex, initialFrameIndex, upperFrameIndex, firstPlane, &firstOptimizedCamera) || shouldThreadStop())
	{
		return false;
	}

	// now we expect a valid plane
	ocean_assert(firstPlane.isValid());
	updatePlane(firstPlane);

	// check whether the optimized camera is plausible
	if (isPlausibleCamera(firstOptimizedCamera, CM_MEDIUM_QUALITY)) // **TODO** CM_MEDIUM_QUALITY
	{
		// try to improve the plane and camera profile accuracy
		ocean_assert(firstOptimizedCamera.isValid());

		scopedEventLayer.modify(Scalar(0.5), Scalar(1.0));

		Plane3 secondOptimizedPlane;
		PinholeCamera secondOptimizedCamera;

		if (!determineInitialPlane(firstOptimizedCamera, lowerFrameIndex, initialFrameIndex, upperFrameIndex, secondOptimizedPlane, &secondOptimizedCamera) || shouldThreadStop())
		{
			return false;
		}

		// now we expect a valid optimized plane - but we will not provide a plane event as this plane might be wrong - we have to check this first
		ocean_assert(secondOptimizedPlane.isValid());

		if (isPlausibleCamera(secondOptimizedCamera, CM_MEDIUM_QUALITY))// **TODO** CM_MEDIUM_QUALITY
		{
			// check whether the normals of both planes are almost identical

			ocean_assert(Numeric::isEqual(firstPlane.normal().length(), 1));
			ocean_assert(Numeric::isEqual(secondOptimizedPlane.normal().length(), 1));

			// the more perpendicular the detected plane with the viewing direction the more critical the determination of the real parameters
			const bool criticalPlane = firstPlane.normal().angle(Vector3(0, 0, 1)) < Numeric::deg2rad(20);
			const Scalar angleThreshold = criticalPlane ? Numeric::cos(Numeric::deg2rad(10)) : Numeric::cos(Numeric::deg2rad(20));

			if (firstPlane.normal() * secondOptimizedPlane.normal() >= angleThreshold)
			{
				// we found a valid and plausible camera profile so we can accept it, further we can accept the optimized plane

				updateCamera(secondOptimizedCamera);
				updatePlane(secondOptimizedPlane);
			}
		}
	}

	ocean_assert(plane_.isValid());
	ocean_assert(camera_.isValid());

	return true;
}

bool SLAMPlaneTracker::determineInitialPlane(const PinholeCamera& pinholeCamera, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, Plane3& optimizedPlane, PinholeCamera* optimizedCamera)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrameIndex <= initialFrameIndex && initialFrameIndex <= upperFrameIndex);

	const AnalysisComponentEvent startEvent(id(), ComponentEvent::CS_STARTED);
	AnalysisComponentEvent breakEvent(id(), ComponentEvent::CS_FAILED);
	AnalysisComponentEvent finishedEvent(id(), ComponentEvent::CS_FINISHED);

	return HomographyTrackerComponent(*this, pinholeCamera, optimizedPlane, Numeric::deg2rad(5), optimizedCamera).invoke(startEvent, breakEvent, finishedEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex);
}

bool SLAMPlaneTracker::optimizePlane(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	ocean_assert(camera_.isValid());
	ocean_assert(lowerFrameIndex <= initialFrameIndex && initialFrameIndex <= upperFrameIndex);

	ScopedEventStackLayer scopedEventLayer(*this, Scalar(0), Scalar(0.33));

	OfflinePoses poses;

	Plane3 firstOptimizedPlane;
	if (!trackPlane(camera_, plane_, lowerFrameIndex, initialFrameIndex, upperFrameIndex, true, &firstOptimizedPlane, &poses) || shouldThreadStop())
	{
		return false;
	}

	// for the first time we have valid camera poses
	updatePoses(poses);

	// check whether the normals of both planes are almost identical
	ocean_assert(Numeric::isEqual(plane_.normal().length(), 1));
	ocean_assert(Numeric::isEqual(firstOptimizedPlane.normal().length(), 1));

	// the initial plane should be accurate so we do not allow a large change of the planes' normals
	if (plane_.normal() * firstOptimizedPlane.normal() >= Numeric::cos(Numeric::deg2rad(5)))
	{
		scopedEventLayer.modify(Scalar(0.33), Scalar(0.66));

		Plane3 secondOptimizedPlane;
		if (!trackPlane(camera_, firstOptimizedPlane, lowerFrameIndex, initialFrameIndex, upperFrameIndex, false, &secondOptimizedPlane, nullptr) || shouldThreadStop())
		{
			return false;
		}

		// check whether the normals of both planes are almost identical
		ocean_assert(Numeric::isEqual(plane_.normal().length(), 1));
		ocean_assert(Numeric::isEqual(secondOptimizedPlane.normal().length(), 1));

		if (plane_.normal() * secondOptimizedPlane.normal() >= Numeric::cos(Numeric::deg2rad(7.5)))
		{
			updatePlane(secondOptimizedPlane);

			scopedEventLayer.modify(Scalar(0.66), Scalar(1.00));

			OfflinePoses finalPoses;
			if (!trackPlane(camera_, plane_, lowerFrameIndex, initialFrameIndex, upperFrameIndex, true, nullptr, &finalPoses) || shouldThreadStop())
			{
				return false;
			}

			updatePoses(finalPoses);
		}
	}

	return true;
}

bool SLAMPlaneTracker::trackPlane(const PinholeCamera& pinholeCamera, const Plane3& plane, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, const bool createStateEvents, Plane3* optimizedPlane, OfflinePoses* poses)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrameIndex <= initialFrameIndex && initialFrameIndex <= upperFrameIndex);

	const TrackingComponentEvent startEvent(id(), ComponentEvent::CS_STARTED);
	TrackingComponentEvent breakEvent(id(), ComponentEvent::CS_FAILED);
	TrackingComponentEvent finishedEvent(id(), ComponentEvent::CS_FINISHED);

	return PlaneTrackerComponent(*this, pinholeCamera, initialPose_, plane, createStateEvents, poses, optimizedPlane).invoke(startEvent, breakEvent, finishedEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex);
}

void SLAMPlaneTracker::updatePlane(const Plane3& plane)
{
	const ScopedLock scopedLock(lock_);
	PlaneTracker::updatePlane(plane);

	// update the object transformation as the camera has been changed
	updateObjectTransformation();
}

void SLAMPlaneTracker::updateCamera(const PinholeCamera& pinholeCamera)
{
	const ScopedLock scopedLock(lock_);
	FrameTracker::updateCamera(pinholeCamera);

	// update the object transformation as the camera has been changed
	updateObjectTransformation();
}

bool SLAMPlaneTracker::updateObjectTransformation()
{
	const Vector2 distortedOrigin = initialSubRegion_.isEmpty() ?
					Vector2(Scalar(camera_.width()), Scalar(camera_.height())) * Scalar(0.5) :
					initialSubRegion_.boundingBox().center();

	const Vector2 undistortedOrigin(camera_.undistort<true>(distortedOrigin));

	const Line3 centerRay(camera_.ray(undistortedOrigin, initialPose_));

	// the back-projected center position will be the origin of the local coordinate system
	Vector3 centerPosition;
	if (!plane_.intersection(centerRay, centerPosition))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// the normal of the plane will be the z-axis of the local coordinate system
	// now we need to find the y-axis

	const Vector3& zAxis = plane_.normal();

	const Vector2 distortedCameraTop(undistortedOrigin.x(), undistortedOrigin.y() * Scalar(0.5));
	const Vector2 undistortedCameraTop(camera_.undistort<true>(distortedCameraTop));

	const Line3 topRay(camera_.ray(undistortedCameraTop, initialPose_));

	Vector3 topPosition;
	if (!plane_.intersection(topRay, topPosition))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const Vector3 notPerpendicularYAxis(topPosition - centerPosition);

	// now we determine the correct x-axis, and find the correct (perpendicular) y-axis afterwards

	Vector3 xAxis(notPerpendicularYAxis.cross(zAxis));
	if (!xAxis.normalize())
	{
		return false;
	}

	const Vector3 yAxis(zAxis.cross(xAxis));
	ocean_assert(Numeric::isEqual(yAxis.length(), 1));

	objectTransformation_ = HomogenousMatrix4(xAxis, yAxis, zAxis, centerPosition);

	eventCallbacks_(TrackerTransformationStateEvent(id_, objectTransformation_));
	return true;
}

}

}

}
