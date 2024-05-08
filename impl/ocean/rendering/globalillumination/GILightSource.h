/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_LIGHT_SOURCE_H
#define META_OCEAN_RENDERING_GI_LIGHT_SOURCE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIObject.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/rendering/LightSource.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

// Forward declaration.
class GILightSource;

/**
 * Definition of a pair holding a light source and a homogenous matrix.
 * @ingroup renderinggi
 */
typedef std::pair<SmartObjectRef<GILightSource>, HomogenousMatrix4> LightPair;

/**
 * Definition of a vector holding light pairs.
 * @ingroup renderinggi
 */
typedef std::vector<LightPair> LightSources;

/**
 * This class is the base class for all Global Illumination light sources.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GILightSource :
	virtual public GIObject,
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
		 * Returns the ambient color of this light source.
		 * @see ambientColor().
		 */
		inline const RGBAColor& ambientColorInline() const;

		/**
		 * Returns the diffuse color of this light source.
		 * @see diffuseColor().
		 */
		inline const RGBAColor& diffuseColorInline() const;

		/**
		 * Returns the specular color of this light source.
		 * @see specularColor().
		 */
		inline const RGBAColor& specularColorInline() const;

		/**
		 * Returns the intensity of this light source.
		 * @see intensity().
		 */
		inline Scalar intensityInline() const;

		/**
		 * Returns whether this light source is enabled.
		 * @see enabled().
		 */
		inline bool enabledInline() const;

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
		 * Sets whether the light source is enabled.
		 * @see LightSource::setEnabled().
		 */
		void setEnabled(const bool state) override;

	protected:

		/**
		 * Creates a new Global Illumination light source.
		 */
		GILightSource();

		/**
		 * Destructs a Global Illumination light source.
		 */
		~GILightSource() override;

	protected:

		/// Ambient light color.
		RGBAColor lightAmbientColor;

		/// Diffuse light color.
		RGBAColor lightDiffuseColor;

		/// Specular light color.
		RGBAColor lightSpecularColor;

		/// Light intensity.
		Scalar lightIntensity;

		/// Determines whether the light is enabled.
		bool lightEnabled;
};

inline const RGBAColor& GILightSource::ambientColorInline() const
{
	return lightAmbientColor;
}

inline const RGBAColor& GILightSource::diffuseColorInline() const
{
	return lightDiffuseColor;
}

inline const RGBAColor& GILightSource::specularColorInline() const
{
	return lightSpecularColor;
}

inline Scalar GILightSource::intensityInline() const
{
	return lightIntensity;
}

inline bool GILightSource::enabledInline() const
{
	return lightEnabled;
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_LIGHT_SOURCE_H
