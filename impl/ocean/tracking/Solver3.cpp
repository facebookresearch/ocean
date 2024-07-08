/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/Solver3.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/ShiftVector.h"
#include "ocean/base/Subset.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/NonLinearOptimizationPlane.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/StereoscopicGeometry.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Pose.h"
#include "ocean/math/SuccessionSubset.h"

#include <iterator>

namespace Ocean
{

namespace Tracking
{

Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor::PoseToObjectPointIdImagePointCorrespondenceAccessor(const Database& database, const Indices32& poseIds, const Indices32& objectPointIds, const unsigned int minimalVisibleObjectPoints, Indices32* validPoseIndices, Indices32* usedObjectPointIndices)
{
	ocean_assert(validPoseIndices == nullptr || validPoseIndices->empty());

	elementGroups_.reserve(poseIds.size());

	if (validPoseIndices)
	{
		validPoseIndices->clear();
		validPoseIndices->reserve(poseIds.size());
	}

	Indices32 visibleObjectPointIndices;
	visibleObjectPointIndices.reserve(objectPointIds.size());

	std::vector<unsigned char> usedObjectPointStatements(objectPointIds.size(), 0u);

	Vectors2 imagePoints;

	for (size_t i = 0; i < poseIds.size(); ++i)
	{
		const Index32 poseId = poseIds[i];

		visibleObjectPointIndices.clear();

		imagePoints = database.imagePointsFromObjectPoints<false>(poseId, objectPointIds, visibleObjectPointIndices);
		ocean_assert(imagePoints.size() == visibleObjectPointIndices.size());

		if (imagePoints.size() < minimalVisibleObjectPoints)
		{
			continue;
		}

		Elements elements(imagePoints.size());

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			const unsigned int objectPointIndex = visibleObjectPointIndices[n];

			elements[n] = Element(objectPointIds[objectPointIndex], imagePoints[n]);

			usedObjectPointStatements[objectPointIndex] = 1u;
		}

		elementGroups_.emplace_back(std::move(elements));

		if (validPoseIndices != nullptr)
		{
			validPoseIndices->emplace_back(Index32(i));
		}
	}

	if (usedObjectPointIndices != nullptr)
	{
		*usedObjectPointIndices = Subset::statements2indices<unsigned int, 1u>(usedObjectPointStatements);
	}
}

Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::ObjectPointToPoseImagePointCorrespondenceAccessor(const Database& database, const Indices32& poseIds, const Indices32& objectPointCandidateIds, const unsigned int minimalObservationsInKeyframes, Indices32* validObjectPoints)
{
	ocean_assert(validObjectPoints == nullptr || validObjectPoints->empty());

	elementGroups_.reserve(objectPointCandidateIds.size());

	if (validObjectPoints)
	{
		validObjectPoints->clear();
		validObjectPoints->reserve(poseIds.size());
	}

	Indices32 validPoseIndices;
	Vectors2 imagePoints;

	for (size_t i = 0; i < objectPointCandidateIds.size(); ++i)
	{
		validPoseIndices.clear();
		imagePoints.clear();
		database.observationsFromObjectPoint<false>(objectPointCandidateIds[i], poseIds, validPoseIndices, nullptr, &imagePoints);
		ocean_assert(validPoseIndices.size() == imagePoints.size());

		if (validPoseIndices.size() >= minimalObservationsInKeyframes)
		{
			Elements elements(imagePoints.size());

			for (size_t n = 0; n < imagePoints.size(); ++n)
			{
				elements[n] = Element(validPoseIndices[n], imagePoints[n]);
			}

			elementGroups_.emplace_back(std::move(elements));

			if (validObjectPoints != nullptr)
			{
				validObjectPoints->emplace_back(Index32(i));
			}
		}
	}
}

Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::ObjectPointToPoseImagePointCorrespondenceAccessor(const Database& database, const Index32 lowerPoseId, const Index32 upperPoseId, const Indices32& objectPointIds, const unsigned int minimalObservationsInKeyframes, const unsigned int minimalKeyFrames, Indices32* usedKeyFrameIds)
{
	// our task is now to find the smallest number of keyframes (>= 'minimalKeyFrames') that see all object points at least 'minimalObservationsInKeyframes' times (unless an object point does have less observations)

	ocean_assert(upperPoseId >= lowerPoseId);
	ocean_assert(minimalKeyFrames <= upperPoseId - lowerPoseId + 1u);

	if (usedKeyFrameIds != nullptr)
	{
		usedKeyFrameIds->clear();
	}

	// the ids of all object points which currently don't have enough observations
	IndexSet32 pendingObjectPointIds(objectPointIds.begin(), objectPointIds.end());

	Indices32 internalUsedKeyFrameIds;

	IndexMap32 objectPointObservationCounterMap;

	Poses keyFrameCandidatePoses;
	keyFrameCandidatePoses.reserve(database.poseNumber<false>());

	const HomogenousMatrices4 poses(database.poses<false>(lowerPoseId, upperPoseId));

	for (size_t n = 0; n < poses.size(); ++n)
	{
		keyFrameCandidatePoses.emplace_back(poses[n]);
	}

	SuccessionSubset<Scalar, 6> successionSubset((SuccessionSubset<Scalar, 6>::Object*)keyFrameCandidatePoses.data(), keyFrameCandidatePoses.size());

	// we start with the pose having most visible observations

	Index32 firstPoseId = (unsigned int)(-1);
	bool debugResult = database.poseWithMostCorrespondences<false, false, false>(lowerPoseId, upperPoseId, &firstPoseId);
	ocean_assert_and_suppress_unused(debugResult, debugResult);

	debugResult = successionSubset.incrementSubset(size_t(firstPoseId - lowerPoseId)); // the first pose in the succession object corresponds to lowerPoseId
	ocean_assert(debugResult);

	internalUsedKeyFrameIds.push_back(firstPoseId);

	for (const Index32 objectPointId : objectPointIds)
	{
		if (database.hasObservation<false>(firstPoseId, objectPointId))
		{
			objectPointObservationCounterMap[objectPointId]++;
		}
	}

	while (successionSubset.subset().size() < successionSubset.size())
	{
		if (internalUsedKeyFrameIds.size() >= minimalKeyFrames)
		{
			// we have enough keyframes so we check whether each object point has enough visible projections in those keyframes

			if (pendingObjectPointIds.empty())
			{
				break;
			}
		}

		// we need another keyframe

		while (successionSubset.subset().size() < successionSubset.size())
		{
			const Index32 newPoseId = Index32(successionSubset.incrementSubset()) + lowerPoseId;

			// now we check whether this pose is a 'good' pose so that this pose contains at least one object point with too less observations

			Indices32 currentObjectPointIds;
			currentObjectPointIds.reserve(objectPointIds.size());

			for (const Index32 objectPointId : objectPointIds)
			{
				if (database.hasObservation<false>(newPoseId, objectPointId))
				{
					currentObjectPointIds.emplace_back(objectPointId);
				}
			}

			ocean_assert(!currentObjectPointIds.empty());

			bool foundGoodObjectPoint = pendingObjectPointIds.empty();
			for (Indices32::const_iterator i = currentObjectPointIds.begin(); !foundGoodObjectPoint && i != currentObjectPointIds.end(); ++i)
			{
				foundGoodObjectPoint = pendingObjectPointIds.find(*i) != pendingObjectPointIds.end();
			}

			if (!foundGoodObjectPoint)
			{
				// we take the next pose
				continue;
			}

			internalUsedKeyFrameIds.push_back(newPoseId);

			for (Indices32::const_iterator i = currentObjectPointIds.begin(); i != currentObjectPointIds.end(); ++i)
			{
				if (++objectPointObservationCounterMap[*i] >= minimalObservationsInKeyframes)
				{
					pendingObjectPointIds.erase(*i);
				}
			}

			// we stop here and check whether we need a further pose
			break;
		}
	}

	elementGroups_.resize(objectPointIds.size());

	Indices32 validPoseIndices;
	Vectors2 imagePoints;

	for (size_t i = 0; i < objectPointIds.size(); ++i)
	{
		validPoseIndices.clear();
		imagePoints.clear();
		database.observationsFromObjectPoint<false>(objectPointIds[i], internalUsedKeyFrameIds, validPoseIndices, nullptr, &imagePoints);

		ocean_assert(validPoseIndices.size() == imagePoints.size());
		ocean_assert(validPoseIndices.size() >= minimalObservationsInKeyframes || database.imagePointsFromObjectPoint<false>(objectPointIds[i]).size() == validPoseIndices.size());

		Elements elements(imagePoints.size());

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			elements[n] = Element(validPoseIndices[n], imagePoints[n]);
		}

		elementGroups_[i] = std::move(elements);
	}

	if (usedKeyFrameIds != nullptr)
	{
		*usedKeyFrameIds = std::move(internalUsedKeyFrameIds);
	}
}

Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const std::vector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock)
{
	ocean_assert(posesSubsetBlock.size() <= imagePointGroups.size());
	if (imagePointGroups.empty())
		return;

	const size_t numberObjectPoints = imagePointGroups.front().size();

	elementGroups_.reserve(numberObjectPoints);

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		Elements elements;
		elements.reserve(posesSubsetBlock.size());

		for (unsigned int i = 0u; i < posesSubsetBlock.size(); ++i)
		{
			ocean_assert(posesSubsetBlock[i] < imagePointGroups.size());
			const Vectors2& imagePoints = imagePointGroups[posesSubsetBlock[i]];

			ocean_assert(imagePoints.size() == numberObjectPoints);

			elements.emplace_back(i, imagePoints[n]);
		}

		elementGroups_.emplace_back(std::move(elements));
	}
}

Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const ShiftVector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock)
{
	ocean_assert(posesSubsetBlock.size() <= imagePointGroups.size());
	if (imagePointGroups.isEmpty())
	{
		return;
	}

	const size_t numberObjectPoints = imagePointGroups.front().size();

	elementGroups_.reserve(numberObjectPoints);

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		Elements elements;
		elements.reserve(posesSubsetBlock.size());

		for (unsigned int i = 0u; i < posesSubsetBlock.size(); ++i)
		{
			ocean_assert(posesSubsetBlock[i] < imagePointGroups.size());
			const Vectors2& imagePoints = imagePointGroups[imagePointGroups.firstIndex() + posesSubsetBlock[i]];

			ocean_assert(imagePoints.size() == numberObjectPoints);

			elements.emplace_back(i, imagePoints[n]);
		}

		elementGroups_.emplace_back(std::move(elements));
	}
}

Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const std::vector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock, const Indices32& objectPointsSubset)
{
	ocean_assert(posesSubsetBlock.size() <= imagePointGroups.size());
	if (imagePointGroups.empty())
	{
		return;
	}

	ocean_assert(objectPointsSubset.size() <= imagePointGroups.front().size());

	elementGroups_.reserve(objectPointsSubset.size());

	for (size_t n = 0; n < objectPointsSubset.size(); ++n)
	{
		Elements elements;
		elements.reserve(posesSubsetBlock.size());

		for (unsigned int i = 0u; i < posesSubsetBlock.size(); ++i)
		{
			ocean_assert(posesSubsetBlock[i] < imagePointGroups.size());
			const Vectors2& imagePoints = imagePointGroups[posesSubsetBlock[i]];

			ocean_assert(imagePoints.size() >= objectPointsSubset.size());
			ocean_assert(objectPointsSubset[n] < imagePoints.size());

			elements.emplace_back(i, imagePoints[objectPointsSubset[n]]);
		}

		elementGroups_.emplace_back(std::move(elements));
	}
}

Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const ShiftVector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock, const Indices32& objectPointsSubset)
{
	ocean_assert(posesSubsetBlock.size() <= imagePointGroups.size());
	if (imagePointGroups.isEmpty())
	{
		return;
	}

	ocean_assert(objectPointsSubset.size() <= imagePointGroups.front().size());

	elementGroups_.reserve(objectPointsSubset.size());

	for (size_t n = 0; n < objectPointsSubset.size(); ++n)
	{
		Elements elements;
		elements.reserve(posesSubsetBlock.size());

		for (unsigned int i = 0u; i < posesSubsetBlock.size(); ++i)
		{
			ocean_assert(posesSubsetBlock[i] < imagePointGroups.size());
			const Vectors2& imagePoints = imagePointGroups[imagePointGroups.firstIndex() + posesSubsetBlock[i]];

			ocean_assert(imagePoints.size() >= objectPointsSubset.size());
			ocean_assert(objectPointsSubset[n] < imagePoints.size());

			elements.emplace_back(i, imagePoints[objectPointsSubset[n]]);
		}

		elementGroups_.emplace_back(std::move(elements));
	}
}

bool Solver3::determineInitialObjectPointsFromSparseKeyFrames(const Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const Scalar maximalStaticImagePointFilterRatio, Vectors3& initialObjectPoints, Indices32& initialObjectPointIds, const RelativeThreshold& pointsThreshold, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Indices32* usedPoseIds, Scalar* finalSqrError, Scalar* finalImagePointDistance, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame < upperFrame);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

	ocean_assert(maximalStaticImagePointFilterRatio >= 0 && maximalStaticImagePointFilterRatio <= 1);

	// we first track image points from frame to frame until only a specific amount (pointsThreshold) of the image points from the first frame are determined

	Indices32 startObjectPointIds;
	database.imagePointsWithObjectPoints<false>(startFrame, startObjectPointIds);

	unsigned int minimalTrackingPoints;
	if (!pointsThreshold.hasValidThreshold<5u>((unsigned int)startObjectPointIds.size(), &minimalTrackingPoints))
		return false;

	Indices32 objectPointIds;
	ImagePointGroups imagePointGroups;
	if (!trackObjectPoints(database, startObjectPointIds, lowerFrame, startFrame, upperFrame, minimalTrackingPoints, 0u, pointsThreshold.upperBoundary(), objectPointIds, imagePointGroups, nullptr, abort))
		return false;

	// ensure that we have enough key frames
	if (imagePointGroups.size() < minimalKeyFrames || (abort && *abort))
		return false;

#ifdef OCEAN_DEBUG
	for (ShiftVector<Vectors2>::Index n = imagePointGroups.firstIndex(); n < imagePointGroups.endIndex(); ++n)
	{
		Vector2 imagePoint;
		const Vectors2& imagePoints = imagePointGroups[n];
		ocean_assert(imagePoints.size() == objectPointIds.size());

		for (size_t i = 0; i < objectPointIds.size(); ++i)
			ocean_assert(database.hasObservation<false>(Index32(n), objectPointIds[i], &imagePoint) && imagePoints[i] == imagePoint);
	}
#endif

	// we removal perfectly static image points that may be located at static logos in the video sequence
	filterStaticImagePoints(imagePointGroups, objectPointIds, maximalStaticImagePointFilterRatio);

	Vectors2 offsets;
	offsets.reserve(imagePointGroups.size());
	offsets.emplace_back(Scalar(0), Scalar(0));

	for (ShiftVector<Vectors2>::Index n = imagePointGroups.firstIndex() + 1; n < imagePointGroups.endIndex(); ++n)
	{
		const Vectors2& points0 = imagePointGroups[n - 1];
		const Vectors2& points1 = imagePointGroups[n];
		ocean_assert(points0.size() == points1.size());

		Vector2 offset(0, 0);

		for (size_t i = 0; i < points0.size(); ++i)
		{
			offset += points1[i] - points0[i];
		}

		offsets.emplace_back(offsets.back() + offset);
	}

	ocean_assert(offsets.size() == imagePointGroups.size());

	SuccessionSubset<Scalar, 2> successionSubset((SuccessionSubset<Scalar, 2>::Object*)offsets.data(), offsets.size());
	const SuccessionSubset<Scalar, 2>::Indices offsetIndices = successionSubset.subset(maximalKeyFrames);

	Indices32 keyFrameIndices(offsetIndices.size());
	for (size_t n = 0; n < offsetIndices.size(); ++n)
	{
		keyFrameIndices[n] = (unsigned int)(offsetIndices[n]) + (unsigned int)(imagePointGroups.firstIndex());
	}

	std::sort(keyFrameIndices.begin(), keyFrameIndices.end());

	Database::ImagePointGroups keyFramesImagePointGroups;
	keyFramesImagePointGroups.reserve(keyFrameIndices.size());

	for (const Index32 keyFrameIndex : keyFrameIndices)
	{
		keyFramesImagePointGroups.emplace_back(imagePointGroups[keyFrameIndex]);
	}

	// now we try to determine an initial 3D structure (we determine the initial 3D object points) by application of the separated key-frames in combination with a RANSAC approach

	const unsigned int ransacIterations = minmax(10u, (unsigned int)(keyFrameIndices.size() * keyFrameIndices.size()) / 2u, 100u);

	HomogenousMatrices4 roughPoses;
	Vectors3 roughObjectPoints;
	Indices32 roughPoseIndices;
	Indices32 roughObjectPointIndices;

	if (!determineInitialObjectPointsFromSparseKeyFramesRANSAC(pinholeCamera, keyFramesImagePointGroups, randomGenerator, roughPoses, roughPoseIndices, roughObjectPoints, roughObjectPointIndices, ransacIterations, RelativeThreshold(10u, Scalar(0.3), 25u), maximalSqrError, &database, &keyFrameIndices, &objectPointIds, abort))
	{
		return false;
	}

	// check whether the caller of this function is interested in the ids of the poses which are used to calculate the initial object points
	if (usedPoseIds != nullptr)
	{
		ocean_assert(usedPoseIds->empty());
		usedPoseIds->clear();

		for (Indices32::const_iterator i = roughPoseIndices.begin(); i != roughPoseIndices.end(); ++i)
		{
			usedPoseIds->push_back(keyFrameIndices[*i]);
		}
	}

	// now we try to stabilize/improve the accuracy of the found 3D object points

	if (finalImagePointDistance)
		*finalImagePointDistance = 0;

	Vectors2 roughImagePoints;
	for (size_t n = 0; n < roughPoses.size(); ++n)
	{
		ocean_assert(roughPoses[n].isValid());

		const Index32 poseIndex = roughPoseIndices[n];

		const Vectors2 points = Subset::subset(keyFramesImagePointGroups[poseIndex], roughObjectPointIndices);
		roughImagePoints.insert(roughImagePoints.end(), points.begin(), points.end());

		if (finalImagePointDistance)
			*finalImagePointDistance += averagePointDistance(points.data(), points.size());
	}

	if (finalImagePointDistance)
		*finalImagePointDistance /= Scalar(roughPoses.size());

	Vectors3 optimizedObjectPoints(roughObjectPointIndices.size());
	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

	const ObjectPointToPoseIndexImagePointCorrespondenceAccessor objectPointToPoseIndexImagePointCorrespondenceAccessor(keyFramesImagePointGroups, roughPoseIndices, roughObjectPointIndices);

	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<HomogenousMatrix4>(roughPoses), ConstArrayAccessor<Vector3>(roughObjectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, nullptr, &optimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, nullptr, finalSqrError))
	{
		return false;
	}

	ocean_assert(initialObjectPoints.empty());
	initialObjectPoints.clear();
	initialObjectPoints.reserve(optimizedObjectPoints.size());

	ocean_assert(initialObjectPointIds.empty());
	initialObjectPointIds.clear();
	initialObjectPointIds.reserve(optimizedObjectPoints.size());

	ocean_assert(roughObjectPointIndices.size() == optimizedObjectPoints.size());
	for (size_t n = 0; n < optimizedObjectPoints.size(); ++n)
	{
		const Index32 objectPointId = objectPointIds[roughObjectPointIndices[n]];
		const Vector3& objectPoint = optimizedObjectPoints[n];

		initialObjectPoints.push_back(objectPoint);
		initialObjectPointIds.push_back(objectPointId);
	}

	return !abort || !*abort;
}

bool Solver3::determineInitialObjectPointsFromDenseFrames(const Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Scalar maximalStaticImagePointFilterRatio, Vectors3& initialObjectPoints, Indices32& initialObjectPointIds, const RelativeThreshold& pointsThreshold, const Scalar minimalTrackedFramesRatio, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Indices32* usedPoseIds, Scalar* finalSqrError, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame < upperFrame);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

	ocean_assert(maximalStaticImagePointFilterRatio >= 0 && maximalStaticImagePointFilterRatio <= 1);

	// we first track image points from frame to frame until only a specific amount (pointsThreshold) of the image points from the first frame are determined

	Indices32 startObjectPointIds;
	const Vectors2 startImagePoints = database.imagePointsWithObjectPoints<false>(startFrame, startObjectPointIds);

	// now we check whether we have a region of interest so that we first have to identify all object points lying inside the region so that we can handled them with higher priority

	Indices32 objectPointIds;
	ImagePointGroups imagePointGroups;

	if (!regionOfInterest.isEmpty())
	{
		ocean_assert(startImagePoints.size() == startObjectPointIds.size());

		Indices32 startPriorityObjectPointIds, startRemainingObjectPointIds;
		startPriorityObjectPointIds.reserve(startImagePoints.size());
		startRemainingObjectPointIds.reserve(startImagePoints.size());

		for (size_t n = 0; n < startImagePoints.size(); ++n)
			if (regionOfInterest.isInside(startImagePoints[n]))
				startPriorityObjectPointIds.push_back(startObjectPointIds[n]);
			else
				startRemainingObjectPointIds.push_back(startObjectPointIds[n]);

		// now we need to decided whether we have enough priority object points so that we can start an object point tracking with focus on the priority object points

		if (startPriorityObjectPointIds.size() >= pointsThreshold.lowerBoundary())
		{
			// we determine the amount of object points which can be tracked within the direct (1 frame) neighborhood as this is a good indicator how complex the current scene is (at the start frame)
			const unsigned int validNeighborObjectPoints = (unsigned int)trackObjectPointsToNeighborFrames(database, startPriorityObjectPointIds, lowerFrame, startFrame, upperFrame).size();
			ocean_assert(validNeighborObjectPoints <= startPriorityObjectPointIds.size());

			Log::info() << "Neighbor object points " << validNeighborObjectPoints << " from " << startPriorityObjectPointIds.size();

			unsigned int relativeThreshold;
			if (pointsThreshold.hasValidThreshold<5u>(validNeighborObjectPoints, &relativeThreshold))
			{
				// in the case that we use the region of interest as sole tracking area we will not have any remaining object points

				if (startRemainingObjectPointIds.empty())
				{
					Log::info() << "Tracking " << startPriorityObjectPointIds.size() << " (priority) object points without investigating remaining object points...";

					const unsigned int maximalTrackedPriorityObjectPoints = min(pointsThreshold.upperBoundary(), (unsigned int)startPriorityObjectPointIds.size());

					if (!trackObjectPoints(database, startPriorityObjectPointIds, lowerFrame, startFrame, upperFrame, relativeThreshold, 0u, maximalTrackedPriorityObjectPoints, objectPointIds, imagePointGroups, nullptr, abort))
						return false;

					Log::info() << "Finished the point tracking with " << objectPointIds.size() << " object points in " << imagePointGroups.size() << " frames/poses";
				}
				else
				{
					Log::info() << "Tracking " << startPriorityObjectPointIds.size() << " priority object points and " << startRemainingObjectPointIds.size() << " remaining object points...";

					const unsigned int maximalTrackedPriorityObjectPoints = min(pointsThreshold.upperBoundary(), (unsigned int)startPriorityObjectPointIds.size());
					const unsigned int maximalTrackedRemainingObjectPoints = min(pointsThreshold.upperBoundary(), (unsigned int)startRemainingObjectPointIds.size());

					Indices32 priorityObjectPointIds;
					trackObjectPoints(database, startPriorityObjectPointIds, startRemainingObjectPointIds, lowerFrame, startFrame, upperFrame, relativeThreshold, Scalar(0.8), maximalTrackedPriorityObjectPoints, maximalTrackedRemainingObjectPoints, objectPointIds, imagePointGroups, &priorityObjectPointIds, nullptr, abort);

					Log::info() << "Finished the point tracking with " << objectPointIds.size() << " object points in " << imagePointGroups.size() << " frames/poses";

					// check whether the resulting tracked points are good enough so that we can work with them, otherwise we stop here
					if (priorityObjectPointIds.size() < relativeThreshold || Scalar(imagePointGroups.size()) < Scalar(upperFrame - lowerFrame + 1u) * minimalTrackedFramesRatio)
					{
						Log::info() << "We expected more tracked points so that we stop here (we need a different tracking strategy)";
						return false;
					}
				}
			}
		}
	}

	if (abort && *abort)
		return false;

	// independent of any region of interest we start a normal tracking if we have too less groups of image points
	if (imagePointGroups.size() < minimalKeyFrames)
	{
		objectPointIds.clear();
		imagePointGroups.clear();

		unsigned int minimalTrackedObjectPoints;
		if (!pointsThreshold.hasValidThreshold<5u>((unsigned int)startObjectPointIds.size(), &minimalTrackedObjectPoints))
			return false;

		Log::info() << "Tracking " << startObjectPointIds.size() << " object points...";

		const unsigned int maximalTrackedObjectPoints = min(pointsThreshold.upperBoundary(), (unsigned int)startObjectPointIds.size());

		if (!trackObjectPoints(database, startObjectPointIds, lowerFrame, startFrame, upperFrame, minimalTrackedObjectPoints, 0u, maximalTrackedObjectPoints, objectPointIds, imagePointGroups, nullptr, abort))
			return false;

		Log::info() << "Finished with " << objectPointIds.size() << " object points in " << imagePointGroups.size() << " poses";
	}

	// ensure that we have enough frames
	if (imagePointGroups.size() < minimalKeyFrames || (abort && *abort))
		return false;

