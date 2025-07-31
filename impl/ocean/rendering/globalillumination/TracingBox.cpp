/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/TracingBox.h"
#include "ocean/rendering/globalillumination/Lighting.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

TracingBox::TracingBox()
{
	// nothing to do here
}

TracingBox::~TracingBox()
{
	// nothing to do here
}

void TracingBox::setBox(const HomogenousMatrix4& objectTransformation, const BoundingBox& localBoundingBox)
{
	setObjectTransformation(objectTransformation);

	tracingLocalBoundingBox = localBoundingBox;
	tracingLocalBoundingSphere = BoundingSphere(localBoundingBox);
}

void TracingBox::findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject) const
{
	ocean_assert(ray.isValid());

	if (this == excludedObject)
		return;

	if (!tracingLocalBoundingBox.hasIntersection(ray, invertedObjectTransformation_))
		return;

	Vector3 intersectionPoint;
	Normal normal;
	Scalar intersectionDistance;
	TextureCoordinate textureCoordinate;

	if (((frontFace && tracingLocalBoundingBox.positiveFrontIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance, normal, textureCoordinate))
		|| (!frontFace && tracingLocalBoundingBox.positiveBackIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance, normal)))
		&& intersectionDistance > eps && intersectionDistance < intersection.distance())
	{
		ocean_assert(Numeric::isEqual(normal.length(), 1));
		ocean_assert(frontFace && normal * ray.direction() < 0 || !frontFace && normal * ray.direction() > 0);

		if (attributes_)
			intersection = RayIntersection(intersectionPoint, ray.direction(), normal, textureCoordinate, intersectionDistance, this, lightSources_);
		else
			intersection = RayIntersection(intersectionPoint, ray.direction(), normal, textureCoordinate, intersectionDistance, this, lightSources_);
	}
}

bool TracingBox::hasIntersection(const Line3& ray, const Scalar maximalDistance, const TracingObject* excludedObject) const
{
	ocean_assert(ray.isValid());

	if (this == excludedObject)
		return false;

	Vector3 intersectionPoint;
	Scalar intersectionDistance;

	return tracingLocalBoundingBox.positiveFrontIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance) && intersectionDistance < maximalDistance;
}

bool TracingBox::determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance) const
{
	ocean_assert(ray.isValid());

	Vector3 intersectionPoint;
	Scalar intersectionDistance;

	if (!tracingLocalBoundingBox.positiveFrontIntersection(ray, objectTransformation_, invertedObjectTransformation_, intersectionPoint, intersectionDistance) || intersectionDistance >= maximalDistance)
	{
		return true;
	}

	if (!material_)
	{
		return false;
	}

	if (material_->transparencyInline() == 0.0f)
	{
		return false;
	}

	const RGBAColor transmittedColor = color.damped(material_->transparencyInline());
	const RGBAColor opaqueColor = color * material_->diffuseColorInline().combined(material_->ambientColorInline()).damped(1.0f - material_->transparencyInline());

	color = transmittedColor.combined(opaqueColor).damped(material_->transparencyInline());

	return true;
}

bool TracingBox::determineColor(const Vector3& viewPosition, const Vector3& viewObjectDirection, const RayIntersection& intersection, const TracingGroup& group, const unsigned int bounces, const TracingObject* excludedObject, const Lighting::LightingModes lightingModes, RGBAColor& color) const
{
	ocean_assert_and_suppress_unused(this != excludedObject, excludedObject);

	return Lighting::dampedLight(viewPosition, viewObjectDirection, intersection.position(), intersection.normal(), intersection.textureCoordinate(), material_, textures_, intersection.lightSources(), *this, group, bounces, lightingModes, color);
}

}

}

}
