// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanorb/OONonBijectiveMatcher.h"
#include "ocean/unifiedfeatures/oceanorb/OODescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanorb/OODescriptorGenerator.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

DescriptorMatcher::Result OONonBijectiveMatcher::matchDescriptors(const DescriptorContainer& descriptorContainer0, const DescriptorContainer& descriptorContainer1, IndexPairs32& correspondences, Worker* worker) const
{
	const OODescriptorContainer* orbContainer0 = dynamic_cast<const OODescriptorContainer*>(&descriptorContainer0);
	const OODescriptorContainer* orbContainer1 = dynamic_cast<const OODescriptorContainer*>(&descriptorContainer1);

	if (orbContainer0 == nullptr || orbContainer1 == nullptr)
	{
		return Result::ERROR_INCOMPATIBLE_DESCRIPTORS;
	}

	if (orbContainer0->size() != 0 && orbContainer1->size() != 0)
	{
		CV::Detector::ORBFeatureDescriptor::determineNonBijectiveCorrespondences(orbContainer0->orbFeatures_, orbContainer1->orbFeatures_, correspondences, parameters_.matchingThreshold_, worker);
	}

	return Result::SUCCESS;
}

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
