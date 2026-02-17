/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestBox2.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Box2.h"
#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestBox2::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Box2 test");

	Log::info() << " ";

	if (selector.shouldRun("constructors"))
	{
		testResult = testConstructors<float>(testDuration);
		Log::info() << " ";
		testResult = testConstructors<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("intersects"))
	{
		testResult = testIntersects<float>(testDuration);
		Log::info() << " ";
		testResult = testIntersects<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("unsignedbox2integer"))
	{
		testResult = testUnsignedBox2integer<float>(testDuration);
		Log::info() << " ";
		testResult = testUnsignedBox2integer<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("signedbox2integer"))
	{
		testResult = testSignedBox2integer<float>(testDuration);
		Log::info() << " ";
		testResult = testSignedBox2integer<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("multiplicationoperators"))
	{
		testResult = testMultiplicationOperators<float>(testDuration);
		Log::info() << " ";
		testResult = testMultiplicationOperators<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestBox2, Constructors_float)
{
	EXPECT_TRUE(TestBox2::testConstructors<float>(GTEST_TEST_DURATION));
}

TEST(TestBox2, Constructors_double)
{
	EXPECT_TRUE(TestBox2::testConstructors<double>(GTEST_TEST_DURATION));
}


TEST(TestBox2, Intersects_float)
{
	EXPECT_TRUE(TestBox2::testIntersects<float>(GTEST_TEST_DURATION));
}

TEST(TestBox2, Intersects_double)
{
	EXPECT_TRUE(TestBox2::testIntersects<double>(GTEST_TEST_DURATION));
}


TEST(TestBox2, UnsignedBox2integer_float)
{
	EXPECT_TRUE(TestBox2::testUnsignedBox2integer<float>(GTEST_TEST_DURATION));
}

TEST(TestBox2, UnsignedBox2integer_double)
{
	EXPECT_TRUE(TestBox2::testUnsignedBox2integer<double>(GTEST_TEST_DURATION));
}


TEST(TestBox2, SignedBox2integer_float)
{
	EXPECT_TRUE(TestBox2::testSignedBox2integer<float>(GTEST_TEST_DURATION));
}

TEST(TestBox2, SignedBox2integer_double)
{
	EXPECT_TRUE(TestBox2::testSignedBox2integer<double>(GTEST_TEST_DURATION));
}


TEST(TestBox2, MultiplicationOperators_float)
{
	EXPECT_TRUE(TestBox2::testMultiplicationOperators<float>(GTEST_TEST_DURATION));
}

