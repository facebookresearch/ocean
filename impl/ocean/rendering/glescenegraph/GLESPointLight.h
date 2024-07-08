/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_POINT_LIGHT_H
#define META_OCEAN_RENDERING_GLES_POINT_LIGHT_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"

#include "ocean/rendering/PointLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph point light object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESPointLight :
	virtual public GLESLightSource,
	virtual public PointLight
{
	friend class GLESFactory;

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
		 * Sets the position of the light object.
		 * @see PointLight::setPosition().
		 */
		void setPosition(const Vector3& position) override;

		/**
		 * Sets the attenuation factors for the point light.
		 * @see PointLight::setAttenuation().
		 */
		bool setAttenuation(const Vector3& factors) override;

		/**
		 * Binds this light source.
		 * @see GLESLightSource::bindLight().
		 */
		void bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index) override;

	protected:

		/**
		 * Creates a new GLESceneGraph point light object.
		 */
		GLESPointLight();

		/**
		 * Destructs a GLESceneGraph point light object.
		 */
		~GLESPointLight() override;

	protected:

		/// Light position inside the local coordinate system.
		Vector3 position_;

		/// Attenuation factor vector, with order (constant, linear, quadratic).
		Vector3 attenuationFactors_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_POINT_LIGHT_H
