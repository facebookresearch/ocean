/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_HTTP_CLIENT_H
#define FACEBOOK_NETWORK_HTTP_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/BufferQueue.h"
#include "ocean/network/TCPClient.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a basic http client.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT HTTPClient
{
	public:

		/**
		 * Definition of individual protocol versions.
		 */
		enum ProtocolVersion
		{
			/// The HTTP 1.0 version.
			PV_HTTP_1_0,
			/// The HTTP 1.1 version.
			PV_HTTP_1_1
		};

		/**
		 * This class implements a http header wrapper.
		 */
		class HTTPHeader
		{
			friend class HTTPClient;

			public:

				/**
				 * Definition of individual http reply codes.
				 */
				enum ReplyCode
				{
					/// Invalid reply code.
					RC_INVALID = 0,
					/// Continue reply code.
					RC_CONTINUE = 100,
					/// OK reply code.
					RC_OK = 200,
					/// Created reply code.
					RC_CREATED = 201,
					/// Accepted reply code.
					RC_ACCEPTED = 202,
					/// This and all future requests should be directed to the given URI
					RC_MOVED_PERMANENTLY = 301,
					/// Bad request reply code.
					RC_BAD_REQUEST = 400,
					/// Unauthorized reply code.
					RC_UNAUTHORIZED = 401,
					/// Payment request reply code.
					RC_PAYMENT_REQUIRED = 402,
					/// Forbidden reply code.
					RC_FORBIDDEN = 403,
					/// Not found reply code.
					RC_NOT_FOUND = 404,
					/// Method not allowed reply code.
					RC_METHOD_NOT_ALLOWED = 405,
					/// Not acceptable reply code.
					RC_NOT_ACCEPTABLE = 406
				};

				/**
				 * Definition of individual content encoding types.
				 */
				enum EncodingType
				{
					/// A standard encoding.
					ET_STANDARD,
					/// A GZIP encoding.
					ET_GZIP
				};

				/**
				 * Definition of a vector holding header lines.
				 */
				typedef std::vector<std::string> Lines;

			public:

				/**
				 * Creates a new header object.
				 */
				HTTPHeader() = default;

				/**
				 * Returns the reply code of this header.
				 * @return The reply code
				 */
				inline ReplyCode code() const;

				/**
				 * Returns the length of the header in bytes.
				 * @return The header length in byte
				 */
				inline size_t length() const;

				/**
				 * Returns the length of the content in bytes.
				 * @return The content length in byte
				 */
				inline size_t contentLength() const;

				/**
				 * Returns whether the transfer encoding is chunked.
				 * @return True, if so
				 */
				inline bool transferEncodingChunked() const;

				/**
				 * Returns the content encoding type.
				 * @return The content encoding type
				 */
				inline EncodingType encodingType() const;

				/**
				 * Returns the location value.
				 * @return The location value
				 */
				inline std::string& location();

				/**
				 * Returns the lines of this header.
				 * @return The header's lines
				 */
				inline const Lines& lines() const;

				/**
				 * Sets the reply code of this header.
				 * @param code The reply code to set
				 */
				inline void setCode(const ReplyCode code);

				/**
				 * Sets the version of the http header.
				 * @param version The version string to set
				 */
				inline void setVersion(const std::string& version);

				/**
				 * Sets the length of the header in bytes.
				 * @param length The length in byte
				 */
				inline void setLength(const size_t length);

				/**
				 * Sets the length of the content in bytes.
				 * @param length The content length in byte
				 */
				inline void setContentLength(const size_t length);

				/**
				 * Sets the transfer encoding chunked state.
				 * @param state True, if the transfer encoding is chunked
				 */
				inline void setTransferEncodingChunked(const bool state);

				/**
				 * Sets the content encoding type.
				 * @param type The content encoding type
				 */
				inline void setContentEncodingType(const EncodingType type);

				/**
				 * Sets the location value.
				 * @param location The location to set
				 */
				inline void setLocation(const std::string& location);

				/**
				 * Sets the lines of this header.
				 * @param lines The lines to move
				 */
				inline void setLines(Lines&& lines);

				/**
				 * Returns whether the code of this header is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Parses a header line and updates the information in a given header object.
				 * @param line The line of an header
				 * @param header The header object receiving the parsed information
				 * @return True, if succeeded
				 */
				static bool parseHeaderLine(const std::string& line, HTTPHeader& header);

			protected:

				/// The replay code the header.
				ReplyCode code_ = RC_INVALID;

				/// The version of the header.
				std::string version_;

				/// The length of the header's content.
				size_t contentLength_ = 0;

				/// The length of the header.
				size_t length_ = 0;

				/// True, if the transfer-encoding is chunked
				bool transferEncodingChunked_ = false;

				/// The content encoding type.
				EncodingType encodingType_ = ET_STANDARD;

				/// The location for e.g., a redirect
				std::string location_;

				/// The lines of this header.
				Lines lines_;
		};

		/**
		 * Definition of a vector holding characters.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Callback for receiving progress information when performing a HTTP request.
		 * The first parameter represents how many Bytes of the payload have been received so far, with range [0, infinity)
		 * The second parameter represents the total size of the payload in bytes, with range [0, infinity). If the total size is unknown this parameter is 0.
		 */
		typedef Callback<void, size_t, size_t> ProgressCallback;

	public:

		/**
		 * Creates a new HTTP client object.
		 * @param host The URL of the HTTP server
		 * @param port The port of the HTTP server
		 */
		HTTPClient(const std::string& host, const Port& port = Port(80, Port::TYPE_READABLE));

		/**
		 * Destructs a HTTP client object.
		 */
		~HTTPClient();

		/**
		 * Connects the client with the HTTP server.
		 * @return True, if succeeded
		 */
		bool connect();

		/**
		 * (Re-)connects the client with the HTTP server.
		 * @param host The URL of the HTTP server
		 * @param port The port of the HTTP server
		 * @return True, if succeeded
		 */
		bool connect(const std::string& host, const Port& port = Port(80, Port::TYPE_READABLE));

		/**
		 * Invokes a HEAD request.
		 * @param uri The universal resource identifier for the HEAD request (not including the first '/' between host and URI)
		 * @param header The resulting header
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @return True, if succeeded
		 */
		bool invokeHeadRequest(const std::string& uri, HTTPHeader& header, const double timeout = 5.0);

		/**
		 * Invokes a GET request.
		 * @param uri The universal resource identifier for the HEAD request (not including the first '/' between host and URI)
		 * @param data The resulting response data
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @param urlRedirection Optional resulting URL if the request has to be redirected
		 * @param replyCode Optional reply code of the GET request
		 * @param abort Optional flag that may be set to true by another thread to abort the request
		 * @param progressCallback Optional callback for receiving progress information
		 * @return True, if succeeded;
		 */
		bool invokeGetRequest(const std::string& uri, Buffer& data, const double timeout = 5.0, std::string* urlRedirection = nullptr, HTTPHeader::ReplyCode* replyCode = nullptr, bool* abort = nullptr, const ProgressCallback& progressCallback = ProgressCallback());

		/**
		 * Helper function to executes an HTTP site/file request.
		 * @param url The URL of the HTTP site which is requested, beginning with "HTTP://"
		 * @param data The resulting request data
		 * @param port The port of the HTTP server
		 * @param timeout The timeout this function waits for the server's response, with range (0, infinity)
		 * @param allowRedirect True, to allow an automatic redirect of the specified URL
		 * @param redirectedURL Optional resulting URL if the request has been redirected
		 * @param replyCode Optional reply code of the GET request
		 * @param abort Optional flag that may be set to true by another thread to abort the request
		 * @param progressCallback Optional callback for receiving progress information
		 * @return True, if succeeded
		 */
		static bool httpGetRequest(const std::string& url, Buffer& data, const Port& port = Port(80, Port::TYPE_READABLE), const double timeout = 5.0, bool allowRedirect = true, std::string* redirectedURL = nullptr, HTTPHeader::ReplyCode* replyCode = nullptr, bool* abort = nullptr, const ProgressCallback& progressCallback = ProgressCallback());

	protected:

		bool sendRequest(const std::string& uri, const std::string& requestMethod);

		/**
		 * The response event function.
		 * @param data The response data
		 * @param size The size of the data in bytes
		 */
		void onResponse(const void* data, const size_t size);

		/**
		 * Converts a URL to a URI.
		 * @param url The URL to convert, e.g., 'http://www.website.com/index.html"
		 * @param protocol The resulting protocol, will be 'http' if successful
		 * @param host The resulting host without ending '/'
		 * @param uri The resulting URI, can be empty if the URL does not include an URI
		 * @return True, if succeeded
		 */
		static bool url2uri(const std::string& url, std::string& protocol, std::string& host, std::string& uri);

		/**
		 * Parses a header from a given header string.
		 * @param data The header of header as string
		 * @param size The size of the given header string in bytes
		 * @param header The resulting parsed header
		 * @return True, if succeeded
		 */
		static bool parseHeader(const char* data, const size_t size, HTTPHeader& header);

		/**
		 * Appends response data from a current request to a given buffer.
		 * @param header The header of the request
		 * @param buffer The target buffer receiving the data
		 * @param bufferPosition The current position in the target buffer
		 * @param payload The source data
		 * @param payloadSize The sizeof the source data
		 * @param pendingChunkSize Input and output size of the current chunk
		 * @return True, if the given payload was the last payload for the target buffer
		 */
		static bool appendData(const HTTPHeader& header, Buffer& buffer, size_t& bufferPosition, const char* payload, size_t payloadSize, size_t& pendingChunkSize);

		/**
		 * Returns one line from a data buffer.
		 * @param data The data buffer
		 * @param size The size of the data buffer in bytes
		 * @param offset The offset which needs to be applied to skip the line
		 * @return The resulting line
		 */
		static std::string line(const char* data, const size_t size, size_t& offset);

		/**
		 * Convert a hex string to a number.
		 * @param hex The hex string to be converted either lower case or upper case, without '0x' prefix
		 * @param value The resulting number
		 * @return True, if succeeded
		 */
		static bool hexToNumber(const std::string& hex, unsigned int& value);

		/**
		 * Returns the HTTP version string for a given version number.
		 * @param version The version number for which the version string is returned
		 * @return The version string of the requested version number
		 */
		static std::string httpVersionString(const ProtocolVersion version);

	protected:

		/// The TCP client of this HTTP client.
		TCPClient tcpClient_;

		/// The version of the client.
		ProtocolVersion version_ = PV_HTTP_1_1;

		/// The URL of the HTTP server.
		std::string host_;

		/// The port of the HTTP server.
		Port port_;

		/// The response data queue.
		BufferQueue responseQueue_;

		/// The lock of the client.
		Lock lock_;
};

