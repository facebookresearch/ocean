/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_HTTPS_CLIENT_H
#define FACEBOOK_NETWORK_HTTPS_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/Port.h"

#include "ocean/base/Callback.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a HTTPS Client supporting GET and POST requests.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT HTTPSClient
{
	public:

		/**
		 * Definition of a vector holding characters.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of a vector holding strings.
		 */
		typedef std::vector<std::string> Strings;

		/**
		 * Callback for receiving progress information when performing a HTTP request.
		 * The first parameter represents how many Bytes of the payload have been received so far, with range [0, infinity)
		 * The second parameter represents the total size of the payload in bytes, with range [0, infinity). If the total size is unknown this parameter is 0.
		 */
		typedef Callback<void, size_t, size_t> ProgressCallback;

	protected:

		/**
		 * Forward declaration.
		 */
		class CurlSessionData;

	public:

		/**
		 * Function to executes a HTTPS GET (site/file download) request.
		 * @param url The URL of the HTTPS site which is requested, beginning with "HTTPS://"
		 * @param data The resulting request data
		 * @param port The port of the HTTPS server
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @param abort Optional flag that may be set to True by another thread to abort the request
		 * @param progressCallback Optional callback for receiving progress information
		 * @return True, if succeeded
		 */
		static bool httpsGetRequest(const std::string& url, Buffer& data, const Port& port = Port(443, Port::TYPE_READABLE), const double timeout = 5.0, bool* abort = nullptr, const ProgressCallback& progressCallback = ProgressCallback());

		/**
		 * Function to executes a HTTPS POST (site/file upload) request.
		 * @param url The URL of the HTTPS site which is requested, beginning with "HTTPS://"
		 * @param requestData The buffer of the request data
		 * @param requestDataSize The number of bytes of the request data
		 * @param data The resulting response data
		 * @param port The port of the HTTPS server
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @param additionalHeaders Optional additional HTTP headers
		 * @return True, if succeeded
		 */
		static bool httpsPostRequest(const std::string& url, const uint8_t* requestData, const size_t requestDataSize, Buffer& data, const Port& port = Port(443, Port::TYPE_READABLE), const double timeout = 5.0, const Strings& additionalHeaders = Strings());

		/**
		 * Converts a URL to a URI.
		 * @param url The URL to convert, e.g., 'https://www.website.com/index.html"
		 * @param protocol The resulting protocol, will be 'https' if successful
		 * @param host The resulting host without ending '/'
		 * @param uri The resulting URI, can be empty if the URL does not include an URI
		 * @return True, if succeeded
		 */
		static bool url2uri(const std::string& url, std::string& protocol, std::string& host, std::string& uri);

	protected:

#if defined(__APPLE__)

		/**
		 * Function to executes a HTTPS GET (site/file download) request (specialized function for Apple platforms).
		 * @sa httpsGetRequest()
		 * @param url The URL of the HTTPS site which is requested, beginning with "HTTPS://"
		 * @param data The resulting request data
		 * @param port The port of the HTTPS server
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @param abort Optional flag that may be set to True by another thread to abort the request
		 * @param progressCallback Optional callback for receiving progress information
		 * @return True, if succeeded
		 */
		static bool httpsGetRequestApple(const std::string& url, Buffer& data, const Port& port, const double timeout, bool* abort, const ProgressCallback& progressCallback);

		/**
		 * Function to executes a HTTPS POST (site/file upload) request (specialized function for Apple platforms).
		 * @sa httpsPostRequest()
		 * @param url The URL of the HTTPS site which is requested, beginning with "HTTPS://"
		 * @param requestData The buffer of the request data
		 * @param requestDataSize The number of bytes of the request data
		 * @param data The resulting response data
		 * @param port The port of the HTTPS server
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @param additionalHeaders Optional additional HTTP headers
		 * @return True, if succeeded
		 */
		static bool httpsPostRequestApple(const std::string& url, const uint8_t* requestData, const size_t requestDataSize, Buffer& data, const Port& port, const double timeout, const Strings& additionalHeaders);

#endif // defined(__APPLE__)

};

}

}

#endif // FACEBOOK_NETWORK_HTTPS_CLIENT_H
