/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gles/EGLContext.h"

namespace Ocean
{

namespace Platform
{

namespace GLES
{

#ifndef __APPLE__

#if !defined( EGL_OPENGL_ES3_BIT_KHR )
	#define EGL_OPENGL_ES3_BIT_KHR 0x0040
#endif

EGLContext::EGLContext() :
	majorVersion_(0),
	minorVersion_(0),
	display_(nullptr),
	config_(nullptr),
	context_(nullptr),
	dummySurface_(nullptr)
{
	// nothing to do here
}

EGLContext::EGLContext(EGLContext&& context) :
	EGLContext()
{
	*this = std::move(context);
}

EGLContext::~EGLContext()
{
	release();
}

bool EGLContext::initialize(const ConfigAttributePairs& configAttributePairs, EGLContext* shareContext)
{
	ocean_assert(display_ == nullptr);
	if (display_ != nullptr)
	{
		return false;
	}

	display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	ocean_assert(display_ != nullptr);
	if (display_ == nullptr)
	{
		ocean_assert(false && "EGLContext::initialize() - eglGetDisplay failed!");
		return false;
	}

	ocean_assert(majorVersion_ == 0 && minorVersion_ == 0);
	if (!eglInitialize(display_, &majorVersion_, &minorVersion_))
	{
		ocean_assert(false && "EGLContext::initialize() - eglInitialize failed!");

		release();
		return false;
	}

	constexpr int maximalConfigs = 1024;
	EGLConfig configs[maximalConfigs];

	EGLint numberConfigs = 0;
	if (eglGetConfigs(display_, configs, maximalConfigs, &numberConfigs) != EGL_TRUE)
	{
		ocean_assert(false && "EGLContext::initialize() - eglGetConfigs failed!");

		release();
		return false;
	}

	ocean_assert(config_ == nullptr);

	for (int n = 0; n < numberConfigs; ++n)
	{
		const EGLConfig& config = configs[n];

		EGLint value = 0;

		if (eglGetConfigAttrib(display_, config, EGL_RENDERABLE_TYPE, &value) != EGL_TRUE)
		{
			ocean_assert(false && "EGLContext::initialize() - eglGetConfigAttrib failed!");

			release();
			return false;
		}

		if ((value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
		{
			continue;
		}

		value = 0;
		if (eglGetConfigAttrib(display_, config, EGL_SURFACE_TYPE, &value) != EGL_TRUE)
		{
			ocean_assert(false && "EGLContext::initialize() - eglGetConfigAttrib failed!");

			release();
			return false;
		}

		if ((value & EGL_WINDOW_BIT) == 0 || (value & EGL_PBUFFER_BIT) == 0)
		{
			continue;
		}

		bool validConfig = true;

		for (const ConfigAttributePair& configAttributePair : configAttributePairs)
		{
			value = 0;
			if (eglGetConfigAttrib(display_, config, configAttributePair.first, &value) != EGL_TRUE)
			{
				ocean_assert(false && "EGLContext::initialize() - eglGetConfigAttrib failed!");

				release();
				return false;
			}

			if (value != configAttributePair.second)
			{
				validConfig = false;
				break;
			}
		}

		if (validConfig)
		{
			config_ = config;
			break;
		}
	}

	if (config_ == nullptr)
	{
		ocean_assert(false && "EGLContext::initialize() - could not find a valid config!");
		return false;
	}

	const EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};

	context_ = eglCreateContext(display_, config_, shareContext, contextAttributes);

	if (context_ == nullptr)
	{
		ocean_assert(false && "EGLContext::initialize() - eglCreateContext failed!");

		release();
		return false;
	}

	const EGLint surfaceAttribs[] =
	{
		EGL_WIDTH, 16,
		EGL_HEIGHT, 16,
		EGL_NONE
	};

	ocean_assert(dummySurface_ == nullptr);
	dummySurface_ = eglCreatePbufferSurface(display_, config_, surfaceAttribs);
	if (dummySurface_ == EGL_NO_SURFACE)
	{
		ocean_assert(false && "EGLContext::initialize() - eglCreatePbufferSurface failed!");

		release();
		return false;
	}

	if (eglMakeCurrent(display_, dummySurface_, dummySurface_, context_) == EGL_FALSE)
	{
		ocean_assert(false && "EGLContext::initialize() - eglMakeCurrent failed!");

		release();
		return false;
	}

	return true;
}

void EGLContext::release()
{
	if (display_ != nullptr)
	{
		if (eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, nullptr /*EGL_NO_CONTEXT*/) != EGL_TRUE)
		{
			ocean_assert(false && "EGLContext::release() - eglMakeCurrent failed!");
		}
	}

	if (context_ != nullptr)
	{
		if (eglDestroyContext(display_, context_) != EGL_TRUE)
		{
			ocean_assert(false && "EGLContext::release() - eglDestroyContext failed!");
		}

		context_ = nullptr;
	}

	if (dummySurface_ != nullptr)
	{
		if (eglDestroySurface(display_, dummySurface_) != EGL_TRUE)
		{
			ocean_assert(false && "EGLContext::release() - eglDestroySurface failed!");
		}

		dummySurface_ = nullptr;
	}

	if (display_ != nullptr)
	{
		if (eglTerminate(display_) != EGL_TRUE)
		{
			ocean_assert(false && "EGLContext::release() - eglTerminate failed!");
		}

		display_ = nullptr;
	}
}

bool EGLContext::isValid() const
{
#ifdef OCEAN_DEBUG
	const bool allValuesInvalid = majorVersion_ == EGLint(0) && minorVersion_ == EGLint(0) && display_ == nullptr && config_ == nullptr && context_ == nullptr && dummySurface_ == nullptr;
	const bool allValuesValid = majorVersion_ != EGLint(0) && display_ != nullptr && config_ != nullptr && context_ != nullptr && dummySurface_ != nullptr;
	ocean_assert(allValuesInvalid || allValuesValid);
#endif

return context_ != nullptr;
}

EGLContext& EGLContext::operator=(EGLContext&& context)
{
	if (this != &context)
	{
		majorVersion_ = context.majorVersion_;
		minorVersion_ = context.minorVersion_;
		display_ = context.display_;
		config_ = context.config_;
		context_ = context.context_;
		dummySurface_ = context.dummySurface_;

		context.majorVersion_ = 0;
		context.minorVersion_ = 0;
		context.display_ = nullptr;
		context.config_ = nullptr;
		context.context_ = nullptr;
		context.dummySurface_ = nullptr;
	}

	return *this;
}

const char* EGLContext::translateError(const EGLint errorValue)
{
	switch (errorValue)
	{
		case EGL_SUCCESS:
			return "EGL_SUCCESS";

		case EGL_NOT_INITIALIZED:
			return "EGL_NOT_INITIALIZED";

		case EGL_BAD_ACCESS:
			return "EGL_BAD_ACCESS";

		case EGL_BAD_ALLOC:
			return "EGL_BAD_ALLOC";

		case EGL_BAD_ATTRIBUTE:
			return "EGL_BAD_ATTRIBUTE";

		case EGL_BAD_CONTEXT:
			return "EGL_BAD_CONTEXT";

		case EGL_BAD_CONFIG:
			return "EGL_BAD_CONFIG";

		case EGL_BAD_CURRENT_SURFACE:
			return "EGL_BAD_CURRENT_SURFACE";

		case EGL_BAD_DISPLAY:
			return "EGL_BAD_DISPLAY";

		case EGL_BAD_SURFACE:
			return "EGL_BAD_SURFACE";

		case EGL_BAD_MATCH:
			return "EGL_BAD_MATCH";

		case EGL_BAD_PARAMETER:
			return "EGL_BAD_PARAMETER";

		case EGL_BAD_NATIVE_PIXMAP:
			return "EGL_BAD_NATIVE_PIXMAP";

		case EGL_BAD_NATIVE_WINDOW:
			return "EGL_BAD_NATIVE_WINDOW";

		case EGL_CONTEXT_LOST:
			return "EGL_CONTEXT_LOST";

		default:
			ocean_assert(false && "Unknown error!");
			return "Unknown";
	}
}

#endif // __APPLE__

}

}

}
