// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deeppatchuint8/DP8DescriptorContainer.h"
#include "ocean/unifiedfeatures/deeppatchuint8/DeepPatchUint8.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

#include <climits>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

unsigned int DP8DescriptorContainer::numberLevels() const
{
	return 1u;
}

unsigned int DP8DescriptorContainer::descriptorLengthInBytes() const
{
	return descriptorLengthInElements() * sizeof(uint8_t);
}

unsigned int DP8DescriptorContainer::descriptorLengthInElements() const
{
	return dp8Descriptor96Size;
}

bool DP8DescriptorContainer::toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.set_libraryName(libraryName());
	unifiedObjectInfo.set_name(name());
	unifiedObjectInfo.set_version(int32_t(objectVersion()));

	Thrift::DescriptorContainer localThriftDescriptorContainer;
	localThriftDescriptorContainer.set_unifiedObjectInfo(unifiedObjectInfo);
	localThriftDescriptorContainer.set_length(int32_t(descriptorLengthInBytes()));
	localThriftDescriptorContainer.set_levels(int32_t(numberLevels()));

	std::vector<Thrift::Uint8Descriptor> thriftUint8Descriptors;
	thriftUint8Descriptors.reserve(descriptors_.size());

	for (const DP8Descriptor96& descriptor : descriptors_)
	{
		Thrift::Uint8Descriptor& thriftUint8Descriptor = thriftUint8Descriptors.emplace_back();
		thriftUint8Descriptor.descriptorData_ref()->emplace_back(descriptor.begin(), descriptor.end());
	}

	localThriftDescriptorContainer.descriptorUnion_ref()->set_uint8Descriptors(std::move(thriftUint8Descriptors));
	ocean_assert(localThriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::uint8Descriptors);

	thriftDescriptorContainer = std::move(localThriftDescriptorContainer);

	return true;
}

bool DP8DescriptorContainer::fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> /* featureContainer */)
{
	descriptors_.clear();

	if (thriftDescriptorContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftDescriptorContainer.get_unifiedObjectInfo().get_name() != name()
		|| (unsigned int)(thriftDescriptorContainer.get_unifiedObjectInfo().get_version()) != objectVersion()
		|| size_t(thriftDescriptorContainer.get_length()) != descriptorLengthInBytes()
		|| (unsigned int)(thriftDescriptorContainer.get_levels()) != numberLevels()
		|| thriftDescriptorContainer.get_descriptorUnion().getType() != Thrift::DescriptorUnion::Type::uint8Descriptors)
	{
		ocean_assert(false && "Incompatible descriptor container");
		return false;
	}

	const std::vector<Thrift::Uint8Descriptor>& thriftUint8Descriptors = thriftDescriptorContainer.get_descriptorUnion().get_uint8Descriptors();

	descriptors_.reserve(thriftUint8Descriptors.size());

	for (const Thrift::Uint8Descriptor& thriftUint8Descriptor : thriftUint8Descriptors)
	{
		const size_t numberDescriptorLevels = thriftUint8Descriptor.descriptorData_ref()->size();

		if (numberDescriptorLevels == 0 || numberDescriptorLevels > size_t(numberLevels()))
		{
			ocean_assert(false && "Invalid number of descriptor levels - this should never happen!");
			return false;
		}

		if ((*thriftUint8Descriptor.descriptorData_ref())[0].size() != dp8Descriptor96Size)
		{
			ocean_assert(false && "Invalid descriptor dim - this should never happen!");
			return false;
		}

		DP8Descriptor96& descriptor = descriptors_.emplace_back();
		memcpy(descriptor.data(), (*thriftUint8Descriptor.descriptorData_ref())[0].data(), descriptorLengthInBytes());
	}

	return true;
}

DP8DescriptorContainer& DP8DescriptorContainer::operator=(DP8DescriptorContainer&& descriptorContainer)
{
	if (this != &descriptorContainer)
	{
		descriptors_ = std::move(descriptorContainer.descriptors_);
	}

	return *this;
}

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
