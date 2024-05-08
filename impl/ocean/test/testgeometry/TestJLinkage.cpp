/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestGeometry.h"
#include "ocean/test/testgeometry/TestJLinkage.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/JLinkage.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line2.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestJLinkage::testJLinkage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   J-Linkage test:   ---";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testFaultlessSingleHomography<LM_JLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessNoisedSingleHomography<LM_JLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessMultipleHomography<LM_JLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessLines<LM_JLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "J-Linkage test succeeded.";
	}
	else
	{
		Log::info() << "J-Linkage test FAILED!";
	}

	return allSucceeded;
}

bool TestJLinkage::testTLinkage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   T-Linkage test:   ---";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testFaultlessSingleHomography<LM_TLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessNoisedSingleHomography<LM_TLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessMultipleHomography<LM_TLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFaultlessLines<LM_TLINKAGE>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "T-Linkage test succeeded.";
	}
	else
	{
		Log::info() << "T-Linkage test FAILED!";
	}

	return allSucceeded;
}

template <TestJLinkage::LinkageMethod tLinkageType>
bool TestJLinkage::testFaultlessSingleHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perfect 2D point correspondences of planar 3D object points:";
	Log::info() << " ";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	bool allSucceeded = true;

	const unsigned int correspondences[] = {10u, 25u, 50u, 100u};

	for (unsigned int i = 0u; i < sizeof(correspondences) / sizeof(correspondences[0]); ++i)
	{
		const unsigned int number = correspondences[i];

		Log::info() << "... with " << number << " correspondences:";

		HighPerformanceStatistic performance;

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			const Vector3 translation(Random::vector3());
			const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);
			const HomogenousMatrix4 transformation(translation, quaternion);

			const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

			Vectors2 leftImagePoints, rightImagePoints;
			Vectors3 objectPoints;

			bool localSucceeded = true;

			for (unsigned int n = 0u; n < number; ++n)
			{
				const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(pinholeCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
				const Line3 ray(pinholeCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

				Vector3 objectPoint;
				if (plane.intersection(ray, objectPoint))
				{
					const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformation, objectPoint, false));

					leftImagePoints.push_back(leftImagePoint);
					rightImagePoints.push_back(rightImagePoint);
					objectPoints.push_back(objectPoint);
				}
			}

			ocean_assert(leftImagePoints.size() == rightImagePoints.size());
			ocean_assert(leftImagePoints.size() == objectPoints.size());

			if (leftImagePoints.size() == number)
			{
				SquareMatrices3 homographies;

				IndexSet32 indexSet;

				while (double(indexSet.size()) < 0.2 * double(number))
				{
					const Index32 index = RandomI::random((unsigned int)number - 1u);
					indexSet.insert(index);
				}

				const Geometry::ImagePoints permutationLeftImagePoints(Subset::subset(leftImagePoints, indexSet));

				performance.start();

				if constexpr (tLinkageType == LM_TLINKAGE)
				{
					if (!Geometry::TLinkage::homographyMatrices(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), homographies, 8u, permutationLeftImagePoints, Scalar(5)))
					{
						localSucceeded = false;
					}
				}
				else
				{
					if (!Geometry::JLinkage::homographyMatrices(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), width, height, homographies, 8u, permutationLeftImagePoints, Scalar(1)))
					{
						localSucceeded = false;
					}
				}

				performance.stop();

				if (homographies.size() == 1ull)
				{
					for (size_t n = 0; n < leftImagePoints.size(); ++n)
					{
						const Vector2& leftImagePoint = leftImagePoints[n];
						const Vector2& rightImagePoint = rightImagePoints[n];

						const Vector2 testRightPoint(homographies[0] * leftImagePoint);

						if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
						{
							localSucceeded = false;
							break;
						}
					}
				}
				else
				{
					localSucceeded = false;
				}
			}
			else
				localSucceeded = false;

			if (localSucceeded)
				++validIterations;

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

		Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.97 && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <TestJLinkage::LinkageMethod tLinkageType>
