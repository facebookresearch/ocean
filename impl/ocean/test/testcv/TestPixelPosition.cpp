/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestPixelPosition.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestPixelPosition::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   PixelPosition test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testWriteToMessenger() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsNeighbor8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInArea9(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNeighbor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultiplication(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDivision(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "PixelPosition test succeeded.";
	}
	else
	{
		Log::info() << "PixelPosition test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestPixelPosition, WriteToMessenger)
{
	EXPECT_TRUE(TestPixelPosition::testWriteToMessenger());
}

TEST(TestPixelPosition, IsNeighbor8)
{
	EXPECT_TRUE(TestPixelPosition::testIsNeighbor8(GTEST_TEST_DURATION));
}

TEST(TestPixelPosition, InArea9)
{
	EXPECT_TRUE(TestPixelPosition::testInArea9(GTEST_TEST_DURATION));
}

TEST(TestPixelPosition, Neighbor)
{
	EXPECT_TRUE(TestPixelPosition::testNeighbor(GTEST_TEST_DURATION));
}

TEST(TestPixelPosition, Multiplication)
{
	EXPECT_TRUE(TestPixelPosition::testMultiplication(GTEST_TEST_DURATION));
}

TEST(TestPixelPosition, Division)
{
	EXPECT_TRUE(TestPixelPosition::testDivision(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestPixelPosition::testWriteToMessenger()
{
	Log::info() << "Write to messenger test:";

	// this is mainly a check whether the code does not compile or crash

	Log::info() << " ";

	Log::info() << CV::PixelPosition(0u, 1u);
	Log::info() << "Position: " << CV::PixelPosition(0u, 1u);
	Log::info() << CV::PixelPosition(0u, 1u) << " <- Position";

	Log::info() << " ";
	Log::info() << "Validation succeeded.";

	return true;
}

bool TestPixelPosition::testIsNeighbor8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Is 8-neighbor test:";

	const unsigned int width = 20u;
	const unsigned int height = 20u;

	bool succeeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x0 = RandomI::random(width - 1u);
			const unsigned int y0 = RandomI::random(height - 1u);

			const unsigned int x1 = RandomI::random(width - 1u);
			const unsigned int y1 = RandomI::random(height - 1u);

			const CV::PixelPosition position0(x0, y0);
			const CV::PixelPosition position1(x1, y1);

			const unsigned int sqrDistance = position0.sqrDistance(position1);

			if (position0.isNeighbor8(position1) != (sqrDistance == 1u || sqrDistance == 2u))
			{
				succeeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int x0 = RandomI::random(-int(width), int(width - 1u));
			const int y0 = RandomI::random(-int(height), int(height - 1u));

			const int x1 = RandomI::random(-int(width), int(width - 1u));
			const int y1 =RandomI::random(-int(height), int(height - 1u));

			const CV::PixelPositionI position0(x0, y0);
			const CV::PixelPositionI position1(x1, y1);

			const unsigned int sqrDistance = position0.sqrDistance(position1);

			if (position0.isNeighbor8(position1) != (sqrDistance == 1u || sqrDistance == 2u))
			{
				succeeded = false;
			}
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

bool TestPixelPosition::testInArea9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Is in 9-area test:";

	const unsigned int width = 20u;
	const unsigned int height = 20u;

	bool succeeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x0 = RandomI::random(width - 1u);
			const unsigned int y0 = RandomI::random(height - 1u);

			const unsigned int x1 = RandomI::random(width - 1u);
			const unsigned int y1 = RandomI::random(height - 1u);

			const CV::PixelPosition position0(x0, y0);
			const CV::PixelPosition position1(x1, y1);

			const unsigned int sqrDistance = position0.sqrDistance(position1);

			if (position0.inArea9(position1) != (sqrDistance <= 2u))
			{
				succeeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int x0 = RandomI::random(-int(width), int(width - 1u));
			const int y0 = RandomI::random(-int(height), int(height - 1u));

			const int x1 = RandomI::random(-int(width), int(width - 1u));
			const int y1 =RandomI::random(-int(height), int(height - 1u));

			const CV::PixelPositionI position0(x0, y0);
			const CV::PixelPositionI position1(x1, y1);

			const unsigned int sqrDistance = position0.sqrDistance(position1);

			if (position0.inArea9(position1) != (sqrDistance <= 2u))
			{
				succeeded = false;
			}
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

bool TestPixelPosition::testNeighbor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Neighbor test:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// testing unsigned PixelPosition

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x = RandomI::random32(randomGenerator);
			const unsigned int y = RandomI::random32(randomGenerator);

			const CV::PixelPosition position(x, y);

			for (unsigned int ccwAngle = 0u; ccwAngle < 360u; ccwAngle += 45u)
			{
				const CV::PixelDirection pixelDirection = CV::PixelDirection(ccwAngle);

				const CV::PixelPosition neighbor = position.neighbor(pixelDirection);

				const Scalar ccwRad = Numeric::deg2rad(Scalar(ccwAngle));

				const Vector2 offset(-Numeric::sin(ccwRad), -Numeric::cos(ccwRad));

				const int64_t xOffset = int64_t(Numeric::round32(offset.x()));
				const int64_t yOffset = int64_t(Numeric::round32(offset.y()));

				const uint32_t xTest = uint32_t(int64_t(position.x()) + xOffset);
				const uint32_t yTest = uint32_t(int64_t(position.y()) + yOffset);

				if (xTest != neighbor.x() || yTest != neighbor.y())
				{
					allSucceeded = false;
				}
			}
		}

		// testing signed PixelPositionI

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int x = int(RandomI::random32(randomGenerator));
			const int y = int(RandomI::random32(randomGenerator));

			const CV::PixelPositionI position(x, y);

			for (unsigned int ccwAngle = 0u; ccwAngle < 360u; ccwAngle += 45u)
			{
				const CV::PixelDirection pixelDirection = CV::PixelDirection(ccwAngle);

				const CV::PixelPositionI neighbor = position.neighbor(pixelDirection);

				const Scalar ccwRad = Numeric::deg2rad(Scalar(ccwAngle));

				const Vector2 offset(-Numeric::sin(ccwRad), -Numeric::cos(ccwRad));

				const int64_t xOffset = int64_t(Numeric::round32(offset.x()));
				const int64_t yOffset = int64_t(Numeric::round32(offset.y()));

				const int32_t xTest = int32_t(int64_t(position.x()) + xOffset);
				const int32_t yTest = int32_t(int64_t(position.y()) + yOffset);

				if (xTest != neighbor.x() || yTest != neighbor.y())
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

bool TestPixelPosition::testMultiplication(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Multiplication operator test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x = RandomI::random(1920u);
			const unsigned int y = RandomI::random(1080u);

			const CV::PixelPosition position(x, y);

			const unsigned int factor = RandomI::random(10u);

			const CV::PixelPosition multpliedPositionA = position * factor;

			CV::PixelPosition multpliedPositionB = position;
			multpliedPositionB *= factor;

			const CV::PixelPosition testPosition(x * factor, y * factor);

			if (multpliedPositionA != testPosition || multpliedPositionB != testPosition)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int x = RandomI::random(-1920, 1920);
			const int y = RandomI::random(-1080, 1080);

			const CV::PixelPositionI position(x, y);

			const int factor = RandomI::random(-10, 10);

			const CV::PixelPositionI multpliedPositionA = position * factor;

			CV::PixelPositionI multpliedPositionB = position;
			multpliedPositionB *= factor;

			const CV::PixelPositionI testPosition(x * factor, y * factor);

			if (multpliedPositionA != testPosition || multpliedPositionB != testPosition)
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

bool TestPixelPosition::testDivision(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Division operator test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x = RandomI::random(1920u);
			const unsigned int y = RandomI::random(1080u);

			const CV::PixelPosition position(x, y);

			const unsigned int factor = RandomI::random(1u, 10u);
			ocean_assert(factor != 0u);

			const CV::PixelPosition multpliedPositionA = position / factor;

			CV::PixelPosition multpliedPositionB = position;
			multpliedPositionB /= factor;

			const CV::PixelPosition testPosition(x / factor, y / factor);

			if (multpliedPositionA != testPosition || multpliedPositionB != testPosition)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const int x = RandomI::random(-1920, 1920);
			const int y = RandomI::random(-1080, 1080);

			const CV::PixelPositionI position(x, y);

			const int factor = RandomI::random(1, 10) * (RandomI::random(1u) == 0u ? 1 : -1);
			ocean_assert(factor != 0);

			const CV::PixelPositionI multpliedPositionA = position / factor;

			CV::PixelPositionI multpliedPositionB = position;
			multpliedPositionB /= factor;

			const CV::PixelPositionI testPosition(x / factor, y / factor);

			if (multpliedPositionA != testPosition || multpliedPositionB != testPosition)
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
