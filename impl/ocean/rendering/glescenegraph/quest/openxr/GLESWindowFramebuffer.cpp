/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/quest/openxr/GLESWindowFramebuffer.h"
#include "ocean/rendering/glescenegraph/quest/GLESEngineQuest.h"

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESStereoView.h"

#include "ocean/rendering/Engine.h"

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR

#include "ocean/platform/openxr/Session.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Quest
{

namespace OpenXR
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

	const HomogenousMatrix4 views_T_world[numberEyes_] =
	{
		glesStereoView->leftTransformation().inverted(),
		glesStereoView->rightTransformation().inverted()
	};

	const SquareMatrix4 projectionMatrices[numberEyes_] =
	{
		glesStereoView->leftProjectionMatrix(),
		glesStereoView->rightProjectionMatrix()
	};

	ocean_assert(glesFramebuffers_.size() == numberEyes_);
	if (glesFramebuffers_.size() != numberEyes_)
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

		Platform::Meta::Quest::OpenXR::GLESFramebuffer& framebuffer = glesFramebuffers_[eye];

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

		framebuffer.unbind();
	}
}

bool GLESWindowFramebuffer::initializeContext()
{
	ocean_assert(id_ != 0);
	if (id_ == 0)
	{
		return false;
	}

	Platform::OpenXR::Session* xrSession = reinterpret_cast<Platform::OpenXR::Session*>(id_);
	ocean_assert(xrSession != nullptr);

	if (!xrSession->isValid())
	{
		ocean_assert(false && "Invalid OpenXR session object");
		return false;
	}

	ocean_assert(glesFramebuffers_.empty());

	glesFramebuffers_ = Platform::Meta::Quest::OpenXR::GLESFramebuffers(numberEyes_);

	const unsigned int framebufferWidth = xrSession->width();
	const unsigned int framebufferHeight = xrSession->height();

	for (Platform::Meta::Quest::OpenXR::GLESFramebuffer& framebuffer : glesFramebuffers_)
	{
		if (!framebuffer.initialize(*xrSession, GL_SRGB8_ALPHA8, framebufferWidth, framebufferHeight, 4u /*multisamples*/, config_.useStencilBuffer))
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

	id_ = 0;
}

}

}

}

}

}

#endif // OCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR
