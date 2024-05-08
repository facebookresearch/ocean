/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESProgramManager.h"
#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESText.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESFramebuffer::GLESFramebuffer() :
	Framebuffer()
{
	// nothing to do here
}

GLESFramebuffer::~GLESFramebuffer()
{
	// nothing to do here
}

void GLESFramebuffer::viewport(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height) const
{
	left = viewportLeft_;
	top = viewportTop_;
	width = viewportWidth_;
	height = viewportHeight_;
}

bool GLESFramebuffer::isAntialiasingSupported(const unsigned int buffers) const
{
	GLint value = 0;

	ocean_assert(GL_NO_ERROR == glGetError());
	glGetIntegerv(GL_SAMPLES, &value);
	ocean_assert(GL_NO_ERROR == glGetError());

	return (unsigned int)value == buffers;
}

bool GLESFramebuffer::isAntialiasing() const
{
	GLint value = 0;

	ocean_assert(GL_NO_ERROR == glGetError());
	glGetIntegerv(GL_SAMPLES, &value);
	ocean_assert(GL_NO_ERROR == glGetError());

	return value > 0;
}

GLESFramebuffer::FaceMode GLESFramebuffer::faceMode() const
{
#ifdef OCEAN_RENDERING_GLES_USE_ES
	throw NotSupportedException("Face mode is not supported OpenGL ES 2.0.");
#else
	return faceMode_;
#endif
}

GLESFramebuffer::CullingMode GLESFramebuffer::cullingMode() const
{
	return cullingMode_;
}

GLESFramebuffer::LightingMode GLESFramebuffer::lightingMode() const
{
	return lightingMode_;
}

GLESFramebuffer::RenderTechnique GLESFramebuffer::renderTechnique() const
{
	throw NotSupportedException("At this moment  render mode is not supported in GLESceneGraph.");
}

GLESFramebuffer::ShadowTechnique GLESFramebuffer::shadowTechnique() const
{
	throw NotSupportedException("At this moment shadow technique is not supported in GLESceneGraph.");
}

GLESFramebuffer::StereoType GLESFramebuffer::stereoType() const
{
	return stereoType_;
}

void GLESFramebuffer::setViewport(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height)
{
	glViewport(GLint(left), GLint(top), GLint(width), GLint(height));

	viewportLeft_ = left;
	viewportTop_ = top;
	viewportWidth_ = width;
	viewportHeight_ = height;
}

bool GLESFramebuffer::setAntialiasing(const bool antialiasing)
{
	GLint value = 0;

	ocean_assert(GL_NO_ERROR == glGetError());
	glGetIntegerv(GL_SAMPLES, &value);
	ocean_assert(GL_NO_ERROR == glGetError());

	return (value > 0 && antialiasing) || (value == 0 && !antialiasing);
}

void GLESFramebuffer::setFaceMode(const FaceMode mode)
{
#ifdef OCEAN_RENDERING_GLES_USE_ES
	throw NotSupportedException("Face mode is not supported OpenGL ES 2.0.");
#else
	faceMode_ = mode;
#endif
}

void GLESFramebuffer::setCullingMode(const CullingMode cullingMode)
{
	cullingMode_ = cullingMode;
}

void GLESFramebuffer::setLightingMode(const LightingMode lightingMode)
{
	lightingMode_ = lightingMode;
}

void GLESFramebuffer::setRenderTechnique(const RenderTechnique /*technique*/)
{
	throw NotSupportedException("At this moment render mode is not supported in GLESceneGraph.");
}

void GLESFramebuffer::setShadowTechnique(const ShadowTechnique /*technique*/)
{
	throw NotSupportedException("At this moment shadow technique is not supported in GLESceneGraph.");
}

void GLESFramebuffer::setStereoType(const StereoType stereoType)
{
	const ScopedLock scopedLock(objectLock);

	stereoType_ = stereoType;
}

void GLESFramebuffer::render()
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(GL_NO_ERROR == glGetError());

	if (viewportWidth_ == 0u || viewportHeight_ == 0u)
	{
		return;
	}

	glViewport(GLint(viewportLeft_), GLint(viewportTop_), GLint(viewportWidth_), GLint(viewportHeight_));
	ocean_assert(GL_NO_ERROR == glGetError());

	const SmartObjectRef<GLESView> glesView(framebufferView);

	if (glesView)
	{
		const RGBAColor backgroundColor = glesView->backgroundColor();

		glClearColor(backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(), backgroundColor.alpha());
		ocean_assert(GL_NO_ERROR == glGetError());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());

#ifndef OCEAN_RENDERING_GLES_USE_ES
		switch (faceMode_)
		{
			case PrimitiveAttribute::MODE_DEFAULT:
			case PrimitiveAttribute::MODE_FACE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;

			case PrimitiveAttribute::MODE_LINE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;

			case PrimitiveAttribute::MODE_POINT:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
		}
