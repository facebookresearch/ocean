// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testmath/TestBox2.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Box2.h"
#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestBox2::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Box2 test:   ---";
	Log::info() << " ";

	allSucceeded = testConsturctors(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIntersects(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testUnsignedBox2integer(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSignedBox2integer(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testMultiplicationOperators(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Box2 test succeeded.";
	}
	else
	{
		Log::info() << "Box2 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBox2, Consturctor)
{
	EXPECT_TRUE(TestBox2::testConsturctors(GTEST_TEST_DURATION));
}

TEST(TestBox2, Intersects)
{
	EXPECT_TRUE(TestBox2::testIntersects(GTEST_TEST_DURATION));
}

TEST(TestBox2, UnsignedBox2integer)
{
	EXPECT_TRUE(TestBox2::testUnsignedBox2integer(GTEST_TEST_DURATION));
}

TEST(TestBox2, SignedBox2integer)
{
	EXPECT_TRUE(TestBox2::testSignedBox2integer(GTEST_TEST_DURATION));
}

TEST(TestBox2, MultiplicationOperators)
{
	EXPECT_TRUE(TestBox2::testMultiplicationOperators(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBox2::testConsturctors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructors test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	const Scalar coordinateRange = std::is_same<Scalar, float>::value ? Scalar(5) : Scalar(500);
	const Scalar dimensionRange = std::is_same<Scalar, float>::value ? Scalar(10) : Scalar(1000);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar left = Random::scalar(-coordinateRange, coordinateRange);
			const Scalar top = Random::scalar(-coordinateRange, coordinateRange);

			const Scalar width = Random::scalar(0, dimensionRange);
			const Scalar height = Random::scalar(0, dimensionRange);

			const Scalar right = left + width;
			const Scalar bottom = top + height;

			ocean_assert(right >= left);
			ocean_assert(bottom >= top);

			const Scalar centerX = left + (right - left) * Scalar(0.5);
			const Scalar centerY = top + (bottom - top) * Scalar(0.5);

			// bounding box based on a top/left corner and width and height

			const Box2 boxTopLeftWidthHeight(width, height, Vector2(left, top));

			if (!boxTopLeftWidthHeight.isValid()
					|| Numeric::isNotEqual(boxTopLeftWidthHeight.left(), left)
					|| Numeric::isNotEqual(boxTopLeftWidthHeight.top(), top)
					|| Numeric::isNotEqual(boxTopLeftWidthHeight.right(), right)
					|| Numeric::isNotEqual(boxTopLeftWidthHeight.bottom(), bottom)
					|| Numeric::isNotEqual(boxTopLeftWidthHeight.width(), width)
					|| Numeric::isNotEqual(boxTopLeftWidthHeight.height(), height)
					|| boxTopLeftWidthHeight.center() != Vector2(centerX, centerY))
			{
				allSucceeded = false;
			}

			// bounding box based on left, top, right, and bottom coordinates

			const Box2 boxLeftTopRightBottom(left, top, right, bottom);

			if (!boxLeftTopRightBottom.isValid()
				|| Numeric::isNotEqual(boxLeftTopRightBottom.left(), left)
				|| Numeric::isNotEqual(boxLeftTopRightBottom.top(), top)
				|| Numeric::isNotEqual(boxLeftTopRightBottom.right(), right)
				|| Numeric::isNotEqual(boxLeftTopRightBottom.bottom(), bottom)
				|| Numeric::isNotEqual(boxLeftTopRightBottom.width(), width)
				|| Numeric::isNotEqual(boxLeftTopRightBottom.height(), height)
				|| boxLeftTopRightBottom.center() != Vector2(centerX, centerY))
			{
				allSucceeded = false;
			}

			// bounding box based on the box's center and width and height

			const Box2 boxCenterWithHeight(Vector2(centerX, centerY), width, height);

			if (!boxCenterWithHeight.isValid()
				|| Numeric::isNotEqual(boxCenterWithHeight.left(), left)
				|| Numeric::isNotEqual(boxCenterWithHeight.top(), top)
				|| Numeric::isNotEqual(boxCenterWithHeight.right(), right)
				|| Numeric::isNotEqual(boxCenterWithHeight.bottom(), bottom)
				|| Numeric::isNotEqual(boxCenterWithHeight.width(), width)
				|| Numeric::isNotEqual(boxCenterWithHeight.height(), height)
				|| boxCenterWithHeight.center() != Vector2(centerX, centerY))
			{
				allSucceeded = false;
			}

			if (boxTopLeftWidthHeight != boxLeftTopRightBottom
					|| boxTopLeftWidthHeight != boxCenterWithHeight
					|| boxLeftTopRightBottom != boxCenterWithHeight)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestBox2::testIntersects(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersection test with two boxes:";

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.001) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar boxLeft0 = Random::scalar(-500, 1000);
		const Scalar boxTop0 = Random::scalar(-500, 1000);

		const Scalar boxRight0 = Random::scalar(boxLeft0 + Scalar(0.01), 1500);
		const Scalar boxBottom0 = Random::scalar(boxTop0 + Scalar(0.01), 1500);

		const Scalar boxLeft1 = Random::scalar(-500, 1000);
		const Scalar boxTop1 = Random::scalar(-500, 1000);

		const Scalar boxRight1 = Random::scalar(boxLeft1 + Scalar(0.01), 1500);
		const Scalar boxBottom1 = Random::scalar(boxTop1 + Scalar(0.01), 1500);

		const Box2 box0(boxLeft0, boxTop0, boxRight0, boxBottom0);
		const Box2 box1(boxLeft1, boxTop1, boxRight1, boxBottom1);

		const bool result = box0.intersects(box1);

		const Scalar centerDistanceX = Numeric::abs(box0.center().x() - box1.center().x());
		const Scalar centerDistanceY = Numeric::abs(box0.center().y() - box1.center().y());

		const bool test = (box0.width() + box1.width()) * Scalar(0.5) >= centerDistanceX
							&& (box0.height() + box1.height()) * Scalar(0.5) >= centerDistanceY;

		bool testEdges = box0.isInside(box1) || box1.isInside(box0);

		const FiniteLine2 lines0[4] =
		{
			FiniteLine2(Vector2(boxLeft0, boxTop0), Vector2(boxLeft0, boxBottom0)),
			FiniteLine2(Vector2(boxLeft0, boxBottom0), Vector2(boxRight0, boxBottom0)),
			FiniteLine2(Vector2(boxRight0, boxBottom0), Vector2(boxRight0, boxTop0)),
			FiniteLine2(Vector2(boxRight0, boxTop0), Vector2(boxLeft0, boxTop0))
		};

		const FiniteLine2 lines1[4] =
		{
			FiniteLine2(Vector2(boxLeft1, boxTop1), Vector2(boxLeft1, boxBottom1)),
			FiniteLine2(Vector2(boxLeft1, boxBottom1), Vector2(boxRight1, boxBottom1)),
			FiniteLine2(Vector2(boxRight1, boxBottom1), Vector2(boxRight1, boxTop1)),
			FiniteLine2(Vector2(boxRight1, boxTop1), Vector2(boxLeft1, boxTop1))
		};

		Vector2 intersectionPoint;
		for (unsigned int a = 0u; !testEdges && a < 4u; ++a)
		{
			for (unsigned int b = 0u; !testEdges && b < 4u; ++b)
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

		if (result == test)
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

bool TestBox2::testUnsignedBox2integer(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Unsigned box2integer test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(1u, 1000u);
		const unsigned int height = RandomI::random(1u, 1000u);

		const Scalar boxLeft = Random::scalar(-500, 1000);
		const Scalar boxTop = Random::scalar(-500, 1000);

		const Scalar boxRight = Random::scalar(boxLeft, 2500);
		const Scalar boxBottom = Random::scalar(boxTop, 2500);

		ocean_assert(boxRight >= boxLeft);
		ocean_assert(boxBottom >= boxTop);

		const Box2 box(boxLeft, boxTop, boxRight, boxBottom);

		unsigned int testLeft, testTop, testWidth, testHeight;

		// check whether the box lies outside the area
		if (boxRight < 0 || boxBottom < 0 || boxLeft >= Scalar(width) || boxTop >= Scalar(height))
		{
			if (box.box2integer(width, height, testLeft, testTop, testWidth, testHeight))
			{
				allSucceeded = false;
				continue;
			}
		}
		else
		{
			if (!box.box2integer(width, height, testLeft, testTop, testWidth, testHeight))
			{
				allSucceeded = false;
				continue;
			}

			const unsigned int left = boxLeft < 0 ? 0u : (unsigned int)boxLeft;
			ocean_assert(left < width);

			const unsigned int top = boxTop < 0 ? 0u : (unsigned int)boxTop;
			ocean_assert(top < height);

			ocean_assert(boxRight >= 0);
			const unsigned int right = min((unsigned int)boxRight, width - 1u);

			ocean_assert(boxBottom >= 0);
			const unsigned int bottom = min((unsigned int)boxBottom, height - 1u);

			if (left != testLeft || top != testTop || testWidth != (right - left + 1u) || testHeight != (bottom - top + 1u))
				allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestBox2::testSignedBox2integer(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Signed box2integer test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const int areaLeft = RandomI::random(-1000, 1000);
		const int areaTop = RandomI::random(-1000, 1000);

		const int areaRight = RandomI::random(areaLeft + 1, 2000);
		const int areaBottom = RandomI::random(areaTop + 1, 2000);

		const Scalar boxLeft = Random::scalar(-1500, 2000);
		const Scalar boxTop = Random::scalar(-1500, 2000);

		const Scalar boxRight = Random::scalar(boxLeft, 2500);
		const Scalar boxBottom = Random::scalar(boxTop, 2500);

		ocean_assert(boxRight >= boxLeft);
		ocean_assert(boxBottom >= boxTop);

		const Box2 box(boxLeft, boxTop, boxRight, boxBottom);

		int testLeft, testTop;
		unsigned int testWidth, testHeight;

		// check whether the box lies outside the area
		if (boxRight < Scalar(areaLeft) || boxBottom < Scalar(areaTop) || boxLeft > Scalar(areaRight) || boxTop > Scalar(areaBottom))
		{
			if (box.box2integer(areaLeft, areaTop, areaRight, areaBottom, testLeft, testTop, testWidth, testHeight))
			{
				allSucceeded = false;
				continue;
			}
		}
		else
		{
			if (!box.box2integer(areaLeft, areaTop, areaRight, areaBottom, testLeft, testTop, testWidth, testHeight))
			{
				allSucceeded = false;
				continue;
			}

			int left, top, right, bottom;

			if (boxLeft < Scalar(areaLeft))
				left = areaLeft;
			else
			{
				if (boxLeft < 0)
				{
					if (Scalar(int(boxLeft)) == boxLeft)
						left = int(boxLeft);
					else
						left = int(boxLeft) - 1;
				}
				else
					left = int(boxLeft);
			}

			if (boxTop < Scalar(areaTop))
				top = areaTop;
			else
			{
				if (boxTop < 0)
				{
					if (Scalar(int(boxTop)) == boxTop)
						top = int(boxTop);
					else
						top = int(boxTop) - 1;
				}
				else
					top = int(boxTop);
			}

			if (boxRight > Scalar(areaRight))
				right = areaRight;
			else
			{
				if (boxRight < 0)
				{
					if (Scalar(int(boxRight)) == boxRight)
						right = int(boxRight);
					else
						right = int(boxRight) - 1;
				}
				else
					right = int(boxRight);
			}

			if (boxBottom > Scalar(areaBottom))
				bottom = areaBottom;
			else
			{
				if (boxBottom < 0)
				{
					if (Scalar(int(boxBottom)) == boxBottom)
						bottom = int(boxBottom);
					else
						bottom = int(boxBottom) - 1;
				}
				else
					bottom = int(boxBottom);
			}

			if (left != testLeft || top != testTop || testWidth != (right - left + 1u) || testHeight != (bottom - top + 1u))
				allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestBox2::testMultiplicationOperators(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Signed multiplication operators test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar width = Random::scalar(Scalar(0.1), 10);
		const Scalar height = Random::scalar(Scalar(0.1), 10);

		const Scalar centerX = Random::scalar(-10, 10);
		const Scalar centerY = Random::scalar(-10, 10);

		const Box2 sourceBox(Vector2(centerX, centerY), width, height);

		ocean_assert(sourceBox.isValid());
		if (!sourceBox.isValid())
		{
			allSucceeded = false;
		}

		{
			// testing scalar multiplication factor

			const Scalar scalarFactor = Random::scalar(-10, 10);

			Box2 copySourceBox(sourceBox);

			const Box2 targetBox = sourceBox * scalarFactor;
			copySourceBox *= scalarFactor;

			if (targetBox != copySourceBox)
			{
				allSucceeded = false;
			}

			Scalar newLeft = sourceBox.left() * scalarFactor;
			Scalar newRight = sourceBox.right() * scalarFactor;
			Utilities::sortLowestToFront2(newLeft, newRight);

			Scalar newTop = sourceBox.top() * scalarFactor;
			Scalar newBottom = sourceBox.bottom() * scalarFactor;
			Utilities::sortLowestToFront2(newTop, newBottom);

			if (Numeric::isNotEqual(newLeft, targetBox.left())
				|| Numeric::isNotEqual(newRight, targetBox.right())
				|| Numeric::isNotEqual(newTop, targetBox.top())
				|| Numeric::isNotEqual(newBottom, targetBox.bottom()))
			{
				allSucceeded = false;
			}
		}

		{
			// testing matrix multiplication factor

			const Quaternion rotation(Vector3(0, 0, 1), Random::scalar(0, Numeric::pi2()));
			const Vector3 xAxis((rotation * Vector3(1, 0, 0)).xy(), 0);
			const Vector3 yAxis((rotation * Vector3(0, 1, 0)).xy(), 0);
			const Vector3 zAxis(Random::vector2(-10, 10), 1);

			const SquareMatrix3 transformation = SquareMatrix3(xAxis, yAxis, zAxis);
			ocean_assert(!transformation.isSingular());

			Box2 copySourceBox(sourceBox);

			const Box2 targetBox = sourceBox * transformation;
			copySourceBox *= transformation;

			if (targetBox != copySourceBox)
			{
				allSucceeded = false;
			}

			const Vector2 transformedTopLeft = transformation * Vector2(sourceBox.left(), sourceBox.top());
			const Vector2 transformedTopRight = transformation * Vector2(sourceBox.right(), sourceBox.top());
			const Vector2 transformedBottomRight = transformation * Vector2(sourceBox.right(), sourceBox.bottom());
			const Vector2 transformedBottomLeft = transformation * Vector2(sourceBox.left(), sourceBox.bottom());

			const Scalar newLeft = std::min(std::min(transformedTopLeft.x(), transformedTopRight.x()), std::min(transformedBottomLeft.x(), transformedBottomRight.x()));
			const Scalar newRight = std::max(std::max(transformedTopLeft.x(), transformedTopRight.x()), std::max(transformedBottomLeft.x(), transformedBottomRight.x()));

			const Scalar newTop = std::min(std::min(transformedTopLeft.y(), transformedTopRight.y()), std::min(transformedBottomLeft.y(), transformedBottomRight.y()));
			const Scalar newBottom = std::max(std::max(transformedTopLeft.y(), transformedTopRight.y()), std::max(transformedBottomLeft.y(), transformedBottomRight.y()));

			if (Numeric::isNotEqual(newLeft, targetBox.left())
				|| Numeric::isNotEqual(newRight, targetBox.right())
				|| Numeric::isNotEqual(newTop, targetBox.top())
				|| Numeric::isNotEqual(newBottom, targetBox.bottom()))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

}

}

}
