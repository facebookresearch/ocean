/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestTransformation.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/math/Random.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/video/tracking.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool TestTransformation::testTransformation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Transformation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSimilarityMatrix<Scalar>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAffineMatrix<Scalar>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomographyMatrix<Scalar>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Transformation test succeeded.";
	}
	else
	{
		Log::info() << "Transformation test FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestTransformation::testSimilarityMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of similarity matrix with " << sizeof(T) * 8 << "bit floating point precision:";

	bool allSucceeded = true;

	allSucceeded = testSimilarityMatrix<T>(testDuration, 100) && allSucceeded;

	allSucceeded = testSimilarityMatrix<T>(testDuration, 1000) && allSucceeded;

	allSucceeded = testSimilarityMatrix<T>(testDuration, 10000) && allSucceeded;

	allSucceeded = testSimilarityMatrix<T>(testDuration, 100000) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Similarity matrix test succeeded.";
	}
	else
	{
		Log::info() << "Similartiy matrix test FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestTransformation::testSimilarityMatrix(const double testDuration, const size_t points)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << points << " points:";

	const unsigned int width  = 1920u;
	const unsigned int height = 1080u;

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 xAxis(Random::vector2(randomGenerator));
		const Vector2 yAxis(xAxis.perpendicular());

		const Scalar scale(Random::scalar(randomGenerator, -2, 2));
		const Vector2 translation(Random::vector2(randomGenerator, -10, 10));

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 rightTleft(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n]  = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			pointsRight[n] = rightTleft * pointsLeft[n];

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
		}

		performance.start();
			const cv::Mat similarity = cv::estimateRigidTransform(ocean2opencv<T>(pointsLeft), ocean2opencv<T>(pointsRightNoised), false);
		performance.stop();

		if (!similarity.empty())
		{
			bool localSucceeded = true;

			const SquareMatrix3 similarityOcean(opencv2ocean_affine<T>(similarity));

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 transformedPoint = similarityOcean * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], 1))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				validIterations++;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return percent >= 0.99;
}

template <typename T>
bool TestTransformation::testAffineMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of affine matrix with " << sizeof(T) * 8 << "bit floating point precision:";

	bool allSucceeded = true;

	allSucceeded = testAffineMatrix<T>(testDuration, 100) && allSucceeded;

	allSucceeded = testAffineMatrix<T>(testDuration, 1000) && allSucceeded;

	allSucceeded = testAffineMatrix<T>(testDuration, 10000) && allSucceeded;

	allSucceeded = testAffineMatrix<T>(testDuration, 100000) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Affine matrix test succeeded.";
	}
	else
	{
		Log::info() << "Affine matrix test FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestTransformation::testAffineMatrix(const double testDuration, const size_t points)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << points << " points:";

	const unsigned int width  = 1920u;
	const unsigned int height = 1080u;

	bool allSucceeded = true;

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 xAxis(Random::vector2(randomGenerator));
		const Vector2 yAxis(xAxis.perpendicular());

		const Scalar scale(Random::scalar(randomGenerator, -2, 2));
		const Vector2 translation(Random::vector2(randomGenerator, -10, 10));

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 rightTleft(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n]  = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			pointsRight[n] = rightTleft * pointsLeft[n];

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
		}

		performance.start();
			const cv::Mat affine = cv::estimateRigidTransform(ocean2opencv<T>(pointsLeft), ocean2opencv<T>(pointsRightNoised), true);
		performance.stop();

		if (!affine.empty())
		{
			bool localSucceeded = true;

			const SquareMatrix3 affineOcean(opencv2ocean_affine<T>(affine));

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 transformedPoint = affineOcean * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], 1))
				{
					allSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				validIterations++;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return allSucceeded && percent >= 0.99;
}

