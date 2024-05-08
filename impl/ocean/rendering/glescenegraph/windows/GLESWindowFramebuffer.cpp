/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/windows/GLESWindowFramebuffer.h"

#include "ocean/base/String.h"

#include "ocean/platform/win/BitmapWindow.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Windows
{

GLESWindowFramebuffer::GLESWindowFramebuffer() :
	GLESFramebuffer(),
	WindowFramebuffer()
{
	// nothing to do here
}

GLESWindowFramebuffer::~GLESWindowFramebuffer()
{
	release();
}

bool GLESWindowFramebuffer::initializeById(const size_t id, const FramebufferRef& shareFramebuffer)
{
	ocean_assert(id != size_t(0));
	ocean_assert_and_suppress_unused(shareFramebuffer.isNull() && "Currently we do not allow resource sharing", shareFramebuffer);

	const ScopedLock scopedLock(objectLock);

	if (windowHandle_ != nullptr)
	{
		throw OceanException("The framebuffer already has been connected to a window.");
	}

	windowHandle_ = HWND(id);
	windowDC_ = GetDC(windowHandle_);
	ocean_assert(windowDC_ != nullptr);

	return initialize();
}

bool GLESWindowFramebuffer::isAntialiasingSupported(const unsigned int buffers) const
{
	return buffers <= 16u;
}

bool GLESWindowFramebuffer::isAntialiasing() const
{
	const ScopedLock scopedLock(objectLock);

	return antialiasingBuffers_ != 0u;
}

void GLESWindowFramebuffer::setPreferredPixelFormat(const FrameType::PixelFormat pixelFormat)
{
	framebufferPreferredPixelFormat = pixelFormat;
}

bool GLESWindowFramebuffer::setSupportAntialiasing(const unsigned int buffers)
{
	const ScopedLock scopedLock(objectLock);

	if (antialiasingBuffers_ == buffers)
	{
		return true;
	}

	antialiasingBuffers_ = buffers;

	if (contextHandle_ == nullptr)
	{
		antialiasingBuffers_ = buffers;
		return true;
	}

	const unsigned int preferredAntialiasingBuffers = antialiasingBuffers_;
	const HWND windowHandle = windowHandle_;

	release();

	if (!initializeById(size_t(windowHandle)))
	{
		return false;
	}

	return preferredAntialiasingBuffers == antialiasingBuffers_;
}

void GLESWindowFramebuffer::render()
{
	if (contextHandle_ == nullptr)
	{
		ocean_assert(false && "Invalid render framebuffer.");
		return;
	}

	GLESFramebuffer::render();

	SwapBuffers(windowDC_);
}

void GLESWindowFramebuffer::makeCurrent()
{
	const bool result = wglMakeCurrent(windowDC_, contextHandle_) == TRUE;
	ocean_assert_and_suppress_unused(result, result);

	ocean_assert(result);
}

bool GLESWindowFramebuffer::initializeContext()
{
	ocean_assert(windowDC_ != nullptr);
	ocean_assert(contextHandle_ == nullptr);

	if (!acquireGLFunctions())
	{
		return false;
	}

	ocean_assert(wglCreateContextAttribsARB_);
	ocean_assert(wglChoosePixelFormatARB_);

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,					// Color Bits Ignored
		8,									// No Alpha Buffer
		0,									// Shift Bit Ignored
		0,									// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		24,									// 24Bit Z-Buffer (Depth Buffer)
		8,									// No Stencil Buffer
		0,									// No Auxiliary Buffer
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved
		0, 0, 0								// Layer Masks Ignored
	};

	int pixelFormat = 0u;

	if (antialiasingBuffers_ >= 2u)
	{
		unsigned int samples = antialiasingBuffers_;

		while (samples != 1u)
		{
			ocean_assert(samples != 0u);

			const int iAttributes[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB, 24,
				WGL_ALPHA_BITS_ARB, 8,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 0,
				WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
				WGL_SAMPLES_ARB, int(samples),
				0, 0
			};

			const float fAttributes[] =
			{
				0.0f, 0.0f
			};

			int pixelFormatARB = 0;
			UINT validFormats = 0u;
			if (wglChoosePixelFormatARB_(windowDC_, iAttributes, fAttributes, 1, &pixelFormatARB, &validFormats) == TRUE)
			{
				pixelFormat = pixelFormatARB;
				antialiasingBuffers_ = samples;

				DescribePixelFormat(windowDC_, pixelFormat, sizeof(pixelFormatDescriptor), &pixelFormatDescriptor);
				break;
			}

			samples /= 2u;
		}
	}

	if (pixelFormat == 0)
	{
		pixelFormat = ChoosePixelFormat(windowDC_, &pixelFormatDescriptor);
		if (pixelFormat == 0)
		{
			return false;
		}

		antialiasingBuffers_ = 1u;
	}

	ocean_assert(pixelFormat != 0u);
	if (!SetPixelFormat(windowDC_, pixelFormat, &pixelFormatDescriptor))
	{
		return false;
	}

	// we use OpenGL 3.3
	const int contextAttributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	contextHandle_ = wglCreateContextAttribsARB_(windowDC_, nullptr, contextAttributes);

	if (contextHandle_ == nullptr)
	{
		// let's try to create the context the old way
		contextHandle_ = wglCreateContext(windowDC_);
	}

	if (contextHandle_)
	{
		wglMakeCurrent(windowDC_, contextHandle_);
	}

	return contextHandle_ != nullptr;
}