#ifdef OCEAN_DEBUG
	for (ShiftVector<Vectors2>::Index n = imagePointGroups.firstIndex(); n < imagePointGroups.endIndex(); ++n)
	{
		Vector2 imagePoint;
		const Vectors2& imagePoints = imagePointGroups[n];
		ocean_assert(imagePoints.size() == objectPointIds.size());

		for (size_t i = 0; i < objectPointIds.size(); ++i)
			ocean_assert(database.hasObservation<false>(Index32(n), objectPointIds[i], &imagePoint) && imagePoints[i] == imagePoint);
	}
#endif

	// we removal perfectly static image points that may be located at static logos in the video sequence
	filterStaticImagePoints(imagePointGroups, objectPointIds, maximalStaticImagePointFilterRatio);

	HomogenousMatrices4 roughPoses;
	Indices32 roughPoseIds;
	Vectors3 roughObjectPoints;
	Indices32 roughObjectPointIndices;
	if (!determineInitialObjectPointsFromDenseFramesRANSAC(pinholeCamera, imagePointGroups, randomGenerator, roughPoses, roughPoseIds, roughObjectPoints, roughObjectPointIndices, 100u, RelativeThreshold(min(10u, pointsThreshold.lowerBoundary()), Scalar(0.3), 25u), maximalSqrError, WorkerPool::get().scopedWorker()(), abort))
		return false;

	const Indices32 keyFrameIndices = Pose::representativePoses(roughPoses.data(), roughPoses.size(), maximalKeyFrames);

	if (keyFrameIndices.size() < minimalKeyFrames)
		return false;

	Indices32 keyFramePoseIds = Subset::subset(roughPoseIds, keyFrameIndices);

	Vectors3 optimizedObjectPoints(roughObjectPointIndices.size());
	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

	std::vector<Vectors2> keyFrameImagePointGroups;
	keyFrameImagePointGroups.reserve(keyFrameIndices.size());

	for (Indices32::const_iterator i = keyFrameIndices.begin(); i != keyFrameIndices.end(); ++i)
		keyFrameImagePointGroups.push_back(imagePointGroups[roughPoseIds[*i]]);

	const ObjectPointToPoseIndexImagePointCorrespondenceAccessor objectPointToPoseIndexImagePointCorrespondenceAccessor(keyFrameImagePointGroups, createIndices(keyFrameImagePointGroups.size(), 0u), roughObjectPointIndices);

	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(AnyCameraPinhole(pinholeCamera), ConstArraySubsetAccessor<HomogenousMatrix4, unsigned int>(roughPoses, keyFrameIndices), ConstArrayAccessor<Vector3>(roughObjectPoints), objectPointToPoseIndexImagePointCorrespondenceAccessor, nullptr, &optimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, nullptr, finalSqrError))
	{
		return false;
	}

	if (usedPoseIds)
		*usedPoseIds = keyFramePoseIds;

	ocean_assert(initialObjectPoints.empty());
	initialObjectPoints.clear();
	initialObjectPoints.reserve(optimizedObjectPoints.size());

	ocean_assert(initialObjectPointIds.empty());
	initialObjectPointIds.clear();
	initialObjectPointIds.reserve(optimizedObjectPoints.size());

	ocean_assert(roughObjectPointIndices.size() == optimizedObjectPoints.size());
	for (size_t n = 0; n < optimizedObjectPoints.size(); ++n)
	{
		const Index32 objectPointId = objectPointIds[roughObjectPointIndices[n]];
		const Vector3& objectPoint = optimizedObjectPoints[n];

		initialObjectPoints.push_back(objectPoint);
		initialObjectPointIds.push_back(objectPointId);
	}

	return !abort || !*abort;
}

bool Solver3::determineInitialObjectPointsFromSparseKeyFramesBySteps(const Database& database, const unsigned int steps, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const Scalar maximalStaticImagePointFilterRatio, Vectors3& initialObjectPoints, Indices32& initialObjectPointIds, const RelativeThreshold& pointsThreshold, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Indices32* usedPoseIds, Worker* worker, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());

	ocean_assert(steps >= 1u);
	ocean_assert(steps <= upperFrame - lowerFrame + 1u);
	ocean_assert(lowerFrame < upperFrame);

	Indices32 startFrames;
	startFrames.reserve(steps);

	if (steps <= 1u)
		startFrames.push_back(lowerFrame);
	else
	{
		for (unsigned int n = 0u; n < steps; ++n)
		{
			const unsigned int startFrame = lowerFrame + ((upperFrame - lowerFrame) * n / (steps - 1u));
			ocean_assert(startFrame >= lowerFrame && startFrame <= upperFrame);

			if (startFrames.empty() || startFrames.back() != startFrame)
				startFrames.push_back(startFrame);

			ocean_assert(n != 0u || startFrames.back() == lowerFrame);
			ocean_assert(n + 1u != steps || startFrames.back() == upperFrame);
		}
	}

	ocean_assert(!startFrames.empty());
	ocean_assert(IndexSet32(startFrames.begin(), startFrames.end()).size() == startFrames.size());

	Vectors3 bestInitialObjectPoints;
	Indices32 bestInitialObjectPointIds;
	Indices32 bestInitialPoseIds;
	Scalar bestInitialPointDistance = 0;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&determineInitialObjectPointsFromSparseKeyFramesByStepsSubset, &database, &pinholeCamera, &randomGenerator, lowerFrame, (const Indices32*)&startFrames, upperFrame, maximalStaticImagePointFilterRatio, &bestInitialObjectPoints, &bestInitialObjectPointIds, &bestInitialPoseIds, &bestInitialPointDistance, &pointsThreshold, minimalKeyFrames, maximalKeyFrames, maximalSqrError, &lock, abort, worker->threads(), 0u, 0u), 0u, worker->threads());
	}
	else
		determineInitialObjectPointsFromSparseKeyFramesByStepsSubset(&database, &pinholeCamera, &randomGenerator, lowerFrame, &startFrames, upperFrame, maximalStaticImagePointFilterRatio, &bestInitialObjectPoints, &bestInitialObjectPointIds, &bestInitialPoseIds, &bestInitialPointDistance, &pointsThreshold, minimalKeyFrames, maximalKeyFrames, maximalSqrError, nullptr, abort, 1u, 0u, 1u);

	const bool result = !bestInitialObjectPoints.empty() && !bestInitialPoseIds.empty() && (!abort || !*abort);

	initialObjectPoints = std::move(bestInitialObjectPoints);
	initialObjectPointIds = std::move(bestInitialObjectPointIds);

	if (usedPoseIds)
		*usedPoseIds = std::move(bestInitialPoseIds);

	return result;
}

bool Solver3::determineInitialObjectPointsFromSparseKeyFramesRANSAC(const PinholeCamera& pinholeCamera, const Database::ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, HomogenousMatrices4& poses, Indices32& validPoseIndices, Vectors3& objectPoints, Indices32& validObjectPointIndices, const unsigned int iterations, const RelativeThreshold& minimalValidObjectPoints, const Scalar maximalSqrError, const Database* database, const Indices32* keyFrameIds, const Indices32* objectPointIds, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(imagePointGroups.size() >= 2);

	ocean_assert(poses.empty() && validPoseIndices.empty());
	ocean_assert(objectPoints.empty() && validObjectPointIndices.empty());

	ocean_assert((database && keyFrameIds && objectPointIds) || (!database && !keyFrameIds && !objectPointIds));

	Scalar bestPointDistance = Numeric::maxValue();

	for (unsigned int n = 0u; n < iterations && (!abort || !*abort); ++n)
	{
		// **TODO** a iteration over each possible pair could be a good idea

		unsigned int index0, index1;
		RandomI::random(randomGenerator, (unsigned int)imagePointGroups.size() - 1u, index0, index1);
		ocean_assert(index0 != index1);

		HomogenousMatrices4 iterationPoses;
		Indices32 iterationPoseIndices;
		Vectors3 iterationObjectPoints;
		Indices32 iterationObjectPointIndices;

		if (determineInitialObjectPointsFromSparseKeyFrames(pinholeCamera, imagePointGroups, randomGenerator, index0, index1, iterationPoses, iterationPoseIndices, iterationObjectPoints, iterationObjectPointIndices, minimalValidObjectPoints, maximalSqrError))
		{
			ocean_assert(iterationPoses.size() == iterationPoseIndices.size());
			ocean_assert(iterationObjectPoints.size() == iterationObjectPointIndices.size());

			// our target is to find several object points visible in several camera poses
			// bad: 2 poses, large number of object points
			// good: several poses, several object points
			// bad: several poses, a small number of object points
			// therefore, we take the product of the number of poses and object points as measure for a good result

			if (iterationPoses.size() * iterationObjectPoints.size() >= poses.size() * objectPoints.size())
			{
				Scalar pointDistance = 0;

				for (size_t i = 0u; i < iterationPoses.size(); ++i)
				{
					const Index32 poseIndex = iterationPoseIndices[i];

					const Vectors2 iterationImagePoints = Subset::subset(imagePointGroups[poseIndex], iterationObjectPointIndices);
					pointDistance += averagePointDistance(iterationImagePoints.data(), iterationImagePoints.size());
				}

				pointDistance /= Scalar(iterationPoses.size());

				// we use the 'point sparsity' of the image points to find image points with large separation (as separated points will provide better than points close to each other)
				if (iterationPoses.size() * iterationObjectPoints.size() > poses.size() * objectPoints.size() || pointDistance > bestPointDistance)
				{
					// now we finally can ensure that all intermediate poses (which have not been investigated) can be determined (than the subset of poses was not representative and we have to select other ones)

					bool allPosesValid = true;
					if (database && keyFrameIds && objectPointIds)
					{
						Indices32 poseIds = Subset::subset(*keyFrameIds, iterationPoseIndices);
						std::sort(poseIds.begin(), poseIds.end());

						const Indices32 iterationObjectPointIds = Subset::subset(*objectPointIds, iterationObjectPointIndices);

						for (unsigned int poseId = poseIds.front() + 1u; (!abort || !*abort) && allPosesValid && poseId < poseIds.back(); ++poseId)
						{
							Scalar finalSqrError = Numeric::maxValue();
							if (determinePose(*database, AnyCameraPinhole(pinholeCamera), randomGenerator, poseId, ConstArrayAccessor<Vector3>(iterationObjectPoints), ConstArrayAccessor<Index32>(iterationObjectPointIds), HomogenousMatrix4(false), Geometry::Estimator::ET_SQUARE, Scalar(0.9), maximalSqrError, &finalSqrError).isNull() || finalSqrError * 2 > maximalSqrError)
							{
								allPosesValid = false;
							}
						}
					}

					if (allPosesValid)
					{
						bestPointDistance = pointDistance;

						objectPoints = std::move(iterationObjectPoints);
						poses = std::move(iterationPoses);

						validPoseIndices = std::move(iterationPoseIndices);
						validObjectPointIndices = std::move(iterationObjectPointIndices);
					}
				}
			}
		}
	}

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		const Vector3& objectPoint = objectPoints[n];
		const Index32 imagePointIndex = validObjectPointIndices[n];

		for (size_t i = 0; i < poses.size(); ++i)
		{
			const Index32 poseIndex = validPoseIndices[i];
			const Vector2& imagePoint = imagePointGroups[poseIndex][imagePointIndex];

			const HomogenousMatrix4& pose = poses[i];

			const Vector2 projectedObjectPoint(pinholeCamera.projectToImage<false>(pose, objectPoint, pinholeCamera.hasDistortionParameters()));
			const Scalar sqrError = imagePoint.sqrDistance(projectedObjectPoint);

			ocean_assert(sqrError < maximalSqrError * 2);
		}
	}
#endif

	return poses.size() >= 2 && (!abort || !*abort);
}

bool Solver3::determineInitialObjectPointsFromDenseFramesRANSAC(const PinholeCamera& pinholeCamera, const ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, HomogenousMatrices4& validPoses, Indices32& validPoseIds, Vectors3& objectPoints, Indices32& validObjectPointIndices, const unsigned int iterations, const RelativeThreshold& minimalValidObjectPoints, const Scalar maximalSqrError, Worker* worker, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(imagePointGroups.size() >= 2);

	ocean_assert(validPoses.empty() && validPoseIds.empty() && objectPoints.empty() && validObjectPointIndices.empty());

	validPoses.clear();
	validPoseIds.clear();
	objectPoints.clear();
	validObjectPointIndices.clear();

	Scalar totalError = Numeric::maxValue();

	unsigned int remainingIterations = iterations;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&determineInitialObjectPointsFromDenseFramesRANSACSubset, &pinholeCamera, &imagePointGroups, &randomGenerator, &validPoses, &validPoseIds, &objectPoints, &validObjectPointIndices, &totalError, &minimalValidObjectPoints, maximalSqrError, &remainingIterations, &lock, abort, 0u, 0u), 0u, iterations);
	}
	else
		determineInitialObjectPointsFromDenseFramesRANSACSubset(&pinholeCamera, &imagePointGroups, &randomGenerator, &validPoses, &validPoseIds, &objectPoints, &validObjectPointIndices, &totalError, &minimalValidObjectPoints, maximalSqrError, &remainingIterations, nullptr, abort, 0u, iterations);

	return validPoseIds.size() >= 2 && (!abort || !*abort);
}

bool Solver3::determineInitialObjectPointsFromSparseKeyFrames(const PinholeCamera& pinholeCamera, const Database::ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, const unsigned int firstGroupIndex, const unsigned int secondGroupIndex, HomogenousMatrices4& poses, Indices32& validPoseIndices, Vectors3& objectPoints, Indices32& validObjectPointIndices, const RelativeThreshold& minimalValidObjectPoints, const Scalar maximalSqrError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(imagePointGroups.size() >= 2);

	ocean_assert(firstGroupIndex != secondGroupIndex);
	ocean_assert(firstGroupIndex < imagePointGroups.size());
	ocean_assert(secondGroupIndex < imagePointGroups.size());

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < imagePointGroups.size(); ++n)
	{
		ocean_assert(imagePointGroups[0].size() == imagePointGroups[n].size());

		for (size_t i = 0; i < imagePointGroups[n].size(); ++i)
		{
			ocean_assert(pinholeCamera.isInside(imagePointGroups[n][i]));
		}
	}
#endif

	const Vectors2& points0 = imagePointGroups[firstGroupIndex];
	const Vectors2& points1 = imagePointGroups[secondGroupIndex];
	ocean_assert(points0.size() == points1.size());

	if (points0.size() < 6)
	{
		return false;
	}

	// first we determine the locations of 3D object points matching to the given set of 2D image points correspondences

	HomogenousMatrix4 secondPose(false);
	Vectors3 validObjectPoints;
	Indices32 validCorrespondenceIndices;
	if (!Geometry::StereoscopicGeometry::cameraPose(pinholeCamera, ConstArrayAccessor<Vector2>(points0), ConstArrayAccessor<Vector2>(points1), randomGenerator, secondPose, &validObjectPoints, &validCorrespondenceIndices, Scalar(1.5 * 1.5), Scalar(3.5 * 3.5), 10u, Scalar(0.9)))
	{
		return false;
	}

	ocean_assert(validObjectPoints.size() == validCorrespondenceIndices.size());

	// we stop here we could not determine enough 3D object point locations (e.g., due to invalid correspondences, or dynamic object points)

	unsigned int absoluteMinimalValidObjectPoints;
	if (!minimalValidObjectPoints.hasValidThreshold<5u>((unsigned int)points0.size(), &absoluteMinimalValidObjectPoints))
	{
		return false;
	}

	if (validObjectPoints.size() < absoluteMinimalValidObjectPoints)
	{
		return false;
	}

	ocean_assert(poses.empty());
	poses.clear();
	poses.reserve(imagePointGroups.size());

	ocean_assert(validPoseIndices.empty());
	validPoseIndices.clear();
	validPoseIndices.reserve(imagePointGroups.size());

	poses.push_back(HomogenousMatrix4(true));
	poses.push_back(secondPose);

	validPoseIndices.push_back(firstGroupIndex);
	validPoseIndices.push_back(secondGroupIndex);

#ifdef OCEAN_DEBUG
	{
		const Vectors2 validPoints0 = Subset::subset(points0, validCorrespondenceIndices);
		const Vectors2 validPoints1 = Subset::subset(points1, validCorrespondenceIndices);
		const Scalar error0 = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(poses[0], pinholeCamera, ConstTemplateArrayAccessor<Vector3>(validObjectPoints), ConstTemplateArrayAccessor<Vector2>(validPoints0), pinholeCamera.hasDistortionParameters());
		const Scalar error1 = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, false>(poses[1], pinholeCamera, ConstTemplateArrayAccessor<Vector3>(validObjectPoints), ConstTemplateArrayAccessor<Vector2>(validPoints1), pinholeCamera.hasDistortionParameters());
		ocean_assert(error0 <= maximalSqrError && error1 <= maximalSqrError);
	}
#endif

	// now determine the accuracy of the remaining poses (not for the two poses which we have used to determine the object point locations)

	std::vector<IndexSet32> remainingValidObjectPointIndexGroups(imagePointGroups.size());
	HomogenousMatrices4 remainingValidPoses(imagePointGroups.size(), HomogenousMatrix4(false));
	IndexSet32 remainingIndices;

	for (unsigned int index = 0u; index < imagePointGroups.size(); ++index)
	{
		if (index != firstGroupIndex && index != secondGroupIndex)
		{
			Indices32 subsetValidObjectPointIndices;
			HomogenousMatrix4 pose;

			if (Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(validObjectPoints), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePointGroups[index], validCorrespondenceIndices), randomGenerator, pose, 5u, true, 50u, maximalSqrError, &subsetValidObjectPointIndices))
			{
				if (subsetValidObjectPointIndices.size() < absoluteMinimalValidObjectPoints)
				{
					break;
				}

				ocean_assert(remainingValidObjectPointIndexGroups[index].empty());
				for (Indices32::const_iterator i = subsetValidObjectPointIndices.begin(); i != subsetValidObjectPointIndices.end(); ++i)
				{
					remainingValidObjectPointIndexGroups[index] = IndexSet32(subsetValidObjectPointIndices.begin(), subsetValidObjectPointIndices.end());
				}

				remainingValidPoses[index] = pose;
				remainingIndices.insert(index);
			}
		}
	}

	// now we iteratively select the best pose with most valid correspondences

	// the set of indices which hold all indices of valid object points from the stereoscopic pose determination step (so we take all)
	IndexSet32 validObjectPointIndexSet;
	for (unsigned int n = 0; n < validObjectPoints.size(); ++n)
	{
		validObjectPointIndexSet.insert(n);
	}

	while (!remainingIndices.empty())
	{
		unsigned int bestIndex = (unsigned int)(-1);
		size_t bestCorrespondences = 0u;

		// find the group of remaining indices with most entries
		for (IndexSet32::const_iterator i = remainingIndices.begin(); i != remainingIndices.end(); ++i)
		{
			if (remainingValidObjectPointIndexGroups[*i].size() > bestCorrespondences)
			{
				bestIndex = *i;
				bestCorrespondences = remainingValidObjectPointIndexGroups[*i].size();
			}
		}

		if (bestCorrespondences < absoluteMinimalValidObjectPoints)
		{
			break;
		}

		// determine the intersection between both sets of correspondence indices

		IndexSet32 unionCorrespondences;
		std::set_intersection(remainingValidObjectPointIndexGroups[bestIndex].begin(), remainingValidObjectPointIndexGroups[bestIndex].end(), validObjectPointIndexSet.begin(), validObjectPointIndexSet.end(), std::inserter(unionCorrespondences, unionCorrespondences.begin()));
		ocean_assert(unionCorrespondences.size() <= remainingValidObjectPointIndexGroups[bestIndex].size());

		poses.push_back(remainingValidPoses[bestIndex]);
		validPoseIndices.push_back(bestIndex);

		// we remove the handled key frame index and replaced the set of valid object point indices (so that we can take the new set for the next remaining poses in the next iteration)

		remainingIndices.erase(bestIndex);
		validObjectPointIndexSet = std::move(unionCorrespondences);

		// now we have to update all sets of remaining correspondence indices so that each remaining set does not contain any index which is not in our target set

		for (IndexSet32::const_iterator i = remainingIndices.begin(); i != remainingIndices.end(); ++i)
		{
			ocean_assert(unionCorrespondences.empty());
			std::set_intersection(remainingValidObjectPointIndexGroups[*i].begin(), remainingValidObjectPointIndexGroups[*i].end(), validObjectPointIndexSet.begin(), validObjectPointIndexSet.end(), std::inserter(unionCorrespondences, unionCorrespondences.begin()));
			remainingValidObjectPointIndexGroups[*i] = std::move(unionCorrespondences);
		}
	}

	ocean_assert(validPoseIndices.size() == poses.size());

	ocean_assert(objectPoints.empty());
	objectPoints.clear();
	objectPoints.reserve(validObjectPointIndexSet.size());

	ocean_assert(validObjectPointIndices.empty());
	validObjectPointIndices.clear();
	validObjectPointIndices.reserve(validObjectPointIndexSet.size());

	for (IndexSet32::const_iterator i = validObjectPointIndexSet.begin(); i != validObjectPointIndexSet.end(); ++i)
	{
		validObjectPointIndices.push_back(validCorrespondenceIndices[*i]);
		objectPoints.push_back(validObjectPoints[*i]);
	}

	ocean_assert(validObjectPointIndices.size() == objectPoints.size());
	ocean_assert(validObjectPointIndices.size() >= absoluteMinimalValidObjectPoints);

#ifdef OCEAN_DEBUG
	for (size_t p = 0; p < poses.size(); ++p)
	{
		const HomogenousMatrix4& pose = poses[p];
		const Index32 poseIndex = validPoseIndices[p];

		Scalar totalError = 0;

		for (size_t o = 0; o < validObjectPointIndices.size(); ++o)
		{
			const Vector3& objectPoint = objectPoints[o];
			const Index32 objectPointIndex = validObjectPointIndices[o];

			const Vector2 projectedObjectPoint = pinholeCamera.projectToImage<false>(pose, objectPoint, pinholeCamera.hasDistortionParameters());

			const Vector2& imagePoint = imagePointGroups[poseIndex][objectPointIndex];
			const Scalar errorValue = projectedObjectPoint.sqrDistance(imagePoint);

			totalError += errorValue;
		}

		ocean_assert(totalError < maximalSqrError * Scalar(2 * validObjectPointIndices.size()));
	}
#endif // OCEAN_DEBUG

	return true;
}

bool Solver3::determineInitialObjectPointsFromDenseFrames(const PinholeCamera& pinholeCamera, const ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, const unsigned int firstGroupIndex, const unsigned int secondGroupIndex, HomogenousMatrices4& validPoses, Indices32& validPoseIds, Scalar& totalSqrError, Vectors3& objectPoints, Indices32& validObjectPointIndices, const RelativeThreshold& minimalValidObjectPoints, const Scalar maximalSqrError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(imagePointGroups.size() >= 2);

	ocean_assert(firstGroupIndex != secondGroupIndex);
	ocean_assert(imagePointGroups.isValidIndex(firstGroupIndex));
	ocean_assert(imagePointGroups.isValidIndex(secondGroupIndex));

#ifdef OCEAN_DEBUG
	for (ImagePointGroups::Index n = imagePointGroups.firstIndex(); n <= imagePointGroups.lastIndex(); ++n)
	{
		ocean_assert(imagePointGroups[n].size() == imagePointGroups.front().size());

		for (size_t i = 0; i < imagePointGroups[n].size(); ++i)
		{
			ocean_assert(pinholeCamera.isInside(imagePointGroups[n][i]));
		}
	}
#endif

	const Vectors2& points0 = imagePointGroups[firstGroupIndex];
	const Vectors2& points1 = imagePointGroups[secondGroupIndex];
	ocean_assert(points0.size() == points1.size());

	ocean_assert(minimalValidObjectPoints.lowerBoundary() >= 5u);

	if (points0.size() < min(5u, minimalValidObjectPoints.lowerBoundary()))
	{
		return false;
	}

	HomogenousMatrix4 secondPose(false);
	if (!Geometry::StereoscopicGeometry::cameraPose(pinholeCamera, ConstArrayAccessor<Vector2>(points0), ConstArrayAccessor<Vector2>(points1), randomGenerator, secondPose, &objectPoints, &validObjectPointIndices, Scalar(1.5 * 1.5), Scalar(3.5 * 3.5), 100u, Scalar(0.9)))
	{
		return false;
	}

	ocean_assert(objectPoints.size() == validObjectPointIndices.size());

	unsigned int absoluteMinimalValidObjectPoints;
	if (!minimalValidObjectPoints.hasValidThreshold<5u>((unsigned int)points0.size(), &absoluteMinimalValidObjectPoints))
	{
		return false;
	}

	if (objectPoints.size() < absoluteMinimalValidObjectPoints)
	{
		return false;
	}

	// now we check the number of poses which are valid for the found object points

	Indices32 validSubsetIndices;

	if (validObjectPointIndices.size() != points0.size())
	{
		ImagePointGroups subsetImagePointGroups(imagePointGroups.firstIndex(), imagePointGroups.size());

		for (ImagePointGroups::Index n = imagePointGroups.firstIndex(); n <= imagePointGroups.lastIndex(); ++n)
		{
			subsetImagePointGroups[n] = Subset::subset(imagePointGroups[n], validObjectPointIndices);
		}

		determineValidPoses(AnyCameraPinhole(pinholeCamera), objectPoints, subsetImagePointGroups, randomGenerator, Solver3::CM_UNKNOWN, firstGroupIndex, HomogenousMatrix4(true), secondGroupIndex, secondPose, Scalar(0.5), maximalSqrError * 2, &validSubsetIndices, &validPoses, &validPoseIds, &totalSqrError);
	}
	else
	{
		determineValidPoses(AnyCameraPinhole(pinholeCamera), objectPoints, imagePointGroups, randomGenerator, Solver3::CM_UNKNOWN, firstGroupIndex, HomogenousMatrix4(true), secondGroupIndex, secondPose, Scalar(0.5), maximalSqrError * 2, &validSubsetIndices, &validPoses, &validPoseIds, &totalSqrError);
	}

	objectPoints = Subset::subset(objectPoints, validSubsetIndices);
	validObjectPointIndices = Subset::subset(validObjectPointIndices, validSubsetIndices);

	return true;
}

