/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/openxr/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

std::string Utilities::translateResult(const XrInstance& xrInstance, const XrResult xrResult)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);

	char buffer[XR_MAX_RESULT_STRING_SIZE] = {};

	if (xrResultToString(xrInstance, xrResult, buffer) == XR_SUCCESS)
	{
		buffer[XR_MAX_RESULT_STRING_SIZE - 1] = '\0';

		return std::string(buffer);
	}

	ocean_assert(false && "Failed to translate result");
	return "Unknown";
}

std::string Utilities::translateSessionState(const XrSessionState xrSessionState)
{
	switch (xrSessionState)
	{
		case XR_SESSION_STATE_UNKNOWN:
			return "XR_SESSION_STATE_UNKNOWN";

		case XR_SESSION_STATE_IDLE:
			return "XR_SESSION_STATE_IDLE";

		case XR_SESSION_STATE_READY:
			return "XR_SESSION_STATE_READY";

		case XR_SESSION_STATE_SYNCHRONIZED:
			return "XR_SESSION_STATE_SYNCHRONIZED";

		case XR_SESSION_STATE_VISIBLE:
			return "XR_SESSION_STATE_VISIBLE";

		case XR_SESSION_STATE_FOCUSED:
			return "XR_SESSION_STATE_FOCUSED";

		case XR_SESSION_STATE_STOPPING:
			return "XR_SESSION_STATE_STOPPING";

		case XR_SESSION_STATE_LOSS_PENDING:
			return "XR_SESSION_STATE_LOSS_PENDING";

		case XR_SESSION_STATE_EXITING:
			return "XR_SESSION_STATE_EXITING";

		case XR_SESSION_STATE_MAX_ENUM:
			return "XR_SESSION_STATE_MAX_ENUM";
	}

	ocean_assert(false && "Unknown type!");
	return "Unknown";
}

std::string Utilities::translateViewConfigurationType(const XrViewConfigurationType xrViewConfigurationType)
{
	switch (xrViewConfigurationType)
	{
		case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO:
			return "XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO";

		case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO:
			return "XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO";

		case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO:
			return "XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO";

		case XR_VIEW_CONFIGURATION_TYPE_SECONDARY_MONO_FIRST_PERSON_OBSERVER_MSFT:
			return "XR_VIEW_CONFIGURATION_TYPE_SECONDARY_MONO_FIRST_PERSON_OBSERVER_MSFT";

		case XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM:
			ocean_assert(false && "Invalid type!");
			return "XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM";
	}

	ocean_assert(false && "Unknown type!");
	return "Unknown";
}

std::string Utilities::translateColorSpace(const XrColorSpaceFB xrColorSpaceFB)
{
	switch (xrColorSpaceFB)
	{
		case XR_COLOR_SPACE_UNMANAGED_FB:
			return "XR_COLOR_SPACE_UNMANAGED_FB";

		case XR_COLOR_SPACE_REC2020_FB:
			return "XR_COLOR_SPACE_REC2020_FB";

		case XR_COLOR_SPACE_REC709_FB:
			return "XR_COLOR_SPACE_REC709_FB";

		case XR_COLOR_SPACE_RIFT_CV1_FB:
			return "XR_COLOR_SPACE_RIFT_CV1_FB";

		case XR_COLOR_SPACE_RIFT_S_FB:
			return "XR_COLOR_SPACE_RIFT_S_FB";

		case XR_COLOR_SPACE_QUEST_FB:
			return "XR_COLOR_SPACE_QUEST_FB";

		case XR_COLOR_SPACE_P3_FB:
			return "XR_COLOR_SPACE_P3_FB";

		case XR_COLOR_SPACE_ADOBE_RGB_FB:
			return "XR_COLOR_SPACE_ADOBE_RGB_FB";

		case XR_COLOR_SPACE_MAX_ENUM_FB:
			break;
	}

	ocean_assert(false && "Unknown type!");
	return "Unknown";
}

std::string Utilities::translatePath(const XrInstance& xrInstance, const XrPath& xrPath)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);

	if (xrPath == XR_NULL_PATH)
	{
		return std::string();
	}

	uint32_t bufferCountOutput = 0u; // count of characters written (including the terminating '\0')

	if (xrPathToString(xrInstance, xrPath, 0u, &bufferCountOutput, nullptr) != XR_SUCCESS)
	{
		ocean_assert(false && "Failed to convert path");
		return std::string();
	}

	ocean_assert(bufferCountOutput >= 1);

	if (bufferCountOutput <= 1u)
	{
		return std::string();
	}

	std::vector<char> buffer(bufferCountOutput);

	bufferCountOutput = 0u;
	if (xrPathToString(xrInstance, xrPath, uint32_t(buffer.size()), &bufferCountOutput, nullptr) != XR_SUCCESS)
	{
		ocean_assert(false && "Failed to convert path");
		return std::string();
	}

	ocean_assert(size_t(bufferCountOutput) == buffer.size());

	ocean_assert(buffer.size() >= 2);
	return std::string(buffer.data(), buffer.size() - 1);
}

XrPath Utilities::translatePath(const XrInstance& xrInstance, const std::string& path)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);

	if (path.empty())
	{
		return XR_NULL_PATH;
	}

	XrPath xrPath = XR_NULL_PATH;
	const XrResult xrResult = xrStringToPath(xrInstance, path.c_str(), &xrPath);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to translate path '" << path << "': " << translateResult(xrInstance, xrResult);
		return XR_NULL_PATH;
	}

	return xrPath;
}

SquareMatrixF4 Utilities::toProjectionMatrix4(const XrFovf& xrFovf, const float nearDistance, const float farDistance)
{
	ocean_assert(xrFovf.angleLeft > -NumericF::pi_2() && xrFovf.angleLeft < NumericF::pi_2());
	ocean_assert(xrFovf.angleRight > -NumericF::pi_2() && xrFovf.angleRight < NumericF::pi_2());
	ocean_assert(xrFovf.angleUp > -NumericF::pi_2() && xrFovf.angleUp < NumericF::pi_2());
	ocean_assert(xrFovf.angleDown > -NumericF::pi_2() && xrFovf.angleDown < NumericF::pi_2());

	ocean_assert(xrFovf.angleLeft < xrFovf.angleRight);
	ocean_assert(xrFovf.angleDown < xrFovf.angleUp);

	ocean_assert(0 < nearDistance && nearDistance < farDistance);

	const float left = NumericF::tan(xrFovf.angleLeft) * nearDistance;
	const float right = NumericF::tan(xrFovf.angleRight) * nearDistance;

	const float top = NumericF::tan(xrFovf.angleUp) * nearDistance;
	const float bottom = NumericF::tan(xrFovf.angleDown) * nearDistance;

	return SquareMatrixF4::frustumMatrix(left, right, top, bottom, nearDistance, farDistance);
}

}

}

}
