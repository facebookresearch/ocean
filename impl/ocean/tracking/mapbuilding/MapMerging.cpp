/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/MapMerging.h"
#include "ocean/tracking/mapbuilding/PoseEstimation.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/geometry/AbsoluteTransformation.h"
#include "ocean/geometry/Octree.h"

#include "ocean/tracking/Solver3.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

bool MapMerging::bundleAdjustment(Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int iterations)
{
	Vectors3 optimizedObjectPoints;
	Indices32 optimizedObjectPointIds;

	HomogenousMatrices4 optimizedPoses;
	Indices32 optimizedPoseIds;

	constexpr unsigned int minimalNumberKeyFrames = 10u;
	const unsigned int maximalNumberKeyFrames = (unsigned int)(database.poseNumber<false>()) / 5u; // we use every 5th frame

	constexpr unsigned int minimalObservations = 10u;

	Scalar initialRobustError;
	Scalar finalRobustError;
	if (!Tracking::Solver3::optimizeObjectPointsWithVariablePoses(database, pinholeCamera, optimizedObjectPoints, optimizedObjectPointIds, &optimizedPoses, &optimizedPoseIds, minimalNumberKeyFrames, maximalNumberKeyFrames, minimalObservations, Geometry::Estimator::ET_HUBER, iterations, &initialRobustError, &finalRobustError))
	{
		Log::error() << "Bundle adjustment failed!";
		return false;
	}

	Log::info() << "Finished Bundle Adjustment with " << optimizedPoses.size() << " key frames, and " << optimizedObjectPoints.size() << " feature points, with error " << initialRobustError << " -> " << finalRobustError;

	database.setObjectPoints<false>(Database::invalidObjectPoint()); // removing the locations of all object points
	database.setObjectPoints<false>(optimizedObjectPointIds.data(), optimizedObjectPoints.data(), optimizedObjectPointIds.size()); // setting the locations only of all optimized object points

	database.setPoses<false>(optimizedPoseIds.data(), optimizedPoses.data(), optimizedPoseIds.size()); // updating all optimized camera poses

	const UnorderedIndexSet32 optimizedPoseIdSet(optimizedPoseIds.cbegin(), optimizedPoseIds.cend());

	for (const Index32& poseId : database.poseIds<false>())
	{
		if (optimizedPoseIdSet.find(poseId) == optimizedPoseIdSet.cend())
		{
			const HomogenousMatrix4 world_T_camera = Tracking::Solver3::determinePose(database, AnyCameraPinhole(pinholeCamera), randomGenerator, poseId, database.pose<false>(poseId), 10u, Geometry::Estimator::ET_HUBER);

			database.setPose<false>(poseId, world_T_camera);
		}
	}

	return true;
}

