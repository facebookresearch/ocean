/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/TracingSphere.h"
#include "ocean/rendering/globalillumination/Lighting.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

TracingSphere::TracingSphere()
{
	// nothing to do here
}

TracingSphere::~TracingSphere()
{
	// nothing to do here
}

void TracingSphere::setSphere(const HomogenousMatrix4& objectTransformation, const BoundingSphere& localBoundingSphere)
{
	setObjectTransformation(objectTransformation);

	tracingLocalBoundingSphere = localBoundingSphere;
}

void TracingSphere::findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject) const
{
	ocean_assert(ray.isValid());

	if (this == excludedObject)
		return;

	Vector3 intersectionPoint;
	Normal normal;
	Scalar intersectionDistance;

	if (((frontFace && tracingLocalBoundingSphere.positiveFrontIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance, normal))
		|| (!frontFace && tracingLocalBoundingSphere.positiveBackIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance, normal)))
		&& intersectionDistance > eps && intersectionDistance < intersection.distance())
	{
		ocean_assert(Numeric::isEqual(normal.length(), 1));
		ocean_assert(frontFace && normal * ray.direction() < 0 || !frontFace && normal * ray.direction() > 0);

		intersection = RayIntersection(intersectionPoint, ray.direction(), normal, TextureCoordinate(0, 0), intersectionDistance, this, lightSources_);
	}
}

bool TracingSphere::hasIntersection(const Line3& ray, const Scalar maximalDistance, const TracingObject* excludedObject) const
{
	ocean_assert(ray.isValid());

	if (this == excludedObject)
	{
		return false;
	}

	Vector3 intersectionPoint;
	Scalar intersectionDistance;

	return tracingLocalBoundingSphere.positiveFrontIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance) && intersectionDistance < maximalDistance;
}

bool TracingSphere::determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance) const
{
	ocean_assert(ray.isValid());

	Vector3 intersectionPoint;
	Scalar intersectionDistance;

	if (!tracingLocalBoundingSphere.positiveFrontIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance) || intersectionDistance >= maximalDistance)
	{
		return true;
	}

	if (!material_)
	{
		return false;
	}

	if (material_->transparencyInline() == 0.0f)
	{
		// the material is fully opaque
		return false;
	}

	const RGBAColor transmittedColor = color.damped(material_->transparencyInline());
	const RGBAColor opaqueColor = color * material_->diffuseColorInline().combined(material_->ambientColorInline()).damped(1.0f - material_->transparencyInline());

	color = transmittedColor.combined(opaqueColor).damped(material_->transparencyInline());

	return true;
}

bool TracingSphere::determineColor(const Vector3& viewPosition, const Vector3& viewObjectDirection, const RayIntersection& intersection, const TracingGroup& group, const unsigned int bounces, const TracingObject* /*excludedObject*/, const Lighting::LightingModes lightingModes, RGBAColor& color) const
{
	TextureCoordinate textureCoordinate(0, 0);

	if (textures_ && tracingLocalBoundingSphere.radius() > Numeric::eps())
	{
		const Normal transformedNormal((objectTransformation_.rotationMatrix().transposed() * intersection.normal()).normalizedOrZero());

		const Scalar latitude = Numeric::asin(transformedNormal.y());
		ocean_assert(Numeric::isInsideRange(-Numeric::pi_2(), latitude, Numeric::pi_2()));

		const Scalar longitude = -Numeric::atan2(transformedNormal.x(), -transformedNormal.z());
		ocean_assert(Numeric::isInsideRange(-Numeric::pi(), longitude, Numeric::pi()));

		textureCoordinate = TextureCoordinate((longitude + Numeric::pi()) / Numeric::pi2(), (latitude + Numeric::pi_2()) / Numeric::pi());
	}

	return Lighting::dampedLight(viewPosition, viewObjectDirection, intersection.position(), intersection.normal(), textureCoordinate, material_, textures_, intersection.lightSources(), *this, group, bounces, lightingModes, color);
}

}

}

}
