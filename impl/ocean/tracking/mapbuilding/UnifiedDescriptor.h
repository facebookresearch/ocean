/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTOR_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTOR_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"

#include "ocean/cv/detector/Descriptor.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements the base class for all unified descriptor objects.
 * @ingroup trackingmapbuilding
 */
class UnifiedDescriptor
{
	protected:

		/// The first bit at which the number of items are defined in the descriptor type.
		static constexpr uint64_t desriptorTypeNumberItemsBeginBit_ = 32ull;

		/// The end/exclusive bit up to which the number of items are defined in the descriptor type.
		static constexpr uint64_t desriptorTypeNumberItemsEndBit_ = desriptorTypeNumberItemsBeginBit_ + 16ull;

		/// The first bit at which custom descriptors are defined in the descriptor type.
		static constexpr uint64_t desriptorTypeCustomTypeBeginBit_ = 56ull;

	public:

		/**
		 * Definition of descriptor types.
		 */
		enum DescriptorType : uint64_t
		{
			/// An invalid descriptor.
			DT_INVALID = 0ull,

			/// A binary-based descriptor.
			DT_BINARY = 1ull << 0ull,
			/// A float-based descriptor.
			DT_FLOAT = 1ull << 1ull,

			/// A descriptor containing only one level.
			DT_SINGLE_LEVEL = 1ull << 2ull,
			/// A descriptor containing multiple levels (e.g., scale level).
			DT_MULTI_LEVEL = 1ull << 3ull,

			/// A descriptor based on a single view (e.g., a descriptor of a 2D image point in a single camera image).
			DT_SINGLE_VIEW = 1ull << 4ull,
			/// A descriptor based on multiple views (e.g., a descriptor of a 3D object point observed from several different locations/angles etc.)
			DT_MULTI_VIEW = 1ull << 5ull,

			/// A single level binary descriptor.
			DT_BINARY_SINGLE_LEVEL = DT_BINARY | DT_SINGLE_LEVEL,
			/// A multi level binary descriptor.
			DT_BINARY_MULTI_LEVEL = DT_BINARY | DT_MULTI_LEVEL,

			/// A single level float descriptor.
			DT_FLOAT_SINGLE_LEVEL = DT_FLOAT | DT_SINGLE_LEVEL,
			/// A multi level float descriptor.
			DT_FLOAT_MULTI_LEVEL = DT_FLOAT | DT_MULTI_LEVEL,

			/// A single view, single level, binary descriptor.
			DT_BINARY_SINGLE_LEVEL_SINGLE_VIEW = DT_BINARY_SINGLE_LEVEL | DT_SINGLE_VIEW,
			/// A multi view, single level, binary descriptor.
			DT_BINARY_SINGLE_LEVEL_MULTI_VIEW = DT_BINARY_SINGLE_LEVEL | DT_MULTI_VIEW,
			/// A single view, multi level, binary descriptor.
			DT_BINARY_MULTI_LEVEL_SINGLE_VIEW = DT_BINARY_MULTI_LEVEL | DT_SINGLE_VIEW,
			/// A multi view, multi level, binary descriptor.
			DT_BINARY_MULTI_LEVEL_MULTI_VIEW = DT_BINARY_MULTI_LEVEL | DT_MULTI_VIEW,

			/// A single view, single level, float descriptor.
			DT_FLOAT_SINGLE_LEVEL_SINGLE_VIEW = DT_FLOAT_SINGLE_LEVEL | DT_SINGLE_VIEW,
			/// A multi view, single level, float descriptor.
			DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW = DT_FLOAT_SINGLE_LEVEL | DT_MULTI_VIEW,
			/// A single view, multi level, float descriptor.
			DT_FLOAT_MULTI_LEVEL_SINGLE_VIEW = DT_FLOAT_MULTI_LEVEL | DT_SINGLE_VIEW,
			/// A multi view, multi level, float descriptor.
			DT_FLOAT_MULTI_LEVEL_MULTI_VIEW = DT_FLOAT_MULTI_LEVEL | DT_MULTI_VIEW,

