// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
