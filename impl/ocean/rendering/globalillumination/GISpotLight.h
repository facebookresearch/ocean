/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_SPOT_LIGHT_H
#define META_OCEAN_RENDERING_GI_SPOT_LIGHT_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIPointLight.h"

#include "ocean/rendering/SpotLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a spot light.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GISpotLight :
	virtual public GIPointLight,
	virtual public SpotLight
{
	friend class GIFactory;

	public:

		/**
		 * Returns the direction of this directional light.
		 * @see SpotLight::direction().
		 */
		Vector3 direction() const override;

		/**
		 * Returns the angle of the light cone in radian.
		 * @see SpotLight::coneAngle().
		 */
		Scalar coneAngle() const override;

		/**
		 * Returns the cosine of the cone angle.
		 * @return Cosine of the cone angle
		 * @see coneAngle()
		 */
		inline Scalar coneAngleCos() const;

		/**
		 * Returns the sharpness of the spot as fall off exponent.
		 * @see SpotLight::spotExponent().
		 */
		Scalar spotExponent() const override;

		/**
		 * Returns the direction of this directional light.
		 * @see direction().
		 */
		inline const Vector3& directionInline() const;

		/**
		 * Returns the angle of the light cone in radian.
		 * @see coneAngle().
		 */
		inline Scalar coneAngleInline() const;

		/**
		 * Returns the sharpness of the spot as fall off exponent.
		 * @see spotExponent().
		 */
		inline Scalar spotExponentInline() const;

		/**
		 * Sets the direction of this directional light.
		 * @see SpotLight::setDirection().
		 */
		void setDirection(const Vector3& direction) override;

		/**
		 * Sets the angle of the light cone in radian.
		 * @see SpotLight::setConeAngle().
		 */
		bool setConeAngle(const Scalar angle) override;

		/**
		 * Sets the sharpness of the spot as fall off exponent.
		 * @see SpotLight::setSpotExponent().
		 */
		bool setSpotExponent(const Scalar exponent) override;

	protected:

		/**
		 * Creates a new spot light object.
		 */
		GISpotLight();

		/**
		 * Destructs a spot light object.
		 */
		~GISpotLight() override;

	protected:

		/// Light direction inside the local coordinate system.
		Vector3 lightDirection;

		/// Light cone angle.
		Scalar lightConeAngle;

		/// Cosine of the light cone angle.
		Scalar lightConeAngleCos;

		/// Light spot exponent.
		Scalar lightSpotExponent;
};

inline Scalar GISpotLight::coneAngleCos() const
{
	return lightConeAngleCos;
}

const Vector3& GISpotLight::directionInline() const
{
	return lightDirection;
}

inline Scalar GISpotLight::coneAngleInline() const
{
	return lightConeAngle;
}

inline Scalar GISpotLight::spotExponentInline() const
{
	return lightSpotExponent;
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_SPOT_LIGHT_H
