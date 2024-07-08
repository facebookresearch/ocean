/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/ORBSamplingPattern.h"

#include "ocean/cv/detector/ORBDescriptor.h"

#include "ocean/math/Rotation.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

ORBSamplingPattern::ORBSamplingPattern() :
	lookupTables_(createLookupTables(angleIncrements_))
{
	static_assert(sizeof(ORBDescriptor::DescriptorBitset) * 8 == 256, "Invalid ORB descriptor bitset length!");

#ifdef OCEAN_DEBUG

	const Scalar anglePerIncrement = Numeric::pi2() / Scalar(angleIncrements_);

	// set the member variable for later calculations
	ocean_assert(Numeric::isEqual(anglePerIncrementFactor_, Scalar(1.0) / anglePerIncrement));

#endif // OCEAN_DEBUG
}

ORBSamplingPattern::LookupTables ORBSamplingPattern::createLookupTables(const unsigned int angleIncrements)
{
	ocean_assert(angleIncrements >= 1u && angleIncrements <= 360u);

	static_assert(sizeof(ORBDescriptor::DescriptorBitset) * 8 == 256, "Invalid ORB descriptor bitset length!");

	// Represents the pattern for the intensity comparisons (pixel comparison in a gray scale image) for a descriptor determination with a bitset length of 256
	// Each line is one comparison (x of point 1, y of point 1, x of point 2, y of point 2)
	// These pattern were generated as follows:
	//     - We detected feature points in 400 images and calculated their orientations
	//     - We determined the results of all possible intensity comparisons in a 31x31 pixel patch around the feature point (1 if intensity of point 1 is less then point 2, 0 otherwise)
	//     - For each individual test the mean of all feature points was determined
	//     - These values were sorted by their distance to 0.5
	//     - To got the 256 best comparisons:
	//         (1) The first resulting test is the first test of the sorted list
	//         (2) We iterated through the sorted list and took a comparison if the correlation compared to the existing final tests was not greater than a threshold
	//         (3) Step 2 was repeated until we had 256 comparison tests; the threshold was increased if we did not have 256 final test after iterating through the full sorted list
	// These pattern is different to the pattern from openCV but provides the same quality results

	const int bit_pattern_31[256 * 4] =
	{
		-2, -12, 10, 13,
		4, -3, -9, 1,
		-6, 4, -13, 8,
		4, -13, 1, -8,
		13, -13, 8, -10,
		-13, -12, -10, -7,
		8, 10, 13, 13,
		3, 1, 8, 2,
		1, 8, 0, 13,
		-10, 8, -11, 13,
		-12, -6, -7, -4,
		8, 1, 13, 3,
		-10, -2, -10, 3,
		-4, 13, 1, 13,
		11, -10, 12, -5,
		-13, 4, -10, 9,
		9, -2, 6, 3,
		-2, 3, -2, 8,
		-6, -13, -1, -12,
		10, 10, 5, 13,
		-2, -1, -2, 4,
		5, -8, 10, -7,
		-12, -8, -13, -3,
		-7, -7, -4, -2,
		-2, -9, 3, -9,
		-6, -13, -11, -11,
		-5, -4, 0, 1,
		11, 2, 13, 7,
		-8, 10, -3, 12,
		13, -6, 13, -1,
		4, 3, 4, 8,
		-3, 7, 3, 7,
		2, -13, 7, -12,
		-13, 13, -8, 13,
		1, -5, 6, -1,
		1, 12, 6, 13,
		2, -9, 2, -4,
		8, -5, 13, -3,
		9, -13, -8, 13,
		11, 6, 8, 11,
		-4, -13, -3, -8,
		8, -13, 8, -8,
		-13, -2, 0, 8,
		-12, -13, 13, 0,
		-7, -10, -9, -5,
		2, 5, 7, 9,
		-2, -5, -4, 0,
		-5, 8, -5, 13,
		8, -8, 6, -3,
		-1, 1, 4, 3,
		-3, 4, -8, 6,
		-7, 1, -6, 6,
		13, -2, 12, 3,
		-1, -7, -6, -6,
		3, -6, 0, -1,
		-8, 5, -6, 10,
		6, 0, 8, 5,
		-5, 2, 4, 13,
		-7, 0, -12, 1,
		2, -4, 3, 6,
		6, 8, 7, 13,
		-13, 12, 12, 12,
		13, -11, -3, -5,
		-13, -7, -13, 12,
		7, 5, 0, 8,
		5, -10, 11, 2,
		-3, -10, -13, 4,
		1, -13, -6, -2,
		4, 6, 13, 7,
		-9, -6, -4, -6,
		-7, -7, -2, 13,
		1, -9, 6, -6,
		-5, -11, 1, -5,
		-10, -13, -7, 3,
		4, -3, 10, -2,
		13, -4, -2, 4,
		-11, 1, -11, 6,
		6, -13, 11, -11,
		13, -13, 13, 9,
		-3, -3, 10, 7,
		-8, -2, -3, -1,
		1, 2, -4, 3,
		-10, -13, -4, -9,
		-1, 8, -9, 13,
		-10, 5, -5, 7,
		-11, -5, -10, 0,
		8, -1, 1, 13,
		6, 3, 12, 12,
		3, 3, -5, 11,
		-3, -2, -10, 9,
		7, -5, 7, 0,
		5, -11, -13, -8,
		13, 2, 9, 7,
		-5, -4, -10, -1,
		8, -12, 13, -7,
		-13, -9, -8, -9,
		1, -13, 4, -7,
		-2, -9, -2, -4,
		12, 4, -7, 6,
		13, -9, 8, -4,
		7, -7, 1, -3,
		-7, -2, -5, 3,
		-6, 1, -1, 4,
		-3, -5, 2, -5,
		1, 5, 1, 10,
		13, 5, 13, 10,
		-11, -13, -13, -8,
		8, -8, 6, 11,
		-12, -9, -2, 3,
		-13, 8, -9, 13,
		3, -13, 10, -4,
		3, 5, -13, 6,
		-4, -9, 6, 3,
		8, 3, 8, 8,
		13, 8, 13, 13,
		4, 10, 9, 13,
		1, -9, -3, 7,
		5, -13, 2, 3,
		4, 0, 0, 5,
		-13, -3, -8, 2,
		-5, 6, 0, 9,
		-4, -13, 10, -8,
		8, -4, 13, 5,
		-8, -13, -7, -8,
		12, -13, 12, -8,
		-8, -11, -4, -5,
		9, -13, 4, -10,
		1, 2, 5, 7,
		-11, 4, -13, 9,
		-5, 10, -11, 11,
		-7, -4, 9, -4,
		13, -5, 8, 0,
		-1, -7, 2, -2,
		-5, -13, 0, 1,
		-10, -6, 6, 10,
		-13, 2, -8, 5,
		-3, 7, 0, 12,
		3, 10, -2, 12,
		-1, -3, 2, 2,
		13, -4, 12, 13,
		-1, -13, 4, -13,
		-2, -13, -8, -9,
		-3, -1, 2, -1,
		-3, 1, 1, 6,
		5, 2, 11, 2,
		-2, 1, -7, 3,
		7, -2, 1, 2,
		-8, -2, -8, 13,
		-10, 3, -1, 13,
		-7, -5, -7, 0,
		-4, 5, -7, 10,
		-10, 1, -4, 3,
		-9, -8, -8, -3,
		-8, -7, -13, -4,
		-1, 8, 4, 11,
		0, -4, -13, -3,
		5, -3, 8, 2,
		7, -5, -13, 9,
		5, -11, 6, -6,
		13, 9, -2, 13,
		3, -7, 4, -1,
		3, -2, 13, 9,
		1, 0, 1, 9,
		9, -7, 11, -2,
		-1, -6, 3, 13,
		7, 6, 5, 11,
		-9, -8, 4, -1,
		1, -13, -4, -11,
		8, 2, 3, 6,
		-4, -11, -6, -6,
		-1, -8, 13, -3,
		10, -9, -13, 0,
		8, -9, 3, -7,
		6, -8, -5, 2,
		-3, 3, 13, 13,
		5, -4, -1, -3,
		0, 2, -13, 13,
		8, -13, 3, -4,
		-8, 3, -9, 8,
		-8, -2, 4, 3,
		-3, -3, -6, 3,
		-13, -13, 2, 12,
		5, 7, 1, 12,
		-12, -3, -7, 8,
		-11, -10, -8, -5,
		6, 9, -8, 10,
		-9, -4, -5, 1,
		5, -1, 8, 13,
		-9, -13, 9, -13,
		13, 11, 8, 13,
		-12, -1, 13, 9,
		1, -10, -4, -7,
		0, -12, -1, -7,
		13, 3, 3, 10,
		-4, 1, 8, 1,
		-13, -12, -1, -3,
		11, -10, 6, 3,
		1, -2, -6, 7,
		0, 5, -5, 7,
		3, -7, 9, 8,
		-13, -13, -8, -12,
		-4, -6, 0, 5,
		-13, 7, -8, 8,
		-8, -1, -13, 5,
		-13, 9, 2, 13,
		3, 7, 4, 12,
		-4, -7, -8, -2,
		-2, -13, 6, -2,
		-4, -11, -13, -6,
		-6, -13, -7, 11,
		-1, -3, -6, -1,
		4, -4, 4, 1,
		13, -9, 0, 10,
		-13, 1, 1, 1,
		0, -2, 13, 2,
		-10, -11, 1, -8,
		3, -5, -4, 13,
		-6, -1, -1, 9,
		-2, 6, -3, 11,
		10, 0, 10, 5,
		-8, -5, -13, 1,
		7, -9, 9, -4,
		-6, -13, 13, 7,
		-13, -11, -13, 5,
		8, 2, -9, 13,
		2, -3, -2, 2,
		0, -7, -3, -2,
		13, -1, 7, 2,
		5, -11, -2, -4,
		13, -7, 2, -4,
		10, -5, 4, 0,
		11, -13, -1, 1,
		-4, -13, 1, -10,
		-5, -1, -10, 3,
		-7, 6, -9, 12,
		-13, 6, -13, 11,
		-13, 1, -5, 11,
		9, 8, 11, 13,
		10, -4, 10, 1,
		-7, 4, -2, 7,
		-13, -3, -13, 2,
		-9, -12, 3, 5,
		-13, -7, -10, -2,
		0, -12, -13, 13,
		-6, 6, 8, 13,
		1, -9, -10, -3,
		3, 9, 8, 9,
		-4, -10, -4, 3,
		1, -11, 13, -9,
		10, -13, 9, -1,
		-4, -7, -2, -1,
		-8, 7, -13, 11,
		-9, 7, -5, 13,
		-7, -5, -8, 6,
		5, 2, -4, 6,
		-7, -10, 6, -7
	};

	const Scalar anglePerIncrement = Numeric::pi2() / Scalar(angleIncrements);

	LookupTables lookupTables(angleIncrements);

	for (size_t i = 0; i < lookupTables.size(); ++i)
	{
		const Scalar angle = Scalar(i) * anglePerIncrement;
		const Quaternion rotation(Vector3(0, 0, 1), angle);

		LookupTable& lookupTable = lookupTables[i];

		lookupTable.resize(sizeof(ORBDescriptor::DescriptorBitset) * 8);

		for (size_t j = 0; j < lookupTable.size(); ++j)
		{
			Vector3 point0(Scalar(bit_pattern_31[4 * j]), Scalar(bit_pattern_31[4 * j + 1]), Scalar(0.0));
			Vector3 point1(Scalar(bit_pattern_31[4 * j + 2]), Scalar(bit_pattern_31[4 * j + 3]), Scalar(0.0));

			point0 = rotation * point0;
			point1 = rotation * point1;

#ifdef OCEAN_DEBUG
			constexpr Scalar threshold = Scalar(18.385);

			ocean_assert(point0.x() > -threshold && point0.x() < threshold);
			ocean_assert(point1.y() > -threshold && point1.y() < threshold);

			const Scalar layerFactor = Numeric::sqrt(Scalar(1.41421)); // sqrt(2)

			constexpr Scalar layerThreshold = Scalar(26);

			ocean_assert(point0.x() * layerFactor > -layerThreshold && point0.x() * layerFactor < layerThreshold);
			ocean_assert(point1.y() * layerFactor > -layerThreshold && point1.y() * layerFactor < layerThreshold);
#endif

			lookupTable[j].setPositions(point0.xy(), point1.xy());
		}
	}

	return lookupTables;
}

}

}

}
