// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_CV_DETECTOR_ORB_SAMPLING_PATTERN_H
#define META_OCEAN_CV_DETECTOR_ORB_SAMPLING_PATTERN_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/math/Numeric.h"

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
		class IntensityComparison
		{
			public:

				/**
				 * Creates an IntensityComparison object and sets all coordinates to zero.
				 */
				inline IntensityComparison();

				/**
				 * Creates an IntensityComparison object by given coordinates.
				 * @param x1 X coordinate of first point
				 * @param y1 Y coordinate of first point
				 * @param x2 X coordinate of second point
				 * @param y2 Y coordinate of second point
				 */
				inline IntensityComparison(const Scalar x1, const Scalar y1, const Scalar x2, const Scalar y2);

				/**
				 * Returns the x coordinate of the first point.
				 * @return X coordinate of first point.
				 */
				inline Scalar x1() const;

				/**
				 * Returns the y coordinate of the first point.
				 * @return Y coordinate of first point
				 */
				inline Scalar y1() const;

				/**
				 * Returns the x coordinate of the second point.
				 * @return X coordinate of second point
				 */
				inline Scalar x2() const;

				/**
				 * Returns the y coordinate of the second point.
				 * @return Y coordinate of second point
				 */
				inline Scalar y2() const;

				/**
				 * Set the coordinates of the two points.
				 * @param x1 X coordinate of first point
				 * @param y1 Y coordinate of first point
				 * @param x2 X coordinate of second point
				 * @param y2 Y coordinate of second point
				 */
				inline void setValues(const Scalar x1, const Scalar y1, const Scalar x2, const Scalar y2);

			protected:

				// The four coordinate values
				Scalar values[4];
		};

		/**
		 * Definition of a vector holding IntensityComparison objects.
		 */
		typedef std::vector<IntensityComparison> IntensityComparisons;

	public:

		/**
		 * Returns a vector of sampling pattern lookup tables for all angle increments.
		 * @return Sampling pattern lookup tables
		 */
		inline const std::vector<IntensityComparisons>& samplingPatterns() const;

		/**
		 * Returns the lookup table of a sampling pattern for a given angle.
		 * It returns the lookup table of that angle increment, the given angle lies into.
		 * @param angle Angle in radian to get the lookup table for, Range [0, 2*PI)
		 * @return Sampling pattern lookup table for the given angle
		 */
		inline const IntensityComparisons& samplingPatternByAngle(const Scalar angle) const;

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
		static std::vector<IntensityComparisons> createLookupTable(const unsigned int angleIncrements);

	protected:

		// Number of increments of a full rotation. Range [1, 360]
		const unsigned int angleIncrements;

		// Represents 1 / (angle per increment)
		const Scalar anglePerIncrementFactor;

		// Sampling pattern lookup tables
		const std::vector<IntensityComparisons> samplingPatternLookupTables;

};

inline ORBSamplingPattern::IntensityComparison::IntensityComparison()
{
	values[0] = Scalar(0.0);
	values[1] = Scalar(0.0);
	values[2] = Scalar(0.0);
	values[3] = Scalar(0.0);
}

inline ORBSamplingPattern::IntensityComparison::IntensityComparison(const Scalar x1, const Scalar y1, const Scalar x2, const Scalar y2)
{
	values[0] = x1;
	values[1] = y1;
	values[2] = x2;
	values[3] = y2;
}

inline Scalar ORBSamplingPattern::IntensityComparison::x1() const
{
	return values[0];
}

inline Scalar ORBSamplingPattern::IntensityComparison::y1() const
{
	return values[1];
}

inline Scalar ORBSamplingPattern::IntensityComparison::x2() const
{
	return values[2];
}

inline Scalar ORBSamplingPattern::IntensityComparison::y2() const
{
	return values[3];
}

inline void ORBSamplingPattern::IntensityComparison::setValues(const Scalar x1, const Scalar y1, const Scalar x2, const Scalar y2)
{
	values[0] = x1;
	values[1] = y1;
	values[2] = x2;
	values[3] = y2;
}

inline const std::vector<ORBSamplingPattern::IntensityComparisons>& ORBSamplingPattern::samplingPatterns() const
{
	return samplingPatternLookupTables;
}

inline const ORBSamplingPattern::IntensityComparisons& ORBSamplingPattern::samplingPatternByAngle(const Scalar angle) const
{
	ocean_assert(angle >= Scalar(0.0) && angle < Numeric::pi2());
	return samplingPatternLookupTables[(unsigned int)(angle * anglePerIncrementFactor + Scalar(0.5)) % angleIncrements];
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORB_SAMPLING_PATTERN_H