bool Solver3::optimizeInitialObjectPoints(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const Vectors3& initialObjectPoints, const Indices32& initialObjectPointIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, const unsigned int minimalObjectPoints, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Indices32* usedPoseIds, Scalar* initialSqrError, Scalar* finalSqrError, bool* abort)
{
	ocean_assert(camera.isValid());
	ocean_assert(lowerFrame < upperFrame);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

	ocean_assert(minimalKeyFrames >= 2u);
	ocean_assert(minimalKeyFrames <= initialObjectPoints.size());

	ocean_assert(minimalObjectPoints >= 5u);
	ocean_assert(initialObjectPoints.size() == initialObjectPointIds.size());

	if ((abort != nullptr && *abort) || !camera.isValid() || initialObjectPoints.size() != initialObjectPointIds.size() || minimalObjectPoints > initialObjectPoints.size())
	{
		return false;
	}

	// first we track the already known (rough) 3D object points from frame to frame

	Indices32 objectPointIds;
	ImagePointGroups imagePointGroups;
	Indices32 validIndices;
	if (!trackObjectPoints(database, initialObjectPointIds, lowerFrame, startFrame, upperFrame, minimalObjectPoints, 0u, (unsigned int)initialObjectPointIds.size(), objectPointIds, imagePointGroups, &validIndices, abort))
	{
		return false;
	}

	ocean_assert(imagePointGroups.isValidIndex(startFrame));

	// ensure that we have enough keyframes
	if (imagePointGroups.size() < minimalKeyFrames)
	{
		return false;
	}

	ocean_assert(imagePointGroups.isValidIndex(startFrame));
	ocean_assert(imagePointGroups.firstIndex() >= ImagePointGroups::Index(lowerFrame) && imagePointGroups.lastIndex() <= ImagePointGroups::Index(upperFrame));
	ocean_assert(imagePointGroups[startFrame].size() >= minimalObjectPoints);

	Vectors3 objectPoints = Subset::subset(initialObjectPoints, validIndices);

	// now we determine the (rough) poses for the given 2D and 3D point correspondences for all frames that have been used for point tracking

	ShiftVector<HomogenousMatrix4> poseMatrices(imagePointGroups.firstIndex(), imagePointGroups.size());

	HomogenousMatrix4 initialPose;
	if (!Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePointGroups[startFrame]), randomGenerator, initialPose, 5u, false)
			|| !Geometry::NonLinearOptimizationPose::optimizePose(camera, initialPose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePointGroups[startFrame]), poseMatrices[startFrame], 20u, Geometry::Estimator::ET_SQUARE))
	{
		return false;
	}

	// forward steps
	for (ImagePointGroups::Index n = startFrame + 1; n < imagePointGroups.endIndex(); ++n)
	{
		ocean_assert(objectPoints.size() == imagePointGroups[n].size());

		ocean_assert(poseMatrices[n - 1].isValid());
		if (!Geometry::NonLinearOptimizationPose::optimizePose(camera, poseMatrices[n - 1], ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePointGroups[n]), poseMatrices[n], 20u, Geometry::Estimator::ET_SQUARE))
		{
			return false;
		}
	}

	// backward steps
	for (ImagePointGroups::Index n = ImagePointGroups::Index(startFrame) - 1; n >= imagePointGroups.firstIndex(); --n)
	{
		ocean_assert(objectPoints.size() == imagePointGroups[n].size());

		ocean_assert(poseMatrices[n + 1].isValid());
		if (!Geometry::NonLinearOptimizationPose::optimizePose(camera, poseMatrices[n + 1], ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePointGroups[n]), poseMatrices[n], 20u, Geometry::Estimator::ET_SQUARE))
		{
			return false;
		}
	}

#ifdef OCEAN_DEBUG
	for (ImagePointGroups::Index n = imagePointGroups.firstIndex(); n < imagePointGroups.endIndex(); ++n)
	{
		Scalar sqrAverage = Numeric::maxValue(), sqrMinimal = Numeric::maxValue(), sqrMaximal = Numeric::maxValue();
		Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(poseMatrices[n], camera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), sqrAverage, sqrMinimal, sqrMaximal);
		ocean_assert(sqrAverage < Scalar(9 * 9));
	}
#endif // OCEAN_DEBUG

	// now we determine the most representative poses for the optimization (beware: the indices are relative w.r.t. the first index of 'poseMatrices')

	const HomogenousMatrices4 poseMatricesBlock(poseMatrices.data());
	Indices32 keyframeIndicesBlock = Pose::representativePoses(poseMatricesBlock.data(), poseMatricesBlock.size(), maximalKeyFrames);
	ocean_assert(keyframeIndicesBlock.size() >= minimalKeyFrames && keyframeIndicesBlock.size() <= maximalKeyFrames);

	// now we optimize the 3D object points and the 6DOF camera poses of the keyframes concurrently

	HomogenousMatrices4 optimizedPoses(keyframeIndicesBlock.size());
	NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessor(optimizedPoses);

	optimizedObjectPoints.resize(objectPoints.size());
	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(camera, ConstArraySubsetAccessor<HomogenousMatrix4, unsigned int>(poseMatricesBlock, keyframeIndicesBlock), ConstArrayAccessor<Vector3>(objectPoints), ObjectPointToPoseIndexImagePointCorrespondenceAccessor(imagePointGroups, keyframeIndicesBlock), &optimizedPosesAccessor, &optimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, initialSqrError, finalSqrError))
	{
		return false;
	}

	optimizedObjectPointIds = Subset::subset(initialObjectPointIds, validIndices);

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < keyframeIndicesBlock.size(); ++n)
	{
		const Index32 poseId = (unsigned int)poseMatrices.firstIndex() + keyframeIndicesBlock[n];

		Scalar sqrAverage = Numeric::maxValue(), sqrMinimal = Numeric::maxValue(), sqrMaximal = Numeric::maxValue();
		Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(optimizedPoses[n], camera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[poseId]), sqrAverage, sqrMinimal, sqrMaximal);
		ocean_assert(sqrAverage < Scalar(9 * 9));
	}
#endif

	// now we check the accuracy of all 3D object points and exclude the object points which are not accurate enough

	std::vector<unsigned char> invalidObjectPoints(optimizedObjectPoints.size(), 0u);
	unsigned int invalidObjectPointCounter = 0u;

	for (size_t n = 0; n < optimizedPoses.size(); ++n)
	{
		const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(optimizedPoses[n]));

		const Vectors2& imagePoints = imagePointGroups[imagePointGroups.firstIndex() + keyframeIndicesBlock[n]];
		ocean_assert(imagePoints.size() == optimizedObjectPoints.size());

		for (size_t i = 0; i < optimizedObjectPoints.size(); ++i)
		{
			if (invalidObjectPoints[i] == 0u)
			{
				const Vector2 projectedObjectPoint(camera.projectToImageIF(flippedCamera_T_world, optimizedObjectPoints[i]));
				const Scalar sqrError = projectedObjectPoint.sqrDistance(imagePoints[i]);

				if (sqrError > maximalSqrError)
				{
					invalidObjectPoints[i] = 1u;
					invalidObjectPointCounter++;
				}
			}
		}
	}

	if (invalidObjectPointCounter != 0u)
	{
		ocean_assert(invalidObjectPointCounter < objectPoints.size());

		// now we optimized the remaining object points if enough object points are left

		if (objectPoints.size() - invalidObjectPointCounter < 5u)
		{
			return false;
		}

		Indices32 objectPointsSubset;
		objectPointsSubset.reserve(objectPoints.size() - invalidObjectPointCounter);

		for (unsigned int n = 0; n < invalidObjectPoints.size(); ++n)
		{
			if (invalidObjectPoints[n] == 0u)
			{
				objectPointsSubset.push_back(n);
			}
		}

		objectPoints = optimizedObjectPoints;

#ifdef OCEAN_DEBUG
		for (unsigned int p = 0u; p < keyframeIndicesBlock.size(); ++p)
		{
			const Index32 poseId = (unsigned int)imagePointGroups.firstIndex() + keyframeIndicesBlock[p];
			const HomogenousMatrix4& world_T_camera = optimizedPoses[p];

			for (unsigned int i = 0u; i < objectPointsSubset.size(); ++i)
			{
				const Vectors2& imagePoints = imagePointGroups[poseId];
				const Vector2& imagePoint = imagePoints[objectPointsSubset[i]];
				const Vector3& objectPoint = objectPoints[objectPointsSubset[i]];

				const Vector2 projectedObjectPoint = camera.projectToImage(world_T_camera, objectPoint);
				const Scalar sqrDistance = imagePoint.sqrDistance(projectedObjectPoint);

				ocean_assert(sqrDistance <= maximalSqrError);
			}
		}
#endif

		HomogenousMatrices4 subsetOptimizedPoses(keyframeIndicesBlock.size());
		NonconstArrayAccessor<HomogenousMatrix4> subsetOptimizedPosesAccessor(subsetOptimizedPoses);

		optimizedObjectPoints.resize(objectPointsSubset.size());
		NonconstArrayAccessor<Vector3> subsetOptimizedObjectPointAccessor(optimizedObjectPoints);

		if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(camera, ConstArrayAccessor<HomogenousMatrix4>(optimizedPoses), ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints, objectPointsSubset), ObjectPointToPoseIndexImagePointCorrespondenceAccessor(imagePointGroups, keyframeIndicesBlock, objectPointsSubset), &subsetOptimizedPosesAccessor, &subsetOptimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, initialSqrError, finalSqrError))
		{
			return false;
		}

		optimizedObjectPointIds = Subset::subset(optimizedObjectPointIds, objectPointsSubset);
		optimizedPoses = std::move(subsetOptimizedPoses);
	}

#ifdef OCEAN_DEBUG
	// ensure that we have a correct set of poses, image points and object points
	for (size_t o = 0; o < optimizedObjectPointIds.size(); ++o)
	{
		const unsigned int objectPointId = optimizedObjectPointIds[o];

		const Vector3& objectPoint = optimizedObjectPoints[o];

		for (size_t p = 0; p < keyframeIndicesBlock.size(); ++p)
		{
			const unsigned int poseId = (unsigned int)poseMatrices.firstIndex() + keyframeIndicesBlock[p];

			const HomogenousMatrix4& world_T_camera = optimizedPoses[p];
			const Vector2 projectedImagePoint(camera.projectToImage(world_T_camera, objectPoint));

			Vector2 imagePoint;
			ocean_assert(database.hasObservation<false>(poseId, objectPointId, &imagePoint));

			const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);
			ocean_assert(sqrError < maximalSqrError);
		}
	}
#endif

	// finally we check whether we have intermediate poses (which have not been used for bundle adjustment) which should now be added to a final bundle adjustment step
	// therefore we measure the average error in the optimized poses and add poses with a 'significant larger' error

	// first we gather all image points groups and determine the current camera poses

	ImagePointGroups optimizedImagePointGroups(poseMatrices.firstIndex(), poseMatrices.size());
	ShiftVector<Scalar> optimizedErrors(poseMatrices.firstIndex(), poseMatrices.size());

	bool validFinalOptimization = true;
	HomogenousMatrix4 previousPose(false);
	for (ShiftVector<HomogenousMatrix4>::Index n = poseMatrices.firstIndex(); n < poseMatrices.endIndex(); ++n)
	{
		const Index32 poseId = (unsigned int)n;

		Indices32 currentValidIndices;
		Vectors2 imagePoints = database.imagePointsFromObjectPoints<false>(poseId, optimizedObjectPointIds, currentValidIndices);
		ocean_assert(imagePoints.size() == optimizedObjectPoints.size());

		if (!previousPose.isValid())
		{
			Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(optimizedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, previousPose, 5u, false);
		}

		// we must initialize these variable to avoid an compiler warning
		HomogenousMatrix4 currentPose(false);
		Scalar currentError = Numeric::minValue();
		if (!previousPose.isValid() || !Geometry::NonLinearOptimizationPose::optimizePose(camera, previousPose, ConstArrayAccessor<Vector3>(optimizedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints), currentPose, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10), nullptr, &currentError))
		{
			validFinalOptimization = false;
			break;
		}

		ocean_assert(currentPose.isValid());
		ocean_assert(currentError != Numeric::minValue());

		poseMatrices[n] = currentPose;
		optimizedImagePointGroups[n] = std::move(imagePoints);
		optimizedErrors[n] = currentError;
	}

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < keyframeIndicesBlock.size(); ++n)
	{
		const Index32 poseId = (unsigned int)poseMatrices.firstIndex() + keyframeIndicesBlock[n];

		Scalar sqrAverage = Numeric::maxValue(), sqrMinimal = Numeric::maxValue(), sqrMaximal = Numeric::maxValue();
		Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>>(poseMatrices[poseId], camera, ConstTemplateArrayAccessor<Vector3>(optimizedObjectPoints), ConstTemplateArrayAccessor<Vector2>(optimizedImagePointGroups[poseId]), sqrAverage, sqrMinimal, sqrMaximal);
		ocean_assert(sqrAverage < Scalar(9 * 9));
	}
#endif

	ocean_assert(validFinalOptimization);

	if (validFinalOptimization)
	{
		const IndexSet32 keyframeIndicesBlockSet(keyframeIndicesBlock.begin(), keyframeIndicesBlock.end());

		PoseErrorPairs remainingErrors;
		remainingErrors.reserve(poseMatrices.size());

		// we determine the maximal error of the poses which have been used for the previous optimization and sort all remaining poses due to their individual errors
		Scalar maxAverageSqrError = 0;
		for (unsigned int n = 0u; n < poseMatrices.size(); ++n)
		{
			const unsigned int poseId = n + (unsigned int)poseMatrices.firstIndex();

			if (keyframeIndicesBlockSet.find(n) != keyframeIndicesBlockSet.end())
			{
				maxAverageSqrError = max(maxAverageSqrError, optimizedErrors[poseId]);
			}
			else
			{
				remainingErrors.push_back(PoseErrorPair(n, optimizedErrors[poseId])); // we take the index and not the id as the index is needed later
			}
		}

		// now we identify all poses which should also be used for optimization (we add a representative subset of all inaccurate poses)
		std::sort(remainingErrors.rbegin(), remainingErrors.rend(), Ocean::Utilities::sortPairSecond<PoseErrorPair>);

		HomogenousMatrices4 inaccuratePoses;
		inaccuratePoses.reserve(remainingErrors.size());

		for (PoseErrorPairs::const_iterator i = remainingErrors.begin(); i != remainingErrors.end(); ++i)
		{
			if (i->second < maxAverageSqrError * 2)
			{
				break;
			}

			inaccuratePoses.push_back(poseMatrices[i->first + (unsigned int)poseMatrices.firstIndex()]);
		}

		Indices32 newKeyframeIndicesBlock(keyframeIndicesBlock);
		if (!inaccuratePoses.empty())
		{
			remainingErrors = Subset::subset(remainingErrors, Pose::representativePoses(inaccuratePoses.data(), inaccuratePoses.size(), min(inaccuratePoses.size(), keyframeIndicesBlock.size())));

			for (PoseErrorPairs::const_iterator i = remainingErrors.begin(); i != remainingErrors.end(); ++i)
			{
				newKeyframeIndicesBlock.push_back(i->first);
				optimizedPoses.push_back(poseMatrices[i->first + (unsigned int)poseMatrices.firstIndex()]);
			}
		}

		ocean_assert(newKeyframeIndicesBlock.size() <= keyframeIndicesBlock.size() * 2);

		// check whether we found new poses so that we have to invoke the optimization again
		if (newKeyframeIndicesBlock.size() > keyframeIndicesBlock.size())
		{
			HomogenousMatrices4 subsetOptimizedPoses(newKeyframeIndicesBlock.size());
			NonconstArrayAccessor<HomogenousMatrix4> subsetOptimizedPosesAccessor(subsetOptimizedPoses);

			objectPoints = optimizedObjectPoints;
			NonconstArrayAccessor<Vector3> subsetOptimizedObjectPointAccessor(optimizedObjectPoints);

			if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(camera, ConstArrayAccessor<HomogenousMatrix4>(optimizedPoses), ConstArrayAccessor<Vector3>(objectPoints), ObjectPointToPoseIndexImagePointCorrespondenceAccessor(optimizedImagePointGroups, newKeyframeIndicesBlock), &subsetOptimizedPosesAccessor, &subsetOptimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, initialSqrError, finalSqrError))
			{
				return false;
			}

			optimizedPoses = std::move(subsetOptimizedPoses);
		}

#ifdef OCEAN_DEBUG
		// ensure that we have a correct set of poses, image points and object points
		for (size_t o = 0; o < optimizedObjectPointIds.size(); ++o)
		{
			const unsigned int objectPointId = optimizedObjectPointIds[o];

			const Vector3& objectPoint = optimizedObjectPoints[o];

			for (size_t p = 0; p < newKeyframeIndicesBlock.size(); ++p)
			{
				const unsigned int poseId = (unsigned int)poseMatrices.firstIndex() + newKeyframeIndicesBlock[p];

				const HomogenousMatrix4& world_T_camera = optimizedPoses[p];
				const Vector2 projectedImagePoint(camera.projectToImage(world_T_camera, objectPoint));

				Vector2 imagePoint;
				ocean_assert(database.hasObservation<false>(poseId, objectPointId, &imagePoint));

				const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);
				ocean_assert(sqrError < maximalSqrError);
			}
		}
#endif

		keyframeIndicesBlock = std::move(newKeyframeIndicesBlock);
	}

	if (usedPoseIds != nullptr)
	{
		usedPoseIds->clear();

		ocean_assert(poseMatrices.firstIndex() >= 0);
		for (Indices32::const_iterator i = keyframeIndicesBlock.begin(); i != keyframeIndicesBlock.end(); ++i)
		{
			usedPoseIds->push_back((unsigned int)poseMatrices.firstIndex() + *i);
		}
	}

	return abort == nullptr || !*abort;
}

bool Solver3::determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const unsigned int lowerFrame, const unsigned int upperFrame, Vectors3& newObjectPoints, Indices32& newObjectPointIds, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Worker* worker, bool* abort)
{
	ocean_assert(camera.isValid());

	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(minimalKeyFrames <= maximalKeyFrames);
	ocean_assert(minimalKeyFrames >= 2);

	// determine the set of representative poses between the lower frame and the upper frame
	const Indices32 representativePoses(determineRepresentativePoses(database, lowerFrame, upperFrame, maximalKeyFrames));

	if ((abort != nullptr && *abort) || representativePoses.size() < minimalKeyFrames)
	{
		return false;
	}

	// now we extract triples of (pose id, object point id, image point id)

	const Database::TopologyTriples topologyTriples(database.topologyTriples<false>(representativePoses));

	if (abort != nullptr && *abort)
	{
		return false;
	}

	// now we filter all object points (their triples respectively) which do not have enough observations (camera poses)

	const Indices32 reliableObjectPoints = Database::reliableObjectPoints(topologyTriples, minimalKeyFrames);

	if (abort != nullptr && *abort)
	{
		return false;
	}

	// now we extract all object points which do not have a valid position

	IndexSet32 objectPointIds;
	for (Indices32::const_iterator i = reliableObjectPoints.begin(); i != reliableObjectPoints.end(); ++i)
	{
		const Vector3& objectPoint = database.objectPoint<false>(*i);

		if (objectPoint == Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()))
		{
			objectPointIds.insert(*i);
		}
	}

	if (abort != nullptr && *abort)
	{
		return false;
	}

	// now we extract the indices of all triples which we will use to determine the new object point positions

	const Indices32 validTriples = Database::filterTopologyTriplesObjectPoints(topologyTriples, objectPointIds);

	if (abort != nullptr && *abort)
	{
		return false;
	}

	// now we re-organize the triples into a data structure which is object point oriented

	const Database::PoseImagePointTopologyGroups objectPointData = Database::objectPointTopology(topologyTriples, &validTriples);

	ocean_assert(newObjectPoints.empty() && newObjectPointIds.empty());
	newObjectPoints.clear();
	newObjectPointIds.clear();

	// now we determine the object point positions individually

	RandomGenerator randomGenerator;

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(determineUnknownObjectPointsSubset, &camera, &database, &objectPointData, &randomGenerator, maximalSqrError, abort, &lock, &newObjectPoints, &newObjectPointIds, 0u, 0u), 0u, (unsigned int)(objectPointData.size()));
	}
	else
	{
		determineUnknownObjectPointsSubset(&camera, &database, &objectPointData, &randomGenerator, maximalSqrError, abort, nullptr, &newObjectPoints, &newObjectPointIds, 0u, (unsigned int)(objectPointData.size()));
	}

	return abort == nullptr || !*abort;
}

bool Solver3::determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, const Indices32& unknownObjectPointIds, Vectors3& newObjectPoints, Indices32& newObjectPointIds, RandomGenerator& randomGenerator, Indices32* newObjectPointObservations, const unsigned int minimalObservations, const bool useAllObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar ransacMaximalSqrError, const Scalar averageRobustError, const Scalar maximalSqrError, Worker* worker, bool* abort)
{
	ocean_assert(cameraMotion != CM_INVALID);
	ocean_assert(newObjectPoints.empty() && newObjectPointIds.empty() && (!newObjectPointObservations || newObjectPointObservations->empty()));

	newObjectPoints.clear();
	newObjectPointIds.clear();

	if (newObjectPointObservations)
	{
		newObjectPointObservations->clear();
	}

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(determineUnknownObjectPointsSubset, &database, &camera, cameraMotion, unknownObjectPointIds.data(), &newObjectPoints, &newObjectPointIds, newObjectPointObservations, &randomGenerator, minimalObservations, useAllObservations, estimator, ransacMaximalSqrError, averageRobustError, maximalSqrError, &lock, abort, 0u, 0u), 0u, (unsigned int)(unknownObjectPointIds.size()));
	}
	else
	{
		determineUnknownObjectPointsSubset(&database, &camera, cameraMotion, unknownObjectPointIds.data(), &newObjectPoints, &newObjectPointIds, newObjectPointObservations, &randomGenerator, minimalObservations, useAllObservations, estimator, ransacMaximalSqrError, averageRobustError, maximalSqrError, nullptr, abort, 0u, (unsigned int)(unknownObjectPointIds.size()));
	}

	return !abort || !*abort;
}

bool Solver3::optimizeObjectPointsWithFixedPoses(const Database& database, const PinholeCamera& pinholeCamera, const CameraMotion cameraMotion, const Indices32& objectPointIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar maximalRobustError, Worker* worker, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(&objectPointIds != &optimizedObjectPointIds);
	ocean_assert(optimizedObjectPoints.empty() && optimizedObjectPointIds.empty());

	optimizedObjectPoints.clear();
	optimizedObjectPointIds.clear();

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(optimizeObjectPointsWithFixedPosesSubset, &database, &pinholeCamera, cameraMotion, objectPointIds.data(), &optimizedObjectPoints, &optimizedObjectPointIds, minimalObservations, estimator, maximalRobustError, &lock, abort, 0u, 0u), 0u, (unsigned int)objectPointIds.size());
	}
	else
		optimizeObjectPointsWithFixedPosesSubset(&database, &pinholeCamera, cameraMotion, objectPointIds.data(), &optimizedObjectPoints, &optimizedObjectPointIds, minimalObservations, estimator, maximalRobustError, nullptr, abort, 0u, (unsigned int)objectPointIds.size());

	return !abort || !*abort;
}

bool Solver3::optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses, Indices32* optimizedKeyFramePoseIds, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(minimalKeyFrames <= maximalKeyFrames);

	HomogenousMatrices4 poses;
	Indices32 poseIds = database.poseIds<false, false>(HomogenousMatrix4(false), &poses);
	const Indices32 representativePoseIndices = Pose::representativePoses(poses.data(), poses.size(), maximalKeyFrames);

	if (representativePoseIndices.size() < minimalKeyFrames)
	{
		return false;
	}

	Indices32 keyframePoseIds;

	if (representativePoseIndices.size() == poseIds.size())
	{
		keyframePoseIds = std::move(poseIds);
	}
	else
	{
		keyframePoseIds = Subset::subset(poseIds, representativePoseIndices);
	}

	if (!optimizeObjectPointsWithVariablePoses(database, pinholeCamera, keyframePoseIds, optimizedObjectPoints, optimizedObjectPointIds, optimizedKeyFramePoses, minimalObservations, estimator, iterations, initialRobustError, finalRobustError))
	{
		return false;
	}

	if (optimizedKeyFramePoseIds != nullptr)
	{
		*optimizedKeyFramePoseIds = std::move(keyframePoseIds);
	}

	return true;
}

bool Solver3::optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& keyFrameIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(keyFrameIds.size() >= 2);

	const Indices32 objectPointCandidateIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

	if (objectPointCandidateIds.empty())
	{
		return true;
	}

	return optimizeObjectPointsWithVariablePoses(database, pinholeCamera, keyFrameIds, objectPointCandidateIds, optimizedObjectPoints, optimizedObjectPointIds, optimizedKeyFramePoses, minimalObservations, estimator, iterations, initialRobustError, finalRobustError);
}

