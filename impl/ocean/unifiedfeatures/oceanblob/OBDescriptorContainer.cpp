// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanblob/OBDescriptorContainer.h"
#include "arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h"
#include "ocean/cv/detector/blob/BlobFeature.h"

#include <iterator>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

bool OBDescriptorContainer::toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = static_cast<int32_t>(objectVersion());

	Thrift::DescriptorContainer localThriftDescriptorContainer;
	localThriftDescriptorContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;
	localThriftDescriptorContainer.length_ref() = static_cast<int32_t>(descriptorLengthInBytes());
	localThriftDescriptorContainer.levels_ref() = numberLevels();

	std::vector<Thrift::FloatDescriptor> thriftFloatDescriptors;
	thriftFloatDescriptors.reserve(blobFeatures_.size());

	for (const CV::Detector::Blob::BlobFeature& feature : blobFeatures_)
	{
		Thrift::FloatDescriptor& thriftFloatDescriptor = thriftFloatDescriptors.emplace_back();

		ocean_assert(thriftFloatDescriptor.is_set());
		std::vector<std::vector<float>>& descriptorData = *(thriftFloatDescriptor.descriptorData_ref());

		// Only 1 layer
		descriptorData.resize(1);

		descriptorData[0].reserve(descriptorDimensions());

		std::copy(
			feature.descriptor()(),
			feature.descriptor()() + descriptorDimensions(),
			std::back_inserter(descriptorData[0]));
	}

	localThriftDescriptorContainer.descriptorUnion_ref()->floatDescriptors_ref() = std::move(thriftFloatDescriptors);
	ocean_assert(localThriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::floatDescriptors);

	thriftDescriptorContainer = std::move(localThriftDescriptorContainer);

	return true;
}

bool OBDescriptorContainer::fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> /*featureContainer*/)
{
	blobFeatures_.clear();

	if (thriftDescriptorContainer.get_unifiedObjectInfo().get_libraryName() != libraryName() || thriftDescriptorContainer.get_unifiedObjectInfo().get_name() != name() || (unsigned int)(thriftDescriptorContainer.get_unifiedObjectInfo().get_version()) != objectVersion() || size_t(thriftDescriptorContainer.get_length()) != descriptorLengthInBytes() || (unsigned int)(thriftDescriptorContainer.get_levels()) != numberLevels() || thriftDescriptorContainer.get_descriptorUnion().getType() != Thrift::DescriptorUnion::Type::floatDescriptors)
	{
		ocean_assert(false && "Incompatible descriptor container");
		return false;
	}

	const std::vector<Thrift::FloatDescriptor>& thriftFloatDescriptors = thriftDescriptorContainer.get_descriptorUnion().get_floatDescriptors();

	blobFeatures_.reserve(thriftFloatDescriptors.size());

	for (const Thrift::FloatDescriptor& thriftFloatDescriptor : thriftFloatDescriptors)
	{
		const size_t numberDescriptorLevels = thriftFloatDescriptor.descriptorData_ref()->size();

		if (numberDescriptorLevels == 0 || numberDescriptorLevels > size_t(numberLevels()))
		{
			ocean_assert(false && "Invalid number of descriptor levels - this should never happen!");
			return false;
		}

		if ((*thriftFloatDescriptor.descriptorData_ref())[0].size() != size_t(descriptorLengthInBytes()))
		{
			ocean_assert(false && "Invalid descriptor size - this should never happen!");
			return false;
		}

		CV::Detector::Blob::BlobFeature& feature = blobFeatures_.emplace_back();
		memcpy(feature.descriptor()(), (*thriftFloatDescriptor.descriptorData_ref())[0].data(), descriptorLengthInBytes());
	}

	return true;
}

OBDescriptorContainer& OBDescriptorContainer::operator=(OBDescriptorContainer&& blobDescriptors)
{
	if (this != &blobDescriptors)
	{
		blobFeatures_ = std::move(blobDescriptors.blobFeatures_);
	}

	return *this;
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
