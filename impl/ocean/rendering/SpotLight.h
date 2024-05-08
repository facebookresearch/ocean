/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SPOT_LIGHT_H
#define META_OCEAN_RENDERING_SPOT_LIGHT_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/PointLight.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class SpotLight;

/**
 * Definition of a smart object reference holding a spot light.
 * @see SmartObjectRef, SpotLight.
 * @ingroup rendering
 */
typedef SmartObjectRef<SpotLight> SpotLightRef;

/**
 * This class is the base class for all spot lights.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT SpotLight : virtual public PointLight
{
	public:

		/**
		 * Returns the direction of this directional light.
		 * The default value is (0, 0, -1).
		 * @return Light direction
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vector3 direction() const;

		/**
		 * Returns the angle of the light cone in radian.
		 * Objects outside this cone are unlit.
		 * @return Angle of light cone in radian with range [0.0, Pi / 2]
		 */
		virtual Scalar coneAngle() const;

		/**
		 * Returns the sharpness of the spot as fall off exponent.
		 * @return Spot exponent
		 */
		virtual Scalar spotExponent() const;

		/**
		 * Sets the direction of this directional light.
		 * The default value is (0, 0, -1).
		 * @param direction Direction to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setDirection(const Vector3& direction);

		/**
		 * Sets the angle of the light cone in radian.
		 * Objects outside this conde are unlit.
		 * @return True, if the angle is valid and could be set
		 * @param angle Angle of the light cone in radian with range [0.0, Pi / 2]
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setConeAngle(const Scalar angle);

		/**
		 * Sets the sharpness of the spot as fall off exponent.
		 * @param exponent Sharpness exponent to set with range [0.0, 1.0]
		 * @return True, if the value is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setSpotExponent(const Scalar exponent);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new sport light.
		 */
		SpotLight();

		/**
		 * Destructs a spot light.
		 */
		~SpotLight() override;
};

}

}

#endif // META_OCEAN_RENDERING_SPOT_LIGHT_H
