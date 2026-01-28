/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/Bullseye.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

Bullseye::Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold, const unsigned int pyramidLayer) :
	position_(position),
	radius_(radius),
	grayThreshold_(grayThreshold),
	pyramidLayer_(pyramidLayer)
{
	// Nothing else to do.
}

Bullseye::Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold, Diameters&& diameters, const unsigned int pyramidLayer) :
	position_(position),
	radius_(radius),
	grayThreshold_(grayThreshold),
	diameters_(std::move(diameters)),
	pyramidLayer_(pyramidLayer)
{
	// Nothing else to do.
}

bool Bullseye::isValid() const
{
	return position_ != invalidPosition() && radius_ != invalidRadius() && radius_ > 0 && grayThreshold_ != invalidGrayThreshold() && grayThreshold_ != 0u && grayThreshold_ < 256u;
}

const Vector2& Bullseye::position() const
{
	return position_;
}

Scalar Bullseye::radius() const
{
	return radius_;
}

unsigned int Bullseye::grayThreshold() const
{
	return grayThreshold_;
}

unsigned int Bullseye::pyramidLayer() const
{
	return pyramidLayer_;
}

bool Bullseye::hasDiameters() const
{
	return !diameters_.empty();
}

const Diameters& Bullseye::diameters() const
{
	return diameters_;
}

Bullseye Bullseye::scaled(const Scalar scaleFactor) const
{
	ocean_assert(scaleFactor > Scalar(0));

	// Scale diameter data
	Diameters scaledDiameters;
	scaledDiameters.reserve(diameters_.size());

	for (const Diameter& diameter : diameters_)
	{
		Diameter scaledDiameter;
		scaledDiameter.isSymmetryValid = diameter.isSymmetryValid;

		// Scale positive half-ray
		for (size_t i = 0; i < diameter.halfRayPositive.transitionPoints.size(); ++i)
		{
			const Vector2& point = diameter.halfRayPositive.transitionPoints[i];
			scaledDiameter.halfRayPositive.transitionPoints[i] = (point != HalfRay::invalidTransitionPoint()) ? point * scaleFactor : HalfRay::invalidTransitionPoint();
		}
		for (size_t i = 0; i < 3; ++i)
		{
			scaledDiameter.halfRayPositive.intensityCheckPoints[i] = diameter.halfRayPositive.intensityCheckPoints[i] * scaleFactor;
			scaledDiameter.halfRayPositive.isIntensityValid[i] = diameter.halfRayPositive.isIntensityValid[i];
		}
		scaledDiameter.halfRayPositive.angle = diameter.halfRayPositive.angle;

		// Scale negative half-ray
		for (size_t i = 0; i < diameter.halfRayNegative.transitionPoints.size(); ++i)
		{
			const Vector2& point = diameter.halfRayNegative.transitionPoints[i];
			scaledDiameter.halfRayNegative.transitionPoints[i] = (point != HalfRay::invalidTransitionPoint()) ? point * scaleFactor : HalfRay::invalidTransitionPoint();
		}
		for (size_t i = 0; i < 3; ++i)
		{
			scaledDiameter.halfRayNegative.intensityCheckPoints[i] = diameter.halfRayNegative.intensityCheckPoints[i] * scaleFactor;
			scaledDiameter.halfRayNegative.isIntensityValid[i] = diameter.halfRayNegative.isIntensityValid[i];
		}
		scaledDiameter.halfRayNegative.angle = diameter.halfRayNegative.angle;

		scaledDiameters.emplace_back(std::move(scaledDiameter));
	}

	return Bullseye(position_ * scaleFactor, radius_ * scaleFactor, grayThreshold_, std::move(scaledDiameters), pyramidLayer_);
}

Vector2 Bullseye::invalidPosition()
{
	return Vector2(Numeric::minValue(), Numeric::minValue());
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
