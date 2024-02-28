// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

#include "ocean/unifiedfeatures/deeppatchuint8/DP8FeatureContainer.h"
#include "ocean/unifiedfeatures/deeppatchuint8/DP8DescriptorContainer.h"

#include <arvr/libraries/thrift_if/common/gen-cpp2/visual_features_types.h>
#include <arvr/libraries/thrift_if/reloc/gen-cpp2/query_types.h>

#include <array>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

/**
 * Definition of utility function related to Landmarks AR and relocalization
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
class OCEAN_UNIFIEDFEATURES_DP8_EXPORT RelocalizationUtilities
{
	public:

		/**
		 * Converts a feature container and descriptor container from the DeepPatchUint8 library to the Thrift data structure used by relocalization/Landmark AR.
		 * @param featureContainer The feature container that will converted into a Thrift data structure
		 * @param descriptorContainer The feature container that will converted into a Thrift data structure
		 * @param imageFeatures The resulting Thrift data structure used by relocalization/Landmark AR
		 * @return True, if the conversion was successful, otherwise false
		 */
		static inline bool toThrift(const FeatureContainer& featureContainer, const DescriptorContainer& descriptorContainer, arvr::thrift_if::relocalization::ImageFeatures& imageFeatures);
};

inline bool RelocalizationUtilities::toThrift(const FeatureContainer& featureContainer, const DescriptorContainer& descriptorContainer, arvr::thrift_if::relocalization::ImageFeatures& imageFeatures)
{
	if (featureContainer.name() != DP8FeatureContainer::nameDP8FeatureContainer() || descriptorContainer.name() != DP8DescriptorContainer::nameDP8DescriptorContainer())
	{
		ocean_assert(false && "Incompatible feature and descriptor containers");
		return false;
	}

	const DP8FeatureContainer* dp8FeatureContainer = dynamic_cast<const DP8FeatureContainer*>(&featureContainer);
	const DP8DescriptorContainer* dp8DescriptorContainer = dynamic_cast<const DP8DescriptorContainer*>(&descriptorContainer);

	if (dp8FeatureContainer == nullptr || dp8DescriptorContainer == nullptr)
	{
		assert(false && "This should never happen!");
		return false;
	}

	const std::vector<perception::features::Keypoint>& keypoints = dp8FeatureContainer->keypoints_;
	const DP8Descriptors96& descriptors = dp8DescriptorContainer->descriptors_;

	const size_t numberFeatures = keypoints.size();

	if (descriptors.size() != numberFeatures)
	{
		ocean_assert(false && "Size of features and descriptors do not match - this should never happen!");
		return false;
	}

	*imageFeatures.descriptor_type_ref() = arvr::thrift_if::common::visual_features::DescriptorType::DeepPatchUint8_96;
	imageFeatures.descriptors_ref()->reserve(numberFeatures);
	imageFeatures.keypoints_ref()->reserve(numberFeatures);

	for (size_t i = 0; i < numberFeatures; ++i)
	{
		// Keypoint
		arvr::thrift_if::mapping::descriptors::Keypoint& keypoint = imageFeatures.keypoints_ref()->emplace_back();
		keypoint.position_level_0_ref()->resize(2);
		keypoint.position_level_0_ref()[0] = keypoints[i].positionLevel0[0];
		keypoint.position_level_0_ref()[1] = keypoints[i].positionLevel0[1];
		*keypoint.scale_ref() = keypoints[i].scale;
		*keypoint.score_ref() = keypoints[i].score;
		*keypoint.angle_ref() = keypoints[i].angle;

		// Descriptor
		arvr::thrift_if::common::visual_features::Uint8Descriptor descriptor;
		descriptor.resize(descriptors[i].size());
		std::copy(descriptors[i].begin(), descriptors[i].end(), descriptor.begin());

		arvr::thrift_if::common::visual_features::AnyFeatureDescriptor& anyFeatureDescriptor = imageFeatures.descriptors_ref()->emplace_back();
		anyFeatureDescriptor.set_uint8_descriptor(std::move(descriptor));
	}

	return true;
}

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
