/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_ORB_SAMPLING_PATTERN_H
#define META_OCEAN_CV_DETECTOR_ORB_SAMPLING_PATTERN_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This singleton holds several lookup tables for the determination of ORB descriptors.
 * It holds one lookup table for different feature orientations.
 * These number is defined by a member variable that graduates the range of the angle of the orientation (in radian [0, 2*PI)) in several increments.
 * A lookup table contains 256 sets of two 2D coordinates that define the position of a binary test during the ORB descriptor calculation.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT ORBSamplingPattern : public Singleton<ORBSamplingPattern>
{
	friend class Singleton<ORBSamplingPattern>;

	public:

		/**
		 * This class holds 2D coordinates of two points which are used for a binary test during the ORB descriptor calculation.
		 * The stored coordinates are offset values that must be added to the feature point observation during a test.
		 */
		class LookupPosition
		{
			friend class ORBSamplingPattern;

			public:

				/**
				 * Default constructor.
				 */
				LookupPosition() = default;

				/**
				 * Returns the first lookup point.
				 * @return The first lookup point
				 */
				inline const Vector2& point0() const;

				/**
				 * Returns the second lookup point.
				 * @return The second lookup point
				 */
				inline const Vector2& point1() const;

			protected:

				/**
				 * Set the coordinates of the two points.
				 * @param point0 The first point to set
				 * @param point1 The second point to set
				 */
				inline void setPositions(const Vector2& point0, const Vector2& point1);

			protected:

				/// The first point.
				Vector2 point0_;

				/// The second point.
				Vector2 point1_;
		};

		/**
		 * Definition of a vector holding LookupPosition objects.
		 */
		using LookupTable = std::vector<LookupPosition>;

	protected:

		/**
		 * Definition of a vector holding lookup tables.
		 */
		using LookupTables = std::vector<LookupTable>;

	public:

		/**
		 * Returns a vector of sampling pattern lookup tables for all angle increments.
		 * @return Sampling pattern lookup tables
		 */
		inline const LookupTables& samplingPatterns() const;

		/**
		 * Returns the lookup table of a sampling pattern for a given angle.
		 * @param angle The angle for which the lookup table is requested, in radian, with range [0, 2*PI)
		 * @return The lookup table with the individual sampling patterns for the given angle, lookup locations with be within the range (-18.385, 18.385)x(-18.385, 18.385)
		 */
		inline const LookupTable& samplingPatternForAngle(const Scalar angle) const;

	protected:

		/**
		 * Creates a new ORBSamplingPattern object.
		 */
		ORBSamplingPattern();

		/**
		 * Creates the sampling pattern lookup tables for the ORB feature descriptor calculation tests.
		 * The number of tables is equal to the given angle increments
		 * @param angleIncrements The number of increments of a full rotation (must match with the corresponding value of this class)
		 * @return The lookup table
		 */
		static LookupTables createLookupTables(const unsigned int angleIncrements);

	protected:

		// Number of increments of a full rotation, with range [1, 360]
		static constexpr unsigned int angleIncrements_ = 72u;

		// Represents 1 / (angle per increment)
		static constexpr Scalar anglePerIncrementFactor_ = Scalar(angleIncrements_) / Numeric::pi2();

		// Sampling pattern lookup tables
		const LookupTables lookupTables_;

};

inline const Vector2& ORBSamplingPattern::LookupPosition::point0() const
{
	return point0_;
}

inline const Vector2& ORBSamplingPattern::LookupPosition::point1() const
{
	return point1_;
}

inline void ORBSamplingPattern::LookupPosition::setPositions(const Vector2& point0, const Vector2& point1)
{
	point0_ = point0;
	point1_ = point1;

	ocean_assert(point0_ != point1_);
}

inline const ORBSamplingPattern::LookupTables& ORBSamplingPattern::samplingPatterns() const
{
	return lookupTables_;
}

inline const ORBSamplingPattern::LookupTable& ORBSamplingPattern::samplingPatternForAngle(const Scalar angle) const
{
	ocean_assert(angle >= Scalar(0) && angle < Numeric::pi2());

	return lookupTables_[(unsigned int)(angle * anglePerIncrementFactor_ + Scalar(0.5)) % angleIncrements_];
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORB_SAMPLING_PATTERN_H
