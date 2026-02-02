/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RendererView.h"

#include "application/ocean/demo/rendering/renderer/RendererUtilities.h"

#include "ocean/rendering/Scene.h"

namespace Ocean
{

const bool RendererView::instanceRegistered_ = RendererView::registerInstanceFunction(RendererView::createInstance);

RendererView::~RendererView()
{
	release();
}

bool RendererView::initialize()
{
	if (!GLRendererView::initialize())
	{
		return false;
	}

	scene_ = RendererUtilities::createScene(*engine_);
	framebuffer_->addScene(scene_);

	framebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));
	framebuffer_->view()->fitCamera();

	return true;
}

bool RendererView::release()
{
	scene_.release();

	return GLRendererView::release();
}

}