TEST(TestBox2, MultiplicationOperators_double)
{
	EXPECT_TRUE(TestBox2::testMultiplicationOperators<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestBox2::testConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructors test with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	const T coordinateRange = std::is_same<T, float>::value ? T(5) : T(500);
	const T dimensionRange = std::is_same<T, float>::value ? T(10) : T(1000);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const T left = RandomT<T>::scalar(randomGenerator, -coordinateRange, coordinateRange);
			const T top = RandomT<T>::scalar(randomGenerator, -coordinateRange, coordinateRange);

			const T width = RandomT<T>::scalar(randomGenerator, 0, dimensionRange);
			const T height = RandomT<T>::scalar(randomGenerator, 0, dimensionRange);

			const T right = left + width;
			const T bottom = top + height;

			ocean_assert(right >= left);
			ocean_assert(bottom >= top);

			const T centerX = left + (right - left) * T(0.5);
			const T centerY = top + (bottom - top) * T(0.5);

			// bounding box based on a top/left corner and width and height

			const BoxT2<T> boxTopLeftWidthHeight(width, height, VectorT2<T>(left, top));

			OCEAN_EXPECT_TRUE(validation, boxTopLeftWidthHeight.isValid());
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxTopLeftWidthHeight.left(), left));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxTopLeftWidthHeight.top(), top));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxTopLeftWidthHeight.right(), right));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxTopLeftWidthHeight.bottom(), bottom));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxTopLeftWidthHeight.width(), width));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxTopLeftWidthHeight.height(), height));
			OCEAN_EXPECT_EQUAL(validation, boxTopLeftWidthHeight.center(), VectorT2<T>(centerX, centerY));

			// bounding box based on left, top, right, and bottom coordinates

			const BoxT2<T> boxLeftTopRightBottom(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boxLeftTopRightBottom.isValid());
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxLeftTopRightBottom.left(), left));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxLeftTopRightBottom.top(), top));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxLeftTopRightBottom.right(), right));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxLeftTopRightBottom.bottom(), bottom));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxLeftTopRightBottom.width(), width));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxLeftTopRightBottom.height(), height));
			OCEAN_EXPECT_EQUAL(validation, boxLeftTopRightBottom.center(), VectorT2<T>(centerX, centerY));

			// bounding box based on the box's center and width and height

			const BoxT2<T> boxCenterWithHeight(VectorT2<T>(centerX, centerY), width, height);

			OCEAN_EXPECT_TRUE(validation, boxCenterWithHeight.isValid());
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxCenterWithHeight.left(), left));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxCenterWithHeight.top(), top));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxCenterWithHeight.right(), right));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxCenterWithHeight.bottom(), bottom));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxCenterWithHeight.width(), width));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(boxCenterWithHeight.height(), height));
			OCEAN_EXPECT_EQUAL(validation, boxCenterWithHeight.center(), VectorT2<T>(centerX, centerY));

			OCEAN_EXPECT_EQUAL(validation, boxTopLeftWidthHeight, boxLeftTopRightBottom);
			OCEAN_EXPECT_EQUAL(validation, boxTopLeftWidthHeight, boxCenterWithHeight);
			OCEAN_EXPECT_EQUAL(validation, boxLeftTopRightBottom, boxCenterWithHeight);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestBox2::testIntersects(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersection test with two boxes with " << TypeNamer::name<T>() << ":";

	const T epsilon = std::is_same<float, T>::value ? T(0.001) : NumericT<T>::eps();

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const T boxLeft0 = RandomT<T>::scalar(randomGenerator, -500, 1000);
		const T boxTop0 = RandomT<T>::scalar(randomGenerator, -500, 1000);

		const T boxRight0 = RandomT<T>::scalar(randomGenerator, boxLeft0 + T(0.01), 1500);
		const T boxBottom0 = RandomT<T>::scalar(randomGenerator, boxTop0 + T(0.01), 1500);

		const T boxLeft1 = RandomT<T>::scalar(randomGenerator, -500, 1000);
		const T boxTop1 = RandomT<T>::scalar(randomGenerator, -500, 1000);

		const T boxRight1 = RandomT<T>::scalar(randomGenerator, boxLeft1 + T(0.01), 1500);
		const T boxBottom1 = RandomT<T>::scalar(randomGenerator, boxTop1 + T(0.01), 1500);

		const BoxT2<T> box0(boxLeft0, boxTop0, boxRight0, boxBottom0);
		const BoxT2<T> box1(boxLeft1, boxTop1, boxRight1, boxBottom1);

		const bool result = box0.intersects(box1);

		const T centerDistanceX = NumericT<T>::abs(box0.center().x() - box1.center().x());
		const T centerDistanceY = NumericT<T>::abs(box0.center().y() - box1.center().y());

		const bool test = (box0.width() + box1.width()) * T(0.5) >= centerDistanceX
							&& (box0.height() + box1.height()) * T(0.5) >= centerDistanceY;

		bool testEdges = box0.isInside(box1) || box1.isInside(box0);

		const FiniteLinesT2<T> lines0 =
		{
			FiniteLineT2<T>(VectorT2<T>(boxLeft0, boxTop0), VectorT2<T>(boxLeft0, boxBottom0)),
			FiniteLineT2<T>(VectorT2<T>(boxLeft0, boxBottom0), VectorT2<T>(boxRight0, boxBottom0)),
			FiniteLineT2<T>(VectorT2<T>(boxRight0, boxBottom0), VectorT2<T>(boxRight0, boxTop0)),
			FiniteLineT2<T>(VectorT2<T>(boxRight0, boxTop0), VectorT2<T>(boxLeft0, boxTop0))
		};

		const FiniteLinesT2<T> lines1 =
		{
			FiniteLineT2<T>(VectorT2<T>(boxLeft1, boxTop1), VectorT2<T>(boxLeft1, boxBottom1)),
			FiniteLineT2<T>(VectorT2<T>(boxLeft1, boxBottom1), VectorT2<T>(boxRight1, boxBottom1)),
			FiniteLineT2<T>(VectorT2<T>(boxRight1, boxBottom1), VectorT2<T>(boxRight1, boxTop1)),
			FiniteLineT2<T>(VectorT2<T>(boxRight1, boxTop1), VectorT2<T>(boxLeft1, boxTop1))
		};

		VectorT2<T> intersectionPoint;
		for (size_t a = 0; !testEdges && a < lines0.size(); ++a)
		{
			for (size_t b = 0; !testEdges && b < lines1.size(); ++b)
			{
				if (lines0[a].intersection(lines1[b], intersectionPoint))
				{
					ocean_assert(lines0[a].isOnLine(intersectionPoint));
					ocean_assert(lines1[b].isOnLine(intersectionPoint));

					ocean_assert_and_suppress_unused(box0.isInside(intersectionPoint, epsilon), epsilon);
					ocean_assert_and_suppress_unused(box1.isInside(intersectionPoint, epsilon), epsilon);

					testEdges = true;
				}
			}
		}

		ocean_assert(test == testEdges);

		if (result != test)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestBox2::testUnsignedBox2integer(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Unsigned box2integer test with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1000u);

		const T boxLeft = RandomT<T>::scalar(randomGenerator, -500, 1000);
		const T boxTop = RandomT<T>::scalar(randomGenerator, -500, 1000);

		const T boxRight = RandomT<T>::scalar(randomGenerator, boxLeft, 2500);
		const T boxBottom = RandomT<T>::scalar(randomGenerator, boxTop, 2500);

		ocean_assert(boxRight >= boxLeft);
		ocean_assert(boxBottom >= boxTop);

		const BoxT2<T> box(boxLeft, boxTop, boxRight, boxBottom);

		unsigned int testLeft, testTop, testWidth, testHeight;

		// check whether the box lies outside the area
		if (boxRight < T(0) || boxBottom < T(0) || boxLeft >= T(width) || boxTop >= T(height))
		{
			if (box.box2integer(width, height, testLeft, testTop, testWidth, testHeight))
			{
				OCEAN_SET_FAILED(validation);
				continue;
			}
		}
		else
		{
			if (!box.box2integer(width, height, testLeft, testTop, testWidth, testHeight))
			{
				OCEAN_SET_FAILED(validation);
				continue;
			}

			const unsigned int left = boxLeft < T(0) ? 0u : (unsigned int)(boxLeft);
			ocean_assert(left < width);

			const unsigned int top = boxTop < T(0) ? 0u : (unsigned int)(boxTop);
			ocean_assert(top < height);

			ocean_assert(boxRight >= T(0));
			const unsigned int right = min((unsigned int)(boxRight), width - 1u);

			ocean_assert(boxBottom >= T(0));
			const unsigned int bottom = min((unsigned int)(boxBottom), height - 1u);

			if (left != testLeft || top != testTop || testWidth != (right - left + 1u) || testHeight != (bottom - top + 1u))
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestBox2::testSignedBox2integer(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Signed box2integer test with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const int areaLeft = RandomI::random(randomGenerator, -1000, 1000);
		const int areaTop = RandomI::random(randomGenerator, -1000, 1000);

		const int areaRight = RandomI::random(randomGenerator, areaLeft + 1, 2000);
		const int areaBottom = RandomI::random(randomGenerator, areaTop + 1, 2000);

		const T boxLeft = RandomT<T>::scalar(randomGenerator, -1500, 2000);
		const T boxTop = RandomT<T>::scalar(randomGenerator, -1500, 2000);

		const T boxRight = RandomT<T>::scalar(randomGenerator, boxLeft, 2500);
		const T boxBottom = RandomT<T>::scalar(randomGenerator, boxTop, 2500);

		ocean_assert(boxRight >= boxLeft);
		ocean_assert(boxBottom >= boxTop);

		const BoxT2<T> box(boxLeft, boxTop, boxRight, boxBottom);

		int testLeft, testTop;
		unsigned int testWidth, testHeight;

		// check whether the box lies outside the area
		if (boxRight < T(areaLeft) || boxBottom < T(areaTop) || boxLeft > T(areaRight) || boxTop > T(areaBottom))
		{
			if (box.box2integer(areaLeft, areaTop, areaRight, areaBottom, testLeft, testTop, testWidth, testHeight))
			{
				OCEAN_SET_FAILED(validation);
				continue;
			}
		}
		else
		{
			if (!box.box2integer(areaLeft, areaTop, areaRight, areaBottom, testLeft, testTop, testWidth, testHeight))
			{
				OCEAN_SET_FAILED(validation);
				continue;
			}

			int left, top, right, bottom;

			if (boxLeft < T(areaLeft))
			{
				left = areaLeft;
			}
			else
			{
				if (boxLeft < T(0))
				{
					if (T(int(boxLeft)) == boxLeft)
					{
						left = int(boxLeft);
					}
					else
					{
						left = int(boxLeft) - 1;
					}
				}
				else
				{
					left = int(boxLeft);
				}
			}

			if (boxTop < T(areaTop))
			{
				top = areaTop;
			}
			else
			{
				if (boxTop < T(0))
				{
					if (T(int(boxTop)) == boxTop)
					{
						top = int(boxTop);
					}
					else
					{
						top = int(boxTop) - 1;
					}
				}
				else
				{
					top = int(boxTop);
				}
			}

			if (boxRight > T(areaRight))
			{
				right = areaRight;
			}
			else
			{
				if (boxRight < T(0))
				{
					if (T(int(boxRight)) == boxRight)
					{
						right = int(boxRight);
					}
					else
					{
						right = int(boxRight) - 1;
					}
				}
				else
				{
					right = int(boxRight);
				}
			}

			if (boxBottom > T(areaBottom))
			{
				bottom = areaBottom;
			}
			else
			{
				if (boxBottom < T(0))
				{
					if (T(int(boxBottom)) == boxBottom)
					{
						bottom = int(boxBottom);
					}
					else
					{
						bottom = int(boxBottom) - 1;
					}
				}
				else
				{
					bottom = int(boxBottom);
				}
			}

			if (left != testLeft || top != testTop || testWidth != (right - left + 1u) || testHeight != (bottom - top + 1u))
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestBox2::testMultiplicationOperators(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Signed multiplication operators test with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const T width = RandomT<T>::scalar(randomGenerator, T(0.1), 10);
		const T height = RandomT<T>::scalar(randomGenerator, T(0.1), 10);

		const T centerX = RandomT<T>::scalar(randomGenerator, -10, 10);
		const T centerY = RandomT<T>::scalar(randomGenerator, -10, 10);

		const BoxT2<T> sourceBox(VectorT2<T>(centerX, centerY), width, height);

		ocean_assert(sourceBox.isValid());
		OCEAN_EXPECT_TRUE(validation, sourceBox.isValid());

		{
			// testing scalar multiplication factor

			const T scalarFactor = RandomT<T>::scalar(randomGenerator, -10, 10);

			BoxT2<T> copySourceBox(sourceBox);

			const BoxT2<T> targetBox = sourceBox * scalarFactor;
			copySourceBox *= scalarFactor;

			OCEAN_EXPECT_EQUAL(validation, targetBox, copySourceBox);

			T newLeft = sourceBox.left() * scalarFactor;
			T newRight = sourceBox.right() * scalarFactor;
			Utilities::sortLowestToFront2(newLeft, newRight);

			T newTop = sourceBox.top() * scalarFactor;
			T newBottom = sourceBox.bottom() * scalarFactor;
			Utilities::sortLowestToFront2(newTop, newBottom);

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newLeft, targetBox.left()));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newRight, targetBox.right()));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newTop, targetBox.top()));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newBottom, targetBox.bottom()));
		}

		{
			// testing matrix multiplication factor

			const QuaternionT<T> rotation(VectorT3<T>(0, 0, 1), RandomT<T>::scalar(randomGenerator, 0, NumericT<T>::pi2()));
			const VectorT3<T> xAxis((rotation * VectorT3<T>(1, 0, 0)).xy(), 0);
			const VectorT3<T> yAxis((rotation * VectorT3<T>(0, 1, 0)).xy(), 0);
			const VectorT3<T> zAxis(RandomT<T>::vector2(randomGenerator, -10, 10), 1);

			const SquareMatrixT3<T> transformation = SquareMatrixT3<T>(xAxis, yAxis, zAxis);
			ocean_assert(!transformation.isSingular());

			BoxT2<T> copySourceBox(sourceBox);

			const BoxT2<T> targetBox = sourceBox * transformation;
			copySourceBox *= transformation;

			OCEAN_EXPECT_EQUAL(validation, targetBox, copySourceBox);

			const VectorT2<T> transformedTopLeft = transformation * VectorT2<T>(sourceBox.left(), sourceBox.top());
			const VectorT2<T> transformedTopRight = transformation * VectorT2<T>(sourceBox.right(), sourceBox.top());
			const VectorT2<T> transformedBottomRight = transformation * VectorT2<T>(sourceBox.right(), sourceBox.bottom());
			const VectorT2<T> transformedBottomLeft = transformation * VectorT2<T>(sourceBox.left(), sourceBox.bottom());

			const T newLeft = std::min(std::min(transformedTopLeft.x(), transformedTopRight.x()), std::min(transformedBottomLeft.x(), transformedBottomRight.x()));
			const T newRight = std::max(std::max(transformedTopLeft.x(), transformedTopRight.x()), std::max(transformedBottomLeft.x(), transformedBottomRight.x()));

			const T newTop = std::min(std::min(transformedTopLeft.y(), transformedTopRight.y()), std::min(transformedBottomLeft.y(), transformedBottomRight.y()));
			const T newBottom = std::max(std::max(transformedTopLeft.y(), transformedTopRight.y()), std::max(transformedBottomLeft.y(), transformedBottomRight.y()));

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newLeft, targetBox.left()));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newRight, targetBox.right()));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newTop, targetBox.top()));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(newBottom, targetBox.bottom()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();


}

}

}

}
