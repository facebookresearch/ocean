// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftDescriptorContainer.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

#include <climits>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

bool VlfeatSiftDescriptorContainer::toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = int32_t(objectVersion());

	Thrift::DescriptorContainer localThriftDescriptorContainer;
	localThriftDescriptorContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;
	localThriftDescriptorContainer.length_ref() = int32_t(descriptorLengthInBytes());
	localThriftDescriptorContainer.levels_ref() = int32_t(numberLevels());

	std::vector<Thrift::FloatDescriptor> thriftFloatDescriptors;
	thriftFloatDescriptors.reserve(descriptors_.size());

	for (const SiftDescriptor& descriptor : descriptors_)
	{
		Thrift::FloatDescriptor& thriftFloatDescriptor = thriftFloatDescriptors.emplace_back();
		thriftFloatDescriptor.descriptorData_ref()->emplace_back(descriptor.descriptorLevels().front().begin(), descriptor.descriptorLevels().front().end());
	}

	localThriftDescriptorContainer.descriptorUnion_ref()->floatDescriptors_ref() = std::move(thriftFloatDescriptors);
	ocean_assert(localThriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::floatDescriptors);

	thriftDescriptorContainer = std::move(localThriftDescriptorContainer);

	return true;
}

bool VlfeatSiftDescriptorContainer::fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> /* featureContainer */)
{
	descriptors_.clear();

	if (thriftDescriptorContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftDescriptorContainer.get_unifiedObjectInfo().get_name() != name()
		|| (unsigned int)(thriftDescriptorContainer.get_unifiedObjectInfo().get_version()) != objectVersion()
		|| size_t(thriftDescriptorContainer.get_length()) != descriptorLengthInBytes()
		|| (unsigned int)(thriftDescriptorContainer.get_levels()) != numberLevels()
		|| thriftDescriptorContainer.get_descriptorUnion().getType() != Thrift::DescriptorUnion::Type::floatDescriptors)
	{
		ocean_assert(false && "Incompatible descriptor container");
		return false;
	}

	const std::vector<Thrift::FloatDescriptor>& thriftFloatDescriptors = thriftDescriptorContainer.get_descriptorUnion().get_floatDescriptors();

	descriptors_.reserve(thriftFloatDescriptors.size());

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

		SiftDescriptor& descriptor = descriptors_.emplace_back();
		memcpy(descriptor.descriptorLevels().front().data(), (*thriftFloatDescriptor.descriptorData_ref())[0].data(), descriptorLengthInBytes());
	}

	return true;
}

VlfeatSiftDescriptorContainer& VlfeatSiftDescriptorContainer::operator=(VlfeatSiftDescriptorContainer&& descriptorContainer)
{
	if (this != &descriptorContainer)
	{
		descriptors_ = std::move(descriptorContainer.descriptors_);
	}

	return *this;
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
