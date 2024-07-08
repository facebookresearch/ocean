/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_POINT_LIGHT_H
#define META_OCEAN_RENDERING_GI_POINT_LIGHT_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GILightSource.h"

#include "ocean/rendering/PointLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a point light.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIPointLight :
	virtual public GILightSource,
	virtual public PointLight
{
	friend class GIFactory;

	public:

		/**
		 * Returns the position of the light object.
		 * @see PointLight::position().
		 */
		Vector3 position() const override;

		/**
		 * Returns the attenuation factors of the point light.
		 * @see PointLight::attenuation().
		 */
		Vector3 attenuation() const override;

		/**
		 * Returns the position of the light object.
		 * @see position().
		 */
		inline const Vector3& positionInline() const;

		/**
		 * Returns the attenuation factors of the point light.
		 * @see attenuation().
		 */
		inline const Vector3& attenuationInline() const;

		/**
		 * Sets the position of the light object.
		 * @see PointLight::setPosition().
		 */
		void setPosition(const Vector3& position) override;

		/**
		 * Sets the attenuation factors for the point light.
		 * @see PointLight::setAttenuation().
		 */
		bool setAttenuation(const Vector3& factors) override;

	protected:

		/**
		 * Creates a new point light object.
		 */
		GIPointLight();

		/**
		 * Destructs a point light object.
		 */
		~GIPointLight() override;

	protected:

		/// Light position inside the local coordinate system.
		Vector3 lightPosition;

		/// Attenuation factor vector, with order (constant, linear, quadratic).
		Vector3 lightAttenuationFactors;
};

inline const Vector3& GIPointLight::positionInline() const
{
	return lightPosition;
}

inline const Vector3& GIPointLight::attenuationInline() const
{
	return lightAttenuationFactors;
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_POINT_LIGHT_H
