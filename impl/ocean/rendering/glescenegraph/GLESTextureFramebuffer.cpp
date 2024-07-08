/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTextureFramebuffer::GLESTextureFramebuffer() :
	GLESTexture(),
	TextureFramebuffer()
{
	// nothing to do here
}

GLESTextureFramebuffer::~GLESTextureFramebuffer()
{
	release();
}

std::string GLESTextureFramebuffer::textureName() const
{
	const ScopedLock scopedLock(objectLock);

	return textureName_;
}

bool GLESTextureFramebuffer::setTextureName(const std::string& name)
{
	const ScopedLock scopedLock(objectLock);

	textureName_ = name;

	return true;
}

bool GLESTextureFramebuffer::resize(const unsigned int width, const unsigned int height)
{
	ocean_assert(width >= 1u && height >= 1u);

	if (width == width_ && height == height_ && framebufferObjectId_ != 0u && colorTextureId_ != 0u && depthTextureId_ != 0u)
	{
		return true;
	}

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (framebufferObjectId_ == 0u)
	{
		glGenFramebuffers(1, &framebufferObjectId_);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(framebufferObjectId_ != 0u);

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId_);
	ocean_assert(glGetError() == GL_NO_ERROR);

	GLint textureInternalFormat = GL_RGB;
	GLenum textureFormat = GL_RGB;

	GLenum textureType = GL_UNSIGNED_BYTE;

	switch (pixelFormat_)
	{
		case FrameType::FORMAT_RGB24:
			break;

		case FrameType::FORMAT_RGBA32:
			textureInternalFormat = GL_RGBA;
			textureFormat = GL_RGBA;
			break;

		case FrameType::FORMAT_Y8:
#ifdef OCEAN_RENDERING_GLES_USE_ES
			textureInternalFormat = GL_R8;
			textureFormat = GL_RED;
#else
			textureInternalFormat = GL_RED;
			textureFormat = GL_RED;
#endif
			break;

		case FrameType::FORMAT_YA16:
#ifdef OCEAN_RENDERING_GLES_USE_ES
			textureInternalFormat =  GL_LUMINANCE_ALPHA;
			textureFormat =   GL_LUMINANCE_ALPHA;
#else
			textureInternalFormat = GL_RG;
			textureFormat = GL_RG;
#endif
			break;

		case FrameType::FORMAT_Y32:
			textureInternalFormat = GL_R32UI;
			textureFormat = GL_RED_INTEGER;
			textureType = GL_UNSIGNED_INT;
			break;

		case FrameType::FORMAT_F32:
#ifdef OCEAN_RENDERING_GLES_USE_ES
			textureInternalFormat = GL_R32F;
			textureFormat =  GL_RED;
#else
			textureInternalFormat = GL_R32F;
			textureFormat = GL_RED;
#endif
			textureType =  GL_FLOAT;
			break;

		default:
			ocean_assert(false && "This must never happen!");
			break;
	}

#ifdef OCEAN_RENDERING_GLES_USE_ES
	const GLenum textureTarget = GL_TEXTURE_2D;
#else
	const GLenum textureTarget = framebufferMultisamples_ > 1u ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
#endif

	if (framebufferMultisamples_ > 1u)
	{
		GLint maximalSamples = 1;
		glGetIntegerv(GL_MAX_SAMPLES, &maximalSamples);
		ocean_assert(glGetError() == GL_NO_ERROR);

		if (framebufferMultisamples_ > (unsigned int)(maximalSamples))
		{
			framebufferMultisamples_ = (unsigned int)(maximalSamples);
		}
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (framebufferMultisamples_ == 1u && createTextureForFramebufferAppleIOS(width, height, textureInternalFormat, textureFormat, textureType))
	{
		ocean_assert(colorTextureId_ != 0u);
		glBindTexture(textureTarget, colorTextureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	{
		if (colorTextureId_ == 0u)
		{
			glGenTextures(1, &colorTextureId_);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}

		ocean_assert(colorTextureId_ != 0u);

		glBindTexture(textureTarget, colorTextureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);

#ifndef OCEAN_RENDERING_GLES_USE_ES
		if (framebufferMultisamples_ > 1u)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, GLsizei(framebufferMultisamples_), textureInternalFormat, width, height, GL_TRUE);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}
		else
#endif // OCEAN_RENDERING_GLES_USE_ES
		{
			glTexImage2D(GL_TEXTURE_2D, 0, textureInternalFormat, width, height, 0, textureFormat, textureType, nullptr);
			ocean_assert(glGetError() == GL_NO_ERROR);
		}
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorTextureId_, 0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glBindTexture(textureTarget, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	ocean_assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	if (depthTextureId_ == 0u)
	{
		glGenTextures(1, &depthTextureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	ocean_assert(depthTextureId_ != 0u);

	glBindTexture(textureTarget, depthTextureId_);
	ocean_assert(glGetError() == GL_NO_ERROR);

#ifndef OCEAN_RENDERING_GLES_USE_ES
	if (framebufferMultisamples_ > 1u)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, GLsizei(framebufferMultisamples_), GL_DEPTH_COMPONENT32F, width, height, GL_TRUE);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}
	else
#endif // OCEAN_RENDERING_GLES_USE_ES
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	glBindTexture(textureTarget, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureTarget, depthTextureId_, 0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	const GLenum value = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0u);

	ocean_assert(value == GL_FRAMEBUFFER_COMPLETE);
	if (value != GL_FRAMEBUFFER_COMPLETE)
	{
		release();
		return false;
	}

	width_ = width;
	height_ = height;

	return true;
}

FrameType::PixelFormat GLESTextureFramebuffer::pixelFormat() const
{
	return pixelFormat_;
}

GLESTextureFramebuffer::WrapType GLESTextureFramebuffer::wrapTypeS() const
{
	const ScopedLock scopedLock(objectLock);

	return wrapTypeS_;
}

GLESTextureFramebuffer::WrapType GLESTextureFramebuffer::wrapTypeT() const
{
	const ScopedLock scopedLock(objectLock);

	return wrapTypeT_;
}

bool GLESTextureFramebuffer::setPixelFormat(const FrameType::PixelFormat pixelFormat)
{
	const ScopedLock scopedLock(objectLock);

	if (pixelFormat_ == pixelFormat)
	{
		return true;
	}

	switch (pixelFormat)
	{
		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_Y8:
		case FrameType::FORMAT_YA16:
		case FrameType::FORMAT_Y32:
		case FrameType::FORMAT_F32:
		{
			pixelFormat_ = pixelFormat;

			if (width_ >= 1u && height_ >= 1u)
			{
				// forcing texture to be re-created

				const unsigned int oldWidth = width_;
				const unsigned int oldHeight = height_;

				width_ = 0u;
				height_ = 0u;

				return resize(oldWidth, oldHeight);
			}
			else
			{
				return true;
			}
		}

		default:
			break;
	}

	return false;
}

bool GLESTextureFramebuffer::setWrapTypeS(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeS_ = type;
	return true;
}

bool GLESTextureFramebuffer::setWrapTypeT(const WrapType type)
{
	const ScopedLock scopedLock(objectLock);

	wrapTypeT_ = type;
	return true;
}

bool GLESTextureFramebuffer::setMultisamples(const unsigned int multisamples)
{
	ocean_assert(multisamples >= 1u);

	const ScopedLock scopedLock(objectLock);

#ifdef OCEAN_RENDERING_GLES_USE_ES

	if (multisamples != 1u)
	{
		return false;
	}

	framebufferMultisamples_ = multisamples;

	return true;

#else

	if (framebufferMultisamples_ == multisamples)
	{
		return true;
	}

	framebufferMultisamples_ = multisamples;

	if (width_ >= 1u && height_ >= 1u)
	{
		// forcing texture to be re-created

		const unsigned int oldWidth = width_;
		const unsigned int oldHeight = height_;

		width_ = 0u;
		height_ = 0u;

		return resize(oldWidth, oldHeight);
	}

	return true;

#endif // OCEAN_RENDERING_GLES_USE_ES
}

unsigned int GLESTextureFramebuffer::bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(colorTextureId_ != 0u);
	ocean_assert(shaderProgram.isCompiled());
	ocean_assert(id < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

	glUseProgram(shaderProgram.id());
	ocean_assert(GL_NO_ERROR == glGetError());

	glActiveTexture(GLenum(GL_TEXTURE0 + id));
	ocean_assert(GL_NO_ERROR == glGetError());

#ifdef OCEAN_RENDERING_GLES_USE_ES
	const GLenum textureTarget = GL_TEXTURE_2D;
#else
	const GLenum textureTarget = framebufferMultisamples_ > 1u ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
#endif

	glBindTexture(textureTarget, colorTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	if (framebufferMultisamples_ <= 1u)
	{
		const GLenum glesMinificationFilterMode = translateMinificationFilterMode(minificationFilterMode_);
		const GLenum glesMagnificationFilterMode = translateMagnificationFilterMode(magnificationFilterMode_);

		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, glesMinificationFilterMode);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, glesMagnificationFilterMode);
		ocean_assert(GL_NO_ERROR == glGetError());

		const GLenum glesWrapTypeS = translateWrapType(wrapTypeS_);
		const GLenum glesWrapTypeT = translateWrapType(wrapTypeT_);

		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, glesWrapTypeS);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, glesWrapTypeT);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	const GLint locationTextureTransformMatrix = glGetUniformLocation(shaderProgram.id(), "textureTransformationMatrix");
	if (locationTextureTransformMatrix != -1)
	{
		setUniform(locationTextureTransformMatrix, transformation_);
	}

	ocean_assert(!textureName_.empty());
	const GLint locationTexture = glGetUniformLocation(shaderProgram.id(), textureName_.c_str());
	if (locationTexture != -1)
	{
		setUniform(locationTexture, int(id));
	}

	const GLint locationTextureOriginLowerLeft = glGetUniformLocation(shaderProgram.id(), "textureOriginLowerLeft");
	if (locationTextureOriginLowerLeft != -1)
	{
		setUniform(locationTextureOriginLowerLeft, 1);
	}

	return 1u;
}

bool GLESTextureFramebuffer::bindFramebuffer()
{
	ocean_assert(framebufferObjectId_ != 0u);
	if (framebufferObjectId_ == 0u)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId_);
	ocean_assert(glGetError() == GL_NO_ERROR);

	return true;
}

