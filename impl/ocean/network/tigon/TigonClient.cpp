// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/io/JSONConfig.h"

#include <AsyncExecutor/InlineSerialExecutor.h>

namespace Ocean
{

namespace Network
{

namespace Tigon
{

TigonClient::TigonClient()
{
	// nothing to do here
}

std::future<TigonRequest::TigonResponse> TigonClient::graphQLRequest(const std::string& query, const bool queryIsPersistedId, const std::string& parameters, const std::string& url)
{
	if (query.empty() || parameters.empty() || url.empty())
	{
		ocean_assert(false && "Invalid input parameters!");
		return std::future<TigonRequest::TigonResponse>();
	}

	const std::shared_ptr<facebook::tigon::TigonService> tigonService(clientTigonService());

	std::shared_ptr<folly::SequencedExecutor> executor(clientExecutor());

	if (!tigonService || !executor)
	{
		return std::future<TigonRequest::TigonResponse>();
	}

	return TigonRequest::graphQLRequest(*tigonService, std::move(executor), query, queryIsPersistedId, parameters, ReponseCallbackFunction(), url);
}

bool TigonClient::graphQLRequest(const std::string& query, const bool queryIsPersistedId, const std::string& parameters, TigonRequest::ReponseCallbackFunction&& responseCallbackFunction, const std::string& url)
{
	if (query.empty() || parameters.empty() || !responseCallbackFunction || url.empty())
	{
		ocean_assert(false && "Invalid input parameters!");
		return false;
	}

	const std::shared_ptr<facebook::tigon::TigonService> tigonService(clientTigonService());

	std::shared_ptr<folly::SequencedExecutor> executor(clientExecutor());

	if (!tigonService || !tigonService)
	{
		return false;
	}

	return TigonRequest::graphQLRequest(*tigonService, std::move(executor), query, queryIsPersistedId, parameters, std::move(responseCallbackFunction), url).valid();
}

std::future<TigonRequest::TigonResponse> TigonClient::httpRequest(const std::string& url, const std::string& method, const Headers& headers, std::vector<uint8_t>&& body)
{
	if (url.empty() || (method != "GET" && method != "POST"))
	{
		ocean_assert(false && "Invalid input parameters!");
		return std::future<TigonRequest::TigonResponse>();
	}

	const std::shared_ptr<facebook::tigon::TigonService> tigonService(clientTigonService());

	std::shared_ptr<folly::SequencedExecutor> executor(clientExecutor());

	if (!tigonService || !executor)
	{
		return std::future<TigonRequest::TigonResponse>();
	}

	return TigonRequest::httpRequest(*tigonService, std::move(executor), url, method, headers, std::move(body), ReponseCallbackFunction());
}

bool TigonClient::httpRequest(const std::string& url, ReponseCallbackFunction&& responseCallbackFunction, const std::string& method, const Headers& headers, std::vector<uint8_t>&& body)
{
	if (url.empty() || (method != "GET" && method != "POST"))
	{
		ocean_assert(false && "Invalid input parameters!");
		return false;
	}

	const std::shared_ptr<facebook::tigon::TigonService> tigonService(clientTigonService());

	std::shared_ptr<folly::SequencedExecutor> executor(clientExecutor());

	if (!tigonService || !tigonService)
	{
		return false;
	}

	return TigonRequest::httpRequest(*tigonService, std::move(executor), url, method, headers, std::move(body), std::move(responseCallbackFunction)).valid();
}

std::shared_ptr<facebook::tigon::TigonService> TigonClient::clientTigonService()
{
	const ScopedLock scopedLock(lock_);

	if (!tigonService_)
	{
		tigonService_ = createTigonService();
	}

	return tigonService_;
}

std::shared_ptr<folly::SequencedExecutor> TigonClient::clientExecutor()
{
	const ScopedLock scopedLock(lock_);

	if (!executor_)
	{
		executor_ = std::make_shared<facebook::mobile::xplat::executor::InlineSerialExecutor>();
	}

	return executor_;
}

bool TigonClient::determineUserName(std::string& userName, const std::string& url, std::string* errorMessage)
{
#if 1
	const std::string query = "4410318805702469";
	constexpr bool queryIsPersistId = true;
#else
	const std::string query = "query WhoAmI { viewer { actor { id, name } } }";
	constexpr bool queryIsPersistId = false;
#endif

	const std::string parameters = "{}";

	std::future<TigonResponse> futureResponse = Network::Tigon::TigonClient::get().graphQLRequest(query, queryIsPersistId, parameters, url);

	if (!futureResponse.valid())
	{
		return false;
	}

	TigonResponse response = futureResponse.get();

	if (!response.succeeded() || response.response().empty())
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = response.error();
		}

		return false;
	}

	IO::JSONConfig config(std::shared_ptr<std::istream>(new std::istringstream(response.response())));

	if (config.exist("data"))
	{
		std::string name = config["data"]["viewer"]["actor"]["name"](std::string());

		if (!name.empty())
		{
			userName = std::move(name);
			return true;
		}
	}

	if (errorMessage != nullptr)
	{
		*errorMessage = "Name not contained in the response";
	}

	return false;
}

std::shared_ptr<facebook::tigon::TigonService> TigonClient::createTigonService()
{
#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	return createTigonServiceAppleIOS();
#endif

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	ocean_assert(false && "This Tigon service needs to be created in Java and provided via the native interface");
#endif

	ocean_assert(false && "This platform is not supported");

	return nullptr;
}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

void TigonClient::setTigonService(std::shared_ptr<facebook::tigon::TigonService> tigonService)
{
	const ScopedLock scopedLock(lock_);

	tigonService_ = std::move(tigonService);
}

void TigonClient::setExecutor(std::shared_ptr<folly::SequencedExecutor> executor)
{
	const ScopedLock scopedLock(lock_);

	executor_ = std::move(executor);
}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

void TigonClient::release()
{
	const ScopedLock scopedLock(lock_);

	tigonService_ = nullptr;

	executor_ = nullptr;
}

}

}

}
