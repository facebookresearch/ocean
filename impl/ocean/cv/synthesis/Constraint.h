/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CONSTRAINT_H
#define META_OCEAN_CV_SYNTHESIS_CONSTRAINT_H

#include "ocean/cv/synthesis/Synthesis.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all constraints.
 * Constraints can be used to improve the image quality of a synthesized image.<br>
 * Constraints cover e.g., geometrical structures, patterns or other kinds of visual information not entirely converted by a patch-based synthesis.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT Constraint
{
	friend class Constraints;

	public:

		/**
		 * Destructs this constraint.
		 */
		virtual ~Constraint() = default;

		/**
		 * Creates an empty constraint.
		 * @param impact The impact factor, with range [0, infinity)
		 */
		inline explicit Constraint(const Scalar impact);

		/**
		 * Returns the impact factor of this constraint.
		 * @return Constraint impact factor
		 */
		inline Scalar impact() const;

		/**
		 * Returns the cost for a given for two given points.
		 * @param inside The inside point (point inside the mask to determine the cost for - according to the second point)
		 * @param outside The outside point (point outside the mask)
		 * @return Constraint cost
		 */
		virtual Scalar cost(const Vector2& inside, const Vector2& outside) const = 0;

		/**
		 * Returns the weight of this constraint according to a given point.
		 * @param point The point to determine the weight for
		 * @return Resulting weight
		 */
		virtual Scalar weight(const Vector2& point) const = 0;

	protected:

		/**
		 * Creates an empty constraint.
		 */
		Constraint() = default;

		/**
		 * Creates a copy of this constraint by an optional scale parameter.
		 * A scale of 0.5 provide a constraint for an image with bisected dimensions.<br>
		 * The resulting object has to be released by the caller.<br>
		 * @param scale The scale to be used
		 */
		virtual std::unique_ptr<Constraint> copy(const Scalar scale = Scalar(1)) const = 0;

	protected:

		/// Impact factor of this constraint.
		Scalar impact_ = Scalar(0);
};

/**
 * This class implements a structure constraint.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT StructureConstraint : public Constraint
{
	public:

		/**
		 * Creates a new structure constraint object.
		 * @param impact The impact factor, with range [0, infinity)
		 * @param radius Impact radius of this constraint, with range (0, infinity)
		 */
		inline explicit StructureConstraint(const Scalar impact, const Scalar radius);

		/**
		 * Returns the radius of this constraint.
		 * @return Impact radius
		 */
		inline Scalar radius() const;

		/**
		 * Returns the weight of this constraint according to a given point.
		 * @param point The point to determine the weight for
		 * @return Resulting weight
		 */
		virtual Scalar weight(const Vector2& point) const = 0;

	protected:

		/// Impact radius of this structure constraint.
		Scalar radius_ = Scalar(0);

		/// Radius parameter of this constraint: 1 / (1/4 * radius)
		Scalar radiusParameter_ = Scalar(0);
};

