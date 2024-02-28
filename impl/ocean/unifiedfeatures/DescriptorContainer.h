// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/UnifiedObject.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace Thrift
{

/// Forward declaration
class DescriptorContainer;

} // namespace Thrift

/// Forward declaration
class FeatureContainer;

/**
 * Definition of a generic container for descriptors
 * @ingroup unifiedfeatures
 */
class DescriptorContainer : public UnifiedObject
{
	public:

		/**
		 * Default destructor
		 */
		virtual ~DescriptorContainer() = default;

		/**
		 * Returns the type of the descriptor
		 * @return The type
		 */
		inline DescriptorCategory descriptorCategory() const;

		/**
		 * Returns the number of descriptors stored in this instance
		 * @return The number of descriptors
		 */
		virtual size_t size() const = 0;

		/**
		 * Returns the number of levels that the descriptors have that are stored in this instance
		 * @return The number of levels, range: [1, infinity)
		 */
		virtual unsigned int numberLevels() const = 0;

		/**
		 * Returns the length in bytes of the descriptors that are stored in this instance
		 * @return The descriptor length in bytes
		 */
		virtual unsigned int descriptorLengthInBytes() const = 0;

		/**
		 * Returns the length in elements of the descriptors that are stored in this instance
		 * @return The descriptor length in elements
		 */
		virtual unsigned int descriptorLengthInElements() const = 0;

		/**
		 * Converts the contents of this container to Thrift data structures
		 * @param thriftDescriptorContainer The Thrift data structure that will be filled
		 * @return True on success, otherwise false
		 */
		virtual bool toThrift(Thrift::DescriptorContainer& thriftDescriptorContainer) const = 0;

		/**
		 * Converts a Thrift data structure back to the hidden data structures of this container
		 * This function optionally takes a pointer to a feature container. This is meant only for special cases when an algorithm does not differentiate between features and descriptors. This allows the feature and the descriptor container to share memory.
		 * @param thriftDescriptorContainer The Thrift data structure that will be used to fill this container
		 * @param featureContainer Optional pointer to a feature container; cf. note on special cases above
		 * @return True on success, otherwise false
		 */
		virtual bool fromThrift(const Thrift::DescriptorContainer& thriftDescriptorContainer, const std::shared_ptr<FeatureContainer> featureContainer = nullptr) = 0;

	protected:

		/**
		 * Constructor
		 * @param libraryName The name of the library that this container belongs to
		 * @param descriptorContainerName The name of the container
		 * @param descriptorCategory The category of this descriptor
		 * @param descriptorVersion Optional version number of the implementation of the descriptor
		 */
		inline DescriptorContainer(const std::string& libraryName, const std::string& descriptorContainerName, const DescriptorCategory descriptorCategory, const unsigned int descriptorVersion = 0u);

	protected:

		/// The type of the descriptor
		const DescriptorCategory descriptorCategory_;
};

inline DescriptorContainer::DescriptorContainer(const std::string& libraryName, const std::string& descriptorContainerName, const DescriptorCategory descriptorCategory, const unsigned int descriptorVersion) :
	UnifiedObject(libraryName, descriptorContainerName, descriptorVersion),
	descriptorCategory_(descriptorCategory)
{
	// Nothing else to do.
}

inline DescriptorCategory DescriptorContainer::descriptorCategory() const
{
	return descriptorCategory_;
}

} // namespace UnifiedFeatures

} // namespace Ocean
