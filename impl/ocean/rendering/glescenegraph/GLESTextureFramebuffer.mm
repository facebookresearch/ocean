/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	#include <OpenGLES/EAGL.h>
#endif

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

bool GLESTextureFramebuffer::createTextureForFramebufferAppleIOS(const unsigned int width, const unsigned int height, const GLint textureInternalFormat, const GLenum textureFormat, const GLenum textureType)
{
	OSType bufferPixelFormat = kCVPixelFormatType_24RGB;

	switch (textureInternalFormat)
	{
		case GL_RGB:
			break;

		case GL_RGBA:
			bufferPixelFormat = kCVPixelFormatType_32BGRA;
			break;

		case GL_R8:
			bufferPixelFormat = kCVPixelFormatType_OneComponent8;
			break;

		case GL_LUMINANCE_ALPHA:
			bufferPixelFormat = kCVPixelFormatType_TwoComponent8;
			break;

		case GL_R32UI:
			// the pixel format has the wrong number of channels, but 32 bit - so that we can use it
			bufferPixelFormat = kCVPixelFormatType_32BGRA;
			break;

		case GL_R32F:
			bufferPixelFormat = kCVPixelFormatType_OneComponent32Float;
			break;

		default:
			return false;
	}

	const CVEAGLContext eaglContext = [EAGLContext currentContext];

	if (eaglContext == nullptr)
	{
		Log::error() << "Failed to create EAGLContext";
		return false;
	}

	ocean_assert(iosTextureCache_ == nullptr);
	CVReturn cvReturn = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, nullptr, eaglContext, nullptr, &iosTextureCache_);

	if (cvReturn != kCVReturnSuccess)
	{
		Log::error() << "Failed to create texture cache: " << int(cvReturn);
		return false;
	}

	CFMutableDictionaryRef attributes;
	CFDictionaryRef empty = CFDictionaryCreate(kCFAllocatorDefault, nullptr, nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	attributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(attributes, kCVPixelBufferIOSurfacePropertiesKey, empty);

	ocean_assert(iosPixelBuffer_ == nullptr);
	cvReturn = CVPixelBufferCreate(kCFAllocatorDefault, width, height, bufferPixelFormat, attributes, &iosPixelBuffer_);

	if (cvReturn != kCVReturnSuccess)
	{
		Log::error() << "Failed to create pixel buffer: " << int(cvReturn);
		return false;
	}

	ocean_assert(iosTexture_ == nullptr);
	cvReturn = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, iosTextureCache_, iosPixelBuffer_, nullptr, GL_TEXTURE_2D, textureInternalFormat, width, height, textureFormat, textureType, 0, &iosTexture_);

	if (cvReturn != kCVReturnSuccess)
	{
		Log::error() << "Failed to create texture framebuffer with cache: " << int(cvReturn);
		return false;
	}

	ocean_assert(colorTextureId_ == 0u);
	colorTextureId_ = CVOpenGLESTextureGetName(iosTexture_);

	return true;
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

}

}

}