/**
 * This class implement a line constraint.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT LineConstraint : public StructureConstraint
{
	public:

		/**
		 * Creates a new line constraint object for infinite lines.
		 * @param point0 First point of the infinite line
		 * @param point1 Second point of the infinite line
		 * @param impact The impact factor, with range [0, infinity)
		 * @param radius Impact radius of this constraint, with range (0, infinity)
		 */
		inline LineConstraint(const Vector2& point0, const Vector2& point1, const Scalar impact, const Scalar radius);

		/**
		 * Creates a new finite line constraint object by an already defined one.
		 * @param constraint Finite line constraint
		 * @param scale The scale factor to be applied
		 */
		inline LineConstraint(const LineConstraint& constraint, const Scalar scale);

		/**
		 * Returns the first point of this line.
		 * @return First point
		 */
		inline const Vector2& point0() const;

		/**
		 * Returns the second point of this line.
		 * @return First point
		 */
		inline const Vector2& point1() const;

		/**
		 * Returns the weight of this constraint according to a given point.
		 * @see StructureConstraint::weight().
		 */
		Scalar weight(const Vector2& point) const override;

		/**
		 * Returns the cost for a given for two given points.
		 * @see Constraint::cost().
		 */
		Scalar cost(const Vector2& inside, const Vector2& outside) const override;

		/**
		 * Returns the distance between a given point and this infinite line.
		 * @param point The point to determine the distance for
		 * @return Resulting distance
		 */
		inline Scalar infiniteLineDistance(const Vector2& point) const;

		/**
		 * Returns the weight of this constraint according to a given distance.
		 * @param distance Already determined distance of an arbitrary point
		 * @return Determined weight
		 */
		inline Scalar weight(const Scalar distance) const;

	protected:

		/**
		 * Creates a copy of this constraint by an optional scale parameter.
		 * @see Constraint::copy().
		 */
		std::unique_ptr<Constraint> copy(const Scalar scale = 1) const override;

	protected:

		/// First point of the line.
		Vector2 point0_;

		/// Second point of the line.
		Vector2 point1_;

		/// Normal (x, y) and distance (z) of the line.
		Vector3 parameter_;
};

/**
 * This class implements a finite line constraint.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT FiniteLineConstraint : public LineConstraint
{
	public:

		/**
		 * Creates a new finite line constraint object.
		 * @param point0 First point of the infinite line
		 * @param point1 Second point of the infinite line
		 * @param impact The impact factor, with range [0, infinity)
		 * @param radius Impact radius of this constraint, with range (0, infinity)
		 * @param penalty The penalty value if the point is outside the finite line
		 * @param finite0 State to finite the line at the first point
		 * @param finite1 State to finite the line at the second point
		 */
		inline FiniteLineConstraint(const Vector2& point0, const Vector2& point1, const Scalar impact, const Scalar radius, const Scalar penalty, const bool finite0, const bool finite1);

		/**
		 * Creates a new finite line constraint object by an already defined one.
		 * @param constraint Finite line constraint
		 * @param scale The scale factor to be applied
		 */
		inline FiniteLineConstraint(const FiniteLineConstraint& constraint, const Scalar scale);

		/**
		 * Returns the penalty factor of this line constraint.
		 * @return Penalty factor
		 */
		inline Scalar penalty() const;

		/**
		 * Returns the finite state of the first point.
		 * @return True, if the line is finite at the first point
		 */
		inline bool finite0() const;

		/**
		 * Returns the finite state of the second point.
		 * @return True, if the line is finite at the second point
		 */
		inline bool finite1() const;

		/**
		 * Returns the weight of this constraint according to a given point.
		 * @see StructureConstraint::weight().
		 */
		Scalar weight(const Vector2& point) const override;

		/**
		 * Returns the cost for a given for two given points.
		 * @see Constraint::cost().
		 */
		Scalar cost(const Vector2& inside, const Vector2& outside) const override;

		/**
		 * Returns the distance between a given point and this infinite line.
		 * @param point The point to determine the distance for
		 * @return Resulting distance
		 */
		inline Scalar finiteLineDistance(const Vector2& point) const;

		/**
		 * Creates a copy of this constraint by an optional scale parameter.
		 * @see Constraint::copy().
		 */
		std::unique_ptr<Constraint> copy(const Scalar scale = 1) const override;

	protected:

		/// Offset vector between the first and the second point.
		Vector2 offset_;

		/// Square length of the offset vector.
		Scalar offsetSquare_ = Scalar(-1);

		/// Penalty value.
		Scalar penalty_ = Scalar(-1);

		/// State to finite the line at the first point.
		bool finite0_ = false;

		/// State to finite the line at the second point.
		bool finite1_ = false;
};

