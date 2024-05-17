/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/Constraint.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

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

std::unique_ptr<Constraint> LineConstraint::copy(const Scalar scale) const
{
	return std::make_unique<LineConstraint>(*this, scale);
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

std::unique_ptr<Constraint> FiniteLineConstraint::copy(const Scalar scale) const
{
	return std::make_unique<FiniteLineConstraint>(*this, scale);
}

Constraints::Constraints(const Constraints& constraints)
{
	constraints_.reserve(constraints.constraints_.size());

	for (const std::unique_ptr<Constraint>& constraint : constraints_)
	{
		ocean_assert(constraint);

		constraints_.emplace_back(constraint->copy());
	}
}

Constraints::Constraints(const Constraints& constraints, const Scalar scale)
{
	ocean_assert(scale > Numeric::eps());

	constraints_.reserve(constraints.constraints_.size());

	for (const std::unique_ptr<Constraint>& constraint : constraints_)
	{
		ocean_assert(constraint);

		constraints_.emplace_back(constraint->copy(scale));
	}
}

void Constraints::initializeDecisions(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements) const
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(constraints_.size() < 250);

	if (!decisionFrame_.set(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	decisionFrame_.setValue(0xFFu);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	for (unsigned int y = 0u; y < decisionFrame_.height(); ++y)
	{
		uint8_t* decisionRow = decisionFrame_.row<uint8_t>(y);
		const uint8_t* maskRow = mask + y * maskStrideElements;

		for (unsigned int x = 0u; x < decisionFrame_.width(); ++x)
		{
			if (*maskRow++ != 0xFF)
			{
				Scalar maxWeight = 0;
				uint8_t maxIndex = 0xFF;

				for (unsigned int n = 0; n < constraints_.size(); ++n)
				{
					const Constraint& constraint = *constraints_[n];
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
}

}

}

}
