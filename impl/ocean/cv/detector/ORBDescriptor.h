/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_ORB_DESCRIPTOR_H
#define META_OCEAN_CV_DETECTOR_ORB_DESCRIPTOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/StaticVector.h"

#include <bitset>

namespace Ocean
{

namespace CV
{

namespace Detector
{

// Forward declaration
class ORBDescriptor;

/**
 * Definition of a static vector holding up to three ORB descriptors.
 * @ingroup cvdetector
 */
typedef StaticVector<ORBDescriptor, 3> ORBDescriptors;

/**
 * This class implement the descriptor for ORB features.
 * A ORB descriptor consists of 256 bits.
 * @ingroup cvdetector
 */
class ORBDescriptor
{
	public:

		/// The length of this descriptor in bits.
		static constexpr size_t descriptorLengthInBits = 256;

		/**
		 * Definition of a bitset with 256 bits.
		 */
		typedef std::bitset<descriptorLengthInBits> DescriptorBitset;

	public:

		/**
		 * Creates a new descriptor object holds zeros on every position.
		 */
		inline ORBDescriptor();

		/**
		 * Creates a new descriptor object by a given bitset.
		 * @param bitset Bitset of the new descriptor
		 */
		inline ORBDescriptor(const DescriptorBitset& bitset);

		/**
		 * Returns the bitset of the descriptor.
		 * @return Bitset
		 */
		inline const DescriptorBitset& bitset() const;

		/**
		 * Returns the number of bits in the bitset of the descriptor that are set.
		 * @return Number of set bits
		 */
		inline size_t count() const;

		/**
		 * Compares two descriptors and returns whether the left descriptor represents a smaller value than the right descriptor.
		 * @param rightDescriptor Descriptor to compare
		 * @return True, if so
		 */
		inline bool operator<(const ORBDescriptor& rightDescriptor) const;

		/**
		 * Returns the descriptor holds a bitset that is the result of the XOR operation of the bitsets of two descriptors.
		 * @param rightDescriptor Right descriptor
		 * @return Resulting descriptor
		 */
		inline ORBDescriptor operator^(const ORBDescriptor& rightDescriptor) const;

		/**
		 * Returns whether the bit of the bitset at a given index is set.
		 * @param index Index of the bit, range [0, 256]
		 * @return True, if so
		 */
		inline bool operator[](const size_t index) const;

		/**
		 * Returns the reference to a bit of the descriptor bitset at a given index
		 * @param index Index of the bit, range [0, 256]
		 * @return Reference to the bit
		 */
		inline DescriptorBitset::reference operator[](const size_t index);

	protected:

		// Bitset for the bits of the descriptor
		DescriptorBitset descriptor;
};

inline ORBDescriptor::ORBDescriptor()
{
	// nothing to do here
}

inline ORBDescriptor::ORBDescriptor(const DescriptorBitset& bitset)
{
	descriptor = bitset;
}

inline const ORBDescriptor::DescriptorBitset& ORBDescriptor::bitset() const
{
	return descriptor;
}

inline size_t ORBDescriptor::count() const
{
	return descriptor.count();
}

inline bool ORBDescriptor::operator<(const ORBDescriptor& rightDescriptor) const
{
	const unsigned long long* valueLeft = (unsigned long long*)&descriptor;
	const unsigned long long* valueRight = (unsigned long long*)&rightDescriptor.descriptor;

	return valueLeft[0] < valueRight[0]
			|| (valueLeft[0] == valueRight[0] && (valueLeft[1] < valueRight[1]
			|| (valueLeft[1] == valueRight[1] && (valueLeft[2] < valueRight[2]
			|| (valueLeft[2] == valueRight[2] && valueLeft[3] < valueRight[3])))));
}

inline ORBDescriptor ORBDescriptor::operator^(const ORBDescriptor& rightDescriptor) const
{
	return ORBDescriptor(descriptor ^ rightDescriptor.descriptor);
}

inline bool ORBDescriptor::operator[](const size_t index) const
{
	ocean_assert(index < sizeof(ORBDescriptor::DescriptorBitset) * 8);
	return descriptor[index];
}

inline ORBDescriptor::DescriptorBitset::reference ORBDescriptor::operator[](const size_t index)
{
	ocean_assert(index < sizeof(ORBDescriptor::DescriptorBitset) * 8);
	return descriptor[index];
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORB_DESCRIPTOR_H