			/// A single view, single level, float descriptor with 128 elements.
			DT_FLOAT_SINGLE_LEVEL_SINGLE_VIEW_128 = DT_FLOAT_SINGLE_LEVEL_SINGLE_VIEW | (128ull << desriptorTypeNumberItemsBeginBit_),
			/// A multi view, single level, float descriptor with 128 elements.
			DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW_128 = DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW | (128ull << desriptorTypeNumberItemsBeginBit_),

			/// A custom single view, multi level, FREAK descriptor  with 256 bits.
			DT_FREAK_MULTI_LEVEL_SINGLE_VIEW_256 = (1ull << (desriptorTypeCustomTypeBeginBit_ + 0ull)) | DT_BINARY_MULTI_LEVEL_SINGLE_VIEW | (256ull << desriptorTypeNumberItemsBeginBit_),
			/// A custom multi view, multi level, FREAK descriptor  with 256 bits.
			DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256 = (1ull << (desriptorTypeCustomTypeBeginBit_ + 0ull)) | DT_BINARY_MULTI_LEVEL_MULTI_VIEW | (256ull << desriptorTypeNumberItemsBeginBit_),
		};

		/**
		 * Definition of a byte descriptor.
		 * @tparam tNumberBytes The number of bytes the descriptor has, with range [1, 8191]
		 */
		template <uint16_t tNumberBytes>
		using ByteDescriptor = std::array<uint8_t, tNumberBytes>;

		/**
		 * Definition of a vector holding byte descriptors.
		 * @tparam tNumberBytes The number of bytes each descriptor has, with range [1, infinity)
		 */
		template <uint16_t tNumberBytes>
		using ByteDescriptors = std::vector<ByteDescriptor<tNumberBytes>>;

		/**
		 * Definition of a binary descriptor.
		 * @tparam tNumberBits The number of bits the descriptor has, with range [1, infinity)
		 */
		template <uint16_t tNumberBits>
		using BinaryDescriptor = ByteDescriptor<tNumberBits / 8u>;

		/**
		 * Definition of a vector holding binary descriptors.
		 * @tparam tNumberBits The number of bits each descriptor has, with range [1, infinity)
		 */
		template <uint16_t tNumberBits>
		using BinaryDescriptors = std::vector<BinaryDescriptor<tNumberBits>>;

		/**
		 * Definition of a float descriptor.
		 * @tparam tNumberElements The number of descriptor elements, with range [1, infinity)
		 */
		template <uint16_t tNumberElements>
		using FloatDescriptor = std::array<float, tNumberElements>;

		/**
		 * Definition of a vector holding float descriptors.
		 * @tparam tNumberElements The number of elements each descriptor has, with range [1, infinity)
		 */
		template <uint16_t tNumberElements>
		using FloatDescriptors = std::vector<FloatDescriptor<tNumberElements>>;

		/**
		 * Definition of a FREAK  Multi Descriptor with 256 bits (32 bytes).
		 * This custom definition should be removed unce entirely replaced with generic binary descriptors.
		 */
		typedef CV::Detector::FREAKDescriptor32 FreakMultiDescriptor256;

		/**
		 * Definition of vector holding FREAK  Multi Descriptors with 256 bits (32 bytes).
		 * This custom definition should be removed unce entirely replaced with generic binary descriptors.
		 */
		typedef std::vector<FreakMultiDescriptor256> FreakMultiDescriptors256;

		/**
		 * This class implements a helper class allowing to determine the type of the distance value between two descriptors.
		 * @param T The data type of the descriptor for which the type of the distance value is needed
		 */
		template <typename T>
		class DistanceTyper
		{
			// nothing to do here, needs to be implemented in a template specialization.
		};

		/**
		 * This class implements a helper class allowing to determine the descriptor type value for a descriptor data type.
		 * @param T The data type of the descriptor for which the type value is needed
		 */
		template <typename T>
		class DescriptorTyper
		{
			// nothing to do here, needs to be implemented in a template specialization.
		};

