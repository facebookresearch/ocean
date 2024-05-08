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

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestPixelBoundingBox::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   PixelBoundingBox test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructors(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPointIsInside(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBoxIsInside(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBoxHasIntersection(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBoxIsTouching(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExtended(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultiplication(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDivision(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "PixelBoundingBox test succeeded.";
	}
	else
	{
		Log::info() << "PixelBoundingBox test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	{
		// create invalid bounding boxes
		const CV::PixelBoundingBox boundingBox;
		const CV::PixelBoundingBoxI boundingBoxI;

		if (boundingBox.isValid() || boundingBoxI.isValid())
			allSucceeded = false;
	}

	const Timestamp startTimestamp(true);

	do
	{
		{
			const CV::PixelPosition pixel(RandomI::random(0u, width - 1u), RandomI::random(0u, height - 1u));
			ocean_assert(pixel.isValid());

			const CV::PixelBoundingBox boundingBox(pixel);

			if (!boundingBox.isValid())
				allSucceeded = false;

			if (!boundingBox.isInside(pixel))
				allSucceeded = false;

			if (boundingBox.size() != 1u)
				allSucceeded = false;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const CV::PixelPosition newPixel(RandomI::random(0u, width - 1u), RandomI::random(0u, height - 1u));

				if (newPixel != pixel && boundingBox.isInside(newPixel))
					allSucceeded = false;
			}
		}

		{
			const CV::PixelPositionI pixel(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height)));
			ocean_assert(pixel.isValid());

			const CV::PixelBoundingBoxI boundingBox(pixel);

			if (!boundingBox.isValid())
				allSucceeded = false;

			if (!boundingBox.isInside(pixel))
				allSucceeded = false;

			if (boundingBox.size() != 1u)
				allSucceeded = false;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const CV::PixelPositionI newPixel(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height)));

				if (newPixel != pixel && boundingBox.isInside(newPixel))
					allSucceeded = false;
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
				const CV::PixelPosition pixel(RandomI::random(0u, width - 1u), RandomI::random(0u, height - 1u));
				ocean_assert(pixel.isValid());

				left = min(left, pixel.x());
				right = max(right, pixel.x());

				top = min(top, pixel.y());
				bottom = max(bottom, pixel.y());

				pixels.push_back(pixel);
			}

			const CV::PixelBoundingBox boundingBox(pixels);

			if (!boundingBox.isValid())
				allSucceeded = false;

			for (CV::PixelPositions::const_iterator i = pixels.begin(); i != pixels.end(); ++i)
				if (!boundingBox.isInside(*i))
					allSucceeded = false;

			if (boundingBox.left() != left || boundingBox.top() != top || boundingBox.right() != right || boundingBox.bottom() != bottom)
				allSucceeded = false;

			if (boundingBox.size() != (right - left + 1u) * (bottom - top + 1u))
				allSucceeded = false;
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
				const CV::PixelPositionI pixel(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height)));
				ocean_assert(pixel.isValid());

				left = min(left, pixel.x());
				right = max(right, pixel.x());

				top = min(top, pixel.y());
				bottom = max(bottom, pixel.y());

				pixels.push_back(pixel);
			}

			const CV::PixelBoundingBoxI boundingBox(pixels);

			if (!boundingBox.isValid())
				allSucceeded = false;

			for (CV::PixelPositionsI::const_iterator i = pixels.begin(); i != pixels.end(); ++i)
				if (!boundingBox.isInside(*i))
					allSucceeded = false;

			if (boundingBox.left() != left || boundingBox.top() != top || boundingBox.right() != right || boundingBox.bottom() != bottom)
				allSucceeded = false;

			if (boundingBox.size() != (unsigned int)((right - left + 1) * (bottom - top + 1)))
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