bool Solver3::optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& keyFrameIds, const Indices32& objectPointIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(objectPointIds.size() >= 1);
	ocean_assert(keyFrameIds.size() >= 2);

	Indices32 validObjectPointIndices;
	const ObjectPointToPoseImagePointCorrespondenceAccessor objectPointToPoseImagePointCorrespondenceAccessor(database, keyFrameIds, objectPointIds, minimalObservations, &validObjectPointIndices);

	if (validObjectPointIndices.empty())
		return false;

	optimizedObjectPointIds = Subset::subset(objectPointIds, validObjectPointIndices);
	const Vectors3 objectPoints(database.objectPoints<false>(optimizedObjectPointIds));

	const HomogenousMatrices4 keyFramePoses(database.poses<false>(keyFrameIds.data(), keyFrameIds.size()));

#ifdef OCEAN_DEBUG
	for (HomogenousMatrices4::const_iterator i = keyFramePoses.begin(); i != keyFramePoses.end(); ++i)
		ocean_assert(i->isValid());
#endif

	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints, objectPoints.size());
	NonconstArrayAccessor<HomogenousMatrix4> optimizedKeyFramePosesAccessor(optimizedKeyFramePoses, keyFrameIds.size());

	return Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<HomogenousMatrix4>(keyFramePoses), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, optimizedKeyFramePosesAccessor.pointer(), &optimizedObjectPointAccessor, iterations, estimator, Scalar(0.001), Scalar(5), true, initialRobustError, finalRobustError);
}

bool Solver3::optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, const Index32 lowerPoseId, const Index32 upperPoseId, const Indices32& objectPointIds, Indices32& usedKeyFrameIds, Vectors3& optimizedObjectPoints, const unsigned int minimalObservations, const unsigned int minimalKeyFrames, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(objectPointIds.size() >= 1);
	ocean_assert(minimalObservations >= 2u);
	ocean_assert(minimalKeyFrames >= 2u);

	const ObjectPointToPoseImagePointCorrespondenceAccessor objectPointToPoseImagePointCorrespondenceAccessor(database, lowerPoseId, upperPoseId, objectPointIds, minimalObservations, minimalKeyFrames, &usedKeyFrameIds);

	const Vectors3 objectPoints(database.objectPoints<false>(objectPointIds));

	const HomogenousMatrices4 keyFramePoses(database.poses<false>(usedKeyFrameIds.data(), usedKeyFrameIds.size()));

#ifdef OCEAN_DEBUG
	for (HomogenousMatrices4::const_iterator i = keyFramePoses.begin(); i != keyFramePoses.end(); ++i)
		ocean_assert(i->isValid());
#endif

	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints, objectPoints.size());

	return Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<HomogenousMatrix4>(keyFramePoses), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, nullptr, &optimizedObjectPointAccessor, iterations, estimator, Scalar(0.001), Scalar(5), true, initialRobustError, finalRobustError);
}

bool Solver3::supposeRotationalCameraMotion(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrame, const unsigned int upperFrame, const bool findInitialFieldOfView, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, Database& optimizedDatabase, const unsigned int minimalObservations, const unsigned int minimalKeyframes, const unsigned int maximalKeyframes, const Scalar lowerFovX, const Scalar upperFovX, const Scalar maximalSqrError, Worker* worker, bool* abort, Scalar* finalMeanSqrError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(minimalKeyframes >= 2u && minimalKeyframes <= maximalKeyframes);

	ocean_assert(lowerFovX >= Numeric::deg2rad(5) && lowerFovX <= upperFovX && upperFovX < Numeric::deg2rad(180));

	Database rotationalDatabase(database);

	// first we update the database: with object points matching to a sole rotational camera motion, and corresponding camera poses (without translation)

	IndexSet32 relocatedObjectPointIdSet;
	RandomGenerator randomGenerator;
	if (!updateDatabaseToRotationalMotion(rotationalDatabase, pinholeCamera, randomGenerator, lowerFrame, upperFrame, minimalObservations, &relocatedObjectPointIdSet))
	{
		return false;
	}

	// now we determine a subset of frame poses best representing the entire set of poses

	Indices32 keyframePoseIds(determineRepresentativePoses(rotationalDatabase, lowerFrame, upperFrame, maximalKeyframes));

	if (keyframePoseIds.size() < minimalKeyframes)
	{
		return false;
	}

	Indices32 relocatedObjectPointIds(relocatedObjectPointIdSet.begin(), relocatedObjectPointIdSet.end());

	Indices32 validSubsetPoseIndices, usedRelocatedObjectPointIndices;
	const PoseToObjectPointIdImagePointCorrespondenceAccessor orientationToObjectPointIdImagePointCorrespondenceAccessor(rotationalDatabase, keyframePoseIds, relocatedObjectPointIds, 10u, &validSubsetPoseIndices, &usedRelocatedObjectPointIndices);
	keyframePoseIds = Subset::subset(keyframePoseIds, validSubsetPoseIndices);

	SquareMatrices3 keyframeOrientations(rotationalDatabase.rotationalPoses<false>(keyframePoseIds.data(), keyframePoseIds.size()));

	PinholeCamera optimizedCameraFoV(pinholeCamera);
	SquareMatrices3 optimizedKeyframeOrientationsFoV(keyframeOrientations);

	if (findInitialFieldOfView)
	{
		// first we guess the best matching field of view of the camera, without modifying any other camera parameters

		bool significantResult = false;

		NonconstArrayAccessor<SquareMatrix3> optimizedOrientationAccessorFoV(optimizedKeyframeOrientationsFoV);
		if (!Geometry::NonLinearOptimizationCamera::findInitialFieldOfView(pinholeCamera, ConstArrayAccessor<SquareMatrix3>(keyframeOrientations), orientationToObjectPointIdImagePointCorrespondenceAccessor, optimizedCameraFoV, &optimizedOrientationAccessorFoV, lowerFovX, upperFovX, 8u, 3u, true, &significantResult, nullptr, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}

		if (significantResult)
		{
			Log::info() << "Rotational camera field of view: " << Numeric::rad2deg(optimizedCameraFoV.fovX()) << "deg (significant)";
		}
		else
		{
			Log::info() << "Rotational camera field of view: " << Numeric::rad2deg(optimizedCameraFoV.fovX()) << "deg (NOT SIGNIFICANT)";
		}

		// we reset the determined parameters if the result is not significant as the result may not be the correct one
		if (!significantResult)
		{
			optimizedCameraFoV = pinholeCamera;
			optimizedKeyframeOrientationsFoV = keyframeOrientations;
		}
	}

	Scalar finalError = Numeric::maxValue();
	PinholeCamera optimizedCameraDistortion(optimizedCameraFoV);

	if (optimizationStrategy == PinholeCamera::OS_NONE)
	{
		Indices32 objectPointIds(Subset::subset(relocatedObjectPointIds, usedRelocatedObjectPointIndices));

		Indices32 validObjectPointIndices;
		const ObjectPointToPoseImagePointCorrespondenceAccessor objectPointToPoseImagePointCorrespondenceAccessor(rotationalDatabase, keyframePoseIds, objectPointIds, 1u, &validObjectPointIndices);
		ocean_assert(validObjectPointIndices.size() == objectPointIds.size());

		const Vectors3 objectPoints = rotationalDatabase.objectPoints<false>(objectPointIds);

		if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientations(optimizedCameraFoV, ConstArrayAccessor<SquareMatrix3>(optimizedKeyframeOrientationsFoV), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, Scalar(1), nullptr, nullptr, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, nullptr, &finalError))
		{
			return false;
		}
	}
	else
	{
		if (!Geometry::NonLinearOptimizationCamera::optimizeCameraOrientations(optimizedCameraFoV, ConstArrayAccessor<SquareMatrix3>(optimizedKeyframeOrientationsFoV), orientationToObjectPointIdImagePointCorrespondenceAccessor, optimizationStrategy, optimizedCameraDistortion, nullptr, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, nullptr, &finalError))
		{
			return false;
		}
	}

	// check whether the final error is higher than the defined threshold so that we cannot expect to have a sole rotational camera motion
	if (finalError > maximalSqrError)
	{
		if (finalMeanSqrError != nullptr)
		{
			*finalMeanSqrError = finalError;
		}

		return false;
	}

	// now we need to update !all! 3D object point locations (not only those which we have updated already) and the camera poses concurrently
	// first we apply a rough object point and pose update

	relocatedObjectPointIdSet.clear();
	if (!updateDatabaseToRotationalMotion(rotationalDatabase, optimizedCameraDistortion, randomGenerator, lowerFrame, upperFrame, 0u, &relocatedObjectPointIdSet))
	{
		return false;
	}

	// now we apply a bundle adjustment to receive highly optimized results

	keyframePoseIds = determineRepresentativePoses(rotationalDatabase, lowerFrame, upperFrame, maximalKeyframes);

	if (keyframePoseIds.size() < minimalKeyframes)
	{
		return false;
	}

	relocatedObjectPointIds = Indices32(relocatedObjectPointIdSet.begin(), relocatedObjectPointIdSet.end());

	Indices32 validObjectPointIndices;
	const ObjectPointToPoseImagePointCorrespondenceAccessor objectPointToPoseImagePointCorrespondenceAccessor(rotationalDatabase, keyframePoseIds, relocatedObjectPointIds, 2u, &validObjectPointIndices);
	relocatedObjectPointIds = Subset::subset(relocatedObjectPointIds, validObjectPointIndices);

	keyframeOrientations = rotationalDatabase.rotationalPoses<false>(keyframePoseIds.data(), keyframePoseIds.size());

	const Vectors3 objectPoints = rotationalDatabase.objectPoints<false>(relocatedObjectPointIds);

	Vectors3 optimizedObjectPoints(objectPoints.size());
	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);
	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOrientations(optimizedCameraDistortion, ConstArrayAccessor<SquareMatrix3>(keyframeOrientations), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, Scalar(1), nullptr, &optimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5)))
	{
		return false;
	}

	rotationalDatabase.setObjectPoints<false>();
	rotationalDatabase.setObjectPoints<false>(relocatedObjectPointIds.data(), optimizedObjectPoints.data(), relocatedObjectPointIds.size());

#ifdef OCEAN_DEBUG
	// ensure that we really have updated all 3D object points
	const Indices32 debugAllValidObjectPointIds(rotationalDatabase.objectPointIds<false, false, false>(lowerFrame, upperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())));
	const IndexSet32 debugSet(debugAllValidObjectPointIds.begin(), debugAllValidObjectPointIds.end());
	ocean_assert(debugSet == relocatedObjectPointIdSet);
#endif

	Scalar finalAverageError = Numeric::maxValue();
	if (!updatePoses(rotationalDatabase, AnyCameraPinhole(optimizedCameraDistortion), CM_ROTATIONAL, randomGenerator, lowerFrame, upperFrame, 5u, Geometry::Estimator::ET_SQUARE, Scalar(1), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), &finalAverageError, nullptr, worker, abort))
	{
		return false;
	}

	// **TODO** is the average a good choice?
	if (finalAverageError > maximalSqrError)
	{
		return false;
	}

	if (finalMeanSqrError != nullptr)
	{
		*finalMeanSqrError = finalAverageError;
	}

	optimizedCamera = optimizedCameraDistortion;
	optimizedDatabase = std::move(rotationalDatabase);

	return !abort || !*abort;
}

bool Solver3::optimizeCamera(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrame, const unsigned int upperFrame, const bool findInitialFieldOfView, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, Database& optimizedDatabase, const unsigned int minimalObservationsInKeyframes, const unsigned int minimalKeyframes, const unsigned int maximalKeyframes, const Scalar lowerFovX, const Scalar upperFovX, Worker* worker, bool* abort, Scalar* finalMeanSqrError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(minimalKeyframes >= 2u && minimalKeyframes <= maximalKeyframes);

	ocean_assert(lowerFovX >= Numeric::deg2rad(5) && lowerFovX <= upperFovX && upperFovX < Numeric::deg2rad(180));

	// we determine a subset of frame poses best representing the entire set of poses

	const Indices32 keyFramePoseIds(determineRepresentativePoses(database, lowerFrame, upperFrame, maximalKeyframes));

	if (keyFramePoseIds.size() < minimalKeyframes)
	{
		return false;
	}

	HomogenousMatrices4 keyFramePoses = database.poses<false>(keyFramePoseIds.data(), keyFramePoseIds.size());

	// now we have to determine a representative set of object points (we take as much as we can so that we finally have only a small number of 3D object point locations which will get lost)

	Vectors3 objectPoints;
	Indices32 objectPointIds = database.objectPointIds<false, false, false>(keyFramePoseIds, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(-1), &objectPoints);

	unsigned int iterationMinimalObservationsInKeyframes = minimalObservationsInKeyframes;
	Indices32 validObjectPointIndices;
	const ObjectPointToPoseImagePointCorrespondenceAccessor objectPointToPoseImagePointCorrespondenceAccessor(database, keyFramePoseIds, objectPointIds, minimalObservationsInKeyframes, &validObjectPointIndices);

	optimizedCamera = pinholeCamera;

	if (findInitialFieldOfView)
	{
		// due to performance reasons we will ensure that (only for the determination of the initial fov) we do not use more than 100 object points
		// therefore, we iteratively increase the minimal number of observations to end with the best possible object points

		ObjectPointToPoseImagePointCorrespondenceAccessor fovAccessor(objectPointToPoseImagePointCorrespondenceAccessor);
		Indices32 fovValidObjectPointIndices(validObjectPointIndices);

		while (fovValidObjectPointIndices.size() > 100 && ++iterationMinimalObservationsInKeyframes <= keyFramePoseIds.size())
		{
			Indices32 validIndices;
			ObjectPointToPoseImagePointCorrespondenceAccessor newAccessor(database, keyFramePoseIds, objectPointIds, iterationMinimalObservationsInKeyframes, &validIndices);

			if (validIndices.size() < 100)
			{
				break;
			}

			fovValidObjectPointIndices = std::move(validIndices);
			fovAccessor = std::move(newAccessor);
		}

		// check whether we have still too many object points so that we should select unique (best representing) object points from the current set
		// the application of the 3D position of the object points can provide bad results if we observe a huge scene with large distances (so that we would select mainly object points very far away)
		// thus, we selected object points spread over the entire camera frames

		Vectors3 fovObjectPoints = Subset::subset(objectPoints, fovValidObjectPointIndices);
		const Indices32 fovObjectPointIds = Subset::subset(objectPointIds, fovValidObjectPointIndices);

		if (fovObjectPoints.size() > 100)
		{
			Vectors2 meanImagePoints;
			meanImagePoints.reserve(fovObjectPointIds.size());

			Indices32 validPoseIndices;
			Vectors2 imagePoints;

			// the object point now is visible in each key frame
			for (size_t n = 0; n < fovObjectPoints.size(); ++n)
			{
				validPoseIndices.clear();
				imagePoints.clear();

				database.observationsFromObjectPoint<false>(fovObjectPointIds[n], keyFramePoseIds, validPoseIndices, nullptr, &imagePoints);
				ocean_assert(validPoseIndices.size() == keyFramePoseIds.size());

				meanImagePoints.push_back(Geometry::Utilities::meanImagePoint(ConstTemplateArrayAccessor<Vector2>(imagePoints)));
			}

			SuccessionSubset<Scalar, 2> successionMeanImagePoints((SuccessionSubset<Scalar, 2>::Object*)meanImagePoints.data(), meanImagePoints.size());
			const Indices32 subsetSubsetIndices(SuccessionSubset<Scalar, 2>::indices2indices32(successionMeanImagePoints.subset(100)));

			fovAccessor = ObjectPointToPoseImagePointCorrespondenceAccessor(std::move(fovAccessor), subsetSubsetIndices);
			fovObjectPoints = Subset::subset(fovObjectPoints, subsetSubsetIndices);

			// from this moment the object point ids are not valid anymore
			ocean_assert(fovObjectPoints.size() != fovObjectPointIds.size());
		}

		// first we guess the best matching field of view of the camera, without modifying any other camera parameters

		bool significantResult = false;
		PinholeCamera optimizedCameraFoV;
		if (Geometry::NonLinearOptimizationCamera::findInitialFieldOfView(pinholeCamera, ConstArrayAccessor<HomogenousMatrix4>(keyFramePoses), ConstArrayAccessor<Vector3>(fovObjectPoints), fovAccessor, optimizedCameraFoV, nullptr, nullptr, lowerFovX, upperFovX, 8u, 3u, true, &significantResult, nullptr, WorkerPool::get().scopedWorker()(), abort))
		{
			if (significantResult)
			{
				Log::info() << "Translational camera field of view: " << Numeric::rad2deg(optimizedCameraFoV.fovX()) << "deg (significant)";

				optimizedCamera = optimizedCameraFoV;
			}
			else
			{
				Log::info() << "Translational camera field of view: " << Numeric::rad2deg(optimizedCameraFoV.fovX()) << "deg (NOT SIGNIFICANT)";
			}
		}
		else
		{
			Log::info() << "We failed to determine the rough field of view of the camera, so we process with the specified field of view.";
		}
	}

	objectPointIds = Subset::subset(objectPointIds, validObjectPointIndices);
	objectPoints = Subset::subset(objectPoints, validObjectPointIndices);

	if (pinholeCamera != optimizedCamera)
	{
		// we must update the poses of all camera frames and the locations of all 3D object points

		HomogenousMatrices4 optimizedKeyFramePosesFoV(keyFramePoseIds.size());
		NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessorFoV(optimizedKeyFramePosesFoV);
		Vectors3 optimizedObjectPointsFoV(objectPoints.size());
		NonconstArrayAccessor<Vector3> optimizedObjectPointAccessorFoV(optimizedObjectPointsFoV);

		Scalar initialError, finalError;
		if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndPoses(AnyCameraPinhole(optimizedCamera), ConstArrayAccessor<HomogenousMatrix4>(keyFramePoses), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, &optimizedPosesAccessorFoV, &optimizedObjectPointAccessorFoV, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, &initialError, &finalError))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		Log::info() << "We determined " << objectPoints.size() << " new object point locations and " << keyFramePoseIds.size() << " new camera poses due to the new camera's field of view: " << initialError << " -> " << finalError;

		objectPoints = std::move(optimizedObjectPointsFoV);
		keyFramePoses = std::move(optimizedKeyFramePosesFoV);
	}

	Vectors3 optimizedObjectPoints(objectPoints);

	if (optimizationStrategy != PinholeCamera::OS_NONE)
	{
		// now we will apply a bundle adjustment concurrently optimizing the camera parameters, the 3D object point locations and the camera poses

		NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints);

		Scalar initialError, finalError;
		PinholeCamera optimizedCameraDistortion;
		if (!Geometry::NonLinearOptimizationCamera::optimizeCameraObjectPointsPoses(optimizedCamera, ConstArrayAccessor<HomogenousMatrix4>(keyFramePoses), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, optimizationStrategy, optimizedCameraDistortion, nullptr, &optimizedObjectPointAccessor, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, &initialError, &finalError))
		{
			return false;
		}

		Log::info() << "Bundle adjustment (including camera profile) improvement: " << initialError << " -> " << finalError;

		optimizedCamera = optimizedCameraDistortion;
	}

	// now we need to update the database with the new locations of the 3D object points
	// therefore we will first invalidate all object point locations, afterwards we will set the positions of the optimized object points
	// thus, we may loose some object points with valid locations

	optimizedDatabase = database;

	ocean_assert(objectPointIds.size() == optimizedObjectPoints.size());

	optimizedDatabase.setObjectPoints<false>();
	optimizedDatabase.setObjectPoints<false>(objectPointIds.data(), optimizedObjectPoints.data(), objectPointIds.size());

	// now finally we update all poses !within! the specified frame range

	RandomGenerator randomGenerator;
	if (!updatePoses(optimizedDatabase, AnyCameraPinhole(optimizedCamera), CM_UNKNOWN, randomGenerator, lowerFrame, upperFrame, 5u, Geometry::Estimator::ET_SQUARE, Scalar(1), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), finalMeanSqrError, nullptr, worker, abort))
	{
		return false;
	}

	return !abort || !*abort;
}

bool Solver3::optimizeCameraWithVariableObjectPointsAndPoses(const Database& database, const PinholeCamera& pinholeCamera, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, Vectors3* optimizedObjectPoints, Indices32* optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses, Indices32* optimizedKeyFramePoseIds, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(minimalKeyFrames <= maximalKeyFrames);

	HomogenousMatrices4 poses;
	Indices32 poseIds = database.poseIds<false, false>(HomogenousMatrix4(false), &poses);
	const Indices32 representativePoseIndices = Pose::representativePoses(poses.data(), poses.size(), maximalKeyFrames);

	if (representativePoseIndices.size() < minimalKeyFrames)
	{
		return false;
	}

	Indices32 keyframePoseIds;

	if (representativePoseIndices.size() == poseIds.size())
	{
		keyframePoseIds = std::move(poseIds);
	}
	else
	{
		keyframePoseIds = Subset::subset(poseIds, representativePoseIndices);
	}

	if (!optimizeCameraWithVariableObjectPointsAndPoses(database, pinholeCamera, optimizationStrategy, keyframePoseIds, optimizedCamera, optimizedObjectPoints, optimizedObjectPointIds, optimizedKeyFramePoses, minimalObservations, estimator, iterations, initialRobustError, finalRobustError))
	{
		return false;
	}

	if (optimizedKeyFramePoseIds != nullptr)
	{
		*optimizedKeyFramePoseIds = std::move(keyframePoseIds);
	}

	return true;
}

bool Solver3::optimizeCameraWithVariableObjectPointsAndPoses(const Database& database, const PinholeCamera& pinholeCamera, const PinholeCamera::OptimizationStrategy optimizationStrategy, const Indices32& keyFrameIds, PinholeCamera& optimizedCamera, Vectors3* optimizedObjectPoints, Indices32* optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(keyFrameIds.size() >= 2);

	const Indices32 objectPointCandidateIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

	if (objectPointCandidateIds.empty())
	{
		return true;
	}

	return optimizeCameraWithVariableObjectPointsAndPoses(database, pinholeCamera, optimizationStrategy, keyFrameIds, objectPointCandidateIds, optimizedCamera, optimizedObjectPoints, optimizedObjectPointIds, optimizedKeyFramePoses, minimalObservations, estimator, iterations, initialRobustError, finalRobustError);
}

bool Solver3::optimizeCameraWithVariableObjectPointsAndPoses(const Database& database, const PinholeCamera& pinholeCamera, const PinholeCamera::OptimizationStrategy optimizationStrategy, const Indices32& keyFrameIds, const Indices32& objectPointIds, PinholeCamera& optimizedCamera, Vectors3* optimizedObjectPoints, Indices32* optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const unsigned int iterations, Scalar* initialRobustError, Scalar* finalRobustError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(objectPointIds.size() >= 1);
	ocean_assert(keyFrameIds.size() >= 2);

	Indices32 validObjectPointIndices;
	const ObjectPointToPoseImagePointCorrespondenceAccessor objectPointToPoseImagePointCorrespondenceAccessor(database, keyFrameIds, objectPointIds, minimalObservations, &validObjectPointIndices);

	if (validObjectPointIndices.empty())
	{
		return false;
	}

	Indices32 internalOptimizedObjectPointIds = Subset::subset(objectPointIds, validObjectPointIndices);
	const Vectors3 objectPoints(database.objectPoints<false>(internalOptimizedObjectPointIds));

	const HomogenousMatrices4 keyFramePoses(database.poses<false>(keyFrameIds.data(), keyFrameIds.size()));

#ifdef OCEAN_DEBUG
	for (HomogenousMatrices4::const_iterator i = keyFramePoses.begin(); i != keyFramePoses.end(); ++i)
		ocean_assert(i->isValid());
#endif

	NonconstArrayAccessor<Vector3> optimizedObjectPointAccessor(optimizedObjectPoints, objectPoints.size());
	NonconstArrayAccessor<HomogenousMatrix4> optimizedKeyFramePosesAccessor(optimizedKeyFramePoses, keyFrameIds.size());

	if (!Geometry::NonLinearOptimizationCamera::optimizeCameraObjectPointsPoses(pinholeCamera, ConstArrayAccessor<HomogenousMatrix4>(keyFramePoses), ConstArrayAccessor<Vector3>(objectPoints), objectPointToPoseImagePointCorrespondenceAccessor, optimizationStrategy, optimizedCamera, &optimizedKeyFramePosesAccessor, &optimizedObjectPointAccessor, iterations, estimator, Scalar(0.001), Scalar(5), true, initialRobustError, finalRobustError))
	{
		return false;
	}

	if (optimizedObjectPointIds != nullptr)
	{
		*optimizedObjectPointIds = std::move(internalOptimizedObjectPointIds);
	}

	return true;
}