	public:

		/**
		 * Returns the descriptor type of all descriptors hold in this object.
		 * @return The type of all descriptors
		 */
		inline DescriptorType descriptorType() const;

		/**
		 * Returns whether a descriptor type represents a binary-based descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isBinary(const DescriptorType descriptorType);

		/**
		 * Returns whether a descriptor type represents a float-based descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isFloat(const DescriptorType descriptorType);

		/**
		 * Returns whether a descriptor type represents a single level descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isSingleLevel(const DescriptorType descriptorType);

		/**
		 * Returns whether a descriptor type represents a multi level descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isMultiLevel(const DescriptorType descriptorType);

		/**
		 * Returns whether a descriptor type represents a single view descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isSingleView(const DescriptorType descriptorType);

		/**
		 * Returns whether a descriptor type represents a multi view descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isMultiView(const DescriptorType descriptorType);

		/**
		 * Returns the number of bits a binary descriptor is composed of.
		 * @param descriptorType The descriptor type for which the number of bits will be returned
		 * @return The number of bits in the binary descriptor, 0 if the descriptor is not a binary descriptor
		 */
		static constexpr uint16_t numberBits(const DescriptorType descriptorType);

		/**
		 * Returns the number of elements a float descriptor is composed of.
		 * @param descriptorType The descriptor type for which the number of elements will be returned
		 * @return The number of elements in the float descriptor, 0 if the descriptor is not a float descriptor
		 */
		static constexpr uint16_t numberElements(const DescriptorType descriptorType);

		/**
		 * Returns whether a descriptor type represents a custom descriptor.
		 * @param descriptorType The descriptor type to check
		 * @return True, if so
		 */
		static constexpr bool isCustom(const DescriptorType descriptorType);

		/**
		 * Returns a binary descriptor type.
		 * @param multiLevel True, if the descriptor is a multi level descriptor; False, if the descriptor is a single level descriptor
		 * @param multiView True, if the descriptor is a multi view descriptor; False, if the descriptor is a single view descriptor
		 * @param numberBits The number of bits the binary descriptor has, with range [1, infinity)
		 * @return The descriptor type of the specified descriptor
		 */
		static constexpr DescriptorType binaryDescriptorType(const bool multiLevel, const bool multiView, const uint16_t numberBits);

		/**
		 * Returns a float descriptor type.
		 * @param multiLevel True, if the descriptor is a multi level descriptor; False, if the descriptor is a single level descriptor
		 * @param multiView True, if the descriptor is a multi view descriptor; False, if the descriptor is a single view descriptor
		 * @param numberElements The number of elements the float descriptor has, with range [1, infinity)
		 * @return The descriptor type of the specified descriptor
		 */
		static constexpr DescriptorType floatDescriptorType(const bool multiLevel, const bool multiView, const uint16_t numberElements);

		/**
		 * Returns a descriptor type with a specific number of items (either bits or elements).
		 * @param descriptorType The descriptor type to for which the number of items will be set, must be valid, must be either a binary descriptor or a float descriptor
		 * @param numberItems The number of items to set, items are bits for binary descriptors and elements for float descriptors, with range [1, infinity)
		 * @return The resulting descriptor type
		 */
		static constexpr DescriptorType descriptorType(const DescriptorType descriptorType, const uint16_t numberItems);

		/**
		 * Returns a binary descriptor type.
		 * @tparam tMultiLevel True, if the descriptor is a multi level descriptor; False, if the descriptor is a single level descriptor
		 * @tparam tMultiView True, if the descriptor is a multi view descriptor; False, if the descriptor is a single view descriptor
		 * @tparam tNumberBits The number of bits the binary descriptor has, with range [1, infinity)
		 * @return The descriptor type of the specified descriptor
		 */
		template <bool tMultiLevel, bool tMultiView, uint16_t tNumberBits>
		static constexpr DescriptorType binaryDescriptorType();

