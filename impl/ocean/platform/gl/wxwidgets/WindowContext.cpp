/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/wxwidgets/WindowContext.h"

#if defined(_WINDOWS)
	#include <gl/wglext.h>
#elif defined(__APPLE__)
	#include <AppKit/AppKit.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace WxWidgets
{

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(WindowContext, wxWindow)
	EVT_SIZE(WindowContext::onResize)
END_EVENT_TABLE()

WindowContext::WindowContext() :
	wxWindow()
{
	// nothing to do here
}

WindowContext::WindowContext(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
	wxWindow(parent, id, pos, size, style, name)
{
	// nothing to do here
}

WindowContext::~WindowContext()
{
	releaseOpenGLContext();
}

void WindowContext::makeCurrent(const bool state)
{

#if defined(_WINDOWS)

	HDC dc = GetDC((HWND)GetHWND());

	if (state)
	{
		if (dc == nullptr || Context::handle_ == nullptr)
		{
			return;
		}

		// OpenGL can operate in the 'creating' thread only
		ocean_assert(Thread::currentThreadId() == threadId_);

		const bool result = wglMakeCurrent(dc, (HGLRC)(Context::handle_)) == TRUE;
		ocean_assert_and_suppress_unused(result, result);
	}
	else
	{
		const bool result = wglMakeCurrent(nullptr, nullptr) == TRUE;
		ocean_assert_and_suppress_unused(result, result);
	}

	ReleaseDC((HWND)GetHWND(), dc);

#elif defined(__APPLE__)

	if (state)
	{
		if (Context::handle_ == nullptr)
		{
			return;
		}

		const CGLError result = CGLSetCurrentContext((CGLContextObj)Context::handle_);
		ocean_assert(result == kCGLNoError);
	}
	else
	{
		const CGLError result = CGLSetCurrentContext(nullptr);
		ocean_assert(result == kCGLNoError);
	}

#else

	#error Missing Implementation.

#endif
}

bool WindowContext::swapBuffers()
{
	// OpenGL can operate in the 'creating' thread only
	ocean_assert(Thread::currentThreadId() == threadId_);

#if defined(_WINDOWS)

	HDC dc = GetDC((HWND)GetHWND());

	SwapBuffers(dc);

	ReleaseDC((HWND)GetHWND(), dc);

	return true;

#elif defined(__APPLE__)

	if (nsContext_ != nullptr)
	{
		[nsContext_ flushBuffer];
	}

	return true;

#else

	#error Missing implementation.

#endif
}

bool WindowContext::createOpenGLContext(const bool initializeOpenGL33, const unsigned int multisamples)
{
#if defined(_WINDOWS)

	HDC dc = GetDC((HWND)GetHWND());
	ocean_assert(dc);

	unsigned int mSamples = multisamples;
	Context::handle_ = windowsCreateOpenGLContextHandle(dc, initializeOpenGL33, mSamples);

	if (Context::handle_ == nullptr)
	{
		return false;
	}

	multisamples_ = mSamples;

#ifdef OCEAN_DEBUG
	ocean_assert(!threadId_.isValid());
	threadId_ = Thread::currentThreadId();
#endif

	ReleaseDC((HWND)GetHWND(), dc);

	makeCurrent();

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (Context::handle_ == nullptr)
	{
		return false;
	}

	return Context::createOpenGLContext(initializeOpenGL33);

#elif defined(__APPLE__)

	ocean_assert(nsContext_ == nullptr);
	ocean_assert(Context::handle_ == nullptr);

	CGLPixelFormatObj pixelFormatObject = nullptr;

	if (initializeOpenGL33 && multisamples >= 2u)
	{
		unsigned int samples = multisamples;

		while (samples != 1u)
		{
			ocean_assert(samples != 0u);

			const CGLPixelFormatAttribute attributes[] =
			{
				kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
				kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
				kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
				kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
				kCGLPFAAccelerated,
				kCGLPFADoubleBuffer,
				kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
				kCGLPFASamples, (CGLPixelFormatAttribute)samples,
				(CGLPixelFormatAttribute)0
			};

			GLint numberVirtualScreens = 0;
			const CGLError result = CGLChoosePixelFormat(attributes, &pixelFormatObject, &numberVirtualScreens);
			ocean_assert(result == kCGLNoError);

			if (pixelFormatObject != nullptr)
			{
				break;
			}

			samples /= 2u;
		}
	}

	if (pixelFormatObject == nullptr)
	{
		const CGLPixelFormatAttribute attributes[] =
		{
			kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)(initializeOpenGL33 ? kCGLOGLPVersion_3_2_Core : kCGLOGLPVersion_Legacy),
			kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
			kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
			kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
			kCGLPFAAccelerated,
			kCGLPFADoubleBuffer,
			(CGLPixelFormatAttribute)0
		};

		GLint numberVirtualScreens = 0;
		const CGLError result = CGLChoosePixelFormat(attributes, &pixelFormatObject, &numberVirtualScreens);
		ocean_assert(result == kCGLNoError);

		if (pixelFormatObject == nullptr)
		{
			return false;
		}
	}

	ocean_assert(pixelFormatObject != nullptr);
	ocean_assert(Context::handle_ == nullptr);

	CGLContextObj contextObject;
	if (CGLCreateContext(pixelFormatObject, nullptr, &contextObject) != kCGLNoError)
	{
		return false;
	}

	Context::handle_ = contextObject;
	ocean_assert(Context::handle_ != nullptr);

	nsContext_ = [NSOpenGLContext alloc];
	[nsContext_ initWithCGLContextObj:(CGLContextObj)Context::handle_];

	NSView* nsView = GetHandle();
	[nsContext_ setView:nsView];

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

#else

	#error Missing Implementation.

#endif

	return false;
}

bool WindowContext::releaseOpenGLContext()
{
#ifdef __APPLE__

	// NSOpenGLContext seems not providing any release function thus we can only decrement the reference counter of the corresponding OpenGL context object
	if (Context::handle_)
	{
		CGLReleaseContext((CGLContextObj)(Context::handle_));
	}
#endif

	return Context::releaseOpenGLContext();
}

void WindowContext::onResize(wxSizeEvent& /*event*/)
{
#ifdef __APPLE__

	// Apple documentation:
	// If you subclass NSView instead of using the NSOpenGLView class, your application must update the rendering context.
	// That's due to a slight difference between the events normally handled by the NSView class and those handled by the NSOpenGLView class.
	if (nsContext_ != nullptr)
	{
		[nsContext_ update];
	}

#endif
}

}

}

}

}