#endif

		if (cullingMode_ == PrimitiveAttribute::CULLING_DEFAULT)
		{
			glEnable(GL_CULL_FACE);
			ocean_assert(GL_NO_ERROR == glGetError());

			glCullFace(GL_BACK);
			ocean_assert(GL_NO_ERROR == glGetError());
		}
		else
		{
			switch (cullingMode_)
			{
				case PrimitiveAttribute::CULLING_NONE:
					glDisable(GL_CULL_FACE);
					ocean_assert(GL_NO_ERROR == glGetError());
					break;

				case PrimitiveAttribute::CULLING_BACK:
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					ocean_assert(GL_NO_ERROR == glGetError());
					break;

				case PrimitiveAttribute::CULLING_FRONT:
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
					ocean_assert(GL_NO_ERROR == glGetError());
					break;

				case PrimitiveAttribute::CULLING_BOTH:
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT_AND_BACK);
					ocean_assert(GL_NO_ERROR == glGetError());
					break;

				default:
					ocean_assert(false && "Invalid parameter!");
			}
		}

		const HomogenousMatrix4 view_T_world(glesView->transformation().inverted());

		Lights lights;

		if (glesView->useHeadlight() && glesView->headlight())
		{
			lights.emplace_back(glesView->headlight(), HomogenousMatrix4(true));
		}

		traverser_.clear();

		const SmartObjectRef<GLESUndistortedBackground> glesUndistortedBackground(glesView->background());
		if (glesUndistortedBackground)
		{
			glesUndistortedBackground->addToTraverser(*this, glesView->projectionMatrix(), view_T_world, lights, traverser_);
		}

		for (Scenes::const_iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
		{
			const SmartObjectRef<GLESScene> glesScene(*i);
			ocean_assert(glesScene);

			lights.clear();

			if (glesScene->useHeadlight() && glesView->useHeadlight() && glesView->headlight())
			{
				lights.emplace_back(glesView->headlight(), HomogenousMatrix4(true));
			}

			glesScene->addToTraverser(*this, glesView->projectionMatrix(), view_T_world, lights, traverser_);
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		traverser_.render(*this, glesView->projectionMatrix(), view_T_world);
	}
}