		/**
		 * Returns a float descriptor type.
		 * @tparam tMultiLevel True, if the descriptor is a multi level descriptor; False, if the descriptor is a single level descriptor
		 * @tparam tMultiView True, if the descriptor is a multi view descriptor; False, if the descriptor is a single view descriptor
		 * @tparam tNumberElements The number of elements the float descriptor has, with range [1, infinity)
		 * @return The descriptor type of the specified descriptor
		 */
		template <bool tMultiLevel, bool tMultiView, uint16_t tNumberElements>
		static constexpr DescriptorType floatDescriptorType();

		/**
		 * Returns a descriptor type with a specific number of items (either bits or elements).
		 * @tparam tDescriptorType The descriptor type to for which the number of items will be set, must be valid, must be either a binary descriptor or a float descriptor
		 * @tparam tNumberItems The number of items to set, items are bits for binary descriptors and elements for float descriptors, with range [1, infinity)
		 * @return The resulting descriptor type
		 */
		template <DescriptorType tDescriptorType, uint16_t tNumberItems>
		static constexpr DescriptorType descriptorType();

	protected:

		/**
		 * Creates a new descriptors object.
		 * @param descriptorType The type of the descriptors
		 */
		explicit inline UnifiedDescriptor(const DescriptorType descriptorType);

	protected:

		/// The descriptor type of all descriptors hold in this object.
		DescriptorType descriptorType_ = DT_INVALID;
};

/**
 * Specialization of UnifiedDescriptor::DistanceTyper for UnifiedDescriptor::ByteDescriptor.
 * @tparam tNumberBytes The number of byte elements the descriptor has, with range [8, 8191]
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberBytes>
class UnifiedDescriptor::DistanceTyper<UnifiedDescriptor::ByteDescriptor<tNumberBytes>>
{
	static_assert(tNumberBytes >= 8u, "Most likely wrong descriptor!");
	static_assert(tNumberBytes <= 8191u, "Invalid descriptor!");

	public:

		/// The distance type the descriptor uses.
		typedef unsigned int Type;
};

/**
 * Specialization of UnifiedDescriptor::DistanceTyper for UnifiedDescriptor::FloatDescriptor.
 * @tparam tNumberElements The number of float elements the descriptor has, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberElements>
class UnifiedDescriptor::DistanceTyper<UnifiedDescriptor::FloatDescriptor<tNumberElements>>
{
	static_assert(tNumberElements >= 1u, "Invalid number of elements!");

	public:

		/// The distance type the descriptor uses.
		typedef float Type;
};

/**
 * Specialization of UnifiedDescriptor::DistanceTyper for CV::Detector::FREAKDescriptor32.
 * This class should be removed once the custom FREAK descriptor is not used anymore.
 * @ingroup trackingmapbuilding
 */
template <>
class UnifiedDescriptor::DistanceTyper<CV::Detector::FREAKDescriptor32>
{
	public:

		/// The distance type the descriptor uses.
		typedef unsigned int Type;
};

/**
 * Specialization of UnifiedDescriptor::DescriptorTyper for UnifiedDescriptor::ByteDescriptor.
 * @tparam tNumberBytes The number of byte elements the descriptor has, with range [8, 8191]
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberBytes>
class UnifiedDescriptor::DescriptorTyper<UnifiedDescriptor::ByteDescriptor<tNumberBytes>>
{
	static_assert(tNumberBytes >= 8u, "Most likely wrong descriptor!");
	static_assert(tNumberBytes <= 8191u, "Invalid descriptor!");

	public:

		/**
		 * Returns the type of the descriptor
		 * @return The descriptor's type
		 */
		static constexpr DescriptorType type()
		{
			return UnifiedDescriptor::descriptorType<DT_BINARY_SINGLE_LEVEL_SINGLE_VIEW, tNumberBytes * 8u>();
		}
};

