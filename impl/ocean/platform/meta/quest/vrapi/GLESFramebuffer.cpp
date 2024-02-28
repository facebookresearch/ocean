// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/GLESFramebuffer.h"

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

namespace VrApi
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

GLESFramebuffer::GLESFramebuffer() :
	width_(0u),
	height_(0u),
	multisamples_(0u),
	colorTextureSwapChain_(nullptr),
	textureSwapChainLength_(0),
	textureSwapChainIndex_(0)
{
	// nothing to do here
}

GLESFramebuffer::GLESFramebuffer(GLESFramebuffer&& framebuffer) :
	GLESFramebuffer()
{
	*this = std::move(framebuffer);
}

GLESFramebuffer::~GLESFramebuffer()
{
	release();
}

bool GLESFramebuffer::initialize(const GLenum colorFormat, const unsigned int width, const unsigned int height, const unsigned int multisamples, const bool useStencilBuffer)
{
	ocean_assert(!isValid());
	ocean_assert(colorFormat != GLenum(0) && width != 0u && height != 0u);

	if (isValid() || colorFormat == GLenum(0) || width == 0u || height == 0u)
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

#if (VRAPI_MAJOR_VERSION == 1 && VRAPI_MINOR_VERSION >= 28) || VRAPI_MAJOR_VERSION >= 2
	colorTextureSwapChain_ = vrapi_CreateTextureSwapChain3(VRAPI_TEXTURE_TYPE_2D, colorFormat, width, height, 1, 3);
#else
	ocean_assert(colorFormat == GL_RGBA8);
	if (colorFormat == GL_RGBA8)
	{
		colorTextureSwapChain_ = vrapi_CreateTextureSwapChain2(VRAPI_TEXTURE_TYPE_2D, VRAPI_TEXTURE_FORMAT_8888_sRGB, width, height, 1, 3);
	}
#endif

	if (colorTextureSwapChain_ == nullptr)
	{
		release();
		return false;
	}

	ocean_assert(textureSwapChainLength_ == 0);
	textureSwapChainLength_ = size_t(vrapi_GetTextureSwapChainLength(colorTextureSwapChain_));

	ocean_assert(depthBuffers_.empty() && colorBuffers_.empty());
	depthBuffers_ = std::vector<GLuint>(textureSwapChainLength_, GLuint(0));
	colorBuffers_ = std::vector<GLuint>(textureSwapChainLength_, GLuint(0));

	for (size_t i = 0; i < textureSwapChainLength_; ++i)
	{
		// Create the color buffer texture.
		const GLuint colorTexture = vrapi_GetTextureSwapChainHandle(colorTextureSwapChain_, int(i));
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

void GLESFramebuffer::bind()
{
	ocean_assert(isValid());

	ocean_assert(!colorBuffers_.empty() && textureSwapChainIndex_ < textureSwapChainLength_);
	if (colorBuffers_.empty() || textureSwapChainIndex_ >= textureSwapChainLength_)
	{
		return;
	}

	ocean_assert(GL_NO_ERROR == glGetError());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, colorBuffers_[textureSwapChainIndex_]);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESFramebuffer::swap()
{
	ocean_assert(isValid());
	ocean_assert(textureSwapChainIndex_ < textureSwapChainLength_);

	textureSwapChainIndex_ = (textureSwapChainIndex_ + 1) % textureSwapChainLength_;
}

void GLESFramebuffer::invalidateDepthBuffer()
{
	ocean_assert(isValid());

	ocean_assert(GL_NO_ERROR == glGetError());

	const GLenum depthAttachment[1] = {GL_DEPTH_ATTACHMENT};
	glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, depthAttachment);
	ocean_assert(GL_NO_ERROR == glGetError());

	glFlush();
	ocean_assert(GL_NO_ERROR == glGetError());
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

	if (colorTextureSwapChain_ != nullptr)
	{
		vrapi_DestroyTextureSwapChain(colorTextureSwapChain_);

		colorTextureSwapChain_ = nullptr;
	}

	textureSwapChainLength_ = 0;

	width_ = 0u;
	height_ = 0u;
	multisamples_ = 0u;
}

bool GLESFramebuffer::isValid() const
{
#ifdef OCEAN_DEBUG
	const bool allValuesInvalid = width_ == 0u && height_ == 0u && multisamples_ == 0u && colorTextureSwapChain_ == nullptr && textureSwapChainLength_ == 0 && textureSwapChainIndex_ == 0u && colorBuffers_.empty() && depthBuffers_.empty();
	const bool allValuesValid = width_ != 0u && height_ != 0u && colorTextureSwapChain_ != nullptr && textureSwapChainLength_ != 0 && textureSwapChainIndex_ < textureSwapChainLength_ && !colorBuffers_.empty() && !depthBuffers_.empty();
	ocean_assert(allValuesInvalid || allValuesValid);
#endif

	return colorTextureSwapChain_ != nullptr;
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

		colorTextureSwapChain_ = framebuffer.colorTextureSwapChain_;
		textureSwapChainLength_ = framebuffer.textureSwapChainLength_;
		textureSwapChainIndex_ = framebuffer.textureSwapChainIndex_;
		framebuffer.colorTextureSwapChain_ = nullptr;
		framebuffer.textureSwapChainLength_ = 0;
		framebuffer.textureSwapChainIndex_ = 0;

		colorBuffers_ = std::move(framebuffer.colorBuffers_);
		depthBuffers_ = std::move(framebuffer.depthBuffers_);
	}

	return *this;
}

}

}

}

}

}
