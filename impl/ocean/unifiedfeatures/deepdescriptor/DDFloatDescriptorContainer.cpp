// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deepdescriptor/DDFloatDescriptorContainer.h"
#include "ocean/unifiedfeatures/deepdescriptor/DeepDescriptor.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

#include <climits>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

unsigned int DDFloatDescriptorContainer::numberLevels() const
{
	return 1u;
}

unsigned int DDFloatDescriptorContainer::descriptorLengthInBytes() const
{
	return deepPatchFloatDescriptorSizeInElements * sizeof(float);
}

unsigned int DDFloatDescriptorContainer::descriptorLengthInElements() const
{
	return descriptorLengthInBytes() * (unsigned int)(CHAR_BIT);
}

bool DDFloatDescriptorContainer::toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.set_libraryName(libraryName());
	unifiedObjectInfo.set_name(name());
	unifiedObjectInfo.set_version(int32_t(objectVersion()));

	Thrift::DescriptorContainer localThriftDescriptorContainer;
	localThriftDescriptorContainer.set_unifiedObjectInfo(unifiedObjectInfo);
	localThriftDescriptorContainer.set_length(int32_t(descriptorLengthInBytes()));
	localThriftDescriptorContainer.set_levels(int32_t(numberLevels()));

	std::vector<Thrift::FloatDescriptor> thriftFloatDescriptors;
	thriftFloatDescriptors.reserve(deepPatchFloatDescriptors_.size());

	for (const DeepPatchFloatDescriptor& deepPatchFloatDescriptor : deepPatchFloatDescriptors_)
	{
		Thrift::FloatDescriptor& thriftFloatDescriptor = thriftFloatDescriptors.emplace_back();
		thriftFloatDescriptor.descriptorData_ref()->emplace_back(deepPatchFloatDescriptor.begin(), deepPatchFloatDescriptor.end());
	}

	localThriftDescriptorContainer.descriptorUnion_ref()->set_floatDescriptors(std::move(thriftFloatDescriptors));
	ocean_assert(localThriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::floatDescriptors);

	thriftDescriptorContainer = std::move(localThriftDescriptorContainer);

	return true;
}

bool DDFloatDescriptorContainer::fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> /* featureContainer */)
{
	deepPatchFloatDescriptors_.clear();

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

	deepPatchFloatDescriptors_.reserve(thriftFloatDescriptors.size());

	for (const Thrift::FloatDescriptor& thriftFloatDescriptor : thriftFloatDescriptors)
	{
		const size_t numberDescriptorLevels = thriftFloatDescriptor.descriptorData_ref()->size();

		if (numberDescriptorLevels == 0 || numberDescriptorLevels > size_t(numberLevels()))
		{
			ocean_assert(false && "Invalid number of descriptor levels - this should never happen!");
			return false;
		}

		if ((*thriftFloatDescriptor.descriptorData_ref())[0].size() != deepPatchFloatDescriptorSizeInElements)
		{
			ocean_assert(false && "Invalid descriptor dim - this should never happen!");
			return false;
		}

		DeepPatchFloatDescriptor& deepPatchFloatDescriptor = deepPatchFloatDescriptors_.emplace_back();
		memcpy(deepPatchFloatDescriptor.data(), (*thriftFloatDescriptor.descriptorData_ref())[0].data(), descriptorLengthInBytes());
	}

	return true;
}

DDFloatDescriptorContainer& DDFloatDescriptorContainer::operator=(DDFloatDescriptorContainer&& descriptorContainer)
{
	if (this != &descriptorContainer)
	{
		deepPatchFloatDescriptors_ = std::move(descriptorContainer.deepPatchFloatDescriptors_);
	}

	return *this;
}

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