/**
 * Specialization of UnifiedDescriptor::DescriptorTyper for UnifiedDescriptor::ByteDescriptors.
 * @tparam tNumberBytes The number of byte elements the descriptor has, with range [8, 8191]
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberBytes>
class UnifiedDescriptor::DescriptorTyper<UnifiedDescriptor::ByteDescriptors<tNumberBytes>>
{
	static_assert(tNumberBytes >= 8u, "Most likely wrong descriptor!");
	static_assert(tNumberBytes <= 8191u, "Invalid descriptor!");

	public:

		/**
		 * Returns the type of the descriptor
		 * @return The descriptor's type
		 */
		static constexpr DescriptorType type()
		{
			return UnifiedDescriptor::descriptorType<DT_BINARY_SINGLE_LEVEL_MULTI_VIEW, tNumberBytes * 8u>();
		}
};

/**
 * Specialization of UnifiedDescriptor::DescriptorTyper for UnifiedDescriptor::FloatDescriptor.
 * @tparam tNumberElements The number of float elements the descriptor has, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberElements>
class UnifiedDescriptor::DescriptorTyper<UnifiedDescriptor::FloatDescriptor<tNumberElements>>
{
	static_assert(tNumberElements >= 1u, "Invalid number of elements!");

	public:

		/**
		 * Returns the type of the descriptor
		 * @return The descriptor's type
		 */
		static constexpr DescriptorType type()
		{
			return descriptorType<DT_FLOAT_SINGLE_LEVEL_SINGLE_VIEW, tNumberElements>();
		}
};

/**
 * Specialization of UnifiedDescriptor::DescriptorTyper for UnifiedDescriptor::FloatDescriptors.
 * @tparam tNumberElements The number of float elements the descriptor has, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberElements>
class UnifiedDescriptor::DescriptorTyper<UnifiedDescriptor::FloatDescriptors<tNumberElements>>
{
	static_assert(tNumberElements >= 1u, "Invalid number of elements!");

	public:

		static constexpr DescriptorType type()
		{
			return descriptorType<DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW, tNumberElements>();
		}
};

/**
 * Specialization of UnifiedDescriptor::DescriptorTyper for FreakMultiDescriptor256.
 * This class should be removed once the custom FREAK descriptor is not used anymore.
 * @ingroup trackingmapbuilding
 */
template <>
class UnifiedDescriptor::DescriptorTyper<UnifiedDescriptor::FreakMultiDescriptor256>
{
	public:

		static constexpr DescriptorType type()
		{
			return DT_FREAK_MULTI_LEVEL_SINGLE_VIEW_256;
		}
};

/**
 * Specialization of UnifiedDescriptor::DescriptorTyper for FreakMultiDescriptors256.
 * This class should be removed once the custom FREAK descriptor is not used anymore.
 * @ingroup trackingmapbuilding
 */
template <>
class UnifiedDescriptor::DescriptorTyper<UnifiedDescriptor::FreakMultiDescriptors256>
{
	public:

		static constexpr DescriptorType type()
		{
			return DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256;
		}
};

inline UnifiedDescriptor::UnifiedDescriptor(const DescriptorType descriptorType) :
	descriptorType_(descriptorType)
{
	// nothing to do here
}

inline UnifiedDescriptor::DescriptorType UnifiedDescriptor::descriptorType() const
{
	return descriptorType_;
}

constexpr bool UnifiedDescriptor::isBinary(const DescriptorType descriptorType)
{
	return (descriptorType & DT_BINARY) == DT_BINARY;
}

constexpr bool UnifiedDescriptor::isFloat(const DescriptorType descriptorType)
{
	return (descriptorType & DT_FLOAT) == DT_FLOAT;
}

constexpr bool UnifiedDescriptor::isSingleLevel(const DescriptorType descriptorType)
{
	return (descriptorType & DT_SINGLE_LEVEL) == DT_SINGLE_LEVEL;
}

constexpr bool UnifiedDescriptor::isMultiLevel(const DescriptorType descriptorType)
{
	return (descriptorType & DT_MULTI_LEVEL) == DT_MULTI_LEVEL;
}

constexpr bool UnifiedDescriptor::isSingleView(const DescriptorType descriptorType)
{
	return (descriptorType & DT_SINGLE_VIEW) == DT_SINGLE_VIEW;
}

