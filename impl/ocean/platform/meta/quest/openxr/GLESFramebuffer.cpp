/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/GLESFramebuffer.h"

#include "ocean/base/String.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

#if !defined(GL_EXT_multisampled_render_to_texture)
	typedef void (GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);
#endif

#if !defined(GL_OVR_multiview)
	typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews);
#endif

#if !defined(GL_OVR_multiview_multisampled_render_to_texture)
	typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseViewIndex, GLsizei numViews);
#endif

#ifndef GL_FRAMEBUFFER_SRGB_EXT
	#define GL_FRAMEBUFFER_SRGB_EXT 0x8DB9
#endif

GLESFramebuffer::GLESFramebuffer(GLESFramebuffer&& framebuffer) :
	GLESFramebuffer()
{
	*this = std::move(framebuffer);
}

GLESFramebuffer::~GLESFramebuffer()
{
	release();
}

bool GLESFramebuffer::initialize(const XrSession& xrSession, const GLenum colorFormat, const unsigned int width, const unsigned int height, const unsigned int multisamples, const bool useStencilBuffer)
{
	ocean_assert(!isValid());
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(colorFormat != GLenum(0) && width != 0u && height != 0u);

	if (isValid() || xrSession == XR_NULL_HANDLE || colorFormat == GLenum(0) || width == 0u || height == 0u)
	{
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress("glRenderbufferStorageMultisampleEXT");
	PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress("glFramebufferTexture2DMultisampleEXT");

#if 0
	// necessary when supporting multiview
	PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC glFramebufferTextureMultiviewOVR =(PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
	PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC glFramebufferTextureMultisampleMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC)eglGetProcAddress("glFramebufferTextureMultisampleMultiviewOVR");
#endif

	uint32_t formatCapacityInput = 0u;
	uint32_t formatCountOutput = 0u;

	XrResult xrResult = xrEnumerateSwapchainFormats(xrSession, formatCapacityInput, &formatCountOutput, nullptr);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine swapchain formats: " << int(xrResult);
		return false;
	}

	std::vector<int64_t> swapchainFormats(formatCountOutput);

	formatCountOutput = 0u;
	xrResult = xrEnumerateSwapchainFormats(xrSession, uint32_t(swapchainFormats.size()), &formatCountOutput, swapchainFormats.data());

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine swapchain formats: " << int(xrResult);
		return false;
	}

	ocean_assert(formatCountOutput == uint32_t(swapchainFormats.size()));
	if (formatCountOutput != uint32_t(swapchainFormats.size()))
	{
		Log::error() << "OpenXR: Wrong number of swapchain formats";
		return false;
	}

#ifdef OCEAN_DEBUG
	{
		Log::debug() << "Found " << swapchainFormats.size() << " supported swapchain formats:";

		std::string debugKnownFormats;
		std::string debugUnknownFormats;

		for (const int64_t swapchainFormat : swapchainFormats)
		{
			std::string debugKnownFormat;

			if (translateSwapchainFormat(swapchainFormat, debugKnownFormat))
			{
				if (!debugKnownFormats.empty())
				{
					debugKnownFormats += ", ";
				}

				debugKnownFormats += debugKnownFormat;
			}
			else
			{
				if (!debugUnknownFormats.empty())
				{
					debugUnknownFormats += ", ";
				}

				debugUnknownFormats += String::toAString(swapchainFormat);
			}
		}

		if (!debugKnownFormats.empty())
		{
			Log::info() << "Known formats: " << debugKnownFormats;
		}

		if (!debugUnknownFormats.empty())
		{
			Log::info() << "Unknown formats: " << debugUnknownFormats;
		}
	}
