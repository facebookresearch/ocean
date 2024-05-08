/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_OBJECT_H
#define META_OCEAN_RENDERING_GI_OBJECT_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"

#include "ocean/rendering/Object.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class is the base class for all Global Illumination objects.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIObject : virtual public Object
{
	public:

		/**
		 * Returns the id of the owner engine.
		 * @see Objecgt::engineId().
		 */
		const std::string& engineName() const override;

	protected:

		/**
		 * Creates a new Global Illumination object.
		 */
		GIObject();

		/**
		 * Destructs a Global Illumination object.
		 */
		~GIObject() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_OBJECT_H
