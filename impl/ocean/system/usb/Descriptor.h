/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_DESCRIPTOR_H
#define META_OCEAN_SYSTEM_USB_DESCRIPTOR_H

#include "ocean/system/usb/USB.h"

namespace Ocean
{

namespace System
{

namespace USB
{

constexpr uint8_t CS_INTERFACE = 0x24u;

/**
 * This  class implements the base class for all interface descriptors.
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_EXPORT Descriptor
{
	public:

		/**
		 * Returns a custom globally unique identifier (GUID).
		 * @return The defined GUID
		 * @tparam tValue0 The first 4 bytes of the GUID, with range [0, infinity)
		 * @tparam tValue1 The next 2 bytes of the GUID, with range [0, infinity)
		 * @tparam tValue2 The next 2 bytes of the GUID, with range [0, infinity)
		 * @tparam tValue3 The next 2 bytes of the GUID, with range [0, infinity)
		 * @tparam tValue4 The last 6 bytes of the GUID, with range [0, 2^48-1]
		 */
		template <uint32_t tValue0, uint16_t tValue1, uint16_t tValue2, uint16_t tValue3, uint64_t tValue4>
		static constexpr std::array<uint8_t, 16u> createGUID();

		/**
		 * Returns a custom globally unique identifier (GUID).
		 * @param value0 The first 4 bytes of the GUID, with range [0, infinity)
		 * @param value1 The next 2 bytes of the GUID, with range [0, infinity)
		 * @param value2 The next 2 bytes of the GUID, with range [0, infinity)
		 * @param value3 The next 2 bytes of the GUID, with range [0, infinity)
		 * @param value4 The last 6 bytes of the GUID, with range [0, 2^48-1]
		 * @return The defined GUID
		 */
		static constexpr std::array<uint8_t, 16u> createGUID(const uint32_t value0, const uint16_t value1, const uint16_t value2, const uint16_t value3, const uint64_t value4);

		/**
		 * Returns a string representation of the given GUID.
		 * @param guid The GUID to return the string representation for
		 * @return The string representation of the GUID
		 */
		static std::string guid2string(const uint8_t* guid);

		/**
		 * Converts a time interval (in 100ns) to frequency in Hz.
		 * @param timeInterval The time interval to convert, in 100ns, with range [1, infinity)
		 * @return The resulting frequency in Hz, -1 if the interval is invalid
		 */
		static double interval2frequency(const uint32_t timeInterval);

		/**
		 * Converts a frequency in Hz to time interval in 100ns.
		 * @param frequency The frequency to convert, with range (0.01, infinity)
		 * @return The resulting time interval in 100ns, 0 if the frequency is invalid
		 */
		static uint32_t frequency2interval(const double frequency);

	protected:

		/**
		 * Copies the value from a given buffer.
		 * @param offset The byte offset in the buffer, with range [0, bufferSizeBytes - 1]
		 * @param value The value to which the buffer memory is copied
		 * @param buffer The buffer from which the value is copied, must be valid
		 * @tparam T The data type of the value
		 */
		template <typename T>
		static void value(const size_t offset, T& value, const void* buffer);

		/**
		 * Invalidates a descriptor.
		 * @param descriptor The invalidated descriptor
		 * @tparam TDescriptor The type of the descriptor
		 */
		template <typename TDescriptor>
		static void invalidate(TDescriptor& descriptor);

		/**
		 * Returns 8 bits from a given value which can be shifted right.
		 * @param value The value from which the bits are extracted
		 * @param bytes The number of bytes the value will be shifted right, with range [0, infinity)
		 * @return The extracted 8 bits (a byte)
		 */
		template <typename T>
		static constexpr uint8_t shift(const T& value, const unsigned int bytes);
};

template <uint32_t tValue0, uint16_t tValue1, uint16_t tValue2, uint16_t tValue3, uint64_t tValue4>
constexpr std::array<uint8_t, 16u> Descriptor::createGUID()
{
	static_assert(tValue4 <= 0xFFFFFFFFFFFFull);

	return {shift(tValue0, 3), shift(tValue0, 2), shift(tValue0, 1), shift(tValue0, 0),
				shift(tValue1, 1), shift(tValue1, 0),
				shift(tValue2, 1), shift(tValue2, 0),
				shift(tValue3, 1), shift(tValue3, 0),
				shift(tValue4, 5), shift(tValue4, 4), shift(tValue4, 3), shift(tValue4, 2), shift(tValue4, 1), shift(tValue4, 0)};
}

constexpr std::array<uint8_t, 16u> Descriptor::createGUID(const uint32_t value0, const uint16_t value1, const uint16_t value2, const uint16_t value3, const uint64_t value4)
{
	ocean_assert(value4 <= 0xFFFFFFFFFFFFull);

	return {shift(value0, 3), shift(value0, 2), shift(value0, 1), shift(value0, 0),
				shift(value1, 1), shift(value1, 0),
				shift(value2, 1), shift(value2, 0),
				shift(value3, 1), shift(value3, 0),
				shift(value4, 5), shift(value4, 4), shift(value4, 3), shift(value4, 2), shift(value4, 1), shift(value4, 0)};
}

template <typename T>
void Descriptor::value(const size_t offset, T& value, const void* buffer)
{
	memcpy(&value, (const uint8_t*)(buffer) + offset, sizeof(T));
}

template <typename TDescriptor>
void Descriptor::invalidate(TDescriptor& descriptor)
{
	descriptor.bLength_ = 0u;
	descriptor.bDescriptorType_ = 0u;
	descriptor.bDescriptorSubtype_ = 0u;

	ocean_assert(!descriptor.isValid());
}

template <typename T>
constexpr uint8_t Descriptor::shift(const T& value, const unsigned int bytes)
{
	return uint8_t((value >> T(bytes * 8)) & T(0xFF));
}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_DESCRIPTOR_H