void Solver3::determineUnknownObjectPointsSubset(const Database* database, const AnyCamera* camera, const CameraMotion cameraMotion, const Index32* objectPointIds, Vectors3* newObjectPoints, Indices32* newObjectPointIds, Indices32* newObjectPointObservations, RandomGenerator* randomGenerator, const unsigned int minimalObservations, const bool useAllObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar ransacMaximalSqrError, const Scalar averageRobustError, const Scalar maximalSqrError, Lock* lock, bool* abort, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints)
{
	ocean_assert(cameraMotion != CM_INVALID);
	ocean_assert(database != nullptr && camera != nullptr && randomGenerator != nullptr);
	ocean_assert(objectPointIds != nullptr && newObjectPoints != nullptr && newObjectPointIds != nullptr);
	ocean_assert(minimalObservations >= 2u);
	ocean_assert(estimator != Geometry::Estimator::ET_INVALID);

	RandomGenerator localGenerator(*randomGenerator);

	HomogenousMatrices4 poses;
	Vectors2 imagePoints;
	SquareMatrices3 orientations;
	SquareMatrices3 orientationsIF;

	Vectors3 localNewObjectPoints;
	Indices32 localNewObjectPointIds;
	Indices32 localNewObjectPointObservations;

	localNewObjectPoints.reserve(numberObjectPoints);
	localNewObjectPointIds.reserve(numberObjectPoints);

	if (newObjectPointObservations != nullptr)
	{
		localNewObjectPointObservations.reserve(numberObjectPoints);
	}

	for (unsigned int n = firstObjectPoint; (!abort || !*abort) && n < firstObjectPoint + numberObjectPoints; ++n)
	{
		poses.clear();
		imagePoints.clear();

		ocean_assert(database->objectPoint<false>(objectPointIds[n]) == Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		database->posesImagePoints<false, false>(objectPointIds[n], poses, imagePoints);
		ocean_assert(poses.size() == imagePoints.size());

		if (poses.size() < minimalObservations)
		{
			continue;
		}

		if (useAllObservations)
		{
			// we must use all observations, thus we start with an initial RANSAC iteration followed by an optimization for the entire set of correspondences

			Vector3 objectPoint;
			Vector3 optimizedObjectPoint;
			Scalar finalError = Numeric::maxValue();

			if (cameraMotion & CM_TRANSLATIONAL)
			{
				if (Geometry::RANSAC::objectPoint(*camera, ConstArrayAccessor<HomogenousMatrix4>(poses), ConstArrayAccessor<Vector2>(imagePoints), localGenerator, objectPoint, 20u, ransacMaximalSqrError, min(5u, minimalObservations), true, Geometry::Estimator::ET_INVALID))
				{
					if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPoses(*camera, ConstArrayAccessor<HomogenousMatrix4>(poses), objectPoint, ConstArrayAccessor<Vector2>(imagePoints), optimizedObjectPoint, 10u, estimator, Scalar(0.001), Scalar(5), true, nullptr, &finalError))
					{
						if (finalError < averageRobustError)
						{
							Scalar sqrError = 0;

							if (maximalSqrError > Numeric::eps())
							{
								determineProjectionErrors(*camera, optimizedObjectPoint, ConstArrayAccessor<HomogenousMatrix4>(poses), ConstArrayAccessor<Vector2>(imagePoints), nullptr, nullptr, &sqrError);
							}

							if (sqrError <= maximalSqrError)
							{
								localNewObjectPoints.push_back(optimizedObjectPoint);
								localNewObjectPointIds.push_back(objectPointIds[n]);

								if (newObjectPointObservations != nullptr)
								{
									localNewObjectPointObservations.emplace_back((unsigned int)(imagePoints.size()));
								}
							}
						}
					}
				}
			}
			else
			{
				ocean_assert(!poses.empty() && !imagePoints.empty());
				objectPoint = camera->ray(imagePoints[0], poses.front()).direction();

				orientationsIF.clear();

				for (size_t i = 0; i < poses.size(); ++i)
				{
					ocean_assert(poses[i].translation().isNull());
					orientationsIF.push_back(PinholeCamera::standard2InvertedFlipped(poses[i].rotationMatrix()));
				}

				if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientationsIF(*camera, ConstArrayAccessor<SquareMatrix3>(orientationsIF), ConstArrayAccessor<Vector2>(imagePoints), objectPoint, Scalar(1), optimizedObjectPoint, 10u, estimator, Scalar(0.001), Scalar(5), true, nullptr, &finalError)
						&& finalError < averageRobustError)
				{
					ocean_assert(maximalSqrError == Numeric::maxValue());

					localNewObjectPoints.push_back(optimizedObjectPoint);
					localNewObjectPointIds.push_back(objectPointIds[n]);

					if (newObjectPointObservations != nullptr)
					{
						localNewObjectPointObservations.push_back((unsigned int)imagePoints.size());
					}
				}
			}
		}
		else
		{
			// we can use a subset of all observations, thus we can use a standard RANSAC iteration (internally followed by an optimization for the subset of observations)

			Vector3 objectPoint;
			Scalar finalError = Numeric::maxValue();

			if (cameraMotion & CM_TRANSLATIONAL)
			{
				Indices32 validIndices;
				if (Geometry::RANSAC::objectPoint(*camera, ConstArrayAccessor<HomogenousMatrix4>(poses), ConstArrayAccessor<Vector2>(imagePoints), localGenerator, objectPoint, 20u, ransacMaximalSqrError, minimalObservations, true, estimator, &finalError, &validIndices)
						&& finalError < averageRobustError)
				{
					Scalar sqrError = 0;

					if (maximalSqrError > Numeric::eps())
					{
						determineProjectionErrors(*camera, objectPoint, ConstArraySubsetAccessor<HomogenousMatrix4, unsigned int>(poses, validIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints, validIndices), nullptr, nullptr, &sqrError);
					}

					if (sqrError <= maximalSqrError)
					{
						localNewObjectPoints.push_back(objectPoint);
						localNewObjectPointIds.push_back(objectPointIds[n]);

						if (newObjectPointObservations != nullptr)
						{
							localNewObjectPointObservations.push_back((unsigned int)imagePoints.size());
						}
					}
				}
			}
			else
			{
				ocean_assert(orientations.empty());

				for (size_t i = 0; i < poses.size(); ++i)
				{
					ocean_assert(poses[i].translation().isNull());
					orientations.push_back(poses[i].rotationMatrix());
				}

				if (Geometry::RANSAC::objectPoint(*camera, ConstArrayAccessor<SquareMatrix3>(orientations), ConstArrayAccessor<Vector2>(imagePoints), localGenerator, objectPoint, Scalar(1), 20u, ransacMaximalSqrError, minimalObservations, true, estimator, &finalError)
						&& finalError < averageRobustError)
				{
					ocean_assert(maximalSqrError == Numeric::maxValue());

					localNewObjectPoints.push_back(objectPoint);
					localNewObjectPointIds.push_back(objectPointIds[n]);

					if (newObjectPointObservations != nullptr)
					{
						localNewObjectPointObservations.push_back((unsigned int)imagePoints.size());
					}
				}
			}
		}
	}

	if (lock != nullptr)
	{
		const ScopedLock scopedLock(*lock);

		newObjectPoints->insert(newObjectPoints->end(), localNewObjectPoints.begin(), localNewObjectPoints.end());
		newObjectPointIds->insert(newObjectPointIds->end(), localNewObjectPointIds.begin(), localNewObjectPointIds.end());

		if (newObjectPointObservations != nullptr)
		{
			newObjectPointObservations->insert(newObjectPointObservations->end(), localNewObjectPointObservations.begin(), localNewObjectPointObservations.end());
		}
	}
	else
	{
		*newObjectPoints = std::move(localNewObjectPoints);
		*newObjectPointIds = std::move(localNewObjectPointIds);

		if (newObjectPointObservations != nullptr)
		{
			*newObjectPointObservations = std::move(localNewObjectPointObservations);
		}
	}
}

void Solver3::optimizeObjectPointsWithFixedPosesSubset(const Database* database, const PinholeCamera* pinholeCamera, const CameraMotion cameraMotion, const Index32* objectPointIds, Vectors3* optimizedObjectPoints, Indices32* optimizedObjectPointIds, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar maximalRobustError, Lock* lock, bool* abort, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints)
{
	ocean_assert(database && pinholeCamera);
	ocean_assert(objectPointIds && optimizedObjectPoints && optimizedObjectPointIds);
	ocean_assert(minimalObservations >= 2u);

	HomogenousMatrices4 poses;
	Vectors2 imagePoints;

	Vectors3 localOptimizedObjectPoints;
	Indices32 localOptimizedObjectPointIds;

	localOptimizedObjectPoints.reserve(numberObjectPoints);
	localOptimizedObjectPointIds.reserve(numberObjectPoints);

	SquareMatrices3 orientationsIF;
	HomogenousMatrices4 posesIF;

	const AnyCameraPinhole camera(PinholeCamera(*pinholeCamera, pinholeCamera->hasDistortionParameters()));

	for (unsigned int n = firstObjectPoint; (!abort || !*abort) && n < firstObjectPoint + numberObjectPoints; ++n)
	{
		poses.clear();
		imagePoints.clear();

		const Vector3& objectPoint = database->objectPoint<false>(objectPointIds[n]);
		ocean_assert(objectPoint.x() != Numeric::minValue() && objectPoint.y() != Numeric::minValue() && objectPoint.z() != Numeric::minValue());
		database->posesImagePoints<false, false>(objectPointIds[n], poses, imagePoints);

		ocean_assert(!poses.empty());
		ocean_assert(poses.size() == imagePoints.size());

		if (poses.size() < minimalObservations)
			continue;

		Scalar finalError = Numeric::maxValue();
		Vector3 optimizedObjectPoint;

		if (cameraMotion & CM_TRANSLATIONAL)
		{
			posesIF = PinholeCamera::standard2InvertedFlipped(poses);

			if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(camera, ConstArrayAccessor<HomogenousMatrix4>(posesIF), objectPoint, ConstArrayAccessor<Vector2>(imagePoints), optimizedObjectPoint, 20u, estimator, Scalar(0.001), Scalar(5), true, nullptr, &finalError))
			{
				ocean_assert(false && "Should never happen!");
				continue;
			}
		}
		else
		{
			orientationsIF.clear();

			for (size_t poseId = 0; poseId < poses.size(); ++poseId)
			{
				ocean_assert(poses[poseId].translation().isNull());
				orientationsIF.push_back(PinholeCamera::standard2InvertedFlipped(poses[poseId].rotationMatrix()));
			}

			if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientationsIF(camera, ConstArrayAccessor<SquareMatrix3>(orientationsIF), ConstArrayAccessor<Vector2>(imagePoints), objectPoint, Scalar(1), optimizedObjectPoint, 20u, estimator, Scalar(0.001), Scalar(5), true, nullptr, &finalError))
			{
				ocean_assert(false && "Should never happen!");
				continue;
			}
		}

		if (finalError <= maximalRobustError)
		{
			localOptimizedObjectPoints.push_back(optimizedObjectPoint);
			localOptimizedObjectPointIds.push_back(objectPointIds[n]);
		}
	}

	if (lock != nullptr)
	{
		const ScopedLock scopedLock(*lock);

		optimizedObjectPoints->insert(optimizedObjectPoints->end(), localOptimizedObjectPoints.begin(), localOptimizedObjectPoints.end());
		optimizedObjectPointIds->insert(optimizedObjectPointIds->end(), localOptimizedObjectPointIds.begin(), localOptimizedObjectPointIds.end());
	}
	else
	{
		*optimizedObjectPoints = std::move(localOptimizedObjectPoints);
		*optimizedObjectPointIds = std::move(localOptimizedObjectPointIds);
	}
}

bool Solver3::updatePoses(Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* finalAverageError, size_t* validPoses, bool* abort)
{
	ocean_assert(camera.isValid());
	ocean_assert(cameraMotion != CM_INVALID);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);
	ocean_assert(minimalCorrespondences >= 5u);

	Scalar totalError = 0;
	size_t numberPoses = 0;

	if (cameraMotion & CM_TRANSLATIONAL)
	{
		HomogenousMatrix4 previousPose(false);

		// forward iteration
		for (unsigned int n = startFrame; (!abort || !*abort) && n <= upperFrame; ++n)
		{
			Scalar errorValue = Numeric::maxValue();
			HomogenousMatrix4 currentPose(determinePose(database, camera, randomGenerator, n, previousPose, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue));

			// check whether the resulting pose is not accurate enough
			if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
			{
				currentPose.toNull();
			}

			database.setPose<false>(n, currentPose);
			previousPose = currentPose;

			if (currentPose.isValid())
			{
				totalError += errorValue;
				numberPoses++;
			}
		}

		previousPose = database.pose<false>(startFrame);

		// backward iteration
		for (int n = int(startFrame) - 1; (!abort || !*abort) && n >= int(lowerFrame); --n)
		{
			ocean_assert(n >= 0);
			Scalar errorValue = Numeric::maxValue();
			HomogenousMatrix4 currentPose(determinePose(database, camera, randomGenerator, (unsigned int)n, previousPose, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue));

			// check whether the resulting pose is not accurate enough
			if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
			{
				currentPose.toNull();
			}

			database.setPose<false>(n, currentPose);
			previousPose = currentPose;

			if (currentPose.isValid())
			{
				totalError += errorValue;
				numberPoses++;
			}
		}
	}
	else
	{
		SquareMatrix3 previousOrientation(false);

		// forward iteration
		for (unsigned int n = startFrame; (!abort || !*abort) && n <= upperFrame; ++n)
		{
			Scalar errorValue = Numeric::maxValue();
			SquareMatrix3 currentOrientation(determineOrientation(database, camera, randomGenerator, n, previousOrientation, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue));

			// check whether the resulting orientation is not accurate enough
			if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
			{
				currentOrientation.toNull();
			}

			if (currentOrientation.isNull())
			{
				database.setPose<false>(n, HomogenousMatrix4(false));
			}
			else
			{
				database.setPose<false>(n, HomogenousMatrix4(currentOrientation));
			}

			previousOrientation = currentOrientation;

			if (!currentOrientation.isNull())
			{
				totalError += errorValue;
				numberPoses++;
			}
		}

		previousOrientation = database.pose<false>(startFrame).rotationMatrix();

		// backward iteration
		for (int n = int(startFrame) - 1; (!abort || !*abort) && n >= int(lowerFrame); --n)
		{
			ocean_assert(n >= 0);
			Scalar errorValue = Numeric::maxValue();
			SquareMatrix3 currentOrientation(determineOrientation(database, camera, randomGenerator, (unsigned int)n, previousOrientation, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue));

			// check whether the resulting orientation is not accurate enough
			if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
			{
				currentOrientation.toNull();
			}

			if (currentOrientation.isNull())
			{
				database.setPose<false>(n, HomogenousMatrix4(false));
			}
			else
			{
				database.setPose<false>(n, HomogenousMatrix4(currentOrientation));
			}

			previousOrientation = currentOrientation;

			if (!currentOrientation.isNull())
			{
				totalError += errorValue;
				numberPoses++;
			}
		}
	}

	if (finalAverageError != nullptr && numberPoses != 0)
	{
		*finalAverageError = totalError / Scalar(numberPoses);
	}

	if (validPoses != nullptr)
	{
		*validPoses = numberPoses;
	}

	return abort == nullptr || !*abort;
}

bool Solver3::updatePoses(Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* finalAverageError, size_t* validPoses, Worker* worker, bool* abort)
{
	ocean_assert(cameraMotion != CM_INVALID);

	if (worker != nullptr)
	{
		Scalar totalError = 0;
		size_t poses = 0;

		Lock lock;

		if (cameraMotion & CM_TRANSLATIONAL)
		{
			worker->executeFunction(Worker::Function::createStatic(&updatePosesSubset, &database, &camera, &randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, &totalError, &poses, &lock, abort, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&updateOrientationsSubset, &database, &camera, &randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, &totalError, &poses, &lock, abort, worker->threads(), 0u, 0u), 0u, worker->threads());
		}

		if (finalAverageError != nullptr && poses != 0)
		{
			*finalAverageError = totalError / Scalar(poses);
		}

		if (validPoses != nullptr)
		{
			*validPoses = poses;
		}

		return abort == nullptr || !*abort;
	}
	else
	{
		return updatePoses(database, camera, cameraMotion, randomGenerator, lowerFrame, lowerFrame, upperFrame, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, finalAverageError, validPoses, abort);
	}
}