size_t MapMerging::closeLoops(Database& database, FreakMultiDescriptorMap256& freakMap, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int minimalNumberValidCorrespondences, const unsigned int maximalNumberOverlappingObjectPointInPosePair, const unsigned int maximalDescriptorDistance, const unsigned int iterationsWithoutImprovements)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(minimalNumberValidCorrespondences >= 1u);
	ocean_assert(iterationsWithoutImprovements >= 1u);

	UnorderedIndexSet32 objectPoseObjectPointIdSet;
	UnorderedIndexSet32 imagePoseObjectPointIdSet;

	Vectors3 objectPoints;
	Indices32 objectPointsObjectPointIds;
	std::vector<FreakMultiDescriptors256> objectPointFeatures;

	Vectors2 imagePoints;
	Indices32 imagePointsObjectPointIds;
	std::vector<FreakMultiDescriptors256> imagePointFeatures;

	Indices32 matchedObjectPointsObjectPointIds;
	Indices32 matchedImagePointsObjectPointIds;
	Vectors3 matchedObjectPoints;
	Vectors2 matchedImagePoints;

	Indices32 validIndices;

	std::set<IndexPair32> correspondingObjectPointIdPairSet;

	const unsigned int ransacIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), Scalar(0.85));

	unsigned int lowerPoseIndex;
	unsigned int upperPoseIndex;
	database.poseBorders<false>(lowerPoseIndex, upperPoseIndex);

	const unsigned int poseRange = upperPoseIndex - lowerPoseIndex + 1u;

	for (unsigned int iteration = 0u; iteration < iterationsWithoutImprovements; ++iteration)
	{
		// we select two random poses for which we check whether we can close a loop between both poses

		unsigned int objectPoseIndex;
		unsigned int imagePoseIndex;
		RandomI::random(randomGenerator, poseRange - 1u, objectPoseIndex, imagePoseIndex);
		objectPoseIndex += lowerPoseIndex;
		imagePoseIndex += lowerPoseIndex;

		const Indices32 objectPoseObjectPointIds = database.objectPointIds<false, false>(objectPoseIndex, Tracking::Database::invalidObjectPoint());
		const Indices32 imagePoseObjectPointIds = database.objectPointIds<false, false>(imagePoseIndex, Tracking::Database::invalidObjectPoint());

		objectPoseObjectPointIdSet.clear(); // re-using the data structure
		objectPoseObjectPointIdSet.insert(objectPoseObjectPointIds.cbegin(), objectPoseObjectPointIds.cend());

		// we ensure that we have only a minor number of overlapping object points in both poses

		unsigned int numberOverlappingObjectPoints = 0u;
		for (const Index32& imagePoseObjectPointId : imagePoseObjectPointIds)
		{
			if (objectPoseObjectPointIdSet.find(imagePoseObjectPointId) != objectPoseObjectPointIdSet.cend())
			{
				++numberOverlappingObjectPoints;
			}
		}

		if (numberOverlappingObjectPoints > maximalNumberOverlappingObjectPointInPosePair)
		{
			continue;
		}

		// both poses do not have too many overlapping object points

		imagePoseObjectPointIdSet.clear(); // re-using the data structure
		imagePoseObjectPointIdSet.insert(imagePoseObjectPointIds.cbegin(), imagePoseObjectPointIds.cend());

		// we extract all object points which are not overlapping

		objectPoints.clear(); // re-using the data structure
		objectPointsObjectPointIds.clear();
		objectPointFeatures.clear();

		for (const Index32& objectPoseObjectPointId : objectPoseObjectPointIds)
		{
			if (imagePoseObjectPointIdSet.find(objectPoseObjectPointId) == imagePoseObjectPointIdSet.cend())
			{
				const FreakMultiDescriptorMap256::const_iterator iFreak = freakMap.find(objectPoseObjectPointId);
				ocean_assert(iFreak != freakMap.cend());

				const FreakMultiDescriptors256& freakFeatures = iFreak->second;

				objectPointFeatures.emplace_back(freakFeatures);
				objectPoints.emplace_back(database.objectPoint<false>(objectPoseObjectPointId));
				objectPointsObjectPointIds.emplace_back(objectPoseObjectPointId);
			}
		}

		imagePoints.clear(); // re-using the data structure
		imagePointsObjectPointIds.clear();
		imagePointFeatures.clear();

		for (const Index32& imagePoseObjectPointId : imagePoseObjectPointIds)
		{
			if (objectPoseObjectPointIdSet.find(imagePoseObjectPointId) == objectPoseObjectPointIdSet.cend())
			{
				const FreakMultiDescriptorMap256::const_iterator iFreak = freakMap.find(imagePoseObjectPointId);
				ocean_assert(iFreak != freakMap.cend());

				const FreakMultiDescriptors256& freakFeatures = iFreak->second;

				imagePointFeatures.emplace_back(freakFeatures);

				Vector2 imagePoint;
				if (database.hasObservation<false>(imagePoseIndex, imagePoseObjectPointId, &imagePoint))
				{
					imagePoints.emplace_back(imagePoint);
					imagePointsObjectPointIds.emplace_back(imagePoseObjectPointId);
				}
				else
				{
					return 1;
					assert(false && "This must never happen!");
				}
			}
		}

		// now, we determine 2D/3D correspondences between both pose pairs

		matchedObjectPointsObjectPointIds.clear(); // re-using the data structure
		matchedImagePointsObjectPointIds.clear();
		matchedObjectPoints.clear();
		matchedImagePoints.clear();

		for (Index32 nImage = 0u; nImage < imagePoints.size(); ++nImage)
		{
			const Vector2& imagePoint = imagePoints[nImage];
			const FreakMultiDescriptors256& imagePointFeature = imagePointFeatures[nImage];

			Index32 bestIndex = Index32(-1);
			unsigned int bestDistance = (unsigned int)(-1);

			for (Index32 nObject = 0u; nObject < objectPoints.size(); ++nObject)
			{
				const FreakMultiDescriptors256& objectPointFeature = objectPointFeatures[nObject];

				unsigned int localBestDistance = (unsigned int)(-1);

				for (const auto& iF : imagePointFeature)
				{
					const unsigned int distance = determineFreakDistance(iF, objectPointFeature);

					if (distance < localBestDistance)
					{
						localBestDistance = distance;
					}
				}

				if (localBestDistance < bestDistance)
				{
					bestDistance = localBestDistance;
					bestIndex = nObject;
				}
			}

			if (bestDistance <= maximalDescriptorDistance)
			{
				const Vector3& objectPoint = objectPoints[bestIndex];

				matchedImagePoints.emplace_back(imagePoint);
				matchedObjectPoints.emplace_back(objectPoint);

				matchedObjectPointsObjectPointIds.emplace_back(objectPointsObjectPointIds[bestIndex]);
				matchedImagePointsObjectPointIds.emplace_back(imagePointsObjectPointIds[nImage]);
			}
		}

		if (matchedObjectPoints.size() <= minimalNumberValidCorrespondences)
		{
			continue;
		}

		const size_t previousNumberCorrespondingObjectPointIdPairs = correspondingObjectPointIdPairSet.size();

		HomogenousMatrix4 world_T_camera;
		validIndices.clear();
		if (Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(matchedObjectPoints), ConstArrayAccessor<Vector2>(matchedImagePoints), randomGenerator, world_T_camera, minimalNumberValidCorrespondences, true, ransacIterations, Scalar(3 * 3), &validIndices))
		{
			ocean_assert(validIndices.size() >= minimalNumberValidCorrespondences);

			for (const Index32& validIndex : validIndices)
			{
				Index32 objectPointObjectPointId = matchedObjectPointsObjectPointIds[validIndex];
				Index32 imagePointImagePointId = matchedImagePointsObjectPointIds[validIndex];

				// sorting the ids to ensure that we do not store the same pair twice
				Ocean::Utilities::sortLowestToFront2(objectPointObjectPointId, imagePointImagePointId);

				correspondingObjectPointIdPairSet.emplace(objectPointObjectPointId, imagePointImagePointId);
			}
		}

		if (previousNumberCorrespondingObjectPointIdPairs < correspondingObjectPointIdPairSet.size())
		{
			Log::info() << "Corresponding points: " << correspondingObjectPointIdPairSet.size() << " (" << iteration << ")";

			// we were able to find new correspondences in this iteration, so with the full amount of additional iterations
			iteration = 0u;
		}
	}

	if (correspondingObjectPointIdPairSet.empty())
	{
		return 0;
	}

	// now we have to consolidate all point pairs into sets of points all corresponding to each other

	std::vector<IndexSet32> correspondingFeaturePointIdGroups;

	for (const IndexPair32& correspondingObjectPointIdPair : correspondingObjectPointIdPairSet)
	{
		IndexSet32 correspondencesSet;
		correspondencesSet.emplace(correspondingObjectPointIdPair.first);
		correspondencesSet.emplace(correspondingObjectPointIdPair.second);

		correspondingFeaturePointIdGroups.emplace_back(std::move(correspondencesSet));
	}

	bool groupHasBeenModified = true;
	while (groupHasBeenModified)
	{
		groupHasBeenModified = false;

		for (size_t nOuter = 0; !groupHasBeenModified && nOuter + 1 < correspondingFeaturePointIdGroups.size(); ++nOuter)
		{
			for (size_t nInner = nOuter + 1; !groupHasBeenModified && nInner < correspondingFeaturePointIdGroups.size(); ++nInner)
			{
				if (Subset::hasIntersectingElement(correspondingFeaturePointIdGroups[nOuter], correspondingFeaturePointIdGroups[nInner]))
				{
					correspondingFeaturePointIdGroups[nOuter].insert(correspondingFeaturePointIdGroups[nInner].begin(), correspondingFeaturePointIdGroups[nInner].end());

					correspondingFeaturePointIdGroups[nInner] = std::move(correspondingFeaturePointIdGroups.back());
					correspondingFeaturePointIdGroups.pop_back();

					groupHasBeenModified = true;
				}
			}
		}
	}

	ocean_assert(correspondingFeaturePointIdGroups.size() >= 1);

	for (const IndexSet32& correspondingFeaturePointIdGroup : correspondingFeaturePointIdGroups)
	{
		const IndexSet32::const_iterator iBegin = correspondingFeaturePointIdGroup.begin();
		IndexSet32::const_iterator iNext = iBegin;
		++iNext;

		const Index32 firstObjectPointId = *iBegin;

		ocean_assert(freakMap.find(firstObjectPointId) != freakMap.cend());
		FreakMultiDescriptors256& firstObjectPointFreakFeatures = freakMap.find(firstObjectPointId)->second;

		while (iNext != correspondingFeaturePointIdGroup.end())
		{
			const Index32 nextObjectPointId = *iNext;

			ocean_assert(database.hasObjectPoint<false>(firstObjectPointId) && database.hasObjectPoint<false>(nextObjectPointId));

			if (database.hasObjectPoint<false>(firstObjectPointId) && database.hasObjectPoint<false>(nextObjectPointId))
			{
				Scalar firstObjectPointPriority;
				Scalar nextObjectPointPriority;
				const Vector3 newObjectPointLocation = (database.objectPoint<false>(firstObjectPointId, firstObjectPointPriority) + database.objectPoint<false>(nextObjectPointId, nextObjectPointPriority)) * Scalar(0.5);

				const Scalar newObjectPointPriority = (firstObjectPointPriority + nextObjectPointPriority) * Scalar(0.5);

				database.mergeObjectPoints<false>(firstObjectPointId, nextObjectPointId, newObjectPointLocation, newObjectPointPriority);

				FreakMultiDescriptorMap256::iterator iFreakNext = freakMap.find(nextObjectPointId);
				ocean_assert(iFreakNext != freakMap.cend());

				const FreakMultiDescriptors256& nextObjectPointFreakFeatures = iFreakNext->second;

				firstObjectPointFreakFeatures.insert(firstObjectPointFreakFeatures.cend(), nextObjectPointFreakFeatures.cbegin(), nextObjectPointFreakFeatures.cend()); // **TODO only different freak features

				freakMap.erase(iFreakNext);
			}

			++iNext;
		}
	}

	Solver3::removeObjectPointsNotInFrontOfCamera(database);

	const bool bundleResult = bundleAdjustment(database, pinholeCamera, randomGenerator, 10u);
	ocean_assert_and_suppress_unused(bundleResult, bundleResult);

	return correspondingFeaturePointIdGroups.size();
}

