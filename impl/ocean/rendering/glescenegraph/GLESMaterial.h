/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_MATERIAL_H
#define META_OCEAN_RENDERING_GLES_MATERIAL_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph material object.
 * @see Material
 * @ingroup renderingles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESMaterial :
	virtual public GLESAttribute,
	virtual public Material
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the ambient color of this material.
		 * @see Material::ambientColor().
		 */
		RGBAColor ambientColor() const override;

		/**
		 * Returns the diffuse color of this material.
		 * @see Material::diffuseColor().
		 */
		RGBAColor diffuseColor() const override;

		/**
		 * Returns the emissive color of this material.
		 * @see Material::emissiveColor().
		 */
		RGBAColor emissiveColor() const override;

		/**
		 * Returns the specular color of this material.
		 * @see Material::specularColor().
		 */
		RGBAColor specularColor() const override;

		/**
		 * Returns the specular exponent of this material.
		 * @see Material::specularExponent().
		 */
		float specularExponent() const override;

		/**
		 * Returns the transparency factor of this material with range [0.0, 1.0]
		 * @see Material::transparency().
		 */
		float transparency() const override;

		/**
		 * Gets all color values of the material at once.
		 * @see Material::get().
		 */
		void get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& emissive, RGBAColor& specular, float& specularExponent, float& transparency) override;

		/**
		 * Sets the ambient color of the material.
		 * @see Material::setAmbientColor().
		 */
		bool setAmbientColor(const RGBAColor& color) override;

		/**
		 * Sets the diffuse color of the material.
		 * @see Material::setDiffuseColor().
		 */
		bool setDiffuseColor(const RGBAColor& color) override;

		/**
		 * Sets the emissive color of the material.
		 * @see Material::setEmissiveColor().
		 */
		bool setEmissiveColor(const RGBAColor& color) override;

		/**
		 * Sets the specular color of the material.
		 * @see Material::setSpecularColor().
		 */
		bool setSpecularColor(const RGBAColor& color) override;

		/**
		 * Sets the specular exponent of this material.
		 * @see Material::setSpecularExponent().
		 */
		bool setSpecularExponent(const float specularExponent) override;

		/**
		 * Sets the specular exponent of this material.
		 * @see Material::setTransparency().
		 */
		bool setTransparency(const float transparency) override;

		/**
		 * Sets the transparency of the meterial.
		 * @see Material::set().
		 */
		bool set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& emissive, const RGBAColor& specular, const float specularExponent, const float transparency) override;

		/**
		 * Binds this attribute.
		 * @see GLESAttribute::bindAttribute().
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Binds this material attribute.
		 * @param framebuffer Framebuffer initiated the rendering process
		 * @param shaderProgram Shader program to be applied
		 * @param materialName The name of the material uniform, must be valid
		 */
		void bindMaterial(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram, const std::string& materialName);

		/**
		 * Returns the shader type necessary to render an object with this attribute.
		 * @see GLESAttribute::necessaryShader().
		 */
		GLESAttribute::ProgramType necessaryShader() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph material object.
		 */
		GLESMaterial();

		/**
		 * Destructs a GLESceneGraph material object.
		 */
		~GLESMaterial() override;

	protected:

		/// Ambient color.
		RGBAColor ambientColor_;

		/// Diffuse color.
		RGBAColor diffuseColor_;

		/// Emissive color.
		RGBAColor emissiveColor_;

		/// Specular color.
		RGBAColor specularColor_;

		/// Specular exponent.
		float specularExponent_;

		/// Transparency value, with range [0, 1]
		float transparency_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_MATERIAL_H
