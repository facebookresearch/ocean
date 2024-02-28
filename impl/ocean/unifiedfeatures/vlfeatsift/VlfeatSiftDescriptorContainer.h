// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"

#include "ocean/unifiedfeatures/Descriptor.h"
#include "ocean/unifiedfeatures/DescriptorContainer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * Definition of the descriptor container that will hold the VLFeat SIFT descriptors
 * @ingroup unifiedfeaturesvlfeatsift
 */
struct OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT VlfeatSiftDescriptorContainer final : public DescriptorContainer
{
	public:

		/**
		 * Definition of the SIFT descriptor with one level.
		 */
		typedef FloatDescriptor<kSiftDescriptorSizeInElements, 1> SiftDescriptor;

		/**
		 * Definition of a vector holding SIFT descriptors.
		 */
		typedef std::vector<SiftDescriptor> SiftDescriptors;

		/**
		 * Default constructor
		 */
		inline VlfeatSiftDescriptorContainer();

		/**
		 * Constructor
		 * @param features The SIFT features that will be stored in this instance
		 */
		inline VlfeatSiftDescriptorContainer(SiftDescriptors&& features);

		/**
		 * @sa DescriptorContainer::size()
		 */
		inline size_t size() const override;

		/**
		 * @sa DescriptorContainer::numberLevels()
		 */
		inline unsigned int numberLevels() const override;

		/**
		 * Returns the length in bytes of the descriptors that are stored in this instance
		 * @sa DescriptorContainer::descriptorLengthInBytes()
		 */
		inline unsigned int descriptorLengthInBytes() const override;

		/**
		 * Returns the length in elements of the descriptors that are stored in this instance
		 * @sa DescriptorContainer::descriptorLengthInElements()
		 */
		inline unsigned int descriptorLengthInElements() const override;

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
		 * Returns the stored Sift descriptors.
		 * @return The descriptors
		 */
		inline const SiftDescriptors& siftDescriptors() const;

		/**
		 * Move-operator
		 * @param descriptors The instance of which all data will be moved to this instance
		 * @return A reference to this instance
		 */
		VlfeatSiftDescriptorContainer& operator=(VlfeatSiftDescriptorContainer&& descriptors);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameVlfeatSiftDescriptorContainer();

		/**
		 * Creates an empty instance of this feature container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& /* ignored */);

	public:

		/// The SIFT features
		SiftDescriptors descriptors_;
};

inline VlfeatSiftDescriptorContainer::VlfeatSiftDescriptorContainer() :
	VlfeatSiftDescriptorContainer(SiftDescriptors())
{
	// Nothing else to do.
}

inline VlfeatSiftDescriptorContainer::VlfeatSiftDescriptorContainer(SiftDescriptors&& descriptors) :
	DescriptorContainer(nameVlfeatSiftLibrary(), nameVlfeatSiftDescriptorContainer(), DescriptorCategory::FLOAT_DESCRIPTOR, /* descriptorVersion */ 0u),
	descriptors_(std::move(descriptors))
{
	// Nothing else to do.
}

inline size_t VlfeatSiftDescriptorContainer::size() const
{
	return descriptors_.size();
}

inline unsigned int VlfeatSiftDescriptorContainer::numberLevels() const
{
	return 1;
}

inline unsigned int VlfeatSiftDescriptorContainer::descriptorLengthInBytes() const
{
	return 128 * sizeof(float);
}

inline unsigned int VlfeatSiftDescriptorContainer::descriptorLengthInElements() const
{
	return 128;
}

inline const VlfeatSiftDescriptorContainer::SiftDescriptors& VlfeatSiftDescriptorContainer::siftDescriptors() const
{
	return descriptors_;
}

inline std::string VlfeatSiftDescriptorContainer::nameVlfeatSiftDescriptorContainer()
{
	return "VlfeatSiftDescriptorContainer";
}

inline std::shared_ptr<UnifiedObject> VlfeatSiftDescriptorContainer::create(const std::shared_ptr<Parameters>& /* ignored */)
{
	return std::make_shared<VlfeatSiftDescriptorContainer>();
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
