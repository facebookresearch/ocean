// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/quest/vrapi/GLESWindowFramebuffer.h"

#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESStereoView.h"

#include "ocean/rendering/Engine.h"

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_VRAPI

#include <VrApi_Types.h>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Quest
{

namespace VrApi
{

GLESWindowFramebuffer::GLESWindowFramebuffer(const Framebuffer::FramebufferConfig& config) :
	GLESFramebuffer(),
	WindowFramebuffer(),
	config_(config)
{
	// nothing to do here
}

GLESWindowFramebuffer::~GLESWindowFramebuffer()
{
	release();
}

bool GLESWindowFramebuffer::initializeById(const size_t id, const FramebufferRef& shareFramebuffer)
{
	ocean_assert(id != 0);
	ocean_assert(shareFramebuffer.isNull());

	release();

	id_ = id;

	if (!initializeContext() || !initializeOpenGLES())
	{
		release();
		return false;
	}

	return true;
}

void GLESWindowFramebuffer::render()
{
	const SmartObjectRef<GLESStereoView> glesStereoView(framebufferView);

	if (glesStereoView.isNull())
	{
		return;
	}

	static_assert(VRAPI_FRAME_LAYER_EYE_MAX == 2, "Invalid number of eyes!");

	const HomogenousMatrix4 views_T_world[2] =
	{
		glesStereoView->leftTransformation().inverted(),
		glesStereoView->rightTransformation().inverted()
	};

	const SquareMatrix4 projectionMatrices[2] =
	{
		glesStereoView->leftProjectionMatrix(),
		glesStereoView->rightProjectionMatrix()
	};

	ocean_assert(glesFramebuffers_.size() == VRAPI_FRAME_LAYER_EYE_MAX);
	if (glesFramebuffers_.size() != 2)
	{
		return;
	}

	TemporaryScopedLock temporaryScopedLock(objectLock);
		const RenderCallback preRenderCallback(preRenderCallback_);
		const RenderCallback postRenderCallback(postRenderCallback_);
	temporaryScopedLock.release();

	const Timestamp renderTimestamp = engine().timestamp();
	ocean_assert(renderTimestamp.isValid());

	for (size_t eye = 0; eye < glesFramebuffers_.size(); ++eye)
	{
		setStereoType(eye == 1 ? ST_RIGHT : ST_LEFT);

		const HomogenousMatrix4& camera_T_world = views_T_world[eye];
		const SquareMatrix4& projectionMatrix = projectionMatrices[eye];

		Platform::Meta::Quest::VrApi::GLESFramebuffer& framebuffer = glesFramebuffers_[eye];

		ocean_assert(GL_NO_ERROR == glGetError());

		framebuffer.bind();

		const RGBAColor backgroundColor = glesStereoView->backgroundColor();

		glClearColor(backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(), backgroundColor.alpha());
		ocean_assert(GL_NO_ERROR == glGetError());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());

		if (preRenderCallback)
		{
			preRenderCallback(eye, camera_T_world, projectionMatrix, renderTimestamp);
		}

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

		traverser_.clear();

		for (Scenes::const_iterator i = framebufferScenes.begin(); i != framebufferScenes.end(); ++i)
		{
			const SmartObjectRef<GLESScene> glesScene(*i);
			ocean_assert(glesScene);

			Lights lights;

			if (glesScene->useHeadlight() && glesStereoView->useHeadlight() && glesStereoView->headlight())
			{
				lights.push_back(LightPair(glesStereoView->headlight(), HomogenousMatrix4(true)));
			}

			glesScene->addToTraverser(*this, projectionMatrix, camera_T_world, lights, traverser_);
			ocean_assert(GL_NO_ERROR == glGetError());
		}

		traverser_.render(*this, projectionMatrix, camera_T_world);

		if (postRenderCallback)
		{
			postRenderCallback(eye, camera_T_world, projectionMatrix, renderTimestamp);
		}

		// we invalidate the depth buffer so that following componenets don't need to copy the data

		framebuffer.invalidateDepthBuffer();

		framebuffer.swap();
	}

	Platform::Meta::Quest::VrApi::GLESFramebuffer::unbind();
}

bool GLESWindowFramebuffer::initializeContext()
{
	ocean_assert(id_ != 0);
	if (id_ == 0)
	{
		return false;
	}

	ovrJava* ovrJavaPointer = reinterpret_cast<ovrJava*>(id_);
	ocean_assert(ovrJavaPointer != nullptr);

	const Platform::GLES::EGLContext::ConfigAttributePairs configAttributePairs =
	{
		{EGL_RED_SIZE, 8},
		{EGL_GREEN_SIZE, 8},
		{EGL_BLUE_SIZE, 8},
		{EGL_ALPHA_SIZE, 8},
		{EGL_DEPTH_SIZE, 0},
		{EGL_STENCIL_SIZE, config_.useStencilBuffer ? 8 : 0},
		{EGL_SAMPLES, 0}
	};

	if (!eglContext_.initialize(configAttributePairs))
	{
		Log::info() << "Failed to initialize an EGL context!";
		return false;
	}

	ocean_assert(glesFramebuffers_.empty());

	glesFramebuffers_ = Platform::Meta::Quest::VrApi::GLESFramebuffers(VRAPI_FRAME_LAYER_EYE_MAX);

	const unsigned int framebufferWidth = vrapi_GetSystemPropertyInt(ovrJavaPointer, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);
	const unsigned int framebufferHeight = vrapi_GetSystemPropertyInt(ovrJavaPointer, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);

	for (Platform::Meta::Quest::VrApi::GLESFramebuffer& framebuffer : glesFramebuffers_)
	{
		if (!framebuffer.initialize(GL_SRGB8_ALPHA8, framebufferWidth, framebufferHeight, 4u /*multisamples*/, config_.useStencilBuffer))
		{
			return false;
		}
	}

	setViewport(0u, 0u, framebufferWidth, framebufferHeight);

	return true;
}

void GLESWindowFramebuffer::release()
{
	glesFramebuffers_.clear();

	GLESFramebuffer::release();

	eglContext_.release();

	id_ = 0;
}

}

}

}

}

}

#endif // OCEAN_RENDERING_GLES_QUEST_PLATFORM_VRAPI
