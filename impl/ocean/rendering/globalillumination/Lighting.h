/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_LIGHTING_H
#define META_OCEAN_RENDERING_GI_LIGHTING_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GILightSource.h"
#include "ocean/rendering/globalillumination/GIMaterial.h"
#include "ocean/rendering/globalillumination/GITextures.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

// Forward declaration.
class TracingObject;

// Forward declaration.
class TracingGroup;

/**
 * This class implements object lighting functions.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT Lighting
{
	public:

		/**
		 * Definition of individual lighting modes.
		 */
		enum LightingModes
		{
			/// No lighting is applied.
			LM_UNLIT = 0,
			/// Lambert lighting is applied.
			LM_SHADING_LAMBERT = (1 << 0),
			/// Lambert and specular lighting is applied.
			LM_SHADING_SPECULAR = (1 << 1) | LM_SHADING_LAMBERT,
			/// Transparency is supported.
			LM_TRANSPARENCY = (1 << 2),
			/// Specular reflection is supported.
			LM_REFLECTION_SPECULAR = (1 << 3),
			/// Transparency and specular refraction is supported
			LM_REFRACTION_SPECULAR = (1 << 4) | LM_TRANSPARENCY,
			/// Shadows are supported.
			LM_SHADOWS = (1 << 5),
			/// Supersample shadows are supported.
			LM_SHADOWS_SUPERSAMPLE = (1 << 6) | LM_SHADOWS,
			/// Translucent shadows and light damping is supported.
			LM_SHADOWS_TRANSLUCENT_LIGHT = (1 << 7) | LM_SHADOWS,
			/// Full shading is supported.
			LM_SHADING_FULL = LM_SHADING_LAMBERT | LM_SHADING_SPECULAR,
			/// Full reflection and refraction is supported.
			LM_REFLECTION_REFRACTION_FULL = LM_REFLECTION_SPECULAR | LM_REFRACTION_SPECULAR,
			/// Full shadows are supported.
			LM_SHADOWS_FULL = LM_SHADOWS | LM_SHADOWS_SUPERSAMPLE | LM_SHADOWS_TRANSLUCENT_LIGHT,
			/// Entire lighting functions are supported.
			LM_LIGHTING_FULL = LM_SHADING_FULL | LM_REFLECTION_REFRACTION_FULL | LM_SHADOWS_FULL
		};

		/**
		 * Determines the light (the color) for a specified viewing ray, intersection point, appearance information and light sources, including damped light from translucent objects between the intersection object and all light sources.
		 * @param viewPosition The start position of the viewing ray
		 * @param viewObjectDirection The direction of the viewing ray
		 * @param objectPosition The intersction location of the viewing ray with any kind of object
		 * @param objectNormal The normal at the intersection location of the object
		 * @param textureCoordinate The texture coordinate at the location of the intersection
		 * @param material The material information of the object which is intersected
		 * @param textures The textures of the object
		 * @param lightSources The light sources defining the entire amount of light in the scene
		 * @param object The tracing object which is intersected
		 * @param root The root of all tracing objects defining the entire scene
		 * @param bounces The number of reflection bounces to be used, with range [0, infinity)
		 * @param lightingModes The lighting modes defining lighting technique to be applied
		 * @param color The resulting color/light for the defined viewing ray
		 * @return True, if succeeded
		 */
		static bool dampedLight(const Vector3& viewPosition, const Vector3& viewObjectDirection, const Vector3& objectPosition, const Vector3& objectNormal, const Vector2& textureCoordinate, const GIMaterial* material, const GITextures* textures, const LightSources& lightSources, const TracingObject& object, const TracingGroup& root, const unsigned int bounces, const LightingModes lightingModes, RGBAColor& color);

		/**
		 * Determines the direct lighting for a specific location and light source using the Lambert shading with optional additional specular shading.
		 * The light as distributed from the light source is expected to be not attenuated by any obstacle between light and object.<br>
		 * However, the resulting color/light can be adjusted by any kind of adjustment factor e.g., to integrate any kind of attenuation.
		 * @param viewObjectDirection Direction starting at the viewing position (the center of projection) and pointing to the object intersection location, with unit length
		 * @param objectPosition The location on the object where the viewing ray intersects the object
		 * @param objectNormal The normal on the object where the viewing ray intersects the object
		 * @param lightObjectDirection Direction starting at the light source and pointing to the object intersection location, with unit length
		 * @param materialLightDiffuse Combination of diffuse light and material components
		 * @param materialLightSpecular Combination of specular light and material components
		 * @param materialSpecularExponent Specular material exponent
		 * @param lightingModes The lighting modes defining lighting technique to be applied
		 * @param diffuse Resulting diffuse color
		 * @param specular Resulting specular color
		 * @return True, if succeeded
		 */
		static bool directLight(const Vector3& viewObjectDirection, const Vector3& objectPosition, const Vector3& objectNormal, const Vector3& lightObjectDirection, const RGBAColor& materialLightDiffuse, const RGBAColor& materialLightSpecular, const Scalar materialSpecularExponent, const LightingModes lightingModes, RGBAColor& diffuse, RGBAColor& specular);

		/**
		 * Determines the light damping factors for a specific position and light source.
		 * @param lightPosition Position of the light source
		 * @param lightObjectDirection Direction starting at the light source pointing to the object intersection position, with unit length
		 * @param lightObjectDistance Distance between the object's intersection point and the light source
		 * @param lightColor Color of the light source
		 * @param root Root tracing object
		 * @param lightingModes The lighting modes defining lighting technique to be applied
		 * @return Resulting damping factors
		 */
		static RGBAColor lightDampingFactors(const Vector3& lightPosition, const Vector3& lightObjectDirection, const Scalar lightObjectDistance, const RGBAColor& lightColor, const TracingGroup& root, const LightingModes lightingModes);

	protected:

		/**
		 * Returns the attenuation factor for a given point light.
		 * @param lightAttenuation Constant, linear and square light attenuation factors
		 * @param lightObjectDistance Distance between the object's intersection point and the light source
		 * @param intensity Intensity of the light source, with range [0, 1]
		 * @return Resulting attenuation factor
		 */
		static inline Scalar pointLightAttenuationFactor(const Vector3& lightAttenuation, const Scalar lightObjectDistance, const Scalar intensity);

		/**
		 * Returns the attenuation factor for a given point light.
		 * @param lightAttenuation Constant, linear and square light attenuation factors
		 * @param lightObjectDistanceSqr Square distance between the object's intersection point and the light source, with range [0, infinity)
		 * @param intensity Intensity of the light source, with range [0, 1]
		 * @return Resulting attenuation factor
		 */
		static inline Scalar pointLightAttenuationFactorSqr(const Vector3& lightAttenuation, const Scalar lightObjectDistanceSqr, const Scalar intensity);

		/**
		 * Returns the attenuation factor for a given spot light.
		 * @param lightAttenuation Constant, linear and square light attenuation factors
		 * @param lightObjectDistance Distance between the object's intersection point and the light source
		 * @param intensity Intensity of the light source, with range [0, 1]
		 * @param lightDirection Direction of the spotlight starting at the light source pointing into the scene, with unit length
		 * @param lightObjectDirection Direction starting at the light source pointing to the object intersection position, with unit length
		 * @param coneAngleCos Cosine of the spont light cone angle
		 * @param spotExponent Lighting exponent of the spot light, with range [0, 128]
		 * @return Resulting attenuation factor
		 */
		static inline Scalar spotLightAttenuationFactor(const Vector3& lightAttenuation, const Scalar lightObjectDistance, const Scalar intensity, const Vector3& lightDirection, const Vector3& lightObjectDirection, const Scalar coneAngleCos, const Scalar spotExponent);
};

