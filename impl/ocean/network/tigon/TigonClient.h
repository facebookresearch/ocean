// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_CLIENT_H
#define FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_CLIENT_H

#include "ocean/network/tigon/Tigon.h"
#include "ocean/network/tigon/TigonRequest.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <Tigon/TigonService.h>

namespace Ocean
{

namespace Network
{

namespace Tigon
{

/**
 * This class implements a client for HTTP and GraphQL requests.
 * @ingroup networktigon
 */
class OCEAN_NETWORK_TIGON_EXPORT TigonClient final :
	public TigonRequest,
	public Singleton<TigonClient>
{
	friend class Singleton<TigonClient>;

	public:

		/**
		 * Executes a GraphQL request using the client's Tigon service.
		 * The query can be specified via the query's 'Persisted Query ID' which is the new intended worklow, or via the query string which is the legacy workflow.
		 * The 'Persisted Query ID' can be looked up in the GraphiQL tool.<br>
		 * When using the legacy string query, the query must be whitlisted, otherwise the query will fail.
		 * @param query The persisted id or the string of the GraphQL query to execute, must not be empty
		 * @param queryIsPersistedId True, if the query contains the 'Persisted Query ID' instead of the string with the query (legacy); Using the persisted id is recommented
		 * @param parameters The parameters of the GraphQL query, must not be empty
		 * @param url The URL to be used for the GraphQL request
		 * @return The resulting response
		 */
		std::future<TigonResponse> graphQLRequest(const std::string& query, const bool queryIsPersistedId, const std::string& parameters, const std::string& url = defaultGraphQLURL);

		/**
		 * Executes a GraphQL request using the client's Tigon service.
		 * The query can be specified via the query's 'Persisted Query ID' which is the new intended worklow, or via the query string which is the legacy workflow.
		 * The 'Persisted Query ID' can be looked up in the GraphiQL tool.<br>
		 * When using the legacy string query, the query must be whitlisted, otherwise the query will fail.
		 * @param query The persisted id or the string of the GraphQL query to execute, must not be empty
		 * @param queryIsPersistedId True, if the query contains the 'Persisted Query ID' instead of the string with the query (legacy); Using the persisted id is recommented
		 * @param parameters The parameters of the GraphQL query, must not be empty
		 * @param responseCallbackFunction Callback function which is called on success or failure on the request, can be used in addition to the returning future
		 * @param url The URL to be used for the GraphQL request
		 * @return True, if the requested was submitted successfully; False, otherwise
		 */
		bool graphQLRequest(const std::string& query, const bool queryIsPersistedId, const std::string& parameters, ReponseCallbackFunction&& responseCallbackFunction, const std::string& url = defaultGraphQLURL);

		/**
		 * Executes a HTTP GET or POST request based on a given Tigon service.
		 * @param url The URL to be used for the HTTP request
		 * @param method The method of the request, either "GET" or "POST"
		 * @param headers The optional headers to be used for the request
		 * @param body The optional body to be used for the request
		 * @return The resulting response
		 */
		std::future<TigonResponse> httpRequest(const std::string& url, const std::string& method = "GET", const Headers& headers = Headers(), Buffer&& body = Buffer());

		/**
		 * Executes a HTTP GET or POST request based on a given Tigon service.
		 * @param url The URL to be used for the HTTP request
		 * @param responseCallbackFunction Optional explicit callback function which is called on success or failure on the request, can be used in addition to the returning future
		 * @param method The method of the request, either "GET" or "POST"
		 * @param headers The optional headers to be used for the request
		 * @param body The optional body to be used for the request
		 * @return True, if the requested was submitted successfully; False, otherwise
		 */
		bool httpRequest(const std::string& url, ReponseCallbackFunction&& responseCallbackFunction, const std::string& method = "GET", const Headers& headers = Headers(), Buffer&& body = Buffer());

		/**
		 * Returns the Tigon service of this client, the service will be created if the client does not yet have a service.
		 * @return The client's Tigon service, may be nullptr if the service could not be created
		 */
		std::shared_ptr<facebook::tigon::TigonService> clientTigonService();

		/**
		 * Returns the executor of this client, the executor will be created if the client does not yet have an executor.
		 * @return The client's executor
		 */
		std::shared_ptr<folly::SequencedExecutor> clientExecutor();

		/**
		 * Determines the name of ther user by applying the 'WhoAmI' GraphQL query.
		 * This function is mainly a basic example how to use GraphQL requests.<br>
		 * The function is blocking until the query response has returned.
		 * @param userName The resulting name of the user which is logged into the session
		 * @param url The url to be used for the GraphQL request
		 * @param errorMessage Optional resulting error message; nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool determineUserName(std::string& userName, const std::string& url = defaultGraphQLURL, std::string* errorMessage = nullptr);

		/**
		 * Creates a new Tigon service which can be used outisde of this class.
		 * The function extracts the user login information from Platform::Meta::Login.
		 * @return The resulting Tigon service, nullptr if the client could not be created
		 * @see Platform::Meta::Login.
		 */
		static std::shared_ptr<facebook::tigon::TigonService> createTigonService();

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Explicitly sets the Tigon service.
		 * This function is intended for platforms on which the service cannot be created out of native code e.g., Android
		 * @param tigonService The Tigon service to be set or to update, must be valid
		 */
		void setTigonService(std::shared_ptr<facebook::tigon::TigonService> tigonService);

		/**
		 * Explicitely sets the executor.
		 * This function is intended for platforms on which the executor may be created out of native code e.g., Android
		 * @param executor The executor to be set or to update, must be valid
		 */
		void setExecutor(std::shared_ptr<folly::SequencedExecutor> executor);

#endif // OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Explicitely releases this Tigon Client.
		 * This function can be called before system shutdown to ensure that all resources can be cleaned up gracefully.
		 */
		void release();

	protected:

		/**
		 * Creates a new client.
		 */
		TigonClient();

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Creates a new Tigon service which can be used outisde of this class.
		 * The function extracts the user login information from Platform::Meta::Login.
		 * @return The resulting Tigon service, nullptr if the client could not be created
		 * @see Platform::Meta::Login.
		 */
		static std::shared_ptr<facebook::tigon::TigonService> createTigonServiceAppleIOS();

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	protected:

		/// The tigon service to be used for all queries.
		std::shared_ptr<facebook::tigon::TigonService> tigonService_;

		/// The executor to be used to execute the queries.
		std::shared_ptr<folly::SequencedExecutor> executor_;

		/// The lock of this client.
		mutable Lock lock_;
};

}

}

}

#endif // FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_CLIENT_H
