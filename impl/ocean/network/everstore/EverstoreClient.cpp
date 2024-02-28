// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/everstore/EverstoreClient.h"

#include <everstore/client/cpp/Client.h>
#include <everstore/client/cpp/ClientBuilder.h>

namespace Ocean
{

namespace Network
{

bool EverstoreClient::download(const std::string& handle, EverstoreClient::Buffer& buffer, const std::string& context)
{
	buffer.clear();

	std::shared_ptr<facebook::everstore::client::Client> client = facebook::everstore::client::ClientBuilder().build(folly::getEventBase());

	if (client)
	{
		try
		{
			std::unique_ptr<folly::IOBuf> result = client->buildReadRequest().withNumRetries(5).withTotalTimeout(std::chrono::milliseconds(60000)).read(context, handle).get();

			if (result)
			{
				result->coalesce();

				if (result->empty() == false)
				{
					buffer = Buffer(result->data(), result->data() + result->length());
				}
			}

		}
		catch (const facebook::everstore::client::ClientException& exception)
		{
			Log::error() << exception.what();
			return false;
		}
	}

	return buffer.empty() == false;
}

} // namespace Network

} // namespace Ocean
