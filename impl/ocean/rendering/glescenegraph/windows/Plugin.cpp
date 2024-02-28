// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/glescenegraph/windows/Plugin.h"
#include "ocean/rendering/glescenegraph/windows/GLESEngineWindows.h"

bool pluginLoad()
{
	Ocean::Rendering::GLESceneGraph::Windows::GLESEngineWindows::registerEngine();
	return true;
}

bool pluginUnload()
{
	return Ocean::Rendering::GLESceneGraph::Windows::GLESEngineWindows::unregisterEngine();
}
