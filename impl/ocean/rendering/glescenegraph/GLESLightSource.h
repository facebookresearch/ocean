/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_LIGHT_SOURCE_H
#define META_OCEAN_RENDERING_GLES_LIGHT_SOURCE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/rendering/LightSource.h"
#include "ocean/rendering/ObjectRef.h"

#include "ocean/math/HomogenousMatrix4.h"

#include <vector>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

// Forward declaration.
class GLESLightSource;

/**
 * Definition of a pair combinding light sources with transformation matrices.
 * @ingroup renderinggles
 */
typedef std::pair<SmartObjectRef<GLESLightSource>, HomogenousMatrix4> LightPair;

/**
 * Definition of a set holding light sources.
 * @ingroup renderinggles
 */
typedef std::vector<LightPair> Lights;

/**
 * This class is the base class for all GLESceneGraph light sources.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESLightSource :
	virtual public GLESObject,
	virtual public LightSource
{
	public:

		/**
		 * Returns the ambient color of this light source.
		 * @see LightSource::ambientColor().
		 */
		RGBAColor ambientColor() const override;

		/**
		 * Returns the diffuse color of this light source.
		 * @see LightSource::diffuseColor().
		 */
		RGBAColor diffuseColor() const override;

		/**
		 * Returns the specular color of this light source.
		 * @see LightSource::specularColor().
		 */
		RGBAColor specularColor() const override;

		/**
		 * Returns the intensity of this light source.
		 * @see LightSource::intensity().
		 */
		Scalar intensity() const override;

		/**
		 * Returns the transformation type of this light.
		 * @see LightSource::transformationType().
		 */
		TransformationType transformationType() const override;

		/**
		 * Returns whether this light source is enabled.
		 * @see LightSource::enabled().
		 */
		bool enabled() const override;

		/**
		 * Gets all color values of the light at once.
		 * @see LightSource::get().
		 */
		void get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& specular, Scalar& intensity) override;

		/**
		 * Sets the ambient color of the light source.
		 * @see LightSource::setAmbientcolor().
		 */
		bool setAmbientColor(const RGBAColor& color) override;

		/**
		 * Sets the diffuse color of the light source.
		 * @see LightSource::setDiffuseColor().
		 */
		bool setDiffuseColor(const RGBAColor& color) override;

		/**
		 * Sets the specular color of the light source.
		 * @see LightSource::setSpecularColor().
		 */
		bool setSpecularColor(const RGBAColor& color) override;

		/**
		 * Sets the specular exponent of this material.
		 * @see LightSource::setIntensity().
		 */
		bool setIntensity(const Scalar intensity) override;

		/**
		 * Sets all color values of the light source at once.
		 * @see LightSource::set().
		 */
		bool set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& specular, const Scalar intensity) override;

		/**
		 * Sets the transformation type of this light.
		 * @see LightSource::setTransformationType().
		 */
		void setTransformationType(const TransformationType transformationType) override;

		/**
		 * Sets whether the light source is enabled.
		 * @see LightSource::setEnabled().
		 */
		void setEnabled(const bool state) override;

		/**
		 * Binds this light source to the shader.
		 * @param shaderProgram Shader program receiving the light source information
		 * @param camera_T_model The transformation between model and camera (aka Modelview matrix), must be valid
		 * @param camera_T_world The transformation between world and camera,(aka View matrix) must be valid
		 * @param normalMatrix Normal transformation matrix with is the transposed inverse of the upper 3x3 model Modelview matrix, must be valid
		 * @param index Light source index inside the shader
		 */
		virtual void bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index);

	protected:

		/**
		 * Creates a new GLESceneGraph light source.
		 */
		GLESLightSource();

		/**
		 * Destructs a GLESceneGraph light source.
		 */
		~GLESLightSource() override;

	protected:

		/// Ambient light color.
		RGBAColor ambientColor_;

		/// Diffuse light color.
		RGBAColor diffuseColor_;

		/// Specular light color.
		RGBAColor specularColor_;

		/// Light intensity.
		Scalar intensity_;

		/// The transformation type of this light source.
		TransformationType transformationType_ = TT_PARENT;

		/// Determines whether the light is enabled.
		bool enabled_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_LIGHT_SOURCE_H
