// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_EVERSTORE_EVERSTORE_CLIENT_H
#define FACEBOOK_NETWORK_EVERSTORE_EVERSTORE_CLIENT_H

#include "ocean/network/everstore/Everstore.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements an Everstore client supporting read (get) and write (put) requests
 * @ingroup networkeverstore
 */
class OCEAN_NETWORK_EVERSTORE_EXPORT EverstoreClient
{
	public:

		/**
		 * Definition of a vector holding data
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Read data from Everstore
		 * @param handle Everstore handle of the asset to download
		 * @param buffer Location where the downloaded data will be stored, will be empty on failure
		 * @param context The context id of the asset to download
		 * @return True, if succeeded otherwise false
		 */
		static bool download(const std::string& handle, EverstoreClient::Buffer& buffer, const std::string& context = contextId());

		/**
		 * Context ID required by the Everstore C++ API
		 * @return The context ID
		 */
		static inline std::string contextId();
};

inline std::string EverstoreClient::contextId()
{
	return std::string("compphoto/ocean");
}

} // namespace Network

} // namespace Ocean

#endif // FACEBOOK_NETWORK_EVERSTORE_EVERSTORE_CLIENT_H
