/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_MATERIAL_H
#define META_OCEAN_RENDERING_MATERIAL_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"

#include "ocean/math/RGBAColor.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Material;

/**
 * Definition of a smart object reference holding a material.
 * @see SmartObjectRef, Material.
 * @ingroup rendering
 */
typedef SmartObjectRef<Material> MaterialRef;

/**
 * This class is the base class for all materials.
 * Each material is composed by different color components and appearance values:<br>
 * \b Ambient color models light that is inherent in the global scene due to global illumination, (0.2, 0.2, 0.2) as default.<br>
 * \b Diffuse color models light reflected on rough surfaces, (0.8, 0.8, 0.8) as default.<br>
 * \b Emissive color enabled objects to seam self-luminous, (0.0, 0.0, 0.0) as default.<br>
 * \b Specular color models  light reflected on shiny surfaces, (0.0, 0.0, 0.0) as default.<br>
 * \b Specular exponent models the sharpness of the reflection with range [0.0, infinity), 0.0 as default.<br>
 * \b Transparency models transparent surfaces with range [0.0, 1.0], 0.0 (fully opaque) as default.<br>
 *
 * Use the material object to define the appearance of geometry nodes.<br>
 * Each geometry node is associated with at most one material object.<br>
 * The material object has to be inserted to an attribute set which itself is connected to the geometry node.<br>
 * @see AttributeSet, Geometry, Attribute
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Material : virtual public Attribute
{
	public:

		/**
		 * Returns the ambient color of this material.
		 * @return Ambient color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setAmbientColor().
		 */
		virtual RGBAColor ambientColor() const;

		/**
		 * Returns the diffuse color of this material.
		 * @return Diffuse color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setDiffuseColor().
		 */
		virtual RGBAColor diffuseColor() const;

		/**
		 * Returns the emissive color of this material.
		 * @return Emissive color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setEmissiveColor().
		 */
		virtual RGBAColor emissiveColor() const;

		/**
		 * Returns the specular color of this material.
		 * @return Specular color
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setSpecularColor().
		 */
		virtual RGBAColor specularColor() const;

		/**
		 * Returns the specular exponent of this material with range [0.0, infinity).
		 * The higher the specular exponent value the sharper the reflection area.
		 * @return Specular exponent
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setSpecularExponent().
		 */
		virtual float specularExponent() const;

		/**
		 * Returns the transparency factor of this material with range [0.0, 1.0].
		 * 0.0 means fully opaque, 1.0 means fully transparent.
		 * @return Transparency factor
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setTransparency().
		 */
		virtual float transparency() const;

		/**
		 * Returns the reflectivity factor of this material with range [0.0, 1.0].
		 * 0.0 means no reflection, 1.0 means total reflection.<br>
		 * Beware: This parameter it used for global illumination and therefore not supported by most engines.
		 * @return Reflection factor
		 * @exception NotSupportedExcecption Is thrown if this function is not supported
		 * @see setReflectivity().
		 */
		virtual float reflectivity() const;

		/**
		 * Returns the index of refraction of this material with range [0.0, 1.0].
		 * The default value is 1 (as for standard for vacuum; water has 1.33, fused quartz glass has 1.46).
		 * Beware: This parameter it used for global illumination and therefore not supported by most engines.
		 * @return Index of refraction
		 * @exception NotSupportedExcecption Is thrown if this function is not supported
		 * @see setRefractionIndex().
		 */
		virtual float refractionIndex() const;

		/**
		 * Gets all color values of the material at once.
		 * @param ambient Resulting ambient color of this material
		 * @param diffuse Resulting diffuse color of this material
		 * @param emissive Resulting emissive color of this material
		 * @param specular Resulting specular color of this material
		 * @param specularExponent Resulting specular exponent of this material with range [0.0, infinity)
		 * @param transparency Resulting transparency factor of this material with range [0.0, 1.0]
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see set().
		 */
		virtual void get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& emissive, RGBAColor& specular, float& specularExponent, float& transparency);

		/**
		 * Sets the ambient color of the material.
		 * @param color Ambient color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see ambientColor().
		 */
		virtual bool setAmbientColor(const RGBAColor& color);

		/**
		 * Sets the diffuse color of the material.
		 * @param color Diffuse color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see diffuseColor().
		 */
		virtual bool setDiffuseColor(const RGBAColor& color);

		/**
		 * Sets the emissive color of the material.
		 * @param color Emissive color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see emissiveColor().
		 */
		virtual bool setEmissiveColor(const RGBAColor& color);

		/**
		 * Sets the specular color of the material.
		 * @param color Specular color
		 * @return True, if the specified color is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see specularColor().
		 */
		virtual bool setSpecularColor(const RGBAColor& color);

		/**
		 * Sets the specular exponent of this material with range [0.0, infinity).
		 * The higher the specular exponent value the sharper the reflection area.
		 * @param specularExponent Specular exponent
		 * @return True, if the specified exponent is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see specularExponent().
		 */
		virtual bool setSpecularExponent(const float specularExponent);

		/**
		 * Sets the transparency of the material.
		 * 0.0 means fully opaque, 1.0 means fully transparent.
		 * @param transparency Transparency factor of the material with range [0.0, 1.0]
		 * @return True, if the specified value is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see transparency().
		 */
		virtual bool setTransparency(const float transparency);

		/**
		 * Sets the reflectivity of this material.
		 * 0 means no reflection, 1 means total reflection.<br>
		 * Beware: This parameter it used for global illumination and therefore not supported by most engines.
		 * @param reflectivity Reflectivity factor of this material with range [0.0, 1.0]
		 * @return True, if the specified value is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see reflectivity().
		 */
		virtual bool setReflectivity(const float reflectivity);

		/**
		 * Sets the index of refraction of this material.
		 * The default value is 1 (as for standard for vacuum; water has 1.33, fused quartz glass has 1.46).
		 * Beware: This parameter it used for global illumination and therefore not supported by most engines.
		 * @param index Refraction index to be set with range [0.0, infinity)
		 * @return True, if the specified value is valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see refractionIndex().
		 */
		virtual bool setRefractionIndex(const float index);

		/**
		 * Sets all color values of the material at once.
		 * @param ambient Ambient color to set
		 * @param diffuse Diffuse color to set
		 * @param emissive Emissive color to set
		 * @param specular Specular color to set
		 * @param specularExponent Specular exponent to set with range [0.0, infinity)
		 * @param transparency Transparency factor to set with range [0.0, 1.0]
		 * @return True, if at least one specified value was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see get().
		 */
		virtual bool set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& emissive, const RGBAColor& specular, const float specularExponent, const float transparency);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new material object.
		 */
		Material();

		/**
		 * Destructs a meterial object.
		 */
		~Material() override;
};

}

}

#endif // META_OCEAN_RENDERING_MATERIAL_H
