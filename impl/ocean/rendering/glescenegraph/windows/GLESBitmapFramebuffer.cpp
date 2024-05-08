/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/windows/GLESBitmapFramebuffer.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/platform/win/BitmapWindow.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Windows
{

GLESBitmapFramebuffer::GLESBitmapFramebuffer() :
	GLESFramebuffer(),
	BitmapFramebuffer(),
	contextBitmapWindow_(GetModuleHandle(nullptr), L"GL Internal Window")
{
	initialize();
}

GLESBitmapFramebuffer::~GLESBitmapFramebuffer()
{
	release();
}

void GLESBitmapFramebuffer::setViewport(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height)
{
	if (textureFrambuffer_)
	{
		const bool wasValid = textureFrambuffer_->isValid();

		textureFrambuffer_.force<GLESTextureFramebuffer>().resize(left + width, top + height);

		if (!wasValid)
		{
			textureFrambuffer_.force<GLESTextureFramebuffer>().bindFramebuffer();
		}
	}

	GLESFramebuffer::setViewport(left, top, width, height);
}

void GLESBitmapFramebuffer::render()
{
	textureFrambuffer_.force<GLESTextureFramebuffer>().bindFramebuffer();

	GLESFramebuffer::render();

	textureFrambuffer_.force<GLESTextureFramebuffer>().unbindFramebuffer();
}

bool GLESBitmapFramebuffer::render(Frame& frame, Frame* depthFrame)
{
	if (textureFrambuffer_ && textureFrambuffer_->isValid())
	{
		render();

		if (!textureFrambuffer_.force<GLESTextureFramebuffer>().copyColorTextureToFrame(frame))
		{
			return false;
		}

		if (depthFrame != nullptr)
		{
			if (!textureFrambuffer_.force<GLESTextureFramebuffer>().copyDepthTextureToFrame(*depthFrame))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void GLESBitmapFramebuffer::makeCurrent()
{
	if (textureFrambuffer_ && textureFrambuffer_->isValid())
	{
		textureFrambuffer_.force<GLESTextureFramebuffer>().bindFramebuffer();
	}
}

void GLESBitmapFramebuffer::makeNoncurrent()
{
	if (textureFrambuffer_ && textureFrambuffer_->isValid())
	{
		textureFrambuffer_.force<GLESTextureFramebuffer>().unbindFramebuffer();
	}
}

bool GLESBitmapFramebuffer::initialize(const FramebufferRef& shareFramebuffer)
{
	ocean_assert_and_suppress_unused(shareFramebuffer.isNull(), shareFramebuffer);

	if (!initializeContext() || !initializeOpenGLES())
	{
		release();
		return false;
	}

	textureFrambuffer_ = engine().factory().createTextureFramebuffer();
	textureFrambuffer_.force<GLESTextureFramebuffer>().setMultisamples(16u);

	return true;
}

bool GLESBitmapFramebuffer::initializeContext()
{
	ocean_assert(contextHandle_ == nullptr);

	contextBitmapWindow_.initialize();

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
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
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

	int pixelFormat = ChoosePixelFormat(contextBitmapWindow_.dc(), &pixelFormatDescriptor);
	if (pixelFormat == 0)
	{
		return false;
	}

	if (!SetPixelFormat(contextBitmapWindow_.dc(), pixelFormat, &pixelFormatDescriptor))
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

	contextHandle_ = wglCreateContextAttribsARB_(contextBitmapWindow_.dc(), nullptr, contextAttributes);

	if (contextHandle_ == nullptr)
	{
		// let's try to create the context the old way
		contextHandle_ = wglCreateContext(contextBitmapWindow_.dc());
	}

	if (contextHandle_)
	{
		wglMakeCurrent(contextBitmapWindow_.dc(), contextHandle_);
	}

	return contextHandle_ != nullptr;
}

void GLESBitmapFramebuffer::release()
{
	textureFrambuffer_.release();

	GLESFramebuffer::release();

	if (contextHandle_ != nullptr)
	{
		wglDeleteContext(contextHandle_);
		contextHandle_ = nullptr;
	}
}

bool GLESBitmapFramebuffer::acquireGLFunctions()
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
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
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