bool Solver3::determinePoses(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, const IndexSet32& priorityObjectPointIds, const bool solePriorityPoints, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, ShiftVector<HomogenousMatrix4>& poses, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* finalAverageError, Worker* worker, bool* abort)
{
	poses = ShiftVector<HomogenousMatrix4>(lowerFrame, upperFrame - lowerFrame + 1u);

	Scalar totalError = 0;

	if (worker != nullptr)
	{
		Lock lock;

		if (cameraMotion & CM_TRANSLATIONAL)
		{
			worker->executeFunction(Worker::Function::createStatic(&determinePosesSubset, &database, &camera, &priorityObjectPointIds, solePriorityPoints, &randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, &poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, &totalError, &lock, abort, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&determineOrientationsSubset, &database, &camera, &priorityObjectPointIds, solePriorityPoints, &randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, &poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, &totalError, &lock, abort, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
	}
	else
	{
		if (cameraMotion & CM_TRANSLATIONAL)
		{
			determinePosesSubset(&database, &camera, &priorityObjectPointIds, solePriorityPoints, &randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, &poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError,  &totalError, nullptr, abort, 1u, 0u, 0u);
		}
		else
		{
			determineOrientationsSubset(&database, &camera, &priorityObjectPointIds, solePriorityPoints, &randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, &poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError,  &totalError, nullptr, abort, 1u, 0u, 0u);
		}
	}

	if (finalAverageError != nullptr && poses.size() != 0)
	{
		*finalAverageError = totalError / Scalar(poses.size());
	}

	return abort == nullptr || !*abort;
}

bool Solver3::trackObjectPoints(const Database& database, const Indices32& objectPointIds, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const unsigned int minimalTrackedObjectPoints, const unsigned int minimalTrackedFrames, const unsigned int maximalTrackedObjectPoints, Indices32& trackedObjectPointIds, ImagePointGroups& trackedImagePointGroups, Indices32* trackedValidIndices, bool* abort)
{
	ocean_assert(!objectPointIds.empty());

	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

	ocean_assert((minimalTrackedObjectPoints == 0u && minimalTrackedFrames != 0u) || (minimalTrackedObjectPoints != 0u && minimalTrackedFrames == 0u));

	ocean_assert(minimalTrackedObjectPoints <= maximalTrackedObjectPoints);
	ocean_assert(maximalTrackedObjectPoints <= objectPointIds.size());

	ocean_assert(trackedImagePointGroups.isEmpty());
	trackedImagePointGroups.clear();

	if (abort != nullptr && *abort)
	{
		return false;
	}

	// the number of object points which must not be exceeded due to the tracking (this threshold is used if the termination condition is based on the minimal number of frames and is set the first time the minimal number of frames are reached)
	unsigned int fixedNumberObjectPoints = 0u;

	// determine all image points which are visible in the start frame and are connected with the defined object points
	Indices32 validIndices;
	Vectors2 startImagePoints = database.imagePointsFromObjectPoints<false>(startFrame, objectPointIds, validIndices);

	ocean_assert(trackedObjectPointIds.empty());
	trackedObjectPointIds.clear();

	ocean_assert(minimalTrackedObjectPoints <= validIndices.size());
	if (validIndices.size() < minimalTrackedObjectPoints)
	{
		return false;
	}

	trackedObjectPointIds = Subset::subset(objectPointIds, validIndices);

	trackedImagePointGroups.setFirstIndex(startFrame);
	trackedImagePointGroups.insert(startFrame, std::move(startImagePoints));
	ocean_assert(startImagePoints.empty());

	// concurrent forward and backward tracking
	ImagePointGroups::Index forwardIndex = ImagePointGroups::Index(startFrame) + 1;
	ImagePointGroups::Index backwardIndex = ImagePointGroups::Index(startFrame) - 1;

	Indices32 validForwardObjectPointIndices, validBackwardObjectPointIndices;
	Vectors2 currentForwardPoints, currentBackwardPoints;

	while ((forwardIndex <= ImagePointGroups::Index(upperFrame) || backwardIndex >= ImagePointGroups::Index(lowerFrame)) && (!abort || !*abort))
	{
		validForwardObjectPointIndices.clear();
		validBackwardObjectPointIndices.clear();

		// forward tracking: track the previous points to the current frame
		if (forwardIndex <= ImagePointGroups::Index(upperFrame))
		{
			currentForwardPoints = database.imagePointsFromObjectPoints<false>(Index32(forwardIndex), trackedObjectPointIds, validForwardObjectPointIndices);
			ocean_assert(currentForwardPoints.size() == validForwardObjectPointIndices.size());
		}

		// backward tracking: track the previous points to the current frame
		if (backwardIndex >= ImagePointGroups::Index(lowerFrame))
		{
			currentBackwardPoints = database.imagePointsFromObjectPoints<false>(Index32(backwardIndex), trackedObjectPointIds, validBackwardObjectPointIndices);
			ocean_assert(currentBackwardPoints.size() == validBackwardObjectPointIndices.size());
		}

		if (validForwardObjectPointIndices.empty() && validBackwardObjectPointIndices.empty())
			break;

		// now we need to check our termination conditions

		if (minimalTrackedObjectPoints != 0u)
		{
			// the termination condition requests a minimal number of tracked points

			// check whether we can stop as neither the forward nor the backward tracking could track enough points
			if (validForwardObjectPointIndices.size() < minimalTrackedObjectPoints && validBackwardObjectPointIndices.size() < minimalTrackedObjectPoints)
				break;
		}
		else
		{
			ocean_assert(minimalTrackedFrames != 0u);
			// the termination condition requests a minimal number of tracked frames

			ocean_assert(fixedNumberObjectPoints != 0u || trackedImagePointGroups.size() < minimalTrackedFrames);

			// check whether if we have reach the minimal number of frames already
			if (fixedNumberObjectPoints != 0u)
			{
				ocean_assert(trackedImagePointGroups.size() >= minimalTrackedFrames);

				// check whether we cannot added a new frame with same number of object points as defined in the moment the minimal number of frames has been reached
				if (validForwardObjectPointIndices.size() < fixedNumberObjectPoints && validBackwardObjectPointIndices.size() < fixedNumberObjectPoints)
				{
					break;
				}
			}
		}

		// check whether we could track all points forward and backward so that we do not have to take one of both (the best one)
		if (validForwardObjectPointIndices.size() == validBackwardObjectPointIndices.size() && validForwardObjectPointIndices.size() == trackedObjectPointIds.size())
		{
			ocean_assert(forwardIndex <= ImagePointGroups::Index(upperFrame) && backwardIndex >= ImagePointGroups::Index(lowerFrame));

			trackedImagePointGroups.insert(forwardIndex, std::move(currentForwardPoints));
			trackedImagePointGroups.insert(backwardIndex, std::move(currentBackwardPoints));

			forwardIndex++;
			backwardIndex--;
		}
		else
		{
			// if we have more backward points than forward points
			if (validBackwardObjectPointIndices.size() > validForwardObjectPointIndices.size())
			{
				if (trackedObjectPointIds.size() != validBackwardObjectPointIndices.size())
				{
					trackedObjectPointIds = Subset::subset(trackedObjectPointIds, validBackwardObjectPointIndices);
					validIndices = Subset::subset(validIndices, validBackwardObjectPointIndices);

					for (ShiftVector<Vectors2>::Index i = trackedImagePointGroups.firstIndex(); i <= trackedImagePointGroups.lastIndex(); ++i)
						trackedImagePointGroups[i] = Subset::subset(trackedImagePointGroups[i], validBackwardObjectPointIndices);
				}

				trackedImagePointGroups.insert(backwardIndex, std::move(currentBackwardPoints));
				backwardIndex--;
			}
			else // if we have more (or equal) forward points than backward points
			{
				if (trackedObjectPointIds.size() != validForwardObjectPointIndices.size())
				{
					trackedObjectPointIds = Subset::subset(trackedObjectPointIds, validForwardObjectPointIndices);
					validIndices = Subset::subset(validIndices, validForwardObjectPointIndices);

					for (ShiftVector<Vectors2>::Index i = trackedImagePointGroups.firstIndex(); i < trackedImagePointGroups.endIndex(); ++i)
						trackedImagePointGroups[i] = Subset::subset(trackedImagePointGroups[i], validForwardObjectPointIndices);
				}

				trackedImagePointGroups.insert(forwardIndex, std::move(currentForwardPoints));
				forwardIndex++;
			}
		}

		// check whether we have the 'frame termination condition' and check whether we have reached the number of frames for the first time
		if (minimalTrackedFrames != 0u && fixedNumberObjectPoints == 0u)
		{
			ocean_assert(minimalTrackedObjectPoints == 0u);

			if (trackedImagePointGroups.size() >= minimalTrackedFrames)
			{
				ocean_assert(trackedImagePointGroups.size() != 0);
				fixedNumberObjectPoints = (unsigned int)trackedImagePointGroups.front().size();
			}
		}
	}

	if (trackedValidIndices)
	{
#ifdef OCEAN_DEBUG
		const IndexSet32 trackedObjectPointIdSet(trackedObjectPointIds.begin(), trackedObjectPointIds.end());
		for (Indices32::const_iterator i = validIndices.begin(); i != validIndices.end(); ++i)
		{
			ocean_assert(trackedObjectPointIdSet.find(objectPointIds[*i]) != trackedObjectPointIdSet.end());
		}
#endif

		*trackedValidIndices = std::move(validIndices);
	}

	// now we ensure that we do not have too much object points
	if (trackedImagePointGroups[startFrame].size() > maximalTrackedObjectPoints)
	{
		// so we selected widely spread image points in the start frame

		SuccessionSubset<Scalar, 2> successionSubet((SuccessionSubset<Scalar, 2>::Object*)trackedImagePointGroups[startFrame].data(), trackedImagePointGroups[startFrame].size());
		const Indices32 subsetIndices = SuccessionSubset<Scalar, 2>::indices2indices32(successionSubet.subset(100));

		for (ImagePointGroups::Index n = trackedImagePointGroups.firstIndex(); n <= trackedImagePointGroups.lastIndex(); ++n)
		{
			ocean_assert(trackedImagePointGroups[n].size() == trackedObjectPointIds.size());
			trackedImagePointGroups[n] = Subset::subset(trackedImagePointGroups[n], subsetIndices);
		}

		trackedObjectPointIds = Subset::subset(trackedObjectPointIds, subsetIndices);

		if (trackedValidIndices != nullptr)
		{
			*trackedValidIndices = Subset::subset(*trackedValidIndices, subsetIndices);
		}
	}

	return true;
}

bool Solver3::trackObjectPoints(const Database& database, const Indices32& priorityObjectPointIds, const Indices32& remainingObjectPointIds, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const unsigned int minimalTrackedPriorityObjectPoints, const Scalar minimalRemainingFramesRatio, const unsigned int maximalTrackedPriorityObjectPoints, const unsigned int maximalTrackedRemainingObjectPoints, Indices32& trackedObjectPointIds, ImagePointGroups& trackedImagePointGroups, Indices32* trackedValidPriorityIndices, Indices32* trackedValidRemainingIndices, bool* abort)
{
	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

	ocean_assert(minimalTrackedPriorityObjectPoints <= maximalTrackedPriorityObjectPoints);
	ocean_assert(minimalTrackedPriorityObjectPoints <= priorityObjectPointIds.size());

	ocean_assert(minimalRemainingFramesRatio > 0 && minimalRemainingFramesRatio <= 1);

#ifdef OCEAN_DEBUG
	{
		IndexSet32 objectPointIdSet(priorityObjectPointIds.begin(), priorityObjectPointIds.end());
		objectPointIdSet.insert(remainingObjectPointIds.begin(), remainingObjectPointIds.end());
		ocean_assert(objectPointIdSet.size() == priorityObjectPointIds.size() + remainingObjectPointIds.size());
	}
#endif

	if (abort != nullptr && *abort)
	{
		return false;
	}

	// first we track the priority points to receive the tracking borders

	Indices32 trackedPriorityObjectPointIds;
	ImagePointGroups trackedPriorityImagePointGroups;
	if (!trackObjectPoints(database, priorityObjectPointIds, lowerFrame, startFrame, upperFrame, minimalTrackedPriorityObjectPoints, 0u, maximalTrackedPriorityObjectPoints, trackedPriorityObjectPointIds, trackedPriorityImagePointGroups, trackedValidPriorityIndices, abort))
	{
		return false;
	}

	ocean_assert(trackedValidPriorityIndices == nullptr || trackedValidPriorityIndices->size() == trackedPriorityObjectPointIds.size());

	if (trackedPriorityImagePointGroups.isEmpty())
		return false;

	const unsigned int validLowerFrame = (unsigned int)trackedPriorityImagePointGroups.firstIndex();
	const unsigned int validUpperFrame = (unsigned int)trackedPriorityImagePointGroups.lastIndex();

	ocean_assert(validLowerFrame <= startFrame && startFrame <= validUpperFrame);

	// afterwards we track the remaining points within the borders

	// we accept all remaining object points which are visible in at least 80% of the frames in which the priority frames are visible
	const unsigned int minimalTrackedRemainingFrames = max(1u, (unsigned int)(Scalar(trackedPriorityImagePointGroups.size()) * minimalRemainingFramesRatio));

	Indices32 trackedRemainingObjectPointIds;
	ImagePointGroups trackedRemainingImagePointGroups;
	if (!trackObjectPoints(database, remainingObjectPointIds, validLowerFrame, startFrame, validUpperFrame, 0u, minimalTrackedRemainingFrames, maximalTrackedRemainingObjectPoints, trackedRemainingObjectPointIds, trackedRemainingImagePointGroups, trackedValidRemainingIndices, abort))
	{
		return false;
	}

	ocean_assert(trackedValidRemainingIndices == nullptr || trackedValidRemainingIndices->size() == trackedRemainingObjectPointIds.size());

	if (trackedRemainingImagePointGroups.isEmpty())
	{
		return false;
	}

	// now we join the priority tracking results and the remaining tracking results

	const unsigned int commonLowerFrame = (unsigned int)trackedRemainingImagePointGroups.firstIndex();
	const unsigned int commonUpperFrame = (unsigned int)trackedRemainingImagePointGroups.lastIndex();

	ocean_assert(commonLowerFrame >= validLowerFrame);
	ocean_assert(commonUpperFrame <= validUpperFrame);

	ocean_assert(trackedImagePointGroups.isEmpty());
	ocean_assert(trackedObjectPointIds.empty());

	trackedImagePointGroups = ImagePointGroups(commonLowerFrame, commonUpperFrame - commonLowerFrame + 1u);

	trackedObjectPointIds = std::move(trackedPriorityObjectPointIds);
	trackedObjectPointIds.insert(trackedObjectPointIds.end(), trackedRemainingObjectPointIds.begin(), trackedRemainingObjectPointIds.end());

	for (unsigned int n = commonLowerFrame; n <= commonUpperFrame; ++n)
	{
		Vectors2& priorityImagePoints = trackedPriorityImagePointGroups[n];
		Vectors2& remainingImagePoints = trackedRemainingImagePointGroups[n];

		priorityImagePoints.insert(priorityImagePoints.end(), remainingImagePoints.begin(), remainingImagePoints.end());

		trackedImagePointGroups[n] = std::move(priorityImagePoints);
	}

	return true;
}

Indices32 Solver3::trackObjectPointsToNeighborFrames(const Database& database, const Indices32& objectPointIds, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame)
{
	ocean_assert(!objectPointIds.empty());

	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

	// determine all image points which are visible in the start frame and are connected with the defined object points
	Indices32 validIndices;
	database.imagePointsFromObjectPoints<false>(startFrame, objectPointIds, validIndices);

	Indices32 trackedObjectPointIds = Subset::subset(objectPointIds, validIndices);

	// forward tracking
	if (startFrame + 1u <= upperFrame)
	{
		validIndices.clear();
		database.imagePointsFromObjectPoints<false>(Index32(startFrame + 1u), trackedObjectPointIds, validIndices);

		trackedObjectPointIds = Subset::subset(trackedObjectPointIds, validIndices);
	}

	// backward tracking
	if (startFrame >= lowerFrame + 1u)
	{
		validIndices.clear();
		database.imagePointsFromObjectPoints<false>(Index32(startFrame - 1u), trackedObjectPointIds, validIndices);

		trackedObjectPointIds = Subset::subset(trackedObjectPointIds, validIndices);
	}

	return trackedObjectPointIds;
}

Indices32 Solver3::determineRepresentativePoses(const Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const size_t numberRepresentative)
{
	ocean_assert(lowerFrame <= upperFrame);

	const unsigned int numberFrames = upperFrame - lowerFrame + 1u;

	Poses poses;
	poses.reserve(numberFrames);

	Indices32 poseIds;
	poseIds.reserve(numberFrames);

	for (unsigned int n = lowerFrame; n <= upperFrame; ++n)
	{
		const HomogenousMatrix4& poseMatrix = database.pose<false>(n);

		if (poseMatrix.isValid())
		{
			poses.push_back(Pose(poseMatrix));
			poseIds.push_back(n);
		}
	}

	if (poseIds.size() <= numberRepresentative)
		return poseIds;

	return Subset::subset(poseIds, Pose::representativePoses(poses, numberRepresentative));
}

Indices32 Solver3::determineRepresentativePoses(const Database& database, const Indices32& poseIds, const size_t numberRepresentative)
{
	Poses poses;
	poses.reserve(poseIds.size());

	for (Indices32::const_iterator i = poseIds.begin(); i != poseIds.end(); ++i)
	{
		const HomogenousMatrix4& poseMatrix = database.pose<false>(*i);
		ocean_assert(poseMatrix.isValid());

		poses.push_back(Pose(poseMatrix));
	}

	return Subset::subset(poseIds, Pose::representativePoses(poses, numberRepresentative));
}

size_t Solver3::determineValidPoses(const AnyCamera& camera, const Vectors3& objectPoints, const ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, const CameraMotion cameraMotion, const unsigned int firstValidPoseIndex, const HomogenousMatrix4& firstValidPose, const unsigned int secondValidPoseIndex, const HomogenousMatrix4& secondValidPose, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Indices32* validObjectPointIndices, HomogenousMatrices4* poses, Indices32* poseIds, Scalar* totalSqrError)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() >= 5);
	ocean_assert(imagePointGroups.size() >= 2);
	ocean_assert(objectPoints.size() == imagePointGroups.front().size());
	ocean_assert(imagePointGroups.isValidIndex(firstValidPoseIndex) && imagePointGroups.isValidIndex(secondValidPoseIndex));
	ocean_assert(firstValidPoseIndex != secondValidPoseIndex);
	ocean_assert(firstValidPose.isValid() && secondValidPose.isValid());

	/**
	 * we have two valid poses in the frame range:
	 * [-------|-------|----------]
	 * where [ and ] denotes the frame range, | the two known poses, and - unknown poses
	 *
	 * first we will determine the poses between the two known poses, afterwards we determine the left and the right areas
	 */

	unsigned int lowerValidPoseIndex = firstValidPoseIndex;
	unsigned int upperValidPoseIndex = secondValidPoseIndex;
	HomogenousMatrix4 lowerValidPose = firstValidPose;
	HomogenousMatrix4 upperValidPose = secondValidPose;

	if (lowerValidPoseIndex > upperValidPoseIndex)
	{
		std::swap(lowerValidPoseIndex, upperValidPoseIndex);
		std::swap(lowerValidPose, upperValidPose);
	}

	Indices32 validPoseIds;
	validPoseIds.reserve(imagePointGroups.size());
	validPoseIds.push_back(lowerValidPoseIndex);
	validPoseIds.push_back(upperValidPoseIndex);

	HomogenousMatrices4 validPoses;
	validPoses.reserve(imagePointGroups.size());
	validPoses.push_back(lowerValidPose);
	validPoses.push_back(upperValidPose);

	const unsigned int minimalValidCorrespondences = max(5u, (unsigned int)(Scalar(objectPoints.size()) * minimalValidCorrespondenceRatio));

	Indices32 internalValidObjectPointIndices = createIndices(objectPoints.size(), 0u);
	Indices32 iterationValidIndices;
	Scalar totalError = 0;

	// we first start with the inner frame poses (interleaved)

	HomogenousMatrix4 previousLeft = lowerValidPose;
	HomogenousMatrix4 previousRight = upperValidPose;

	int leftIndex = int(lowerValidPoseIndex);
	int rightIndex = int(upperValidPoseIndex);

	for (unsigned int i = 0u; i < imagePointGroups.size(); ++i)
	{
		const bool forwardStep = i % 2u == 0u;

		int index = forwardStep ? ++leftIndex : --rightIndex;
		HomogenousMatrix4* previousPose = forwardStep ? &previousLeft : &previousRight;

		// check whether we have met in the middle already
		if (leftIndex >= rightIndex || rightIndex <= leftIndex)
		{
			break;
		}

		Scalar finalError = Numeric::maxValue();
		iterationValidIndices.clear();

		if (cameraMotion & CM_TRANSLATIONAL)
		{
			const HomogenousMatrix4 pose(determinePose(camera, randomGenerator, ConstArraySubsetAccessor<Vector3, Index32>(objectPoints, internalValidObjectPointIndices), ConstArraySubsetAccessor<Vector2, Index32>(imagePointGroups[index], internalValidObjectPointIndices), *previousPose, Geometry::Estimator::ET_SQUARE, minimalValidCorrespondenceRatio, maximalSqrError, &finalError, &iterationValidIndices));

			if (!pose.isValid() || iterationValidIndices.size() < minimalValidCorrespondences)
			{
				break;
			}

			ocean_assert(finalError != Numeric::maxValue());
			*previousPose = pose;
		}
		else
		{
			const SquareMatrix3 orientation(determineOrientation(camera, randomGenerator, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints, internalValidObjectPointIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePointGroups[index], internalValidObjectPointIndices), previousPose->rotationMatrix(), Geometry::Estimator::ET_SQUARE, minimalValidCorrespondenceRatio, maximalSqrError, &finalError, &iterationValidIndices));

			if (orientation.isNull() || iterationValidIndices.size() < minimalValidCorrespondences)
			{
				break;
			}

			ocean_assert(finalError != Numeric::maxValue());
			*previousPose = HomogenousMatrix4(orientation);
		}

		if (iterationValidIndices.size() != internalValidObjectPointIndices.size())
		{
			internalValidObjectPointIndices = Subset::subset(internalValidObjectPointIndices, iterationValidIndices);
		}

		totalError += finalError;

		validPoseIds.push_back(index);
		validPoses.push_back(*previousPose);
	}

	// now we should have poses for each inner frame
	// however if the scene is very complex we can fail to determine a pose for each frame so that we stop here (as in this case the object points are bad)

	if (validPoses.size() == upperValidPoseIndex - lowerValidPoseIndex + 1u)
	{
		// now we investigate the left and the right frames (interleaved)

		previousLeft = lowerValidPose;
		previousRight = upperValidPose;

		leftIndex = int(lowerValidPoseIndex);
		rightIndex = int(upperValidPoseIndex);

		for (unsigned int i = 0u; i < imagePointGroups.size(); ++i)
		{
			const bool forwardStep = i % 2u == 0u;

			// check whether we have met both boundaries already
			if (leftIndex <= imagePointGroups.firstIndex() && rightIndex >= imagePointGroups.lastIndex())
			{
				break;
			}
			else if (forwardStep && leftIndex == imagePointGroups.firstIndex())
			{
				continue;
			}
			else if (!forwardStep && rightIndex == imagePointGroups.lastIndex())
			{
				continue;
			}

			int index = forwardStep ? --leftIndex : ++rightIndex;
			HomogenousMatrix4* previousPose = forwardStep ? &previousLeft : &previousRight;

			Scalar finalError = Numeric::maxValue();
			iterationValidIndices.clear();

			if (cameraMotion & CM_TRANSLATIONAL)
			{
				const HomogenousMatrix4 pose(determinePose(camera, randomGenerator, ConstArraySubsetAccessor<Vector3, Index32>(objectPoints, internalValidObjectPointIndices), ConstArraySubsetAccessor<Vector2, Index32>(imagePointGroups[index], internalValidObjectPointIndices), *previousPose, Geometry::Estimator::ET_SQUARE, minimalValidCorrespondenceRatio, maximalSqrError, &finalError, &iterationValidIndices));

				if (!pose.isValid() || iterationValidIndices.size() < minimalValidCorrespondences)
				{
					break;
				}

				ocean_assert(finalError != Numeric::maxValue());
				*previousPose = pose;
			}
			else
			{
				const SquareMatrix3 orientation(determineOrientation(camera, randomGenerator, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints, internalValidObjectPointIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePointGroups[index], internalValidObjectPointIndices), previousPose->rotationMatrix(), Geometry::Estimator::ET_SQUARE, minimalValidCorrespondenceRatio, maximalSqrError, &finalError, &iterationValidIndices));

				if (orientation.isNull() || iterationValidIndices.size() < minimalValidCorrespondences)
				{
					break;
				}

				ocean_assert(finalError != Numeric::maxValue());
				*previousPose = HomogenousMatrix4(orientation);
			}

			if (iterationValidIndices.size() != internalValidObjectPointIndices.size())
			{
				internalValidObjectPointIndices = Subset::subset(internalValidObjectPointIndices, iterationValidIndices);
			}

			totalError += finalError;

			validPoseIds.push_back(index);
			validPoses.push_back(*previousPose);
		}
	}

	ocean_assert(IndexSet32(validPoseIds.begin(), validPoseIds.end()).size() == validPoseIds.size());

	if (totalSqrError != nullptr)
	{
		*totalSqrError = totalError;
	}

	const size_t result = validPoseIds.size();

	if (validObjectPointIndices != nullptr)
	{
		*validObjectPointIndices = std::move(internalValidObjectPointIndices);
	}

	if (poses != nullptr)
	{
		*poses = std::move(validPoses);
	}

	if (poseIds != nullptr)
	{
		*poseIds = std::move(validPoseIds);
	}

	return result;
}

Solver3::CameraMotion Solver3::determineCameraMotion(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrame, const unsigned int upperFrame, const bool onlyVisibleObjectPoints, Worker* worker, const Scalar minimalTinyTranslationObservationAngle, const Scalar minimalModerateTranslationObservationAngle, const Scalar minimalSignificantTranslationObservationAngle, const Scalar minimalTinyRotationAngle, const Scalar minimalModerateRotationAngle, const Scalar minimalSignificantRotationAngle)
{
	ocean_assert(lowerFrame <= upperFrame);

	ocean_assert(minimalTinyTranslationObservationAngle < minimalModerateTranslationObservationAngle);
	ocean_assert(minimalModerateTranslationObservationAngle < minimalSignificantTranslationObservationAngle);
	ocean_assert(minimalSignificantTranslationObservationAngle < Numeric::pi_2());

	ocean_assert(minimalTinyRotationAngle < minimalModerateRotationAngle);
	ocean_assert(minimalModerateRotationAngle < minimalSignificantRotationAngle);
	ocean_assert(minimalSignificantRotationAngle < Numeric::pi_2());

	// if we have only one frame than we have a static camera in any case
	if (lowerFrame == upperFrame)
		return CM_STATIC;

	const Indices32 objectPointIds = onlyVisibleObjectPoints ? database.objectPointIds<false, false, false>(lowerFrame, upperFrame) : database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

	if (objectPointIds.empty())
		return CM_STATIC;

	Scalars objectPointAccuracies(determineObjectPointsAccuracy(database, pinholeCamera, objectPointIds, AM_MEAN_DIRECTION_MEDIAN_COSINE, lowerFrame, upperFrame, worker));
	std::sort(objectPointAccuracies.begin(), objectPointAccuracies.end());

	// we select the 5% index to filter outliers
	const Scalar objectPointAccuraciesCosine5 = objectPointAccuracies[objectPointAccuracies.size() * 5 / 100];
	//const Scalar medianObjectPointAccuracyCosine = Median::median(objectPointAccuracies.data(), objectPointAccuracies.size());

	Scalars xOrientations(upperFrame - lowerFrame + 1u);
	Scalars yOrientations(upperFrame - lowerFrame + 1u);
	Scalars zOrientations(upperFrame - lowerFrame + 1u);
	determinePosesOrientation(database, lowerFrame, upperFrame, xOrientations.data(), yOrientations.data(), zOrientations.data());

	const Scalar xOrientationMedianCosine = Median::median(xOrientations.data(), xOrientations.size());
	const Scalar yOrientationMedianCosine = Median::median(yOrientations.data(), yOrientations.size());
	const Scalar zOrientationMedianCosine = Median::median(zOrientations.data(), zOrientations.size());

	const Scalar orientationMedianCosine = min(xOrientationMedianCosine, min(yOrientationMedianCosine, zOrientationMedianCosine));

	const Scalar medianObjectPointAccuracy10 = Numeric::acos(objectPointAccuraciesCosine5);
	const Scalar orientationMedian = Numeric::acos(orientationMedianCosine);

	CameraMotion cameraMotion = CM_INVALID;

	if (medianObjectPointAccuracy10 >= minimalSignificantTranslationObservationAngle)
		cameraMotion = CameraMotion(cameraMotion | CM_TRANSLATIONAL_SIGNIFICANT);
	else if (medianObjectPointAccuracy10 >= minimalModerateTranslationObservationAngle)
		cameraMotion = CameraMotion(cameraMotion | CM_TRANSLATIONAL_MODERATE);
	else if (medianObjectPointAccuracy10 >= minimalTinyTranslationObservationAngle)
		cameraMotion = CameraMotion(cameraMotion | CM_TRANSLATIONAL_TINY);

	if (orientationMedian >= minimalSignificantRotationAngle)
		cameraMotion = CameraMotion(cameraMotion | CM_ROTATIONAL_SIGNIFICANT);
	else if (orientationMedian >= minimalModerateRotationAngle)
		cameraMotion = CameraMotion(cameraMotion | CM_ROTATIONAL_MODERATE);
	else if (orientationMedian >= minimalTinyRotationAngle)
		cameraMotion = CameraMotion(cameraMotion | CM_ROTATIONAL_TINY);

	if (cameraMotion == CM_INVALID)
		cameraMotion = CM_STATIC;

	return cameraMotion;
}

std::string Solver3::translateCameraMotion(const CameraMotion cameraMotion)
{
	if (cameraMotion == CM_UNKNOWN)
		return std::string("Unknown");

	if (cameraMotion == CM_STATIC)
		return std::string("Static");

	std::string motion;

	if (cameraMotion & CM_TRANSLATIONAL)
	{
		motion += "Translational motion";

		if ((cameraMotion & CM_TRANSLATIONAL_TINY) == CM_TRANSLATIONAL_TINY)
			motion += " (tiny)";
		else if ((cameraMotion & CM_TRANSLATIONAL_MODERATE) == CM_TRANSLATIONAL_MODERATE)
			motion += " (moderate)";
		else if ((cameraMotion & CM_TRANSLATIONAL_SIGNIFICANT) == CM_TRANSLATIONAL_SIGNIFICANT)
			motion += " (significant)";
	}

	if (cameraMotion & CM_ROTATIONAL)
	{
		if (!motion.empty())
			motion += " and ";

		motion += "Rotational motion";

		if ((cameraMotion & CM_ROTATIONAL_TINY) == CM_ROTATIONAL_TINY)
			motion += " (tiny)";
		else if ((cameraMotion & CM_ROTATIONAL_MODERATE) == CM_ROTATIONAL_MODERATE)
			motion += " (moderate)";
		else if ((cameraMotion & CM_ROTATIONAL_SIGNIFICANT) == CM_ROTATIONAL_SIGNIFICANT)
			motion += " (significant)";
	}

	return motion;
}

Scalar Solver3::determineObjectPointAccuracy(const PinholeCamera& pinholeCamera, const HomogenousMatrix4* poses, const Vector2* imagePoints, const size_t observations, const AccuracyMethod accuracyMethod)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(poses && imagePoints);
	ocean_assert(observations >= 2);

	if (observations <= 1)
		return Scalar(1); // cos(0)

	const bool undistortImagePoints = pinholeCamera.hasDistortionParameters();

	Vectors3 observationDirections;
	observationDirections.reserve(observations);

	Vector3 meanDirection(0, 0, 0);

	for (size_t n = 0; n < observations; ++n)
	{
		const Line3 ray = pinholeCamera.ray(undistortImagePoints ? pinholeCamera.undistort<true>(imagePoints[n]) : imagePoints[n], poses[n]);

		ocean_assert(Numeric::isEqual(ray.direction().length(), 1));
		observationDirections.push_back(ray.direction());

		meanDirection += ray.direction();
	}

	// check whether we had parallel but opposite directions
	if (!meanDirection.normalize())
		return Scalar(0); // cos(1)

	switch (accuracyMethod)
	{
		case AM_MEAN_DIRECTION_MEAN_COSINE:
		{
			Scalar minAbsCos = 0;
			for (size_t n = 0; n < observations; ++n)
				minAbsCos += Numeric::abs(meanDirection * observationDirections[n]);

			return minAbsCos / Scalar(observations);
		}

		case AM_MEAN_DIRECTION_MEDIAN_COSINE:
		{
			Scalars minAbsCos(observations);

			for (size_t n = 0; n < observations; ++n)
				minAbsCos[n] = Numeric::abs(meanDirection * observationDirections[n]);

			return Median::median(minAbsCos.data(), minAbsCos.size());
		}

		default:
			break;
	}

	ocean_assert(accuracyMethod == accuracyMethod);

	Scalar minAbsCos = 1;
	for (size_t n = 0; n < observations; ++n)
		minAbsCos = min(Numeric::abs(meanDirection * observationDirections[n]), minAbsCos);

	return minAbsCos;
}

Scalars Solver3::determineObjectPointsAccuracy(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& objectPointIds, const AccuracyMethod accuracyMethhod, const unsigned int lowerFrame, const unsigned int upperFrame, Worker* worker)
{
	ocean_assert((lowerFrame != (unsigned int)(-1) && upperFrame != (unsigned int)(-1) && lowerFrame <= upperFrame) || (lowerFrame == (unsigned int)(-1) && upperFrame == (unsigned int)(-1)));

	Scalars cosineValues(objectPointIds.size());

	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&determineObjectPointsAccuracySubset, &database, &pinholeCamera, objectPointIds.data(), accuracyMethhod, lowerFrame, upperFrame, cosineValues.data(), 0u, 0u), 0u, (unsigned int)objectPointIds.size());
	else
		determineObjectPointsAccuracySubset(&database, &pinholeCamera, objectPointIds.data(), accuracyMethhod, lowerFrame, upperFrame, cosineValues.data(), 0u, (unsigned int)objectPointIds.size());

	return cosineValues;
}

void Solver3::determineProjectionErrors(const AnyCamera& camera, const Vector3& objectPoint, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector2>& imagePoints, Scalar* minimalSqrError, Scalar* averageSqrError, Scalar* maximalSqrError)
{
	ocean_assert(camera.isValid());
	ocean_assert(world_T_cameras.size() == imagePoints.size());

	Scalar minError = Numeric::maxValue();
	Scalar totalError = 0;
	Scalar maxError = 0;

	for (size_t n = 0; n < world_T_cameras.size(); ++n)
	{
		const Scalar errorValue = imagePoints[n].sqrDistance(camera.projectToImage(world_T_cameras[n], objectPoint));

		minError = std::min(minError, errorValue);
		maxError = std::max(maxError, errorValue);

		totalError += errorValue;
	}

	if (minimalSqrError != nullptr)
	{
		*minimalSqrError = minError;
	}

	if (world_T_cameras.size() != 0 && averageSqrError != nullptr)
	{
		*averageSqrError = totalError / Scalar(world_T_cameras.size());
	}

	if (maximalSqrError != nullptr)
	{
		*maximalSqrError = maxError;
	}
}

