// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftBruteForceDescriptorMatcher.h"

#include "ocean/base/Base.h"
#include "ocean/math/Numeric.h"
#include "ocean/unifiedfeatures/FeatureDetector.h"

#include "ocean/unifiedfeatures/vlfeatsift/Utilities.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftDescriptorContainer.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureContainer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

DescriptorMatcher::Result VlfeatSiftBruteForceDescriptorMatcher::matchDescriptors(const DescriptorContainer& descriptorContainer0, const DescriptorContainer& descriptorContainer1, IndexPairs32& correspondences, Worker* worker) const
{
	const VlfeatSiftDescriptorContainer* siftContainer0 = dynamic_cast<const VlfeatSiftDescriptorContainer*>(&descriptorContainer0);
	const VlfeatSiftDescriptorContainer* siftContainer1 = dynamic_cast<const VlfeatSiftDescriptorContainer*>(&descriptorContainer1);

	if (siftContainer0 == nullptr || siftContainer1 == nullptr)
	{
		return Result::ERROR_INCOMPATIBLE_DESCRIPTORS;
	}

	if (siftContainer0->size() == 0 || siftContainer1->size() == 0)
	{
		// Nothing to be matched.
		correspondences = IndexPairs32();
		return Result::SUCCESS;
	}

	const VlfeatSiftDescriptorContainer::SiftDescriptors& siftDescriptors0 = siftContainer0->descriptors_;
	const VlfeatSiftDescriptorContainer::SiftDescriptors& siftDescriptors1 = siftContainer1->descriptors_;
	ocean_assert(!siftDescriptors0.empty() && !siftDescriptors1.empty());

	const float ratio = parameters_.matchingRatio;
	ocean_assert(ratio > 0.0f);
	const float maximumMatchingDistance = parameters_.maximumMatchingDistance;

	for (size_t idx0 = 0; idx0 < siftDescriptors0.size(); ++idx0)
	{
		const VlfeatSiftDescriptorContainer::SiftDescriptor& siftDescriptor0 = siftDescriptors0[idx0];
		BestMatchHistory bestMatchHistory;

		for (size_t idx1 = 0; idx1 < siftDescriptors1.size(); ++idx1)
		{
			bestMatchHistory.addMatch(Index32(idx1), siftDescriptor0.computeDistance(siftDescriptors1[idx1]));
		}

		if (bestMatchHistory.isSignificantMatch(ratio))
		{
			float bestMatchDistance = NumericF::maxValue();
			const Index32 bestMatchIndex = Index32(bestMatchHistory.bestMatch(0u, &bestMatchDistance));
			ocean_assert(bestMatchIndex >= 0u && size_t(bestMatchIndex) < siftDescriptors1.size());

			if (bestMatchDistance < maximumMatchingDistance)
			{
				correspondences.emplace_back(Index32(idx0), bestMatchIndex);
			}
		}
	}

	return Result::SUCCESS;
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
