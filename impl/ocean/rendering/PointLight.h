/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_POINT_LIGHT_H
#define META_OCEAN_RENDERING_POINT_LIGHT_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/LightSource.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class PointLight;

/**
 * Definition of a smart object reference holding a point light.
 * @see SmartObjectRef, PointLight.
 * @ingroup rendering
 */
typedef SmartObjectRef<PointLight> PointLightRef;

/**
 * This class is the base class for all point lights.
 * In addition to the values of each light source a point light is defined by a position and three attenuation factors.<br>
 * The position defined the 3D position of the point light in relation to the local coordinate system.<br>
 * Default position is (0, 0, 0).<br>
 * The attenuation factors define the light attenuation depending on the distance between light source and object.<br>
 * A three element vector with a constant, a linear and a quadratic attenuation factor can be defined.<br>
 * The entire attenuation factor is defined by: 1 / constant + linear * r + quadratic * r^2, with distance r.<br>
 * Default attenuation factors are (1, 0, 0) meaning a disabled light attenuation.<br>
 * @see LightSource, SpotLight, DirectionalLight.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT PointLight : virtual public LightSource
{
	public:

		/**
		 * Returns the position of the light object.
		 * @return Light position, default is (0, 0, 0)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vector3 position() const;

		/**
		 * Returns the attenuation factors of the point light.
		 * @return Three element vector with attenuation factors, with order (constant, linear, quadratic)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vector3 attenuation() const;

		/**
		 * Sets the position of the light object.
		 * @param position Light position to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setPosition(const Vector3& position);

		/**
		 * Sets the attenuation factors for the point light.
		 * @param factors Three element attenuation factor vector with order (constant, linear, quadratic)
		 * @return True, if all three attenuation parameters are valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setAttenuation(const Vector3& factors);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new point light.
		 */
		PointLight();

		/**
		 * Destructs a point light.
		 */
		~PointLight() override;
};

}

}

#endif // META_OCEAN_RENDERING_POINT_LIGHT_H
