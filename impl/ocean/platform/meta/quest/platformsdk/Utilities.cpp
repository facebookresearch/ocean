/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace PlatformSDK
{

Indices64 Utilities::extractUsers(ovrUserArrayHandle userArrayHandle, std::vector<std::string>* userNames)
{
	ocean_assert(userArrayHandle != nullptr);

	if (userNames != nullptr)
	{
		userNames->clear();
	}

	if (userArrayHandle == nullptr)
	{
		return Indices64();
	}

	Indices64 userIds;

	const size_t arraySize = ovr_UserArray_GetSize(userArrayHandle);
	userIds.reserve(arraySize);

	if (userNames != nullptr)
	{
		userNames->reserve(arraySize);
	}

	for (size_t n = 0; n < arraySize; ++n)
	{
		const ovrUserHandle userHandle = ovr_UserArray_GetElement(userArrayHandle, n);

		const uint64_t userId = ovr_User_GetID(userHandle);

		ocean_assert(userId != 0ull);
		if (userId != 0ull)
		{
			userIds.emplace_back(userId);

			if (userNames != nullptr)
			{
				const char* userName = ovr_User_GetDisplayName(userHandle);

				if (userName != nullptr)
				{
					userNames->emplace_back(userName);
				}
				else
				{
					ocean_assert(false && "Invalid user name");
					userNames->emplace_back(std::string());
				}
			}
		}
	}

	return userIds;
}

std::string Utilities::errorMessage(ovrMessage* message)
{
	ocean_assert(message != nullptr);
	ocean_assert(ovr_Message_IsError(message));

	const ovrErrorHandle errorHandle = ovr_Message_GetError(message);

	const char* errorMessageDisplay = ovr_Error_GetDisplayableMessage(errorHandle);
	const char* errorMessageTechnical = ovr_Error_GetMessage(errorHandle);

	std::string result;

	if (errorMessageDisplay != nullptr && errorMessageDisplay[0] != '\0')
	{
		result = std::string(errorMessageDisplay);
	}

	if (errorMessageTechnical == nullptr || errorMessageTechnical[0] == '\0')
	{
		if (result.empty())
		{
			result = "unknown error";
		}
	}
	else
	{
		if (result.empty())
		{
			result = std::string(errorMessageTechnical);
		}
		else
		{
			result += ", " + std::string(errorMessageTechnical);
		}
	}

	const int errorCode = ovr_Error_GetCode(errorHandle);
	const int httpCode = ovr_Error_GetHttpCode(errorHandle);

	if (errorCode > 0)
	{
		result += ", error code " + String::toAString(errorCode);
	}

	if (httpCode > 0)
	{
		result += ", http code " + String::toAString(httpCode);
	}

	return result;
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
