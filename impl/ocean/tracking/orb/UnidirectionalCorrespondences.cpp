/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/orb/UnidirectionalCorrespondences.h"

#include "ocean/cv/detector/ORBFeatureDescriptor.h"

#include "ocean/geometry/Quadric.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

using namespace CV;
using namespace CV::Detector;

UnidirectionalCorrespondences::UnidirectionalCorrespondences(const unsigned int numberForward, const unsigned int numberBackward)
{
	correspondenceForwardCounters.resize(numberForward);
	correspondenceBackwardCounters.resize(numberBackward);
}

void UnidirectionalCorrespondences::addCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex)
{
	correspondenceCandidates.push_back(CorrespondencePair(forwardIndex, backwardIndex));

	ocean_assert(forwardIndex < correspondenceForwardCounters.size());
	++correspondenceForwardCounters[forwardIndex];

	ocean_assert(backwardIndex < correspondenceBackwardCounters.size());
	++correspondenceBackwardCounters[backwardIndex];
}

void UnidirectionalCorrespondences::lockedAddCandidate(const unsigned int forwardIndex, const unsigned int backwardIndex)
{
	const ScopedLock scopedLock(correspondenceLock);
	addCandidate(forwardIndex, backwardIndex);
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::correspondences() const
{
	CorrespondencePairs validCorrespondences;

	// find unidirectional correspondences
	for (CorrespondencePairs::const_iterator i = correspondenceCandidates.begin(); i != correspondenceCandidates.end(); ++i)
		if (correspondenceForwardCounters[i->first] == 1 && correspondenceBackwardCounters[i->second] == 1)
			validCorrespondences.push_back(CorrespondencePair(i->first, i->second));

	return validCorrespondences;
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineBijectiveCorrespondences(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, const float threshold, Worker* worker)
{
	ocean_assert(forwardFeatures.size() >= 1 && backwardFeatures.size() >= 1);
	ocean_assert(threshold >= 0.0f && threshold <= 1.0f);

	UnidirectionalCorrespondences candidates((unsigned int)(forwardFeatures.size()), (unsigned int)(backwardFeatures.size()));

	CorrespondencePairs nonBijectiveCorrespondences;
	ORBFeatureDescriptor::determineNonBijectiveCorrespondences(forwardFeatures, backwardFeatures, nonBijectiveCorrespondences, threshold, worker);

	for (const CorrespondencePair& nonBijectiveCorrespondence : nonBijectiveCorrespondences)
	{
		candidates.addCandidate(nonBijectiveCorrespondence.first, nonBijectiveCorrespondence.second);
	}

	return candidates.correspondences();
}

Correspondences::CorrespondencePairs UnidirectionalCorrespondences::determineBijectiveCorrespondencesWithPose(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const float threshold)
{
	ocean_assert(forwardFeatures.size() >= 1 && backwardFeatures.size() >= 1);
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(threshold >= 0.0f && threshold <= 1.0f);

	const size_t numberDescriptorBitset = sizeof(ORBDescriptor::DescriptorBitset) * 8;
	const unsigned maximalHammingDistance = (unsigned int)(float(numberDescriptorBitset) * threshold + 0.5f);

	UnidirectionalCorrespondences candidates((unsigned int)(forwardFeatures.size()), (unsigned int)(backwardFeatures.size()));

	Vectors2 imagePoints;
	imagePoints.reserve(forwardFeatures.size());

	for (size_t i = 0; i < forwardFeatures.size(); ++i)
	{
		imagePoints.push_back(forwardFeatures[i].observation());
	}

	const Geometry::SpatialDistribution::DistributionArray distributionArray = Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), imagePoints.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), (unsigned int)(pinholeCamera.width() * 0.1), (unsigned int)(pinholeCamera.height() * 0.1));

	for (size_t nBackward = 0; nBackward < backwardFeatures.size(); ++nBackward)
	{
		const ORBFeature& backwardFeature = backwardFeatures[nBackward];

		const Vector2 projectedBackwardFeaturePoint = pinholeCamera.projectToImage<false>(pose, backwardFeature.position(), true);

		if (!pinholeCamera.isInside(projectedBackwardFeaturePoint))
		{
			continue;
		}

		const int horizontalBin = distributionArray.horizontalBin(projectedBackwardFeaturePoint.x());
		const int verticalBin = distributionArray.verticalBin(projectedBackwardFeaturePoint.y());

		ocean_assert(horizontalBin >= 0 && verticalBin >= 0);

		const Indices32 forwardIndices = distributionArray.indicesNeighborhood9((unsigned int)horizontalBin, (unsigned int)verticalBin);

		unsigned int bestMatchingForwardFeature = (unsigned int)(-1);
		unsigned int bestHammingDistance = maximalHammingDistance + 1u;

		const ORBDescriptors& backwardDescriptors = backwardFeature.descriptors();

		for (const Index32& forwardIndex : forwardIndices)
		{
			const ORBDescriptors& forwardDescriptors = forwardFeatures[forwardIndex].descriptors();

			for (size_t nForwardDescriptor = 0; nForwardDescriptor < forwardDescriptors.size(); ++nForwardDescriptor)
			{
				const ORBDescriptor& forwardDescriptor = forwardDescriptors[nForwardDescriptor];

				for (size_t nBackwardDescriptor = 0; nBackwardDescriptor < backwardDescriptors.size(); ++nBackwardDescriptor)
				{
					const ORBDescriptor& backwardDescriptor = backwardDescriptors[nBackwardDescriptor];

					const unsigned int hammingDistance = CV::Detector::ORBFeatureDescriptor::calculateHammingDistance(forwardDescriptor, backwardDescriptor);

					if (hammingDistance < bestHammingDistance)
					{
						bestMatchingForwardFeature = forwardIndex;
						bestHammingDistance = hammingDistance;
					}
				}
			}
		}

		if (bestMatchingForwardFeature != (unsigned int)(-1))
		{
			ocean_assert(bestHammingDistance <= maximalHammingDistance);
			candidates.addCandidate(bestMatchingForwardFeature, (unsigned int)nBackward);
		}
	}

	return candidates.correspondences();
}

}

}

}