/**
 * This class implements a container holding constraints.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT Constraints
{
	public:

		/**
		 * Definition of a vector holding constraints.
		 */
		typedef std::vector<std::unique_ptr<Constraint>> ConstraintsVector;

	public:

		/**
		 * Creates a new constraint container.
		 */
		Constraints() = default;

		/**
		 * Copy constructor.
		 * @param constraints The constraints to be copied
		 */
		Constraints(const Constraints& constraints);

		/**
		 * Default move constructor.
		 * @param constraints The constraints to be moved
		 */
		Constraints(Constraints&& constraints) = default;

		/**
		 * Copies a constraint container and applies an explicit scale factor.
		 * @param constraints The constraints to be copied
		 * @param scale The scale factor to be applied for each constraint to be copied, with range (0, infinity)
		 */
		Constraints(const Constraints& constraints, const Scalar scale);

		/**
		 * Destructs the constraint container.
		 */
		~Constraints() = default;

		/**
		 * Adds a new constraint.
		 * The given constraint will be released by the container.
		 * @param constraint The constraint to be added
		 */
		inline void addConstraint(std::unique_ptr<Constraint> constraint);

		/**
		 * Returns the number of constraints.
		 * The number of constraints
		 */
		inline size_t size() const;

		/**
		 * Initializes the constraint decisions.
		 * @param mask The mask to create the decisions for, must be valid
		 * @param width The width of the mask in pixel
		 * @param height The height of the mask in pixel
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 */
		void initializeDecisions(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements) const;

		/**
		 * Returns the cost for one inside point and one outside point.
		 * @param insideX Horizontal inside position
		 * @param insideY Vertical inside position
		 * @param outside The outside point
		 * @return Resulting cost
		 */
		inline Scalar cost(const unsigned int insideX, const unsigned int insideY, const Vector2& outside) const;

		/**
		 * Returns the cost for one inside point and one outside point.
		 * @param insideX Horizontal inside position
		 * @param insideY Vertical inside position
		 * @param outsideX Horizontal outside point
		 * @param outsideY Vertical outside point
		 * @return Resulting cost
		 */
		inline Scalar cost(const unsigned int insideX, const unsigned int insideY, const unsigned int outsideX, const unsigned int outsideY) const;

		/**
		 * Returns whether this container does not hold any constraint.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this container holds at least one constraint.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Default move operator.
		 * @param constraints The constraints to be moved
		 * @return Reference to this object
		 */
		Constraints& operator=(Constraints&& constraints) = default;

	private:

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		Constraints& operator=(const Constraints& constraints) = delete;

	private:

		/// Vector holding the internal constraints.
		ConstraintsVector constraints_;

		/// Decision frame.
		mutable Frame decisionFrame_;
};

inline Constraint::Constraint(const Scalar impact) :
	impact_(impact)
{
	ocean_assert(impact_ >= 0);
}

inline Scalar Constraint::impact() const
{
	return impact_;
}

inline StructureConstraint::StructureConstraint(const Scalar impact, const Scalar radius) :
	Constraint(impact),
	radius_(radius),
	radiusParameter_(1 / (radius * Scalar(0.25)))
{
	// nothing to do here
}

inline Scalar StructureConstraint::radius() const
{
	return radius_;
}

inline LineConstraint::LineConstraint(const Vector2& point0, const Vector2& point1, const Scalar impact, const Scalar radius) :
	StructureConstraint(impact, radius),
	point0_(point0),
	point1_(point1)
{
	ocean_assert(point0 != point1);
	const Vector2 direction(point1 - point0);

	const Scalar length = direction.length();
	ocean_assert(Numeric::isNotEqualEps(length));

	const Vector2 normal = direction.perpendicular() / length;
	ocean_assert(Numeric::isEqual(normal.length(), 1));

	const Scalar distance = normal * point0_;

	parameter_ = Vector3(normal, -distance);
}

inline LineConstraint::LineConstraint(const LineConstraint& constraint, const Scalar scale) :
	StructureConstraint(constraint.impact(), constraint.radius() * scale),
	point0_(constraint.point0() * scale),
	point1_(constraint.point1() * scale)
{
	ocean_assert(point0_ != point1_);
	const Vector2 direction(point1_ - point0_);

	const Scalar length = direction.length();
	ocean_assert(Numeric::isNotEqualEps(length));

	const Vector2 normal = direction.perpendicular() / length;
	ocean_assert(Numeric::isEqual(normal.length(), 1));

	const Scalar distance = normal * point0_;

	parameter_ = Vector3(normal, -distance);
}

