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

Bullseye::Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold) :
	position_(position),
	radius_(radius),
	grayThreshold_(grayThreshold)
{
	// Nothing else to do.
}

bool Bullseye::isValid() const
{
	return position_.x() >= 0 && position_.y() >= 0 && radius_ > 0 && grayThreshold_ != 0u && grayThreshold_ < 256u;
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

Vector2 Bullseye::invalidPosition()
{
	return Vector2(Numeric::minValue(), Numeric::minValue());
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
