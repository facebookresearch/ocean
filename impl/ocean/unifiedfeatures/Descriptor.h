// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

#include "ocean/math/Numeric.h"

#include <array>
#include <type_traits>

namespace Ocean
{

namespace UnifiedFeatures
{

/// Forward declaration
template <typename TType, size_t tLength, size_t tLevels>
class DescriptorT;


/**
 * Typedef for general uint8 descriptors
 * @ingroup unifiedfeatures
 */
template <size_t tLength, size_t tLevels>
using Uint8Descriptor = DescriptorT<uint8_t, tLength, tLevels>;

/**
 * Typedef for binary descriptors that are 96-byte long and have 1 level
 * @ingroup unifiedfeatures
 */
typedef Uint8Descriptor<96, 1> Uint8Descriptor96;

/**
 * Typedef for binary descriptors that are 96-byte long and have 1 level
 * @ingroup unifiedfeatures
 */
typedef Uint8Descriptor<128, 1> Uint8Descriptor128;


/**
 * Typedef for general binary descriptors
 * @ingroup unifiedfeatures
 */
template <size_t tLength, size_t tLevels>
using BinaryDescriptor = DescriptorT<uint8_t, tLength, tLevels>;

/**
 * Typedef for binary descriptors that are 32-byte long and have 1 level
 * @ingroup unifiedfeatures
 */
typedef BinaryDescriptor<32, 1> BinaryDescriptor32;

/**
 * Typedef for vectors of binary descriptors that are 32-byte long and have 1 level
 * @ingroup unifiedfeatures
 */
typedef std::vector<BinaryDescriptor32> BinaryDescriptors32;


/**
 * Typedef for binary descriptors that are 32-byte long and have 3 levels
 * @ingroup unifiedfeatures
 */
typedef BinaryDescriptor<32, 3> BinaryDescriptor32L3;

/**
 * Typedef for vectors of binary descriptors that are 32-byte long and have 3 levels
 * @ingroup unifiedfeatures
 */
typedef std::vector<BinaryDescriptor32L3> BinaryDescriptors32L3;


/**
 * Typedef for binary descriptors that are 64-byte long and have 1 level
 * @ingroup unifiedfeatures
 */
typedef BinaryDescriptor<64, 1> BinaryDescriptor64;

/**
 * Typedef for vectors of binary descriptors that are 64-byte long and have 1 level
 * @ingroup unifiedfeatures
 */
typedef std::vector<BinaryDescriptor64> BinaryDescriptors64;


/**
 * Typedef for binary descriptors that are 64-byte long and have 3 levels
 * @ingroup unifiedfeatures
 */
typedef BinaryDescriptor<64, 3> BinaryDescriptor64L3;

/**
 * Typedef for vectors of binary descriptors that are 64-byte long and have 3 levels
 * @ingroup unifiedfeatures
 */
typedef std::vector<BinaryDescriptor64L3> BinaryDescriptors64L3;


/**
 * Typedef for general float descriptors
 * @ingroup unifiedfeatures
 */
template <size_t tLength, size_t tLevels>
using FloatDescriptor = DescriptorT<float, tLength, tLevels>;


/**
 * Typedef for float descriptors that have 32 elements and 1 level
 * @ingroup unifiedfeatures
 */
typedef FloatDescriptor<32, 1> FloatDescriptor32;

/**
 * Typedef for vectors of float descriptors that have 32 elements and have 1 level
 * @ingroup unifiedfeatures
 */
typedef std::vector<FloatDescriptor32> FloatDescriptors32;


/**
 * Helper class to determine the type of the descriptor distance
 * @tparam T The type that the descriptors are stored as
 * @ingroup unifiedfeatures
 */
template <typename T>
class DistanceTyper
{
	public:

		/// Definition of the distance type
		typedef float Type;
};

/**
 * This class implements a helper to compute the different descriptor distances based on the type of the descriptors
 * @tparam T The underlying data type that is used by the descriptor, must be an arithmetic type
 * @ingroup unifiedfeatures
 */
template <typename T>
class DescriptorDistanceComputation
{
	public:

		/// Definition of the distance type
		typedef typename DistanceTyper<T>::Type DistanceType;

	public:

		/**
		 * Computes the distance between two 1-level descriptors
		 * Note: for `T == uint8`, this function computes the Hamming distance, otherwise it will compute the L2 norm
		 * @param descriptor0 The first descriptor
		 * @param descriptor1 The second descriptor
		 * @return The distance between the two descriptors
		 * @tparam tLength The length of the descriptor; for `T == uint8`, the value of `tLength` must be a multiple of 8; range: [1, infinity)
		 */
		template <size_t tLength>
		static DistanceType computeDistance(const std::array<T, tLength>& descriptor0, const std::array<T, tLength>& descriptor1);
};

/**
 * Definition of a generic descriptor class
 * @ingroup unifiedfeatures
 */
template <typename TType, size_t tLength, size_t tLevels>
class DescriptorT
{
	public:

		/// Typedef for the actual descriptor data on each level
		typedef std::array<std::array<TType, tLength>, tLevels> DescriptorLevels;

		/// Definition of the distance type
		typedef typename DistanceTyper<TType>::Type DistanceType;

	public:

		/**
		 * Default constructor
		 */
		DescriptorT() = default;

		/**
		 * Constructor
		 * @param descriptorLevels The descriptors that will be stored in this instance
		 */
		DescriptorT(const DescriptorLevels& descriptorLevels);

