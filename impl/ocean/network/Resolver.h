/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_RESOLVER_H
#define FACEBOOK_NETWORK_RESOLVER_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements an address resolver.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Resolver : public Singleton<Resolver>
{
	friend class Singleton<Resolver>;

	public:

		/**
		 * Definition of a vector holding addresses.
		 */
		typedef std::vector<Address4> Addresses4;

		/**
		 * Definition of an unordered set holding addresses.
		 */
		typedef std::unordered_set<Address4, Address4> AddressSet4;

	public:

		/**
		 * Returns all current local addresses.
		 * @return The local addresses of the system, empty if unknown
		 */
		Addresses4 localAddresses() const;

		/**
		 * Adds an explicit local address.
		 * @param localAddress The explicit local address to be set, must be valid
		 */
		void addLocalAddress(const Address4& localAddress);

		/**
		 * Resolves a given host name to it's first found ip4 address.
		 * @param host Name of the host to resolve
		 * @return Resolved address, a zero address is returned if the host could not be resolved
		 */
		static Address4 resolveFirstIp4(const std::string& host);

		/**
		 * Resolves a given host name to all possible ip4 addresses.
		 * @param host Name of the host to resolve
		 * @return Resolved addresses
		 */
		static Addresses4 resolveIp4(const std::string& host);

		/**
		 * Resolves a given address to it's host name.
		 * If the address could not be resolved an empty string is returned.
		 * @param address Address4 to resolve
		 * @return Resolved host name
		 */
		static std::string resolve(const Address4& address);

	protected:

		/**
		 * Creates a new resolver object.
		 */
		Resolver();

	protected:

		/// The local addresses explicitly set.
		AddressSet4 explicitLocalAddresses_;

		/// The resolver's lock.
		mutable Lock lock_;
};

}

}

#endif // FACEBOOK_NETWORK_RESOLVER_H
