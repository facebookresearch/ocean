// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/tigon/TigonRequest.h"

#include <TigonBodyUtils/TigonBodyUtils.h>

#include <Tigon/FacebookTigonLogging.h>
#include <Tigon/TigonSimpleCallbacks.h>
#include <Tigon/TigonRequest.h>

namespace Ocean
{

namespace Network
{

namespace Tigon
{

TigonRequest::TigonResponse::TigonResponse() :
	succeeded_(false)
{
	// nothing to to here
}

TigonRequest::TigonResponse::TigonResponse(const bool succeeded, const uint16_t code, std::string&& response, std::string&& error, const facebook::tigon::TigonHeaderMap& headers) :
	succeeded_(succeeded),
	code_(code),
	response_(std::move(response)),
	error_(std::move(error)),
	headers_(headers)
{
	// nothing to to here
}

TigonRequest::TigonRequestCallbacks::TigonRequestCallbacks(const std::shared_ptr<std::promise<TigonResponse>>& responsePromise, ReponseCallbackFunction&& responseCallbackFunction) :
	facebook::tigon::TigonSimpleCallbacks(facebook::tigon::TigonSuccessCallback(std::bind(&TigonRequestCallbacks::onTigonSuccess, this, std::placeholders::_1, std::placeholders::_2)), facebook::tigon::TigonFailureCallback(std::bind(&TigonRequestCallbacks::onTigonFailure, this, std::placeholders::_1))),
	responsePromise_(std::move(responsePromise)),
	responseCallbackFunction_(std::move(responseCallbackFunction))
{
	ocean_assert(responsePromise_);
}

void TigonRequest::TigonRequestCallbacks::onTigonSuccess(facebook::tigon::TigonResponse&& tigonResponse, std::unique_ptr<folly::IOBuf> ioBuf)
{
	ocean_assert(responsePromise_);

	TigonResponse response;

	if (tigonResponse.code() >= 200 && tigonResponse.code() < 300)
	{
		ocean_assert(ioBuf);

		if (ioBuf)
		{
			response = TigonResponse(true, tigonResponse.code(), ioBuf->moveToFbString().toStdString(), std::string(), tigonResponse.headers());
		}
	}
	else
	{
		std::string error = "Error code " + String::toAString(tigonResponse.code());

		response = TigonResponse(false, tigonResponse.code(), std::string(), std::move(error));
	}

	if (responseCallbackFunction_)
	{
		responseCallbackFunction_(response);
	}

	responsePromise_->set_value(std::move(response));
}

void TigonRequest::TigonRequestCallbacks::onTigonFailure(const facebook::tigon::TigonError& tigonError)
{
	ocean_assert(responsePromise_);

	TigonResponse response;

	std::string error = tigonError.analytics().domain() + ", error code " + String::toAString(tigonError.analytics().code());

	if (!tigonError.analytics().detail().empty())
	{
		error += ", " + tigonError.analytics().detail();
	}

	ocean_assert(tigonError.analytics().code() < 65536);
	response = TigonResponse(false, uint16_t(tigonError.analytics().code()), std::string(), std::move(error));

	if (responseCallbackFunction_)
	{
		responseCallbackFunction_(response);
	}

	responsePromise_->set_value(std::move(response));
}

#ifdef OCEAN_DEBUG

void TigonRequest::TigonRequestCallbacks::onResponse(facebook::tigon::TigonResponse&& tigonResponse) noexcept
{
	// nothing to do here, can be used for debugging

	debugResponseCode_ = tigonResponse.code();

	facebook::tigon::TigonSimpleCallbacks::onResponse(std::move(tigonResponse));
}

void TigonRequest::TigonRequestCallbacks::onBody(std::unique_ptr<folly::IOBuf> ioBuf) noexcept
{
	if (ioBuf && (debugResponseCode_ < 200 || debugResponseCode_ >= 300))
	{
		const std::string value = ioBuf->moveToFbString().toStdString();

		Log::error() << "Tigon response error: " << value;
	}

	facebook::tigon::TigonSimpleCallbacks::onBody(std::move(ioBuf));
}

#endif

std::future<TigonRequest::TigonResponse> TigonRequest::graphQLRequest(facebook::tigon::TigonService& tigonService, std::shared_ptr<folly::SequencedExecutor> executor, const std::string& query, const bool queryIsPersistedId, const std::string& parameters, ReponseCallbackFunction&& responseCallbackFunction, const std::string& url)
{
	ocean_assert(!query.empty() && !parameters.empty() && !url.empty());

	if (query.empty() || parameters.empty() || url.empty())
	{
		return std::future<TigonResponse>();
	}

	std::vector<std::pair<std::string, std::string>> bodyValues;

	const std::string queryField = queryIsPersistedId ? "doc_id" : "doc";

	bodyValues.emplace_back(queryField, query);
	bodyValues.emplace_back("method", "post");
	bodyValues.emplace_back("pretty", "false");
	bodyValues.emplace_back("format", "json");
	bodyValues.emplace_back("variables", parameters);

	std::unique_ptr<std::vector<uint8_t>> escapedQueryBody = facebook::tigon::body::getEscapedQueryBody(bodyValues);

	std::unique_ptr<facebook::tigon::TigonBodyProvider> tigonBodyProvider = facebook::tigon::TigonBodyProvider::fromVector(std::move(*escapedQueryBody));

	facebook::tigon::TigonRequest tigonRequest("POST", url);

	tigonRequest.addLayerInformation(facebook::tigon::FacebookTigonLogging("Ocean GraphQL Request" /*logName*/, "Ocean" /*analyticsTag*/, "Ocean::Network::Tigon::TigonRequest" /*callerClass*/));
	tigonRequest.addHeader(facebook::tigon::body::kURLEncodedBodyHeader);

	std::shared_ptr<std::promise<TigonResponse>> responsePromise = std::make_shared<std::promise<TigonResponse>>();

	std::unique_ptr<facebook::tigon::TigonCallbacks> responseHandler = std::make_unique<TigonRequestCallbacks>(responsePromise, std::move(responseCallbackFunction));

	tigonService.sendRequest(std::move(tigonRequest), std::move(tigonBodyProvider), std::move(responseHandler), std::move(executor));

	return responsePromise->get_future();
}

std::future<TigonRequest::TigonResponse> TigonRequest::httpRequest(facebook::tigon::TigonService& tigonService, std::shared_ptr<folly::SequencedExecutor> executor, const std::string& url, const std::string& method, const Headers& headers, Buffer&& body, ReponseCallbackFunction&& responseCallbackFunction)
{
	ocean_assert(!url.empty());

	if (url.empty())
	{
		ocean_assert(false && "Invalid url!");
		return std::future<TigonRequest::TigonResponse>();
	}

	if (method != "GET" && method != "POST")
	{
		ocean_assert(false && "Invalid method!");
		return std::future<TigonRequest::TigonResponse>();
	}

	facebook::tigon::TigonRequest tigonRequest(method, url);

	for (const Header& header : headers)
	{
		tigonRequest.addHeader(header);
	}

	std::unique_ptr<facebook::tigon::TigonBodyProvider> tigonBodyProvider;

	if (!body.empty())
	{
		tigonBodyProvider = facebook::tigon::TigonBodyProvider::fromVector(std::move(body));
	}

	std::shared_ptr<std::promise<TigonResponse>> responsePromise = std::make_shared<std::promise<TigonResponse>>();

	std::unique_ptr<facebook::tigon::TigonCallbacks> responseHandler = std::make_unique<TigonRequestCallbacks>(responsePromise, std::move(responseCallbackFunction));

	tigonService.sendRequest(std::move(tigonRequest), std::move(tigonBodyProvider), std::move(responseHandler), std::move(executor));

	return responsePromise->get_future();
}

}

}

}
