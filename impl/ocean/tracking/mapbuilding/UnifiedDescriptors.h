/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTORS_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTORS_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements the base class for all unified descriptor buffers stored in a continuous memory like a vector.
 * @see UnifiedDescriptorMap
 * @ingroup trackingmapbuilding
 */
class UnifiedDescriptors : public UnifiedDescriptor
{
	public:

		/**
		 * Disposes this object.
		 */
		virtual ~UnifiedDescriptors() = default;

		/**
		 * Returns the number of descriptors this object holds.
		 * @return The number of descriptors, with range [0, infinity)
		 */
		virtual size_t numberDescriptors() const = 0;

		/**
		 * Returns whether this object holds at least one descriptor.
		 * @return True, if so
		 */
		virtual bool isValid() const = 0;

	protected:

		/**
		 * Creates a new descriptors object.
		 * @param descriptorType The type of the descriptors
		 */
		explicit inline UnifiedDescriptors(const DescriptorType descriptorType);
};

/**
 * Definition of a shared pointer holding a UnifiedDescriptors object.
 * @see UnifiedDescriptors
 * @ingroup trackingmapbuilding
 */
using SharedUnifiedDescriptors = std::shared_ptr<UnifiedDescriptors>;

/**
 * This class implements a type-based container for descriptors stored in a continuous memory like a vector.
 * @tparam TDescriptor The data type of each individual descriptor this object will store
 */
template <typename TDescriptor>
class UnifiedDescriptorsT final : public UnifiedDescriptors
{
	public:

		/// The data type of the descriptor.
		using Descriptor = TDescriptor;

	public:

		/**
		 * Default constructor creating an object without any descriptors.
		 */
		inline UnifiedDescriptorsT();

		/**
		 * Creates a new object with new descriptors.
		 * @param descriptors The descriptors which will be moved into this object
		 */
		explicit UnifiedDescriptorsT(std::vector<TDescriptor>&& descriptors);

		/**
		 * Returns the pointer to the memory holding all descriptors of this object.
		 * @return The memory with all descriptors if 'isValid() == true'
		 */
		inline const TDescriptor* descriptors() const;

		/**
		 * Returns the number of descriptors this object holds.
		 * @see UnifiedDescriptors::numberDescriptors().
		 */
		size_t numberDescriptors() const override;

		/**
		 * Returns whether this object holds at least one descriptor.
		 * @see UnifiedDescriptors::isValid().
		 */
		bool isValid() const override;

	protected:

		/// The object's descriptors.
		std::vector<TDescriptor> descriptors_;
};

using UnifiedDescriptorsFreakMultiLevelSingleViewDescriptor256 = UnifiedDescriptorsT<DescriptorHandling::FreakMultiDescriptor256>;

using UnifiedDescriptorsFreakMultiLevelMultiViewDescriptor256 = UnifiedDescriptorsT<DescriptorHandling::FreakMultiDescriptors256>;

template <unsigned int tNumberBits>
using UnifiedDescriptorsBinarySingleLevelSingleView = UnifiedDescriptorsT<UnifiedDescriptor::BinaryDescriptor<tNumberBits>>;

template <unsigned int tNumberElements>
using UnifiedDescriptorsFloatSingleLevelSingleView = UnifiedDescriptorsT<UnifiedDescriptor::FloatDescriptor<tNumberElements>>;

template <unsigned int tNumberElements>
using UnifiedDescriptorsFloatSingleLevelMultiView = UnifiedDescriptorsT<UnifiedDescriptor::FloatDescriptors<tNumberElements>>;

inline UnifiedDescriptors::UnifiedDescriptors(const UnifiedDescriptor::DescriptorType descriptorType) :
	UnifiedDescriptor(descriptorType)
{
	// nothing to do here
}

template <typename TDescriptor>
UnifiedDescriptorsT<TDescriptor>::UnifiedDescriptorsT() :
	UnifiedDescriptors(DescriptorTyper<TDescriptor>::type())
{
	ocean_assert(descriptorType() != DescriptorType::DT_INVALID);
}

template <typename TDescriptor>
UnifiedDescriptorsT<TDescriptor>::UnifiedDescriptorsT(std::vector<TDescriptor>&& descriptors) :
	UnifiedDescriptors(DescriptorTyper<TDescriptor>::type()),
	descriptors_(std::move(descriptors))
{
	ocean_assert(descriptorType() != DescriptorType::DT_INVALID);
}

template <typename TDescriptor>
inline const TDescriptor* UnifiedDescriptorsT<TDescriptor>::descriptors() const
{
	return descriptors_.data();
}

template <typename TDescriptor>
size_t UnifiedDescriptorsT<TDescriptor>::numberDescriptors() const
{
	return descriptors_.size();
}

template <typename TDescriptor>
bool UnifiedDescriptorsT<TDescriptor>::isValid() const
{
	return !descriptors_.empty();
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTORS_H
