/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/apple/Plugin.h"
#include "ocean/rendering/glescenegraph/apple/GLESEngineApple.h"

bool pluginLoad()
{
	Ocean::Rendering::GLESceneGraph::Apple::GLESEngineApple::registerEngine();
	return true;
}

bool pluginUnload()
{
	return Ocean::Rendering::GLESceneGraph::Apple::GLESEngineApple::unregisterEngine();
}
