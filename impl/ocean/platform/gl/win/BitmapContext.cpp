/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/win/BitmapContext.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace Win
{

BitmapContext::BitmapContext(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin) :
	Context(),
	pixelFormat_(pixelFormat),
	pixelOrigin_(pixelOrigin)
{
	setSize(width, height);
}

BitmapContext::~BitmapContext()
{
	releaseOpenGLContext();
}

bool BitmapContext::setSize(const unsigned int width, const unsigned int height)
{
	if (bitmap_.width() == width && bitmap_.height() == height)
	{
		return true;
	}

	releaseOpenGLContext();

	if (width == 0u || height == 0u)
	{
		return true;
	}

	if (pixelFormat_ == FrameType::FORMAT_UNDEFINED || pixelOrigin_ == FrameType::ORIGIN_INVALID)
	{
		return false;
	}

	bitmap_ = Platform::Win::Bitmap(FrameType(width, height, pixelFormat_, pixelOrigin_));
	if (!bitmap_.isValid())
	{
		return false;
	}

	pixelFormat_ = bitmap_.pixelFormat();

	if (!createOpenGLContext(false))
	{
		return false;
	}

	ocean_assert(glGetError() == GL_NO_ERROR);

	makeCurrent();

	ocean_assert(glGetError() == GL_NO_ERROR);

	glViewport(0, 0, (int)width, (int)height);
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

void BitmapContext::makeCurrent(const bool state)
{
	if (state)
	{
		if (!bitmap_.isValid() || Context::handle_ == nullptr)
		{
			return;
		}

		const bool result = wglMakeCurrent(bitmap_.dc(), (HGLRC)(Context::handle_)) == TRUE;
		ocean_assert_and_suppress_unused(result, result);
	}
	else
	{
		const bool result = wglMakeCurrent(nullptr, nullptr) == TRUE;
		ocean_assert_and_suppress_unused(result, result);
	}
}

bool BitmapContext::createOpenGLContext(const bool initializeOpenGL31, const unsigned int multisamples)
{
	ocean_assert(initializeOpenGL31 == false);
	ocean_assert_and_suppress_unused(multisamples <= 1u, multisamples);

	ocean_assert(bitmap_.isValid());
	if (!bitmap_.isValid())
	{
		return false;
	}

	ocean_assert(FrameType::numberPlanes(pixelFormat_) == 1u && FrameType::formatIsGeneric(pixelFormat_));

	const unsigned int bitsPerPixel = FrameType::channels(pixelFormat_) * FrameType::bytesPerDataType(FrameType::dataType(pixelFormat_)) * 8u;

	const PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,
		BYTE(bitsPerPixel),
		0, 0, 0, 0, 0, 0,					// Color Bits Ignored
		0,									// No Alpha Buffer
		0,									// Shift Bit Ignored
		0,									// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		24,									// 24Bit Z-Buffer (Depth Buffer)
		0,									// No Stencil Buffer
		0,									// No Auxiliary Buffer
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved
		0, 0, 0								// Layer Masks Ignored
	};

	const GLuint pixelFormat = ChoosePixelFormat(bitmap_.dc(), &pixelFormatDescriptor);
	if (pixelFormat == 0)
	{
		bitmap_.release();
		return false;
	}

	if (!SetPixelFormat(bitmap_.dc(), pixelFormat, &pixelFormatDescriptor))
	{
		bitmap_.release();
		return false;
	}

	ocean_assert(bitmap_.isValid() && bitmap_.dc());

	ocean_assert(Context::handle_ == nullptr);
	Context::handle_ = wglCreateContext(bitmap_.dc());
	ocean_assert(Context::handle_ != nullptr);

	makeCurrent();

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (Context::handle_ == nullptr)
	{
		return false;
	}

	return Context::createOpenGLContext(initializeOpenGL31);
}

bool BitmapContext::releaseOpenGLContext()
{
	if (!bitmap_.isValid())
	{
		return true;
	}

	const bool result = Context::releaseOpenGLContext();
	bitmap_.release();

	return result;
}

}

}

}

}
