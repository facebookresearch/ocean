/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_DIRECTIONAL_LIGHT_H
#define META_OCEAN_RENDERING_GLES_DIRECTIONAL_LIGHT_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"

#include "ocean/rendering/DirectionalLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph directional light object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESDirectionalLight :
	virtual public GLESLightSource,
	virtual public DirectionalLight
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the direction of this directional light.
		 * @see DirectionalLight::direction().
		 */
		Vector3 direction() const override;

		/**
		 * Sets the direction of this directional light.
		 * @see DirectionalLight::setDirection().
		 */
		void setDirection(const Vector3& direction) override;

		/**
		 * Binds this light source.
		 * @see GLESLightSource::bindLight().
		 */
		void bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index) override;

	protected:

		/**
		 * Creates a new GLESceneGraph directional light object.
		 */
		GLESDirectionalLight();

		/**
		 * Destructs a GLESceneGraph directional light object.
		 */
		~GLESDirectionalLight() override;

	protected:

		/// Direction of the light in relation to the local coordinate system.
		Vector3 direction_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_DIRECTIONAL_LIGHT_H
