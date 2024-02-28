// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/globalillumination/GIScene.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIScene::GIScene() :
	GITransform(),
	Scene()
{
	// nothing to do here
}

GIScene::~GIScene()
{
	// nothing to do here
}

void GIScene::setUseHeadlight(const bool state)
{
	ocean_assert(state && "Missing implementation!");

	Scene::setUseHeadlight(state);
}

}

}

}
