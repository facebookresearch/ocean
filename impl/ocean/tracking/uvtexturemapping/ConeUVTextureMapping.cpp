// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/uvtexturemapping/ConeUVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

ConeUVTextureMapping::ConeUVTextureMapping() :
	height_(Scalar(0.)),
	yAxisIntersection_(Scalar(-1.), Scalar(-1.))
{
	// nothing to do here
}

ConeUVTextureMapping::ConeUVTextureMapping(const Scalar height, const Scalar largerDiameter, const Scalar smallerDiameter, const Vector2& largerArcEnd, const Vector2& smallerArcEnd, const Vector2& yAxisIntersection, const bool originOnConeSurface) :
	height_(height),
	smallerRadius_(Scalar(0.5) * smallerDiameter),
	radiusDifference_(Scalar(0.5) * (largerDiameter - smallerDiameter)),
	yAxisIntersection_(yAxisIntersection),
	apex2d_(smallerArcEnd),
	lengthOF_(Scalar(0.)),
	invLengthFC_(Scalar(0.)),
	rotationFrameOC_(Scalar(0.), Scalar(0.), Scalar(0.), Scalar(0.)),
	invAngleAOC_(Scalar(0.)),
	thetaMin_(-Numeric::pi_2()),
	yOffset_(Scalar(0.)),
	zOffset_(Scalar(0.))
{
	// Here's the mapping, again, for reference:
	//
	//  ---------------------                ^     .--AC--.      <- larger curve
	//  |                   |                |    /        \
	//  |    _.---B---._    |        segment |   |          |           .__X
	//  |  /             \  |         height |   '.        .'          /|
	//  | A     cone      C |                |     \__B___/           Y Z
	//  |  \   surface   /  |  <=>           |
	//  |   \           /   |                v       .DF.        <- smaller curve,
	//  |    \   .E.   /    |                        \E_/           diameter >= 0
	//  |     D./   \.F     |
	//  |                   |                         .          <- tip of cone
	//  ---------------------
	//            O  <- apex of the flattened cone (may be outside of the image)
	//
	// Note that AC and DF in the left image are arcs of concentric circles with
	// center O.

	// The unwrapped cone consists of two circle arc segments with the same center (which is the 2D projection of the apex).
	const Vector2& pointC = largerArcEnd;
	const Vector2& pointF = smallerArcEnd;

	// Initial check that the provided parameters are valid.
	ocean_assert(height_ > Numeric::weakEps());
	ocean_assert(radiusDifference_ > Numeric::weakEps() && smallerRadius_ >= Scalar(0.));
	ocean_assert(!pointC.isEqual(pointF, Numeric::weakEps()));
	if (height_ <= Numeric::weakEps() || radiusDifference_ <= Numeric::weakEps() || smallerRadius_ < Scalar(0.) || pointC.isEqual(pointF, Numeric::weakEps()))
	{
		height_ = Scalar(0.); // denote an invalid cone
		return;
	}

	const Vector2 segmentFC = pointC - pointF;
	const Scalar lengthFC = segmentFC.length();
	invLengthFC_ = Scalar(1.) / lengthFC;

	const Vector2 directionOC = segmentFC * invLengthFC_;
	Scalar lengthOC = lengthFC;

	// If the cone is truncated before its tip, compute its projected apex point
	// by extending line FC; otherwise, the projected apex has already been set as
	// point F.
	if (smallerRadius_ > Scalar(0.))
	{
		// In 3D, line FC is the hypotenuse of a right triangle with base
		// radiusDifference_ and height height_. We can compute the 3D distance OC
		// via similar triangles and, from this, the associated distance in 2D.
		lengthOC = lengthFC * largerRadius() / radiusDifference_;
		apex2d_ = pointC - directionOC * lengthOC;
		lengthOF_ = lengthOC - lengthFC;
	}

	// We perform 2D-to-3D mappings in the coordinate frame where line OC lies
	// along the x-axis.
	rotationFrameOC_ = SquareMatrix2(directionOC.x(), -directionOC.y(), directionOC.y(), directionOC.x());

	// Compute 2D angle AOC: This is the angle of an arc around a circle with
	// radius OC. Since we know the conversion ratio of 3D to 2D distances (via
	// FC) and the circumference of the cone base in 3D, we can compute the arc
	// length in 2D and thus derive angle AOC = 2*pi * (2*pi*S*R) / (2*pi*|OC|),
	// where S is the scaling factor to convert distances in 3D to distance in 2D,
	// and R is the larger radius of the cone base in 3D.
	const Scalar scale2DFrom3D = lengthFC / Numeric::pythagoras(radiusDifference_, height_);
	invAngleAOC_ = lengthOC / (Numeric::pi2() * scale2DFrom3D * largerRadius());

	// Use the user-provided y-axis intersection, if provided; otherwise, use
	// values that place the intersection at the center of the pattern.
	if (yAxisIntersection_.x() >= Scalar(0.) && yAxisIntersection_.y() >= Scalar(0.))
	{
		// Consider the 3D coordinate frame placed in the center of the cone (see
		// the figure above). Let X be a given 2D point within the unwrapped cone
		// surface. Since arc AC all has z=-H/2 and arc DF has z=H/2, the 3D
		// z-coordinate for X is simply linearly scaled between these two values.
		// The radius of the cone at this z value can also be computed via linear
		// scaling between the two arcs, and the rotation in the 3D xy plane can be
		// computed by the angular distance between CF and AD.

		Vector2 directionOX = yAxisIntersection_ - apex2d_;
		const Scalar lengthOX = directionOX.length();
		if (lengthOX > Numeric::weakEps())
		{
			directionOX /= lengthOX;

			const Vector2 directionOXinFrameOC = rotationFrameOC_ * directionOX;
			Scalar angleXOC = Numeric::acos(directionOXinFrameOC.x());
			if (directionOXinFrameOC.y() > Scalar(0.)) // > because +y points down
			{
				angleXOC = Numeric::pi2() - angleXOC;
			}

			// At the +y axis intersection, theta=pi/2.
			thetaMin_ = Numeric::pi_2() - angleXOC * invAngleAOC_ * Numeric::pi2();

			// Linear scaling term "alpha" is zero for points on the DF (smaller)
			// circle and one for points on the AC (larger) circle; this is based on
			// distance from the circle's center.
			const Scalar alpha = (lengthOX - lengthOF_) * invLengthFC_;
			if (originOnConeSurface)
			{
				yOffset_ = -alpha * radiusDifference_ - smallerRadius_;
			}
			zOffset_ = (alpha - Scalar(0.5)) * height_;
		}
		else
		{
			// The y-axis is at the apex -- don't adjust thetaMin_, but do shift in z.
			zOffset_ = Scalar(-0.5) * height_;
		}
	}
}