void GLESTextureFramebuffer::unbindFramebuffer()
{
	ocean_assert(glGetError() == GL_NO_ERROR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0u);
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (useMipmap_)
	{
		createMipmap();
	}
}

bool GLESTextureFramebuffer::copyColorTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion)
{
	unsigned int subRegionLeft = 0u;
	unsigned int subRegionTop = 0u;
	unsigned int subRegionWidth = width_;
	unsigned int subRegionHeight = height_;

	if (subRegion.isValid())
	{
		if (subRegion.rightEnd() > width_ || subRegion.bottomEnd() > height_)
		{
			ocean_assert(false && "Invalid sub-region!");
			return false;
		}

		subRegionLeft = subRegion.left();
		subRegionTop = subRegion.top();
		subRegionWidth = subRegion.width();
		subRegionHeight = subRegion.height();
	}

	const FrameType frameType(subRegionWidth, subRegionHeight, pixelFormat_, FrameType::ORIGIN_LOWER_LEFT);

	if (!frameType.isValid())
	{
		return false;
	}

	if (framebufferMultisamples_ == 1u)
	{
		if (!frame.set(frameType, false /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		GLenum textureFormat = GL_RGB;
		GLenum textureType = GL_UNSIGNED_BYTE;

		switch (pixelFormat_)
		{
			case FrameType::FORMAT_RGB24:
				break;

			case FrameType::FORMAT_RGBA32:
				textureFormat = GL_RGBA;
				break;

			case FrameType::FORMAT_Y8:
				textureFormat = GL_RED;
				break;

			case FrameType::FORMAT_YA16:
				textureFormat = GL_RG;
				break;

			case FrameType::FORMAT_Y32:
				textureFormat = GL_RED_INTEGER;
				textureType = GL_UNSIGNED_INT;
				break;

			case FrameType::FORMAT_F32:
#ifdef OCEAN_RENDERING_GLES_USE_ES
				ocean_assert(false && "OpenGLES does not support to read float framebuffers, use the Y32 workaround instead");
				return false;
#else
				textureFormat = GL_RED;
				textureType = GL_FLOAT;
#endif
				break;

			default:
				ocean_assert(false && "This must never happen!");
				break;
		}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
		if (iosPixelBuffer_ != nullptr)
		{
			glFinish();
			ocean_assert(GL_NO_ERROR == glGetError());

			const CVReturn status = CVPixelBufferLockBaseAddress(iosPixelBuffer_, kCVPixelBufferLock_ReadOnly);

			ocean_assert(status == kCVReturnSuccess);
			if (status == kCVReturnSuccess)
			{
				const void* data = CVPixelBufferGetBaseAddressOfPlane(iosPixelBuffer_, 0);

				if (subRegionWidth != width_ || subRegionHeight != height_)
				{
					const Frame fullFrame(FrameType(width_, height_, pixelFormat_, FrameType::ORIGIN_LOWER_LEFT), data, Frame::CM_USE_KEEP_LAYOUT, 0u);

					frame.copy(0u, 0u, fullFrame.subFrame(subRegionLeft, subRegionTop, subRegionWidth, subRegionHeight, Frame::CM_USE_KEEP_LAYOUT));
				}
				else
				{
					memcpy(frame.data<void>(), data, frame.size());
				}

				CVPixelBufferUnlockBaseAddress(iosPixelBuffer_, kCVPixelBufferLock_ReadOnly);

				return true;
			}
		}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		ocean_assert(GL_NO_ERROR == glGetError());

		glReadPixels(GLint(subRegionLeft), GLint(subRegionTop), subRegionWidth, subRegionHeight, textureFormat, textureType, frame.data<void>());

		const GLenum result = glGetError();
		ocean_assert(result == GL_NO_ERROR);

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);
		ocean_assert(GL_NO_ERROR == glGetError());

		return result == GL_NO_ERROR;
	}
#ifndef OCEAN_RENDERING_GLES_USE_ES
	else
	{
		// we need to render the multi sample texture into s single sample texture before we can copy the image content

		if (textureFramebufferOneSample_.isNull())
		{
			textureFramebufferOneSample_ = engine().factory().createTextureFramebuffer();
		}

		GLESTextureFramebuffer& textureFramebufferOneSample = textureFramebufferOneSample_.force<GLESTextureFramebuffer>();

		textureFramebufferOneSample.setPixelFormat(pixelFormat_);
		textureFramebufferOneSample.resize(width_, height_);

		if (shaderProgramForOneSample_.isNull())
		{
			shaderProgramForOneSample_ = engine().factory().createShaderProgram();

			const std::string vertexShaderCode =
			R"SHADER(#version 330

				// Vertex attribute
				in vec4 aVertex;

				// Texture coordinate attribute
				in vec4 aTextureCoordinate;

				// Resulting texture coordinate
				out vec2 vTextureCoordinate;

				void main(void)
				{
					gl_Position = aVertex;
					vTextureCoordinate = aTextureCoordinate.xy;
				}
			)SHADER";

			const std::string fragmentShaderCode =
			R"SHADER(#version 330

				// Input texture coordinate
				in vec2 vTextureCoordinate;

				// Texture sampler object
				uniform sampler2DMS primaryTexture;

				// The number of multi samples
				uniform int numberSamples;

				// The out fragment color
				out vec4 fragColor;

				void main()
				{
					ivec2 coordinate = ivec2(vTextureCoordinate * textureSize(primaryTexture));

					vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

					for (int i = 0; i < numberSamples; ++i)
					{
						color += texelFetch(primaryTexture, coordinate, i);
					}

					color /= float(numberSamples);

					fragColor = color;
				}
			)SHADER";

			std::string errorMessage;
			if (!shaderProgramForOneSample_->setShader(ShaderProgram::SL_GLSL, vertexShaderCode, fragmentShaderCode, errorMessage))
			{
				Log::error() << "Failed to compile custom shader: " << errorMessage;
			}
		}

		if (!shaderProgramForOneSample_->isCompiled())
		{
			return false;
		}

		if (vertexSetForOneSample_.isNull())
		{
			vertexSetForOneSample_ = engine().factory().createVertexSet();

			const Vectors3 vertices =
			{
				Vector3(-1, 1, 0),
				Vector3(-1, -1, 0),
				Vector3(1, -1, 0),
				Vector3(1, 1, 0),
			};

			const Vectors2 textureCoordinates =
			{
				Vector2(0, 1),
				Vector2(0, 0),
				Vector2(1, 0),
				Vector2(1, 1)
			};

			vertexSetForOneSample_->setVertices(vertices);
			vertexSetForOneSample_->setTextureCoordinates(textureCoordinates, 0u);
		}

		if (trianglesForOneSample_.isNull())
		{
			trianglesForOneSample_ = engine().factory().createTriangles();

			const TriangleFaces triangleFaces =
			{
				TriangleFace(0u, 1u, 2u),
				TriangleFace(0u, 2u, 3u)
			};

			trianglesForOneSample_->setFaces(triangleFaces);
		}

		GLESShaderProgram& shaderProgramForOneSample = shaderProgramForOneSample_.force<GLESShaderProgram>();

		// we bind the multi sample texture of this multi sample framebuffer as input texture
		bindTexture(shaderProgramForOneSample, 0u);

		const GLint locationNumberSamples = glGetUniformLocation(shaderProgramForOneSample.id(), "numberSamples");
		if (locationNumberSamples != -1)
		{
			GLESObject::setUniform(locationNumberSamples, int(framebufferMultisamples_));
		}

		vertexSetForOneSample_.force<GLESVertexSet>().bindVertexSet(shaderProgramForOneSample.id());

		GLint oldBoundFramebufferId = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldBoundFramebufferId);
		ocean_assert(GL_NO_ERROR == glGetError());

		// we bind the one sample texture as target framebuffer
		textureFramebufferOneSample.bindFramebuffer();

		glViewport(GLint(subRegionLeft), GLint(subRegionTop), subRegionWidth, subRegionHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());

		trianglesForOneSample_.force<GLESTriangles>().drawTriangles();

		glBindFramebuffer(GL_FRAMEBUFFER, oldBoundFramebufferId);
		ocean_assert(GL_NO_ERROR == glGetError());

		// we finally copy the image content one sample texture to memory

		return textureFramebufferOneSample.copyColorTextureToFrame(frame, subRegion);
	}
