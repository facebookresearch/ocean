/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/HTTPClient.h"
#include "ocean/network/Resolver.h"

#include "ocean/base/String.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Compression.h"

namespace Ocean
{

namespace Network
{

bool HTTPClient::HTTPHeader::parseHeaderLine(const std::string& line, HTTPHeader& header)
{
	ocean_assert(!line.empty());

	const std::string::size_type lineSize = line.length();

	// HTTP/
	if (line.find("HTTP/") == 0)
	{
		std::string::size_type firstPos = 0, secondPos = 4;

		while (secondPos <= lineSize && line[secondPos] != ' ')
		{
			secondPos++;
		}

		if (secondPos > lineSize)
		{
			return false;
		}

		header.setVersion(line.substr(firstPos, secondPos - firstPos));
		firstPos = ++secondPos;

		// Status Code
		while (secondPos <= lineSize && isdigit((unsigned char)line[secondPos]))
		{
			secondPos++;
		}

		if (secondPos > lineSize || secondPos == firstPos)
		{
			return false;
		}

		std::string statusCode = line.substr(firstPos, secondPos - firstPos);
		header.setCode(HTTPHeader::ReplyCode(atoi(statusCode.c_str())));

		return true;
	}
	// Content-Length
	else if (line.find("Content-Length") == 0)
	{
		std::string::size_type firstPos = 13, secondPos;

		while (firstPos <= lineSize && !isdigit((unsigned char)line[firstPos]))
		{
			firstPos++;
		}

		if (firstPos > lineSize)
		{
			return false;
		}

		secondPos = firstPos;

		while (secondPos <= lineSize && isdigit((unsigned char)line[secondPos]))
		{
			secondPos++;
		}

		if (firstPos < secondPos)
		{
			std::string digits = line.substr(firstPos, secondPos - firstPos);

			const int value = atoi(digits.c_str());
			if (value < 0)
			{
				return false;
			}

			header.setContentLength(size_t(value));
			return true;
		}

		return false;
	}
	// Transfer-Encoding: chunked
	else if (line.find("Transfer-Encoding: chunked") == 0)
	{
		header.setTransferEncodingChunked(true);
		return true;
	}
	// Content-Encoding: gzip
	else if (line.find("Content-Encoding: gzip") == 0)
	{
		header.setContentEncodingType(HTTPHeader::ET_GZIP);
		return true;
	}
	else if (line.find("Location: ") == 0)
	{
		header.setLocation(line.substr(10));
		return true;
	}

	return true;
}

HTTPClient::HTTPClient(const std::string& host, const Port& port) :
	host_(host),
	port_(port)
{
	tcpClient_.setReceiveCallback(TCPClient::ReceiveCallback::create(*this, &HTTPClient::onResponse));
}

HTTPClient::~HTTPClient()
{
	tcpClient_.setReceiveCallback(TCPClient::ReceiveCallback());
}

bool HTTPClient::connect()
{
	const ScopedLock scopedLock(lock_);

	if (host_.empty() || tcpClient_.isConnected())
	{
		return true;
	}

	const Address4 address = Resolver::resolveFirstIp4(host_);
	if (address.isNull())
	{
		return false;
	}

	return tcpClient_.connect(address, port_);
}

bool HTTPClient::connect(const std::string& host, const Port& port)
{
	const ScopedLock scopedLock(lock_);

	host_ = host;
	port_ = port;

	const Address4 address = Resolver::resolveFirstIp4(host_);
	if (address.isNull())
	{
		tcpClient_.disconnect();
		return false;
	}

	return tcpClient_.connect(address, port_);
}

bool HTTPClient::invokeHeadRequest(const std::string& uri, HTTPHeader& header, const double timeout)
{
	ocean_assert(!uri.empty());
	ocean_assert(timeout >= 0);

	if (!sendRequest(uri, "HEAD"))
	{
		return false;
	}

	// now we wait for the response

	header = HTTPHeader();
	Timestamp startTimestamp(true);

	do
	{
		Buffer buffer(responseQueue_.pop());

		if (!buffer.empty())
		{
			if (!parseHeader((char*)buffer.data(), buffer.size(), header))
			{
				return false;
			}

			if (header.isValid())
			{
				ocean_assert(responseQueue_.isEmpty());

				// the header could be parsed and due the HEAD method we do not expect more information to be delivered
				break;
			}
		}

		Thread::sleep(1u);
	}
	while (startTimestamp + timeout > Timestamp(true));

	return header.isValid();
}

bool HTTPClient::invokeGetRequest(const std::string& uri, Buffer& data, const double timeout, std::string* uriRedirection, HTTPHeader::ReplyCode* replyCode, bool* abort, const ProgressCallback& progressCallback)
{
	ocean_assert(!uri.empty());
	ocean_assert(data.empty());
	ocean_assert(timeout > 0);

	data.clear();

	if (!sendRequest(uri, "GET"))
	{
		return false;
	}

	// now we wait for the response

	HTTPHeader responseHeader;
	Timestamp startTimestamp(true);

	Buffer responseBuffer;
	size_t responseBufferPosition = 0;
	size_t responsePendingChunkSize = 0;

	do
	{
		Buffer buffer;

		if (abort && *abort)
		{
			return false;
		}

		if (responseHeader.isValid())
		{
			// the header could be parsed so we add pending data

			ocean_assert(responseHeader.transferEncodingChunked() || responseHeader.contentLength() != 0);

			buffer = responseQueue_.pop();

			if (!buffer.empty())
			{
				if (appendData(responseHeader, responseBuffer, responseBufferPosition, (char*)buffer.data(), buffer.size(), responsePendingChunkSize))
					break;

				startTimestamp.toNow();
			}
		}
		else
		{
			buffer = responseQueue_.pop();

			if (!buffer.empty())
			{
				if (!parseHeader((char*)buffer.data(), buffer.size(), responseHeader) || responseHeader.code() != HTTPHeader::RC_OK)
				{
					if (responseHeader.code() == HTTPHeader::RC_MOVED_PERMANENTLY && uriRedirection)
					{
						*uriRedirection = responseHeader.location();
					}

					if (replyCode)
					{
						*replyCode = responseHeader.code();
					}

					return false;
				}

				ocean_assert(responseHeader.length() != 0);

				// if the response does not provide any (payload) content we can break here
				if (responseHeader.contentLength() == 0)
				{
					break;
				}

				if (buffer.size() > responseHeader.length())
				{
					if (appendData(responseHeader, responseBuffer, responseBufferPosition, (char*)buffer.data() + responseHeader.length(), buffer.size() - responseHeader.length(), responsePendingChunkSize))
						break;
				}

				startTimestamp.toNow();
			}
		}

		if (progressCallback)
		{
			progressCallback(responseBufferPosition, responseHeader.contentLength());
		}

		Thread::sleep(1);
	}
	while (startTimestamp + timeout > Timestamp(true));

	if (responseHeader.encodingType() == HTTPHeader::ET_GZIP)
	{
		if (!IO::Compression::gzipDecompress(responseBuffer.data(), responseBuffer.size(), data))
		{
			return false;
		}

		responseBuffer.clear();
	}
	else
	{
		data = std::move(responseBuffer);
	}

	if (replyCode)
	{
		*replyCode = responseHeader.code();
	}

	return true;
}

bool HTTPClient::httpGetRequest(const std::string& url, Buffer& data, const Port& port, const double timeout, bool allowRedirect, std::string* redirectedURI, HTTPHeader::ReplyCode* replyCode, bool* abort, const ProgressCallback& progressCallback)
{
	std::string protocol, hostString, uri;
	if (!url2uri(url, protocol, hostString, uri))
	{
		return false;
	}

	std::string urlRedirection;

	if (replyCode)
	{
		*replyCode = HTTPHeader::RC_INVALID;
	}

	{
		HTTPClient httpClient(hostString, port);

		if (!httpClient.connect())
		{
			return false;
		}

		if (httpClient.invokeGetRequest(uri, data, timeout, &urlRedirection, replyCode, abort, progressCallback))
		{
			return true;
		}
	}

	if (!allowRedirect || urlRedirection.empty())
	{
		return false;
	}

	if (redirectedURI)
	{
		std::string dummyProtocol, dummyHostString;
		if (!url2uri(urlRedirection, dummyProtocol, dummyHostString, *redirectedURI))
		{
			return false;
		}
	}

	return httpGetRequest(urlRedirection, data, port, timeout, false, nullptr, replyCode);
}

bool HTTPClient::sendRequest(const std::string& uri, const std::string& requestMethod)
{
	ocean_assert(!uri.empty() && !requestMethod.empty());

	responseQueue_.clear();

	const ScopedLock scopedLock(lock_);

	if (!tcpClient_.isConnected())
	{
		return false;
	}

	const std::string command = requestMethod + std::string(" /") + uri + std::string(" ") + httpVersionString(version_) + std::string("\r\nHost: ") + host_  + std::string("\r\n\r\n");

	return tcpClient_.send(command) == Socket::SR_SUCCEEDED;
}

void HTTPClient::onResponse(const void* data, const size_t size)
{
	ocean_assert(data != nullptr || size == 0);

	responseQueue_.push(data, size);
}

bool HTTPClient::url2uri(const std::string& url, std::string& protocol, std::string& host, std::string& uri)
{
	if (url.size() <= 7)
	{
		return false;
	}

	// we check whether the URL start with "HTTP://"
	if (String::toLower(url.substr(0, 7)) != std::string("http://"))
	{
		return false;
	}

	// now we identify the host
	const std::string::size_type pos = url.find('/', 7);
	if (pos == 7)
	{
		return false;
	}

	protocol = std::string("http://");

	if (pos == std::string::npos)
	{
		host = url.substr(7);
		ocean_assert(!host.empty());

		uri.clear();
		return true;
	}

	host = url.substr(7, pos - 7);
	ocean_assert(!host.empty());

	uri = url.substr(pos + 1);
	return true;
}

bool HTTPClient::parseHeader(const char* data, const size_t size, HTTPHeader& header)
{
	ocean_assert(data && size > 0);

	if (!data || size < 4)
	{
		return false;
	}

	if (std::string(data, 5) != "HTTP/")
	{
		return false;
	}

	size_t firstPos = 0, secondPos;

	HTTPHeader::Lines headerLines;

	while (true)
	{
		if (firstPos + 1 >= size)
		{
			return false;
		}

		// End of header
		if (data[firstPos] == '\r' && data[firstPos + 1] == '\n')
		{
			header.setLength(firstPos + 2);
			header.setLines(std::move(headerLines));
			return true;
		}

		secondPos = firstPos;

		while (secondPos < size && (data[secondPos] != '\r' && data[secondPos] != '\n'))
		{
			secondPos++;
		}

		if (firstPos < secondPos)
		{
			std::string line = std::string(data + firstPos, secondPos - firstPos);

			if (!HTTPHeader::parseHeaderLine(line, header))
			{
				return false;
			}

			headerLines.push_back(std::move(line));
		}
		else
		{
			header.setLines(std::move(headerLines));
			return true;
		}

		firstPos = secondPos + 2;
	}
}

bool HTTPClient::appendData(const HTTPHeader& header, Buffer& buffer, size_t& bufferPosition, const char* payload, size_t payloadSize, size_t& pendingChunkSize)
{
	ocean_assert(header.isValid());
	ocean_assert(header.code() == HTTPHeader::RC_OK);
	ocean_assert(header.length() != 0);
	ocean_assert(payloadSize != 0);

	ocean_assert(bufferPosition <= buffer.size());
	ocean_assert(header.transferEncodingChunked() || pendingChunkSize == 0);

	if (buffer.empty() && header.contentLength() != 0)
		buffer.resize(header.contentLength());

	size_t iterationSize = 0;

	if (header.transferEncodingChunked())
	{
		// check whether we have to start with a new chunk
		if (pendingChunkSize == 0)
		{
			size_t offset = 0;
			const std::string chungSizeLine = line((const char*)payload, payloadSize, offset);

			unsigned int chunkSize = 0u;
			if (!hexToNumber(chungSizeLine, chunkSize))
			{
				return false;
			}

			// check whether we have the end-chunk
			if (chunkSize == 0u)
			{
				return true;
			}

			pendingChunkSize = chunkSize;

			ocean_assert(offset <= payloadSize);
			payloadSize -= offset;
			payload += offset;
		}

		iterationSize = min(payloadSize, pendingChunkSize);
	}
	else
	{
		iterationSize = min(payloadSize, header.contentLength());
	}

	if (bufferPosition + iterationSize > buffer.size())
	{
		buffer.resize(bufferPosition + iterationSize);
	}

	memcpy((void*)(buffer.data() + bufferPosition), (void*)payload, iterationSize);
	bufferPosition += iterationSize;

	ocean_assert(payloadSize >= iterationSize);
	payloadSize -= iterationSize;
	payload += iterationSize;

	if (pendingChunkSize != 0)
	{
		pendingChunkSize -= iterationSize;

		// we need to remove the end-chunk signs ('\r\n')
		if (pendingChunkSize == 0)
		{
			payloadSize -= 2;
			payload += 2;
		}
	}

	if (payloadSize != 0)
	{
		return appendData(header, buffer, bufferPosition, payload, payloadSize, pendingChunkSize);
	}

	if (!header.transferEncodingChunked() && header.contentLength() != 0 && bufferPosition >= header.contentLength())
	{
		if (bufferPosition > header.contentLength())
		{
			// a request can end with a connection-close response can follow which we skip
			buffer.resize(header.contentLength());
		}

		return true;
	}

	return false;
}

std::string HTTPClient::line(const char* data, const size_t size, size_t& offset)
{
	offset = size;

	if (data == nullptr || size <= 2)
	{
		return std::string();
	}

	for (size_t n = 0; n < size - 1; ++n)
	{
		if (data[n] == '\r' && data[n + 1] == '\n')
		{
			offset = n + 2;
			return std::string(data, n);
		}
	}

	return std::string(data, size);
}

bool HTTPClient::hexToNumber(const std::string& hex, unsigned int& value)
{
	value = 0u;

	if (hex.empty())
	{
		return false;
	}

	for (size_t n = 0; n < hex.length(); ++n)
	{
		unsigned int digit = 0u;
		const char character = hex[hex.length() - n - 1];

		if (character >= '0' && character <= '9')
		{
			digit = character - '0';
		}
		else if (character >= 'a' && character <= 'f')
		{
			digit = 10u + (character - 'a');
		}
		else if (character >= 'A' && character <= 'F')
		{
			digit = 10u + (character - 'A');
		}
		else
		{
			return false;
		}

		ocean_assert(value <= value + (digit << (n * 4u)));
		value += (digit << (n * 4u));
	}

	return true;
}

std::string HTTPClient::httpVersionString(const ProtocolVersion version)
{
	switch (version)
	{
		case PV_HTTP_1_0:
			return std::string("HTTP/1.0");

		case PV_HTTP_1_1:
			return std::string("HTTP/1.1");
	}

	ocean_assert(false && "Invalid HTTP protocol version!");
	return std::string("HTTP/1.1");
}

}

}
