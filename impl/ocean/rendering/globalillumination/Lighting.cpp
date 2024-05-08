/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/Lighting.h"
#include "ocean/rendering/globalillumination/GISpotLight.h"
#include "ocean/rendering/globalillumination/TracingGroup.h"

#include "ocean/rendering/PointLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

bool Lighting::dampedLight(const Vector3& viewPosition, const Vector3& viewObjectDirection, const Vector3& objectPosition, const Vector3& objectNormal, const Vector2& textureCoordinate, const GIMaterial* material, const GITextures* textures, const LightSources& lightSources, const TracingObject& object, const TracingGroup& root, const unsigned int bounces, const LightingModes lightingModes, RGBAColor& colorResult)
{
	ocean_assert(Numeric::isEqual(viewObjectDirection.length(), 1));
	ocean_assert(Numeric::isEqual(objectNormal.length(), 1));

	RGBAColor textureColor(1, 1, 1);
	if (textures && !textures->textureColor(textureCoordinate, textureColor))
	{
		textureColor = RGBAColor(0.7f, 0.7f, 0.7f);
	}

	if (!material)
	{
		colorResult = textureColor;
		return true;
	}

	RGBAColor diffuseColor(0, 0, 0);
	RGBAColor specularColor(0, 0, 0);

	if (lightingModes & LM_SHADING_LAMBERT)
	{
		for (LightSources::const_iterator i = lightSources.begin(); i != lightSources.end(); ++i)
		{
			ocean_assert(i->first);

			Scalar attenuationFactor = 0, lightObjectDistance = 0;
			Vector3 lightPosition, lightObjectDirection;

			switch (i->first->type())
			{
				case Object::TYPE_POINT_LIGHT:
				{
					const SmartObjectRef<GIPointLight> pointLight(i->first);
					ocean_assert(pointLight);

					lightPosition = i->second * pointLight->positionInline();
					lightObjectDirection = objectPosition - lightPosition;
					lightObjectDistance = lightObjectDirection.length();

					if (lightObjectDistance <= Numeric::eps())
					{
						continue;
					}

					lightObjectDirection /= lightObjectDistance;

					attenuationFactor = pointLightAttenuationFactorSqr(pointLight->attenuationInline(), lightObjectDistance, pointLight->intensityInline());
					break;
				}

				case Object::TYPE_SPOT_LIGHT:
				{
					const SmartObjectRef<GISpotLight> spotLight(i->first);
					ocean_assert(spotLight);

					lightPosition = i->second * spotLight->positionInline();
					lightObjectDirection = objectPosition - lightPosition;
					lightObjectDistance = lightObjectDirection.length();

					if (lightObjectDistance <= Numeric::eps())
					{
						continue;
					}

					lightObjectDirection /= lightObjectDistance;

					Vector3 lightDirection = i->second.rotationMatrix() * spotLight->directionInline();

					if (lightObjectDistance > Numeric::eps() && lightDirection.normalize())
					{
						attenuationFactor = spotLightAttenuationFactor(spotLight->attenuationInline(), lightObjectDistance, spotLight->intensityInline(), lightDirection, lightObjectDirection, spotLight->coneAngleCos(), spotLight->spotExponentInline());
					}

					break;
				}

				default:
					ocean_assert(false && "Missing implementation!");
					continue;
			}

			if (attenuationFactor < Numeric::eps())
			{
				continue;
			}

			ocean_assert(lightObjectDistance > 0);

			const RGBAColor dampingFactors(lightDampingFactors(lightPosition, lightObjectDirection, lightObjectDistance, i->first->diffuseColorInline(), root, lightingModes).damped(float(attenuationFactor)));
			if (dampingFactors.isBlack())
			{
				continue;
			}

			RGBAColor diffuse(0, 0, 0);
			RGBAColor specular(0, 0, 0);

			if (directLight(viewObjectDirection, objectPosition, objectNormal, lightObjectDirection, i->first->diffuseColorInline() * material->diffuseColorInline(), i->first->specularColorInline() * material->specularColorInline(), material->specularExponentInline(), lightingModes, diffuse, specular))
			{
				diffuseColor.combine(diffuse * dampingFactors);
				specularColor.combine(specular * dampingFactors);
			}
		}
	}

	diffuseColor.combine(material->ambientColorInline());
	diffuseColor *= textureColor;

	// the emissive color is independent of the texture
	diffuseColor.combine(material->emissiveColorInline());

	RGBAColor color(diffuseColor.combined(specularColor));

	if ((lightingModes & LM_REFLECTION_SPECULAR) && material->reflectivityInline() > 0 && bounces != 0)
	{
		ocean_assert(material->reflectivityInline() >= 0 && material->reflectivityInline() <= 1);

		RGBAColor reflectiveColor(0, 0, 0); // **TODO** should be framebufferView->backgroundColor();

		const Vector3 reflectiveDirection = (-viewObjectDirection).reflect(objectNormal);
		const Line3 reflectiveRay(objectPosition, reflectiveDirection);

		RayIntersection intersection;
		root.findNearestIntersection(reflectiveRay, intersection, true, Numeric::eps());//, object);

		if (intersection)
		{
			// determine the color for the most nearest intersection
			const TracingObject* tracingObject = intersection.tracingObject();
			tracingObject->determineColor(viewPosition, reflectiveDirection, intersection, root, bounces - 1u, nullptr, lightingModes, reflectiveColor);//, object);
		}

		color = color.damped(1.0f - material->reflectivityInline()).combined(reflectiveColor.damped(material->reflectivityInline()));
	}

	if ((lightingModes & LM_TRANSPARENCY) && material->transparencyInline() > 0 && bounces != 0)
	{
		RGBAColor transparencyColor(0.0f, 0.0f, 0.0f); // **TODO** should be framebufferView->backgroundColor();

		// we have no refraction
		if (material->refractionIndexInline() == 1.0f)
		{
			const Line3 transparentRay(objectPosition, viewObjectDirection);

			RayIntersection intersection;
			root.findNearestIntersection(transparentRay, intersection, true, Numeric::eps());//, object);

			if (intersection)
			{
				// determine the color for the most nearest intersection
				const TracingObject* tracingObject = intersection.tracingObject();
				tracingObject->determineColor(viewPosition, viewObjectDirection, intersection, root, bounces - 1u, nullptr, lightingModes, transparencyColor);//, object);
			}

			color = color.damped(1.0f - material->transparencyInline()).combined(transparencyColor.damped(material->transparencyInline()));
		}
		else // we have to determine the refraction
		{
			const Vector3 innerRefractionDirection = (-viewObjectDirection).refract(objectNormal, Scalar(1 / 1.05));//material.refractionIndex());

			RayIntersection innerIntersection;
			object.findNearestIntersection(Line3(objectPosition, innerRefractionDirection), innerIntersection, false, Numeric::eps());
			ocean_assert(innerIntersection);

			const Vector3& outerRefractionPosition = innerIntersection.position();
			const Vector3 outerRefractionNormal = -innerIntersection.normal();

			const Vector3 outerRefractionDirection = (-innerRefractionDirection).refract(outerRefractionNormal, Scalar(1.05 / 1));//material.refractionIndex(), 1);

			RayIntersection outerIntersection;
			root.findNearestIntersection(Line3(outerRefractionPosition, outerRefractionDirection), outerIntersection, true, Numeric::eps());//, object);

			if (outerIntersection)
			{
				// determine the color for the most nearest intersection
				const TracingObject* tracingObject = outerIntersection.tracingObject();
				tracingObject->determineColor(viewPosition, outerRefractionDirection, outerIntersection, root, bounces - 1u, nullptr, lightingModes, transparencyColor);//, object);
			}

			color = color.damped(1.0f - material->transparencyInline()).combined(transparencyColor.damped(material->transparencyInline()));
		}
	}

	colorResult = color;
	return true;
}

