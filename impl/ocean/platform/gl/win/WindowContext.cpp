/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/win/WindowContext.h"

#include <gl/wglext.h>

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace Win
{

WindowContext::WindowContext(HINSTANCE applicationInstance, const std::wstring& name, const bool isChild) :
	Window(applicationInstance, name, nullptr, isChild)
{
	// nothing to do here
}

WindowContext::~WindowContext()
{
	releaseOpenGLContext();
}

void WindowContext::onInitialized()
{
	Window::onInitialized();
}

void WindowContext::makeCurrent(const bool state)
{
	if (state)
	{
		if (dc_ == nullptr || Context::handle_ == nullptr)
		{
			return;
		}

		const bool result = wglMakeCurrent(dc_, (HGLRC)(Context::handle_)) == TRUE;
		ocean_assert_and_suppress_unused(result, result);
	}
	else
	{
		const bool result = wglMakeCurrent(nullptr, nullptr) == TRUE;
		ocean_assert_and_suppress_unused(result, result);
	}
}

bool WindowContext::swapBuffers()
{
	ocean_assert(dc_ != nullptr);
	SwapBuffers(dc_);
	return true;
}

bool WindowContext::createOpenGLContext(const bool initializeOpenGL33, const unsigned int multisamples)
{
	if (dc_ == nullptr)
	{
		return false;
	}

	unsigned int mSamples = multisamples;
	Context::handle_ = windowsCreateOpenGLContextHandle(dc_, initializeOpenGL33, mSamples);

	if (Context::handle_ == nullptr)
	{
		return false;
	}

	multisamples_ = mSamples;

#ifdef OCEAN_DEBUG
	ocean_assert(!threadId_.isValid());
	threadId_ = Thread::currentThreadId();
#endif

	makeCurrent();

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (Context::handle_ == nullptr)
	{
		return false;
	}

	return Context::createOpenGLContext(initializeOpenGL33);
}

void WindowContext::modifyWindowClass(WNDCLASS& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = nullptr;
}

}

}

}

}
