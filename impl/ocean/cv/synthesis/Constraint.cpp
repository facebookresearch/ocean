// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/Constraint.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

Constraint::~Constraint()
{
	// nothing to do here
}

Scalar LineConstraint::cost(const Vector2& inside, const Vector2& outside) const
{
	const Scalar distanceInside = infiniteLineDistance(inside);
	const Scalar distanceOutside = infiniteLineDistance(outside);

	return Numeric::sqr(distanceInside - distanceOutside) * weight(distanceInside);
}

Scalar LineConstraint::weight(const Vector2& point) const
{
	return weight(infiniteLineDistance(point));
}

Constraint* LineConstraint::copy(const Scalar scale) const
{
	return new LineConstraint(*this, scale);
}

Scalar FiniteLineConstraint::cost(const Vector2& inside, const Vector2& outside) const
{
	const Scalar distanceInside = finiteLineDistance(inside);
	const Scalar distanceOutside = finiteLineDistance(outside);

	return Numeric::sqr(distanceInside - distanceOutside) * LineConstraint::weight(distanceInside);
}

Scalar FiniteLineConstraint::weight(const Vector2& point) const
{
	return LineConstraint::weight(finiteLineDistance(point));
}

Constraint* FiniteLineConstraint::copy(const Scalar scale) const
{
	return new FiniteLineConstraint(*this, scale);
}

Constraints::Constraints(const Constraints& constraints) :
	decision_(nullptr),
	width_(0u)
{
	constraintVector_.reserve(constraints.constraintVector_.size());

	for (ConstraintVector::const_iterator i = constraints.constraintVector_.begin(); i != constraints.constraintVector_.end(); ++i)
		constraintVector_.push_back((*i)->copy());
}

Constraints::Constraints(const Constraints& constraints, const Scalar scale) :
	decision_(nullptr),
	width_(0u)
{
	constraintVector_.reserve(constraints.constraintVector_.size());

	for (ConstraintVector::const_iterator i = constraints.constraintVector_.begin(); i != constraints.constraintVector_.end(); ++i)
		constraintVector_.push_back((*i)->copy(scale));
}

Constraints::~Constraints()
{
	for (ConstraintVector::iterator i = constraintVector_.begin(); i != constraintVector_.end(); ++i)
		delete *i;
}

void Constraints::initializeDecisions(const uint8_t* mask, const unsigned int width, const unsigned int height) const
{
	ocean_assert(mask);
	ocean_assert(constraintVector_.size() < 250);

	decisionFrame_.set(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);
	decisionFrame_.setValue(0xFFu);

	for (unsigned int y = 0u; y < decisionFrame_.height(); ++y)
	{
		uint8_t* decisionRow = decisionFrame_.row<uint8_t>(y);

		for (unsigned int x = 0u; x < decisionFrame_.width(); ++x)
		{
			if (*mask++ != 0xFF)
			{
				Scalar maxWeight = 0;
				uint8_t maxIndex = 0xFF;

				for (unsigned int n = 0; n < constraintVector_.size(); ++n)
				{
					const Constraint& constraint = *constraintVector_[n];
					const Scalar weight = constraint.weight(Vector2(Scalar(x), Scalar(y)));

					if (weight > maxWeight)
					{
						maxWeight = weight;
						maxIndex = uint8_t(n);
					}
				}

				*decisionRow = maxIndex;
			}

			++decisionRow;
		}
	}

	decision_ = decisionFrame_.data<uint8_t>();
	width_ = decisionFrame_.width();
}

}

}

}