bool TestPixelBoundingBox::testPointIsInside(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Point isInside() test:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		{
			const unsigned int left = RandomI::random(0u, width - 1u);
			const unsigned int top = RandomI::random(0u, height - 1u);

			const unsigned int right = RandomI::random(left, width - 1u);
			const unsigned int bottom = RandomI::random(top, height - 1u);

			const CV::PixelBoundingBox boundingBox(left, top, right, bottom);

			if (!boundingBox.isValid())
				allSucceeded = false;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const CV::PixelPosition pixel(RandomI::random(0u, 2u * width - 1u), RandomI::random(0u, 2u * height - 1u));
				ocean_assert(pixel.isValid());

				const bool result = boundingBox.isInside(pixel);
				const bool test = !(pixel.x() < boundingBox.left() || pixel.y() < boundingBox.top() || pixel.x() > boundingBox.right() || pixel.y() > boundingBox.bottom());

				if (result != test)
					allSucceeded = false;
			}
		}

		{
			const int left = RandomI::random(-int(width), int(width));
			const int top = RandomI::random(-int(height), int(height));

			const int right = RandomI::random(left, int(width));
			const int bottom = RandomI::random(top, int(height));

			const CV::PixelBoundingBoxI boundingBox(left, top, right, bottom);

			if (!boundingBox.isValid())
				allSucceeded = false;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const CV::PixelPositionI pixel(RandomI::random(-int(width * 2u), int(width * 2u)), RandomI::random(-int(height * 2u), int(height * 2u)));
				ocean_assert(pixel.isValid());

				const bool result = boundingBox.isInside(pixel);
				const bool test = !(pixel.x() < boundingBox.left() || pixel.y() < boundingBox.top() || pixel.x() > boundingBox.right() || pixel.y() > boundingBox.bottom());

				if (result != test)
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

bool TestPixelBoundingBox::testBoxIsInside(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Box isInside() test:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		{
			const unsigned int left = RandomI::random(0u, width - 1u);
			const unsigned int top = RandomI::random(0u, height - 1u);

			const unsigned int right = RandomI::random(left, width - 1u);
			const unsigned int bottom = RandomI::random(top, height - 1u);

			const CV::PixelBoundingBox boundingBox(left, top, right, bottom);

			if (!boundingBox.isValid())
				allSucceeded = false;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const unsigned int childLeft = RandomI::random(0u, 2u * width - 1u);
				const unsigned int childTop = RandomI::random(0u, 2u * height - 1u);

				const unsigned int childRight = RandomI::random(childLeft, 2u * width - 1u);
				const unsigned int childBottom = RandomI::random(childTop, 2u * height - 1u);

				const CV::PixelBoundingBox childBox(childLeft, childTop, childRight, childBottom);
				ocean_assert(childBox.isValid());

				const bool result = boundingBox.isInside(childBox);
				const bool test = boundingBox.isInside(childBox.topLeft()) && boundingBox.isInside(childBox.topRight()) && boundingBox.isInside(childBox.bottomLeft()) && boundingBox.isInside(childBox.bottomRight());

				if (result != test)
					allSucceeded = false;
			}
		}

		{
			const int left = RandomI::random(-int(width), int(width));
			const int top = RandomI::random(-int(height), int(height));

			const int right = RandomI::random(left, int(width));
			const int bottom = RandomI::random(top, int(height));

			const CV::PixelBoundingBoxI boundingBox(left, top, right, bottom);

			if (!boundingBox.isValid())
				allSucceeded = false;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				const int childLeft = RandomI::random(-int(width * 2u), int(width * 2u));
				const int childTop = RandomI::random(-int(height * 2u), int(height * 2u));

				const int childRight = RandomI::random(childLeft, int(width * 2u));
				const int childBottom = RandomI::random(childTop, int(height * 2u));

				const CV::PixelBoundingBoxI childBox(childLeft, childTop, childRight, childBottom);
				ocean_assert(childBox.isValid());

				const bool result = boundingBox.isInside(childBox);
				const bool test = boundingBox.isInside(childBox.topLeft()) && boundingBox.isInside(childBox.topRight()) && boundingBox.isInside(childBox.bottomLeft()) && boundingBox.isInside(childBox.bottomRight());

				if (result != test)
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

bool TestPixelBoundingBox::testBoxHasIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Box hasIntersection() test:";

	constexpr unsigned int width = 1000u;
	constexpr unsigned int height = 1000u;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const CV::PixelBoundingBox boxA(CV::PixelPosition(RandomI::random(0u, width), RandomI::random(0u, height)), RandomI::random(1u, width), RandomI::random(1u, height));
			const CV::PixelBoundingBox boxB(CV::PixelPosition(RandomI::random(0u, width), RandomI::random(0u, height)), RandomI::random(1u, width), RandomI::random(1u, height));

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

			if (hasIntersection != boxA.hasIntersection(boxB))
			{
				allSucceeded = false;
			}

			if (hasIntersection != boxB.hasIntersection(boxA))
			{
				allSucceeded = false;
			}

			if (!boxA.hasIntersection(boxA))
			{
				allSucceeded = false;
			}

			if (!boxB.hasIntersection(boxB))
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const CV::PixelBoundingBoxI boxA(CV::PixelPositionI(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height))), RandomI::random(1u, width), RandomI::random(1u, height));
			const CV::PixelBoundingBoxI boxB(CV::PixelPositionI(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height))), RandomI::random(1u, width), RandomI::random(1u, height));

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

			if (hasIntersection != boxA.hasIntersection(boxB))
			{
				allSucceeded = false;
			}

			if (hasIntersection != boxB.hasIntersection(boxA))
			{
				allSucceeded = false;
			}

			if (!boxA.hasIntersection(boxA))
			{
				allSucceeded = false;
			}

			if (!boxB.hasIntersection(boxB))
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

