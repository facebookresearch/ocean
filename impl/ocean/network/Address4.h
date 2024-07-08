/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_ADDRESS4_H
#define FACEBOOK_NETWORK_ADDRESS4_H

#include "ocean/network/Network.h"
#include "ocean/network/Data.h"

namespace Ocean
{

namespace Network
{

/**
 * This class wraps an address number with 32 bits.
 * Internally, the address number is stored in big-endian order.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Address4 : public Data
{
	public:

		/**
		 * Creates a new address object with zero as default value.
		 */
		inline Address4() = default;

		/**
		 * Creates a new address object.
		 * @param address Address4 number in big-endian order
		 */
		inline Address4(const uint32_t address);

		/**
		 * Creates a new address object by a given address number in big-endian, little-endian order or local platform.
		 * @param address Address4 number to be wrapped
		 * @param orderType Type of the order
		 */
		inline Address4(const uint32_t address, const OrderType orderType);

		/**
		 * Creates a new address object.
		 * @param sub0 First sub address
		 * @param sub1 Second sub address
		 * @param sub2 Third sub address
		 * @param sub3 Fourth sub address
		 */
		inline Address4(const uint8_t sub0, const uint8_t sub1, const uint8_t sub2, const uint8_t sub3);

		/**
		 * Returns the address number in little-endian order.
		 * @return Address4 number in little-endian order
		 */
		inline uint32_t littleEndian() const;

		/**
		 * Returns the address number as a readable string.
		 * @return Readable address string
		 */
		std::string readable() const;

		/**
		 * Returns whether this address hold a valid address.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this address hold an invalid address (a zero address).
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns the local host address.
		 * @return Local host address
		 */
		static inline Address4 localHost();

		/**
		 * Returns whether two address objects are identical.
		 * @param address Right address object to compare
		 * @return True, if so
		 */
		inline bool operator==(const Address4& address) const;

		/**
		 * Returns whether two address objects are not identical.
		 * @param address Right address object to compare
		 * @return True, if so
		 */
		inline bool operator!=(const Address4& address) const;

		/**
		 * Casts the wrapped address number
		 * @return Wrapped address number in big-endian order
		 */
		inline operator uint32_t() const;

		/**
		 * Hash function.
		 * @param address The address for which the hash will be determined
		 * @return The hash value
		 */
		inline size_t operator()(const Address4& address) const;

	protected:

		/// Address4 number to wrap in big-endian order
		uint32_t address_ = 0u;
};

inline Address4::Address4(const uint32_t address) :
	address_(address)
{
	// nothing to do here
}

inline Address4::Address4(const uint32_t address, const OrderType orderType)
{
	switch (orderType)
	{
		case TYPE_BIG_ENDIAN:
			address_ = address;
			break;

		case TYPE_LITTLE_ENDIAN:
			address_ = (address >> 24u) | ((address >> 8u) & 0xFF00u) | ((address << 8u) & 0xFF0000u) | (address << 24u);
			break;

		case TYPE_READABLE:
			address_ = toBigEndian(address);
			break;
	}
}

inline Address4::Address4(const uint8_t sub0, const uint8_t sub1, const uint8_t sub2, const uint8_t sub3) :
	address_(uint32_t(sub0) | (uint32_t(sub1) << 8u) | (uint32_t(sub2) << 16u) | (uint32_t(sub3) << 24u))
{
	// nothing to do here
}

inline unsigned int Address4::littleEndian() const
{
	return (address_ >> 24u) | ((address_ >> 8u) & 0xFF00u) | ((address_ << 8u) & 0xFF0000u) | (address_ << 24u);
}

inline bool Address4::isValid() const
{
	return !isNull();
}

inline bool Address4::isNull() const
{
	return address_ == 0u;
}

inline Address4 Address4::localHost()
{
	return Address4(127u, 0u, 0u, 1u);
}

inline bool Address4::operator==(const Address4& address) const
{
	return address_ == address.address_;
}

inline bool Address4::operator!=(const Address4& address) const
{
	return address_ != address.address_;
}

inline Address4::operator uint32_t() const
{
	return address_;
}

inline size_t Address4::operator()(const Address4& address) const
{
	return size_t(address.address_);
}

}

}

#endif // FACEBOOK_NETWORK_ADDRESS4_H
