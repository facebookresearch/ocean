/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestPixelBoundingBox.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestPixelBoundingBox::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("PixelBoundingBox test");

	Log::info() << " ";

	if (selector.shouldRun("constructors"))
	{
		testResult = testConstructors(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("pointisinside"))
	{
		testResult = testPointIsInside(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("boxisinside"))
	{
		testResult = testBoxIsInside(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("boxhasintersection"))
	{
		testResult = testBoxHasIntersection(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("boxistouching"))
	{
		testResult = testBoxIsTouching(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("extended"))
	{
		testResult = testExtended(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("multiplication"))
	{
		testResult = testMultiplication(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("division"))
	{
		testResult = testDivision(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << " ";

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestPixelBoundingBox, Constructors)
{
	EXPECT_TRUE(TestPixelBoundingBox::testConstructors(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, PointIsInside)
{
	EXPECT_TRUE(TestPixelBoundingBox::testPointIsInside(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, BoxIsInside)
{
	EXPECT_TRUE(TestPixelBoundingBox::testBoxIsInside(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, BoxHasIntersection)
{
	EXPECT_TRUE(TestPixelBoundingBox::testBoxHasIntersection(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, BoxIsTouching)
{
	EXPECT_TRUE(TestPixelBoundingBox::testBoxIsTouching(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, Extended)
{
	EXPECT_TRUE(TestPixelBoundingBox::testExtended(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, Multiplication)
{
	EXPECT_TRUE(TestPixelBoundingBox::testMultiplication(GTEST_TEST_DURATION));
}

TEST(TestPixelBoundingBox, Division)
{
	EXPECT_TRUE(TestPixelBoundingBox::testDivision(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestPixelBoundingBox::testConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Constructors:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// create invalid bounding boxes
		const CV::PixelBoundingBox boundingBox;
		const CV::PixelBoundingBoxI boundingBoxI;

		OCEAN_EXPECT_FALSE(validation, boundingBox.isValid());
		OCEAN_EXPECT_FALSE(validation, boundingBoxI.isValid());
	}

	const Timestamp startTimestamp(true);

	do
	{
		{
			const unsigned int pixelX = RandomI::random(randomGenerator, 0u, width - 1u);
			const unsigned int pixelY = RandomI::random(randomGenerator, 0u, height - 1u);
			const CV::PixelPosition pixel(pixelX, pixelY);
			ocean_assert(pixel.isValid());

			const CV::PixelBoundingBox boundingBox(pixel);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			OCEAN_EXPECT_TRUE(validation, boundingBox.isInside(pixel));

			OCEAN_EXPECT_EQUAL(validation, boundingBox.size(), 1u);

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int newPixelX = RandomI::random(randomGenerator, 0u, width - 1u);
				const unsigned int newPixelY = RandomI::random(randomGenerator, 0u, height - 1u);
				const CV::PixelPosition newPixel(newPixelX, newPixelY);

				if (newPixel != pixel && boundingBox.isInside(newPixel))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

		{
			const int pixelX = RandomI::random(randomGenerator, -int(width), int(width));
			const int pixelY = RandomI::random(randomGenerator, -int(height), int(height));
			const CV::PixelPositionI pixel(pixelX, pixelY);
			ocean_assert(pixel.isValid());

			const CV::PixelBoundingBoxI boundingBox(pixel);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			OCEAN_EXPECT_TRUE(validation, boundingBox.isInside(pixel));

			OCEAN_EXPECT_EQUAL(validation, boundingBox.size(), 1u);

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const int newPixelX = RandomI::random(randomGenerator, -int(width), int(width));
				const int newPixelY = RandomI::random(randomGenerator, -int(height), int(height));
				const CV::PixelPositionI newPixel(newPixelX, newPixelY);

				if (newPixel != pixel && boundingBox.isInside(newPixel))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

		{
			CV::PixelPositions pixels;
			pixels.reserve(100);

			unsigned int left = 0xFFFFFFFF;
			unsigned int top = 0xFFFFFFFF;
			unsigned int right = 0u;
			unsigned int bottom = 0u;

			while (pixels.size() < 100)
			{
				const unsigned int pixelX = RandomI::random(randomGenerator, 0u, width - 1u);
				const unsigned int pixelY = RandomI::random(randomGenerator, 0u, height - 1u);
				const CV::PixelPosition pixel(pixelX, pixelY);
				ocean_assert(pixel.isValid());

				left = min(left, pixel.x());
				right = max(right, pixel.x());

				top = min(top, pixel.y());
				bottom = max(bottom, pixel.y());

				pixels.push_back(pixel);
			}

			const CV::PixelBoundingBox boundingBox(pixels);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			for (CV::PixelPositions::const_iterator i = pixels.begin(); i != pixels.end(); ++i)
			{
				OCEAN_EXPECT_TRUE(validation, boundingBox.isInside(*i));
			}

			OCEAN_EXPECT_EQUAL(validation, boundingBox.left(), left);
			OCEAN_EXPECT_EQUAL(validation, boundingBox.top(), top);
			OCEAN_EXPECT_EQUAL(validation, boundingBox.right(), right);
			OCEAN_EXPECT_EQUAL(validation, boundingBox.bottom(), bottom);

			OCEAN_EXPECT_EQUAL(validation, boundingBox.size(), (right - left + 1u) * (bottom - top + 1u));
		}

		{
			CV::PixelPositionsI pixels;
			pixels.reserve(100);

			int left = NumericT<int>::maxValue();
			int top = NumericT<int>::maxValue();
			int right = NumericT<int>::minValue();
			int bottom = NumericT<int>::minValue();

			while (pixels.size() < 100)
			{
				const int pixelX = RandomI::random(randomGenerator, -int(width), int(width));
				const int pixelY = RandomI::random(randomGenerator, -int(height), int(height));
				const CV::PixelPositionI pixel(pixelX, pixelY);
				ocean_assert(pixel.isValid());

				left = min(left, pixel.x());
				right = max(right, pixel.x());

				top = min(top, pixel.y());
				bottom = max(bottom, pixel.y());

				pixels.push_back(pixel);
			}

			const CV::PixelBoundingBoxI boundingBox(pixels);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			for (CV::PixelPositionsI::const_iterator i = pixels.begin(); i != pixels.end(); ++i)
			{
				OCEAN_EXPECT_TRUE(validation, boundingBox.isInside(*i));
			}

			OCEAN_EXPECT_EQUAL(validation, boundingBox.left(), left);
			OCEAN_EXPECT_EQUAL(validation, boundingBox.top(), top);
			OCEAN_EXPECT_EQUAL(validation, boundingBox.right(), right);
			OCEAN_EXPECT_EQUAL(validation, boundingBox.bottom(), bottom);

			OCEAN_EXPECT_EQUAL(validation, boundingBox.size(), (unsigned int)((right - left + 1) * (bottom - top + 1)));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testPointIsInside(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Point isInside() test:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		{
			const unsigned int left = RandomI::random(randomGenerator, 0u, width - 1u);
			const unsigned int top = RandomI::random(randomGenerator, 0u, height - 1u);

			const unsigned int right = RandomI::random(randomGenerator, left, width - 1u);
			const unsigned int bottom = RandomI::random(randomGenerator, top, height - 1u);

			const CV::PixelBoundingBox boundingBox(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int pixelX = RandomI::random(randomGenerator, 0u, 2u * width - 1u);
				const unsigned int pixelY = RandomI::random(randomGenerator, 0u, 2u * height - 1u);
				const CV::PixelPosition pixel(pixelX, pixelY);
				ocean_assert(pixel.isValid());

				const bool result = boundingBox.isInside(pixel);
				const bool test = !(pixel.x() < boundingBox.left() || pixel.y() < boundingBox.top() || pixel.x() > boundingBox.right() || pixel.y() > boundingBox.bottom());

				OCEAN_EXPECT_EQUAL(validation, result, test);
			}
		}

		{
			const int left = RandomI::random(randomGenerator, -int(width), int(width));
			const int top = RandomI::random(randomGenerator, -int(height), int(height));

			const int right = RandomI::random(randomGenerator, left, int(width));
			const int bottom = RandomI::random(randomGenerator, top, int(height));

			const CV::PixelBoundingBoxI boundingBox(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const int pixelX = RandomI::random(randomGenerator, -int(width * 2u), int(width * 2u));
				const int pixelY = RandomI::random(randomGenerator, -int(height * 2u), int(height * 2u));
				const CV::PixelPositionI pixel(pixelX, pixelY);
				ocean_assert(pixel.isValid());

				const bool result = boundingBox.isInside(pixel);
				const bool test = !(pixel.x() < boundingBox.left() || pixel.y() < boundingBox.top() || pixel.x() > boundingBox.right() || pixel.y() > boundingBox.bottom());

				OCEAN_EXPECT_EQUAL(validation, result, test);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testBoxIsInside(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Box isInside() test:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		{
			const unsigned int left = RandomI::random(randomGenerator, 0u, width - 1u);
			const unsigned int top = RandomI::random(randomGenerator, 0u, height - 1u);

			const unsigned int right = RandomI::random(randomGenerator, left, width - 1u);
			const unsigned int bottom = RandomI::random(randomGenerator, top, height - 1u);

			const CV::PixelBoundingBox boundingBox(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int childLeft = RandomI::random(randomGenerator, 0u, 2u * width - 1u);
				const unsigned int childTop = RandomI::random(randomGenerator, 0u, 2u * height - 1u);

				const unsigned int childRight = RandomI::random(randomGenerator, childLeft, 2u * width - 1u);
				const unsigned int childBottom = RandomI::random(randomGenerator, childTop, 2u * height - 1u);

				const CV::PixelBoundingBox childBox(childLeft, childTop, childRight, childBottom);
				ocean_assert(childBox.isValid());

				const bool result = boundingBox.isInside(childBox);
				const bool test = boundingBox.isInside(childBox.topLeft()) && boundingBox.isInside(childBox.topRight()) && boundingBox.isInside(childBox.bottomLeft()) && boundingBox.isInside(childBox.bottomRight());

				OCEAN_EXPECT_EQUAL(validation, result, test);
			}
		}

		{
			const int left = RandomI::random(randomGenerator, -int(width), int(width));
			const int top = RandomI::random(randomGenerator, -int(height), int(height));

			const int right = RandomI::random(randomGenerator, left, int(width));
			const int bottom = RandomI::random(randomGenerator, top, int(height));

			const CV::PixelBoundingBoxI boundingBox(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const int childLeft = RandomI::random(randomGenerator, -int(width * 2u), int(width * 2u));
				const int childTop = RandomI::random(randomGenerator, -int(height * 2u), int(height * 2u));

				const int childRight = RandomI::random(randomGenerator, childLeft, int(width * 2u));
				const int childBottom = RandomI::random(randomGenerator, childTop, int(height * 2u));

				const CV::PixelBoundingBoxI childBox(childLeft, childTop, childRight, childBottom);
				ocean_assert(childBox.isValid());

				const bool result = boundingBox.isInside(childBox);
				const bool test = boundingBox.isInside(childBox.topLeft()) && boundingBox.isInside(childBox.topRight()) && boundingBox.isInside(childBox.bottomLeft()) && boundingBox.isInside(childBox.bottomRight());

				OCEAN_EXPECT_EQUAL(validation, result, test);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testBoxHasIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Box hasIntersection() test:";

	constexpr unsigned int width = 1000u;
	constexpr unsigned int height = 1000u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int boxAX = RandomI::random(randomGenerator, 0u, width);
			const unsigned int boxAY = RandomI::random(randomGenerator, 0u, height);
			const unsigned int boxAWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxAHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBox boxA(CV::PixelPosition(boxAX, boxAY), boxAWidth, boxAHeight);

			const unsigned int boxBX = RandomI::random(randomGenerator, 0u, width);
			const unsigned int boxBY = RandomI::random(randomGenerator, 0u, height);
			const unsigned int boxBWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxBHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBox boxB(CV::PixelPosition(boxBX, boxBY), boxBWidth, boxBHeight);

			bool hasIntersection = false;

			if (boxA.isInside(boxB.topLeft()) || boxA.isInside(boxB.topRight()) || boxA.isInside(boxB.bottomLeft()) || boxA.isInside(boxB.bottomRight()))
			{
				// at least one corner is inside the other box
				hasIntersection = true;
			}

			if (boxB.isInside(boxA.topLeft()) || boxB.isInside(boxA.topRight()) || boxB.isInside(boxA.bottomLeft()) || boxB.isInside(boxA.bottomRight()))
			{
				// at least one corner is inside the other box
				hasIntersection = true;
			}

			if (boxA.left() >= boxB.left() && boxA.right() <= boxB.right() && boxA.top() <= boxB.top() && boxA.bottom() >= boxB.bottom())
			{
				// we have a cross shape
				hasIntersection = true;
			}

			if (boxB.left() >= boxA.left() && boxB.right() <= boxA.right() && boxB.top() <= boxA.top() && boxB.bottom() >= boxA.bottom())
			{
				// we have a cross shape
				hasIntersection = true;
			}

			OCEAN_EXPECT_EQUAL(validation, hasIntersection, boxA.hasIntersection(boxB));

			OCEAN_EXPECT_EQUAL(validation, hasIntersection, boxB.hasIntersection(boxA));

			OCEAN_EXPECT_TRUE(validation, boxA.hasIntersection(boxA));

			OCEAN_EXPECT_TRUE(validation, boxB.hasIntersection(boxB));
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int boxAX = RandomI::random(randomGenerator, -int(width), int(width));
			const int boxAY = RandomI::random(randomGenerator, -int(height), int(height));
			const unsigned int boxAWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxAHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBoxI boxA(CV::PixelPositionI(boxAX, boxAY), boxAWidth, boxAHeight);

			const int boxBX = RandomI::random(randomGenerator, -int(width), int(width));
			const int boxBY = RandomI::random(randomGenerator, -int(height), int(height));
			const unsigned int boxBWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxBHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBoxI boxB(CV::PixelPositionI(boxBX, boxBY), boxBWidth, boxBHeight);

			bool hasIntersection = false;

			if (boxA.isInside(boxB.topLeft()) || boxA.isInside(boxB.topRight()) || boxA.isInside(boxB.bottomLeft()) || boxA.isInside(boxB.bottomRight()))
			{
				// at least one corner is inside the other box
				hasIntersection = true;
			}

			if (boxB.isInside(boxA.topLeft()) || boxB.isInside(boxA.topRight()) || boxB.isInside(boxA.bottomLeft()) || boxB.isInside(boxA.bottomRight()))
			{
				// at least one corner is inside the other box
				hasIntersection = true;
			}

			if (boxA.left() >= boxB.left() && boxA.right() <= boxB.right() && boxA.top() <= boxB.top() && boxA.bottom() >= boxB.bottom())
			{
				// we have a cross shape
				hasIntersection = true;
			}

			if (boxB.left() >= boxA.left() && boxB.right() <= boxA.right() && boxB.top() <= boxA.top() && boxB.bottom() >= boxA.bottom())
			{
				// we have a cross shape
				hasIntersection = true;
			}

			OCEAN_EXPECT_EQUAL(validation, hasIntersection, boxA.hasIntersection(boxB));

			OCEAN_EXPECT_EQUAL(validation, hasIntersection, boxB.hasIntersection(boxA));

			OCEAN_EXPECT_TRUE(validation, boxA.hasIntersection(boxA));

			OCEAN_EXPECT_TRUE(validation, boxB.hasIntersection(boxB));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testBoxIsTouching(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Box isTouching() test:";

	constexpr unsigned int width = 1000u;
	constexpr unsigned int height = 1000u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int boxAX = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxAY = RandomI::random(randomGenerator, 1u, height);
			const unsigned int boxAWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxAHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBox boxA(CV::PixelPosition(boxAX, boxAY), boxAWidth, boxAHeight);

			const unsigned int boxBX = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxBY = RandomI::random(randomGenerator, 1u, height);
			const unsigned int boxBWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxBHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBox boxB(CV::PixelPosition(boxBX, boxBY), boxBWidth, boxBHeight);

			bool isTouching = boxA.hasIntersection(boxB);

			const CV::PixelBoundingBox extendedBoxA(boxA.left() - 1u, boxA.top() - 1u, boxA.right() + 1u, boxA.bottom() + 1u);
			const CV::PixelBoundingBox extendedBoxB(boxB.left() - 1u, boxB.top() - 1u, boxB.right() + 1u, boxB.bottom() + 1u);

			for (const bool useNeighborhood8 : {false, true})
			{
				if (extendedBoxA.hasIntersection(boxB) || boxA.hasIntersection(extendedBoxB))
				{
					if (useNeighborhood8)
					{
						isTouching = true;
					}
					else
					{
						if (extendedBoxA.topLeft() == boxB.bottomRight()
							|| extendedBoxA.topRight() == boxB.bottomLeft()
							|| extendedBoxA.bottomLeft() == boxB.topRight()
							|| extendedBoxA.bottomRight() == boxB.topLeft())
						{
							// not touching
						}
						else if (extendedBoxB.topLeft() == boxA.bottomRight()
							|| extendedBoxB.topRight() == boxA.bottomLeft()
							|| extendedBoxB.bottomLeft() == boxA.topRight()
							|| extendedBoxB.bottomRight() == boxA.topLeft())
						{
							// not touching
						}
						else
						{
							isTouching = true;
						}
					}
				}

				OCEAN_EXPECT_EQUAL(validation, isTouching, boxA.isTouching(boxB, useNeighborhood8));

				OCEAN_EXPECT_EQUAL(validation, boxA.isTouching(boxB, useNeighborhood8), boxB.isTouching(boxA, useNeighborhood8));
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int boxAX = RandomI::random(randomGenerator, -int(width), int(width));
			const int boxAY = RandomI::random(randomGenerator, -int(height), int(height));
			const unsigned int boxAWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxAHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBoxI boxA(CV::PixelPositionI(boxAX, boxAY), boxAWidth, boxAHeight);

			const int boxBX = RandomI::random(randomGenerator, -int(width), int(width));
			const int boxBY = RandomI::random(randomGenerator, -int(height), int(height));
			const unsigned int boxBWidth = RandomI::random(randomGenerator, 1u, width);
			const unsigned int boxBHeight = RandomI::random(randomGenerator, 1u, height);
			const CV::PixelBoundingBoxI boxB(CV::PixelPositionI(boxBX, boxBY), boxBWidth, boxBHeight);

			bool isTouching = boxA.hasIntersection(boxB);

			const CV::PixelBoundingBoxI extendedBoxA(boxA.left() - 1, boxA.top() - 1, boxA.right() + 1, boxA.bottom() + 1);
			const CV::PixelBoundingBoxI extendedBoxB(boxB.left() - 1, boxB.top() - 1, boxB.right() + 1, boxB.bottom() + 1);

			for (const bool useNeighborhood8 : {false, true})
			{
				if (extendedBoxA.hasIntersection(boxB) || boxA.hasIntersection(extendedBoxB))
				{
					if (useNeighborhood8)
					{
						isTouching = true;
					}
					else
					{
						if (extendedBoxA.topLeft() == boxB.bottomRight()
							|| extendedBoxA.topRight() == boxB.bottomLeft()
							|| extendedBoxA.bottomLeft() == boxB.topRight()
							|| extendedBoxA.bottomRight() == boxB.topLeft())
						{
							// not touching
						}
						else if (extendedBoxB.topLeft() == boxA.bottomRight()
							|| extendedBoxB.topRight() == boxA.bottomLeft()
							|| extendedBoxB.bottomLeft() == boxA.topRight()
							|| extendedBoxB.bottomRight() == boxA.topLeft())
						{
							// not touching
						}
						else
						{
							isTouching = true;
						}
					}
				}

				OCEAN_EXPECT_EQUAL(validation, isTouching, boxA.isTouching(boxB, useNeighborhood8));

				OCEAN_EXPECT_EQUAL(validation, boxA.isTouching(boxB, useNeighborhood8), boxB.isTouching(boxA, useNeighborhood8));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testExtended(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "extended() test:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		{
			const unsigned int left = RandomI::random(randomGenerator, 0u, width - 1u);
			const unsigned int top = RandomI::random(randomGenerator, 0u, height - 1u);

			const unsigned int right = RandomI::random(randomGenerator, left, width - 1u);
			const unsigned int bottom = RandomI::random(randomGenerator, top, height - 1u);

			const CV::PixelBoundingBox boundingBox(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			const unsigned int pixels = RandomI::random(randomGenerator, 0u, 20u);
			const CV::PixelBoundingBox extendedBoundingBox(boundingBox.extended(pixels, 0u, 0u, width - 1u, height - 1u));

			if (extendedBoundingBox.left() == 0u)
			{
				if (boundingBox.left() > pixels)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, boundingBox.left() - extendedBoundingBox.left(), pixels);
			}

			if (extendedBoundingBox.top() == 0u)
			{
				if (boundingBox.top() > pixels)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, boundingBox.top() - extendedBoundingBox.top(), pixels);
			}

			if (extendedBoundingBox.right() == width - 1u)
			{
				if (boundingBox.right() < width - 1u - pixels)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, extendedBoundingBox.right() - boundingBox.right(), pixels);
			}

			if (extendedBoundingBox.bottom() == height - 1u)
			{
				if (boundingBox.bottom() < height - 1u - pixels)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, extendedBoundingBox.bottom() - boundingBox.bottom(), pixels);
			}
		}

		{
			const int left = RandomI::random(randomGenerator, -int(width), int(width));
			const int top = RandomI::random(randomGenerator, -int(height), int(height));

			const int right = RandomI::random(randomGenerator, left, int(width));
			const int bottom = RandomI::random(randomGenerator, top, int(height));

			const CV::PixelBoundingBoxI boundingBox(left, top, right, bottom);

			OCEAN_EXPECT_TRUE(validation, boundingBox.isValid());

			const unsigned int pixels = RandomI::random(randomGenerator, 0u, 20u);
			const CV::PixelBoundingBoxI extendedBoundingBox(boundingBox.extended(pixels, -int(width) - 2, -int(height) - 2, int(width) + 2, int(height) + 2));

			if (extendedBoundingBox.left() == -int(width) - 2)
			{
				if (boundingBox.left() > int(pixels) - 2)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, boundingBox.left() - extendedBoundingBox.left(), int(pixels));
			}

			if (extendedBoundingBox.top() == -int(height) - 2)
			{
				if (boundingBox.top() > int(pixels) - 2)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, boundingBox.top() - extendedBoundingBox.top(), int(pixels));
			}

			if (extendedBoundingBox.right() == int(width) + 2)
			{
				if (boundingBox.right() < int(width) + 2 - int(pixels))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, extendedBoundingBox.right() - boundingBox.right(), int(pixels));
			}

			if (extendedBoundingBox.bottom() == int(height + 2))
			{
				if (boundingBox.bottom() < int(height) + 2 - int(pixels))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, extendedBoundingBox.bottom() - boundingBox.bottom(), int(pixels));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testMultiplication(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Multiplication operator test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int left = RandomI::random(randomGenerator, 1920u);
			const unsigned int top = RandomI::random(randomGenerator, 1080u);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int right = left + width - 1u;
			const unsigned int bottom = top + height - 1u;

			const CV::PixelBoundingBox boundingBox(CV::PixelPosition(left, top), width, height);

			const unsigned int factor = RandomI::random(randomGenerator, 10u);

			const CV::PixelBoundingBox multpliedBoundingBoxA = boundingBox * factor;

			CV::PixelBoundingBox multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB *= factor;

			const CV::PixelBoundingBox testBoundingBox(left * factor, top * factor, right * factor, bottom * factor);

			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxA, testBoundingBox);
			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxB, testBoundingBox);
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int left = RandomI::random(randomGenerator, -1920, 1920);
			const int top = RandomI::random(randomGenerator, -1080, 1080);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const int right = left + int(width) - 1;
			const int bottom = top + int(height) - 1;

			const CV::PixelBoundingBoxI boundingBox(CV::PixelPositionI(left, top), width, height);

			const int factor = RandomI::random(randomGenerator, -10, 10);

			const CV::PixelBoundingBoxI multpliedBoundingBoxA = boundingBox * factor;

			CV::PixelBoundingBoxI multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB *= factor;

			const CV::PixelBoundingBoxI testBoundingBox(left * factor, top * factor, right * factor, bottom * factor);

			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxA, testBoundingBox);
			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxB, testBoundingBox);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestPixelBoundingBox::testDivision(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Division operator test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int left = RandomI::random(randomGenerator, 1920u);
			const unsigned int top = RandomI::random(randomGenerator, 1080u);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int right = left + width - 1u;
			const unsigned int bottom = top + height - 1u;

			const CV::PixelBoundingBox boundingBox(CV::PixelPosition(left, top), width, height);

			const unsigned int factor = RandomI::random(randomGenerator, 1u, 10u);
			ocean_assert(factor != 0u);

			const CV::PixelBoundingBox multpliedBoundingBoxA = boundingBox / factor;

			CV::PixelBoundingBox multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB /= factor;

			const CV::PixelBoundingBox testBoundingBox(left / factor, top / factor, right / factor, bottom / factor);

			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxA, testBoundingBox);
			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxB, testBoundingBox);
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int left = RandomI::random(randomGenerator, -1920, 1920);
			const int top = RandomI::random(randomGenerator, -1080, 1080);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const int right = left + int(width) - 1;
			const int bottom = top + int(height) - 1;

			const CV::PixelBoundingBoxI boundingBox(CV::PixelPositionI(left, top), width, height);

			const int sign = RandomI::boolean(randomGenerator) ? 1 : -1;
			const int factor = RandomI::random(randomGenerator, 1, 10) * sign;
			ocean_assert(factor != 0);

			const CV::PixelBoundingBoxI multpliedBoundingBoxA = boundingBox / factor;

			CV::PixelBoundingBoxI multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB /= factor;

			const CV::PixelBoundingBoxI testBoundingBox(left / factor, top / factor, right / factor, bottom / factor);

			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxA, testBoundingBox);
			OCEAN_EXPECT_EQUAL(validation, multpliedBoundingBoxB, testBoundingBox);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