inline const Vector2& LineConstraint::point0() const
{
	return point0_;
}

inline const Vector2& LineConstraint::point1() const
{
	return point1_;
}

inline Scalar LineConstraint::infiniteLineDistance(const Vector2& point) const
{
	// lineParameter * Vector3(point, 1);
	return parameter_.x() * point.x() + parameter_.y() * point.y() + parameter_.z();
}

inline Scalar LineConstraint::weight(const Scalar distance) const
{
	if (Numeric::abs(distance) > radius_)
	{
		return 0;
	}

	// impact * e ^ (-1/2 * (distance / (1/4 * radius)))
	return impact_ * Numeric::exp(Scalar(-0.5) * Numeric::sqr(distance * radiusParameter_));
}

inline FiniteLineConstraint::FiniteLineConstraint(const Vector2& point0, const Vector2& point1, const Scalar impact, const Scalar radius, const Scalar penalty, const bool finite0, const bool finite1) :
	LineConstraint(point0, point1, impact, radius),
	offset_(point1 - point0),
	offsetSquare_(offset_.sqr()),
	penalty_(penalty),
	finite0_(finite0),
	finite1_(finite1)
{
	// nothing to do here
}

inline FiniteLineConstraint::FiniteLineConstraint(const FiniteLineConstraint& constraint, const Scalar scale) :
	LineConstraint(constraint.point0() * scale, constraint.point1() * scale, constraint.impact(), max(Scalar(2), constraint.radius() * scale)),
	offset_((constraint.point1() - constraint.point0()) * scale),
	offsetSquare_(offset_.sqr()),
	penalty_(constraint.penalty() * scale),
	finite0_(constraint.finite0_),
	finite1_(constraint.finite1_)
{
	// nothing to do here
}

inline Scalar FiniteLineConstraint::penalty() const
{
	return penalty_;
}

inline bool FiniteLineConstraint::finite0() const
{
	return finite0_;
}

inline bool FiniteLineConstraint::finite1() const
{
	return finite1_;
}

inline Scalar FiniteLineConstraint::finiteLineDistance(const Vector2& point) const
{
	if (!finite0_ && !finite1_)
	{
		return LineConstraint::infiniteLineDistance(point);
	}

	const Scalar product = offset_ * (point - point0_);
	return ((finite0_ && product < 0) || (finite1_ && product > offsetSquare_)) ? penalty_ : LineConstraint::infiniteLineDistance(point);
}

inline void Constraints::addConstraint(std::unique_ptr<Constraint> constraint)
{
	constraints_.emplace_back(std::move(constraint));
}

inline size_t Constraints::size() const
{
	return constraints_.size();
}

inline Scalar Constraints::cost(const unsigned int insideX, const unsigned int insideY, const Vector2& outside) const
{
	ocean_assert(decisionFrame_.isValid());

	ocean_assert(insideX < decisionFrame_.width());
	ocean_assert(insideY < decisionFrame_.height());

	const uint8_t decision = decisionFrame_.constpixel<uint8_t>(insideX, insideY)[0];

	if (decision == 0xFFu)
	{
		return 0;
	}

	ocean_assert((size_t)decision < constraints_.size());

	return constraints_[decision]->cost(Vector2(Scalar(insideX), Scalar(insideY)), outside);
}

inline Scalar Constraints::cost(const unsigned int insideX, const unsigned int insideY, const unsigned int outsideX, const unsigned int outsideY) const
{
	return cost(insideX, insideY, Vector2(Scalar(outsideX), Scalar(outsideY)));
}

inline bool Constraints::isEmpty() const
{
	return constraints_.empty();
}

inline Constraints::operator bool() const
{
	return !constraints_.empty();
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CONSTRAINT_H
