// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/deepdescriptor/DeepDescriptor.h"

#include "ocean/unifiedfeatures/DescriptorContainer.h"


namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

/**
 * Definition of the descriptor container that will hold the float-based deep descriptors
 * @ingroup unifiedfeaturesdeepdescriptor
 */
struct OCEAN_UNIFIEDFEATURES_DD_EXPORT DDFloatDescriptorContainer final : public DescriptorContainer
{
	public:

		/**
		 * Default constructor which creates an empty container
		 */
		inline DDFloatDescriptorContainer();

		/**
		 * Constructor
		 * @param deepPatchFloatDescriptors The descriptors that will be stored in this container
		 */
		 inline DDFloatDescriptorContainer(DeepPatchFloatDescriptors&& deepPatchFloatDescriptors);

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
		DDFloatDescriptorContainer& operator=(DDFloatDescriptorContainer&& descriptor);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameDDFloatDescriptorContainer();

		/**
		 * Creates an instance of this container
		 * @param parameters Optional parameters to initialize this container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	public:

		/// The descriptors that are stored in this container
		DeepPatchFloatDescriptors deepPatchFloatDescriptors_;
};

inline size_t DDFloatDescriptorContainer::size() const
{
	return deepPatchFloatDescriptors_.size();
}

inline DDFloatDescriptorContainer::DDFloatDescriptorContainer() :
	DDFloatDescriptorContainer(DeepPatchFloatDescriptors())
{
	// Nothing else to do.
}

inline DDFloatDescriptorContainer::DDFloatDescriptorContainer(DeepPatchFloatDescriptors&& deepPatchFloatDescriptors) :
	DescriptorContainer(nameDeepDescriptorLibrary(), nameDDFloatDescriptorContainer(), DescriptorCategory::FLOAT_DESCRIPTOR, /* descriptorVersion */ 0u),
	deepPatchFloatDescriptors_(std::move(deepPatchFloatDescriptors))
{
	// Nothing else to do.
}

inline std::string DDFloatDescriptorContainer::nameDDFloatDescriptorContainer()
{
	return "DDFloatDescriptorContainer";
}

inline std::shared_ptr<UnifiedObject> DDFloatDescriptorContainer::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<DDFloatDescriptorContainer>();
}

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
