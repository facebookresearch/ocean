/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTOR_MAP_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTOR_MAP_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

// Forward declaration.
class UnifiedDescriptorMap;

/**
 * Definition of a shared pointer holding a UnifiedDescriptorMap object.
 * @ingroup trackingmapbuilding
 */
using SharedUnifiedDescriptorMap = std::shared_ptr<UnifiedDescriptorMap>;

/**
 * This class implements the base class for all unified descriptor maps in which ids are mapped to descriptors.
 * @see UnifiedDescriptors.
 * @ingroup trackingmapbuilding
 */
class UnifiedDescriptorMap : public UnifiedDescriptor
{
	public:

		/**
		 * Disposes the object.
		 */
		virtual ~UnifiedDescriptorMap() = default;

		/**
		 * Returns the number of object points.
		 * @return The map's number of object points
		 */
		virtual size_t numberObjectPoints() const = 0;

		/**
		 * Returns the ids of all object points.
		 * @return The object ids of all 3D object points this map holds
		 */
		virtual Indices32 objectPointIds() const = 0;

		/**
		 * Returns the number of descriptors associated with a specified object point id.
		 * @param objectPointId The id of the object point
		 * @return The number of descriptors
		 */
		virtual size_t numberDescriptors(const Index32& objectPointId) const = 0;

		/**
		 * Removes the descriptors for a specified object point id.
		 * @param objectPointId The id of the object point to be removed
		 * @return True, if the map contained the specified object point and if the descriptors were removed; False, if the map does not contain the specified object point
		 */
		virtual bool removeDescriptors(const Index32& objectPointId) = 0;

		/**
		 * Returns a clone of this map.
		 * @return The map's clone
		 */
		virtual std::unique_ptr<UnifiedDescriptorMap> clone() const = 0;

	protected:

		/**
		 * Creates a new descriptors object.
		 * @param descriptorType The type of the descriptors
		 */
		explicit inline UnifiedDescriptorMap(const DescriptorType descriptorType);
};

/**
 * This class implements the descriptor map for arbitrary descriptors.
 * @tparam TDescriptor The data type of the descriptor to be used (can also be a multi-view descriptor - e.g., a vector with descriptors)
 * @ingroup trackingmapbuilding
 */
template <typename TDescriptor>
class UnifiedDescriptorMapT : public UnifiedDescriptorMap
{
	public:

		/**
		 * Definition of the descriptors this map is using.
		 */
		using Descriptor = TDescriptor;

		/**
		 * Definition of an unordered map mapping object point ids to multi-view float descriptors.
		 */
		using DescriptorMap = std::unordered_map<Index32, Descriptor>;

	public:

		/**
		 * Default constructor.
		 */
		inline UnifiedDescriptorMapT();

		/**
		 * Creates a new descriptor map object.
		 * @param descriptorMap The actual internal descriptor map to be moved into the new object
		 */
		inline explicit UnifiedDescriptorMapT(DescriptorMap&& descriptorMap);

		/**
		 * Returns the number of object points.
		 * @see UnifiedDescriptorMap::numberObjectPoints().
		 */
		size_t numberObjectPoints() const override;

		/**
		 * Returns the ids of all object points.
		 * @return The object ids of all 3D object points this map holds
		 */
		Indices32 objectPointIds() const override;

		/**
		 * Returns the number of descriptors associated with a specified object point id.
		 * @see UnifiedDescriptorMap::numberDescriptors().
		 */
		size_t numberDescriptors(const Index32& objectPointId) const override;

		/**
		 * Removes the descriptors for a specified object point id.
		 * @see UnifiedDescriptorMap::removeDescriptors().
		 */
		bool removeDescriptors(const Index32& objectPointId) override;

		/**
		 * Returns a clone of this map.
		 * @see UnifiedDescriptorMap::clone().
		 */
		std::unique_ptr<UnifiedDescriptorMap> clone() const override;

