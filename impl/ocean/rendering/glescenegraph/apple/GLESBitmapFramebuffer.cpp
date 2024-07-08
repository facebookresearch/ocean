/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/apple/GLESBitmapFramebuffer.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESView.h"
#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Apple
{

GLESBitmapFramebuffer::GLESBitmapFramebuffer() :
	GLESFramebuffer(),
	BitmapFramebuffer()
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
	ocean_assert(shareFramebuffer.isNull());

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
	ocean_assert(contextObject_ == nullptr);

	unsigned int samples = 32u;

	CGLPixelFormatObj pixelFormatObject = nullptr;

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

	if (pixelFormatObject == nullptr)
	{
		return false;
	}

	if (CGLCreateContext(pixelFormatObject, nullptr, &contextObject_) != kCGLNoError)
	{
		return false;
	}

	CGLDestroyPixelFormat(pixelFormatObject);

	CGLSetCurrentContext(contextObject_);

	return true;
}

void GLESBitmapFramebuffer::release()
{
	textureFrambuffer_.release();

	GLESFramebuffer::release();

	if (contextObject_ != nullptr)
	{
		CGLReleaseContext(contextObject_);

		contextObject_ = nullptr;
	}
}

}

}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_MACOS
