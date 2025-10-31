/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/UnidirectionalCorrespondences.h"

#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace Tracking
{

UnidirectionalCorrespondences::UnidirectionalCorrespondences(const size_t numberForward, const size_t numberBackward)
{
	forwardCounters_.resize(numberForward);
	backwardCounters_.resize(numberBackward);
}

UnidirectionalCorrespondences::CorrespondencePairs UnidirectionalCorrespondences::correspondences() const
{
	const ScopedLock scopedLock(lock_);

	// let's find unidirectional correspondences

	CorrespondencePairs validCorrespondences;
	validCorrespondences.reserve(std::min(forwardCounters_.size(), backwardCounters_.size()) * 10 / 100);

	for (const CorrespondencePair& candidate : candidates_)
	{
		if (forwardCounters_[candidate.first] == 1u && backwardCounters_[candidate.second] == 1u)
		{
			validCorrespondences.push_back(candidate);
		}
	}

	return validCorrespondences;
}

size_t UnidirectionalCorrespondences::countBijectiveCorrespondences(const Index32* usedPointIndices, const size_t size)
{
	if (size == 0)
	{
		return 0;
	}

	ocean_assert(usedPointIndices != nullptr);

	std::unordered_map<Index32, size_t> imagePointMap;
	imagePointMap.reserve(size);

	size_t nonBijectiveCorrespondences = 0;

	for (size_t nCorrespondence = 0; nCorrespondence < size; ++nCorrespondence)
	{
		const Index32 imagePointIndex = usedPointIndices[nCorrespondence];

		size_t& usageCounter = imagePointMap[imagePointIndex];
		++usageCounter;

		if (usageCounter != 1)
		{
			// the image point has been used more than once

			if (usageCounter == 2)
			{
				nonBijectiveCorrespondences += 2;
			}
			else
			{
				ocean_assert(usageCounter >= 3);
				++nonBijectiveCorrespondences;
			}
		}
	}

	ocean_assert(nonBijectiveCorrespondences <= size);

	return size - nonBijectiveCorrespondences;
}

size_t UnidirectionalCorrespondences::countNonBijectiveCorrespondences(const Index32* usedPointIndices, const size_t size)
{
	if (size == 0)
	{
		return 0;
	}

	ocean_assert(usedPointIndices != nullptr);

	std::unordered_map<Index32, size_t> imagePointMap;
	imagePointMap.reserve(size);

	size_t nonBijectiveCorrespondences = 0;

	for (size_t nCorrespondence = 0; nCorrespondence < size; ++nCorrespondence)
	{
		const Index32 imagePointIndex = usedPointIndices[nCorrespondence];

		size_t& usageCounter = imagePointMap[imagePointIndex];
		++usageCounter;

		if (usageCounter != 1)
		{
			// the image point has been used more than once

			if (usageCounter == 2)
			{
				++nonBijectiveCorrespondences;
			}
		}
	}

	ocean_assert(nonBijectiveCorrespondences <= size);

	return nonBijectiveCorrespondences;
}

void UnidirectionalCorrespondences::removeNonBijectiveCorrespondences(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3* objectPoints, const Vector2* imagePoints, Indices32& objectPointIndices, Indices32& imagePointIndices, const bool checkImagePoints)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints != nullptr && imagePoints != nullptr);

	ocean_assert(!objectPointIndices.empty());
	ocean_assert(objectPointIndices.size() == imagePointIndices.size());

	// let's ensure that each object point is used only by one image point

#ifdef OCEAN_DEBUG
	const size_t debugInitialBijectiveCorrespondences = countBijectiveCorrespondences(checkImagePoints ? imagePointIndices.data() : objectPointIndices.data(), objectPointIndices.size());
	const size_t debugInitialNonBijectiveCorrespondences = countNonBijectiveCorrespondences(checkImagePoints ? imagePointIndices.data() : objectPointIndices.data(), objectPointIndices.size());
