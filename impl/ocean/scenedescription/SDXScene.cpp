/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/SDXScene.h"

namespace Ocean
{

namespace SceneDescription
{

SDXScene::SDXScene(const std::string& filename, const Library& library, const Rendering::EngineRef& engine) :
	Scene(filename),
	SDXNode(nullptr)
{
	ocean_assert(&library);
	ocean_assert(engine);

	const ScopedLock scopedLock(lock_);

	// scene id will be the same as the node id
	sceneEnvironment_ = SDXEnvironment(engine, &library, filename, nodeId_);
	environment_ = &sceneEnvironment_;
}

void SDXScene::registerGlobalLight(const Rendering::LightSourceRef& lightSource)
{
	const Rendering::GroupRef group(renderingObject());
	ocean_assert(group);

	if (group)
		group->registerLight(lightSource);
}

void SDXScene::unregisterGlobalLight(const Rendering::LightSourceRef& lightSource)
{
	const Rendering::GroupRef group(renderingObject());
	ocean_assert(group);

	if (group)
	{
		group->unregisterLight(lightSource);
	}
}

void SDXScene::initialize(const Timestamp timestamp)
{
	ocean_assert(!initialized_);

	onInitialize(timestamp);
	initialized_ = true;
}

void SDXScene::initialize(const Rendering::SceneRef& /*scene*/, const Timestamp /*timestamp*/, const bool /*reinitialize*/)
{
	ocean_assert(false && "This function should not be used!");
}

void SDXScene::onInitialize(const Timestamp /*timestamp*/)
{
	// nothing to do here
}

void SDXScene::onInitialize(const Rendering::SceneRef& /*scene*/, const Timestamp /*timestamp*/)
{
	ocean_assert(false && "This function should not be used!");
}

}

}
