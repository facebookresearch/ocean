// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/platform/meta/quest/openxr/vrnativeapplication/quest/OpenXRVRNativeApplication.h"

OpenXRVRNativeApplication::OpenXRVRNativeApplication(struct android_app* androidApp) :
	VRNativeApplication(androidApp)
{
	// nothing to do here
}

void OpenXRVRNativeApplication::onFramebufferInitialized()
{
	VRNativeApplication::onFramebufferInitialized();

	ocean_assert(engine_ && framebuffer_);

	// create a rendering scene, attach it to the framebuffer, add further objects ...

	// const Rendering::SceneRef scene = engine_->factory().createScene();
	// framebuffer_->addScene(scene);
}
