/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/HTTPSClient.h"

#include "ocean/base/ScopedObject.h"

#include "ocean/math/Numeric.h"

#if defined(_WINDOWS)
	#include <Winhttp.h>
#endif

#if defined(OCEAN_PLATFORM_BUILD_LINUX) || defined(OCEAN_PLATFORM_BUILD_ANDROID)
	#include <curl/curl.h>
#endif

namespace Ocean
{

namespace Network
{

#if defined(OCEAN_PLATFORM_BUILD_LINUX) || defined(OCEAN_PLATFORM_BUILD_ANDROID)

/**
 * This class holds relevant information for a curl session.
 */
class HTTPSClient::CurlSessionData
{
	protected:

		/**
		 * Definition of a vector holding several buffers.
		 */
		typedef std::vector<Buffer> Chunks;

	public:

		/**
		 * Creates a new session object.
		 * @param abort Optional flag that may be set to True by another thread to abort the request, nullptr otherwise
		 * @param progressCallback Optional callback function for the progress, can be invalid
		 */
		CurlSessionData(bool* abort, ProgressCallback progressCallback);

		/**
		 * Returns the entire data of the session.
		 */
		Buffer data() const;

		/**
		 * Curl event function for new data.
		 * @param ptr The new data
		 * @param size Always 1
		 * @param nmemb The size of the data, in bytes, with range [0, infinity)
		 * @param userdata The user data of the curl session, must be valid
		 * @return The number of bytes handled
		 */
		static size_t onNewData(char* ptr, size_t size, size_t nmemb, void* userdata);

		/**
		 * Curl event function for progress.
		 * @param clientp The user data of the curl session, must be valid
		 * @param dltotal The total number of bytes libcurl expects to download
		 * @param dlnow The number of bytes downloaded so far
		 * @param ultotal The total number of bytes libcurl expects to upload in this transfer
		 * @param ulnow The number of bytes uploaded so far
		 * @return Return CURL_PROGRESSFUNC_CONTINUE to continue
		 */
		static int onProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

	protected:

		/// The chunks of the curl session.
		Chunks chunks_;

		/// Optional flag that may be set to True by another thread to abort the request, nullptr otherwise.
		bool* abort_;

		/// Optional callback function for the progress.
		ProgressCallback progressCallback_;
};

HTTPSClient::CurlSessionData::CurlSessionData(bool* abort, ProgressCallback progressCallback) :
	abort_(abort),
	progressCallback_(std::move(progressCallback))
{
	// nothing to do here
}

HTTPSClient::Buffer HTTPSClient::CurlSessionData::data() const
{
	size_t size = 0;

	for (const Buffer& chunk : chunks_)
	{
		size += chunk.size();
	}

	if (size == 0)
	{
		return Buffer();
	}

	Buffer buffer(size);

	uint8_t* pointer = buffer.data();

	for (const Buffer& chunk : chunks_)
	{
		memcpy(pointer, chunk.data(), chunk.size());
		pointer += chunk.size();
	}

	return buffer;
}

size_t HTTPSClient::CurlSessionData::onNewData(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	if (nmemb != 0)
	{
		ocean_assert(ptr != nullptr);
		ocean_assert(size == 1);

		const uint8_t* const dataStart = (const uint8_t*)(ptr);
		const uint8_t* const dataEnd = dataStart + nmemb;

		ocean_assert(userdata != nullptr);
		CurlSessionData* session = (CurlSessionData*)(userdata);

		session->chunks_.emplace_back(dataStart, dataEnd);
	}

	return nmemb;
}

int HTTPSClient::CurlSessionData::onProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	ocean_assert(clientp != nullptr);

	CurlSessionData* session = (CurlSessionData*)(clientp);

	if (session->progressCallback_)
	{
		session->progressCallback_(size_t(dlnow), size_t(dltotal));
	}

	if (session->abort_ != nullptr && *session->abort_)
	{
		return 1;
	}

