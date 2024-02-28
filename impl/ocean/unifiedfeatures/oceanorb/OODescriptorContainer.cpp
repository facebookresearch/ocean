// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanorb/OODescriptorContainer.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

bool OODescriptorContainer::toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName_ref() = libraryName();
	unifiedObjectInfo.name_ref() = name();
	unifiedObjectInfo.version_ref() = int32_t(objectVersion());

	Thrift::DescriptorContainer localThriftDescriptorContainer;
	localThriftDescriptorContainer.unifiedObjectInfo_ref() = unifiedObjectInfo;
	localThriftDescriptorContainer.length_ref() = int32_t(descriptorLengthInBytes());
	localThriftDescriptorContainer.levels_ref() = numberLevels();

	std::vector<Thrift::BinaryDescriptor> thriftBinaryDescriptors;
	thriftBinaryDescriptors.reserve(orbFeatures_.size());

	for (const CV::Detector::ORBFeature& orbFeature : orbFeatures_)
	{
		const CV::Detector::ORBDescriptors& orbDescriptors = orbFeature.descriptors();
		const unsigned int numberDescriptorLevels = (unsigned int)(orbDescriptors.size());

		Thrift::BinaryDescriptor& thriftBinaryDescriptor = thriftBinaryDescriptors.emplace_back();

		for (unsigned int i = 0u; i < numberDescriptorLevels; ++i)
		{
			const CV::Detector::ORBDescriptor::DescriptorBitset& bitset = orbDescriptors[i].bitset();
			ocean_assert(bitset.size() == size_t(descriptorLengthInElements()));

			std::string thriftBitsetString(descriptorLengthInBytes(), 0);
			char* data = thriftBitsetString.data();
			ocean_assert(data != nullptr);

			for (size_t bitIndex = 0; bitIndex < bitset.size();)
			{
				ocean_assert(data <= thriftBitsetString.data() + thriftBitsetString.size() /* inclusive end! */);
				*data = 0;

				for (size_t shift = 0; shift < CHAR_BIT; ++shift)
				{
					ocean_assert(bitIndex < bitset.size());
					*data = *data | (bitset[bitIndex] << shift);

					++bitIndex;
				}

				++data;
			}

			thriftBinaryDescriptor.descriptorData_ref()->emplace_back(std::move(thriftBitsetString));

			// Ignore other information of `orbFeature` because those need to be stored in a feature container, not a descriptor container.
		}
	}

	localThriftDescriptorContainer.descriptorUnion_ref()->binaryDescriptors_ref() = std::move(thriftBinaryDescriptors);
	ocean_assert(localThriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::binaryDescriptors);

	thriftDescriptorContainer = std::move(localThriftDescriptorContainer);

	return true;
}

bool OODescriptorContainer::fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> featureContainer)
{
	orbFeatures_.clear();

	if (thriftDescriptorContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftDescriptorContainer.get_unifiedObjectInfo().get_name() != name()
		|| (unsigned int)(thriftDescriptorContainer.get_unifiedObjectInfo().get_version()) != objectVersion()
		|| (unsigned int)(thriftDescriptorContainer.get_length()) != descriptorLengthInBytes()
		|| (unsigned int)(thriftDescriptorContainer.get_levels()) != numberLevels()
		|| thriftDescriptorContainer.get_descriptorUnion().getType() != Thrift::DescriptorUnion::Type::binaryDescriptors)
	{
		ocean_assert(false && "Incompatible descriptor container");
		return false;
	}

	const std::vector<Thrift::BinaryDescriptor>& thriftBinaryDescriptors = thriftDescriptorContainer.get_descriptorUnion().get_binaryDescriptors();

	orbFeatures_.reserve(thriftBinaryDescriptors.size());

	for (const Thrift::BinaryDescriptor& thriftBinaryDescriptor : thriftBinaryDescriptors)
	{
		const std::vector<std::string>& thriftDescriptorData = thriftBinaryDescriptor.get_descriptorData();
		const size_t numberDescriptorLevels = thriftDescriptorData.size();

		if (numberDescriptorLevels == 0 || numberDescriptorLevels > size_t(numberLevels()))
		{
			ocean_assert(false && "Invalid number of descriptor levels - this should never happen!");
			return false;
		}

		CV::Detector::ORBFeature orbFeature;
		CV::Detector::ORBDescriptors& orbDescriptors = orbFeature.descriptors();

		for (size_t i = 0; i < numberDescriptorLevels; ++i)
		{
			CV::Detector::ORBDescriptor::DescriptorBitset bitset;

			if (thriftDescriptorData[i].size() != descriptorLengthInBytes() || bitset.size() == size_t(descriptorLengthInElements()))
			{
				ocean_assert(false && "Mismatch of descriptor sizes - this should never happen!");
				return false;
			}

			const std::string& thriftBitsetString = thriftDescriptorData[i];
			const char* data = thriftBitsetString.data();
			ocean_assert(data != nullptr);

			for (size_t bitIndex = 0; bitIndex < bitset.size();)
			{
				ocean_assert(data < thriftBitsetString.data() + thriftBitsetString.size() /* inclusive end! */);

				for (size_t shift = 0; shift < CHAR_BIT; ++shift)
				{
					ocean_assert(bitIndex < bitset.size());
					bitset[bitIndex] = (*data >> shift) & 0x01u;

					++bitIndex;
				}

				++data;
			}

			orbDescriptors.pushBack(CV::Detector::ORBDescriptor(bitset));
		}

		orbFeatures_.emplace_back(std::move(orbFeature));
	}

	return true;
}

OODescriptorContainer& OODescriptorContainer::operator=(OODescriptorContainer&& orbDescriptors)
{
	if (this != &orbDescriptors)
	{
		orbFeatures_ = std::move(orbDescriptors.orbFeatures_);
	}

	return *this;
}

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
