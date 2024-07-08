/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/Passthrough.h"

#include "ocean/platform/openxr/Utilities.h"

namespace Ocean
{

namespace Platform
{

using namespace OpenXR;

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

Passthrough::~Passthrough()
{
	release();
}

bool Passthrough::initialize(const XrInstance& xrInstance, const XrSession& xrSession, const XrPassthroughLayerPurposeFB& xrPassthroughLayerPurposeFB)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSession != XR_NULL_HANDLE);

	if (xrInstance == XR_NULL_HANDLE || xrSession == XR_NULL_HANDLE)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (xrInstance_ != XR_NULL_HANDLE)
	{
		ocean_assert(false && "Passthrough has already been initialized");
		return true;
	}

	ocean_assert(xrSession_ == XR_NULL_HANDLE);
	ocean_assert(xrCreatePassthroughFB_ == nullptr);

	XrResult xrResult = xrGetInstanceProcAddr(xrInstance, "xrCreatePassthroughFB", (PFN_xrVoidFunction*)(&xrCreatePassthroughFB_));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine create passthrough function: " << Utilities::translateResult(xrInstance, xrResult) << ", missing permission or not defining extension?";
		return false;
	}

	bool allSucceeded = true;

	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrDestroyPassthroughFB", (PFN_xrVoidFunction*)(&xrDestroyPassthroughFB_)) == XR_SUCCESS;
	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrPassthroughStartFB", (PFN_xrVoidFunction*)(&xrPassthroughStartFB_)) == XR_SUCCESS;
	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrPassthroughPauseFB", (PFN_xrVoidFunction*)(&xrPassthroughPauseFB_)) == XR_SUCCESS;
	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrCreatePassthroughLayerFB", (PFN_xrVoidFunction*)(&xrCreatePassthroughLayerFB_)) == XR_SUCCESS;
	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrDestroyPassthroughLayerFB", (PFN_xrVoidFunction*)(&xrDestroyPassthroughLayerFB_)) == XR_SUCCESS;
	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrPassthroughLayerResumeFB", (PFN_xrVoidFunction*)(&xrPassthroughLayerResumeFB_)) == XR_SUCCESS;
	allSucceeded = allSucceeded && xrGetInstanceProcAddr(xrInstance, "xrPassthroughLayerSetStyleFB", (PFN_xrVoidFunction*)(&xrPassthroughLayerSetStyleFB_)) == XR_SUCCESS;

	if (!allSucceeded)
	{
		Log::error() << "OpenXR: Failed to determine passthrough functions";
		ocean_assert(false && "This should never happen!");

		release();
		return false;
	}

	XrPassthroughCreateInfoFB xrPassthroughCreateInfoFB{XR_TYPE_PASSTHROUGH_CREATE_INFO_FB};
	xrResult = xrCreatePassthroughFB_(xrSession, &xrPassthroughCreateInfoFB, &xrPassthroughFB_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create passthrough: " << Utilities::translateResult(xrInstance, xrResult);

		release();
		return false;
	}

	XrPassthroughLayerCreateInfoFB xrPassthroughLayerCreateInfoFB = {XR_TYPE_PASSTHROUGH_LAYER_CREATE_INFO_FB};
	xrPassthroughLayerCreateInfoFB.passthrough = xrPassthroughFB_;
	xrPassthroughLayerCreateInfoFB.purpose = xrPassthroughLayerPurposeFB;

	xrResult = xrCreatePassthroughLayerFB_(xrSession, &xrPassthroughLayerCreateInfoFB, &xrPassthroughLayerFB_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create passthrough layer: " << Utilities::translateResult(xrInstance, xrResult);

		release();
		return false;
	}

	xrResult = xrPassthroughLayerResumeFB_(xrPassthroughLayerFB_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to resume passthrough layer: " << Utilities::translateResult(xrInstance, xrResult);

		release();
		return false;
	}

	XrPassthroughStyleFB xrPassthroughStyleFB{XR_TYPE_PASSTHROUGH_STYLE_FB};
	xrPassthroughStyleFB.textureOpacityFactor = 1.0f;
	xrPassthroughStyleFB.edgeColor = {0.0f, 0.0f, 0.0f, 0.0f};

	xrResult = xrPassthroughLayerSetStyleFB_(xrPassthroughLayerFB_, &xrPassthroughStyleFB);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to set passthrough style: " << Utilities::translateResult(xrInstance, xrResult);

		release();
		return false;
	}

	xrInstance_ = xrInstance;
	xrSession_ = xrSession;

	return true;
}