	return 0;
}

#endif // OCEAN_PLATFORM_BUILD_LINUX

bool HTTPSClient::httpsGetRequest(const std::string& url, Buffer& data, const Port& port, const double timeout, bool* abort, const ProgressCallback& progressCallback)
{
#if defined(_WINDOWS)

	ocean_assert(data.empty());
	ocean_assert(timeout > 0);
	data.clear();

	std::string protocol, host, uri;
	if (!url2uri(url, protocol, host, uri))
	{
		return false;
	}

	const HINTERNET session = WinHttpOpen(L"HTTPS Client/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (session == nullptr)
	{
		return false;
	}

	const BOOL timeoutResult = WinHttpSetTimeouts(session, int(timeout * 1000.0), int(timeout * 1000.0), int(timeout * 1000.0), int(timeout * 1000.0));
	ocean_assert_and_suppress_unused(timeoutResult == TRUE, timeoutResult);

	const HINTERNET connection = WinHttpConnect(session, String::toWString(host).c_str(), port.readable(), 0);

	if (connection == nullptr)
	{
		WinHttpCloseHandle(session);
		return false;
	}

	const HINTERNET request = WinHttpOpenRequest(connection, L"GET", String::toWString(uri).c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

	if (request == nullptr)
	{
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);
		return false;
	}

	bool requestSucceeded = false;

	if (WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(request, 0))
	{
		requestSucceeded = true;

		size_t contentLength = 0;

		wchar_t contentLengthBuffer[1024 + 2];
		DWORD contentLengthBufferLength = 1024u;
		if (WinHttpQueryHeaders(request, WINHTTP_QUERY_CONTENT_LENGTH, WINHTTP_HEADER_NAME_BY_INDEX, contentLengthBuffer, &contentLengthBufferLength, WINHTTP_NO_HEADER_INDEX))
		{
			int value = 0;
			if (String::isInteger32(String::toAString(std::wstring(contentLengthBuffer, contentLengthBufferLength / 2)), &value) && value > 0)
			{
				contentLength = size_t(value);
			}
		}

		DWORD bytesAvailable = 0;
		size_t position = 0;

		while (WinHttpQueryDataAvailable(request, &bytesAvailable) && bytesAvailable != 0)
		{
			if (abort && *abort)
			{
				return false;
			}

			if (position + bytesAvailable > data.size())
			{
				data.resize(position + bytesAvailable);
			}

			DWORD read = 0;
			WinHttpReadData(request, data.data() + position, bytesAvailable, &read);

			position += read;
			ocean_assert(contentLength == 0 || position <= contentLength);

			if (progressCallback)
			{
				progressCallback(position, contentLength);
			}
		}
	}

	WinHttpCloseHandle(request);
	WinHttpCloseHandle(connection);
	WinHttpCloseHandle(session);

	return requestSucceeded;

#elif defined(__APPLE__)

	return httpsGetRequestApple(url, data, port, timeout, abort, progressCallback);

#elif defined(OCEAN_PLATFORM_BUILD_LINUX) || defined(OCEAN_PLATFORM_BUILD_ANDROID)

	using ScopedCurlHandle = ScopedObjectCompileTimeVoidT<CURL*, curl_easy_cleanup>;

	ScopedCurlHandle curlHandle(curl_easy_init());

	if (!curlHandle.isValid())
	{
		return false;
	}

	curl_easy_setopt(*curlHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(*curlHandle, CURLOPT_PORT, long(port.readable()));

	CurlSessionData curlSessionData(abort, progressCallback);

	curl_easy_setopt(*curlHandle, CURLOPT_WRITEFUNCTION, CurlSessionData::onNewData);
	curl_easy_setopt(*curlHandle, CURLOPT_WRITEDATA, &curlSessionData);

	curl_easy_setopt(*curlHandle, CURLOPT_XFERINFOFUNCTION, CurlSessionData::onProgress);
	curl_easy_setopt(*curlHandle, CURLOPT_XFERINFODATA, &curlSessionData);

	const CURLcode result = curl_easy_perform(*curlHandle);

	if (result != CURLE_OK)
	{
		long responseCode = 0;
		curl_easy_getinfo(*curlHandle, CURLINFO_RESPONSE_CODE, &responseCode);

		Log::error() << "HTTP request failed with response " << responseCode;

		return false;
	}

	data = curlSessionData.data();

	return true;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

	OCEAN_SUPPRESS_UNUSED_WARNING(url);
	OCEAN_SUPPRESS_UNUSED_WARNING(data);
	OCEAN_SUPPRESS_UNUSED_WARNING(port);
	OCEAN_SUPPRESS_UNUSED_WARNING(timeout);
	OCEAN_SUPPRESS_UNUSED_WARNING(abort);
	OCEAN_SUPPRESS_UNUSED_WARNING(progressCallback);

	return false;

#endif
}

bool HTTPSClient::httpsPostRequest(const std::string& url, const uint8_t* requestData, const size_t requestDataSize, Buffer& data, const Port& port, const double timeout, const Strings& additionalHeaders)
{
#if defined(_WINDOWS)

	ocean_assert(data.empty());
	ocean_assert(timeout > 0);
	data.clear();

	std::string protocol, host, uri;
	if (!url2uri(url, protocol, host, uri))
	{
		return false;
	}

	const HINTERNET session = WinHttpOpen(L"HTTPS Client/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (session == nullptr)
	{
		return false;
	}

	const BOOL timeoutResult = WinHttpSetTimeouts(session, int(timeout * 1000.0), int(timeout * 1000.0), int(timeout * 1000.0), int(timeout * 1000.0));
	ocean_assert_and_suppress_unused(timeoutResult == TRUE, timeoutResult);

	const HINTERNET connection = WinHttpConnect(session, String::toWString(host).c_str(), port.readable(), 0);

	if (connection == nullptr)
	{
		WinHttpCloseHandle(session);
		return false;
	}

	const HINTERNET request = WinHttpOpenRequest(connection, L"POST", String::toWString(uri).c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

	if (request == nullptr)
	{
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);
		return false;
	}

	std::wstring additionalHeadersString;
	for (size_t n = 0; n < additionalHeaders.size(); ++n)
	{
		additionalHeadersString += String::toWString(additionalHeaders[n]);

		if (n + 1 < additionalHeaders.size())
		{
			additionalHeadersString += L"\r\n";
		}
	}

	if (!additionalHeadersString.empty())
	{
		if (WinHttpAddRequestHeaders(request, additionalHeadersString.c_str(), DWORD(additionalHeadersString.size()), WINHTTP_ADDREQ_FLAG_ADD) != TRUE)
		{
			WinHttpCloseHandle(connection);
			WinHttpCloseHandle(session);
			return false;
		}
	}

	if (WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (void*)(requestData), DWORD(requestDataSize), DWORD(requestDataSize), 0) && WinHttpReceiveResponse(request, 0))
	{
		DWORD bytesAvailable = 0;
		size_t position = 0;

		while (WinHttpQueryDataAvailable(request, &bytesAvailable) && bytesAvailable != 0)
		{
			if (position + bytesAvailable > data.size())
			{
				data.resize(position + bytesAvailable);
			}

			DWORD read = 0;
			WinHttpReadData(request, data.data() + position, bytesAvailable, &read);

			position += read;
		}
	}

	WinHttpCloseHandle(request);
	WinHttpCloseHandle(connection);
	WinHttpCloseHandle(session);

	return true;

#elif defined(__APPLE__)

	return httpsPostRequestApple(url, requestData, requestDataSize, data, port, timeout, additionalHeaders);

#elif defined(OCEAN_PLATFORM_BUILD_LINUX) || defined(OCEAN_PLATFORM_BUILD_ANDROID)

	using ScopedCurlHandle = ScopedObjectCompileTimeVoidT<CURL*, curl_easy_cleanup>;
	using ScopedCurlList = ScopedObjectCompileTimeVoidT<struct curl_slist*, curl_slist_free_all>;

	ScopedCurlHandle curlHandle(curl_easy_init());

	if (!curlHandle.isValid())
	{
		return false;
	}

	curl_easy_setopt(*curlHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(*curlHandle, CURLOPT_PORT, long(port.readable()));

	CurlSessionData curlSessionData(nullptr, ProgressCallback());

	curl_easy_setopt(*curlHandle, CURLOPT_WRITEFUNCTION, CurlSessionData::onNewData);
	curl_easy_setopt(*curlHandle, CURLOPT_WRITEDATA, &curlSessionData);

	curl_easy_setopt(*curlHandle, CURLOPT_XFERINFOFUNCTION, CurlSessionData::onProgress);
	curl_easy_setopt(*curlHandle, CURLOPT_XFERINFODATA, &curlSessionData);

	if (!NumericT<long>::isInsideValueRange(requestDataSize))
	{
		ocean_assert(false && "Data too large!");
		return false;
	}

	curl_easy_setopt(*curlHandle, CURLOPT_POSTFIELDSIZE, long(requestDataSize));

	const char* charRequestData = (const char*)(requestData);
	curl_easy_setopt(*curlHandle, CURLOPT_POSTFIELDS, const_cast<char*>(charRequestData));

	ScopedCurlList headerList;

	if (!additionalHeaders.empty())
	{
		struct curl_slist* list = nullptr;

		for (const std::string& additionalHeader : additionalHeaders)
		{
			list = curl_slist_append(list, additionalHeader.c_str());
		}

		headerList = ScopedCurlList(list);

		curl_easy_setopt(*curlHandle, CURLOPT_HTTPHEADER, *headerList);
	}

	const CURLcode result = curl_easy_perform(*curlHandle);

	if (result != CURLE_OK)
	{
		long responseCode = 0;
		curl_easy_getinfo(*curlHandle, CURLINFO_RESPONSE_CODE, &responseCode);

		Log::error() << "HTTP request failed with response " << responseCode;

		return false;
	}

	data = curlSessionData.data();

	return true;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

	OCEAN_SUPPRESS_UNUSED_WARNING(url);
	OCEAN_SUPPRESS_UNUSED_WARNING(requestData);
	OCEAN_SUPPRESS_UNUSED_WARNING(requestDataSize);
	OCEAN_SUPPRESS_UNUSED_WARNING(data);
	OCEAN_SUPPRESS_UNUSED_WARNING(port);
	OCEAN_SUPPRESS_UNUSED_WARNING(timeout);
	OCEAN_SUPPRESS_UNUSED_WARNING(additionalHeaders);

	return false;

#endif
}

bool HTTPSClient::url2uri(const std::string& url, std::string& protocol, std::string& host, std::string& uri)
{
	if (url.size() <= 8)
	{
		return false;
	}

	// we check whether the URL start with "HTTPS://"
	if (String::toLower(url.substr(0, 8)) != std::string("https://"))
	{
		return false;
	}

	// now we identify the host
	const std::string::size_type pos = url.find('/', 8);
	if (pos == 8)
	{
		return false;
	}

	protocol = std::string("https://");

	if (pos == std::string::npos)
	{
		host = url.substr(8);
		ocean_assert(!host.empty());

		uri.clear();
		return true;
	}

	host = url.substr(8, pos - 8);
	ocean_assert(!host.empty());

	uri = url.substr(pos + 1);
	return true;
}

}

}