bool Lighting::directLight(const Vector3& viewObjectDirection, const Vector3& /*objectPosition*/, const Vector3& objectNormal, const Vector3& lightObjectDirection, const RGBAColor& materialLightDiffuse, const RGBAColor& materialLightSpecular, const Scalar materialSpecularExponent, const LightingModes lightingModes, RGBAColor& diffuse, RGBAColor& specular)
{
	ocean_assert(Numeric::isEqual(viewObjectDirection.length(), 1));
	ocean_assert(Numeric::isEqual(objectNormal.length(), 1));
	ocean_assert(Numeric::isEqual(lightObjectDirection.length(), 1));

	ocean_assert(lightingModes & LM_SHADING_LAMBERT);

	const Scalar lambertFactor = max(Scalar(0), -lightObjectDirection * objectNormal);

	if (lambertFactor == 0)
	{
		return false;
	}

	diffuse = materialLightDiffuse.damped(float(lambertFactor));

	if ((lightingModes & LM_SHADING_SPECULAR) == 0)
	{
		return true;
	}

	const Vector3 reflectedLightDirection((-lightObjectDirection).reflect(objectNormal));
	specular = materialLightSpecular.damped(float(Numeric::pow(max(Scalar(0), reflectedLightDirection * (-viewObjectDirection)), materialSpecularExponent)));

	return true;
}

RGBAColor Lighting::lightDampingFactors(const Vector3& lightPosition, const Vector3& lightObjectDirection, const Scalar lightObjectDistance, const RGBAColor& lightColor, const TracingGroup& root, const LightingModes lightingModes)
{
	if (lightingModes & LM_SHADOWS_TRANSLUCENT_LIGHT)
	{
		RGBAColor dampingColor(lightColor);

		if (!root.determineDampingColor(Line3(lightPosition, lightObjectDirection), dampingColor, lightObjectDistance - Numeric::weakEps()))
		{
			return RGBAColor(0, 0, 0);
		}

		return dampingColor;
	}

	if ((lightingModes & (LM_SHADOWS_SUPERSAMPLE | LM_SHADOWS)) && root.hasIntersection(Line3(lightPosition, lightObjectDirection), lightObjectDistance - Numeric::weakEps(), nullptr))
	{
		return RGBAColor(0, 0, 0);
	}

	return RGBAColor(1, 1, 1);
}

}

}

}
