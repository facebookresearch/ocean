/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SPOT_LIGHT_H
#define META_OCEAN_RENDERING_GLES_SPOT_LIGHT_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESPointLight.h"

#include "ocean/rendering/SpotLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph spot light.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESSpotLight :
	virtual public GLESPointLight,
	virtual public SpotLight
{
	friend class GLESFactory;

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
		 * Returns the sharpness of the spot as fall off exponent.
		 * @see SpotLight::spotExponent().
		 */
		Scalar spotExponent() const override;

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

		/**
		 * Binds this light source.
		 * @see GLESLightSource::bindLight().
		 */
		void bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index) override;

	protected:

		/**
		 * Creates a new GLESceneGraph spot light object.
		 */
		GLESSpotLight();

		/**
		 * Destructs a GLESceneGraph spot light object.
		 */
		~GLESSpotLight() override;

	protected:

		/// Light direction inside the local coordinate system.
		Vector3 direction_;

		/// Light cone angle.
		Scalar coneAngle_;

		/// Light spot exponent.
		Scalar spotExponent_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SPOT_LIGHT_H
