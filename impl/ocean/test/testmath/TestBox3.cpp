/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestBox3.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/String.h"

#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestBox3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool result = true;

	Log::info() << "---   Box3 test:   ---";
	Log::info() << " ";

	result = testPoint<float>(testDuration) && result;
	Log::info() << " ";
	result = testPoint<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testIsInside<float>(testDuration) && result;
	Log::info() << " ";
	result = testIsInside<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testAddition<float>(testDuration) && result;
	Log::info() << " ";
	result = testAddition<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testTransformation<float>(testDuration) && result;
	Log::info() << " ";
	result = testTransformation<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testLine<float>(testDuration) && result;
	Log::info() << " ";
	result = testLine<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testPlane<float>(testDuration) && result;
	Log::info() << " ";
	result = testPlane<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testSpace<float>(testDuration) && result;
	Log::info() << " ";
	result = testSpace<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testHasIntersection<float>(testDuration) && result;
	Log::info() << " ";
	result = testHasIntersection<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testExpand<float>(testDuration) && result;
	Log::info() << " ";
	result = testExpand<double>(testDuration) && result;

	Log::info() << " ";

	if (result)
	{
		Log::info() << "Box3 test succeeded.";
	}
	else
	{
		Log::info() << "Box3 test FAILED";
	}

	return result;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBox3, Point_Float)
{
	EXPECT_TRUE((TestBox3::testPoint<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Point_Double)
{
	EXPECT_TRUE((TestBox3::testPoint<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, IsInside_Float)
{
	EXPECT_TRUE((TestBox3::testIsInside<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, IsInside_Double)
{
	EXPECT_TRUE((TestBox3::testIsInside<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, Addition_Float)
{
	EXPECT_TRUE((TestBox3::testAddition<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Addition_Double)
{
	EXPECT_TRUE((TestBox3::testAddition<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, Transformation_Float)
{
	EXPECT_TRUE((TestBox3::testTransformation<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Transformation_Double)
{
	EXPECT_TRUE((TestBox3::testTransformation<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, Line_Float)
{
	EXPECT_TRUE((TestBox3::testLine<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Line_Double)
{
	EXPECT_TRUE((TestBox3::testLine<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, Plane_Float)
{
	EXPECT_TRUE((TestBox3::testPlane<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Plane_Double)
{
	EXPECT_TRUE((TestBox3::testPlane<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, Space_Float)
{
	EXPECT_TRUE((TestBox3::testSpace<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Space_Double)
{
	EXPECT_TRUE((TestBox3::testSpace<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, HasIntersection_Float)
{
	EXPECT_TRUE((TestBox3::testHasIntersection<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, HasIntersection_Double)
{
	EXPECT_TRUE((TestBox3::testHasIntersection<double>(GTEST_TEST_DURATION)));
}


TEST(TestBox3, Expand_Float)
{
	EXPECT_TRUE((TestBox3::testExpand<float>(GTEST_TEST_DURATION)));
}

TEST(TestBox3, Expand_Double)
{
	EXPECT_TRUE((TestBox3::testExpand<double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestBox3::testPoint(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test point with '" << TypeNamer::name<T>() << "':";

	bool result = true;

	const Timestamp startTimestamp(true);

	do
	{
		BoxT3<T> box;

		if (box.isValid())
		{
			result = false;
		}

		box += RandomT<T>::vector3(-10, 10);

		if (!box.isValid())
		{
			result = false;
		}

		VectorT3<T> point;
		if (!box.isPoint(&point))
		{
			result = false;
		}

		PlaneT3<T> plane;
		if (!box.isPlanar(plane))
		{
			result = false;
		}

		VectorT3<T> points[8];
		if (box.corners(points) != 1)
		{
			result = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (result)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return result;
}

template <typename T>
bool TestBox3::testIsInside(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test isInside with '" << TypeNamer::name<T>() << "':";

	bool result = true;

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> lower(RandomT<T>::vector3(-1000, 1000));
		const VectorT3<T> higher(RandomT<T>::scalar(lower.x(), 1000), RandomT<T>::scalar(lower.y(), 1000), RandomT<T>::scalar(lower.z(), 1000));

		const BoxT3<T> box(lower, higher);
		const VectorT3<T> point(RandomT<T>::vector3(-1000, 1000));

		bool testInside = true;

		if (point.x() < box.lower().x())
		{
			testInside = false;
		}
		if (point.y() < box.lower().y())
		{
			testInside = false;
		}
		if (point.z() < box.lower().z())
		{
			testInside = false;
		}

		if (point.x() > box.higher().x())
		{
			testInside = false;
		}
		if (point.y() > box.higher().y())
		{
			testInside = false;
		}
		if (point.z() > box.higher().z())
		{
			testInside = false;
		}

		if (box.isInside(point) != testInside)
		{
			result = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (result)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return result;
}

template <typename T>
bool TestBox3::testAddition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test addition operator with '" << TypeNamer::name<T>() << "':";

	bool succeeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		T x0 = RandomT<T>::scalar(-1000, 1000);
		T y0 = RandomT<T>::scalar(-1000, 1000);
		T z0 = RandomT<T>::scalar(-1000, 1000);

		T x1 = x0 + RandomT<T>::scalar(0, 1000);
		T y1 = y0 + RandomT<T>::scalar(0, 1000);
		T z1 = z0 + RandomT<T>::scalar(0, 1000);

		BoxT3<T> box(VectorT3<T>(x0, y0, z0), VectorT3<T>(x1, y1, z1));

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const T newX0 = RandomT<T>::scalar(-1000, 1000);
			const T newY0 = RandomT<T>::scalar(-1000, 1000);
			const T newZ0 = RandomT<T>::scalar(-1000, 1000);

			const T newX1 = x0 + RandomT<T>::scalar(0, 1000);
			const T newY1 = y0 + RandomT<T>::scalar(0, 1000);
			const T newZ1 = z0 + RandomT<T>::scalar(0, 1000);

			const BoxT3<T> newBox(VectorT3<T>(newX0, newY0, newZ0), VectorT3<T>(newX1, newY1, newZ1));

			box += newBox;

			x0 = min(x0, newX0);
			y0 = min(y0, newY0);
			z0 = min(z0, newZ0);

			x1 = max(x1, newX1);
			y1 = max(y1, newY1);
			z1 = max(z1, newZ1);
		}

		const BoxT3<T> finalBox(VectorT3<T>(x0, y0, z0), VectorT3<T>(x1, y1, z1));

		if (box != finalBox)
		{
			succeeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (succeeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

template <typename T>
bool TestBox3::testTransformation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test box transformation with '" << TypeNamer::name<T>() << "':";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const T x0 = RandomT<T>::scalar(-100, 100);
			const T y0 = RandomT<T>::scalar(-100, 100);
			const T z0 = RandomT<T>::scalar(-100, 100);

			const T x1 = x0 + RandomT<T>::scalar(0, 100);
			const T y1 = y0 + RandomT<T>::scalar(0, 100);
			const T z1 = z0 + RandomT<T>::scalar(0, 100);

			const BoxT3<T> box(VectorT3<T>(x0, y0, z0), VectorT3<T>(x1, y1, z1));
			const HomogenousMatrixT4<T> world_T_box(RandomT<T>::vector3(-10, 10), RandomT<T>::rotation(), RandomT<T>::vector3(T(0.01), 10));
			const BoxT3<T> worldBox(box * world_T_box);

			T lowX = NumericT<T>::maxValue();
			T lowY = NumericT<T>::maxValue();
			T lowZ = NumericT<T>::maxValue();

			T highX = NumericT<T>::minValue();
			T highY = NumericT<T>::minValue();
			T highZ = NumericT<T>::minValue();

			const VectorT3<T> vertices[8] =
			{
				VectorT3<T>(x0, y0, z0),
				VectorT3<T>(x0, y0, z1),
				VectorT3<T>(x0, y1, z0),
				VectorT3<T>(x0, y1, z1),
				VectorT3<T>(x1, y0, z0),
				VectorT3<T>(x1, y0, z1),
				VectorT3<T>(x1, y1, z0),
				VectorT3<T>(x1, y1, z1)
			};

			VectorT3<T> worldVertices[8];

			for (unsigned int i = 0u; i < 8u; ++i)
			{
				worldVertices[i] = world_T_box * vertices[i];
			}

			for (unsigned int i = 0u; i < 8u; ++i)
			{
				lowX = min(lowX, worldVertices[i].x());
				lowY = min(lowY, worldVertices[i].y());
				lowZ = min(lowZ, worldVertices[i].z());

				highX = max(highX, worldVertices[i].x());
				highY = max(highY, worldVertices[i].y());
				highZ = max(highZ, worldVertices[i].z());
			}

			const BoxT3<T> testBox(VectorT3<T>(lowX, lowY, lowZ), VectorT3<T>(highX, highY, highZ));

			if (testBox.isEqual(worldBox, NumericT<T>::weakEps()))
			{
				++validIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <typename T>
bool TestBox3::testLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test box composed of a line with '" << TypeNamer::name<T>() << "':";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int index = 0u; index < 3u; ++index)
		{
			bool result = true;

			BoxT3<T> box;

			const VectorT3<T> first(RandomT<T>::vector3(-100, 100));

			VectorT3<T> second(first);
			second[index] = RandomT<T>::scalar(-100, 100);

			if (first == second)
			{
				continue;
			}

			box += first;
			box += second;

			VectorT3<T> points[8];
			if (box.corners(points) != 2u)
			{
				result = false;
			}

			if ((first != points[0] || second != points[1]) && (first != points[1] || second != points[0]))
			{
				result = false;
			}

			VectorT3<T> point;
			if (box.isPoint(&point))
			{
				result = false;
			}

			PlaneT3<T> plane;
			if (!box.isPlanar(plane))
			{
				result = false;
			}

			if (result)
			{
				++validIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <typename T>
bool TestBox3::testPlane(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test plane with '" << TypeNamer::name<T>() << "':";

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	const T epsilon = std::is_same<float, T>::value ? NumericT<T>::eps() * 100 : NumericT<T>::eps();

	do
	{
		for (unsigned int index = 0u; index < 3u; ++index)
		{
			bool localResult = true;

			BoxT3<T> box;

			const T value = RandomT<T>::scalar(-100, 100);

			for (unsigned int n = 0; n < 1000u; ++n)
			{
				VectorT3<T> point = RandomT<T>::vector3(-100, 100);
				point[index] = value;

				box += point;
			}

			VectorT3<T> points[8];
			if (box.corners(points) != 4)
			{
				localResult = false;
			}

			const T xDim = max(NumericT<T>::abs(points[0].x() - points[1].x()), NumericT<T>::abs(points[0].x() - points[2].x()));
			const T yDim = max(NumericT<T>::abs(points[0].y() - points[1].y()), NumericT<T>::abs(points[0].y() - points[2].y()));
			const T zDim = max(NumericT<T>::abs(points[0].z() - points[1].z()), NumericT<T>::abs(points[0].z() - points[2].z()));

			if (NumericT<T>::isNotEqual(xDim, box.xDimension(), epsilon) || NumericT<T>::isNotEqual(yDim, box.yDimension(), epsilon) || NumericT<T>::isNotEqual(zDim, box.zDimension(), epsilon))
			{
				localResult = false;
			}

			VectorT3<T> point;
			if (box.isPoint(&point))
			{
				localResult = false;
			}

			PlaneT3<T> plane;
			if (!box.isPlanar(plane))
			{
				localResult = false;
			}

			if (localResult)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

template <typename T>
bool TestBox3::testSpace(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test space with '" << TypeNamer::name<T>() << "':";

	uint64_t validIterations = 0u;
	uint64_t iterations = 0u;

	const Timestamp startTimestamp(true);

	const T epsilon = std::is_same<float, T>::value ? NumericT<T>::eps() * 100 : NumericT<T>::eps();

	do
	{
		bool localSucceeded = true;

		BoxT3<T> box;

		for (unsigned int n = 0; n < 1000u; ++n)
		{
			box += RandomT<T>::vector3(-100, 100);
		}

		VectorT3<T> point;
		if (box.isPoint(&point))
		{
			localSucceeded = false;
		}

		PlaneT3<T> plane;
		if (box.isPlanar(plane))
		{
			localSucceeded = false;
		}

		VectorT3<T> points[8];
		if (box.corners(points) != 8)
		{
			localSucceeded = false;
		}

		const T xDim = max(NumericT<T>::abs(points[0].x() - points[1].x()), max(NumericT<T>::abs(points[0].x() - points[2].x()), max(NumericT<T>::abs(points[0].x() - points[3].x()), max(NumericT<T>::abs(points[0].x() - points[4].x()), max(NumericT<T>::abs(points[0].x() - points[5].x()), max(NumericT<T>::abs(points[0].x() - points[6].x()), NumericT<T>::abs(points[0].x() - points[7].x())))))));
		const T yDim = max(NumericT<T>::abs(points[0].y() - points[1].y()), max(NumericT<T>::abs(points[0].y() - points[2].y()), max(NumericT<T>::abs(points[0].y() - points[3].y()), max(NumericT<T>::abs(points[0].y() - points[4].y()), max(NumericT<T>::abs(points[0].y() - points[5].y()), max(NumericT<T>::abs(points[0].y() - points[6].y()), NumericT<T>::abs(points[0].y() - points[7].y())))))));
		const T zDim = max(NumericT<T>::abs(points[0].z() - points[1].z()), max(NumericT<T>::abs(points[0].z() - points[2].z()), max(NumericT<T>::abs(points[0].z() - points[3].z()), max(NumericT<T>::abs(points[0].z() - points[4].z()), max(NumericT<T>::abs(points[0].z() - points[5].z()), max(NumericT<T>::abs(points[0].z() - points[6].z()), NumericT<T>::abs(points[0].z() - points[7].z())))))));

		if (NumericT<T>::isNotEqual(xDim, box.xDimension(), epsilon) || NumericT<T>::isNotEqual(yDim, box.yDimension(), epsilon) || NumericT<T>::isNotEqual(zDim, box.zDimension(), epsilon))
		{
			localSucceeded = false;
		}

		if (localSucceeded)
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <typename T>
bool TestBox3::testHasIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has intersection test with '" << TypeNamer::name<T>() << "':";

	const size_t constIterations = 100000;

	LinesT3<T> lines;
	lines.reserve(constIterations);

	for (size_t n = 0; n < constIterations; ++n)
	{
		const VectorT3<T> position(RandomT<T>::vector3(-100, 100));
		const QuaternionT<T> quaternion(RandomT<T>::quaternion());

		const LineT3<T> line(position, quaternion * VectorT3<T>(0, 0, -1));
		lines.push_back(line);
	}

	unsigned int iterations = 0u;
	unsigned int totalIntersections = 0u;
	const Timestamp startTimestamp(true);

	HighPerformanceTimer timer;

	do
	{
		const VectorT3<T> lower(RandomT<T>::vector3(-2, 0));
		const VectorT3<T> offset(RandomT<T>::vector3(T(0.01), 2));
		const BoxT3<T> box(lower, lower + offset);

		unsigned int intersections = 0u;

		for (const LineT3<T>& line : lines)
		{
			if (box.hasIntersection(line))
			{
				++intersections;
			}
		}

		totalIntersections += intersections;
		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (totalIntersections % 2u == 0u)
	{
		Log::info() << "Average performance: " << timer.yseconds() / double (iterations * constIterations) << "mys";
	}
	else
	{
		Log::info() << "Average performance: " << timer.yseconds() / double (iterations * constIterations) << "mys";
	}

	const bool result = validateHasIntersection<T>(testDuration);

	return result;
}

template <typename T>
bool TestBox3::testExpand(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Expand test:";

	size_t iterations = 0;
	size_t validIterations = 0;

	const Timestamp startTimestamp(true);

	do
	{
		const BoxT3<T> box(RandomT<T>::vector3(-100, 100), RandomT<T>::vector3(-100, 100));
		ocean_assert(box.isValid());

		const VectorT3<T> offsets(RandomT<T>::vector3(-10, 10));

		BoxT3<T> copyBox(box);
		copyBox.expand(offsets);

		const BoxT3<T> expandedBox = box.expanded(offsets);

		if (copyBox.isValid() && expandedBox.isValid())
		{
			if (copyBox == expandedBox)
			{
				if (box.center().isEqual(expandedBox.center(), NumericT<T>::weakEps()))
				{
					const T xExpandedDimension = std::max(T(0), box.xDimension() + offsets.x());
					const T yExpandedDimension = std::max(T(0), box.yDimension() + offsets.y());
					const T zExpandedDimension = std::max(T(0), box.zDimension() + offsets.z());

					if (NumericT<T>::isWeakEqual(expandedBox.xDimension(), xExpandedDimension)
							&& NumericT<T>::isWeakEqual(expandedBox.yDimension(), yExpandedDimension)
							&& NumericT<T>::isWeakEqual(expandedBox.zDimension(), zExpandedDimension))
					{
						++validIterations;
					}
				}
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

template <typename T>
bool TestBox3::validateHasIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	uint64_t iterations = 0ull;
	uint64_t succeeded = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			bool localResult = true;

			const VectorT3<T> lower(RandomT<T>::vector3(-2, 0));
			const BoxT3<T> box(lower, lower + RandomT<T>::vector3(T(0.01), 2));

			{
				const VectorT3<T> position(RandomT<T>::vector3(-10, 10));
				const QuaternionT<T> quaternion(RandomT<T>::quaternion());
				const LineT3<T> line(position, quaternion * VectorT3<T>(0, 0, -1));

				const bool result = box.hasIntersection(line);
				const bool test = hasIntersection(box, line);

				if (result != test)
				{
					localResult = false;
				}
			}

			{
				const VectorT3<T> position(box.center() + RandomT<T>::vector3((box.higher() - box.lower()) * T(0.49)));
				ocean_assert(box.isInside(position));

				const QuaternionT<T> quaternion(RandomT<T>::quaternion());
				const VectorT3<T> direction(quaternion * VectorT3<T>(0, 0, -1));
				const T offset = RandomT<T>::scalar(-100, 100);

				const LineT3<T> line(position + direction * offset, direction);

				const bool result = box.hasIntersection(line);
				const bool test = hasIntersection(box, line);

				ocean_assert((std::is_same<T, float>::value) || result == true);
				ocean_assert((std::is_same<T, float>::value) || test == true);

				if (result != test)
				{
					localResult = false;
				}
			}

			if (localResult)
			{
				succeeded++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

template <typename T>
bool TestBox3::hasIntersection(const BoxT3<T>& box, const LineT3<T>& line)
{
	ocean_assert(box.isValid() && line.isValid());

	VectorT3<T> point;

	const PlaneT3<T> frontPlane(box.higher(), VectorT3<T>(0, 0, 1));
	if (frontPlane.intersection(line, point))
	{
		if (point.x() >= box.lower().x() && point.x() <= box.higher().x()
			&& point.y() >= box.lower().y() && point.y() <= box.higher().y())
		{
			ocean_assert(NumericT<T>::isWeakEqual(point.z(), box.higher().z()));
			return true;
		}
	}

	const PlaneT3<T> backPlane(box.lower(), VectorT3<T>(0, 0, -1));
	if (backPlane.intersection(line, point))
	{
		if (point.x() >= box.lower().x() && point.x() <= box.higher().x()
			&& point.y() >= box.lower().y() && point.y() <= box.higher().y())
		{
			ocean_assert(NumericT<T>::isWeakEqual(point.z(), box.lower().z()));
			return true;
		}
	}

	const PlaneT3<T> rightPlane(box.higher(), VectorT3<T>(1, 0, 0));
	if (rightPlane.intersection(line, point))
	{
		if (point.y() >= box.lower().y() && point.y() <= box.higher().y()
			&& point.z() >= box.lower().z() && point.z() <= box.higher().z())
		{
			ocean_assert(NumericT<T>::isWeakEqual(point.x(), box.higher().x()));
			return true;
		}
	}

	const PlaneT3<T> leftPlane(box.lower(), VectorT3<T>(-1, 0, 0));
	if (leftPlane.intersection(line, point))
	{
		if (point.y() >= box.lower().y() && point.y() <= box.higher().y()
			&& point.z() >= box.lower().z() && point.z() <= box.higher().z())
		{
			ocean_assert(NumericT<T>::isWeakEqual(point.x(), box.lower().x()));
			return true;
		}
	}

	const PlaneT3<T> topPlane(box.higher(), VectorT3<T>(0, 1, 0));
	if (topPlane.intersection(line, point))
	{
		if (point.x() >= box.lower().x() && point.x() <= box.higher().x()
			&& point.z() >= box.lower().z() && point.z() <= box.higher().z())
		{
			ocean_assert(NumericT<T>::isWeakEqual(point.y(), box.higher().y()));
			return true;
		}
	}

	const PlaneT3<T> bottomPlane(box.higher(), VectorT3<T>(0, -1, 0));
	if (bottomPlane.intersection(line, point))
	{
		if (point.x() >= box.lower().x() && point.x() <= box.higher().x()
			&& point.z() >= box.lower().z() && point.z() <= box.higher().z())
		{
			ocean_assert(NumericT<T>::isWeakEqual(point.y(), box.lower().y()));
			return true;
		}
	}

	return false;
}

}

}

}
