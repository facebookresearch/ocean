// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

#include "ocean/unifiedfeatures/deepdescriptor/DDHarrisFeatureContainer.h"
#include "ocean/unifiedfeatures/deepdescriptor/DDFloatDescriptorContainer.h"

#include <perception/features/deep/Deep.h>

#include <arvr/libraries/thrift_if/reloc/gen-cpp2/query_types.h>

#include <array>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

/**
 * Definition of utility function related to Landmarks AR and relocalization
 * @ingroup unifiedfeaturesdeepdescriptor
 */
class OCEAN_UNIFIEDFEATURES_DD_EXPORT RelocalizationUtilities
{
	public:

		/**
		 * Converts a feature container and descriptor container from the deep descriptor library to the Thrift data structure used by relocalization/Landmark AR.
		 * @param featureContainer The feature container that will converted into a Thrift data structure
		 * @param descriptorContainer The feature container that will converted into a Thrift data structure
		 * @param imageFeatures The resulting Thrift data structure used by relocalization/Landmark AR
		 * @return True, if the conversion was successful, otherwise false
		 */
		static inline bool toThrift(const FeatureContainer& featureContainer, const DescriptorContainer& descriptorContainer, arvr::thrift_if::relocalization::ImageFeatures& imageFeatures);
};

inline bool RelocalizationUtilities::toThrift(const FeatureContainer& featureContainer, const DescriptorContainer& descriptorContainer, arvr::thrift_if::relocalization::ImageFeatures& imageFeatures)
{
	if (featureContainer.name() != DDHarrisFeatureContainer::nameDDHarrisFeatureContainer() || descriptorContainer.name() != DDFloatDescriptorContainer::nameDDFloatDescriptorContainer())
	{
		ocean_assert(false && "Incompatible feature and descriptor containers");
		return false;
	}

	const DDHarrisFeatureContainer* ddHarrisFeatureContainer = dynamic_cast<const DDHarrisFeatureContainer*>(&featureContainer);
	const DDFloatDescriptorContainer* ddFloatDescriptorContainer = dynamic_cast<const DDFloatDescriptorContainer*>(&descriptorContainer);

	if (ddHarrisFeatureContainer == nullptr || ddFloatDescriptorContainer == nullptr)
	{
		assert(false && "This should never happen!");
		return false;
	}

	const std::vector<perception::features::Keypoint>& perceptionKeypoints = ddHarrisFeatureContainer->perceptionKeypoints_;
	const DeepPatchFloatDescriptors& deepPatchFloatDescriptors = ddFloatDescriptorContainer->deepPatchFloatDescriptors_;

	const size_t numberFeatures = perceptionKeypoints.size();

	if (deepPatchFloatDescriptors.size() != numberFeatures)
	{
		ocean_assert(false && "Size of features and descriptors do not match - this should never happen!");
		return false;
	}

	*imageFeatures.descriptor_type_ref() = arvr::thrift_if::common::visual_features::DescriptorType::DeepPatchFloat128;
	imageFeatures.descriptors_ref()->reserve(numberFeatures);
	imageFeatures.keypoints_ref()->reserve(numberFeatures);

	for (size_t i = 0; i < numberFeatures; ++i)
	{
		// Keypoint
		arvr::thrift_if::mapping::descriptors::Keypoint& keypoint = imageFeatures.keypoints_ref()->emplace_back();
		keypoint.position_level_0_ref()->resize(2);
		keypoint.position_level_0_ref()[0] = perceptionKeypoints[i].positionLevel0[0];
		keypoint.position_level_0_ref()[1] = perceptionKeypoints[i].positionLevel0[1];
		*keypoint.scale_ref() = perceptionKeypoints[i].scale;
		*keypoint.score_ref() = perceptionKeypoints[i].score;
		*keypoint.angle_ref() = perceptionKeypoints[i].angle;

		// Descriptor
		arvr::thrift_if::common::visual_features::FloatDescriptor floatDescriptor;
		floatDescriptor.resize(deepPatchFloatDescriptors[i].size());
		std::copy(deepPatchFloatDescriptors[i].begin(), deepPatchFloatDescriptors[i].end(), floatDescriptor.begin());

		arvr::thrift_if::common::visual_features::AnyFeatureDescriptor& anyFeatureDescriptor = imageFeatures.descriptors_ref()->emplace_back();
		anyFeatureDescriptor.set_float_descriptor(std::move(floatDescriptor));
	}

	return true;
}

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
