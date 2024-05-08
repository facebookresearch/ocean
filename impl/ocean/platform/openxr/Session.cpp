/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/openxr/Session.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

Session::~Session()
{
	release();
}

bool Session::initialize(const XrInstance& xrInstance, const XrSystemId& xrSystemId, const void* xrGraphicsBinding, const unsigned int width, const unsigned int height)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSystemId != XR_NULL_SYSTEM_ID);
	ocean_assert(xrGraphicsBinding != nullptr);

	if (xrInstance == XR_NULL_HANDLE || xrSystemId == XR_NULL_SYSTEM_ID || xrGraphicsBinding == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (xrSession_ != XR_NULL_HANDLE)
	{
		ocean_assert(false && "This session has already been initialized");
		return true;
	}

	XrSessionCreateInfo xrSessionCreateInfo = {XR_TYPE_SESSION_CREATE_INFO};
	xrSessionCreateInfo.next = xrGraphicsBinding;
	xrSessionCreateInfo.createFlags = 0;
	xrSessionCreateInfo.systemId = xrSystemId;

	XrResult xrResult = xrCreateSession(xrInstance, &xrSessionCreateInfo, &xrSession_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create session: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	ocean_assert(xrSession_ != XR_NULL_HANDLE);

#ifdef OCEAN_DEBUG
	XrColorSpacesFB debugColorSpaces;
	if (determineExistingColorSpaces(xrInstance, xrSession_, debugColorSpaces))
	{
		Log::debug() << debugColorSpaces.size() << " supported color spaces:";

		for (const XrColorSpaceFB& debugColorSpace : debugColorSpaces)
		{
			Log::debug() << Utilities::translateColorSpace(debugColorSpace);
		}
	}
#endif

	xrInstance_ = xrInstance;
	xrSystemId_ = xrSystemId;

	width_ = width;
	height_ = height;

	ocean_assert(!isRunning_);

	return true;
}

void Session::release()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!isRunning_);

	if (xrSession_ != XR_NULL_HANDLE)
	{
		xrDestroySession(xrSession_);
		xrSession_ = XR_NULL_HANDLE;
	}

	xrInstance_ = XR_NULL_HANDLE;
}

bool Session::begin(const XrViewConfigurationType xrViewConfigurationType)
{
	const ScopedLock scopedLock(lock_);

	if (isRunning_)
	{
		ocean_assert(false && "The session is already running");
		return false;
	}

	ocean_assert(isValid());

	XrSessionBeginInfo xrSessionBeginInfo = {XR_TYPE_SESSION_BEGIN_INFO};
	xrSessionBeginInfo.primaryViewConfigurationType = xrViewConfigurationType;

	const XrResult xrResult = xrBeginSession(xrSession_, &xrSessionBeginInfo);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to begin session: " << translateResult(xrResult);
		return false;
	}

	isRunning_ = true;

	return true;
}

bool Session::end()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isRunning_)
	{
		ocean_assert(false && "The session is not running");
	}

	const XrResult xrResult = xrEndSession(xrSession_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to end session: " << translateResult(xrResult);
		return false;
	}

	isRunning_ = false;

	return true;
}

bool Session::nextFrame(XrTime& predictedDisplayTime, XrDuration* predictedDisplayPeriod)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	XrFrameWaitInfo xrFrameWaitInfo = {XR_TYPE_FRAME_WAIT_INFO};

	XrFrameState xrFrameState = {XR_TYPE_FRAME_STATE};

	const XrResult xrResult = xrWaitFrame(xrSession_, &xrFrameWaitInfo, &xrFrameState);
	ocean_assert_and_suppress_unused(xrResult == XR_SUCCESS, xrResult);

	predictedDisplayTime = xrFrameState.predictedDisplayTime;

	if (predictedDisplayPeriod != nullptr)
	{
		*predictedDisplayPeriod = xrFrameState.predictedDisplayPeriod;
	}

	return xrFrameState.shouldRender == XR_TRUE;
}

std::string Session::translateResult(const XrResult xrResult) const
{
	const ScopedLock scopedLock(lock_);

	if (xrInstance_ != XR_NULL_HANDLE)
	{
		return Utilities::translateResult(xrInstance_, xrResult);
	}

	ocean_assert(false && "Failed to translate XrResult");
	return "Unknown: OpenXR Instance not initialized";
}

