/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/renderer/quest/OpenXRRendererApplication.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Utilities.h"

OpenXRRendererApplication::OpenXRRendererApplication(struct android_app* androidApp) :
	VRNativeApplication(androidApp)
{
	// nothing to do here
}

XrSpace OpenXRRendererApplication::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void OpenXRRendererApplication::onFramebufferInitialized()
{
	VRNativeApplication::onFramebufferInitialized();

	ocean_assert(engine_ && framebuffer_);

	// we create a new scenegraph scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();
	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	// we create a 3D coordinate system based on cylinders and cones
	const Rendering::TransformRef transform = Rendering::Utilities::createCoordinateSystem(engine_, Scalar(0.3), Scalar(0.05), Scalar(0.02)); // 30cm, 5cm, 2cm

	// we place the coordinate system 1 meter in front of the world's origin
	transform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1.0)));
	// we add the new scene graph object to the scene
	scene->addChild(transform);

#ifdef OCEAN_USE_CUSTOM_RENDER_CALLBACK

	// enable the definition of the pre-render callback to apply custom OpenGL ES render calls before Ocean's scene graph rendering engine is applied
	framebuffer_->setPreRenderCallback(Rendering::Framebuffer::RenderCallback::create(*this, &OpenXRRendererApplication::onPreRenderFramebuffer));

	// enable the definition of the post-render callback to apply custom OpenGL ES render calls after Ocean's scene graph rendering engine is applied
	framebuffer_->setPostRenderCallback(Rendering::Framebuffer::RenderCallback::create(*this, &OpenXRRendererApplication::onPostRenderFramebuffer));

#endif // OCEAN_USE_CUSTOM_RENDER_CALLBACK

}

#ifdef OCEAN_USE_CUSTOM_RENDER_CALLBACK

void OpenXRRendererApplication::onPreRenderFramebuffer(const size_t eyeIndex, const HomogenousMatrix4 /*view_T_world*/, const SquareMatrix4 /*projection*/, const Timestamp /*predictedDisplayTime*/)
{
	// we apply a custom background color instead of black
	// the SceneGraph object (the coordinate system) will still be visible

	if (eyeIndex == 0)
	{
		glClearColor(0.3f, 0.0f, 0.0f, 0.0f);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glClearColor(0.0f, 0.3f, 0.0f, 0.0f);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void OpenXRRendererApplication::onPostRenderFramebuffer(const size_t eyeIndex, const HomogenousMatrix4 /*view_T_world*/, const SquareMatrix4 /*projection*/, const Timestamp /*predictedDisplayTime*/)
{
	// we clear the framebuffer with custom colors
	// the SceneGraph object (the coordinate system) will not be vsibile anymore

	if (eyeIndex == 0)
	{
		glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());
}

#endif // OCEAN_USE_CUSTOM_RENDER_CALLBACK