bool Solver3::determineProjectionError(const Database& database, const PinholeCamera& pinholeCamera, const Index32 poseId, const bool useDistortionParameters, unsigned int* validCorrespondences, Scalar* minimalSqrError, Scalar* averageSqrError, Scalar* maximalSqrError)
{
	ocean_assert(pinholeCamera.isValid());

	if (validCorrespondences)
		*validCorrespondences = 0u;

	HomogenousMatrix4 pose;
	if (!database.hasPose<false>(poseId, &pose) || pose.isNull())
		return false;

	const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	database.imagePointsObjectPoints<false, false>(poseId, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
	ocean_assert(imagePoints.size() == objectPoints.size());

	if (imagePoints.empty())
		return false;

	Scalar minError = Numeric::maxValue();
	Scalar totalError = 0;
	Scalar maxError = 0;

	for (size_t n = 0; n < imagePoints.size(); ++n)
	{
		const Scalar errorValue = imagePoints[n].sqrDistance(pinholeCamera.projectToImageIF<true>(poseIF, objectPoints[n], useDistortionParameters));

		minError = min(minError, errorValue);
		totalError += errorValue;
		maxError = max(maxError, errorValue);
	}

	if (validCorrespondences)
		*validCorrespondences = (unsigned int)imagePoints.size();

	if (minimalSqrError)
		*minimalSqrError = minError;

	ocean_assert(imagePoints.size() != 0);
	if (averageSqrError)
		*averageSqrError = totalError / Scalar(imagePoints.size());

	if (maximalSqrError)
		*maximalSqrError = maxError;

	return true;
}

bool Solver3::determineProjectionErrors(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& objectPointIds, const bool useDistortionParameters, const unsigned int lowerFrame, const unsigned int upperFrame, Scalar* minimalSqrErrors, Scalar* averagedSqrErrors, Scalar* maximalSqrErrors, unsigned int* observations, Worker* worker)
{
	ocean_assert((lowerFrame != (unsigned int)(-1) && upperFrame != (unsigned int)(-1) && lowerFrame <= upperFrame) || (lowerFrame == (unsigned int)(-1) && upperFrame == (unsigned int)(-1)));

	Index32 lowerId, upperId;
	if (!database.poseBorders<false>(lowerId, upperId))
	{
		return false;
	}

	if (lowerFrame != (unsigned int)(-1) && upperFrame != (unsigned int)(-1) && lowerFrame <= upperFrame)
	{
		lowerId = lowerFrame;
		upperId = upperFrame;
	}

	const HomogenousMatrices4 poses(database.poses<false>(lowerId, upperId));

	HomogenousMatrices4 posesIF(poses.size(), HomogenousMatrix4(false));

	for (size_t n = 0; n < poses.size(); ++n)
	{
		if (poses[n].isValid())
		{
			posesIF[n] = PinholeCamera::standard2InvertedFlipped(poses[n]);
		}
	}

	ocean_assert(upperId - lowerId + 1 == poses.size());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&determineProjectionErrorsSubset, &database, &pinholeCamera, objectPointIds.data(), (const HomogenousMatrix4*)posesIF.data(), lowerId, upperId, useDistortionParameters, minimalSqrErrors, averagedSqrErrors, maximalSqrErrors, observations, 0u, 0u), 0u, (unsigned int)objectPointIds.size());
	}
	else
	{
		determineProjectionErrorsSubset(&database, &pinholeCamera, objectPointIds.data(), posesIF.data(), lowerId, upperId, useDistortionParameters, minimalSqrErrors, averagedSqrErrors, maximalSqrErrors, observations, 0u, (unsigned int)objectPointIds.size());
	}

	return true;
}

void Solver3::determinePosesOrientation(const Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, Scalar* xOrientations, Scalar* yOrientations, Scalar* zOrientations)
{
	ocean_assert(lowerFrame <= upperFrame);
	ocean_assert(xOrientations && yOrientations && zOrientations);

	const HomogenousMatrices4 poses(database.poses<false>(lowerFrame, upperFrame));

	Vector3 xMeanDirection(0, 0, 0);
	Vector3 yMeanDirection(0, 0, 0);
	Vector3 zMeanDirection(0, 0, 0);

	for (HomogenousMatrices4::const_iterator i = poses.begin(); i != poses.end(); ++i)
	{
		ocean_assert(i->isValid());

		ocean_assert(Vector3((*i)[0], (*i)[1], (*i)[2]) == i->rotationMatrix().xAxis());
		ocean_assert(Vector3((*i)[4], (*i)[5], (*i)[6]) == i->rotationMatrix().yAxis());
		ocean_assert(Vector3((*i)[8], (*i)[9], (*i)[10]) == i->rotationMatrix().zAxis());

		xMeanDirection += Vector3((*i)[0], (*i)[1], (*i)[2]);
		yMeanDirection += Vector3((*i)[4], (*i)[5], (*i)[6]);
		zMeanDirection += Vector3((*i)[8], (*i)[9], (*i)[10]);
	}

	// if the mean directions cannot be normalized than the scalar product will be zero, thus we are fine and do not need further normalization-handling
	xMeanDirection.normalize();
	yMeanDirection.normalize();
	zMeanDirection.normalize();

	for (size_t n = 0; n < poses.size(); ++n)
	{
		const HomogenousMatrix4& pose = poses[n];

		ocean_assert(xMeanDirection.x() * pose[0] + xMeanDirection.y() * pose[1] + xMeanDirection.z() * pose[2] == xMeanDirection * pose.rotationMatrix().xAxis());
		ocean_assert(yMeanDirection.x() * pose[4] + yMeanDirection.y() * pose[5] + yMeanDirection.z() * pose[6] == yMeanDirection * pose.rotationMatrix().yAxis());
		ocean_assert(zMeanDirection.x() * pose[8] + zMeanDirection.y() * pose[9] + zMeanDirection.z() * pose[10] == zMeanDirection * pose.rotationMatrix().zAxis());

		xOrientations[n] = xMeanDirection.x() * pose[0] + xMeanDirection.y() * pose[1] + xMeanDirection.z() * pose[2];
		yOrientations[n] = yMeanDirection.x() * pose[4] + yMeanDirection.y() * pose[5] + yMeanDirection.z() * pose[6];
		zOrientations[n] = zMeanDirection.x() * pose[8] + zMeanDirection.y() * pose[9] + zMeanDirection.z() * pose[10];
	}
}

bool Solver3::determineNumberCorrespondences(const Database& database, const bool needValidPose, const unsigned int lowerFrame, const unsigned int upperFrame, unsigned int* minimalCorrespondences, Scalar* averageCorrespondences, unsigned int* medianCorrespondences, unsigned int* maximalCorrespondences, Worker* worker)
{
	ocean_assert(lowerFrame <= upperFrame);

	Indices32 posesCorrespondences;

	if (needValidPose)
		posesCorrespondences = database.numberCorrespondences<false, false, true>(lowerFrame, upperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(-1), worker);
	else
		posesCorrespondences = database.numberCorrespondences<false, false, false>(lowerFrame, upperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(-1), worker);

	if (posesCorrespondences.empty())
		return false;

	unsigned int minCorrespondences = 0xFFFFFFFFu;
	unsigned int totalCorrespondences = 0u;
	unsigned int maxCorrespondences = 0u;
	for (Indices32::const_iterator i = posesCorrespondences.begin(); i != posesCorrespondences.end(); ++i)
	{
		if (*i < minCorrespondences)
			minCorrespondences = *i;
		if (*i > maxCorrespondences)
			maxCorrespondences = *i;

		totalCorrespondences += *i;
	}

	ocean_assert(!posesCorrespondences.empty());

	if (minimalCorrespondences)
		*minimalCorrespondences = minCorrespondences;

	if (averageCorrespondences)
		*averageCorrespondences = Scalar(totalCorrespondences) / Scalar(posesCorrespondences.size());

	if (maximalCorrespondences)
		*maximalCorrespondences = maxCorrespondences;

	if (medianCorrespondences)
		*medianCorrespondences = Median::median(posesCorrespondences.data(), posesCorrespondences.size());

	return true;
}

bool Solver3::determinePlane(const Database& database, const Index32 frameIndex, const CV::SubRegion& subRegion, RandomGenerator& randomGenerator, Plane3& plane, const RelativeThreshold& minimalValidObjectPoints, const Geometry::Estimator::EstimatorType estimator, Scalar* finalError, Indices32* usedObjectPointIds)
{
	ocean_assert(database.hasPose<false>(frameIndex));
	ocean_assert(!subRegion.isEmpty());

	// we check whether the given frame has a valid pose
	HomogenousMatrix4 pose;
	if (!database.hasPose<false>(frameIndex, &pose) || !pose.isValid())
		return false;

	// we first need all 2D/3D point correspondences for the specified frame with valid 3D object point location

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Indices32 objectPointIds;
	database.imagePointsObjectPoints<false, false>(frameIndex, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), 0, nullptr, usedObjectPointIds ? &objectPointIds : nullptr);

	if (imagePoints.size() < 3)
		return false;

	// now we identify all point correspondences for which the image point lies inside the specified sub-region

	Indices32 validRegionIndices;
	validRegionIndices.reserve(imagePoints.size());

	for (unsigned int n = 0u; n < imagePoints.size(); ++n)
		if (subRegion.isInside(imagePoints[n]))
			validRegionIndices.push_back(n);

	if (validRegionIndices.size() < 3)
		return false;

	Indices32 validRegionIndicesIndices;
	if (!determinePlane(ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints, validRegionIndices), randomGenerator, plane, minimalValidObjectPoints, estimator, finalError, usedObjectPointIds ? &validRegionIndicesIndices : nullptr))
		return false;

	if (usedObjectPointIds)
	{
		ocean_assert(!objectPointIds.empty());

		const Indices32 validRegionObjectPointIds = Subset::subset(objectPointIds, validRegionIndices);
		*usedObjectPointIds = Subset::subset(validRegionObjectPointIds, validRegionIndicesIndices);
	}

	// we ensure that the normal of the plane is looking towards the camera

	ocean_assert(pose.isValid());
	const Vector3 pointInPlane(plane.projectOnPlane(pose.translation()));
	ocean_assert(plane.isInPlane(pointInPlane));

	const Vector3 directionToPose(pose.translation() - pointInPlane); // may be the zero vector

	if (plane.normal() * directionToPose < 0)
		plane = -plane;

	return true;
}

bool Solver3::determinePlane(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrameIndex, const unsigned int subRegionFrameIndex, const unsigned int upperFrameIndex, const CV::SubRegion& subRegion, RandomGenerator& randomGenerator, Plane3& plane, const bool useDistortionParameters, const RelativeThreshold& minimalValidObjectPoints, const Scalar medianDistanceFactor, const Geometry::Estimator::EstimatorType estimator, Scalar* finalError, Indices32* usedObjectPointIds)
{
	ocean_assert(lowerFrameIndex <= subRegionFrameIndex && subRegionFrameIndex <= upperFrameIndex);
	ocean_assert(medianDistanceFactor >= 0);

	// we determine the initial pose only for the frame for which the sub-region is defined

	Plane3 initialPlane;
	Indices32 initialObjectPointIds;
	if (!determinePlane(database, subRegionFrameIndex, subRegion, randomGenerator, initialPlane, minimalValidObjectPoints, estimator, nullptr, &initialObjectPointIds))
		return false;

	// now we determine the median error between the initial plane and all used object points so that we can define a suitable threshold for all remaining object points

	const Vectors3 initialObjectPoints(database.objectPoints<false>(initialObjectPointIds));

	Scalars initialObjectPointDistances(initialObjectPoints.size());
	for (size_t n = 0; n < initialObjectPoints.size(); ++n)
		initialObjectPointDistances[n] = Numeric::abs(initialPlane.signedDistance(initialObjectPoints[n]));

	const Scalar medianInitialObjectPointDistance = Median::median(initialObjectPointDistances.data(), initialObjectPointDistances.size());
	const Scalar maximalPointDistance = medianInitialObjectPointDistance * medianDistanceFactor;

	// now we investigate all other camera frames and try to find object points which are not visible in the sub-region-frame but also belong to the plane
	// a 3D object point belongs to the plane if the object point is 'near' to the plane and if the corresponding image point lies inside the transformed sub-region
	// the transformed sub-region is determined by back-projecting the sub-region from the sub-region-frame onto the plane and projecting this plane-sub-region into the individual camera frames

	HomogenousMatrix4 subRegionFramePose;
	if (!database.hasPose<false>(subRegionFrameIndex, &subRegionFramePose) || !subRegionFramePose.isValid())
		return false;

	typedef std::map<Index32, Vector3> ObjectPointMap;
	ObjectPointMap objectPointMap;

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Indices32 objectPointIds;

	const bool applyDistortion = useDistortionParameters && pinholeCamera.hasDistortionParameters();

	for (unsigned int frameIndex = lowerFrameIndex; frameIndex <= upperFrameIndex; ++frameIndex)
	{
		HomogenousMatrix4 framePose;
		if (database.hasPose<false>(frameIndex, &framePose) && framePose.isValid())
		{
			const SquareMatrix3 homography = Geometry::Homography::homographyMatrix(framePose, subRegionFramePose, pinholeCamera, pinholeCamera, initialPlane);
			const SquareMatrix3 normalizedHomography(pinholeCamera.invertedIntrinsic() * homography * pinholeCamera.intrinsic());

			imagePoints.clear();
			objectPoints.clear();
			objectPointIds.clear();
			database.imagePointsObjectPoints<false, false>(frameIndex, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), 0, nullptr, &objectPointIds);

			for (size_t n = 0; n < objectPointIds.size(); ++n)
				if (objectPointMap.find(objectPointIds[n]) == objectPointMap.end() && Numeric::abs(initialPlane.signedDistance(objectPoints[n])) < maximalPointDistance)
				{
					if (applyDistortion)
					{
						const Vector2 undistortedNormalizedImagePoint(pinholeCamera.imagePoint2normalizedImagePoint<true>(imagePoints[n], true));
						const Vector2 undistortedNormalizedSubRegionFrameImagePoint(normalizedHomography * undistortedNormalizedImagePoint);
						const Vector2 subRegionFrameImagePoint(pinholeCamera.normalizedImagePoint2imagePoint<true>(undistortedNormalizedSubRegionFrameImagePoint, true));

						if (subRegion.isInside(subRegionFrameImagePoint))
							objectPointMap.insert(std::make_pair(objectPointIds[n], objectPoints[n]));
					}
					else
					{
						if (subRegion.isInside(homography * imagePoints[n]))
							objectPointMap.insert(std::make_pair(objectPointIds[n], objectPoints[n]));
					}
				}
		}
	}

	ocean_assert(objectPointMap.size() >= 3);

	// now we take all the locations of the 3D object points to determine a final plane which is based on all 3D object points

	Vectors3 planeObjectPoints;
	planeObjectPoints.reserve(objectPointMap.size());
	for (ObjectPointMap::const_iterator i = objectPointMap.begin(); i != objectPointMap.end(); ++i)
		planeObjectPoints.push_back(i->second);

#ifdef OCEAN_DEBUG
	{
		Indices32 debugObjectPointIds;
		for (ObjectPointMap::const_iterator i = objectPointMap.begin(); i != objectPointMap.end(); ++i)
			debugObjectPointIds.push_back(i->first);
		const Vectors3 debugObjectPoints = database.objectPoints<false>(debugObjectPointIds);
		ocean_assert(debugObjectPoints == planeObjectPoints);
	}
#endif

	if (!Geometry::NonLinearOptimizationPlane::optimizePlane(initialPlane, ConstArrayAccessor<Vector3>(planeObjectPoints), plane, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalError))
		return false;

	if (usedObjectPointIds)
	{
		ocean_assert(usedObjectPointIds->empty());
		usedObjectPointIds->clear();
		usedObjectPointIds->reserve(objectPointMap.size());
		for (ObjectPointMap::const_iterator i = objectPointMap.begin(); i != objectPointMap.end(); ++i)
			usedObjectPointIds->push_back(i->first);
	}

	return true;
}

bool Solver3::determinePerpendicularPlane(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int frameIndex, const Vector2& imagePoint, const Scalar distance, Plane3& plane, const bool useDistortionParameters, Vector3* pointOnPlane)
{
	HomogenousMatrix4 pose;
	if (!database.hasPose<false>(frameIndex, &pose) || !pose.isValid())
		return false;

	return determinePerpendicularPlane(pinholeCamera, pose, imagePoint, distance, plane, useDistortionParameters, pointOnPlane);
}

bool Solver3::determinePerpendicularPlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector2& imagePoint, const Scalar distance, Plane3& plane, const bool useDistortionParameters, Vector3* pointOnPlane)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid() && pinholeCamera.isInside(imagePoint));
	ocean_assert(distance > Numeric::eps());

	if (!pinholeCamera.isValid() || !pose.isValid())
		return false;

	const Line3 ray(pinholeCamera.ray(useDistortionParameters ? pinholeCamera.undistort<true>(imagePoint) : imagePoint, pose));

	ocean_assert(Numeric::isEqual(ray.direction().length(), 1));

	const Vector3 objectPoint(ray.point(distance));
	plane = Plane3(objectPoint, -ray.direction());

	if (pointOnPlane)
		*pointOnPlane = objectPoint;

	return true;
}

bool Solver3::removeSparseObjectPoints(Database& database, const Scalar minimalBoundingBoxDiagonal, const Scalar medianFactor, const Scalar maximalSparseObjectPointRatio)
{
	Log::info() << "We check whether the database holds very sparse object points which should be removed";

	Indices32 validObjectPointIds;
	const Vectors3 validObjectPoints = database.objectPoints<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), &validObjectPointIds);

	if (validObjectPoints.empty())
		return false;

	const Box3 boundingBox(validObjectPoints);
	const Scalar diagonal = boundingBox.diagonal();

	Log::info() << "The diagonal of the object points' bounding box: " << diagonal;

	if (diagonal < minimalBoundingBoxDiagonal)
		return false;

	// now we determine the median distance between all valid object points and the median object point location

	const Vector3 medianPoint = Geometry::Utilities::medianObjectPoint(ConstTemplateArrayAccessor<Vector3>(validObjectPoints));
	const Scalar medianDistance = Geometry::Utilities::medianDistance(medianPoint, ConstTemplateArrayAccessor<Vector3>(validObjectPoints));

	Indices32 farObjectPointIds;

	for (size_t i = 0; i < validObjectPoints.size(); ++i)
		if (medianPoint.sqrDistance(validObjectPoints[i]) > Numeric::sqr(medianDistance * medianFactor))
			farObjectPointIds.push_back(validObjectPointIds[i]);

	ocean_assert(validObjectPoints.size() != 0);
	Log::info() << "The amount of far object points: " << Scalar(farObjectPointIds.size()) * Scalar(100) / Scalar(validObjectPoints.size());

	if (Scalar(farObjectPointIds.size()) / Scalar(validObjectPoints.size()) > maximalSparseObjectPointRatio)
		return false;

	Log::info() << "Therefore we remove the far object points and try the bundle adjustment again";

	database.setObjectPoints<false>(farObjectPointIds.data(), farObjectPointIds.size(), Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

	Log::info() << "Now the bounding box has the following diagonal: " << Box3(database.objectPoints<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()))).diagonal();

	return true;
}

size_t Solver3::removeObjectPointsNotInFrontOfCamera(Database& database, Indices32* removedObjectPointIds)
{
	size_t removedObjectPoints = 0;

	Vectors3 objectPoints;
	const Indices32 objectPointIds = database.objectPointIds<false, false>(Database::invalidObjectPoint(), &objectPoints);

	Indices32 poseIds;
	Indices32 imagePointIds;
	Vectors2 imagePoints;

	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		const Index32& objectPointId = objectPointIds[n];
		const Vector3& objectPoint = objectPoints[n];

		poseIds.clear();
		imagePointIds.clear();
		imagePoints.clear();

		database.observationsFromObjectPoint<false>(objectPointId, poseIds, imagePointIds, &imagePoints);

		const HomogenousMatrices4 world_T_cameras = database.poses<false>(poseIds.data(), poseIds.size());

		bool objectPointInFront = true;

		for (size_t nCorrespondences = 0; objectPointInFront && nCorrespondences < imagePoints.size(); ++nCorrespondences)
		{
			const HomogenousMatrix4& world_T_camera = world_T_cameras[nCorrespondences];

			if (world_T_camera.isValid())
			{
				objectPointInFront = PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), objectPoint);
			}
		}

		if (!objectPointInFront)
		{
			database.removeObjectPointAndAttachedImagePoints<false>(objectPointId);

			if (removedObjectPointIds != nullptr)
			{
				removedObjectPointIds->emplace_back(objectPointId);
			}

			++removedObjectPoints;
		}
	}

	return removedObjectPoints;
}

size_t Solver3::removeObjectPointsWithoutEnoughObservations(Database& database, const size_t minimalNumberObservations, Indices32* removedObjectPointIds)
{
	ocean_assert(minimalNumberObservations >= 1);

	size_t removedObjectPoints = 0;

	const Indices32 objectPointIds = database.objectPointIds<false>();

	for (const Index32& objectPointId : objectPointIds)
	{
		const size_t numberImagePoints = database.numberImagePointsFromObjectPoint<false>(objectPointId);

		if (numberImagePoints < minimalNumberObservations)
		{
			database.removeObjectPointAndAttachedImagePoints<false>(objectPointId);

			if (removedObjectPointIds != nullptr)
			{
				removedObjectPointIds->emplace_back(objectPointId);
			}

			++removedObjectPoints;
		}
	}

	return removedObjectPoints;
}

size_t Solver3::removeObjectPointsWithSmallBaseline(Database& database, const Scalar minimalBoxDiagonal, Indices32* removedObjectPointIds)
{
	ocean_assert(minimalBoxDiagonal > 0);

	size_t removedObjectPoints = 0;

	Indices32 objectPointIds = database.objectPointIds<false>();

	for (const Index32& objectPointId : objectPointIds)
	{
		const IndexSet32 poseIds = database.posesFromObjectPoint<false>(objectPointId);

		Box3 box;

		for (const Index32& poseId : poseIds)
		{
			const HomogenousMatrix4& pose = database.pose<false>(poseId);

			if (pose.isValid())
			{
				box += pose.translation();
			}
		}

		if (!box.isValid() || box.diagonal() < minimalBoxDiagonal)
		{
			database.removeObjectPointAndAttachedImagePoints<false>(objectPointId);

			if (removedObjectPointIds != nullptr)
			{
				removedObjectPointIds->emplace_back(objectPointId);
			}

			++removedObjectPoints;
		}
	}

	return removedObjectPoints;
}

void Solver3::determineObjectPointsAccuracySubset(const Database* database, const PinholeCamera* pinholeCamera, const Index32* objectPointIds, const AccuracyMethod accuracyMethhod, const unsigned int lowerFrame, const unsigned int upperFrame, Scalar* values, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints)
{
	ocean_assert(database && pinholeCamera && pinholeCamera->isValid() && values);

	ocean_assert(lowerFrame == Database::invalidId || upperFrame == Database::invalidId || lowerFrame <= upperFrame);

	HomogenousMatrices4 poses;
	Vectors2 imagePoints;

	for (unsigned int n = firstObjectPoint; n < firstObjectPoint + numberObjectPoints; ++n)
	{
		poses.clear();
		imagePoints.clear();

		database->posesImagePoints<false, false>(objectPointIds[n], poses, imagePoints, HomogenousMatrix4(false), nullptr, nullptr, lowerFrame, upperFrame);
		ocean_assert(poses.size() == imagePoints.size());

		if (poses.empty())
			values[n] = -1;
		else
			values[n] = determineObjectPointAccuracy(*pinholeCamera, poses.data(), imagePoints.data(), poses.size(), accuracyMethhod);
	}
}

void Solver3::determineProjectionErrorsSubset(const Database* database, const PinholeCamera* pinholeCamera, const Index32* objectPointIds, const HomogenousMatrix4* posesIF, const Index32 lowerPoseId, const unsigned int upperPoseId, const bool useDistortionParameters, Scalar* minimalSqrErrors, Scalar* averagedSqrErrors, Scalar* maximalSqrErrors, unsigned int* observations, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints)
{
	ocean_assert(database && pinholeCamera && objectPointIds && posesIF);

	Indices32 poseIds, imagePointIds;
	Vectors2 imagePoints;

	for (unsigned int n = firstObjectPoint; n < firstObjectPoint + numberObjectPoints; ++n)
	{
		poseIds.clear();
		imagePointIds.clear();
		imagePoints.clear();

		const Index32 objectPointId = objectPointIds[n];

		const Vector3& objectPoint = database->objectPoint<false>(objectPointId);

		database->observationsFromObjectPoint<false>(objectPointId, poseIds, imagePointIds, &imagePoints);
		ocean_assert(poseIds.size() == imagePoints.size());

		Scalar minimalSqrDistance = Numeric::maxValue();
		Scalar maximalSqrDistance = poseIds.empty() ? Numeric::maxValue() : Scalar(0);

		Scalar totalSqrDistance = 0;

		unsigned int validObservations = 0u;

		for (size_t i = 0; i < poseIds.size(); ++i)
		{
			const Index32 poseId = poseIds[i];

			if (poseId >= lowerPoseId && poseId <= upperPoseId && posesIF[poseId - lowerPoseId].isValid())
			{
				const Vector2 projectedObjectPoint(pinholeCamera->projectToImageIF<true>(posesIF[poseId - lowerPoseId], objectPoint, useDistortionParameters));

				const Scalar sqrDistance = imagePoints[i].sqrDistance(projectedObjectPoint);

				if (sqrDistance > maximalSqrDistance)
					maximalSqrDistance = sqrDistance;

				if (sqrDistance < minimalSqrDistance)
					minimalSqrDistance = sqrDistance;

				totalSqrDistance += sqrDistance;
				validObservations++;
			}
		}

		if (maximalSqrErrors)
			maximalSqrErrors[n] = maximalSqrDistance;
		if (minimalSqrErrors)
			minimalSqrErrors[n] = minimalSqrDistance;

		if (averagedSqrErrors)
		{
			if (validObservations == 0u)
				averagedSqrErrors[n] = Numeric::maxValue();
			else
				averagedSqrErrors[n] = totalSqrDistance / Scalar(validObservations);
		}

		if (observations)
			observations[n] = validObservations;
	}
}

