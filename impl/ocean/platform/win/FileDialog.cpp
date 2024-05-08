/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/FileDialog.h"

#include <commdlg.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

bool FileDialog::openFile(std::wstring& filePath)
{
	OPENFILENAMEW openFileNameData;
	memset(&openFileNameData, 0, sizeof(OPENFILENAMEW));
	openFileNameData.lStructSize = sizeof(OPENFILENAMEW);

	wchar_t localFilePath[1024 + 1];
	localFilePath[0] = '\0';

	openFileNameData.lpstrFile = localFilePath;
	openFileNameData.nMaxFile = 1024;
	openFileNameData.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_EXPLORER;
	openFileNameData.lpstrFilter = L"All Files\0*.*\0\0";

	if (GetOpenFileNameW(&openFileNameData) != TRUE)
		return false;

	filePath = std::wstring(localFilePath);

	return true;
}

bool FileDialog::saveFile(std::wstring& filePath)
{
	OPENFILENAMEW openFileNameData;
	memset(&openFileNameData, 0, sizeof(OPENFILENAMEW));
	openFileNameData.lStructSize = sizeof(OPENFILENAMEW);

	wchar_t localFilePath[1024 + 1];
	localFilePath[0] = '\0';

	openFileNameData.lpstrFile = localFilePath;
	openFileNameData.nMaxFile = 1024;
	openFileNameData.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	openFileNameData.lpstrFilter = L"All Files\0*.*\0\0";

	if (GetSaveFileNameW(&openFileNameData) != TRUE)
		return false;

	filePath = std::wstring(localFilePath);

	return true;
}

}

}

}