bool TestPixelBoundingBox::testBoxIsTouching(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Box isTouching() test:";

	constexpr unsigned int width = 1000u;
	constexpr unsigned int height = 1000u;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const CV::PixelBoundingBox boxA(CV::PixelPosition(RandomI::random(1u, width), RandomI::random(1u, height)), RandomI::random(1u, width), RandomI::random(1u, height));
			const CV::PixelBoundingBox boxB(CV::PixelPosition(RandomI::random(1u, width), RandomI::random(1u, height)), RandomI::random(1u, width), RandomI::random(1u, height));

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

				if (isTouching != boxA.isTouching(boxB, useNeighborhood8))
				{
					allSucceeded = false;
				}

				if (boxA.isTouching(boxB, useNeighborhood8) != boxB.isTouching(boxA, useNeighborhood8))
				{
					allSucceeded = false;
				}
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const CV::PixelBoundingBoxI boxA(CV::PixelPositionI(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height))), RandomI::random(1u, width), RandomI::random(1u, height));
			const CV::PixelBoundingBoxI boxB(CV::PixelPositionI(RandomI::random(-int(width), int(width)), RandomI::random(-int(height), int(height))), RandomI::random(1u, width), RandomI::random(1u, height));

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

				if (isTouching != boxA.isTouching(boxB, useNeighborhood8))
				{
					allSucceeded = false;
				}

				if (boxA.isTouching(boxB, useNeighborhood8) != boxB.isTouching(boxA, useNeighborhood8))
				{
					allSucceeded = false;
				}
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

