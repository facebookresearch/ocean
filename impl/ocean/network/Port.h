/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PORT_H
#define FACEBOOK_NETWORK_PORT_H

#include "ocean/network/Network.h"
#include "ocean/network/Data.h"

namespace Ocean
{

namespace Network
{

/**
 * This class wraps a port number with 16 bits.<br>
 * Internally, the port number is stored in big-endian order.
 * @ingroup network
 */
class Port : public Data
{
	public:

		/**
		 * Creates a new port object with zero as default value.
		 */
		Port() = default;

		/**
		 * Creates a new port object.
		 * @param port The port number to be wrapped in big-endian order
		 */
		explicit inline Port(const uint16_t port);

		/**
		 * Creates a new port object by a given port number in big-endian, little-endian order or local platform order.
		 * @param port The port number to be wrapped
		 * @param orderType Type of the order
		 */
		inline Port(const uint16_t port, const OrderType orderType);

		/**
		 * Returns the port number in little-endian order.
		 * @return Port number in little-endian order
		 */
		inline uint16_t littleEndian() const;

		/**
		 * Returns the port number in a readable order.
		 * This function is platform specific.
		 */
		inline uint16_t readable() const;

		/**
		 * Returns whether this port hold a non-zero value.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this port holds a zero value.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether two port number are identical.
		 * @param port Right port to compare
		 * @return True, if so
		 */
		inline bool operator==(const Port& port) const;

		/**
		 * Returns whether two port number are not identical.
		 * @param port Right port to compare
		 * @return True, if so
		 */
		inline bool operator!=(const Port& port) const;

		/**
		 * Casts the wrapped port number
		 * @return Wrapped port number in big-endian order
		 */
		inline operator uint16_t() const;

	protected:

		/// Port number to wrap in big-endian order.
		uint16_t port_ = 0u;
};

inline Port::Port(const uint16_t port) :
	port_(port)
{
	// nothing to do here
}

inline Port::Port(const uint16_t port, const OrderType orderType)
{
	switch (orderType)
	{
		case TYPE_BIG_ENDIAN:
			port_ = port;
			break;

		case TYPE_LITTLE_ENDIAN:
			port_ = uint16_t((port >> 8u) | (port << 8u));
			break;

		case TYPE_READABLE:
			port_ = toBigEndian(port);
			break;
	}
}

inline uint16_t Port::littleEndian() const
{
	return uint16_t((port_ >> 8u) | (port_ << 8u));
}

inline uint16_t Port::readable() const
{
	return fromBigEndian(port_);
}

inline bool Port::isValid() const
{
	return !isNull();
}

inline bool Port::isNull() const
{
	return port_ == 0u;
}

inline bool Port::operator==(const Port& port) const
{
	return port_ == port.port_;
}

inline bool Port::operator!=(const Port& port) const
{
	return port_ != port.port_;
}

inline Port::operator uint16_t() const
{
	return port_;
}

}

}

#endif