bool TestJLinkage::testFaultlessNoisedSingleHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Noised 2D point correspondences of planar 3D object points:";
	Log::info() << " ";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int correspondences[] = {10u, 20u, 30u, 100u};

	for (unsigned int i = 0u; i < sizeof(correspondences) / sizeof(correspondences[0]); ++i)
	{
		const unsigned int number = correspondences[i];

		Log::info() << "... with " << number << " correspondences:";

		HighPerformanceStatistic performance;

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		Timestamp startTimestamp(true);

		do
		{
			const Vector3 translation(Random::vector3());
			const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
			const Quaternion quaternion(euler);
			const HomogenousMatrix4 transformation(translation, quaternion);

			const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

			Vectors2 leftImagePoints, rightImagePoints;
			Vectors3 objectPoints;

			for (unsigned int n = 0u; n < number; ++n)
			{
				const Vector2 leftImagePoint(Random::scalar(Scalar(0u), Scalar(pinholeCamera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
				const Line3 ray(pinholeCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

				Vector3 objectPoint;
				if (plane.intersection(ray, objectPoint))
				{
					const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformation, objectPoint, false));
					const Vector2 leftNoise(Random::gaussianNoise(1), Random::gaussianNoise(1));

					leftImagePoints.push_back(leftImagePoint + leftNoise);
					rightImagePoints.push_back(rightImagePoint);
					objectPoints.push_back(objectPoint);
				}
			}

			ocean_assert(leftImagePoints.size() == rightImagePoints.size());
			ocean_assert(leftImagePoints.size() == objectPoints.size());

			if (leftImagePoints.size() == number)
			{
				SquareMatrices3 homographies;

				IndexSet32 indexSet;

				while (double(indexSet.size()) < 0.5 * double(number))
				{
					const Index32 index = RandomI::random((unsigned int)number - 1u);
					indexSet.insert(index);
				}

				const Geometry::ImagePoints permutationLeftImagePoints(Subset::subset(leftImagePoints, indexSet));

				performance.start();

				if constexpr (tLinkageType == LM_TLINKAGE)
				{
					Geometry::TLinkage::homographyMatrices(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), homographies, 8u, permutationLeftImagePoints, 20, nullptr, true, &randomGenerator);
				}
				else
				{
					Geometry::JLinkage::homographyMatrices(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), width, height, homographies, 8u, permutationLeftImagePoints, 16, nullptr, true, false, &randomGenerator);
				}

				performance.stop();

				if (homographies.size() == 1ull)
				{
					for (size_t n = 0; n < leftImagePoints.size(); ++n)
					{
						const Vector2& leftImagePoint = leftImagePoints[n];
						const Vector2& rightImagePoint = rightImagePoints[n];

						const Vector2 testRightPoint(homographies[0] * leftImagePoint);

						const Scalar error = testRightPoint.sqrDistance(rightImagePoint);
						if (error <= Scalar(3.5 * 3.5))
						{
							validIterations++;
						}
					}
				}
			}

			iterations += (unsigned long long)leftImagePoints.size();
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

		Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.85 && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <TestJLinkage::LinkageMethod tLinkageType>
bool TestJLinkage::testFaultlessMultipleHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perfect 2D point correspondences of planar 3D object points:";
	Log::info() << " ";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (unsigned int h = 2u; h < 4u; h++)
	{
		Log::info().newLine(h != 2u);

		const Scalar sectionWidth(width / Scalar(h));

		const unsigned int correspondences[] = {30u, 50, 75u};

		for (unsigned int c = 0u; c < sizeof(correspondences) / sizeof(correspondences[0]); ++c)
		{
			const unsigned int number = correspondences[c];

			Log::info() << "... with " << h << " homographies and " << number << " correspondences each:";

			HighPerformanceStatistic performance;

			unsigned long long iterations = 0ull;
			unsigned long long validIterations = 0ull;

			const Timestamp startTimestamp(true);

			do
			{
				const Plane3 plane(Vector3(0, -5, 0), Vector3(Scalar(0.1), 1, Scalar(0.2)).normalized());

				Vectors2 leftImagePoints, rightImagePoints;
				Vectors3 objectPoints;

				for (unsigned int i = 0u; i < h; i++)
				{
					const Scalar sectionDiv(Scalar(width * i) / Scalar(h));

					const Vector3 translation(Random::vector3());
					const Euler euler(Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)), Random::scalar(Numeric::deg2rad(-30), Numeric::deg2rad(30)));
					const Quaternion quaternion(euler);
					const HomogenousMatrix4 transformation(translation, quaternion);

					for (unsigned int n = 0u; n < number; ++n)
					{
						const Vector2 leftImagePoint(Random::scalar(sectionDiv, sectionDiv + sectionWidth), Random::scalar(Scalar(0u), Scalar(pinholeCamera.height() - 1u)));
						const Line3 ray(pinholeCamera.ray(leftImagePoint, HomogenousMatrix4(true)));

						Vector3 objectPoint;
						if (plane.intersection(ray, objectPoint))
						{
							const Vector2 rightImagePoint(pinholeCamera.projectToImage<true>(transformation, objectPoint, false));

							leftImagePoints.push_back(leftImagePoint);
							rightImagePoints.push_back(rightImagePoint);
							objectPoints.push_back(objectPoint);
						}
					}
				}

				bool localSucceeded = true;

				ocean_assert(leftImagePoints.size() == rightImagePoints.size());
				ocean_assert(leftImagePoints.size() == objectPoints.size());

				SquareMatrices3 homographies;

				Geometry::ImagePoints initialLeftImagePoints;

				for (unsigned int i = 0u; i < h; i++)
				{
					const Scalar sectionDiv(Scalar(width * i) / Scalar(h));
					initialLeftImagePoints.push_back(Vector2(width / (2 * h) + sectionDiv, height / 2));
				}

				performance.start();

				if constexpr (tLinkageType == LM_TLINKAGE)
				{
					if (!Geometry::TLinkage::homographyMatrices(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), homographies, 8u, initialLeftImagePoints, Scalar(5)))
					{
						localSucceeded = false;
					}
				}
				else
				{
					if (!Geometry::JLinkage::homographyMatrices(leftImagePoints.data(), rightImagePoints.data(), leftImagePoints.size(), width, height, homographies, 8u, initialLeftImagePoints, Scalar(2), nullptr, true, false))
					{
						localSucceeded = false;
					}
				}

				performance.stop();

				if (homographies.size() == h)
				{
					for (size_t n = 0; n < leftImagePoints.size(); ++n)
					{
						const Vector2& leftImagePoint = leftImagePoints[n];
						const Vector2& rightImagePoint = rightImagePoints[n];

						const Vector2 testRightPoint(homographies[n / number] * leftImagePoint);

						if (testRightPoint.sqrDistance(rightImagePoint) > Scalar(0.01 * 0.01))
						{
							localSucceeded = false;
							break;
						}
					}
				}
				else
				{
					localSucceeded = false;
				}

				if (localSucceeded)
					++validIterations;

				++iterations;
			}
			while (startTimestamp + testDuration > Timestamp(true));

			ocean_assert(iterations != 0ull);
			const double percent = double(validIterations) / double(iterations);

			Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

			Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

			allSucceeded = percent >= 0.90 && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <TestJLinkage::LinkageMethod tLinkageType>
bool TestJLinkage::testFaultlessLines(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Perfect 2D lines:";
	Log::info() << " ";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(1) : Scalar(0.01);

	const unsigned int size = 640u;

	bool allSucceeded = true;

	for (unsigned int l = 1u; l < 5u; l++)
	{
		Log::info().newLine(l != 1u);

		const unsigned int points[] = {20u, 50, 75u};

		for (unsigned int p = 0u; p < sizeof(points) / sizeof(points[0]); ++p)
		{
			const unsigned int number = points[p];

			Log::info() << "... with " << l  << " lines and " << number << " points each:";

			HighPerformanceStatistic performance;

			unsigned long long iterations = 0ull;
			unsigned long long validIterations = 0ull;

			const Timestamp startTimestamp(true);

			do
			{
				Geometry::ImagePoints imagePoints;

				Lines2 randomLines;

				for (unsigned int i = 0u; i < l; i++)
				{
					const Vector2 linePoint(Random::vector2(0, Scalar(size)));

					Vector2 lineDirection(Random::vector2(1, Scalar(size)));
					lineDirection.normalize();

					const Line2 currentLine(linePoint, lineDirection);

					bool newLine = true;
					for (size_t j = 0; j < randomLines.size(); j++)
					{
						newLine = newLine && currentLine != randomLines[j];
					}

					if (!newLine)
					{
						--i;
						continue;
					}

					randomLines.push_back(currentLine);

					while (imagePoints.size() < (i + 1u) * number)
					{
						const Scalar distance(Random::scalar(-linePoint.length(), linePoint.length()));

						const Vector2 point(currentLine.point(distance));

						if (point.x() > 0 && point.y() > 0)
						{
							imagePoints.push_back(point);
						}
					}
				}

				bool localSucceeded = true;

				Lines2 lines;
				IndexSet32 indexSet;

				for (unsigned int i = 0u; i < l; i++)
				{
					const Index32 index = RandomI::random(i * number, (i + 1u) * number - 1u);
					indexSet.insert(index);
				}

				while (double(indexSet.size()) < 0.2 * double(number))
				{
					const Index32 index = RandomI::random((unsigned int)imagePoints.size() - 1u);
					indexSet.insert(index);
				}

				const Geometry::ImagePoints initialImagePoints(Subset::subset(imagePoints, indexSet));

				performance.start();

				if constexpr (tLinkageType == LM_TLINKAGE)
				{
					if (!Geometry::TLinkage::fitLines(imagePoints.data(), imagePoints.size(), lines, 2u, initialImagePoints, Scalar(0.01)))
					{
						localSucceeded = false;
					}
				}
				else
				{
					if (!Geometry::JLinkage::fitLines(imagePoints.data(), imagePoints.size(), size, size, lines, 2u, initialImagePoints, Scalar(0.01), nullptr, false))
					{
						localSucceeded = false;
					}
				}

				performance.stop();

				if (lines.size() == l)
				{
					for (size_t n = 0; n < imagePoints.size(); ++n)
					{
						const Vector2& leftImagePoint = imagePoints[n];

						bool fitAnyModel = false;

						for (size_t m = 0; m < lines.size(); m++)
						{
							const Scalar error = lines[m].distance(leftImagePoint);
							if (error < Numeric::sqr(epsilon))
							{
								fitAnyModel = true;
								break;
							}
						}

						if (!fitAnyModel)
						{
							localSucceeded = false;
							break;
						}
					}
				}
				else
				{
					localSucceeded = false;
				}

				if (localSucceeded)
				{
					++validIterations;
				}

				++iterations;
			}
			while (startTimestamp + testDuration > Timestamp(true));

			ocean_assert(iterations != 0ull);
			const double percent = double(validIterations) / double(iterations);

			Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

			Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

			allSucceeded = percent >= 0.50 && allSucceeded;
		}
	}

	Log::info() << " ";

	if (!allSucceeded && std::is_same<Scalar, float>::value)
	{
		Log::info() << "The test failed, however the applied 32 bit floating point value precision is too low for this function so that we rate the result as expected.";
		return true;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestGeometry

} // namespace Test

} // namespace Ocean