#endif

	const HomogenousMatrix4 flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_camera));

	using CorrespondenceMap = std::unordered_map<Index32, size_t>;

	CorrespondenceMap backwardMap;
	backwardMap.reserve(objectPointIndices.size());

	for (size_t nCorrespondence = 0; nCorrespondence < objectPointIndices.size(); /*noop*/)
	{
		const Index32 newObjectPointIndex = objectPointIndices[nCorrespondence];
		const Index32 newImagePointIndex = imagePointIndices[nCorrespondence];

		const Index32 newPointIndexToCheck = checkImagePoints ? newImagePointIndex : newObjectPointIndex;

		const CorrespondenceMap::const_iterator iBackward = backwardMap.find(newPointIndexToCheck);
		if (iBackward == backwardMap.cend())
		{
			// the point index we checked (either from an object point or image point) has not been used before, so currently it is a bijective correspondence
			backwardMap.emplace(newPointIndexToCheck, nCorrespondence);
		}
		else
		{
			// the point index we checked is part of a bijective correspondence, so we need to decide which correspondence is better

			const size_t existingCorrespondenceIndex = iBackward->second;
			ocean_assert(existingCorrespondenceIndex < nCorrespondence);

			Index32 exitingImagePointIndex = Index32(-1);
			Index32 existingObjectPointIndex = Index32(-1);

			if (checkImagePoints)
			{
				ocean_assert(newImagePointIndex == iBackward->first);

				exitingImagePointIndex = newImagePointIndex;
				existingObjectPointIndex = objectPointIndices[existingCorrespondenceIndex];
			}
			else
			{
				ocean_assert(newObjectPointIndex == iBackward->first);

				exitingImagePointIndex = imagePointIndices[existingCorrespondenceIndex];
				existingObjectPointIndex = newObjectPointIndex;
			}

			const Vector3& existingObjectPoint =  objectPoints[existingObjectPointIndex];
			const Vector2& existingImagePoint = imagePoints[exitingImagePointIndex];

			const Vector3& newObjectPoint = objectPoints[newObjectPointIndex];
			const Vector2& newImagePoint = imagePoints[newImagePointIndex];

			ocean_assert(existingObjectPoint == newObjectPoint || existingImagePoint == newImagePoint);

			const Vector2 projectedExistingObjectPoint = camera.projectToImageIF(flippedCamera_T_world, existingObjectPoint);
			const Vector2 projectedNewObjectPoint = camera.projectToImageIF(flippedCamera_T_world, newObjectPoint);

			const Scalar sqrDistanceExisting = projectedExistingObjectPoint.sqrDistance(existingImagePoint);
			const Scalar sqrDistanceNew = projectedNewObjectPoint.sqrDistance(newImagePoint);

			if (sqrDistanceNew < sqrDistanceExisting)
			{
				// the new correspondence is better, so we replace the existing one

				if (checkImagePoints)
				{
					ocean_assert(imagePointIndices[existingCorrespondenceIndex] == newImagePointIndex);
					ocean_assert(objectPointIndices[existingCorrespondenceIndex] != newObjectPointIndex);

					objectPointIndices[existingCorrespondenceIndex] = newObjectPointIndex;
				}
				else
				{
					ocean_assert(imagePointIndices[existingCorrespondenceIndex] != newImagePointIndex);
					ocean_assert(objectPointIndices[existingCorrespondenceIndex] == newObjectPointIndex);

					imagePointIndices[existingCorrespondenceIndex] = newImagePointIndex;
				}
			}
			else
			{
				// the existing correspondence is better, so we ignore the new one
			}

			// the current correspondence has either ben moved to the existing correspondence or has been removed

			objectPointIndices[nCorrespondence] = objectPointIndices.back();
			imagePointIndices[nCorrespondence] = imagePointIndices.back();

			objectPointIndices.pop_back();
			imagePointIndices.pop_back();

			continue;
		}

		++nCorrespondence;
	}

	ocean_assert(objectPointIndices.size() == imagePointIndices.size());

#ifdef OCEAN_DEBUG
	{
		const size_t debugFinalBijectiveCorrespondences = countBijectiveCorrespondences(checkImagePoints ? imagePointIndices.data() : objectPointIndices.data(), objectPointIndices.size());

		ocean_assert(debugInitialBijectiveCorrespondences + debugInitialNonBijectiveCorrespondences == debugFinalBijectiveCorrespondences);
		ocean_assert(objectPointIndices.size() == debugFinalBijectiveCorrespondences);
	}
#endif
}

}

}
