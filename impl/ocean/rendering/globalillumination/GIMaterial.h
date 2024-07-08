/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_MATERIAL_H
#define META_OCEAN_RENDERING_GI_MATERIAL_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIAttribute.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a Global Illumination material object.
 * @see Material
 * @ingroup renderingi
 */
class OCEAN_RENDERING_GI_EXPORT GIMaterial :
	virtual public GIAttribute,
	virtual public Material
{
	friend class GIFactory;

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
		 * Returns the reflectivity factor of this material with range [0.0, 1.0].
		 * @see Material::reflectivity().
		 */
		float reflectivity() const override;

		/**
		 * Returns the index of refraction of this material with range [0.0, 1.0].
		 * @see Material::refractionIndex().
		 */
		float refractionIndex() const override;

		/**
		 * Gets all color values of the material at once.
		 * @see Material::get().
		 */
		void get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& emissive, RGBAColor& specular, float& specularExponent, float& transparency) override;

		/**
		 * Returns the ambient color of this material.
		 * @see ambientColor().
		 */
		inline const RGBAColor& ambientColorInline() const;

		/**
		 * Returns the diffuse color of this material.
		 * @see diffuseColor().
		 */
		inline const RGBAColor& diffuseColorInline() const;

		/**
		 * Returns the emissive color of this material.
		 * @see emissiveColor().
		 */
		inline const RGBAColor& emissiveColorInline() const;

		/**
		 * Returns the specular color of this material.
		 * @see specularColor().
		 */
		inline const RGBAColor& specularColorInline() const;

		/**
		 * Returns the specular exponent of this material.
		 * @see specularExponent().
		 */
		inline float specularExponentInline() const;

		/**
		 * Returns the transparency factor of this material with range [0.0, 1.0]
		 * @see transparency().
		 */
		inline float transparencyInline() const;

		/**
		 * Returns the reflectivity factor of this material with range [0.0, 1.0].
		 * @see reflectivity().
		 */
		inline float reflectivityInline() const;

		/**
		 * Returns the index of refraction of this material with range [0.0, 1.0].
		 * @see refractionIndex().
		 */
		inline float refractionIndexInline() const;

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
		 * Sets the reflectivity of this material.
		 * @see Material::setReflectivity().
		 */
		bool setReflectivity(const float reflectivity) override;

		/**
		 * Sets the index of refraction of this material.
		 * @see Material::setRefractionIndex().
		 */
		bool setRefractionIndex(const float index) override;

		/**
		 * Sets the transparency of the material.
		 * @see Material::set().
		 */
		bool set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& emissive, const RGBAColor& specular, const float specularExponent, const float transparency) override;

	protected:

		/**
		 * Creates a new Global Illumination material object.
		 */
		GIMaterial();

		/**
		 * Destructs a Global Illumination material object.
		 */
		~GIMaterial() override;

	protected:

		/// Ambient color.
		RGBAColor materialAmbientColor;

		/// Diffuse color.
		RGBAColor materialDiffuseColor;

		/// Emissive color.
		RGBAColor materialEmissiveColor;

		/// Specular color.
		RGBAColor materialSpecularColor;

		/// Specular exponent.
		float materialSpecularExponent;

		/// Transparency value.
		float materialTransparency;

		/// Specular exponent.
		float materialReflectivity;

		/// Transparency value.
		float materialRefractionIndex;
};

inline const RGBAColor& GIMaterial::ambientColorInline() const
{
	return materialAmbientColor;
}

inline const RGBAColor& GIMaterial::diffuseColorInline() const
{
	return materialDiffuseColor;
}

inline const RGBAColor& GIMaterial::emissiveColorInline() const
{
	return materialEmissiveColor;
}

inline const RGBAColor& GIMaterial::specularColorInline() const
{
	return materialSpecularColor;
}

inline float GIMaterial::specularExponentInline() const
{
	return materialSpecularExponent;
}

inline float GIMaterial::transparencyInline() const
{
	return materialTransparency;
}

inline float GIMaterial::reflectivityInline() const
{
	return materialReflectivity;
}

inline float GIMaterial::refractionIndexInline() const
{
	return materialRefractionIndex;
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_MATERIAL_H