#endif // OCEAN_RENDERING_GLES_USE_ES

	return false;
}

#ifdef OCEAN_RENDERING_GLES_USE_ES

bool GLESTextureFramebuffer::copyDepthTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion)
{
	unsigned int subRegionLeft = 0u;
	unsigned int subRegionWidth = width_;
	unsigned int subRegionTop = 0u;
	unsigned int subRegionHeight = height_;

	if (subRegion.isValid())
	{
		if (subRegion.rightEnd() > width_ || subRegion.bottomEnd() > height_)
		{
			ocean_assert(false && "Invalid sub-region!");
			return false;
		}

		subRegionLeft = subRegion.left();
		subRegionTop = subRegion.top();
		subRegionWidth = subRegion.width();
		subRegionHeight = subRegion.height();
	}

	// OpenGLES does not support glReadPixels with GL_DEPTH_COMPONENT
	// therefore, we need to render the depth texture into an intermediate color framebuffer

	if (textureFramebufferOneSample_.isNull())
	{
		textureFramebufferOneSample_ = engine().factory().createTextureFramebuffer();
	}

	GLESTextureFramebuffer& textureFramebufferOneSample = textureFramebufferOneSample_.force<GLESTextureFramebuffer>();

	textureFramebufferOneSample.setPixelFormat(FrameType::FORMAT_Y32); // OpenGL glReadPixels does not support F32 framebuffers, therefore using Y32 with normalization afterwards
	textureFramebufferOneSample.resize(width_, height_);

	if (shaderProgramForOneSample_.isNull())
	{
		shaderProgramForOneSample_ = engine().factory().createShaderProgram();

		const std::string vertexShaderCode =
		R"SHADER(#version 300 es

			// Vertex attribute
			in vec4 aVertex;

			// Texture coordinate attribute
			in vec4 aTextureCoordinate;

			// Resulting texture coordinate
			out mediump vec2 vTextureCoordinate;

			void main(void)
			{
				gl_Position = aVertex;
				vTextureCoordinate = aTextureCoordinate.xy;
			}
		)SHADER";

		const std::string fragmentShaderCode =
		R"SHADER(#version 300 es

			// Input texture coordinate
			in mediump vec2 vTextureCoordinate;

			// Texture sampler object
			uniform highp sampler2D primaryTexture;

			// The out fragment color
			out highp uint fragColor;

			void main()
			{
				highp float depth = texture(primaryTexture, vTextureCoordinate).r;

				fragColor = uint(depth * 4294967000.0); // 2^32 - eps for rounding
			}
		)SHADER";

		std::string errorMessage;
		if (!shaderProgramForOneSample_->setShader(ShaderProgram::SL_GLSL, vertexShaderCode, fragmentShaderCode, errorMessage))
		{
			Log::error() << "Failed to compile custom shader: " << errorMessage;
		}
	}

	if (!shaderProgramForOneSample_->isCompiled())
	{
		return false;
	}

	if (vertexSetForOneSample_.isNull())
	{
		vertexSetForOneSample_ = engine().factory().createVertexSet();

		const Vectors3 vertices =
		{
			Vector3(-1, 1, 0),
			Vector3(-1, -1, 0),
			Vector3(1, -1, 0),
			Vector3(1, 1, 0),
		};

		const Vectors2 textureCoordinates =
		{
			Vector2(0, 1),
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1)
		};

		vertexSetForOneSample_->setVertices(vertices);
		vertexSetForOneSample_->setTextureCoordinates(textureCoordinates, 0u);
	}

	if (trianglesForOneSample_.isNull())
	{
		trianglesForOneSample_ = engine().factory().createTriangles();

		const TriangleFaces triangleFaces =
		{
			TriangleFace(0u, 1u, 2u),
			TriangleFace(0u, 2u, 3u)
		};

		trianglesForOneSample_->setFaces(triangleFaces);
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	GLESShaderProgram& shaderProgramForOneSample = shaderProgramForOneSample_.force<GLESShaderProgram>();
	glUseProgram(shaderProgramForOneSample.id());
	ocean_assert(GL_NO_ERROR == glGetError());

	// we bind the depth texture of this framebuffer as input texture

	ocean_assert(depthTextureId_ != 0u);
	glActiveTexture(GLenum(GL_TEXTURE0 + 0));
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, depthTextureId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(!textureName_.empty());
	const GLint locationTexture = glGetUniformLocation(shaderProgramForOneSample.id(), textureName_.c_str());
	ocean_assert(locationTexture != -1);
	setUniform(locationTexture, int(0));

	vertexSetForOneSample_.force<GLESVertexSet>().bindVertexSet(shaderProgramForOneSample.id());

	GLint oldBoundFramebufferId = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldBoundFramebufferId);
	ocean_assert(GL_NO_ERROR == glGetError());

	// we bind the one sample texture as target framebuffer
	textureFramebufferOneSample.bindFramebuffer();

	glViewport(GLint(subRegionLeft), GLint(subRegionTop), subRegionWidth, subRegionHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	trianglesForOneSample_.force<GLESTriangles>().drawTriangles();

	glBindFramebuffer(GL_FRAMEBUFFER, oldBoundFramebufferId);
	ocean_assert(GL_NO_ERROR == glGetError());

	// we finally copy the image content one sample texture to memory

	Frame yFrame;
	if (!textureFramebufferOneSample.copyColorTextureToFrame(yFrame, subRegion))
	{
		return false;
	}

	if (!frame.set(FrameType(yFrame, FrameType::FORMAT_F32), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	constexpr float normalization = float(1.0 / 4294967000.0);

	for (unsigned int y = 0u; y < yFrame.height(); ++y)
	{
		const unsigned int* const sourceRow = yFrame.constrow<unsigned int>(y);
		float* const targetRow = frame.row<float>(y);

		for (unsigned int x = 0u; x < yFrame.width(); ++x)
		{
			targetRow[x] = float(sourceRow[x]) * normalization;
		}
	}

	return true;
}

#else

bool GLESTextureFramebuffer::copyDepthTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion)
{
	unsigned int subRegionLeft = 0u;
	unsigned int subRegionTop = 0u;
	unsigned int subRegionWidth = width_;
	unsigned int subRegionHeight = height_;

	if (subRegion.isValid())
	{
		if (subRegion.rightEnd() > width_ || subRegion.bottomEnd() > height_)
		{
			ocean_assert(false && "Invalid sub-region!");
			return false;
		}

		subRegionLeft = subRegion.left();
		subRegionTop = subRegion.top();
		subRegionWidth = subRegion.width();
		subRegionHeight = subRegion.height();
	}

	const FrameType frameType(subRegionWidth, subRegionHeight, FrameType::FORMAT_F32, FrameType::ORIGIN_LOWER_LEFT);

	if (!frameType.isValid())
	{
		return false;
	}

	if (!frame.set(frameType, false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	if (framebufferMultisamples_ == 1u)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectId_);
		ocean_assert(GL_NO_ERROR == glGetError());

		const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		ocean_assert_and_suppress_unused(status == GL_FRAMEBUFFER_COMPLETE, status);

		glReadPixels(GLint(subRegionLeft), GLint(subRegionTop), subRegionWidth, subRegionHeight, GL_DEPTH_COMPONENT, GL_FLOAT, frame.data<void>());

		const GLenum result = glGetError();
		ocean_assert(result == GL_NO_ERROR);

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);
		ocean_assert(GL_NO_ERROR == glGetError());

		return result == GL_NO_ERROR;
	}

	ocean_assert(framebufferMultisamples_ > 1u);

	Rendering::TextureFramebufferRef _textureFramebufferOneSample = engine().factory().createTextureFramebuffer();

	GLESTextureFramebuffer& textureFramebufferOneSample = _textureFramebufferOneSample.force<GLESTextureFramebuffer>();

	textureFramebufferOneSample.setPixelFormat(FrameType::FORMAT_F32);
	textureFramebufferOneSample.resize(width_, height_);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferObjectId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, textureFramebufferOneSample.framebufferObjectId_);
	ocean_assert(GL_NO_ERROR == glGetError());

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

	if (glBlitFramebuffer_ == nullptr)
	{
		glBlitFramebuffer_ = PFNGLBLITFRAMEBUFFERPROC(wglGetProcAddress("glBlitFramebuffer"));
		ocean_assert(glBlitFramebuffer_ != nullptr);

		if (glBlitFramebuffer_ == nullptr)
		{
			return false;
		}
	}

	glBlitFramebuffer_(0, 0, width_, height_, 0, 0, width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

#else

	glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

#endif // OCEAN_PLATFORM_BUILD_WINDOWS

	glBindFramebuffer(GL_FRAMEBUFFER, textureFramebufferOneSample.framebufferObjectId_);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ocean_assert_and_suppress_unused(status == GL_FRAMEBUFFER_COMPLETE, status);

	glReadPixels(GLint(subRegionLeft), GLint(subRegionTop), subRegionWidth, subRegionHeight, GL_DEPTH_COMPONENT, GL_FLOAT, frame.data<void>());

	const GLenum result = glGetError();
	ocean_assert(result == GL_NO_ERROR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0u);
	ocean_assert(GL_NO_ERROR == glGetError());

	return result == GL_NO_ERROR;
}