inline HTTPClient::HTTPHeader::ReplyCode HTTPClient::HTTPHeader::code() const
{
	return code_;
}

inline size_t HTTPClient::HTTPHeader::length() const
{
	return length_;
}

inline size_t HTTPClient::HTTPHeader::contentLength() const
{
	return contentLength_;
}

inline bool HTTPClient::HTTPHeader::transferEncodingChunked() const
{
	return transferEncodingChunked_;
}

inline HTTPClient::HTTPHeader::EncodingType HTTPClient::HTTPHeader::encodingType() const
{
	return encodingType_;
}

inline std::string& HTTPClient::HTTPHeader::location()
{
	return location_;
}

inline const HTTPClient::HTTPHeader::Lines& HTTPClient::HTTPHeader::lines() const
{
	return lines_;
}

inline void HTTPClient::HTTPHeader::setCode(const ReplyCode code)
{
	code_ = code;
}

inline void HTTPClient::HTTPHeader::setVersion(const std::string& version)
{
	version_ = version;
}

inline void HTTPClient::HTTPHeader::setLength(const size_t length)
{
	length_ = length;
}

inline void HTTPClient::HTTPHeader::setContentLength(const size_t length)
{
	contentLength_ = length;
}

inline void HTTPClient::HTTPHeader::setTransferEncodingChunked(const bool state)
{
	transferEncodingChunked_ = state;
}

inline void HTTPClient::HTTPHeader::setContentEncodingType(const EncodingType type)
{
	encodingType_ = type;
}

inline void HTTPClient::HTTPHeader::setLocation(const std::string& location)
{
	location_ = location;
}

inline void HTTPClient::HTTPHeader::setLines(Lines&& lines)
{
	lines_ = std::move(lines);
}

inline bool HTTPClient::HTTPHeader::isValid() const
{
	return code_ != RC_INVALID;
}

}

}

#endif // FACEBOOK_NETWORK_HTTP_CLIENT_H