template <typename T>
bool TestTransformation::testHomographyMatrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing determination of homography matrix with " << sizeof(T) * 8 << "bit floating point precision:";

	bool allSucceeded = true;

	allSucceeded = testHomographyMatrix<T>(testDuration, 100) && allSucceeded;

	allSucceeded = testHomographyMatrix<T>(testDuration, 1000) && allSucceeded;

	allSucceeded = testHomographyMatrix<T>(testDuration, 10000) && allSucceeded;

	allSucceeded = testHomographyMatrix<T>(testDuration, 100000) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Homography test succeeded.";
	}
	else
	{
		Log::info() << "Homography test FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestTransformation::testHomographyMatrix(const double testDuration, const size_t points)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << points << " points:";

	const unsigned int width  = 1920u;
	const unsigned int height = 1080u;

	Vectors2 pointsLeft(points);
	Vectors2 pointsRight(points);
	Vectors2 pointsRightNoised(points);

	RandomGenerator randomGenerator;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		const Vector2 xAxis(Random::vector2(randomGenerator));
		const Vector2 yAxis(xAxis.perpendicular());

		const Scalar scale(Random::scalar(randomGenerator, -2, 2));
		const Vector2 translation(Random::vector2(randomGenerator, -10, 10));

		// transformation transforming left points to right points (right = T * left)
		const SquareMatrix3 rightTleft(Vector3(xAxis * scale, 0), Vector3(yAxis * scale, 0), Vector3(translation, 1));

		for (size_t n = 0; n < points; ++n)
		{
			pointsLeft[n]  = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
			pointsRight[n] = rightTleft * pointsLeft[n];

			pointsRightNoised[n] = pointsRight[n] + Vector2(Random::gaussianNoise(0.5), Random::gaussianNoise(0.5));
		}

		performance.start();
			const cv::Mat homography = cv::findHomography(ocean2opencv<T>(pointsLeft), ocean2opencv<T>(pointsRightNoised));
		performance.stop();

		if (!homography.empty())
		{
			bool localSucceeded = true;

			const SquareMatrix3 homographyOcean(opencv2ocean_homography<T>(homography));

			for (size_t n = 0; n < points; ++n)
			{
				const Vector2 transformedPoint = homographyOcean * pointsLeft[n];
				if (!transformedPoint.isEqual(pointsRight[n], 1))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				validIterations++;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << " ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << " % succeeded.";

	return percent >= 0.99;
}

template <typename T>
std::vector<cv::Point_<T>> TestTransformation::ocean2opencv(const Vectors2& vectors)
{
	std::vector<cv::Point_<T>> result;
	result.reserve(vectors.size());

	for (size_t n = 0; n < vectors.size(); ++n)
	{
		result.push_back(cv::Point_<T>(T(vectors[n].x()), T(vectors[n].y())));
	}

	return result;
}

template <typename T>
SquareMatrix3 TestTransformation::opencv2ocean(const cv::Mat& matrix)
{
	ocean_assert(matrix.rows == 3 && matrix.cols == 3);

	SquareMatrix3 result;

	for (unsigned int r = 0u; r < 3u; ++r)
	{
		for (unsigned int c = 0u; c < 3u; ++c)
		{
			result(c, r) = Scalar(matrix.at<T>(r, c));
		}
	}

	return result;
}

template <typename T>
SquareMatrix3 TestTransformation::opencv2ocean_homography(const cv::Mat& matrix)
{
	ocean_assert(matrix.rows == 3 && matrix.cols == 3);

	SquareMatrix3 result;

	for (unsigned int r = 0u; r < 3u; ++r)
	{
		for (unsigned int c = 0u; c < 3u; ++c)
		{
			result(r, c) = Scalar(matrix.at<T>(r, c));
		}
	}

	result(2, 0) = 0;
	result(2, 1) = 0;

	return result;
}

template <typename T>
SquareMatrix3 TestTransformation::opencv2ocean_affine(const cv::Mat& matrix)
{
	ocean_assert(matrix.rows == 2 && matrix.cols == 3);
	SquareMatrix3 result;

	for (unsigned int r = 0u; r < 2u; ++r)
	{
		for (unsigned int c = 0u; c < 3u; ++c)
		{
			result(r, c) = Scalar(matrix.at<T>(r, c));
		}
	}

	result(2, 0) = 0;
	result(2, 1) = 0;
	result(2, 2) = 1;

	return result;
}

}

}

}

}
