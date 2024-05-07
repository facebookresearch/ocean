// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/uvtexturemapping/CylinderUVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

CylinderUVTextureMapping::CylinderUVTextureMapping(const Scalar patternWidth, const Scalar patternHeight, const Scalar metricHeight, const Scalar patternXAxisOffset, const Vector2& patternYAxisIntersection, const bool originOnCylinderSurface) :
	patternWidth_(patternWidth),
	patternHeight_(patternHeight),
	height_(metricHeight),
	radius_(Numeric::ratio(metricHeight, patternHeight, 0 /*fallback*/) * patternWidth / Numeric::pi2()),
	patternXAxisOffset_(patternXAxisOffset),
	patternYAxisIntersection_(patternYAxisIntersection)
{
	// Initial check that the provided parameters are valid.
	ocean_assert(patternWidth_ > Numeric::weakEps());
	ocean_assert(patternHeight_ > Numeric::weakEps());
	ocean_assert(metricHeight > Scalar(0));

	if (patternWidth_ <= Numeric::weakEps() || patternHeight_ <= Numeric::weakEps() || metricHeight <= Numeric::weakEps())
	{
		height_ = Scalar(0); // denote an invalid cylinder
		return;
	}

	// Use the user-provided y-axis intersection, if provided; otherwise, use
	// values that place the intersection at the center of the pattern.
	if (patternYAxisIntersection_.x() >= Scalar(0.) && patternYAxisIntersection_.y() >= Scalar(0))
	{
		// At the +y axis intersection, theta=pi/2.
		thetaMin_ = (patternYAxisIntersection_.x() - patternXAxisOffset_) / patternWidth_ * Numeric::pi2() - Scalar(3) * Numeric::pi_2();

		if (originOnCylinderSurface)
		{
			yOffset_ = -radius_;
		}
		zOffset_ = (Scalar(0.5) - patternYAxisIntersection_.y() / patternHeight_) * height_;
	}
}

bool CylinderUVTextureMapping::textureCoordinateTo3DCoordinate(const Vector2& point2D, Vector3& point3D) const
{
	ocean_assert(isValid());

	// Due to wrap-around, allow any arbitrary x-coordinate, but ignore y coordinates above and below.
	if (!isValid() || point2D.y() < Scalar(0) || point2D.y() > patternHeight_)
	{
		return false;
	}

	ocean_assert(Numeric::isNotEqualEps(patternWidth_));
	ocean_assert(Numeric::isNotEqualEps(patternHeight_));

	const Scalar theta = (Scalar(1) - (point2D.x() - patternXAxisOffset_) / patternWidth_) * Numeric::pi2() + thetaMin_;

	point3D = Vector3(
		Numeric::cos(theta) * radius_,
		Numeric::sin(theta) * radius_ + yOffset_,
		(point2D.y() / patternHeight_ - Scalar(0.5)) * height_ + zOffset_);

	return true;
}

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean
