// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanorb/OceanOrb.h"

#include "ocean/unifiedfeatures/DescriptorContainer.h"

#include "ocean/cv/detector/ORBFeature.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

/**
 * Definition of the descriptor container that will hold the Ocean ORB descriptors
 * @ingroup unifiedfeaturesoceanorb
 */
struct OCEAN_UNIFIEDFEATURES_OO_EXPORT OODescriptorContainer final : public DescriptorContainer
{
	public:

		/**
		 * Default constructor
		 */
		inline OODescriptorContainer();

		/**
		 * Constructor
		 * @param orbFeatures The ORB features that will be stored in this instance
		 */
		inline OODescriptorContainer(CV::Detector::ORBFeatures&& orbFeatures);

		/**
		 * @sa DescriptorContainer::size()
		 */
		inline size_t size() const override;

		/**
		 * Returns the number of levels that the descriptors have that are stored in this instance
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
		 * Move-operator
		 * @param orbDescriptors The instance of which all data will be moved to this instance
		 * @return A reference to this instance
		 */
		OODescriptorContainer& operator=(OODescriptorContainer&& orbDescriptors);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameOODescriptorContainer();

		/**
		 * Creates an empty instance of this feature container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& /* ignored */);
		
	public:

		/// The ORB features
		CV::Detector::ORBFeatures orbFeatures_;
};

inline size_t OODescriptorContainer::size() const
{
	return orbFeatures_.size();
}

inline unsigned int OODescriptorContainer::numberLevels() const
{
	return CV::Detector::ORBDescriptors::capacity();
}

inline unsigned int OODescriptorContainer::descriptorLengthInBytes() const
{
	static_assert(CV::Detector::ORBDescriptor::descriptorLengthInBits % CHAR_BIT == 0u, "The descriptor length must be a multiple of CHAR_BIT");
	return CV::Detector::ORBDescriptor::descriptorLengthInBits / CHAR_BIT;
}

inline unsigned int OODescriptorContainer::descriptorLengthInElements() const
{
	return CV::Detector::ORBDescriptor::descriptorLengthInBits;
}

inline OODescriptorContainer::OODescriptorContainer() :
	OODescriptorContainer(CV::Detector::ORBFeatures())
{
	// Nothing else to do.
}

inline OODescriptorContainer::OODescriptorContainer(CV::Detector::ORBFeatures&& orbFeatures) :
	DescriptorContainer(nameOceanOrbLibrary(), nameOODescriptorContainer(), DescriptorCategory::BINARY_DESCRIPTOR, /* descriptorVersion */ 0u),
	orbFeatures_(orbFeatures)
{
	// Nothing else to do.
}

inline std::string OODescriptorContainer::nameOODescriptorContainer()
{
	return "OODescriptorContainer";
}

inline std::shared_ptr<UnifiedObject> OODescriptorContainer::create(const std::shared_ptr<Parameters>& /* ignored */)
{
	return std::make_shared<OODescriptorContainer>();
}

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