#endif

	int64_t foundSwapchainFormat = 0;

	for (const int64_t swapchainFormat : swapchainFormats)
	{
		if (GLenum(swapchainFormat) == colorFormat)
		{
			foundSwapchainFormat = swapchainFormat;
			break;
		}
	}

	if (foundSwapchainFormat == 0)
	{
		Log::error() << "OpenXR: Color format not supported";
		return false;
	}

	XrSwapchainCreateInfo xrSwapchainCreateInfo = {XR_TYPE_SWAPCHAIN_CREATE_INFO};
	xrSwapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
	xrSwapchainCreateInfo.format = foundSwapchainFormat;
	xrSwapchainCreateInfo.sampleCount = 1u;
	xrSwapchainCreateInfo.width = width;
	xrSwapchainCreateInfo.height = height;
	xrSwapchainCreateInfo.faceCount = 1u;
	xrSwapchainCreateInfo.arraySize = 1u;
	xrSwapchainCreateInfo.mipCount = 1u;

	xrResult = xrCreateSwapchain(xrSession, &xrSwapchainCreateInfo, &xrSwapchain_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create swapchain: " << int(xrResult);
		return false;
	}

	uint32_t imageCapacityInput = 0u;
	uint32_t imageCountOutput = 0u;

	xrResult = xrEnumerateSwapchainImages(xrSwapchain_, imageCapacityInput, &imageCountOutput, nullptr);

	if (xrResult != XR_SUCCESS || imageCountOutput == 0u)
	{
		Log::error() << "OpenXR: Failed to determine swapchain images: " << int(xrResult);
		return false;
	}

	xrSwapchainImages_.resize(imageCountOutput, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});

	imageCountOutput = 0u;
	xrResult = xrEnumerateSwapchainImages(xrSwapchain_, uint32_t(xrSwapchainImages_.size()), &imageCountOutput, (XrSwapchainImageBaseHeader*)(xrSwapchainImages_.data()));

	if (xrResult != XR_SUCCESS || imageCountOutput != uint32_t(xrSwapchainImages_.size()))
	{
		Log::error() << "OpenXR: Failed to enumerate swapchain images: " << int(xrResult);
		release();
		return false;
	}

	ocean_assert(depthBuffers_.empty() && colorBuffers_.empty());
	depthBuffers_ = std::vector<GLuint>(xrSwapchainImages_.size(), GLuint(0));
	colorBuffers_ = std::vector<GLuint>(xrSwapchainImages_.size(), GLuint(0));

	for (size_t i = 0; i < xrSwapchainImages_.size(); ++i)
	{
		// Create the color buffer texture.
		const GLuint colorTexture = GLuint(xrSwapchainImages_[i].image);
		const GLenum colorTextureTarget = GL_TEXTURE_2D;

		glBindTexture(colorTextureTarget, colorTexture);
		ocean_assert(GL_NO_ERROR == glGetError());


		// Just clamp to edge. However, this requires manually clearing the border
		// around the layer to clear the edge texels.
		glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(colorTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(colorTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindTexture(colorTextureTarget, 0);
		ocean_assert(GL_NO_ERROR == glGetError());

		GLenum renderFramebufferStatus = GLenum(0);

		if (multisamples > 1u && glRenderbufferStorageMultisampleEXT != nullptr && glFramebufferTexture2DMultisampleEXT != nullptr)
		{
			// Create multisampled depth buffer.
			glGenRenderbuffers(1, &depthBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, multisamples, useStencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24, width, height);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			ocean_assert(GL_NO_ERROR == glGetError());


			// Create the frame buffer.
			// NOTE: glFramebufferTexture2DMultisampleEXT only works with GL_FRAMEBUFFER.
			glGenFramebuffers(1, &colorBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindFramebuffer(GL_FRAMEBUFFER, colorBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0, multisamples);
			ocean_assert(GL_NO_ERROR == glGetError());

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, useStencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			renderFramebufferStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindFramebuffer( GL_FRAMEBUFFER, 0);
			ocean_assert(GL_NO_ERROR == glGetError());
		}
		else
		{
			// Create depth buffer.
			glGenRenderbuffers(1, &depthBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glRenderbufferStorage(GL_RENDERBUFFER, useStencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24, width, height);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			ocean_assert(GL_NO_ERROR == glGetError());


			// Create the frame buffer.
			glGenFramebuffers(1, &colorBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, colorBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, useStencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffers_[i]);
			ocean_assert(GL_NO_ERROR == glGetError());

			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
			ocean_assert(GL_NO_ERROR == glGetError());

			renderFramebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
			ocean_assert(GL_NO_ERROR == glGetError());

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		if (renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::info() << "Incomplete frame buffer object!";

			release();
			return false;
		}
	}

	width_ = width;
	height_ = height;
	multisamples_ = multisamples;
	textureSwapChainIndex_ = 0;

	return true;
}

bool GLESFramebuffer::bind()
{
	ocean_assert(isValid());

	ocean_assert(!colorBuffers_.empty() && textureSwapChainIndex_ < xrSwapchainImages_.size());
	if (colorBuffers_.empty() || textureSwapChainIndex_ >= xrSwapchainImages_.size())
	{
		return false;
	}

	ocean_assert(colorBuffers_.size() == xrSwapchainImages_.size());

	const XrSwapchainImageAcquireInfo xrSwapchainImageAcquireInfo = {XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

	uint32_t textureSwapChainIndex = 0u;
	XrResult xrResult = xrAcquireSwapchainImage(xrSwapchain_, &xrSwapchainImageAcquireInfo, &textureSwapChainIndex);

	if (xrResult != XR_SUCCESS)
	{
		return false;
	}

	ocean_assert(textureSwapChainIndex < colorBuffers_.size());

	XrSwapchainImageWaitInfo xrSwapchainImageWaitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
	xrSwapchainImageWaitInfo.timeout = 10000000; // 10ms

	xrResult = xrWaitSwapchainImage(xrSwapchain_, &xrSwapchainImageWaitInfo);

	if (xrResult != XR_SUCCESS)
	{
		Log::warning() << "OpenXR: Failed to wait for swapchain: " << int64_t(xrResult);
		return false;
	}

	textureSwapChainIndex_ = size_t(textureSwapChainIndex);

	ocean_assert(GL_NO_ERROR == glGetError());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, colorBuffers_[textureSwapChainIndex_]);
	ocean_assert(GL_NO_ERROR == glGetError());

	// When GL_FRAMEBUFFER_SRGB is disabled, the system assumes that the color written by the fragment shader is in whatever colorspace the image it is being written to is.
	// Therefore, no colorspace correction is performed.

	glDisable(GL_FRAMEBUFFER_SRGB_EXT);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool GLESFramebuffer::unbind()
{
	ocean_assert(isValid());

	ocean_assert(GL_NO_ERROR == glGetError());

	// we invalidate the depth buffer so that following componenets don't need to copy the data

	const GLenum depthAttachment[1] = {GL_DEPTH_ATTACHMENT};
	glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, depthAttachment);
	ocean_assert(GL_NO_ERROR == glGetError());

	glFlush();
	ocean_assert(GL_NO_ERROR == glGetError());

	const XrSwapchainImageReleaseInfo xrSwapchainImageReleaseInfo = {XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
	const XrResult xrResult = xrReleaseSwapchainImage(xrSwapchain_, &xrSwapchainImageReleaseInfo);

	if (xrResult != XR_SUCCESS)
	{
		Log::warning() << "OpenXR: Failed to release swapchain: " << int64_t(xrResult);
		return false;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

void GLESFramebuffer::release()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (!colorBuffers_.empty())
	{
		glDeleteFramebuffers(colorBuffers_.size(), colorBuffers_.data());
		ocean_assert(GL_NO_ERROR == glGetError());

		colorBuffers_.clear();
	}

	if (!depthBuffers_.empty())
	{
		glDeleteRenderbuffers(depthBuffers_.size(), depthBuffers_.data());
		ocean_assert(GL_NO_ERROR == glGetError());

		depthBuffers_.clear();
	}

	if (xrSwapchain_ != XR_NULL_HANDLE)
	{
		xrDestroySwapchain(xrSwapchain_);

		xrSwapchain_ = XR_NULL_HANDLE;
	}

	xrSwapchainImages_.clear();

	width_ = 0u;
	height_ = 0u;
	multisamples_ = 0u;
}

bool GLESFramebuffer::isValid() const
{
#ifdef OCEAN_DEBUG
	const bool allValuesInvalid = width_ == 0u && height_ == 0u && multisamples_ == 0u && xrSwapchain_ == XR_NULL_HANDLE && xrSwapchainImages_.empty() && textureSwapChainIndex_ == 0u && colorBuffers_.empty() && depthBuffers_.empty();
	const bool allValuesValid = width_ != 0u && height_ != 0u && xrSwapchain_ != XR_NULL_HANDLE && !xrSwapchainImages_.empty() && textureSwapChainIndex_ < xrSwapchainImages_.size() && !colorBuffers_.empty() && !depthBuffers_.empty();
	ocean_assert(allValuesInvalid || allValuesValid);
#endif

	return xrSwapchain_ != XR_NULL_HANDLE;
}

GLESFramebuffer& GLESFramebuffer::operator=(GLESFramebuffer&& framebuffer)
{
	if (this != &framebuffer)
	{
		release();

		width_ = framebuffer.width_;
		height_ = framebuffer.height_;
		multisamples_ = framebuffer.multisamples_;
		framebuffer.width_ = 0u;
		framebuffer.height_ = 0u;
		framebuffer.multisamples_ = 0u;

		xrSwapchain_ = framebuffer.xrSwapchain_;
		xrSwapchainImages_ = std::move(framebuffer.xrSwapchainImages_);
		textureSwapChainIndex_ = framebuffer.textureSwapChainIndex_;
		framebuffer.xrSwapchain_ = XR_NULL_HANDLE;
		framebuffer.textureSwapChainIndex_ = 0;

		colorBuffers_ = std::move(framebuffer.colorBuffers_);
		depthBuffers_ = std::move(framebuffer.depthBuffers_);
	}

	return *this;
}

bool GLESFramebuffer::translateSwapchainFormat(const int64_t swapchainFormat, std::string& readableSwapchainFormat)
{
	typedef std::unordered_map<int64_t, std::string> SwapchainFormatMap;

	static const SwapchainFormatMap swapchainFormatMap =
	{
		// Unsized Internal Format
		{GL_RGB, "GL_RGB"},
		{GL_RGBA, "GL_RGBA"},
		{GL_LUMINANCE_ALPHA, "GL_LUMINANCE_ALPHA"},
		{GL_LUMINANCE, "GL_LUMINANCE"},
		{GL_ALPHA, "GL_ALPHA"},

		// Sized Internal Format
		{GL_R8_SNORM, "GL_R8_SNORM"},
		{GL_R16F, "GL_R16F"},
		{GL_R32F, "GL_R32F"},
		{GL_R8UI, "GL_R8UI"},
		{GL_R8I, "GL_R8I"},
		{GL_R16UI, "GL_R16UI"},
		{GL_R16I, "GL_R16I"},
		{GL_R32UI, "GL_R32UI"},
		{GL_R32I, "GL_R32I"},
		{GL_RG8, "GL_RG8"},
		{GL_RG8_SNORM, "GL_RG8_SNORM"},
		{GL_RG16F, "GL_RG16F"},
		{GL_RG32F, "GL_RG32F"},
		{GL_RG8UI, "GL_RG8UI"},
		{GL_RG8I, "GL_RG8I"},
		{GL_RG16UI, "GL_RG16UI"},
		{GL_RG16I, "GL_RG16I"},
		{GL_RG32UI, "GL_RG32UI"},
		{GL_RG32I, "GL_RG32I"},
		{GL_RGB8, "GL_RGB8"},
		{GL_SRGB8, "GL_SRGB8"},
		{GL_RGB565, "GL_RGB565"},
		{GL_RGB8_SNORM, "GL_RGB8_SNORM"},
		{GL_R11F_G11F_B10F, "GL_R11F_G11F_B10F"},
		{GL_RGB9_E5, "GL_RGB9_E5"},
		{GL_RGB16F, "GL_RGB16F"},
		{GL_RGB32F, "GL_RGB32F"},
		{GL_RGB8UI, "GL_RGB8UI"},
		{GL_RGB8I, "GL_RGB8I"},
		{GL_RGB16UI, "GL_RGB16UI"},
		{GL_RGB16I, "GL_RGB16I"},
		{GL_RGB32UI, "GL_RGB32UI"},
		{GL_RGB32I, "GL_RGB32I"},
		{GL_RGBA8, "GL_RGBA8"},
		{GL_SRGB8_ALPHA8, "GL_SRGB8_ALPHA8"},
		{GL_RGBA8_SNORM, "GL_RGBA8_SNORM"},
		{GL_RGB5_A1, "GL_RGB5_A1"},
		{GL_RGBA4, "GL_RGBA4"},
		{GL_RGB10_A2, "GL_RGB10_A2"},
		{GL_RGBA16F, "GL_RGBA16F"},
		{GL_RGBA32F, "GL_RGBA32F"},
		{GL_RGBA8UI, "GL_RGBA8UI"},
		{GL_RGBA8I, "GL_RGBA8I"},
		{GL_RGB10_A2UI, "GL_RGB10_A2UI"},
		{GL_RGBA16UI, "GL_RGBA16UI"},
		{GL_RGBA16I, "GL_RGBA16I"},
		{GL_RGBA32I, "GL_RGBA32I"},
		{GL_RGBA32UI, "GL_RGBA32UI"},

		// Sized Internal Format
		{GL_DEPTH_COMPONENT16, "GL_DEPTH_COMPONENT16"},
		{GL_DEPTH_COMPONENT24, "GL_DEPTH_COMPONENT24"},
		{GL_DEPTH_COMPONENT32F, "GL_DEPTH_COMPONENT32F"},
		{GL_DEPTH24_STENCIL8, "GL_DEPTH24_STENCIL8"},
		{GL_DEPTH32F_STENCIL8, "GL_DEPTH32F_STENCIL8"}
	};

	const SwapchainFormatMap::const_iterator i = swapchainFormatMap.find(swapchainFormat);

	if (i == swapchainFormatMap.cend())
	{
		return false;
	}

	readableSwapchainFormat = i->second;
	return true;
}

}

}

}

}

}