size_t Solver3::filterStaticImagePoints(ImagePointGroups& imagePointGroups, Indices32& objectPointIds, const Scalar maximalStaticImagePointFilterRatio)
{
#ifdef OCEAN_DEBUG
	ocean_assert(imagePointGroups.size() >= 0 && imagePointGroups.front().size());
	ocean_assert(maximalStaticImagePointFilterRatio >= 0 && maximalStaticImagePointFilterRatio <= 1);

	for (ImagePointGroups::Index n = imagePointGroups.firstIndex(); n <= imagePointGroups.lastIndex(); ++n)
		ocean_assert(imagePointGroups[n].size() == imagePointGroups.front().size());
#endif

	if (maximalStaticImagePointFilterRatio <= 0 || imagePointGroups.isEmpty() || imagePointGroups.front().empty())
		return 0;

	Vectors2 meanImagePoints(imagePointGroups.front().size(), Vector2(0, 0));

	for (ShiftVector<Vectors2>::Index n = imagePointGroups.firstIndex(); n < imagePointGroups.endIndex(); ++n)
		for (size_t i = 0; i < meanImagePoints.size(); ++i)
			meanImagePoints[i] += imagePointGroups[n][i];

	const Scalar factor = Scalar(1) / Scalar(imagePointGroups.size());

	for (size_t n = 0; n < meanImagePoints.size(); ++n)
		meanImagePoints[n] *= factor;

	size_t staticImagePointNumber = meanImagePoints.size();
	std::vector<unsigned char> staticImagePoints(meanImagePoints.size(), 1u);

	for (ShiftVector<Vectors2>::Index n = imagePointGroups.firstIndex(); staticImagePointNumber != 0 && n < imagePointGroups.endIndex(); ++n)
		for (size_t i = 0; staticImagePointNumber != 0 && i < meanImagePoints.size(); ++i)
			if (staticImagePoints[i] != 0u && meanImagePoints[i].sqrDistance(imagePointGroups[n][i]) > Scalar(1.5 * 1.5))
			{
				staticImagePoints[i] = 0u;

				ocean_assert(staticImagePointNumber >= 1);
				staticImagePointNumber--;
			}

	ocean_assert(staticImagePointNumber <= meanImagePoints.size());

	const Scalar staticRatio = Scalar(staticImagePointNumber) / Scalar(meanImagePoints.size());

	if (staticImagePointNumber != 0u && staticRatio <= maximalStaticImagePointFilterRatio)
	{
		const Indices32 nonStaticImagePointIndices(Subset::statements2indices<Index32, 0u>(staticImagePoints));

		objectPointIds = Subset::subset(objectPointIds, nonStaticImagePointIndices);
		for (ShiftVector<Vectors2>::Index n = imagePointGroups.firstIndex(); n < imagePointGroups.endIndex(); ++n)
			imagePointGroups[n] = Subset::subset(imagePointGroups[n], nonStaticImagePointIndices);
	}

	return staticImagePointNumber;
}

void Solver3::determineInitialObjectPointsFromSparseKeyFramesByStepsSubset(const Database* database, const PinholeCamera* pinholeCamera, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const Indices32* startFrames, const unsigned int upperFrame, const Scalar maximalStaticImagePointFilterRatio, Vectors3* initialObjectPoints, Indices32* initialObjectPointIds, Indices32* initialPoseIds, Scalar* initialPointDistance, const RelativeThreshold* pointsThreshold, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne)
{
	ocean_assert(database && pinholeCamera && pinholeCamera->isValid() && startFrames && !startFrames->empty());
	ocean_assert(lowerFrame <=  upperFrame);
	ocean_assert_and_suppress_unused(numberThreadsOne == 1u, numberThreadsOne);
	ocean_assert(numberThreads == 1u || lock);
	ocean_assert(randomGenerator);

	RandomGenerator localRandomGenerator(*randomGenerator);

	Vectors3 iterationObjectPoints;
	Indices32 iterationObjectPointIds;
	Indices32 iterationPoseIds;

	for (unsigned int n = threadIndex; (!abort || !*abort) && n < startFrames->size(); n += numberThreads)
	{
		const unsigned int startFrame = (*startFrames)[n];

		ocean_assert(lowerFrame <= startFrame && startFrame <= upperFrame);

		Scalar iterationSqrError = Numeric::maxValue();
		Scalar iterationPointDistance = 0;

		iterationObjectPoints.clear();
		iterationObjectPointIds.clear();
		iterationPoseIds.clear();

		if (determineInitialObjectPointsFromSparseKeyFrames(*database, *pinholeCamera, localRandomGenerator, lowerFrame, startFrame, upperFrame, maximalStaticImagePointFilterRatio, iterationObjectPoints, iterationObjectPointIds, *pointsThreshold, minimalKeyFrames, maximalKeyFrames, maximalSqrError, &iterationPoseIds, &iterationSqrError, &iterationPointDistance, abort))
		{
			const size_t iterationProduct = iterationObjectPoints.size() * iterationPoseIds.size();

			const OptionalScopedLock scopedLock(lock);

			const size_t currentBestProduct = initialObjectPoints->size() * initialPoseIds->size();

			if (iterationProduct > currentBestProduct || (iterationProduct == currentBestProduct && iterationPointDistance > *initialPointDistance))
			{
				*initialPointDistance = iterationPointDistance;
				*initialPoseIds = std::move(iterationPoseIds);

				*initialObjectPoints = std::move(iterationObjectPoints);
				*initialObjectPointIds = std::move(iterationObjectPointIds);
			}
		}
	}
}

void Solver3::determineInitialObjectPointsFromDenseFramesRANSACSubset(const PinholeCamera* pinholeCamera, const ImagePointGroups* imagePointGroups, RandomGenerator* randomGenerator, HomogenousMatrices4* validPoses, Indices32* validPoseIds, Vectors3* objectPoints, Indices32* validObjectPointIndices, Scalar* totalError, const RelativeThreshold* minimalValidObjectPoints, const Scalar maximalSqrError, unsigned int* remainingIterations, Lock* lock, bool* abort, unsigned int /*firstIteration*/, unsigned int /*numberIterations*/)
{
	ocean_assert(pinholeCamera && pinholeCamera->isValid());
	ocean_assert(imagePointGroups->size() >= 2);
	ocean_assert(randomGenerator && minimalValidObjectPoints);
	ocean_assert(remainingIterations);

	RandomGenerator localRandomGenerator(*randomGenerator);

	Scalar localTotalError = Numeric::maxValue();
	HomogenousMatrices4 localValidPoses;
	Indices32 localValidPoseIds;
	Vectors3 localObjectPoints;
	Indices32 localValidObjectPointIndices;

	HomogenousMatrices4 iterationValidPoses;
	Indices32 iterationValidPoseIds;
	Vectors3 iterationObjectPoints;
	Indices32 iterationObjectPointIndices;

	while (!abort || !*abort)
	{
		unsigned int index0, index1;
		RandomI::random(localRandomGenerator, (unsigned int)imagePointGroups->size() - 1u, index0, index1);
		ocean_assert(index0 != index1);

		index0 += (unsigned int)imagePointGroups->firstIndex();
		index1 += (unsigned int)imagePointGroups->firstIndex();

		iterationValidPoses.clear();
		iterationValidPoseIds.clear();
		iterationObjectPoints.clear();
		iterationObjectPointIndices.clear();
		Scalar iterationTotalError;

		if (determineInitialObjectPointsFromDenseFrames(*pinholeCamera, *imagePointGroups, localRandomGenerator, index0, index1, iterationValidPoses, iterationValidPoseIds, iterationTotalError, iterationObjectPoints, iterationObjectPointIndices, *minimalValidObjectPoints, maximalSqrError))
		{
			ocean_assert(iterationObjectPoints.size() == iterationObjectPointIndices.size());

			// our target is to find several object points visible in several camera poses
			// bad: 2 poses, large number of object points
			// good: several poses, several object points
			// bad: several poses, a small number of object points
			// therefore, we take the product of the number of poses and object points as measure for a good result

			if (iterationValidPoseIds.size() * iterationObjectPoints.size() > localValidPoseIds.size() * localObjectPoints.size()
					|| (iterationValidPoseIds.size() * iterationObjectPoints.size() == localValidPoseIds.size() * localObjectPoints.size() && iterationTotalError < localTotalError))
			{
				localValidPoses = std::move(iterationValidPoses);
				localValidPoseIds = std::move(iterationValidPoseIds);

				localObjectPoints = std::move(iterationObjectPoints);
				localValidObjectPointIndices = std::move(iterationObjectPointIndices);

				localTotalError = iterationTotalError;
			}
		}

		// now we check whether we can stop as all parallel thread have handled the number of requested iterations
		const OptionalScopedLock scopedlock(lock);

		if (*remainingIterations == 0u)
			break;

		(*remainingIterations)--;
	}

	if (!abort || !*abort)
	{
		const OptionalScopedLock scopedLock(lock);

		if (localValidPoseIds.size() * localObjectPoints.size() > validPoseIds->size() * objectPoints->size()
				|| (localValidPoseIds.size() * localObjectPoints.size() == validPoseIds->size() * objectPoints->size() && localTotalError < *totalError))
		{
			*validPoses = std::move(localValidPoses);
			*validPoseIds = std::move(localValidPoseIds);

			*objectPoints = std::move(localObjectPoints);
			*validObjectPointIndices = std::move(localValidObjectPointIndices);

			*totalError = localTotalError;
		}
	}
}

void Solver3::updatePosesSubset(Database* database, const AnyCamera* camera, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, size_t* validPoses, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne)
{
	ocean_assert(database != nullptr);
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(lowerFrame <=  upperFrame);
	ocean_assert(minimalCorrespondences >= 5u);
	ocean_assert_and_suppress_unused(numberThreadsOne == 1u, numberThreadsOne);
	ocean_assert(randomGenerator != nullptr);

	RandomGenerator localRandomGenerator(*randomGenerator);

	Scalar localTotalError = 0;
	size_t localValidPoses = 0;
	HomogenousMatrix4 previousPose(false);

	for (unsigned int frameIndex = lowerFrame + threadIndex; (!abort || !*abort) && frameIndex <= upperFrame; frameIndex += numberThreads)
	{
		// check whether the database does not have any information about this frame
		if (!database->hasPose<false>(frameIndex))
		{
			previousPose.toNull();
			continue;
		}

		Scalar errorValue = Numeric::maxValue();

		const HomogenousMatrix4& oldPose = database->pose<false>(frameIndex);

		unsigned int correspondences;
		HomogenousMatrix4 currentPose(determinePose(*database, *camera, localRandomGenerator, frameIndex, oldPose.isValid() ? oldPose : previousPose, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue, &correspondences));

		// check whether the resulting pose is not accurate enough
		if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
		{
			currentPose.toNull();
		}

		database->setPose<false>(frameIndex, currentPose);
		previousPose = currentPose;

		if (currentPose.isValid())
		{
			localTotalError += errorValue;
			localValidPoses++;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	*totalError += localTotalError;
	*validPoses += localValidPoses;
}

void Solver3::updateOrientationsSubset(Database* database, const AnyCamera* camera, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, size_t* validPoses, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne)
{
	ocean_assert(database != nullptr);
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(lowerFrame <=  upperFrame);
	ocean_assert(minimalCorrespondences >= 5u);
	ocean_assert_and_suppress_unused(numberThreadsOne == 1u, numberThreadsOne);
	ocean_assert(randomGenerator != nullptr);

	RandomGenerator localRandomGenerator(*randomGenerator);

	Scalar localTotalError = 0;
	size_t localValidPoses = 0;
	SquareMatrix3 previousOrientation(false);

	for (unsigned int frameIndex = lowerFrame + threadIndex; (!abort || !*abort) && frameIndex <= upperFrame; frameIndex += numberThreads)
	{
		// check whether the database does not have any information about this frame
		if (!database->hasPose<false>(frameIndex))
		{
			previousOrientation.toNull();
			continue;
		}

		Scalar errorValue = Numeric::maxValue();

		const HomogenousMatrix4& oldPose = database->pose<false>(frameIndex);

		const SquareMatrix3 oldOrientation = oldPose.isValid() ? oldPose.rotationMatrix() : SquareMatrix3(false);

		unsigned int correspondences;
		SquareMatrix3 currentOrientation(determineOrientation(*database, *camera, localRandomGenerator, frameIndex, oldOrientation.isNull() ? previousOrientation : oldOrientation, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue, &correspondences));

		// check whether the resulting pose is not accurate enough
		if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
		{
			currentOrientation.toNull();
		}

		if (currentOrientation.isNull())
		{
			database->setPose<false>(frameIndex, HomogenousMatrix4(false));
		}
		else
		{
			database->setPose<false>(frameIndex, HomogenousMatrix4(currentOrientation));
		}

		previousOrientation = currentOrientation;

		if (!currentOrientation.isNull())
		{
			localTotalError += errorValue;
			localValidPoses++;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	*totalError += localTotalError;
	*validPoses += localValidPoses;
}

void Solver3::determinePosesSubset(const Database* database, const AnyCamera* camera, const IndexSet32* priorityObjectPointIds, const bool solePriorityPoints, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, ShiftVector<HomogenousMatrix4>* poses, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne)
{
	ocean_assert(database != nullptr);
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(lowerFrame <=  upperFrame);
	ocean_assert(minimalCorrespondences >= 5u);
	ocean_assert_and_suppress_unused(numberThreadsOne == 1u, numberThreadsOne);
	ocean_assert(priorityObjectPointIds && poses);
	ocean_assert(randomGenerator != nullptr);

	RandomGenerator localRandomGenerator(*randomGenerator);

	Scalar localTotalError = 0;
	HomogenousMatrix4 previousPose(false);

	for (unsigned int frameIndex = lowerFrame + threadIndex; (!abort || !*abort) && frameIndex <= upperFrame; frameIndex += numberThreads)
	{
		// check whether the database does not have any information about this frame
		if (!database->hasPose<false>(frameIndex))
		{
			(*poses)[frameIndex].toNull();
			previousPose.toNull();

			continue;
		}

		Scalar errorValue = Numeric::maxValue();

		const HomogenousMatrix4& oldPose = database->pose<false>(frameIndex);

		unsigned int correspondences;
		HomogenousMatrix4 currentPose;

		if (priorityObjectPointIds->empty())
		{
			currentPose = determinePose(*database, *camera, localRandomGenerator, frameIndex, oldPose.isValid() ? oldPose : previousPose, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue, &correspondences);
		}
		else
		{
			currentPose = determinePose(*database, *camera, localRandomGenerator, frameIndex, *priorityObjectPointIds, solePriorityPoints, oldPose.isValid() ? oldPose : previousPose, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue, &correspondences);
		}

		// check whether the resulting pose is not accurate enough
		if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
		{
			currentPose.toNull();
		}

		ocean_assert(poses->isValidIndex(frameIndex));
		(*poses)[frameIndex] = currentPose;

		previousPose = currentPose;

		if (currentPose.isValid())
		{
			localTotalError += errorValue;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	*totalError += localTotalError;
}

void Solver3::determineOrientationsSubset(const Database* database, const AnyCamera* camera, const IndexSet32* priorityObjectPointIds, const bool solePriorityPoints, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, ShiftVector<HomogenousMatrix4>* poses, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne)
{
	ocean_assert(database != nullptr);
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(lowerFrame <=  upperFrame);
	ocean_assert(minimalCorrespondences >= 5u);
	ocean_assert_and_suppress_unused(numberThreadsOne == 1u, numberThreadsOne);
	ocean_assert(priorityObjectPointIds && poses);
	ocean_assert(randomGenerator != nullptr);

	RandomGenerator localRandomGenerator(*randomGenerator);

	Scalar localTotalError = 0;
	SquareMatrix3 previousOrientation(false);

	for (unsigned int frameIndex = lowerFrame + threadIndex; (!abort || !*abort) && frameIndex <= upperFrame; frameIndex += numberThreads)
	{
		// check whether the database does not have any information about this frame
		if (!database->hasPose<false>(frameIndex))
		{
			(*poses)[frameIndex].toNull();
			previousOrientation.toNull();

			continue;
		}

		Scalar errorValue = Numeric::maxValue();

		const SquareMatrix3 oldOrientation = database->pose<false>(frameIndex).rotationMatrix();

		unsigned int correspondences;
		SquareMatrix3 currentOrientation;

		if (priorityObjectPointIds->empty())
		{
			currentOrientation = determineOrientation(*database, *camera, localRandomGenerator, frameIndex, oldOrientation.isNull() ? previousOrientation : oldOrientation, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue, &correspondences);
		}
		else
		{
			currentOrientation = determineOrientation(*database, *camera, localRandomGenerator, frameIndex, *priorityObjectPointIds, solePriorityPoints, oldOrientation.isNull() ? previousOrientation : oldOrientation, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, &errorValue, &correspondences);
		}

		// check whether the resulting pose is not accurate enough
		if (errorValue != Numeric::maxValue() && errorValue > maximalRobustError)
		{
			currentOrientation.toNull();
		}

		ocean_assert(poses->isValidIndex(frameIndex));

		if (currentOrientation.isNull())
		{
			(*poses)[frameIndex] = HomogenousMatrix4(false);
		}
		else
		{
			(*poses)[frameIndex] = HomogenousMatrix4(currentOrientation);
		}

		previousOrientation = currentOrientation;

		if (!currentOrientation.isNull())
		{
			localTotalError += errorValue;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	*totalError += localTotalError;
}

bool Solver3::updateDatabaseToRotationalMotion(Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalObservations, IndexSet32* relocatedObjectPointIds)
{
	ocean_assert(pinholeCamera.isValid());

	const AnyCameraPinhole anyCamera(pinholeCamera);

	// we identify the pose with most valid correspondences within the specified frame ranges
	// we start at this pose which will receive a default orientation,
	// we create all 3D object point locations for this pose and go further in both directions until we reach the defined frame ranges
	// we add new 3D object points whenever necessary

	Index32 poseId = Tracking::Database::invalidId;
	unsigned int bestCorrespondences = 0u;
	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, &poseId, &bestCorrespondences, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())) || bestCorrespondences < 5u)
	{
		return false;
	}

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Indices32 objectPointIds;
	database.imagePointsObjectPoints<false, false>(poseId, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), minimalObservations, nullptr, &objectPointIds);

	// we determine the new location of the 3D object points and overwrite the locations in the database
	objectPoints = Geometry::Utilities::createObjectPoints(pinholeCamera, HomogenousMatrix4(true), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), 1);
	database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPointIds.size());
	database.setPose<false>(poseId, HomogenousMatrix4(true));

	// now we proceed in both directions trying to determine now 3D object point locations

	IndexSet32 relocatedObjectPointIdSet(objectPointIds.begin(), objectPointIds.end());

	unsigned int leftFrame = poseId;
	unsigned int rightFrame = poseId;

	Indices32 relocatedIndices, pendingIndices;

	while (true)
	{
		unsigned int frameIndex = (unsigned int)(-1);

		bool processedPose = false;

		for (unsigned int i = 0u; i < 2u; ++i)
		{
			if (i == 0u && leftFrame > lowerFrame)
			{
				frameIndex = --leftFrame;
			}
			else if (i == 1u && rightFrame < upperFrame)
			{
				frameIndex = ++rightFrame;
			}

			HomogenousMatrix4 pose;
			if (frameIndex != (unsigned int)(-1) && database.hasPose<false>(frameIndex, &pose) && pose.isValid())
			{
				imagePoints.clear();
				objectPoints.clear();
				objectPointIds.clear();
				relocatedIndices.clear();
				pendingIndices.clear();

				database.imagePointsObjectPoints<false, false>(frameIndex, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), minimalObservations, nullptr, &objectPointIds);

				// we need to separate the already re-located object points from the object points not having a re-located position

				for (unsigned int n = 0; n < objectPointIds.size(); ++n)
				{
					if (relocatedObjectPointIdSet.find(objectPointIds[n]) != relocatedObjectPointIdSet.end())
					{
						relocatedIndices.push_back(n);
					}
					else
					{
						pendingIndices.push_back(n);
					}
				}

				const ConstArraySubsetAccessor<Vector3, unsigned int> relocatedObjectPoints(objectPoints, relocatedIndices);
				const ConstArraySubsetAccessor<Vector2, unsigned int> relocatedImagePoints(imagePoints, relocatedIndices);

				SquareMatrix3 orientation(true);
				if (!Geometry::RANSAC::orientation(anyCamera, relocatedObjectPoints, relocatedImagePoints, randomGenerator, orientation, 3u, 50u, Scalar(10 * 10)))
				{
					ocean_assert(false && "This should never happen - however, we take the default rotation");
				}

				SquareMatrix3 optimizedOrientation(false);
				if (Geometry::NonLinearOptimizationOrientation::optimizeOrientation(anyCamera, orientation, relocatedObjectPoints, relocatedImagePoints, optimizedOrientation, 10u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5)))
				{
					ocean_assert(!optimizedOrientation.isSingular());

					database.setPose<false>(frameIndex, HomogenousMatrix4(optimizedOrientation));

					// now we have to determine the position of all pending object points

					if (!pendingIndices.empty())
					{
						imagePoints = Subset::subset(imagePoints, pendingIndices);
						objectPointIds = Subset::subset(objectPointIds, pendingIndices);

						objectPoints = Geometry::Utilities::createObjectPoints(anyCamera, HomogenousMatrix4(optimizedOrientation), ConstArrayAccessor<Vector2>(imagePoints), Scalar(1));

						database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPointIds.size());

						relocatedObjectPointIdSet.insert(pendingIndices.begin(), pendingIndices.end());
					}
				}
				else
				{
					database.setPose<false>(frameIndex, HomogenousMatrix4(false));
				}

				processedPose = true;
			}
		}

		if (!processedPose)
		{
			break;
		}
	}

	if (relocatedObjectPointIds != nullptr)
	{
		*relocatedObjectPointIds = std::move(relocatedObjectPointIdSet);
	}

	return true;
}

void Solver3::determineUnknownObjectPointsSubset(const AnyCamera* camera, const Database* database, const Database::PoseImagePointTopologyGroups* objectPointsData, RandomGenerator* randomGenerator, const Scalar maximalSqrError, bool* abort, Lock* lock, Vectors3* newObjectPoints, Indices32* newObjectPointIds, unsigned int firstObjectPoint, unsigned int numberObjectPoints)
{
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(database != nullptr && objectPointsData != nullptr);
	ocean_assert(newObjectPoints != nullptr && newObjectPointIds != nullptr);
	ocean_assert(randomGenerator != nullptr);

	ocean_assert(firstObjectPoint + numberObjectPoints <= objectPointsData->size());

	RandomGenerator localGenerator(*randomGenerator);

	Vectors3 localNewObjectPoints;
	Indices32 localNewObjectPointIds;

	localNewObjectPoints.reserve(numberObjectPoints);
	localNewObjectPointIds.reserve(numberObjectPoints);

	for (unsigned int n = firstObjectPoint; (!abort || !*abort) && n < firstObjectPoint + numberObjectPoints; ++n)
	{
		const Index32 objectPointId = (*objectPointsData)[n].first;
		const Database::PoseImagePointTopology& data = (*objectPointsData)[n].second;

		Vector3 objectPoint;
		if (Geometry::RANSAC::objectPoint(*camera, Database::ConstPoseAccessorTopology<false>(*database, data), Database::ConstImagePointAccessorTopology<false>(*database, data), localGenerator, objectPoint, 20u, maximalSqrError, max(2, int(data.size()) - 1)))
		{
			localNewObjectPoints.push_back(objectPoint);
			localNewObjectPointIds.push_back(objectPointId);
		}
	}

	// if this function is invoked by a worker we need to add the results within a critical section, otherwise we can simply move the results

	if (lock != nullptr)
	{
		const ScopedLock scopedLock(*lock);

		newObjectPoints->insert(newObjectPoints->end(), localNewObjectPoints.begin(), localNewObjectPoints.end());
		newObjectPointIds->insert(newObjectPointIds->end(), localNewObjectPointIds.begin(), localNewObjectPointIds.end());
	}
	else
	{
		*newObjectPoints = std::move(localNewObjectPoints);
		*newObjectPointIds = std::move(localNewObjectPointIds);
	}
}

Scalar Solver3::averagePointDistance(const Vector2* points, const size_t size)
{
	ocean_assert(points && size != 0);

	Vector2 centerOfGravity(0, 0);
	for (size_t n = 0; n < size; ++n)
	{
		centerOfGravity += points[n];
	}

	centerOfGravity /= Scalar(size);

	Scalar distance = 0;
	for (size_t n = 0; n < size; ++n)
	{
		distance += centerOfGravity.distance(points[n]);
	}

	return distance / Scalar(size);
}

}

}
