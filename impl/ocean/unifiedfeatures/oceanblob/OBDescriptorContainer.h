// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/cv/detector/blob/BlobDescriptor.h"
#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"

#include "ocean/unifiedfeatures/DescriptorContainer.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

/**
 * Definition of the descriptor container that will hold the Ocean ORB descriptors
 * @ingroup unifiedfeaturesoceanblob
 */
struct OCEAN_UNIFIEDFEATURES_OB_EXPORT OBDescriptorContainer final : public DescriptorContainer
{
	public:

		/**
		 * Default constructor
		 */
		inline OBDescriptorContainer();

		/**
		 * Constructor
		 * @param blobFeatures The ORB features that will be stored in this instance
		 */
		inline OBDescriptorContainer(CV::Detector::Blob::BlobFeatures&& blobFeatures);

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
		 * @param blobDescriptors The instance of which all data will be moved to this instance
		 * @return A reference to this instance
		 */
		OBDescriptorContainer& operator=(OBDescriptorContainer&& blobDescriptors);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameOBDescriptorContainer();

		/**
		 * Creates an empty instance of this feature container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& /* ignored */);

		/**
		 * Returns the dimensionality of its descriptor
		 * @return The dim.
		 */
		inline size_t descriptorDimensions() const;

		/**
		 * Returns the blob features stored in this container
		 * @return A reference to the Blob corners
		 */
		inline const CV::Detector::Blob::BlobFeatures& blobFeatures() const;

	private:
		/// The blob features - std::vector of BlobFeature
		CV::Detector::Blob::BlobFeatures blobFeatures_;
};

inline size_t OBDescriptorContainer::size() const
{
	return blobFeatures_.size();
}

inline unsigned int OBDescriptorContainer::numberLevels() const
{
	return 1u;
}

inline unsigned int OBDescriptorContainer::descriptorLengthInBytes() const
{
	return sizeof(CV::Detector::Blob::BlobDescriptor::DescriptorElement) * CV::Detector::Blob::BlobDescriptor::elements();
}

inline size_t OBDescriptorContainer::descriptorDimensions() const
{
	return static_cast<size_t>(CV::Detector::Blob::BlobDescriptor::elements());
}

inline const CV::Detector::Blob::BlobFeatures& OBDescriptorContainer::blobFeatures() const {
	return blobFeatures_;
}

inline unsigned int OBDescriptorContainer::descriptorLengthInElements() const
{
	return static_cast<unsigned int>(CV::Detector::Blob::BlobDescriptor::elements());
}

inline OBDescriptorContainer::OBDescriptorContainer() :
	OBDescriptorContainer(CV::Detector::Blob::BlobFeatures())
{
	// Nothing else to do.
}

inline OBDescriptorContainer::OBDescriptorContainer(CV::Detector::Blob::BlobFeatures&& blobFeatures) :
	DescriptorContainer(nameOceanBlobLibrary(), nameOBDescriptorContainer(), DescriptorCategory::FLOAT_DESCRIPTOR, /* descriptorVersion */ 0u),
	blobFeatures_(blobFeatures)
{
	// Nothing else to do.
}

inline std::string OBDescriptorContainer::nameOBDescriptorContainer()
{
	return "OBDescriptorContainer";
}

inline std::shared_ptr<UnifiedObject> OBDescriptorContainer::create(const std::shared_ptr<Parameters>& /* ignored */)
{
	return std::make_shared<OBDescriptorContainer>();
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
