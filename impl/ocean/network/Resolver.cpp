/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/Resolver.h"
#include "ocean/network/NetworkResource.h"
#include "ocean/network/Socket.h"

#ifdef _WINDOWS
	#include <ws2tcpip.h>
#else
	#include <netinet/in.h>
	#include <netdb.h>
#endif

#ifdef __APPLE__
	#include <ifaddrs.h>
#endif

namespace Ocean
{

namespace Network
{

Resolver::Resolver()
{
	// nothing to do here
}

Resolver::Addresses4 Resolver::localAddresses() const
{
	const ScopedLock scopedLock(lock_);

	Addresses4 result = Addresses4(explicitLocalAddresses_.cbegin(), explicitLocalAddresses_.cend());

#ifdef __APPLE__

	ifaddrs* interfaces = nullptr;
	if (getifaddrs(&interfaces) == 0)
	{
		ifaddrs* nextInterface = interfaces;

		while (nextInterface != nullptr)
		{
			if (nextInterface->ifa_addr->sa_family == AF_INET)
			{
				const Address4 address(((struct sockaddr_in*)(nextInterface->ifa_addr))->sin_addr.s_addr);

				bool skip = false;

				if (address == Address4::localHost())
				{
					skip = true;
				}

				if (nextInterface->ifa_name == nullptr || std::string(nextInterface->ifa_name) != "en0")
				{
					skip = true;
				}

				if (!skip)
				{
					result.emplace_back(address);
				}
			}

			nextInterface = nextInterface->ifa_next;
		}
	}

	freeifaddrs(interfaces);

#endif // __APPLE__

	return result;
}

void Resolver::addLocalAddress(const Address4& localAddress)
{
	ocean_assert(localAddress.isValid());

	const ScopedLock scopedLock(lock_);

	explicitLocalAddresses_.insert(localAddress);
}

Address4 Resolver::resolveFirstIp4(const std::string& host)
{
	const NetworkResource networkResource;

	addrinfo* addressInfo = nullptr;
	Address4 result;

	if (getaddrinfo(host.c_str(), nullptr, nullptr, &addressInfo) == 0)
	{
		addrinfo* nextAddressInfo = addressInfo;

		while (nextAddressInfo != nullptr)
		{
			if (nextAddressInfo->ai_family == AF_INET)
			{
				ocean_assert(nextAddressInfo->ai_addrlen >= sizeof(Address4));

				sockaddr_in* sockAddress = (sockaddr_in*)nextAddressInfo->ai_addr;
				result = *(Address4*)(&sockAddress->sin_addr);

				break;
			}

			nextAddressInfo = nextAddressInfo->ai_next;
		}

		freeaddrinfo(addressInfo);
	}

	return result;
}

Resolver::Addresses4 Resolver::resolveIp4(const std::string& host)
{
	const NetworkResource networkResource;

	addrinfo* addressInfo = nullptr;
	Addresses4 result;

	if (getaddrinfo(host.c_str(), nullptr, nullptr, &addressInfo) == 0)
	{
		addrinfo* nextAddressInfo = addressInfo;

		while (nextAddressInfo != nullptr)
		{
			if (nextAddressInfo->ai_family == AF_INET)
			{
				ocean_assert(nextAddressInfo->ai_addrlen >= sizeof(Address4));
				result.push_back(Address4(*(Address4*)nextAddressInfo->ai_addr));
			}

			nextAddressInfo = nextAddressInfo->ai_next;
		}

		freeaddrinfo(addressInfo);
	}

	return result;
}

std::string Resolver::resolve(const Address4& address)
{
	const NetworkResource networkResource;

	sockaddr_in addressInfo;

	addressInfo.sin_family = AF_INET;
	addressInfo.sin_addr.s_addr = address;
	addressInfo.sin_port = 0;

	char host[1025];
	char service[1025];

	if (getnameinfo((sockaddr*)&addressInfo, sizeof(addressInfo), host, 1024, service, 1024, 0) != 0)
	{
		return std::string();
	}

	ocean_assert(strlen(host) <= 1024);

	return std::string(host);
}

}

}