constexpr bool UnifiedDescriptor::isMultiView(const DescriptorType descriptorType)
{
	return (descriptorType & DT_MULTI_VIEW) == DT_MULTI_VIEW;
}

constexpr uint16_t UnifiedDescriptor::numberBits(const DescriptorType descriptorType)
{
	return isFloat(descriptorType) ? uint16_t(descriptorType >> desriptorTypeNumberItemsBeginBit_) : 0u;
}

constexpr uint16_t UnifiedDescriptor::numberElements(const DescriptorType descriptorType)
{
	return isFloat(descriptorType) ? uint16_t(descriptorType >> desriptorTypeNumberItemsBeginBit_) : 0u;
}

constexpr bool UnifiedDescriptor::isCustom(const DescriptorType descriptorType)
{
	return (descriptorType >> desriptorTypeCustomTypeBeginBit_) != 0ull;
}

constexpr UnifiedDescriptor::DescriptorType UnifiedDescriptor::binaryDescriptorType(const bool multiLevel, const bool multiView, const uint16_t numberBits)
{
	ocean_assert(numberBits >= 1u && numberBits % 8u == 0u);

	return DescriptorType(DT_BINARY | (multiLevel ? DT_MULTI_LEVEL : DT_SINGLE_LEVEL) | (multiView ? DT_MULTI_VIEW : DT_SINGLE_VIEW) | (uint64_t(numberBits) << desriptorTypeNumberItemsBeginBit_));
}

constexpr UnifiedDescriptor::DescriptorType UnifiedDescriptor::floatDescriptorType(const bool multiLevel, const bool multiView, const uint16_t numberElements)
{
	return DescriptorType(DT_FLOAT | (multiLevel ? DT_MULTI_LEVEL : DT_SINGLE_LEVEL) | (multiView ? DT_MULTI_VIEW : DT_SINGLE_VIEW) | (uint64_t(numberElements) << desriptorTypeNumberItemsBeginBit_));
}

constexpr UnifiedDescriptor::DescriptorType UnifiedDescriptor::descriptorType(const DescriptorType descriptorType, const uint16_t numberItems)
{
	ocean_assert(numberBits(descriptorType) == 0u);

	return DescriptorType(descriptorType | (uint64_t(numberItems) << desriptorTypeNumberItemsBeginBit_));
}

template <bool tMultiLevel, bool tMultiView, uint16_t tNumberBits>
constexpr UnifiedDescriptor::DescriptorType UnifiedDescriptor::binaryDescriptorType()
{
	static_assert(tNumberBits >= 1u, "Invalid number bits!");

	return binaryDescriptorType(tMultiLevel, tMultiView, tNumberBits);
}

template <bool tMultiLevel, bool tMultiView, uint16_t tNumberElements>
constexpr UnifiedDescriptor::DescriptorType UnifiedDescriptor::floatDescriptorType()
{
	static_assert(tNumberElements >= 1u, "Invalid number elements!");

	return floatDescriptorType(tMultiLevel, tMultiView, tNumberElements);
}

template <UnifiedDescriptor::DescriptorType tDescriptorType, uint16_t tNumberItems>
constexpr UnifiedDescriptor::DescriptorType UnifiedDescriptor::descriptorType()
{
	static_assert(tDescriptorType != DescriptorType::DT_INVALID, "Invalid descriptor type!");
	static_assert(tNumberItems >= 1u, "Invalid item number!");

	return descriptorType(tDescriptorType, tNumberItems);
}

/**
 * This class implements the base class for unified descriptor objects with specific descriptor type.
 * This class needs to be specialized for the individual descriptors.
 * @tparam TDescriptor The data type of the descriptor to be used
 * @tparam TDistance The data type of the distance between two descriptors
 * @ingroup trackingmapbuilding
 */
template <typename TDescriptor, typename TDistance = typename UnifiedDescriptor::DistanceTyper<TDescriptor>::Type>
class UnifiedDescriptorT : public UnifiedDescriptor
{
	// nothing to do here
};