size_t MapMerging::mergeObjectPoints(Database& database, FreakMultiDescriptorMap256& freakMap, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const Scalar maximalProjectionError, const unsigned int maximalDescriptorDistance, const unsigned int iterationsWithoutImprovements)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(iterationsWithoutImprovements >= 1u);

	std::set<IndexPair32> correspondingObjectPointIdPairSet;

	IndexSet32 objectPointPoseIds;
	Vectors2 imagePoints;
	std::vector<const Indices32*> leafs;

	Indices32 observationPoseIds;
	Indices32 observationImagePointIds;
	Vectors2 observationImagePoints;

	Geometry::Octree::ReusableData reusableData;

	Indices32 objectPointIds;
	const Vectors3 objectPoints = database.objectPoints<false, false>(Database::invalidObjectPoint(), &objectPointIds);
	const Geometry::Octree octree(objectPoints.data(), objectPoints.size());

	unsigned int lowerPoseIndex;
	unsigned int upperPoseIndex;
	database.poseBorders<false>(lowerPoseIndex, upperPoseIndex);

	for (unsigned int iteration = 0u; iteration < iterationsWithoutImprovements; ++iteration)
	{
		// we try to find corresponding object point pairs in a random pose

		const unsigned int poseIndex = RandomI::random(randomGenerator, lowerPoseIndex, upperPoseIndex);

		const HomogenousMatrix4& world_T_camera = database.pose<false>(poseIndex);

		if (!world_T_camera.isValid())
		{
			continue;
		}

		const size_t previousNumberCorrespondingObjectPointIdPairs = correspondingObjectPointIdPairSet.size();

		// each image point in a camera image (which has an associated 3D object point) may fit to another 3D object point visible in the same camera image

		const IndexSet32& imagePointIds = database.imagePointsFromPose<false>(poseIndex);

		for (const Index32& imagePointId : imagePointIds)
		{
			objectPointPoseIds.clear();

			const Index32 objectPointId = database.objectPointFromImagePoint<false>(imagePointId);

			if (objectPointId == Database::invalidId)
			{
				// the image point is not associated with a 3D object point
				continue;
			}

			if (database.objectPoint<false>(objectPointId) == Database::invalidObjectPoint())
			{
				// the 3D object point location is not known so it cannot be part of a pair of corresponding object points
				continue;
			}

			ocean_assert(freakMap.find(objectPointId) != freakMap.cend());
			const FreakMultiDescriptors256& objectPointFreakFeatures = freakMap.find(objectPointId)->second;

			const Vector2& imagePoint = database.imagePoint<false>(imagePointId);

			// we consider all 3D object points projecting close to the image point as candidate

			const Line3 ray = pinholeCamera.ray(imagePoint, world_T_camera);

			leafs.clear();
			octree.intersectingLeafs(ray, leafs, reusableData);

			Index32 bestCandidateObjectPointId = Database::invalidId;
			unsigned int bestDistance = (unsigned int)(-1);

			for (const Indices32* leaf : leafs)
			{
				for (const Index32& candidateObjectPointIndex : *leaf)
				{
					ocean_assert(candidateObjectPointIndex < objectPointIds.size());

					const Index32& candidateObjectPointId = objectPointIds[candidateObjectPointIndex];

					if (candidateObjectPointId == objectPointId)
					{
						continue;
					}

					// let's ensure that both objects points are every visible in the same camera image

					if (objectPointPoseIds.empty())
					{
						objectPointPoseIds = database.posesFromObjectPoint<false>(objectPointId);
						ocean_assert(!objectPointPoseIds.empty());
					}

					if (database.hasObservation<false>(poseIndex, candidateObjectPointId))
					{
						// fast check: the object point is already visible in the camera image, so it cannot be a candidate
						continue;
					}

					// we first check whether the candidate object point projects close to the current image point

					const Vector3& candidateObjectPoint = objectPoints[candidateObjectPointIndex];

					if (pinholeCamera.projectToImage<true>(world_T_camera, candidateObjectPoint, true).sqrDistance(imagePoint) > Numeric::sqr(maximalProjectionError))
					{
						// fast check: projection is not close enough
						continue;
					}

					ocean_assert(freakMap.find(candidateObjectPointId) != freakMap.cend());
					const FreakMultiDescriptors256& candidateObjectPointFreakFeatures = freakMap.find(candidateObjectPointId)->second;

					const unsigned int distance = determineFreakDistance(objectPointFreakFeatures, candidateObjectPointFreakFeatures);

					if (distance < bestDistance)
					{
						bestDistance = distance;
						bestCandidateObjectPointId = candidateObjectPointId;
					}
				}
			}

			if (bestDistance <= maximalDescriptorDistance)
			{
				ocean_assert(bestCandidateObjectPointId != Database::invalidId);

				const IndexSet32 candidateObjectPointPoseIds = database.posesFromObjectPoint<false>(bestCandidateObjectPointId);

				if (Subset::hasIntersectingElement(objectPointPoseIds, candidateObjectPointPoseIds))
				{
					// we have at least one camera image in which both object points are visible at the same time
					continue;
				}

				// now we check whether the candidate object point mostly projects close to our image point in all other poses

				const Vector3& candidateObjectPoint = database.objectPoint<false>(bestCandidateObjectPointId);
				ocean_assert(candidateObjectPoint != Database::invalidObjectPoint());

				unsigned int preciseProjections = 0u;
				unsigned int validProjections = 0u;

				// first we test the projection error for the candidate object point

				observationPoseIds.clear();
				observationImagePointIds.clear();
				observationImagePoints.clear();
				database.observationsFromObjectPoint<false>(objectPointId, observationPoseIds, observationImagePointIds, &observationImagePoints);

				for (size_t n = 0; n < observationPoseIds.size(); ++n)
				{
					ocean_assert(database.hasPose<false>(observationPoseIds[n]));

					const HomogenousMatrix4& world_T_observationCamera = database.pose<false>(observationPoseIds[n]);

					if (world_T_observationCamera.isValid())
					{
						const Vector2& observationImagePoint = observationImagePoints[n];
						const Vector2 projectedCandidateObjectPoint = pinholeCamera.projectToImage<true>(world_T_camera, candidateObjectPoint, true);

						const Scalar sqrDistance = projectedCandidateObjectPoint.sqrDistance(observationImagePoint);

						if (sqrDistance <= Numeric::sqr(maximalProjectionError))
						{
							++preciseProjections;
						}

						++validProjections;
					}
				}

				// now we test the projection error for the original object points

				const Vector3& objectPoint = database.objectPoint<false>(objectPointId);

				observationPoseIds.clear();
				observationImagePointIds.clear();
				observationImagePoints.clear();
				database.observationsFromObjectPoint<false>(bestCandidateObjectPointId, observationPoseIds, observationImagePointIds, &observationImagePoints);

				for (size_t n = 0; n < observationPoseIds.size(); ++n)
				{
					ocean_assert(database.hasPose<false>(observationPoseIds[n]));

					const HomogenousMatrix4& world_T_observationCamera = database.pose<false>(observationPoseIds[n]);

					if (world_T_observationCamera.isValid())
					{
						const Vector2& observationImagePoint = observationImagePoints[n];
						const Vector2 projectedCandidateObjectPoint = pinholeCamera.projectToImage<true>(world_T_camera, objectPoint, true);

						const Scalar sqrDistance = projectedCandidateObjectPoint.sqrDistance(observationImagePoint);

						if (sqrDistance <= Numeric::sqr(maximalProjectionError))
						{
							++preciseProjections;
						}

						++validProjections;
					}
				}

				if (validProjections == 0u)
				{
					continue;
				}

				if (preciseProjections >= 1u && preciseProjections >= validProjections * 10u / 100u) // we expect at least 10%
				{
					ocean_assert(objectPointId != bestCandidateObjectPointId);
					if (objectPointId < bestCandidateObjectPointId)
					{
						correspondingObjectPointIdPairSet.emplace(objectPointId, bestCandidateObjectPointId);
					}
					else
					{
						correspondingObjectPointIdPairSet.emplace(bestCandidateObjectPointId, objectPointId);
					}
				}
			}
		}

		if (previousNumberCorrespondingObjectPointIdPairs < correspondingObjectPointIdPairSet.size())
		{
			Log::info() << "Corresponding points: " << correspondingObjectPointIdPairSet.size() << " (" << iteration << ")";

			// we were able to find new correspondences in this iteration, so with the full amount of additional iterations
			iteration = 0u;
		}
	}

	if (correspondingObjectPointIdPairSet.empty())
	{
		return 0;
	}

	// now we have to consolidate all point pairs into sets of points all corresponding to each other

	std::vector<IndexSet32> correspondingFeaturePointIdGroups;

	for (const IndexPair32& correspondingObjectPointIdPair : correspondingObjectPointIdPairSet)
	{
		IndexSet32 correspondencesSet;
		correspondencesSet.emplace(correspondingObjectPointIdPair.first);
		correspondencesSet.emplace(correspondingObjectPointIdPair.second);

		correspondingFeaturePointIdGroups.emplace_back(std::move(correspondencesSet));
	}

	bool groupHasBeenModified = true;
	while (groupHasBeenModified)
	{
		groupHasBeenModified = false;

		for (size_t nOuter = 0; !groupHasBeenModified && nOuter + 1 < correspondingFeaturePointIdGroups.size(); ++nOuter)
		{
			for (size_t nInner = nOuter + 1; !groupHasBeenModified && nInner < correspondingFeaturePointIdGroups.size(); ++nInner)
			{
				if (Subset::hasIntersectingElement(correspondingFeaturePointIdGroups[nOuter], correspondingFeaturePointIdGroups[nInner]))
				{
					correspondingFeaturePointIdGroups[nOuter].insert(correspondingFeaturePointIdGroups[nInner].begin(), correspondingFeaturePointIdGroups[nInner].end());

					correspondingFeaturePointIdGroups[nInner] = std::move(correspondingFeaturePointIdGroups.back());
					correspondingFeaturePointIdGroups.pop_back();

					groupHasBeenModified = true;
				}
			}
		}
	}

	ocean_assert(correspondingFeaturePointIdGroups.size() >= 1);

	for (const IndexSet32& correspondingFeaturePointIdGroup : correspondingFeaturePointIdGroups)
	{
		const IndexSet32::const_iterator iBegin = correspondingFeaturePointIdGroup.begin();
		IndexSet32::const_iterator iNext = iBegin;
		++iNext;

		const Index32 firstObjectPointId = *iBegin;

		ocean_assert(freakMap.find(firstObjectPointId) != freakMap.cend());
		FreakMultiDescriptors256& firstObjectPointFreakFeatures = freakMap.find(firstObjectPointId)->second;

		while (iNext != correspondingFeaturePointIdGroup.end())
		{
			const Index32 nextObjectPointId = *iNext;

			ocean_assert(database.hasObjectPoint<false>(firstObjectPointId) && database.hasObjectPoint<false>(nextObjectPointId));

			if (database.hasObjectPoint<false>(firstObjectPointId) && database.hasObjectPoint<false>(nextObjectPointId))
			{
				Scalar firstObjectPointPriority;
				Scalar nextObjectPointPriority;
				const Vector3 newObjectPointLocation = (database.objectPoint<false>(firstObjectPointId, firstObjectPointPriority) + database.objectPoint<false>(nextObjectPointId, nextObjectPointPriority)) * Scalar(0.5);

				const Scalar newObjectPointPriority = (firstObjectPointPriority + nextObjectPointPriority) * Scalar(0.5);

				database.mergeObjectPoints<false>(firstObjectPointId, nextObjectPointId, newObjectPointLocation, newObjectPointPriority);

				FreakMultiDescriptorMap256::iterator iFreakNext = freakMap.find(nextObjectPointId);
				ocean_assert(iFreakNext != freakMap.cend());

				const FreakMultiDescriptors256& nextObjectPointFreakFeatures = iFreakNext->second;

				firstObjectPointFreakFeatures.insert(firstObjectPointFreakFeatures.cend(), nextObjectPointFreakFeatures.cbegin(), nextObjectPointFreakFeatures.cend()); // **TODO only different freak features

				freakMap.erase(iFreakNext);
			}

			++iNext;
		}
	}

	Solver3::removeObjectPointsNotInFrontOfCamera(database);

	const bool bundleResult = bundleAdjustment(database, pinholeCamera, randomGenerator, 10u);
	ocean_assert_and_suppress_unused(bundleResult, bundleResult);

	return correspondingFeaturePointIdGroups.size();
}

