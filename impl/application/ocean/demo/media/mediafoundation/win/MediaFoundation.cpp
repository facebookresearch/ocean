/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/mediafoundation/win/MediaFoundation.h"

#include "ocean/base/String.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <uuids.h>
#include <wmcodecdsp.h>
#include <Propvarutil.h>

using namespace Ocean;

int wmain(int /*argc*/, wchar_t** /*argv*/)
{
	std::cout << "Windows Media Foundation demo:" << std::endl << std::endl;

	bool noError = true;

	// initialize the COM library
	const HRESULT initializeResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	ocean_assert_and_suppress_unused(initializeResult == S_OK, initializeResult);

	// initialize the Media Foundation
	const HRESULT startupResult = MFStartup(MF_VERSION, MFSTARTUP_FULL);
	noError = noError && startupResult == S_OK;

	IMFAttributes* attributes = nullptr;
	const HRESULT attributesResult = MFCreateAttributes(&attributes, 1);
	noError = noError && attributesResult == S_OK;

	if (noError)
	{
		const HRESULT setResult = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
		noError = noError && setResult == S_OK;
	}

	IMFActivate** devices = nullptr;
	UINT32 count = 0;
	const HRESULT enumResult = MFEnumDeviceSources(attributes, &devices, &count);
	noError = noError && enumResult == S_OK;

	if (noError)
	{
		std::cout << "Found " << count << " source devices." << std::endl;
	}

	for (UINT32 n = 0; n < count; ++n)
	{
		constexpr size_t bufferSize = 4096;

		UINT32 length = 0;
		const HRESULT lengthResult = devices[n]->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &length);
		ocean_assert_and_suppress_unused(lengthResult == S_OK, lengthResult);

		ocean_assert(length <= bufferSize - 1);

		if (length <= bufferSize - 1)
		{
			wchar_t name[bufferSize];
			HRESULT nameResult = devices[n]->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, name, bufferSize, nullptr);
			ocean_assert_and_suppress_unused(nameResult == S_OK, nameResult);

			std::cout << "Device " << n << " has the name: " << String::toAString(name) << std::endl;

			const HRESULT linkResult = devices[n]->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &length);
			ocean_assert_and_suppress_unused(linkResult == S_OK, linkResult);

			ocean_assert(length <= bufferSize - 1);
			if (length <= bufferSize - 1)
			{
				nameResult = devices[n]->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, name, bufferSize, nullptr);
				ocean_assert_and_suppress_unused(nameResult == S_OK, nameResult);

				std::cout << "and has the symbolic link: " << String::toAString(name) << std::endl;
			}
		}
	}

	std::cout << std::endl;

	// release all media foundation objects
	for (UINT32 n = 0; n < count; ++n)
	{
		if (devices[n])
		{
			devices[n]->Release();
		}
	}

	CoTaskMemFree(devices);

	if (attributes != nullptr)
	{
		attributes->Release();
	}

	// uninitialize the Media Foundation
	const HRESULT shutdownResult = MFShutdown();
	ocean_assert_and_suppress_unused(shutdownResult == S_OK, shutdownResult);

	// uninitialize the COM library
	CoUninitialize();

	std::cout << "Press a key to exit" << std::endl;
	getchar();

	return 0;
}