/**
 * Specialization of UnifiedDescriptorT for UnifiedDescriptor::ByteDescriptor.
 * @tparam tNumberBytes The number of bytes the descriptor has, with range [8, 8191]
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberBytes>
class UnifiedDescriptorT<UnifiedDescriptor::ByteDescriptor<tNumberBytes>> : public UnifiedDescriptor
{
	static_assert(tNumberBytes >= 8u && tNumberBytes <= 8191u, "Most likely wrong descriptor!");
	static_assert(tNumberBytes <= 8191u, "Invalid descriptor!");

	public:

		/**
		 * The data type of the descriptor.
		 */
		typedef ByteDescriptor<tNumberBytes> Descriptor;

		/**
		 * Definition of a vector holding descriptors.
		 */
		typedef std::vector<Descriptor> Descriptors;

	public:

		/**
		 * Determines the distance between two single-view descriptors.
		 * The used metric depends on the feature descriptor type (e.g., square distance for float features, or Hamming distance for binary features).
		 * @param descriptorA The first single-view descriptor
		 * @param descriptorB The second single-view descriptor
		 * @return The resulting distance between both features
		 */
		static OCEAN_FORCE_INLINE unsigned int determineDistance(const Descriptor& descriptorA, const Descriptor& descriptorB);

		/**
		 * Determines the distance between a single-view descriptor and a multi-view descriptor.
		 * The used metric depends on the feature descriptor type (e.g., square distance for float features, or Hamming distance for binary features).
		 * @param descriptorA The single-view descriptor
		 * @param descriptorsB The multi-view descriptor
		 * @return The resulting distance between both features
		 */
		static OCEAN_FORCE_INLINE unsigned int determineDistance(const Descriptor& descriptorA, const Descriptors& descriptorsB);
};

template <uint16_t tNumberBytes>
OCEAN_FORCE_INLINE unsigned int UnifiedDescriptorT<typename UnifiedDescriptor::ByteDescriptor<tNumberBytes>>::determineDistance(const Descriptor& descriptorA, const Descriptor& descriptorB)
{
	return CV::Detector::Descriptor::calculateHammingDistance<sizeof(Descriptor) * 8>(&descriptorA, &descriptorB);
}

