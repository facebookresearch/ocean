/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/SDLScene.h"

namespace Ocean
{

namespace SceneDescription
{

SDLScene::SDLScene(const std::string& filename) :
	Scene(filename),
	SDLNode()
{
	// nothing to do here
}

Rendering::SceneRef SDLScene::apply(const Rendering::EngineRef& engine)
{
	if (engine.isNull())
	{
		return Rendering::ObjectRef();
	}

	Timestamp startTimestamp(true);

	Rendering::SceneRef scene(internalApply(engine));

	Timestamp stopTimestamp(true);

	if (scene.isNull())
	{
		Log::error() << "Failure during creation of a rendering scene description of \"" << filename() << "\".";
	}
	else
	{
		Log::info() << "Successfully creation of a rendering scene description of \"" << filename() << "\" in " << double(stopTimestamp - startTimestamp) << " seconds.";
	}

	return scene;
}

Rendering::ObjectRef SDLScene::apply(const Rendering::EngineRef& /*engine*/, const SDLScene& /*scene*/, SDLNode& /*parentDescription*/, const Rendering::ObjectRef& /*parentRendering*/)
{
	ocean_assert(false && "This function should never be used.");
	throw OceanException("This function should never be used.");
}

}

}