#endif //  OCEAN_RENDERING_GLES_USE_ES

bool GLESTextureFramebuffer::isValid() const
{
	return framebufferObjectId_ != 0u;
}

void GLESTextureFramebuffer::release()
{
#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	if (iosTexture_ != nullptr)
	{
		CFRelease(iosTexture_);
		iosTexture_ = nullptr;
		colorTextureId_ = 0u;
	}

	if (iosPixelBuffer_ != nullptr)
	{
		CFRelease(iosPixelBuffer_);
		iosPixelBuffer_ = nullptr;
	}

	if (iosTextureCache_ != nullptr)
	{
		CFRelease(iosTextureCache_);
		iosTextureCache_ = nullptr;
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	if (colorTextureId_ != 0u)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);

		glDeleteTextures(1, &colorTextureId_);
		colorTextureId_ = 0u;

		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	if (depthTextureId_ != 0u)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);

		glDeleteTextures(1, &depthTextureId_);
		depthTextureId_ = 0u;

		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	if (framebufferObjectId_ != 0u)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);

		glDeleteFramebuffers(1, &framebufferObjectId_);
		framebufferObjectId_ = 0u;

		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	width_ = 0u;
	height_ = 0u;
	framebufferMultisamples_ = 1u;
	pixelFormat_ = FrameType::FORMAT_RGBA32;
}

void GLESTextureFramebuffer::createMipmap()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	if (colorTextureId_ != 0u)
	{
#ifdef OCEAN_RENDERING_GLES_USE_ES
		const GLenum textureTarget = GL_TEXTURE_2D;
#else
		const GLenum textureTarget = framebufferMultisamples_ > 1u ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
#endif

		glBindTexture(textureTarget, colorTextureId_);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glGenerateMipmap(textureTarget);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}
}

}

}

}
