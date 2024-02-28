// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/deeppatchuint8/DeepPatchUint8.h"

#include "ocean/unifiedfeatures/DescriptorContainer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

/**
 * Definition of the descriptor container that will hold the descriptors of DeepPatchUint8
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
struct OCEAN_UNIFIEDFEATURES_DP8_EXPORT DP8DescriptorContainer final : public DescriptorContainer
{
	public:

		/**
		 * Default constructor which creates an empty container
		 */
		inline DP8DescriptorContainer();

		/**
		 * Constructor
		 * @param descriptors The descriptors that will be stored in this container
		 */
		 inline DP8DescriptorContainer(DP8Descriptors96&& descriptors);

		/**
		 * Returns the number of descriptors stored in this instance
		 * @sa DescriptorContainer::size()
		 */
		inline size_t size() const override;

		/**
		 * Returns the number of levels that the descriptors have that are stored in this instance
		 * @sa DescriptorContainer::size()
		 */
		unsigned int numberLevels() const override;

		/**
		 * Returns the length in bytes of the descriptors that are stored in this instance
		 * @sa DescriptorContainer::size()
		 */
		unsigned int descriptorLengthInBytes() const override;

		/**
		 * Returns the length in elements of the descriptors that are stored in this instance
		 * @sa DescriptorContainer::size()
		 */
		unsigned int descriptorLengthInElements() const override;

		/**
		 * Converts the contents of this container to Thrift data structures
		 * @sa DescriptorContainer::size()
		 */
		bool toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const override;

		/**
		 * Converts a Thrift data structure back to the hidden data structures of this container
		 * @sa DescriptorContainer::size()
		 */
		bool fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> featureContainer = nullptr) override;

		/**
		 * Move-operator
		 * @param descriptor The instance of which all data will be moved to this instance
		 * @return A reference to this instance
		 */
		DP8DescriptorContainer& operator=(DP8DescriptorContainer&& descriptor);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameDP8DescriptorContainer();

		/**
		 * Creates an instance of this container
		 * @param parameters Optional parameters to initialize this container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	public:

		/// The descriptors that are stored in this container
		DP8Descriptors96 descriptors_;
};

inline size_t DP8DescriptorContainer::size() const
{
	return descriptors_.size();
}

inline DP8DescriptorContainer::DP8DescriptorContainer() :
	DP8DescriptorContainer(DP8Descriptors96())
{
	// Nothing else to do.
}

inline DP8DescriptorContainer::DP8DescriptorContainer(DP8Descriptors96&& descriptors) :
	DescriptorContainer(nameDeepPatchUint8Library(), nameDP8DescriptorContainer(), DescriptorCategory::UINT8_DESCRIPTOR, /* descriptorVersion */ 0u),
	descriptors_(std::move(descriptors))
{
	// Nothing else to do.
}

inline std::string DP8DescriptorContainer::nameDP8DescriptorContainer()
{
	return "DP8DescriptorContainer";
}

inline std::shared_ptr<UnifiedObject> DP8DescriptorContainer::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<DP8DescriptorContainer>();
}

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