bool Session::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return xrSession_ != XR_NULL_HANDLE;
}

Session& Session::operator=(Session&& session)
{
	if (this != &session)
	{
		release();

		xrSession_ = session.xrSession_;
		session.xrSession_ = XR_NULL_HANDLE;
	}

	return *this;
}

Session::operator XrSession() const
{
	const ScopedLock scopedLock(lock_);

	return xrSession_;
}

ScopedXrSpace Session::createSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrReferenceSpaceType xrReferenceSpaceType)
{
	ocean_assert(xrSession != XR_NULL_HANDLE);

	XrReferenceSpaceCreateInfo xrReferenceSpaceCreateInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
	xrReferenceSpaceCreateInfo.referenceSpaceType = xrReferenceSpaceType;
	xrReferenceSpaceCreateInfo.poseInReferenceSpace.orientation.w = 1.0f; // using identity pose

	XrSpace xrSpace = XR_NULL_HANDLE;
	const XrResult xrResult = xrCreateReferenceSpace(xrSession, &xrReferenceSpaceCreateInfo, &xrSpace);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create reference space: " << Utilities::translateResult(xrInstance, xrResult);
		return ScopedXrSpace();
	}

	return ScopedXrSpace(xrSpace);
}

ScopedXrSpace Session::createActionSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrAction& xrAction, const XrPath& xrSubactionPath)
{
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(xrAction != XR_NULL_HANDLE);

	if (xrAction == XR_NULL_HANDLE)
	{
		return ScopedXrSpace();
	}

	XrActionSpaceCreateInfo xrActionSpaceCreateInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
	xrActionSpaceCreateInfo.action = xrAction;
	xrActionSpaceCreateInfo.subactionPath = xrSubactionPath;
	xrActionSpaceCreateInfo.poseInActionSpace.orientation.w = 1.0f; // using identity pose

	XrSpace xrSpace = XR_NULL_HANDLE;
	const XrResult xrResult = xrCreateActionSpace(xrSession, &xrActionSpaceCreateInfo, &xrSpace);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create action space: " << Utilities::translateResult(xrInstance, xrResult);
		return ScopedXrSpace();
	}

	return ScopedXrSpace(xrSpace);
}

bool Session::determineExistingColorSpaces(const XrInstance& xrInstance, const XrSession& xrSession, XrColorSpacesFB& xrColorSpacesFB)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSession != XR_NULL_HANDLE);

	PFN_xrEnumerateColorSpacesFB xrEnumerateColorSpacesFB = nullptr;

	XrResult xrResult = xrGetInstanceProcAddr(xrInstance, "xrEnumerateColorSpacesFB", (PFN_xrVoidFunction*)&xrEnumerateColorSpacesFB);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine enumerate color spaces function: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	uint32_t colorSpaceCountOutput = 0u;
	xrResult = xrEnumerateColorSpacesFB(xrSession, 0u, &colorSpaceCountOutput, nullptr);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine number of color spaces: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	xrColorSpacesFB.resize(colorSpaceCountOutput);

	colorSpaceCountOutput = 0u;
	xrResult = xrEnumerateColorSpacesFB(xrSession, uint32_t(xrColorSpacesFB.size()), &colorSpaceCountOutput, xrColorSpacesFB.data());

	if (xrResult != XR_SUCCESS || size_t(colorSpaceCountOutput) != xrColorSpacesFB.size())
	{
		Log::error() << "OpenXR: Failed to determine color spaces: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	return true;
}

bool Session::setColorSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrColorSpaceFB xrColorSpaceFB)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSession != XR_NULL_HANDLE);

	PFN_xrSetColorSpaceFB xrSetColorSpaceFB = nullptr;

	XrResult xrResult = xrGetInstanceProcAddr(xrInstance, "xrSetColorSpaceFB", (PFN_xrVoidFunction*)&xrSetColorSpaceFB);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine set color space function: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	xrResult = xrSetColorSpaceFB(xrSession, xrColorSpaceFB);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to set color space: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	return true;
}

}

}

}
