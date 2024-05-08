/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