void GLESWindowFramebuffer::release()
{
	GLESFramebuffer::release();

	if (contextHandle_ != nullptr)
	{
		wglDeleteContext(contextHandle_);
	}

	ReleaseDC(windowHandle_, windowDC_);
	windowDC_ = nullptr;
	windowHandle_ = nullptr;
}

bool GLESWindowFramebuffer::acquireGLFunctions()
{
	if (wglCreateContextAttribsARB_ != nullptr && wglChoosePixelFormatARB_ != nullptr)
	{
		return true;
	}

	Platform::Win::BitmapWindow dummyWindow(GetModuleHandle(nullptr), L"GL Dummy Window");
	dummyWindow.initialize();

	if (wglCreateContextAttribsARB_ == nullptr)
	{
		// we create an intermediate context to get access to the advanced context creation function wglCreateContextAttribsARB

		const PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW  | PFD_SUPPORT_OPENGL, PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,					// Color Bits Ignored
			8,									// No Alpha Buffer
			0,									// Shift Bit Ignored
			0,									// No Accumulation Buffer
			0, 0, 0, 0,							// Accumulation Bits Ignored
			24,									// 24Bit Z-Buffer (Depth Buffer)
			8,									// No Stencil Buffer
			0,									// No Auxiliary Buffer
			PFD_MAIN_PLANE,						// Main Drawing Layer
			0,									// Reserved
			0, 0, 0								// Layer Masks Ignored
		};

		const int pixelFormat = ChoosePixelFormat(dummyWindow.dc(), &pixelFormatDescriptor);
		if (pixelFormat == 0)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!SetPixelFormat(dummyWindow.dc(), pixelFormat, &pixelFormatDescriptor))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		HGLRC dummyContext = wglCreateContext(dummyWindow.dc());

		if (dummyContext == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (wglMakeCurrent(dummyWindow.dc(), dummyContext) != TRUE)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
		ocean_assert(glGetError() == GL_NO_ERROR);

		// now we can initialize all GL 2.0+ function
		if (!Ocean::Rendering::GLESceneGraph::GLESDynamicLibrary::initialize())
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		wglCreateContextAttribsARB_ = PFNWGLCREATECONTEXTATTRIBSARBPROC(wglGetProcAddress("wglCreateContextAttribsARB"));

		if (wglCreateContextAttribsARB_ == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (wglMakeCurrent(nullptr, nullptr) != TRUE)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		// we have the function pointer, so that we can delete the dummy context

		if (wglDeleteContext(dummyContext) != TRUE)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	ocean_assert(wglCreateContextAttribsARB_ != nullptr);

	if (wglChoosePixelFormatARB_ == nullptr)
	{
		// recreate the dummy context again, now with a specific OpenGL version

		// we use OpenGL 3.3
		const int contextAttributes[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

		HGLRC dummyContext = wglCreateContextAttribsARB_(dummyWindow.dc(), nullptr, contextAttributes);

		if (dummyContext == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (wglMakeCurrent(dummyWindow.dc(), dummyContext) != TRUE)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		wglChoosePixelFormatARB_ = (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));

		if (wglChoosePixelFormatARB_ == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (wglMakeCurrent(nullptr, nullptr) != TRUE)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		// we have the function pointer, so that we can delete the dummy context

		if (wglDeleteContext(dummyContext) != TRUE)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	ocean_assert(wglChoosePixelFormatARB_ != nullptr);

	return true;
}

}

}

}

}
