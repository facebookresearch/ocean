// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanblob/OBNonBijectiveMatcher.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"
#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"
#include "ocean/unifiedfeatures/oceanblob/OBDescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanblob/OBDescriptorGenerator.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

DescriptorMatcher::Result OBNonBijectiveMatcher::matchDescriptors(
	const DescriptorContainer& descriptorContainer0,
	const DescriptorContainer& descriptorContainer1,
	IndexPairs32& correspondences,
	Worker* worker) const
{
	const OBDescriptorContainer* blobContainer0 = dynamic_cast<const OBDescriptorContainer*>(&descriptorContainer0);
	const OBDescriptorContainer* blobContainer1 = dynamic_cast<const OBDescriptorContainer*>(&descriptorContainer1);

	if (blobContainer0 == nullptr || blobContainer1 == nullptr)
	{
		return Result::ERROR_INCOMPATIBLE_DESCRIPTORS;
	}

	if (blobContainer0->size() != 0 && blobContainer1->size() != 0)
	{

		if (parameters_.earlyRejection_)
		{
			correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesEarlyReject(blobContainer0->blobFeatures(), blobContainer1->blobFeatures(), blobContainer0->blobFeatures().size(), parameters_.matchingThreshold_);
		}
		else
		{
			correspondences = Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondences(blobContainer0->blobFeatures(), blobContainer1->blobFeatures(), blobContainer0->blobFeatures().size(), parameters_.matchingThreshold_);
		}
	}

	return Result::SUCCESS;
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