template <uint16_t tNumberBytes>
OCEAN_FORCE_INLINE unsigned int UnifiedDescriptorT<typename UnifiedDescriptor::ByteDescriptor<tNumberBytes>>::determineDistance(const Descriptor& descriptorA, const Descriptors& descriptorsB)
{
	unsigned int bestDistance = (unsigned int)(-1);

	for (const Descriptor& descriptorB : descriptorsB)
	{
		const unsigned int distance = determineDistance(descriptorA, descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

/**
 * Specialization of UnifiedDescriptorT for UnifiedDescriptor::FloatDescriptor.
 * @tparam tNumberElements The number of float elements the descriptor has, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tNumberElements>
class UnifiedDescriptorT<UnifiedDescriptor::FloatDescriptor<tNumberElements>> : public UnifiedDescriptor
{
	static_assert(tNumberElements >= 1u, "Invalid number of elements!");

	public:

		/**
		 * The data type of the descriptor.
		 */
		typedef FloatDescriptor<tNumberElements> Descriptor;

		/**
		 * Definition of a vector holding descriptors.
		 */
		typedef std::vector<Descriptor> Descriptors;

	public:

		/**
		 * Determines the distance between two single-view descriptors.
		 * The used metric depends on the feature descriptor type (e.g., square distance for float features, or Hamming distance for binary features).
		 * @param descriptorA The first single-view descriptor
		 * @param descriptorB The second single-view descriptor
		 * @return The resulting distance between both features
		 */
		static OCEAN_FORCE_INLINE float determineDistance(const Descriptor& descriptorA, const Descriptor& descriptorB);

		/**
		 * Determines the distance between a single-view descriptor and a multi-view descriptor.
		 * The used metric depends on the feature descriptor type (e.g., square distance for float features, or Hamming distance for binary features).
		 * @param descriptorA The single-view descriptor
		 * @param descriptorsB The multi-view descriptor
		 * @return The resulting distance between both features
		 */
		static OCEAN_FORCE_INLINE float determineDistance(const Descriptor& descriptorA, const Descriptors& descriptorsB);
};

template <uint16_t tNumberElements>
OCEAN_FORCE_INLINE float UnifiedDescriptorT<typename UnifiedDescriptor::FloatDescriptor<tNumberElements>>::determineDistance(const Descriptor& descriptorA, const Descriptor& descriptorB)
{
	static_assert(tNumberElements >= 1u, "Invalid number of elements!");

	float sqrDistance = 0.0f;

	for (unsigned int n = 0u; n < tNumberElements; ++n)
	{
		const float difference = descriptorA[n] - descriptorB[n];

		sqrDistance += difference * difference;
	}

	return sqrDistance;
}

template <uint16_t tNumberElements>
OCEAN_FORCE_INLINE float UnifiedDescriptorT<typename UnifiedDescriptor::FloatDescriptor<tNumberElements>>::determineDistance(const Descriptor& descriptorA, const Descriptors& descriptorsB)
{
	static_assert(tNumberElements >= 1u, "Invalid number of elements!");

	ocean_assert(descriptorsB.size() >= 1);

	float bestSqrDistance = NumericF::maxValue();

	for (const Descriptor& descriptorB : descriptorsB)
	{
		const float sqrDistance = determineDistance(descriptorA, descriptorB);

		if (sqrDistance < bestSqrDistance)
		{
			bestSqrDistance = sqrDistance;
		}
	}

	return bestSqrDistance;
}

/**
 * Specialization of UnifiedDescriptorT for FreakMultiDescriptor256.
 * @ingroup trackingmapbuilding
 */
template <>
class UnifiedDescriptorT<UnifiedDescriptor::FreakMultiDescriptor256> : public UnifiedDescriptor
{
	public:

		/**
		 * The data type of the descriptor.
		 */
		typedef UnifiedDescriptor::FreakMultiDescriptor256 Descriptor;

		/**
		 * Definition of a vector holding descriptors.
		 */
		typedef std::vector<Descriptor> Descriptors;

	public:

		/**
		 * Determines the distance between two single-view descriptors.
		 * The used metric depends on the feature descriptor type (e.g., square distance for float features, or Hamming distance for binary features).
		 * @param descriptorA The first single-view descriptor
		 * @param descriptorB The second single-view descriptor
		 * @return The resulting distance between both features
		 */
		static OCEAN_FORCE_INLINE unsigned int determineDistance(const Descriptor& descriptorA, const Descriptor& descriptorB);

		/**
		 * Determines the distance between a single-view descriptor and a multi-view descriptor.
		 * The used metric depends on the feature descriptor type (e.g., square distance for float features, or Hamming distance for binary features).
		 * @param descriptorA The single-view descriptor
		 * @param descriptorsB The multi-view descriptor
		 * @return The resulting distance between both features
		 */
		static OCEAN_FORCE_INLINE unsigned int determineDistance(const Descriptor& descriptorA, const Descriptors& descriptorsB);
};

OCEAN_FORCE_INLINE unsigned int UnifiedDescriptorT<UnifiedDescriptor::FreakMultiDescriptor256>::determineDistance(const Descriptor& descriptorA, const Descriptor& descriptorB)
{
	return descriptorA.distance(descriptorB);
}

OCEAN_FORCE_INLINE unsigned int UnifiedDescriptorT<UnifiedDescriptor::FreakMultiDescriptor256>::determineDistance(const Descriptor& descriptorA, const Descriptors& descriptorsB)
{
	unsigned int bestDistance = (unsigned int)(-1);

	for (const Descriptor& descriptorB : descriptorsB)
	{
		const unsigned int distance = determineDistance(descriptorA, descriptorB);

		if (distance < bestDistance)
		{
			bestDistance = distance;
		}
	}

	return bestDistance;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_DESCRIPTOR_H
