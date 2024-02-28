// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_REQUEST_H
#define FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_REQUEST_H

#include "ocean/network/tigon/Tigon.h"

#include <Tigon/TigonHTTPHeader.h>
#include <Tigon/TigonService.h>
#include <Tigon/TigonSimpleCallbacks.h>

#include <future>

namespace Ocean
{

namespace Network
{

namespace Tigon
{

/**
 * This class implements a client for HTTP or GraphQL requests.
 * @ingroup networktigon
 */
class OCEAN_NETWORK_TIGON_EXPORT TigonRequest
{
	public:

		/// The default URL for all GraphQL requests.
		constexpr static const char* defaultGraphQLURL = "https://graph.facebook.com/graphql/";

	public:

		/**
		 * Definition of a pair of two strings as an HTTP header.
		 */
		typedef std::pair<std::string, std::string> Header;

		/**
		 * Definition of a vector holding header elements.
		 */
		typedef std::vector<Header> Headers;

		/**
		 * Definition of a vector holding bytes as a simple buffer.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * This class implements a container holding the relevant information for a response of a request.
		 */
		class OCEAN_NETWORK_TIGON_EXPORT TigonResponse
		{
			friend class TigonRequest;

			public:

				/**
				 * Returns whether the request was successful.
				 * @return True, if so
				 */
				inline bool succeeded() const;

				/**
				 * Returns the response code of the request, e.g., the HTTP response code.
				 * @return The request's response code
				 */
				inline uint16_t code() const;

				/**
				 * Returns the response of the request, e.g., the payload.
				 * @return The request's response
				 */
				inline const std::string& response() const;

				/**
				 * Returns the error of the request in case the request was not successful.
				 * @return The request's error
				 */
				inline const std::string& error() const;

				/**
				 * Returns the header map of the response
				 * @return The response's header map
				 */
				inline const facebook::tigon::TigonHeaderMap& headers() const;

				/**
 				 * Helper method to find a header by name in the header map
				 * @param headerName The name of the header that will be searched, must be valid
				 * @param header The returning header, will be undefined if the function returns false
				 * @return True if the header was found, otherwise false
 				 */
				inline bool findHeader(const std::string& headerName, std::string& header) const;

			protected:

				/**
				 * Creates a new invalid/failed response.
				 */
				TigonResponse();

				/**
				 * Creates a new response.
				 * @param succeeded True, if the request succeeded
				 * @param code The response code of the request, e.g., the HTTP response code
				 * @param response The response of the request, e.g., the payload, in case the request succeeded
				 * @param error The error of the request, in case the request was not successful
				 * @param headers The optional headers of the response
				 */
				TigonResponse(const bool succeeded, const uint16_t code, std::string&& response, std::string&& error, const facebook::tigon::TigonHeaderMap& headers = {});

			protected:

				/// True, if the request was successful.
				bool succeeded_;

				/// True, if the request was successful.
				uint16_t code_;

				/// The response of the request, e.g., the payload.
				std::string response_;

				/// The error of the request, in case succeeded() == false
				std::string error_;

				/// The header of the response.
				facebook::tigon::TigonHeaderMap headers_;
		};

		/**
		 * Definition of a callback function for succeeded and failed requests.
		 */
		typedef std::function<void(const TigonResponse& response)> ReponseCallbackFunction;

	protected:

		/**
		 * Definition of a custom Tigon Callbacks object to enable the usage of futures.
		 */
		class OCEAN_NETWORK_TIGON_EXPORT TigonRequestCallbacks final : public facebook::tigon::TigonSimpleCallbacks
		{
			public:

				/**
				 * Creates a new callbacks object and connects it with a given promise.
				 * @param responsePromise The existing promise to which this new callback object will be connected
				 * @param responseCallbackFunction Optional explicit callback function which is called on success or failure on the response
				 */
				TigonRequestCallbacks(const std::shared_ptr<std::promise<TigonResponse>>& responsePromise, ReponseCallbackFunction&& responseCallbackFunction = ReponseCallbackFunction());

				/**
				 * Event function in case the request succeeded.
				 * @param tigonResponse The response information
				 * @param ioBuf The buffer holding the payload of the response
				 */
				void onTigonSuccess(facebook::tigon::TigonResponse&& tigonResponse, std::unique_ptr<folly::IOBuf> ioBuf);

				/**
				 * Event function in case the request failed.
				 * @param tigonError The error information
				 */
				void onTigonFailure(const facebook::tigon::TigonError& tigonError);

