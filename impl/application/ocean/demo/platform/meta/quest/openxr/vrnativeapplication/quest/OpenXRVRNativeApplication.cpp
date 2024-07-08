/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

	Log::info() << "OpenXRVRNativeApplication::onFramebufferInitialized(), create a rendering scene...";
}
