// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanfreak/OceanFreak.h"

#include "ocean/unifiedfeatures/DescriptorContainer.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

#include <arvr/libraries/thrift_if/ufi/gen-cpp2/UnifiedFeatures_types.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

/// Forward declaration
template <size_t tDescriptorSizeBytes>
class OFDescriptorContainerT;

/// Typedef for the descriptor container that holds the 32 byte long FREAK descriptors
typedef OFDescriptorContainerT<32> OFDescriptorContainer32;

/// Typedef for the descriptor container that holds the 64 byte long FREAK descriptors
typedef OFDescriptorContainerT<64> OFDescriptorContainer64;

/**
 * Definition of the descriptor container that will hold the Ocean FREAK descriptors
 * @ingroup unifiedfeaturesoceanfreak
 */

template <size_t tDescriptorSizeBytes>
class OCEAN_UNIFIEDFEATURES_OF_EXPORT OFDescriptorContainerT final : public DescriptorContainer
{
	public:

		/// Short-hand definition of the currently selected type of Freak descriptor
		typedef CV::Detector::FREAKDescriptorT<tDescriptorSizeBytes> FreakDescriptor;

		/// Short-hand definition of a vector of the currently selected type of Freak descriptor
		typedef std::vector<FreakDescriptor> FreakDescriptors;

	public:

		/**
		 * Default constructor
		 */
		OFDescriptorContainerT();

		/**
		 * Constructor
		 * @param freakDescriptors The FREAK descriptors that will be stored in this instance
		 */
		OFDescriptorContainerT(FreakDescriptors&& freakDescriptors);

		/**
		 * @sa DescriptorContainer::size()
		 */
		size_t size() const override;

		/**
		 * Returns the number of levels that the descriptors have that are stored in this instance
		 * @sa DescriptorContainer::numberLevels()
		 */
		unsigned int numberLevels() const override;

		/**
		 * Returns the length in bytes of the descriptors that are stored in this instance
		 * @sa DescriptorContainer::descriptorLengthInBytes()
		 */
		unsigned int descriptorLengthInBytes() const override;

		/**
		 * Returns the length in elements of the descriptors that are stored in this instance
		 * @sa DescriptorContainer::descriptorLengthInElements()
		 */
		unsigned int descriptorLengthInElements() const override;

		/**
		 * Converts the contents of this container to Thrift data structures
		 * @sa DescriptorContainer::toThrift()
		 */
		bool toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const override;

		/**
		 * Converts a Thrift data structure back to the hidden data structures of this container
		 * @sa DescriptorContainer::fromThrift()
		 */
		bool fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> featureContainer = nullptr) override;

		/**
		 * Returns the stored FREAK descriptors
		 * @return The descriptors
		 */
		inline const FreakDescriptors& freakDescriptors() const;

		/**
		 * Move-operator
		 * @param freakDescriptors The FREAK descriptors that will be moved to this instance
		 * @return A reference to this instance
		 */
		OFDescriptorContainerT& operator=(OFDescriptorContainerT&& freakDescriptors);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static std::string nameOFDescriptorContainer();

		/**
		 * Creates an instance of this descriptor container
		 * @return The pointer to new instance
		 */
		static std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	public:

		/// The FREAK descriptors
		FreakDescriptors freakDescriptors_;
};

template <size_t tDescriptorSizeBytes>
size_t OFDescriptorContainerT<tDescriptorSizeBytes>::size() const
{
	return freakDescriptors_.size();
}

template <size_t tDescriptorSizeBytes>
unsigned int OFDescriptorContainerT<tDescriptorSizeBytes>::numberLevels() const
{
	constexpr unsigned int levels = 3u;
	static_assert(std::is_same<typename FreakDescriptor::MultilevelDescriptorData, std::array<std::array<uint8_t, tDescriptorSizeBytes>, size_t(levels)>>::value, "Unexpected descriptor type");

	return levels;
}

template <size_t tDescriptorSizeBytes>
unsigned int OFDescriptorContainerT<tDescriptorSizeBytes>::descriptorLengthInBytes() const
{
	return tDescriptorSizeBytes;
}

