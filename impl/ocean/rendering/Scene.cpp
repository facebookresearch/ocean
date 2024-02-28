// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
