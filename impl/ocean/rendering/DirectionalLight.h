/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_DIRECTIONAL_LIGHT_H
#define META_OCEAN_RENDERING_DIRECTIONAL_LIGHT_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/LightSource.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class DirectionalLight;

/**
 * Definition of a smart object reference holding a directional light.
 * @see SmartObjectRef, DirectionalLight.
 * @ingroup rendering
 */
typedef SmartObjectRef<DirectionalLight> DirectionalLightRef;

/**
 * This class is the base class for all directional lights.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT DirectionalLight : virtual public LightSource
{
	public:

		/**
		 * Returns the direction of this directional light.
		 * @return Light direction
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vector3 direction() const;

		/**
		 * Sets the direction of this directional light.
		 * @param direction Direction to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setDirection(const Vector3& direction);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new directional light.
		 */
		DirectionalLight();

		/**
		 * Destructs a directional light.
		 */
		~DirectionalLight() override;
};

}

}

#endif // META_OCEAN_RENDERING_DIRECTIONAL_LIGHT_H