bool GLESFramebuffer::intersection(const Line3& ray, RenderableRef& renderable, Vector3& position)
{
	const ScopedLock scopedLock(objectLock);

	const SmartObjectRef<GLESView> glesView(framebufferView);

	if (glesView.isNull())
	{
		return false;
	}

	const HomogenousMatrix4 world_T_ray(ray.point(), Quaternion(Vector3(0, 0, -1), ray.direction()));
	const HomogenousMatrix4 ray_T_world(world_T_ray.inverted());

	traverser_.clear();

	Lights lights;

	const SmartObjectRef<GLESUndistortedBackground> glesUndistortedBackground(glesView->background());
	if (glesUndistortedBackground)
	{
		glesUndistortedBackground->addToTraverser(*this, glesView->projectionMatrix(), ray_T_world, lights, traverser_);
	}

	for (Scenes::const_iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
	{
		const SmartObjectRef<GLESScene> glesScene(*i);
		ocean_assert(glesScene);

		glesScene->addToTraverser(*this, glesView->projectionMatrix(), ray_T_world, lights, traverser_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	constexpr unsigned int width = 3u;
	constexpr unsigned int height = 3u;

	if (pickingTextureFramebuffer_.isNull())
	{
		pickingTextureFramebuffer_ = engine().factory().createTextureFramebuffer();

		if (pickingTextureFramebuffer_.isNull())
		{
			return false;
		}

		pickingTextureFramebuffer_->setPixelFormat(FrameType::FORMAT_Y32);
		pickingTextureFramebuffer_->resize(width, height);
	}

	constexpr Scalar fovX = Numeric::deg2rad(1);

	const Scalar nearDistance = glesView->nearDistance();
	const Scalar farDistance = glesView->farDistance();

	const SquareMatrix4 projection = SquareMatrix4::projectionMatrix(fovX, 1, nearDistance, farDistance);

	pickingTextureFramebuffer_->bindFramebuffer();
	ocean_assert(GL_NO_ERROR == glGetError());

	glViewport(0, 0, width, height);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	traverser_.renderColorIds(engine(), projection, ray_T_world);

	pickingTextureFramebuffer_->unbindFramebuffer();

	Frame colorFrame;
	if (!pickingTextureFramebuffer_->copyColorTextureToFrame(colorFrame))
	{
		return false;
	}

	Frame depthFrame;
	if (!pickingTextureFramebuffer_->copyDepthTextureToFrame(depthFrame))
	{
		return false;
	}

	ocean_assert(colorFrame.isPixelFormatCompatible(FrameType::FORMAT_Y32));
	ocean_assert(depthFrame.isPixelFormatCompatible(FrameType::FORMAT_F32));

	const uint32_t colorId = colorFrame.constpixel<uint32_t>(colorFrame.width() / 2u, colorFrame.height() / 2u)[0];

	renderable = traverser_.renderableFromColorId(colorId);

	if (renderable.isNull())
	{
		return false;
	}

	const float depth = depthFrame.constpixel<float>(depthFrame.width() / 2u, depthFrame.height() / 2u)[0];
	const Scalar distance = (2 * nearDistance * farDistance) / (farDistance + nearDistance - Scalar(depth * 2.0f - 1.0f) * (farDistance - nearDistance));

	position = ray.point(distance);

	return true;
}

bool GLESFramebuffer::hasExtension(const std::string& extension) const
{
	return extensions_.find(extension) != std::string::npos;
}

bool GLESFramebuffer::linearizeDepthFrame(Frame& depthFrame, const Scalar nearDistance, const Scalar farDistance) const
{
	ocean_assert(depthFrame.isValid() && depthFrame.isPixelFormatCompatible(FrameType::FORMAT_F32));

	ocean_assert(nearDistance >= Scalar(0));
	ocean_assert(nearDistance < farDistance);

	if (!depthFrame.isValid() || !depthFrame.isPixelFormatCompatible(FrameType::FORMAT_F32))
	{
		return false;
	}

	const double zBufferRange = double(farDistance) - double(nearDistance);

	for (unsigned int y = 0u; y < depthFrame.height(); ++y)
	{
		float* const row = depthFrame.row<float>(y);

		for (unsigned int x = 0u; x < depthFrame.width(); ++x)
		{
			const float zBuffer = row[x];

			ocean_assert(zBuffer >= -1.0f && zBuffer <= 1.0f);
			const double zBufferNormalized = 2.0 * double(zBuffer) - 1.0;

			const double linearDepth = 2.0 * double(nearDistance) * double(farDistance) / (double(farDistance) + double(nearDistance) - double(zBufferNormalized) * zBufferRange);

			row[x] = float(linearDepth);
		}
	}

	return true;
}

bool GLESFramebuffer::initialize(const FramebufferRef& shareFramebuffer)
{
	ocean_assert_and_suppress_unused(shareFramebuffer.isNull(), shareFramebuffer);

	if (!initializeContext() || !initializeOpenGLES())
	{
		release();
		return false;
	}

	return true;
}

bool GLESFramebuffer::initializeContext()
{
	ocean_assert(false && "Should be implemented in derived classes.");
	return false;
}

bool GLESFramebuffer::initializeOpenGLES()
{
	const char* vendor = (const char*)(glGetString(GL_VENDOR));
	ocean_assert(GL_NO_ERROR == glGetError());
	Log::info() << "GLES vendor: " << vendor;

	const char* renderer = (const char*)(glGetString(GL_RENDERER));
	ocean_assert(GL_NO_ERROR == glGetError());
	Log::info() << "GLES renderer: " << renderer;

	const char* version = (const char*)(glGetString(GL_VERSION));
	ocean_assert(GL_NO_ERROR == glGetError());
	Log::info() << "GLES version: " << version;

	const char* shadingLanguageVersion = (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION));
	ocean_assert(GL_NO_ERROR == glGetError());
	Log::info() << "GLES SL version: " << shadingLanguageVersion;

#ifdef OCEAN_RENDERING_GLES_USE_ES

	const char* extensions = (const char*)(glGetString(GL_EXTENSIONS));
	ocean_assert(GL_NO_ERROR == glGetError());

	if (extensions != nullptr)
	{
		extensions_ = std::string(extensions);
	}

#elif defined(_WINDOWS)

	GLint numberExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numberExtensions);
	ocean_assert(GL_NO_ERROR == glGetError());

	for (GLint n = 0; n < numberExtensions; ++n)
	{
		const char* extension = (const char*)(glGetStringi(GL_EXTENSIONS, n));
		ocean_assert(GL_NO_ERROR == glGetError());

		if (extension != nullptr)
		{
			if (extensions_.empty())
			{
				extensions_ = std::string(extension);
			}
			else
			{
				extensions_ += std::string(" ") + std::string(extension);
			}
		}
	}

#endif // OCEAN_RENDERING_GLES_USE_ES

#ifdef OCEAN_DEBUG
	Log::info() << "GLES extensions: " << extensions_;
#endif

	int textureUnits = 0;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnits);
	ocean_assert(GL_NO_ERROR == glGetError());
	Log::info() << "GLES supported texture units: " << textureUnits;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

#ifdef __APPLE__
	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		glClearDepthf(1.0f);
	#else
		glClearDepth(1);
	#endif
#else
	glClearDepthf(1.0f);
	ocean_assert(GL_NO_ERROR == glGetError());
#endif

	glEnable(GL_DEPTH_TEST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDepthFunc(GL_LESS);
	ocean_assert(GL_NO_ERROR == glGetError());

	glEnable(GL_CULL_FACE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glCullFace(GL_BACK);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

void GLESFramebuffer::release()
{
	pickingTextureFramebuffer_.release();

	traverser_.clear();

	GLESText::release();
	GLESProgramManager::get().release();

	Framebuffer::release();
}

}

}

}
