/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Scene.h"

namespace Ocean
{

namespace Rendering
{

Scene::Scene() :
	Transform(),
	useViewHeadlight_(true)
{
	// nothing to do here
}

Scene::~Scene()
{
	// nothing to do here
}

Scene::ObjectType Scene::type() const
{
	return TYPE_SCENE;
}

bool Scene::useHeadlight() const
{
	return useViewHeadlight_;
}

void Scene::setUseHeadlight(const bool state)
{
	const ScopedLock scopedLock(objectLock);

	useViewHeadlight_ = state;
}

}

}