inline Scalar Lighting::pointLightAttenuationFactor(const Vector3& lightAttenuation, const Scalar lightObjectDistance, const Scalar intensity)
{
	ocean_assert(intensity >= 0 && intensity <= 1);

	if (lightAttenuation.isNull())
		return intensity;

	ocean_assert(lightAttenuation[0] >= 0);
	ocean_assert(lightAttenuation[1] >= 0);
	ocean_assert(lightAttenuation[2] >= 0);

	ocean_assert(intensity >= 0 && intensity <= 1);

	// check whether we have a simple attenuation case
	if (lightAttenuation[1] == 0 && lightAttenuation[2] == 0)
		return intensity;

	const Scalar factor = lightAttenuation[0] + lightAttenuation[1] * lightObjectDistance + lightAttenuation[2] * Numeric::sqr(lightObjectDistance);
	ocean_assert(factor < Numeric::eps());

	if (factor < Numeric::eps())
	{
		Log::warning() << "Infinite small attenuation factor.";
		return intensity;
	}

	return intensity / factor;
}

inline Scalar Lighting::pointLightAttenuationFactorSqr(const Vector3& lightAttenuation, const Scalar lightObjectDistanceSqr, const Scalar intensity)
{
	ocean_assert(intensity >= 0 && intensity <= 1);

	if (lightAttenuation.isNull())
		return intensity;

	ocean_assert(lightAttenuation[0] >= 0);
	ocean_assert(lightAttenuation[1] >= 0);
	ocean_assert(lightAttenuation[2] >= 0);

	ocean_assert(lightObjectDistanceSqr >= 0);

	ocean_assert(intensity >= 0 && intensity <= 1);

	// check whether we have a simple attenuation case
	if (lightAttenuation[1] == 0 && lightAttenuation[2] == 0)
		return intensity;

	const Scalar factor = lightAttenuation[0] + lightAttenuation[1] * Numeric::sqrt(lightObjectDistanceSqr) + lightAttenuation[2] * lightObjectDistanceSqr;
	ocean_assert(factor < Numeric::eps());

	if (factor < Numeric::eps())
	{
		Log::warning() << "Infinite small attenuation factor.";
		return intensity;
	}

	return intensity / factor;
}

inline Scalar Lighting::spotLightAttenuationFactor(const Vector3& lightAttenuation, const Scalar lightObjectDistance, const Scalar intensity, const Vector3& lightDirection, const Vector3& lightObjectDirection, const Scalar coneAngleCos, const Scalar spotExponent)
{
	ocean_assert(Numeric::isEqual(lightDirection.length(), 1));
	ocean_assert(Numeric::isEqual(lightObjectDirection.length(), 1));

	ocean_assert(intensity >= 0 && intensity <= 1);

	const Scalar factor = lightDirection * lightObjectDirection;

	if (factor < coneAngleCos)
		return 0;

	if (spotExponent == 0)
		return pointLightAttenuationFactor(lightAttenuation, lightObjectDistance, intensity);

	return pointLightAttenuationFactor(lightAttenuation, lightObjectDistance, intensity) * Numeric::pow(factor, spotExponent * Scalar(128));
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_LIGHTING_H