			protected:

#ifdef OCEAN_DEBUG

				/**
				 * The internal respnose event function, can be used for debugging.
				 * @param tigonResponse The response information
				 */
				void onResponse(facebook::tigon::TigonResponse&& tigonResponse) noexcept override;

				/**
				 * The internal body event function, can be used for debugging.
				 * @param ioBuf The buffer containing the body
				 */
				void onBody(std::unique_ptr<folly::IOBuf> ioBuf) noexcept override;

#endif // OCEAN_DEBUG

			protected:

				/// The promise which is connected with this callback function.
				std::shared_ptr<std::promise<TigonResponse>> responsePromise_;

				/// The optional explicit callback function which is called on success or failure on the response
				ReponseCallbackFunction responseCallbackFunction_;

#ifdef OCEAN_DEBUG
				/// The code of the resonse.
				uint16_t debugResponseCode_ = uint16_t(0);
#endif
		};

	public:

		/**
		 * Executes a GraphQL request based on a given Tigon service.
		 * The query can be specified via the query's 'Persisted Query ID' which is the new intended worklow, or via the query string which is the legacy workflow.
		 * The 'Persisted Query ID' can be looked up in the GraphiQL tool, e.g., the id for the 'WhoAMI' query is '4410318805702469'.<br>
		 * When using the legacy string query, the query must be whitlisted, otherwise the query will fail.
		 * @param tigonService The Tigon service to be used for the request, must be valid
		 * @param executor The executor to be used to execute the request, must be valid
		 * @param query The persisted id or the string of the GraphQL query to execute, must not be empty
		 * @param queryIsPersistedId True, if the query contains the 'Persisted Query ID' instead of the string with the query (legacy); Using the persisted id is recommented
		 * @param parameters The parameters of the GraphQL query, must not be empty
		 * @param responseCallbackFunction Optional explicit callback function which is called on success or failure on the request, can be used in addition to the returning future
		 * @param url The URL to be used for the GraphQL request
		 * @return The resulting response, can be ingnored if a callback function is provided
		 */
		static std::future<TigonResponse> graphQLRequest(facebook::tigon::TigonService& tigonService, std::shared_ptr<folly::SequencedExecutor> executor, const std::string& query, const bool queryIsPersistedId, const std::string& parameters, ReponseCallbackFunction&& responseCallbackFunction = ReponseCallbackFunction(), const std::string& url = defaultGraphQLURL);

		/**
		 * Executes a HTTP GET or POST request based on a given Tigon service.
		 * @param tigonService The Tigon service to be used for the request, must be valid
		 * @param executor The executor to be used to execute the request, must be valid
		 * @param url The URL to be used for the HTTP request
		 * @param method The method of the request, either "GET" or "POST"
		 * @param headers The optional headers to be used for the request
		 * @param body The optional body to be used for the request
		 * @param responseCallbackFunction Optional explicit callback function which is called on success or failure on the request, can be used in addition to the returning future
		 * @return The resulting response, can be ingnored if a callback function is provided
		 */
		static std::future<TigonResponse> httpRequest(facebook::tigon::TigonService& tigonService, std::shared_ptr<folly::SequencedExecutor> executor, const std::string& url, const std::string& method = "GET", const Headers& headers = Headers(), Buffer&& body = Buffer(), ReponseCallbackFunction&& responseCallbackFunction = ReponseCallbackFunction());
};

inline bool TigonRequest::TigonResponse::succeeded() const
{
	return succeeded_;
}


inline uint16_t TigonRequest::TigonResponse::code() const
{
	return code_;
}

inline const std::string& TigonRequest::TigonResponse::response() const
{
	ocean_assert(succeeded_);

	return response_;
}

inline const std::string& TigonRequest::TigonResponse::error() const
{
	return error_;
}

inline const facebook::tigon::TigonHeaderMap& TigonRequest::TigonResponse::headers() const
{
	return headers_;
}

inline bool TigonRequest::TigonResponse::findHeader(const std::string& headerName, std::string& name) const
{
	ocean_assert(!headerName.empty());

	const std::optional<std::string> optionalHeader = ::facebook::tigon::detail::findHeaderByNameHelper(headers_, headerName);

	if (optionalHeader.has_value())
	{
		name = optionalHeader.value();

		return true;
	}

	return false;
}

} // end namespace XRNavigation

} // end namespace Ocean

} // end namespace XRPlayground

#endif // FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_REQUEST_H
