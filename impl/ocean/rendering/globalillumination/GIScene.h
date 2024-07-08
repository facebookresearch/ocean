/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_SCENE_H
#define META_OCEAN_RENDERING_GI_SCENE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GITransform.h"

#include "ocean/rendering/Scene.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class wraps a Global Illumination scene object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIScene :
	virtual public GITransform,
	virtual public Scene
{
	friend class GIFactory;

	protected:

		/**
		 * Creates a new Global Illumination scene object.
		 */
		GIScene();

		/**
		 * Destructs a Global Illumination scene object.
		 */
		~GIScene() override;

		/**
		 * Sets whether this scene is lit with the view's headlight in case the view's headlight is active.
		 * @see Scene::setUseHeadlight().
		 */
		void setUseHeadlight(const bool state) override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_SCENE_H