		/**
		 * Returns the actual underlying descriptor map.
		 * @return Internal descriptor map
		 */
		inline const DescriptorMap& descriptorMap() const;

		/**
		 * Returns the actual underlying descriptor map.
		 * @return Internal descriptor map
		 */
		inline DescriptorMap& descriptorMap();

	protected:

		/// The internal descriptor map.
		DescriptorMap descriptorMap_;
};

/**
 * Definition of a UnifiedDescriptorMapT object for FREAK multi-view, multi-level descriptors with 256 bits.
 * @ingroup trackingmapbuilding
 */
using UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256 = UnifiedDescriptorMapT<UnifiedDescriptor::FreakMultiDescriptors256>;

/**
 * Definition of a UnifiedDescriptorMapT object for float multi-view, single-level descriptors.
 * @tparam tElements The number of float elements each float descriptor has, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tElements>
using UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor = UnifiedDescriptorMapT<UnifiedDescriptor::FloatDescriptors<tElements>>;

inline UnifiedDescriptorMap::UnifiedDescriptorMap(const DescriptorType descriptorType) :
	UnifiedDescriptor(descriptorType)
{
	// nothing to do here
}

template <typename TDescriptor>
inline UnifiedDescriptorMapT<TDescriptor>::UnifiedDescriptorMapT() :
	UnifiedDescriptorMap(DescriptorTyper<TDescriptor>::type())
{
	ocean_assert(descriptorType() != DescriptorType::DT_INVALID);
}

template <typename TDescriptor>
inline UnifiedDescriptorMapT<TDescriptor>::UnifiedDescriptorMapT(DescriptorMap&& descriptorMap) :
	UnifiedDescriptorMap(DescriptorTyper<TDescriptor>::type()),
	descriptorMap_(std::move(descriptorMap))
{
	ocean_assert(descriptorType() != DescriptorType::DT_INVALID);
}

template <typename TDescriptor>
inline const typename UnifiedDescriptorMapT<TDescriptor>::DescriptorMap& UnifiedDescriptorMapT<TDescriptor>::descriptorMap() const
{
	return descriptorMap_;
}

template <typename TDescriptor>
inline typename UnifiedDescriptorMapT<TDescriptor>::DescriptorMap& UnifiedDescriptorMapT<TDescriptor>::UnifiedDescriptorMapT::descriptorMap()
{
	return descriptorMap_;
}

template <typename TDescriptor>
size_t UnifiedDescriptorMapT<TDescriptor>::numberObjectPoints() const
{
	return descriptorMap_.size();
}

template <typename TDescriptor>
Indices32 UnifiedDescriptorMapT<TDescriptor>::objectPointIds() const
{
	Indices32 ids;
	ids.reserve(descriptorMap_.size());

	for (const typename DescriptorMap::value_type& descriptorPair : descriptorMap_)
	{
		ids.emplace_back(descriptorPair.first);
	}

	return ids;
}

template <typename TDescriptor>
size_t UnifiedDescriptorMapT<TDescriptor>::numberDescriptors(const Index32& objectPointId) const
{
	const typename DescriptorMap::const_iterator i = descriptorMap_.find(objectPointId);

	if (i == descriptorMap_.cend())
	{
		return 0;
	}

	return i->second.size();
}

template <typename TDescriptor>
bool UnifiedDescriptorMapT<TDescriptor>::removeDescriptors(const Index32& objectPointId)
{
	const typename DescriptorMap::iterator i = descriptorMap_.find(objectPointId);

	if (i == descriptorMap_.cend())
	{
		return false;
	}

	descriptorMap_.erase(i);

	return true;
}

template <typename TDescriptor>
std::unique_ptr<UnifiedDescriptorMap> UnifiedDescriptorMapT<TDescriptor>::clone() const
{
	return std::make_unique<UnifiedDescriptorMapT<TDescriptor>>(DescriptorMap(descriptorMap_));
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTOR_H