		/**
		 * Returns a reference to the descriptor data
		 * @return The descriptor data
		 */
		DescriptorLevels& descriptorLevels();

		/**
		 * Returns a const reference to the descriptor data
		 * @return The descriptor data
		 */
		const DescriptorLevels& descriptorLevels() const;

		/**
		 * Computes the distance between this instance and another descriptor
		 * For multi-level descriptors this function computes the distance per level and returns the minimum distance value.
		 * @param descriptor The other descriptor that will be used
		 * @return The distance value; will be `uint32_t` if `TType == uint8_t`, otherwise `float`
		 */
		DistanceType computeDistance(const DescriptorT<TType, tLength, tLevels>& descriptor) const;

		/**
		 * Returns the number of levels of this descriptor
		 * @return The number of levels
		 */
		static constexpr size_t levels();

	protected:

		/// Stores the actual descriptor data
		DescriptorLevels descriptorLevels_;
};

/**
 * Specialization of the helper class
 * @ingroup unifiedfeatures
 */
template <>
class DistanceTyper<uint8_t>
{
	public:

		/// Definition of the distance type
		typedef uint32_t Type;
};

template <typename T>
template <size_t tLength>
typename DescriptorDistanceComputation<T>::DistanceType DescriptorDistanceComputation<T>::computeDistance(const std::array<T, tLength>& descriptor0, const std::array<T, tLength>& descriptor1)
{
	// L2 vector norm (float-based descriptors)
	static_assert(std::is_arithmetic<T>::value, "The elements of a descriptor need to be of an arithmetic type");
	static_assert(std::is_floating_point<DistanceType>::value, "This distance computation assumes the distance type to be a floating-point type.");

	DistanceType distance = DistanceType(0);

	for (size_t i = 0; i < tLength; ++i)
	{
		distance += (DistanceType(descriptor0[i]) - DistanceType(descriptor1[i])) * (DistanceType(descriptor0[i]) - DistanceType(descriptor1[i]));
	}

	ocean_assert(distance >= DistanceType(0));

	return NumericT<DistanceType>::sqrt(distance);
}

template <>
template <size_t tLength>
typename DescriptorDistanceComputation<uint8_t>::DistanceType DescriptorDistanceComputation<uint8_t>::computeDistance(const std::array<uint8_t, tLength>& descriptor0, const std::array<uint8_t, tLength>& descriptor1)
{
	static_assert(std::is_integral<DistanceType>::value && sizeof(DistanceType) > sizeof(uint8_t), "This distance computations assumes the distance to be an integer type that is larger than uint8_t");
	// Hamming distance (binary descriptors)
	//
	// For compiler optimizations, the bits are split into groups of uint64_t.
	static_assert(tLength != 0 && tLength % sizeof(uint64_t) == 0, "The descriptor size must be a multiple of the size of uint64_t");
	constexpr size_t multiplesOfUint64_t = tLength / sizeof(uint64_t);

	const uint64_t* data0 = reinterpret_cast<const uint64_t*>(descriptor0.data());
	const uint64_t* data1 = reinterpret_cast<const uint64_t*>(descriptor1.data());

	DistanceType distance = DistanceType(0);
	for (size_t i = 0; i < multiplesOfUint64_t; ++i)
	{
		uint64_t difference = *data0 ^ *data1;

		while (difference != 0)
		{
			++distance;

			// Counting bits using Brian Kernighan's way: https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
			difference = difference & (difference - 1);
		}

		++data0;
		++data1;
	}

	return distance;
}

template <typename TType, size_t tLength, size_t tLevels>
DescriptorT<TType, tLength, tLevels>::DescriptorT(const DescriptorLevels& descriptorLevels) :
	descriptorLevels_(descriptorLevels)
{
	static_assert(std::is_arithmetic<TType>::value, "The elements of a descriptor need to be of an arithmetic type");
	static_assert(tLevels >= 1, "Binary descriptors must have one or more levels");
}

template <typename TType, size_t tLength, size_t tLevels>
typename DescriptorT<TType, tLength, tLevels>::DescriptorLevels& DescriptorT<TType, tLength, tLevels>::descriptorLevels()
{
	return descriptorLevels_;
}

template <typename TType, size_t tLength, size_t tLevels>
const typename DescriptorT<TType, tLength, tLevels>::DescriptorLevels& DescriptorT<TType, tLength, tLevels>::descriptorLevels() const
{
	return descriptorLevels_;
}

template <typename TType, size_t tLength, size_t tLevels>
typename DescriptorT<TType, tLength, tLevels>::DistanceType DescriptorT<TType, tLength, tLevels>::computeDistance(const DescriptorT<TType, tLength, tLevels>& descriptor) const
{
	const DescriptorLevels& descriptorLevels0 = descriptorLevels();
	const DescriptorLevels& descriptorLevels1 = descriptor.descriptorLevels();

	DistanceType minimumDistance = DescriptorDistanceComputation<TType>::computeDistance(descriptorLevels0[0], descriptorLevels1[0]);

	for (size_t level = 1; level < tLevels; ++level)
	{
		const float distance = DescriptorDistanceComputation<TType>::computeDistance(descriptorLevels0[level], descriptorLevels1[level]);
		minimumDistance = std::min(minimumDistance, distance);
	}

	return minimumDistance;
}

template <typename TType, size_t tLength, size_t tLevels>
constexpr size_t DescriptorT<TType, tLength, tLevels>::levels()
{
	return tLevels;
}

} // namespace UnifiedFeatures

} // namespace Ocean