template <size_t tDescriptorSizeBytes>
unsigned int OFDescriptorContainerT<tDescriptorSizeBytes>::descriptorLengthInElements() const
{
	return tDescriptorSizeBytes * CHAR_BIT;
}

template <size_t tDescriptorSizeBytes>
inline const typename OFDescriptorContainerT<tDescriptorSizeBytes>::FreakDescriptors& OFDescriptorContainerT<tDescriptorSizeBytes>::freakDescriptors() const
{
	return freakDescriptors_;
}

template <size_t tDescriptorSizeBytes>
OFDescriptorContainerT<tDescriptorSizeBytes>::OFDescriptorContainerT() :
	DescriptorContainer(nameOceanFreakLibrary(), nameOFDescriptorContainer(), DescriptorCategory::BINARY_DESCRIPTOR, /* descriptorVersion */ 0u)
{
	static_assert(tDescriptorSizeBytes == 32 || tDescriptorSizeBytes == 64, "This descriptor must be 32 or 64 bytes long");
}

template <size_t tDescriptorSizeBytes>
OFDescriptorContainerT<tDescriptorSizeBytes>::OFDescriptorContainerT(FreakDescriptors&& freakDescriptors) :
	DescriptorContainer(nameOceanFreakLibrary(), nameOFDescriptorContainer(), DescriptorCategory::BINARY_DESCRIPTOR, /* descriptorVersion */ 0u),
	freakDescriptors_(freakDescriptors)
{
	static_assert(tDescriptorSizeBytes == 32 || tDescriptorSizeBytes == 64, "This descriptor must be 32 or 64 bytes long");
}

template <size_t tDescriptorSizeBytes>
std::string OFDescriptorContainerT<tDescriptorSizeBytes>::nameOFDescriptorContainer()
{
	return "OFDescriptorContainer" + String::toAString(tDescriptorSizeBytes);
}

template <size_t tDescriptorSizeBytes>
std::shared_ptr<UnifiedObject> OFDescriptorContainerT<tDescriptorSizeBytes>::create(const std::shared_ptr<Parameters>& parameters)
{
	ocean_assert_and_suppress_unused(parameters == nullptr && "This container does not have any parameters", parameters);

	return std::make_shared<OFDescriptorContainerT<tDescriptorSizeBytes>>();
}


template <size_t tDescriptorSizeBytes>
bool OFDescriptorContainerT<tDescriptorSizeBytes>::toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const
{
	Thrift::UnifiedObject unifiedObjectInfo;
	unifiedObjectInfo.libraryName() = libraryName();
	unifiedObjectInfo.name() = name();
	unifiedObjectInfo.version() = int32_t(objectVersion());

	Thrift::DescriptorContainer localThriftDescriptorContainer;
	localThriftDescriptorContainer.unifiedObjectInfo() = unifiedObjectInfo;
	localThriftDescriptorContainer.length() = int32_t(tDescriptorSizeBytes);

	static_assert(std::is_same<typename FreakDescriptor::MultilevelDescriptorData, std::array<std::array<uint8_t, tDescriptorSizeBytes>, /* number of levels */ 3>>::value, "Unexpected descriptor type");
	localThriftDescriptorContainer.levels() = 3;

	std::vector<Thrift::BinaryDescriptor> thriftBinaryDescriptors;
	thriftBinaryDescriptors.reserve(freakDescriptors_.size());

	for (const FreakDescriptor& freakDescriptor : freakDescriptors_)
	{
		Thrift::BinaryDescriptor& thriftBinaryDescriptor = thriftBinaryDescriptors.emplace_back();

		ocean_assert(freakDescriptor.orientation() >= -Numeric::pi() || freakDescriptor.orientation() <= Numeric::pi());
		thriftBinaryDescriptor.orientation() = float(freakDescriptor.orientation());

		const unsigned int numberDescriptorLevels = freakDescriptor.descriptorLevels();
		const typename FreakDescriptor::MultilevelDescriptorData& multilevelDescriptorData = freakDescriptor.data();
		ocean_assert(size_t(numberDescriptorLevels) <= multilevelDescriptorData.size());

		for (unsigned int i = 0u; i < numberDescriptorLevels; ++i)
		{
			ocean_assert(multilevelDescriptorData[i].size() == tDescriptorSizeBytes);

			thriftBinaryDescriptor.descriptorData()->emplace_back(multilevelDescriptorData[i].data(), multilevelDescriptorData[i].data() + multilevelDescriptorData[i].size());
		}
	}

	localThriftDescriptorContainer.descriptorUnion()->binaryDescriptors_ref() = std::move(thriftBinaryDescriptors);
	ocean_assert(localThriftDescriptorContainer.descriptorUnion_ref()->getType() == Thrift::DescriptorUnion::Type::binaryDescriptors);

	thriftDescriptorContainer = std::move(localThriftDescriptorContainer);

	return true;
}