bool ConeUVTextureMapping::textureCoordinateTo3DCoordinate(const Vector2& point2D, Vector3& point3D) const
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return false;
	}

	// First, compute linear scaling term "alpha", which is zero for points on
	// the DF (smaller) circle and one for points on the AC (larger) circle;
	// this is based on distance from the circle's center.
	Vector2 directionOX = point2D - apex2d_;
	const Scalar lengthOX = directionOX.length();
	const Scalar alpha = (lengthOX - lengthOF_) * invLengthFC_;

	// Disallow alpha == 0 to avoid possible division by lengthOX = 0, below.
	if (alpha < Scalar(0.) || Numeric::isWeakEqualEps(alpha) || alpha > Scalar(1.))
	{
		return false;
	}

	directionOX /= lengthOX;

	// Next, compute the 2D angle for this point relative to the pattern's DF line.
	const Vector2 directionOXinFrameOC = rotationFrameOC_ * directionOX;
	Scalar angleXOC = Numeric::acos(directionOXinFrameOC.x());
	if (directionOXinFrameOC.y() > Scalar(0.)) // > because +y points down
	{
		angleXOC = Numeric::pi2() - angleXOC;
	}

	// Convert the 2D polar coordinates in the images into their corresponding
	// coordinates in the 3D cone frame.
	const Scalar theta = angleXOC * invAngleAOC_ * Numeric::pi2() + thetaMin_;
	const Scalar radius = alpha * radiusDifference_ + smallerRadius_; // radius at Z=X.z()

	point3D = Vector3(
		Numeric::cos(theta) * radius,
		Numeric::sin(theta) * radius + yOffset_,
		(Scalar(0.5) - alpha) * height_ + zOffset_);

	return true;
}

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean
