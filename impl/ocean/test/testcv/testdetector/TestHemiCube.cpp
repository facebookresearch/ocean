// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testdetector/TestHemiCube.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"
#include "ocean/cv/detector/HemiCube.h"
#include "ocean/math/Random.h"

#include <algorithm>
#include <numeric>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using namespace Ocean::CV::Detector;

bool TestHemiCube::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Hemi cube test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAdd(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testLineFusion(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testMergeGreedyBruteForce(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testMerge(testDuration);

	if (allSucceeded)
	{
		Log::info() << "Hemi cube test succeeded.";
	}
	else
	{
		Log::info() << "Hemi cube test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHemiCube, Add)
{
	EXPECT_TRUE(TestHemiCube::testAdd(GTEST_TEST_DURATION));
}

TEST(TestHemiCube, LineFusion)
{
	EXPECT_TRUE(TestHemiCube::testLineFusion(GTEST_TEST_DURATION));
}

TEST(TestHemiCube, MergeGreedyBruteForce)
{
	EXPECT_TRUE(TestHemiCube::testMergeGreedyBruteForce(GTEST_TEST_DURATION));
}

TEST(TestHemiCube, Merge)
{
	EXPECT_TRUE(TestHemiCube::testMerge(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHemiCube::testAdd(const double testDuration)
{
	Log::info() << "Hemi cube fill test:";

	const unsigned int imageWidth = 1920u;
	const unsigned int imageHeight = 1080u;
	const Scalar focalLength = Scalar(1);

	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		const unsigned int hemiCubeBins = RandomI::random(2u, 320u);

		{
			// Case 1: random number of only collinear lines + random bin size, all lines should land in the same bin
			const FiniteLine2 randomSeedLine = generateRandomFiniteLine2(imageWidth, imageHeight);
			const unsigned int linesCount = RandomI::random(2u, 10000u);
			FiniteLines2 randomCollinearLines;
			randomCollinearLines.reserve(linesCount);

			for (unsigned int i = 0u; i < linesCount; ++i)
			{
				const FiniteLine2 randomCollinearLine = generateRandomCollinearFiniteLine2(randomSeedLine, imageWidth, imageHeight, Scalar(0.1) * randomSeedLine.length());
				randomCollinearLines.emplace_back(randomCollinearLine);
			}

			HemiCube hemiCube(hemiCubeBins, imageWidth, imageHeight, focalLength);
			hemiCube.insert(randomCollinearLines);

			if (hemiCube.nonEmptyBins() != 1 || hemiCube.size() != randomCollinearLines.size())
			{
				allSucceeded = false;
			}
		}

		{
			// Case 2: two orthogonal lines - should be in separate bins
			const FiniteLine2 line0 = generateRandomFiniteLine2(imageWidth, imageHeight);
			const FiniteLine2 line1 = generateRandomOrthogonalFiniteLine2(line0, imageWidth, imageHeight);
			const FiniteLines2 lines = { line0, line1 };

			HemiCube hemiCube(hemiCubeBins, imageWidth, imageHeight, focalLength);
			hemiCube.insert(lines);

			ocean_assert(hemiCubeBins >= 2u && "This test case requires more than one bin in the Hemi cube");
			if (hemiCube.nonEmptyBins() != 2)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTime + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestHemiCube::testLineFusion(const double testDuration)
{
	Log::info() << "Hemi cube line merging test";

	const unsigned int imageWidth = 1920u;
	const unsigned int imageHeight = 1080u;

	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		const FiniteLine2 line0 = generateRandomFiniteLine2(imageWidth, imageHeight);
		const FiniteLine2 line1 = generateRandomFiniteLine2(imageWidth, imageHeight);

		const FiniteLine2 mergedLine = HemiCube::fuse(line0, line1);

		const FiniteLines2 lines = {line0, line1};

		if (!validateLineFusion(mergedLine, lines))
		{
			allSucceeded = false;
		}
	}
	while (startTime + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestHemiCube::testMergeGreedyBruteForce(const double testDuration)
{
	Log::info() << "Hemi cube merge (greedy, brute-force)";

	const unsigned int imageWidth = 1920u;
	const unsigned int imageHeight = 1080u;
	const unsigned int imageDiagonal = (unsigned int)Numeric::sqrt(imageWidth * imageWidth + imageHeight * imageHeight);

	const Scalar maxLineGap = Scalar(imageDiagonal);
	const Scalar maxLineDistance(Scalar(0.5));

	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		{
			// Case 1: random number of only collinear lines + random bin size, all lines should land in the same bin
			const FiniteLine2 randomSeedLine = generateRandomFiniteLine2(imageWidth, imageHeight);
			const unsigned int linesCount = RandomI::random(2u, 10000u);
			FiniteLines2 randomCollinearLines;

			for (unsigned int i = 0u; i < linesCount; ++i)
			{
				const FiniteLine2 randomCollinearLine = generateRandomCollinearFiniteLine2(randomSeedLine, imageWidth, imageHeight, Scalar(0.1) * randomSeedLine.length());
				randomCollinearLines.emplace_back(randomCollinearLine);
			}

			const FiniteLines2 mergedLines = HemiCube::mergeGreedyBruteForce(randomCollinearLines, maxLineDistance, maxLineGap);

			if (mergedLines.size() != 1)
			{
				allSucceeded = false;
			}
		}

		{
			// Case 2: two orthogonal lines - should be in separate bins
			const FiniteLine2 line0 = generateRandomFiniteLine2(imageWidth, imageHeight);
			const FiniteLine2 line1 = generateRandomOrthogonalFiniteLine2(line0, imageWidth, imageHeight);
			const FiniteLines2 lines = { line0, line1 };

			const FiniteLines2 mergedLines = HemiCube::mergeGreedyBruteForce(lines, maxLineDistance, maxLineGap);

			if (mergedLines.size() != 2)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTime + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestHemiCube::testMerge(const double testDuration)
{
	Log::info() << "Hemi cube merge";

	const Scalar focalLength = Scalar(1);
	const unsigned int imageWidth = 1920u;
	const unsigned int imageHeight = 1080u;
	const unsigned int imageDiagonal = (unsigned int)Numeric::sqrt(imageWidth * imageWidth + imageHeight * imageHeight);

	const Scalar maxLineGap = Scalar(imageDiagonal);
	const Scalar maxLineDistance(Scalar(0.5));

	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		const unsigned int hemiCubeBins = RandomI::random(2u, 320u);

		{
			// Case 1: random number of only collinear lines + random bin size, all lines should land in the same bin
			const FiniteLine2 randomSeedLine = generateRandomFiniteLine2(imageWidth, imageHeight);
			const unsigned int linesCount = RandomI::random(2u, 10000u);
			FiniteLines2 randomCollinearLines;

			for (unsigned int i = 0u; i < linesCount; ++i)
			{
				const FiniteLine2 randomCollinearLine = generateRandomCollinearFiniteLine2(randomSeedLine, imageWidth, imageHeight);
				randomCollinearLines.emplace_back(randomCollinearLine);
			}

			HemiCube hemiCube(hemiCubeBins, imageWidth, imageHeight, focalLength);
			hemiCube.merge(randomCollinearLines, maxLineDistance, maxLineGap);

			if (hemiCube.nonEmptyBins() != 1)
			{
				allSucceeded = false;
			}
		}

		{
			// Case 2: two orthogonal lines - should be in separate bins
			const FiniteLine2 line0 = generateRandomFiniteLine2(imageWidth, imageHeight);
			const FiniteLine2 line1 = generateRandomOrthogonalFiniteLine2(line0, imageWidth, imageHeight);
			const FiniteLines2 lines = { line0, line1 };

			HemiCube hemiCube(hemiCubeBins, imageWidth, imageHeight, focalLength);
			hemiCube.merge(lines, maxLineDistance, maxLineGap);

			ocean_assert(hemiCubeBins >= 2u && "This test case requires more than one bin in the Hemi cube");
			if (hemiCube.nonEmptyBins() != 2)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTime + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

FiniteLine2 TestHemiCube::generateRandomFiniteLine2(const unsigned int imageWidth, const unsigned int imageHeight)
{
	ocean_assert(imageWidth != 0u && imageHeight != 0u);

	const Vector2 point0 = Random::vector2(Scalar(0), Scalar(imageWidth - 1u), Scalar(0), Scalar(imageHeight - 1u));
	Vector2 point1 = Random::vector2(Scalar(0), Scalar(imageWidth - 1u), Scalar(0), Scalar(imageHeight - 1u));

	while ((point0 - point1).length() < Numeric::eps())
	{
		point1 = Random::vector2(Scalar(0), Scalar(imageWidth - 1u), Scalar(0), Scalar(imageHeight - 1u));
	}

	return FiniteLine2(point0, point1);
};

FiniteLine2 TestHemiCube::generateRandomOrthogonalFiniteLine2(const FiniteLine2& line, const unsigned int imageWidth, const unsigned int imageHeight, const Scalar minLineLength)
{
	ocean_assert(imageWidth != 0u && imageHeight != 0u);
	ocean_assert(minLineLength >= Numeric::eps());

	const Vector2 point0 = Random::vector2(Scalar(0), Scalar(imageWidth - 1u), Scalar(0), Scalar(imageHeight - 1u));
	Vector2 point1;

	while (true)
	{
		const Scalar length = Random::scalar(minLineLength, Scalar(std::min(imageHeight, imageWidth)));
		point1 = point0 + line.normal() * length * Random::sign();

		if (point1.x() < Scalar(0) || point1.x() > Scalar(imageWidth - 1u) || point1.y() < Scalar(0) || point1.y() > Scalar(imageHeight - 1u))
		{
			continue;
		}

		const FiniteLine2 orthogonalLine(point0, point1);
		ocean_assert(orthogonalLine.length() > Numeric::eps());

		ocean_assert(line.normal().isOrthogonal(orthogonalLine.normal(), Numeric::weakEps()));

		if (line.normal().isOrthogonal(orthogonalLine.normal())) // explicit
		{
			return orthogonalLine;
		}
	}
}

FiniteLine2 TestHemiCube::generateRandomCollinearFiniteLine2(const FiniteLine2& line, const unsigned int imageWidth, const unsigned int imageHeight, const Scalar minLineLength)
{
	ocean_assert(minLineLength > Numeric::eps());

	const Scalar distanceEpsilon = Numeric::weakEps();
	const Scalar cosAngleEpsilon = Numeric::weakEps();

	Vector2 point0;
	Vector2 point1;

	const Scalar maxDistance = std::is_same<Scalar, float>() ? Scalar(0.5) : Numeric::weakEps();

	do
	{
		const Scalar length = Random::scalar(minLineLength, Scalar(std::min(imageHeight, imageWidth)));
		point0 = line.point0() + line.direction() * length * Random::sign();
		ocean_assert((line.nearestPointOnInfiniteLine(point0) - point0).length() <= maxDistance);
	}
	while (point0.x() < Scalar(0) || point0.x() >= Scalar(imageWidth) || point0.y() < Scalar(0) || point0.y() >= Scalar(imageHeight));

	FiniteLine2 collinearLine;

	do
	{
		const Scalar length = Random::scalar(minLineLength, Scalar(std::min(imageHeight, imageWidth)));
		point1 = point0 + line.direction() * length * Random::sign();
		ocean_assert((line.nearestPointOnInfiniteLine(point1) - point1).length() <= maxDistance);

		collinearLine = FiniteLine2(point0, point1);
		ocean_assert((collinearLine.nearestPointOnInfiniteLine(line.point0()) - line.point0()).length() <= maxDistance && (collinearLine.nearestPointOnInfiniteLine(line.point1()) - line.point1()).length() <= maxDistance);
	}
	while (point1.x() < Scalar(0) || point1.x() >= Scalar(imageWidth) || point1.y() < Scalar(0) || point1.y() >= Scalar(imageHeight) || collinearLine.isValid() == false || collinearLine.length() <= minLineLength);

	bool isValid = false;
	FiniteLine2 pertubedCollinearLine;

	do
	{
		point0 = collinearLine.point0() + Vector2(collinearLine.normal() * Random::scalar(Scalar(0), Scalar(distanceEpsilon)));
		point1 = collinearLine.point1() + Vector2(collinearLine.normal() * Random::scalar(Scalar(0), Scalar(distanceEpsilon)));
		pertubedCollinearLine = FiniteLine2(point0, point1);

		// Check the angle: cos(angle) = |a| * |b| * (a * b), here a and b are unit vectors (normal)
		const bool validAngle = Numeric::isEqual(Numeric::abs(pertubedCollinearLine.normal() * line.normal()), Scalar(1), cosAngleEpsilon);

		// Check the distances
		const bool validDistance0 = (line.nearestPointOnInfiniteLine(pertubedCollinearLine.point0()) - pertubedCollinearLine.point0()).length() <= maxDistance && (line.nearestPointOnInfiniteLine(pertubedCollinearLine.point1()) - pertubedCollinearLine.point1()).length() <= maxDistance;
		const bool validDistance1 = (pertubedCollinearLine.nearestPointOnInfiniteLine(line.point0()) - line.point0()).length() <= maxDistance && (pertubedCollinearLine.nearestPointOnInfiniteLine(line.point1()) - line.point1()).length() <= maxDistance;
		isValid = pertubedCollinearLine.isValid() && validAngle && validDistance0 && validDistance1;
	}
	while (isValid == false);

	return pertubedCollinearLine;
}

bool TestHemiCube::validateLineFusion(const FiniteLine2& testLine, const FiniteLines2& lines)
{
	ocean_assert(lines.empty() == false);

	Scalar sumLineLengths = 0;

	for (const FiniteLine2& line : lines)
	{
		ocean_assert(line.isValid());

		sumLineLengths += line.length();
	}

	ocean_assert(sumLineLengths > Scalar(0));

	Vector2 centroid(Scalar(0), Scalar(0));
	Vector2 direction(Scalar(0), Scalar(0));

	for (const FiniteLine2& line : lines)
	{
		const Scalar weight = line.length() / sumLineLengths;

		centroid += ((line.point0() + line.point1()) * weight);

		const Vector2 lineDirection = direction * line.direction() >= Scalar(0) ? line.direction() : -line.direction();

		direction += lineDirection * weight;
	}

	centroid /= Scalar(lines.size());

	ocean_assert(direction.length() > 0);
	const FiniteLine2 referenceLine(centroid, centroid + direction);

	// Project the end-points of the line segment on the infinite line
	Vectors2 projectedPoints;

	for (const FiniteLine2& line : lines)
	{
		projectedPoints.emplace_back(referenceLine.nearestPointOnInfiniteLine(line.point0()));
		projectedPoints.emplace_back(referenceLine.nearestPointOnInfiniteLine(line.point1()));
	}

	// Out of all combinations of end-points, find the one that maximizes the length of the result line segment
	ocean_assert(projectedPoints.size() >= 2 && projectedPoints.size() % 2 == 0);

	FiniteLine2 mergedLine(projectedPoints[0], projectedPoints[1]);
	ocean_assert(mergedLine.isValid());

	Scalar longestLineLength = mergedLine.length();

	for (unsigned int nOuter = 0u; nOuter < projectedPoints.size() - 1; ++nOuter)
	{
		for (unsigned int nInner = nOuter + 1u; nInner < projectedPoints.size(); ++nInner)
		{
			const FiniteLine2 currentLine(projectedPoints[nOuter], projectedPoints[nInner]);
			ocean_assert(currentLine.isValid());

			const Scalar currentLineLength = currentLine.length();

			if (currentLineLength > longestLineLength)
			{
				mergedLine = currentLine;
				longestLineLength = currentLineLength;
			}
		}
	}
	ocean_assert(mergedLine.isValid());

	// Make sure the endpoints of the test line and the merged line are within acceptable proximity to each other
	const Scalar distance0 = std::min(testLine.point0().distance(mergedLine.point0()), testLine.point0().distance(mergedLine.point1()));
	const Scalar distance1 = std::min(testLine.point1().distance(mergedLine.point0()), testLine.point1().distance(mergedLine.point1()));

	const Scalar threshold = std::is_same<float, Scalar>::value ? Scalar(0.5) : Numeric::weakEps(); // in pixel

	const bool validationSuccessful = Numeric::isEqual(distance0, Scalar(0), threshold) && Numeric::isEqual(distance1, Scalar(0), threshold);

	return validationSuccessful;
}

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