template <size_t tDescriptorSizeBytes>
bool OFDescriptorContainerT<tDescriptorSizeBytes>::fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> featureContainer)
{
	freakDescriptors_.clear();

	if (thriftDescriptorContainer.get_unifiedObjectInfo().get_libraryName() != libraryName()
		|| thriftDescriptorContainer.get_unifiedObjectInfo().get_name() != name()
		|| (unsigned int)(thriftDescriptorContainer.get_unifiedObjectInfo().get_version()) != objectVersion()
		|| size_t(thriftDescriptorContainer.get_length()) != tDescriptorSizeBytes
		|| (unsigned int)(thriftDescriptorContainer.get_levels()) != numberLevels()
		|| thriftDescriptorContainer.get_descriptorUnion().getType() != Thrift::DescriptorUnion::Type::binaryDescriptors)
	{
		ocean_assert(false && "Incompatible descriptor container");
		return false;
	}

	const std::vector<Thrift::BinaryDescriptor>& thriftBinaryDescriptors = thriftDescriptorContainer.get_descriptorUnion().get_binaryDescriptors();

	freakDescriptors_.reserve(thriftBinaryDescriptors.size());

	for (const Thrift::BinaryDescriptor& thriftBinaryDescriptor : thriftBinaryDescriptors)
	{
		const unsigned int numberDescriptorLevels = (unsigned int)(thriftBinaryDescriptor.get_descriptorData().size());

		if (numberDescriptorLevels == 0u || numberDescriptorLevels > numberLevels())
		{
			ocean_assert(false && "Invalid number of descriptor levels - this should never happen!");
			return false;
		}

		const float* orientation = thriftBinaryDescriptor.get_orientation();

		if (orientation == nullptr)
		{
			ocean_assert(false && "Invalid Thrift data! This should never happen!");
			return false;
		}

		if (*orientation < -NumericF::pi() || *orientation > NumericF::pi())
		{
			ocean_assert(false && "The orientation value must be in the range [-pi, pi] - this should never happen!");
			return false;
		}

		typename FreakDescriptor::MultilevelDescriptorData multilevelDescriptorData;
		for (unsigned int i = 0u; i < numberDescriptorLevels; ++i)
		{
			if (thriftBinaryDescriptor.get_descriptorData()[i].size() != multilevelDescriptorData[i].size())
			{
				ocean_assert(false && "Mismatch of descriptor sizes - this should never happen!");
				return false;
			}

			memcpy((void*)multilevelDescriptorData[i].data(), (void*)thriftBinaryDescriptor.get_descriptorData()[i].data(), tDescriptorSizeBytes);
		}

		freakDescriptors_.emplace_back(std::move(multilevelDescriptorData), numberDescriptorLevels, *orientation);
	}

	return true;
}

template <size_t tDescriptorSizeBytes>
OFDescriptorContainerT<tDescriptorSizeBytes>& OFDescriptorContainerT<tDescriptorSizeBytes>::operator=(OFDescriptorContainerT&& freakDescriptors)
{
	if (this != &freakDescriptors)
	{
		freakDescriptors_ = std::move(freakDescriptors.freakDescriptors_);
	}

	return *this;
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
