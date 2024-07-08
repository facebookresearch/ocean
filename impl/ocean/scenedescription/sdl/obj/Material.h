/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MATERIAL_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MATERIAL_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Engine.h"

#include "ocean/math/RGBAColor.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration
class SDLScene;

namespace SDL
{

namespace OBJ
{

/**
 * This class hold a material defined in a mtl obj material file.
 * @ingroup scenedescriptionsdlobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT Material
{
	public:

		/**
		 * Definition of illumination models.
		 */
		typedef unsigned int IlluminationModel;

	public:

		/**
		 * Creates a new material object with default values.
		 */
		Material() = default;

		/**
		 * Returns the name of this material.
		 * @return Material name
		 */
		inline const std::string& name() const;

		/**
		 * Returns the ambient color of this material.
		 * @return Ambient color
		 */
		inline const RGBAColor& ambientColor() const;

		/**
		 * Returns the diffuse color of this material.
		 * @return Diffuse color
		 */
		inline const RGBAColor& diffuseColor() const;

		/**
		 * Returns the emissive color of this material.
		 * @return Emissive color
		 */
		inline const RGBAColor& emissiveColor() const;

		/**
		 * Returns the specular color of this material.
		 * @return Specular color
		 */
		inline const RGBAColor& specularColor() const;

		/**
		 * Returns the transparency of this material with range [0.0, 1.0]
		 * 0 means fully opaque, 1 means fully transparent.
		 * @return Material transparency
		 */
		inline float transparency() const;

		/**
		 * Returns the transmission filter of this material.
		 * @return Transmission filter
		 */
		inline const RGBAColor& transmissionFilter() const;

		/**
		 * Returns the illumination model of this material.
		 * @return Illumination model
		 */
		inline IlluminationModel illuminationModel() const;

		/**
		 * Returns the specular exponent of this material.
		 * @return Specular exponent
		 */
		inline Scalar specularExponent() const;

		/**
		 * Returns the texture name of this material.
		 * @return Texture name
		 */
		inline const std::string& textureName() const;

		/**
		 * Returns the attribute set of this material.
		 * @param engine The rendering engine to use
		 * @param scene The scene object for which the attribute set will be created
		 * @return The resulting attribute set
		 */
		Rendering::AttributeSetRef attributeSet(const Rendering::EngineRef& engine, const SDLScene& scene);

		/**
		 * Sets the name of this material.
		 * @param name Name to set
		 */
		inline void setName(const std::string& name);

		/**
		 * Sets the ambient color of this material.
		 * @param color Ambient color to set
		 */
		inline void setAmbientColor(const RGBAColor& color);

		/**
		 * Sets the diffuse color of this material.
		 * @param color Diffuse color to set
		 */
		inline void setDiffuseColor(const RGBAColor& color);

		/**
		 * Sets the emissive color of this material.
		 * @param color Emissive color to set
		 */
		inline void setEmissiveColor(const RGBAColor& color);

		/**
		 * Sets the specular color of this material.
		 * @param color Specular color to set
		 */
		inline void setSpecularColor(const RGBAColor& color);

		/**
		 * Sets the transparency value of this material with range [0.0, 1.0].
		 * 0 means fully opaque, 1 means fully transparent.
		 * @param transparency Transparency value to set
		 */
		inline void setTransparency(const float transparency);

		/**
		 * Sets the transmission filter of this material.
		 * @param filter Filter to set
		 */
		inline void setTransmissionFilter(const RGBAColor& filter);

		/**
		 * Sets the illumination model of this material.
		 * @param model Illumination model to set
		 */
		inline void setIlluminationModel(const IlluminationModel model);

		/**
		 * Sets the specular exponent of this material.
		 * @param exponent Specular exponent to set
		 */
		inline void setSpecularExponent(const Scalar exponent);

		/**
		 * Sets the texture name of this material.
		 * @param name Texture name to set
		 */
		inline void setTextureName(const std::string& name);

	protected:

		/// Holds the name of this material.
		std::string name_;

		/// Holds the ambient color of this material.
		RGBAColor ambientColor_ = RGBAColor(0.2f, 0.2f, 0.2f);

		/// Holds the diffuse color of this material.
		RGBAColor diffuseColor_ = RGBAColor(0.8f, 0.8f, 0.8f);

		/// Holds the emissive color of this material.
		RGBAColor emissiveColor_ = RGBAColor(0, 0, 0);

		/// Holds the specular color of this material.
		RGBAColor specularColor_ = RGBAColor(1, 1, 1);

		/// Holds the transparency value of this material with range [0 (opaque), 1 (transparent)].
		float transparency_ = 0.0f;

		/// Holds the transmission filter of this material.
		RGBAColor transmissionFilter_;

		/// Holds the illumination model of this material, a  diffuse illumination model by default.
		IlluminationModel iIlluminationModel_ = IlluminationModel(1);

		/// Holds the specular exponent of this material.
		Scalar specularExponent_ = Scalar(0);

		/// Holds the texture name of this material.
		std::string textureName_;

		/// Rendering attribute set.
		Rendering::AttributeSetRef attributeSet_;
};

inline const std::string& Material::name() const
{
	return name_;
}

inline const RGBAColor& Material::ambientColor() const
{
	return ambientColor_;
}

inline const RGBAColor& Material::diffuseColor() const
{
	return diffuseColor_;
}

inline const RGBAColor& Material::emissiveColor() const
{
	return emissiveColor_;
}

inline const RGBAColor& Material::specularColor() const
{
	return specularColor_;
}

inline float Material::transparency() const
{
	return transparency_;
}

inline const RGBAColor& Material::transmissionFilter() const
{
	return transmissionFilter_;
}

inline Material::IlluminationModel Material::illuminationModel() const
{
	return iIlluminationModel_;
}

inline Scalar Material::specularExponent() const
{
	return specularExponent_;
}

const std::string& Material::textureName() const
{
	return textureName_;
}

inline void Material::setName(const std::string& name)
{
	name_ = name;
}

inline void Material::setAmbientColor(const RGBAColor& color)
{
	ambientColor_ = color;
}

inline void Material::setDiffuseColor(const RGBAColor& color)
{
	diffuseColor_ = color;
}

inline void Material::setEmissiveColor(const RGBAColor& color)
{
	emissiveColor_ = color;
}

inline void Material::setSpecularColor(const RGBAColor& color)
{
	specularColor_ = color;
}

inline void Material::setTransparency(const float transparency)
{
	transparency_ = transparency;
}

inline void Material::setTransmissionFilter(const RGBAColor& filter)
{
	transmissionFilter_ = filter;
}

inline void Material::setIlluminationModel(const IlluminationModel model)
{
	iIlluminationModel_ = model;
}

inline void Material::setSpecularExponent(const Scalar exponent)
{
	specularExponent_ = exponent;
}

inline void Material::setTextureName(const std::string& name)
{
	textureName_ = name;
}

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MATERIAL_H