bool Passthrough::start()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid())
	{
		return false;
	}

	if (isStarted_)
	{
		return true;
	}

	ocean_assert(xrPassthroughStartFB_ != nullptr);
	const XrResult xrResult = xrPassthroughStartFB_(xrPassthroughFB_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to start passthrough: " << Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	isStarted_ = true;
	return true;
}

bool Passthrough::pause()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid())
	{
		return false;
	}

	if (!isStarted_)
	{
		return true;
	}

	ocean_assert(xrPassthroughPauseFB_ != nullptr);
	const XrResult xrResult = xrPassthroughPauseFB_(xrPassthroughFB_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to pause passthrough: " << Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	isStarted_ = false;
	return true;
}

bool Passthrough::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return isStarted_;
}

void Passthrough::release()
{
	const ScopedLock scopedLock(lock_);

	if (isStarted_)
	{
		const bool result = pause();
		ocean_assert_and_suppress_unused(result, result);
	}

	if (xrPassthroughLayerFB_ != XR_NULL_HANDLE)
	{
		ocean_assert(xrDestroyPassthroughLayerFB_ != nullptr);

		const XrResult xrResult = xrDestroyPassthroughLayerFB_(xrPassthroughLayerFB_);
		ocean_assert_and_suppress_unused(xrResult == XR_SUCCESS, xrResult);

		xrPassthroughLayerFB_ = XR_NULL_HANDLE;
	}

	if (xrPassthroughFB_ != XR_NULL_HANDLE)
	{
		ocean_assert(xrDestroyPassthroughFB_ != nullptr);

		const XrResult xrResult = xrDestroyPassthroughFB_(xrPassthroughFB_);
		ocean_assert_and_suppress_unused(xrResult == XR_SUCCESS, xrResult);

		xrPassthroughFB_ = XR_NULL_HANDLE;
	}

	xrInstance_ = XR_NULL_HANDLE;
	xrSession_ = XR_NULL_HANDLE;

	xrCreatePassthroughFB_ = nullptr;
	xrDestroyPassthroughFB_ = nullptr;
	xrPassthroughStartFB_ = nullptr;
	xrPassthroughPauseFB_ = nullptr;
	xrCreatePassthroughLayerFB_ = nullptr;
	xrPassthroughLayerResumeFB_ = nullptr;
	xrPassthroughLayerSetStyleFB_ = nullptr;

	isStarted_ = false;
}

Passthrough& Passthrough::operator=(Passthrough&& passthrough)
{
	if (this != &passthrough)
	{
		release();

		xrInstance_ = passthrough.xrInstance_;
		xrSession_ = passthrough.xrSession_;
		passthrough.xrInstance_ = XR_NULL_HANDLE;
		passthrough.xrSession_ = XR_NULL_HANDLE;

		xrPassthroughFB_ = passthrough.xrPassthroughFB_;
		xrPassthroughLayerFB_ = passthrough.xrPassthroughLayerFB_;
		passthrough.xrPassthroughFB_ = XR_NULL_HANDLE;
		passthrough.xrPassthroughLayerFB_ = XR_NULL_HANDLE;

		xrCreatePassthroughFB_ = passthrough.xrCreatePassthroughFB_;
		xrDestroyPassthroughFB_ = passthrough.xrDestroyPassthroughFB_;
		xrPassthroughStartFB_ = passthrough.xrPassthroughStartFB_;
		xrPassthroughPauseFB_ = passthrough.xrPassthroughPauseFB_;
		xrCreatePassthroughLayerFB_ = passthrough.xrCreatePassthroughLayerFB_;
		xrPassthroughLayerResumeFB_ = passthrough.xrPassthroughLayerResumeFB_;
		xrPassthroughLayerSetStyleFB_ = passthrough.xrPassthroughLayerSetStyleFB_;
		passthrough.xrCreatePassthroughFB_ = nullptr;
		passthrough.xrDestroyPassthroughFB_ = nullptr;
		passthrough.xrPassthroughStartFB_ = nullptr;
		passthrough.xrPassthroughPauseFB_ = nullptr;
		passthrough.xrCreatePassthroughLayerFB_ = nullptr;
		passthrough.xrPassthroughLayerResumeFB_ = nullptr;
		passthrough.xrPassthroughLayerSetStyleFB_ = nullptr;

		isStarted_ = passthrough.isStarted_;
		passthrough.isStarted_ = false;
	}

	return *this;
}

const Passthrough::StringSet& Passthrough::necessaryOpenXRExtensionNames()
{
	static const StringSet extensionNames =
	{
		XR_FB_PASSTHROUGH_EXTENSION_NAME
	};

	return extensionNames;
}

}

}

}

}

}