bool TestPixelBoundingBox::testExtended(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "extended() test:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		{
			const unsigned int left = RandomI::random(0u, width - 1u);
			const unsigned int top = RandomI::random(0u, height - 1u);

			const unsigned int right = RandomI::random(left, width - 1u);
			const unsigned int bottom = RandomI::random(top, height - 1u);

			const CV::PixelBoundingBox boundingBox(left, top, right, bottom);

			if (!boundingBox.isValid())
				allSucceeded = false;

			const unsigned int pixels = RandomI::random(0u, 20u);
			const CV::PixelBoundingBox extendedBoundingBox(boundingBox.extended(pixels, 0u, 0u, width - 1u, height - 1u));

			if (extendedBoundingBox.left() == 0u)
			{
				if (boundingBox.left() > pixels)
					allSucceeded = false;
			}
			else
			{
				if (boundingBox.left() - extendedBoundingBox.left() != pixels)
					allSucceeded = false;
			}

			if (extendedBoundingBox.top() == 0u)
			{
				if (boundingBox.top() > pixels)
					allSucceeded = false;
			}
			else
			{
				if (boundingBox.top() - extendedBoundingBox.top() != pixels)
					allSucceeded = false;
			}

			if (extendedBoundingBox.right() == width - 1u)
			{
				if (boundingBox.right() < width - 1u - pixels)
					allSucceeded = false;
			}
			else
			{
				if (extendedBoundingBox.right() - boundingBox.right() != pixels)
					allSucceeded = false;
			}

			if (extendedBoundingBox.bottom() == height - 1u)
			{
				if (boundingBox.bottom() < height - 1u - pixels)
					allSucceeded = false;
			}
			else
			{
				if (extendedBoundingBox.bottom() - boundingBox.bottom() != pixels)
					allSucceeded = false;
			}
		}

		{
			const int left = RandomI::random(-int(width), int(width));
			const int top = RandomI::random(-int(height), int(height));

			const int right = RandomI::random(left, int(width));
			const int bottom = RandomI::random(top, int(height));

			const CV::PixelBoundingBoxI boundingBox(left, top, right, bottom);

			if (!boundingBox.isValid())
				allSucceeded = false;

			const unsigned int pixels = RandomI::random(0u, 20u);
			const CV::PixelBoundingBoxI extendedBoundingBox(boundingBox.extended(pixels, -int(width) - 2, -int(height) - 2, int(width) + 2, int(height) + 2));

			if (extendedBoundingBox.left() == -int(width) - 2)
			{
				if (boundingBox.left() > int(pixels) - 2)
					allSucceeded = false;
			}
			else
			{
				if (boundingBox.left() - extendedBoundingBox.left() != int(pixels))
					allSucceeded = false;
			}

			if (extendedBoundingBox.top() == -int(height) - 2)
			{
				if (boundingBox.top() > int(pixels) - 2)
					allSucceeded = false;
			}
			else
			{
				if (boundingBox.top() - extendedBoundingBox.top() != int(pixels))
					allSucceeded = false;
			}

			if (extendedBoundingBox.right() == int(width) + 2)
			{
				if (boundingBox.right() < int(width) + 2 - int(pixels))
					allSucceeded = false;
			}
			else
			{
				if (extendedBoundingBox.right() - boundingBox.right() != int(pixels))
					allSucceeded = false;
			}

			if (extendedBoundingBox.bottom() == int(height + 2))
			{
				if (boundingBox.bottom() < int(height) + 2 - int(pixels))
					allSucceeded = false;
			}
			else
			{
				if (extendedBoundingBox.bottom() - boundingBox.bottom() != int(pixels))
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

bool TestPixelBoundingBox::testMultiplication(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Multiplication operator test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int left = RandomI::random(1920u);
			const unsigned int top = RandomI::random(1080u);

			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);

			const unsigned int right = left + width - 1u;
			const unsigned int bottom = top + height - 1u;

			const CV::PixelBoundingBox boundingBox(CV::PixelPosition(left, top), width, height);

			const unsigned int factor = RandomI::random(10u);

			const CV::PixelBoundingBox multpliedBoundingBoxA = boundingBox * factor;

			CV::PixelBoundingBox multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB *= factor;

			const CV::PixelBoundingBox testBoundingBox(left * factor, top * factor, right * factor, bottom * factor);

			if (multpliedBoundingBoxA != testBoundingBox || multpliedBoundingBoxB != testBoundingBox)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int left = RandomI::random(-1920, 1920);
			const int top = RandomI::random(-1080, 1080);

			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);

			const int right = left + int(width) - 1;
			const int bottom = top + int(height) - 1;

			const CV::PixelBoundingBoxI boundingBox(CV::PixelPositionI(left, top), width, height);

			const int factor = RandomI::random(-10, 10);

			const CV::PixelBoundingBoxI multpliedBoundingBoxA = boundingBox * factor;

			CV::PixelBoundingBoxI multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB *= factor;

			const CV::PixelBoundingBoxI testBoundingBox(left * factor, top * factor, right * factor, bottom * factor);

			if (multpliedBoundingBoxA != testBoundingBox || multpliedBoundingBoxB != testBoundingBox)
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

bool TestPixelBoundingBox::testDivision(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Division operator test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int left = RandomI::random(1920u);
			const unsigned int top = RandomI::random(1080u);

			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);

			const unsigned int right = left + width - 1u;
			const unsigned int bottom = top + height - 1u;

			const CV::PixelBoundingBox boundingBox(CV::PixelPosition(left, top), width, height);

			const unsigned int factor = RandomI::random(1u, 10u);
			ocean_assert(factor != 0u);

			const CV::PixelBoundingBox multpliedBoundingBoxA = boundingBox / factor;

			CV::PixelBoundingBox multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB /= factor;

			const CV::PixelBoundingBox testBoundingBox(left / factor, top / factor, right / factor, bottom / factor);

			if (multpliedBoundingBoxA != testBoundingBox || multpliedBoundingBoxB != testBoundingBox)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int left = RandomI::random(-1920, 1920);
			const int top = RandomI::random(-1080, 1080);

			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);

			const int right = left + int(width) - 1;
			const int bottom = top + int(height) - 1;

			const CV::PixelBoundingBoxI boundingBox(CV::PixelPositionI(left, top), width, height);

			const int factor = RandomI::random(1, 10) * (RandomI::random(1u) == 0u ? 1 : -1);
			ocean_assert(factor != 0);

			const CV::PixelBoundingBoxI multpliedBoundingBoxA = boundingBox / factor;

			CV::PixelBoundingBoxI multpliedBoundingBoxB = boundingBox;
			multpliedBoundingBoxB /= factor;

			const CV::PixelBoundingBoxI testBoundingBox(left / factor, top / factor, right / factor, bottom / factor);

			if (multpliedBoundingBoxA != testBoundingBox || multpliedBoundingBoxB != testBoundingBox)
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