bool MapMerging::mergeMaps(const PinholeCamera& sourceCamera, const Database& sourceDatabase, const UnifiedDescriptorMap& sourceDescriptorMap, const PinholeCamera& targetCamera, Database& targetDatabase, UnifiedDescriptorMap& targetDescriptorMap, RandomGenerator& randomGenerator, const unsigned int minimalNumberCorrespondingFeaturesPerPose, const unsigned int minimalNumberCorrespondingPoses, const unsigned int iterationsWithoutImprovements, const unsigned int maximalNumberImprovements)
{
	ocean_assert(minimalNumberCorrespondingFeaturesPerPose >= 4u);
	ocean_assert(minimalNumberCorrespondingPoses >= 1u);
	ocean_assert(iterationsWithoutImprovements >= 1u);

	if (sourceDescriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256
			|| targetDescriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		return false;
	}

	Vectors3 sourceObjectPoints;
	Indices32 sourceObjectPointIds;
	UnifiedDescriptor::BinaryDescriptors<256u> sourceObjectPointDescriptors;
	Indices32 sourceObjectPointDescriptorIndices;
	UnifiedHelperFreakMultiDescriptor256::BinaryVocabularyForest sourceObjectPointDescriptorsForest;
	Geometry::Octree sourceObjectPointOctree;
	std::unique_ptr<Tracking::MapBuilding::UnifiedUnguidedMatchingFreakMultiFeatures256Group> sourceUnifiedUnguidedMatching;
	std::unique_ptr<Tracking::MapBuilding::UnifiedGuidedMatchingFreakMultiDescriptor256Group> sourceUnifiedGuidedMatching;
	if (!UnifiedHelperFreakMultiDescriptor256::initialize(sourceDatabase, sourceDescriptorMap, randomGenerator, sourceObjectPoints, sourceObjectPointIds, sourceObjectPointDescriptors, sourceObjectPointDescriptorIndices, sourceObjectPointDescriptorsForest, sourceObjectPointOctree, sourceUnifiedUnguidedMatching, sourceUnifiedGuidedMatching))
	{
		return false;
	}

	Vectors3 targetObjectPoints;
	Indices32 targetObjectPointIds;
	UnifiedDescriptor::BinaryDescriptors<256u> targetObjectPointDescriptors;
	Indices32 targetObjectPointDescriptorIndices;
	UnifiedHelperFreakMultiDescriptor256::BinaryVocabularyForest targetObjectPointDescriptorsForest;
	Geometry::Octree targetObjectPointOctree;
	std::unique_ptr<Tracking::MapBuilding::UnifiedUnguidedMatchingFreakMultiFeatures256Group> targetUnifiedUnguidedMatching;
	std::unique_ptr<Tracking::MapBuilding::UnifiedGuidedMatchingFreakMultiDescriptor256Group> targetUnifiedGuidedMatching;
	if (!UnifiedHelperFreakMultiDescriptor256::initialize(targetDatabase, targetDescriptorMap, randomGenerator, targetObjectPoints, targetObjectPointIds, targetObjectPointDescriptors, targetObjectPointDescriptorIndices, targetObjectPointDescriptorsForest, targetObjectPointOctree, targetUnifiedUnguidedMatching, targetUnifiedGuidedMatching))
	{
		return false;
	}

#ifdef OCEAN_KEEP_DEBUG_CODE
	Indices32 targetObjectPointIds;
	Vectors3 targetObjectPoints;
	BinaryDescriptors256 targetObjectPointDescriptors;
	Indices32 targetObjectPointDescriptorIndices;

	targetObjectPointIds = targetDatabase.objectPointIds<false, false>(Tracking::Database::invalidObjectPoint(), &targetObjectPoints);

	targetObjectPointDescriptors.reserve(targetObjectPointIds.size() * 40);
	targetObjectPointDescriptorIndices.reserve(targetObjectPointDescriptors.capacity());

	for (size_t n = 0; n < targetObjectPointIds.size(); /*noop*/)
	{
		const Index32& objectPointId = targetObjectPointIds[n];

		const FreakMultiDescriptorMap256::const_iterator i = targetFreakMap.find(objectPointId);
		if (i != targetFreakMap.cend())
		{
			const FreakMultiDescriptors256& descriptors = i->second;

			for (const FreakMultiDescriptor256& multiDescriptor : descriptors)
			{
				for (unsigned int t = 0u; t < multiDescriptor.descriptorLevels(); ++t)
				{
					const auto& layerDescriptor = multiDescriptor.data()[t];
					static_assert(sizeof(layerDescriptor) == 32, "Invalid size!");

					BinaryDescriptor256 targetObjectPointDescriptor;
					memcpy(&targetObjectPointDescriptor, &layerDescriptor, sizeof(layerDescriptor));

					targetObjectPointDescriptors.emplace_back(targetObjectPointDescriptor);

					targetObjectPointDescriptorIndices.emplace_back(Index32(n));
				}
			}

			++n;
		}
		else
		{
			targetObjectPointIds[n] = targetObjectPointIds.back();
			targetObjectPointIds.pop_back();

			targetObjectPoints[n] = targetObjectPoints.back();
			targetObjectPoints.pop_back();
		}
	}

	typedef Tracking::VocabularyForest<BinaryDescriptor256, unsigned int, Tracking::MapBuilding::DescriptorHandling::calculateHammingDistance> BinaryVocabularyForest;
	typedef BinaryVocabularyForest::TVocabularyTree BinaryVocabularyTree;
	const BinaryVocabularyTree::ClustersMeanFunction clustersMeanFunction = &BinaryVocabularyTree::determineClustersMeanForBinaryDescriptor<sizeof(BinaryDescriptor256) * 8>;

	const BinaryVocabularyForest targetobjectPointDescriptorsForest(2, targetObjectPointDescriptors.data(), targetObjectPointDescriptors.size(), clustersMeanFunction, BinaryVocabularyForest::Parameters(), WorkerPool::get().scopedWorker()(), &randomGenerator);
	const Geometry::Octree octree(targetObjectPoints.data(), targetObjectPoints.size(), Geometry::Octree::Parameters(40u, true));
#endif // OCEAN_KEEP_DEBUG_CODE

	std::set<IndexPair32> correspondingSourceTargetObjectPointIdSet;

	Vectors2 imagePoints;
	std::vector<const FreakMultiDescriptors256*> imagePointDescriptorGroups;

	imagePoints.reserve(2000);
	imagePointDescriptorGroups.reserve(2000);

	Indices32 usedObjectPointIndices;
	Indices32 usedObjectPointIds;
	Indices32 usedImagePointIndices;

	unsigned int sourceLowerPoseIndex;
	unsigned int sourceUpperPoseIndex;
	sourceDatabase.poseBorders<false>(sourceLowerPoseIndex, sourceUpperPoseIndex);

	unsigned int targetLowerPoseIndex;
	unsigned int targetUpperPoseIndex;
	targetDatabase.poseBorders<false>(targetLowerPoseIndex, targetUpperPoseIndex);

	HomogenousMatrices4 world_T_sourceCameras;
	HomogenousMatrices4 world_T_targetCameras;
	world_T_sourceCameras.reserve(200);
	world_T_targetCameras.reserve(200);

	const FreakMultiDescriptorMap256& sourceFreakMap = ((const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256&)(sourceDescriptorMap)).descriptorMap();
	FreakMultiDescriptorMap256& targetFreakMap = ((UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256&)(targetDescriptorMap)).descriptorMap();

	const UnifiedMatching::DistanceValue maximalDescriptorDistance(64u);

	constexpr Scalar maximalProjectionError = Scalar(3);
	constexpr Scalar inlierRate = Scalar(0.15);

	for (unsigned int iteration = 0u; iteration < iterationsWithoutImprovements; ++iteration)
	{
		if (iteration % 2u == 0u)
		{
			const unsigned int sourcePoseIndex = RandomI::random(randomGenerator, sourceLowerPoseIndex, sourceUpperPoseIndex);

			const HomogenousMatrix4 world_T_sourceCamera = sourceDatabase.pose<false>(sourcePoseIndex);

			if (!world_T_sourceCamera.isValid())
			{
				continue;
			}

			Indices32 visibleSourceObjectPointIds = sourceDatabase.objectPointIds<false, false>(sourcePoseIndex, Tracking::Database::invalidObjectPoint());

			if (visibleSourceObjectPointIds.size() < minimalNumberCorrespondingFeaturesPerPose)
			{
				continue;
			}

			imagePoints.clear();
			imagePointDescriptorGroups.clear();

			for (size_t n = 0; n < visibleSourceObjectPointIds.size(); /*noop*/)
			{
				const Index32& sourceObjectPointId = visibleSourceObjectPointIds[n];

				const FreakMultiDescriptorMap256::const_iterator iSource = sourceFreakMap.find(sourceObjectPointId);
				if (iSource != sourceFreakMap.cend())
				{
					Vector2 imagePoint;
					if (sourceDatabase.hasObservation<false>(sourcePoseIndex, sourceObjectPointId, &imagePoint))
					{
						imagePoints.emplace_back(imagePoint);
						imagePointDescriptorGroups.emplace_back(&iSource->second);

						++n;

						continue;
					}
				}

				visibleSourceObjectPointIds[n] = visibleSourceObjectPointIds.back();
				visibleSourceObjectPointIds.pop_back();
			}

			const size_t previousNumberCorrespondingObjectPointIdPairs = correspondingSourceTargetObjectPointIdSet.size();

			HomogenousMatrix4 world_T_targetCamera(false);
			usedObjectPointIndices.clear();
			usedObjectPointIds.clear();
			usedImagePointIndices.clear();

#ifdef OCEAN_KEEP_DEBUG_CODE
			if (PoseEstimationT::determinePoseBruteForceWithArbitraryDescriptorOrder<const FreakMultiDescriptors256*, BinaryDescriptor256, unsigned int, determineFreakDistance>(sourceCamera, targetObjectPoints.data(), targetObjectPointDescriptors.data(), targetObjectPointDescriptorIndices.data(), targetObjectPointDescriptors.size(), sourceImagePoints.data(), sourceImagePointDescriptorGroups.data(), sourceImagePoints.size(), randomGenerator, world_T_targetCamera, minimalNumberCorrespondences, 64u, Scalar(3), &usedObjectPointIndices, &usedImagePointIndices))

				if (PoseEstimation::determinePose(sourceCamera, unguided, guided, randomGenerator, world_T_targetCamera, minimalNumberCorrespondences, 64u, Scalar(3), &usedObjectPointIndices, &usedImagePointIndices))
				{
					ocean_assert(usedObjectPointIndices.size() == usedImagePointIndices.size());

					for (size_t n = 0; n < usedObjectPointIndices.size(); ++n)
					{
						const Index32& targetObjectPointIndex = usedObjectPointIndices[n];
						const Index32& targetObjectPointId = targetObjectPointIds[targetObjectPointIndex];

						const Index32& sourceImagePointIndex = usedImagePointIndices[n];
						const Index32& sourceObjectPointId = sourceObjectPointIds[sourceImagePointIndex];

						correspondingObjectPointIdPairSet.emplace(targetObjectPointId, sourceObjectPointId);

						a.emplace(targetObjectPointId);
					}

					if (previousNumberCorrespondingObjectPointIdPairs < correspondingObjectPointIdPairSet.size())
					{
						Log::info() << "Valid pose: " << correspondingObjectPointIdPairSet.size() << ", " << ", " << a.size() << ", " << usedObjectPointIndices.size() << " (" << iteration << ")";

						world_T_sourceCameras.emplace_back(world_T_sourceCamera);
						world_T_targetCameras.emplace_back(world_T_targetCamera);

						iteration = 0u;
					}
				}
#endif // OCEAN_KEEP_DEBUG_CODE

			targetUnifiedUnguidedMatching->updateImagePoints(imagePoints.data(), imagePointDescriptorGroups.data(), imagePoints.size());
			targetUnifiedGuidedMatching->updateImagePoints(imagePoints.data(), imagePointDescriptorGroups.data(), imagePoints.size());

			if (PoseEstimation::determinePose(AnyCameraPinhole(sourceCamera), *targetUnifiedUnguidedMatching, *targetUnifiedGuidedMatching, randomGenerator, world_T_targetCamera, minimalNumberCorrespondingFeaturesPerPose, maximalDescriptorDistance, maximalProjectionError, inlierRate,  &usedObjectPointIds, &usedImagePointIndices))
			{
				ocean_assert(usedObjectPointIds.size() == usedImagePointIndices.size());

				for (size_t n = 0; n < usedObjectPointIds.size(); ++n)
				{
					const Index32& targetObjectPointId = usedObjectPointIds[n];

					const Index32& sourceImagePointIndex = usedImagePointIndices[n];
					const Index32& sourceObjectPointId = visibleSourceObjectPointIds[sourceImagePointIndex];

					correspondingSourceTargetObjectPointIdSet.emplace(sourceObjectPointId, targetObjectPointId);
				}

				if (previousNumberCorrespondingObjectPointIdPairs < correspondingSourceTargetObjectPointIdSet.size())
				{
					Log::info() << "Valid pose A: " << correspondingSourceTargetObjectPointIdSet.size() << ", " << usedObjectPointIds.size() << " (" << iteration << ")";

					world_T_sourceCameras.emplace_back(world_T_sourceCamera);
					world_T_targetCameras.emplace_back(world_T_targetCamera);

					iteration = 0u;
				}
			}

			targetUnifiedUnguidedMatching->clearImagePoints();
			targetUnifiedGuidedMatching->clearImagePoints();
		}
		else
		{
			const unsigned int targetPoseIndex = RandomI::random(randomGenerator, targetLowerPoseIndex, targetUpperPoseIndex);

			const HomogenousMatrix4 world_T_targetCamera = targetDatabase.pose<false>(targetPoseIndex);

			if (!world_T_targetCamera.isValid())
			{
				continue;
			}

			Indices32 visibleTargetObjectPointIds = targetDatabase.objectPointIds<false, false>(targetPoseIndex, Tracking::Database::invalidObjectPoint());

			if (visibleTargetObjectPointIds.size() < minimalNumberCorrespondingFeaturesPerPose)
			{
				continue;
			}

			imagePoints.clear();
			imagePointDescriptorGroups.clear();

			for (size_t n = 0; n < visibleTargetObjectPointIds.size(); /*noop*/)
			{
				const Index32& targetObjectPointId = visibleTargetObjectPointIds[n];

				const FreakMultiDescriptorMap256::const_iterator iTarget = targetFreakMap.find(targetObjectPointId);
				if (iTarget != targetFreakMap.cend())
				{
					Vector2 imagePoint;
					if (targetDatabase.hasObservation<false>(targetPoseIndex, targetObjectPointId, &imagePoint))
					{
						imagePoints.emplace_back(imagePoint);
						imagePointDescriptorGroups.emplace_back(&iTarget->second);

						++n;

						continue;
					}
				}

				visibleTargetObjectPointIds[n] = visibleTargetObjectPointIds.back();
				visibleTargetObjectPointIds.pop_back();
			}

			const size_t previousNumberCorrespondingObjectPointIdPairs = correspondingSourceTargetObjectPointIdSet.size();

			HomogenousMatrix4 world_T_sourceCamera(false);
			usedObjectPointIndices.clear();
			usedObjectPointIds.clear();
			usedImagePointIndices.clear();

			sourceUnifiedUnguidedMatching->updateImagePoints(imagePoints.data(), imagePointDescriptorGroups.data(), imagePoints.size());
			sourceUnifiedGuidedMatching->updateImagePoints(imagePoints.data(), imagePointDescriptorGroups.data(), imagePoints.size());

			if (PoseEstimation::determinePose(AnyCameraPinhole(targetCamera), *sourceUnifiedUnguidedMatching, *sourceUnifiedGuidedMatching, randomGenerator, world_T_sourceCamera, minimalNumberCorrespondingFeaturesPerPose, maximalDescriptorDistance, maximalProjectionError, inlierRate, &usedObjectPointIds, &usedImagePointIndices))
			{
				ocean_assert(usedObjectPointIds.size() == usedImagePointIndices.size());

				for (size_t n = 0; n < usedObjectPointIds.size(); ++n)
				{
					const Index32& sourceObjectPointId = usedObjectPointIds[n];

					const Index32& targetImagePointIndex = usedImagePointIndices[n];
					const Index32& targetObjectPointId = visibleTargetObjectPointIds[targetImagePointIndex];

					correspondingSourceTargetObjectPointIdSet.emplace(sourceObjectPointId, targetObjectPointId);
				}

				if (previousNumberCorrespondingObjectPointIdPairs < correspondingSourceTargetObjectPointIdSet.size())
				{
					Log::info() << "Valid pose B: " << correspondingSourceTargetObjectPointIdSet.size() << ", " << usedObjectPointIds.size() << " (" << iteration << ")";

					world_T_sourceCameras.emplace_back(world_T_sourceCamera);
					world_T_targetCameras.emplace_back(world_T_targetCamera);

					iteration = 0u;
				}
			}

			sourceUnifiedUnguidedMatching->clearImagePoints();
			sourceUnifiedGuidedMatching->clearImagePoints();
		}

		if (world_T_sourceCameras.size() >= size_t(maximalNumberImprovements))
		{
			break;
		}
	}

	if (world_T_sourceCameras.size() < size_t(minimalNumberCorrespondingPoses))
	{
		return false;
	}

	HomogenousMatrix4 target_T_source(false);
	Scalar scale;
	if (!Geometry::AbsoluteTransformation::calculateTransformation(world_T_sourceCameras.data(), world_T_targetCameras.data(), world_T_sourceCameras.size(), target_T_source, Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale))
	{
		return false;
	}

	target_T_source.applyScale(Vector3(scale, scale, scale));


	std::vector<std::pair<UnorderedIndexSet32, UnorderedIndexSet32>> correspondingSourceTargetObjectPointPairs;
	correspondingSourceTargetObjectPointPairs.reserve(correspondingSourceTargetObjectPointIdSet.size());

	Indices32 mergedPairs;

	for (const IndexPair32& sourceTargetObjectPointId : correspondingSourceTargetObjectPointIdSet)
	{
		const Index32& sourceObjectPointId = sourceTargetObjectPointId.first;
		const Index32& targetObjectPointId = sourceTargetObjectPointId.second;

		mergedPairs.clear();

		for (size_t n = 0; n < correspondingSourceTargetObjectPointPairs.size(); ++n)
		{
			UnorderedIndexSet32& correspondingSourceObjectPoints = correspondingSourceTargetObjectPointPairs[n].first;
			UnorderedIndexSet32& correspondingTargetObjectPoints = correspondingSourceTargetObjectPointPairs[n].second;

			if (correspondingSourceObjectPoints.find(sourceObjectPointId) != correspondingSourceObjectPoints.cend() || correspondingTargetObjectPoints.find(targetObjectPointId) != correspondingTargetObjectPoints.cend())
			{
				correspondingSourceObjectPoints.emplace(sourceObjectPointId);
				correspondingTargetObjectPoints.emplace(targetObjectPointId);

				mergedPairs.emplace_back(Index32(n));
			}
		}

		if (mergedPairs.empty())
		{
			// the combination does not yet exist

			correspondingSourceTargetObjectPointPairs.emplace_back(UnorderedIndexSet32({sourceObjectPointId}), UnorderedIndexSet32({targetObjectPointId}));
		}
		else if (mergedPairs.size() > 1)
		{
			// let's merged all groups together

			UnorderedIndexSet32& firstCorrespondingSourceObjectPoints = correspondingSourceTargetObjectPointPairs[mergedPairs.front()].first;
			UnorderedIndexSet32& firstCorrespondingTargetObjectPoints = correspondingSourceTargetObjectPointPairs[mergedPairs.front()].second;

			for (size_t n = mergedPairs.size() - 1; n >= 1; --n)
			{
				const UnorderedIndexSet32& mergingCorrespondingSourceObjectPoints = correspondingSourceTargetObjectPointPairs[mergedPairs[n]].first;
				const UnorderedIndexSet32& mergingCorrespondingTargetObjectPoints = correspondingSourceTargetObjectPointPairs[mergedPairs[n]].second;

				firstCorrespondingSourceObjectPoints.insert(mergingCorrespondingSourceObjectPoints.cbegin(), mergingCorrespondingSourceObjectPoints.cend());
				firstCorrespondingTargetObjectPoints.insert(mergingCorrespondingTargetObjectPoints.cbegin(), mergingCorrespondingTargetObjectPoints.cend());

				correspondingSourceTargetObjectPointPairs[n] = std::move(correspondingSourceTargetObjectPointPairs.back());
				correspondingSourceTargetObjectPointPairs.pop_back();
			}
		}
	}

	Log::info() << "Found correspondingSourceTargetObjectPointPairs: " << correspondingSourceTargetObjectPointPairs.size();

	const Indices32 sourcePoseIds(sourceDatabase.poseIds<false, false>(HomogenousMatrix4(false)));
	const Index32 firstNewTargetPoseId = (((targetUpperPoseIndex) / 1000u) + 1u) * 1000u;

	std::unordered_map<Index32, Index32> oldSourcePoseIdToNewTargetPoseIdMap;
	oldSourcePoseIdToNewTargetPoseIdMap.reserve(sourceDatabase.poseNumber<false>());

	for (const Index32& sourcePoseId : sourcePoseIds)
	{
		const HomogenousMatrix4& sourceWorld_T_camera = sourceDatabase.pose<false>(sourcePoseId);
		ocean_assert(sourceWorld_T_camera.isValid());

		const Index32 newTargetPoseId = firstNewTargetPoseId + sourcePoseId;

		HomogenousMatrix4 target_T_camera = target_T_source * sourceWorld_T_camera;
		target_T_camera = HomogenousMatrix4(target_T_camera.translation(), target_T_camera.rotation());

		ocean_assert(!targetDatabase.hasPose<false>(newTargetPoseId));
		targetDatabase.addPose<false>(newTargetPoseId, target_T_camera);

		oldSourcePoseIdToNewTargetPoseIdMap.emplace(sourcePoseId, newTargetPoseId);
	}

	std::unordered_map<Index32, Index32> oldSourceObjectPointIdToNewTargetObjectPointIdMap;
	oldSourceObjectPointIdToNewTargetObjectPointIdMap.reserve(sourceDatabase.objectPointNumber<false>());

	std::unordered_map<Index32, Index32> oldSourceImagePointIdToNewTargetImagePointIdMap;
	oldSourceImagePointIdToNewTargetImagePointIdMap.reserve(sourceDatabase.imagePointNumber<false>());

	for (const Index32& sourceObjectPointId : sourceObjectPointIds)
	{
		const Vector3& sourceObjectPoint = sourceDatabase.objectPoint<false>(sourceObjectPointId);
		const Index32 newTargetObjectPointId = targetDatabase.addObjectPoint<false>(target_T_source * sourceObjectPoint);

		ocean_assert(targetFreakMap.find(newTargetObjectPointId) == targetFreakMap.cend());
		targetFreakMap[newTargetObjectPointId] = sourceFreakMap.find(sourceObjectPointId)->second;

		oldSourceObjectPointIdToNewTargetObjectPointIdMap.emplace(sourceObjectPointId, newTargetObjectPointId);

		const IndexSet32& sourceImagePointIds = sourceDatabase.imagePointsFromObjectPoint<false>(sourceObjectPointId);

		for (const Index32& sourceImagePointId : sourceImagePointIds)
		{
			const Index32 oldSourcePoseId = sourceDatabase.poseFromImagePoint<false>(sourceImagePointId);

			const Vector2& sourceImagePoint = sourceDatabase.imagePoint<false>(sourceImagePointId);
			const Index32 newTargetImagePointId = targetDatabase.addImagePoint<false>(sourceImagePoint);

			const std::unordered_map<Index32, Index32>::const_iterator iOldSource = oldSourcePoseIdToNewTargetPoseIdMap.find(oldSourcePoseId);

			if (iOldSource != oldSourcePoseIdToNewTargetPoseIdMap.cend())
			{
				const Index32 newTargetPoseId = iOldSource->second;

				targetDatabase.attachImagePointToPose<false>(newTargetImagePointId, newTargetPoseId);
			}

			oldSourceImagePointIdToNewTargetImagePointIdMap.emplace(sourceImagePointId, newTargetImagePointId);

			targetDatabase.attachImagePointToObjectPoint<false>(newTargetImagePointId, newTargetObjectPointId);
		}
	}

	for (size_t n = 0; n < correspondingSourceTargetObjectPointPairs.size(); ++n)
	{
		const UnorderedIndexSet32& correspondingSourceObjectPoints = correspondingSourceTargetObjectPointPairs[n].first;
		const UnorderedIndexSet32& correspondingTargetObjectPoints = correspondingSourceTargetObjectPointPairs[n].second;

		if (correspondingSourceObjectPoints.size() == 1 && correspondingTargetObjectPoints.size() == 1) // **TODO*
		{
			const Index32 oldSourceObjectPointId = *correspondingSourceObjectPoints.cbegin();
			const Index32 targetObjectPointId = *correspondingTargetObjectPoints.cbegin();

			ocean_assert(oldSourceObjectPointIdToNewTargetObjectPointIdMap.find(oldSourceObjectPointId) != oldSourceObjectPointIdToNewTargetObjectPointIdMap.cend());
			const Index32 newTargetObjectPointId = oldSourceObjectPointIdToNewTargetObjectPointIdMap[oldSourceObjectPointId];

			const Vector3 newObjectPoint = (targetDatabase.objectPoint<false>(targetObjectPointId) + targetDatabase.objectPoint<false>(newTargetObjectPointId)) * Scalar(0.5);
			const Scalar newPriority = Scalar(-1); // **TODO**

			targetDatabase.mergeObjectPoints<false>(targetObjectPointId, newTargetObjectPointId, newObjectPoint, newPriority);

			FreakMultiDescriptorMap256::iterator iMerged = targetFreakMap.find(targetObjectPointId);
			FreakMultiDescriptorMap256::iterator iRemoved = targetFreakMap.find(newTargetObjectPointId);

			ocean_assert(iMerged != targetFreakMap.cend());
			ocean_assert(iRemoved != targetFreakMap.cend());

			iMerged->second.insert(iMerged->second.cend(), iRemoved->second.cbegin(), iRemoved->second.cend());
		}
	}

	Tracking::Solver3::removeObjectPointsNotInFrontOfCamera(targetDatabase);

	if (!Tracking::MapBuilding::MapMerging::bundleAdjustment(targetDatabase, targetCamera, randomGenerator, 40u))
	{
		return false;
	}

	return true;
}

}

}

}
