/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "RendererViewController.h"

#include "application/ocean/demo/rendering/renderer/RendererUtilities.h"

#include "ocean/base/RandomI.h"

#include "ocean/rendering/Scene.h"

using namespace Ocean;

@interface RendererViewController ()
{
	Rendering::SceneRef scene_;
}

@end

@implementation RendererViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	RandomI::initialize();

	if (renderingEngine_.isNull() || renderingFramebuffer_.isNull())
	{
		return;
	}

	scene_ = RendererUtilities::createScene(*renderingEngine_);
	renderingFramebuffer_->addScene(scene_);

	renderingFramebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));
	renderingFramebuffer_->view()->fitCamera();
}

@end
