/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsegmentation/TestMaskAnalyzer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/segmentation/MaskCreator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSegmentation
{

bool TestMaskAnalyzer::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Mask analyzer test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHasMaskNeighbor4(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor4Center(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor5(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor5Center(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor8Center(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor9(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasMaskNeighbor9Center(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetermineDistancesToBorder(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFindBorderPixels4(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFindBorderPixels8(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFindNonUniquePixels4(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFindNonUniquePixels8(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFindOutlinePixels4(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComputeChessboardDistanceTransform8Bit(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComputeL1DistanceTransform8Bit(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComputeL2DistanceTransform8Bit(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectBoundingBoxes(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCountMaskPixels(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask analyzer test succeeded.";
	}
	else
	{
		Log::info() << "Mask analyzer test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMaskAnalyzer, HasMaskNeighbor4)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor4(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor4Center)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor4Center(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor5)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor5(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor5Center)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor5Center(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor8)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor8(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor8Center)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor8Center(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor9)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor9(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, HasMaskNeighbor9Center)
{
	EXPECT_TRUE(TestMaskAnalyzer::testHasMaskNeighbor9Center(GTEST_TEST_DURATION));
}


TEST(TestMaskAnalyzer, DetermineDistancesToBorder_200x200_3)
{
	Worker worker;
	constexpr unsigned int explicitMaximalDistance = 3u;
	EXPECT_TRUE(TestMaskAnalyzer::testDetermineDistancesToBorder(200u, 200u, GTEST_TEST_DURATION, explicitMaximalDistance, worker));
}

TEST(TestMaskAnalyzer, DetermineDistancesToBorder_200x200_5)
{
	Worker worker;
	constexpr unsigned int explicitMaximalDistance = 5u;
	EXPECT_TRUE(TestMaskAnalyzer::testDetermineDistancesToBorder(200u, 200u, GTEST_TEST_DURATION, explicitMaximalDistance, worker));
}

TEST(TestMaskAnalyzer, DetermineDistancesToBorder_200x200_10)
{
	Worker worker;
	constexpr unsigned int explicitMaximalDistance = 10u;
	EXPECT_TRUE(TestMaskAnalyzer::testDetermineDistancesToBorder(200u, 200u, GTEST_TEST_DURATION, explicitMaximalDistance, worker));
}

TEST(TestMaskAnalyzer, DetermineDistancesToBorder_200x200_15)
{
	Worker worker;
	constexpr unsigned int explicitMaximalDistance = 15u;
	EXPECT_TRUE(TestMaskAnalyzer::testDetermineDistancesToBorder(200u, 200u, GTEST_TEST_DURATION, explicitMaximalDistance, worker));
}


TEST(TestMaskAnalyzer, FindBorderPixels4_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestMaskAnalyzer::testFindBorderPixels4(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestMaskAnalyzer, FindBorderPixels8_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestMaskAnalyzer::testFindBorderPixels8(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestMaskAnalyzer, FindNonUniquePixels4_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestMaskAnalyzer::testFindNonUniquePixels4(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestMaskAnalyzer, FindNonUniquePixels8_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestMaskAnalyzer::testFindNonUniquePixels8(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestMaskAnalyzer, FindOutlinePixels4_200x200)
{
	EXPECT_TRUE(TestMaskAnalyzer::testFindOutlinePixels4(200u, 200u, GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, ComputeChessboardDistanceTransform8Bit)
{
	EXPECT_TRUE(TestMaskAnalyzer::testComputeChessboardDistanceTransform8Bit(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, ComputeL1DistanceTransform8Bit)
{
	EXPECT_TRUE(TestMaskAnalyzer::testComputeL1DistanceTransform8Bit(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, ComputeL2DistanceTransform8Bit)
{
	EXPECT_TRUE(TestMaskAnalyzer::testComputeL2DistanceTransform8Bit(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, DetectBoundingBoxes)
{
	EXPECT_TRUE(TestMaskAnalyzer::testDetectBoundingBoxes(GTEST_TEST_DURATION));
}

TEST(TestMaskAnalyzer, CountMaskPixels)
{
	EXPECT_TRUE(TestMaskAnalyzer::testCountMaskPixels(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMaskAnalyzer::testHasMaskNeighbor4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-4 test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const CV::PixelPositionsI offsets =
	{
		CV::PixelPositionI(0, -1),
		CV::PixelPositionI(0, 1),
		CV::PixelPositionI(-1, 0),
		CV::PixelPositionI(1, 0),
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor4<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor4<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (const CV::PixelPositionI& offset : offsets)
					{
						const int posX = int(x) + offset.x();
						const int posY = int(y) + offset.y();

						if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
						{
							if (isEqual)
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
								{
									test = true;
								}
							}
							else
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
								{
									test = true;
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor4Center(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-4 center test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const CV::PixelPositionsI offsets =
	{
		CV::PixelPositionI(0, -1),
		CV::PixelPositionI(0, 1),
		CV::PixelPositionI(-1, 0),
		CV::PixelPositionI(1, 0),
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 1u; y < height - 1u; ++y)
			{
				for (unsigned int x = 1u; x < width - 1u; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor4Center<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor4Center<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (const CV::PixelPositionI& offset : offsets)
					{
						const int posX = int(x) + offset.x();
						const int posY = int(y) + offset.y();

						if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
						{
							if (isEqual)
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
								{
									test = true;
								}
							}
							else
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
								{
									test = true;
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-5 test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const CV::PixelPositionsI offsets =
	{
		CV::PixelPositionI(0, 0),
		CV::PixelPositionI(0, -1),
		CV::PixelPositionI(0, 1),
		CV::PixelPositionI(-1, 0),
		CV::PixelPositionI(1, 0),
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor5<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor5<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (const CV::PixelPositionI& offset : offsets)
					{
						const int posX = int(x) + offset.x();
						const int posY = int(y) + offset.y();

						if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
						{
							if (isEqual)
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
								{
									test = true;
								}
							}
							else
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
								{
									test = true;
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor5Center(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-5 center test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const CV::PixelPositionsI offsets =
	{
		CV::PixelPositionI(0, 0),
		CV::PixelPositionI(0, -1),
		CV::PixelPositionI(0, 1),
		CV::PixelPositionI(-1, 0),
		CV::PixelPositionI(1, 0),
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 1u; y < height - 1u; ++y)
			{
				for (unsigned int x = 1u; x < width - 1u; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor5Center<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor5Center<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (const CV::PixelPositionI& offset : offsets)
					{
						const int posX = int(x) + offset.x();
						const int posY = int(y) + offset.y();

						if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
						{
							if (isEqual)
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
								{
									test = true;
								}
							}
							else
							{
								if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
								{
									test = true;
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-8 test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor8<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor8<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (int xx = -1; xx <= 1; ++xx)
					{
						for (int yy = -1; yy <= 1; ++yy)
						{
							if (xx != 0 || yy != 0)
							{
								const int posX = int(x) + xx;
								const int posY = int(y) + yy;

								if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
								{
									if (isEqual)
									{
										if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
										{
											test = true;
										}
									}
									else
									{
										if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
										{
											test = true;
										}
									}
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor8Center(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-8 center test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 1u; y < height - 1u; ++y)
			{
				for (unsigned int x = 1u; x < width - 1u; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor8Center<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor8Center<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (int xx = -1; xx <= 1; ++xx)
					{
						for (int yy = -1; yy <= 1; ++yy)
						{
							if (xx != 0 || yy != 0)
							{
								const int posX = int(x) + xx;
								const int posY = int(y) + yy;

								if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
								{
									if (isEqual)
									{
										if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
										{
											test = true;
										}
									}
									else
									{
										if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
										{
											test = true;
										}
									}
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-9 test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor9<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor9<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (int xx = -1; xx <= 1; ++xx)
					{
						for (int yy = -1; yy <= 1; ++yy)
						{
							const int posX = int(x) + xx;
							const int posY = int(y) + yy;

							if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
							{
								if (isEqual)
								{
									if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
									{
										test = true;
									}
								}
								else
								{
									if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
									{
										test = true;
									}
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testHasMaskNeighbor9Center(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has mask neighbor-9 center test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 2000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

		const Frame mask(generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue));

		for (const bool isEqual : {true, false})
		{
			for (unsigned int y = 1u; y < height - 1u; ++y)
			{
				for (unsigned int x = 1u; x < width - 1u; ++x)
				{
					bool result;

					if (isEqual)
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor9Center<true>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}
					else
					{
						result = CV::Segmentation::MaskAnalyzer::hasMaskNeighbor9Center<false>(mask.constdata<uint8_t>(), width, height, mask.paddingElements(), CV::PixelPosition(x, y), nonMaskValue);
					}

					bool test = false;

					for (int xx = -1; xx <= 1; ++xx)
					{
						for (int yy = -1; yy <= 1; ++yy)
						{
							const int posX = int(x) + xx;
							const int posY = int(y) + yy;

							if (posX >= 0 && posX < int(width) && posY >= 0 && posY < int(height))
							{
								if (isEqual)
								{
									if (mask.constpixel<uint8_t>(posX, posY)[0] == nonMaskValue)
									{
										test = true;
									}
								}
								else
								{
									if (mask.constpixel<uint8_t>(posX, posY)[0] != nonMaskValue)
									{
										test = true;
									}
								}
							}
						}
					}

					if (result != test)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Testing validation: succeeded.";
	}
	else
	{
		Log::info() << "Testing validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testDetermineDistancesToBorder(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	Log::info() << "Mask distance determination test:";

	bool allSucceeded = true;

	for (const unsigned int maximalDistance : {3u, 5u, 10u, 15u, 25u})
	{
		if (!testDetermineDistancesToBorder(width, height, testDuration, maximalDistance, worker))
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testDetermineDistancesToBorder(const unsigned int width, const unsigned int height, const double testDuration, const unsigned int maximalDistance, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	constexpr uint8_t nonMaskValue = 0xFFu;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	Log::info() << " ";
	Log::info() << "... with maximal distance: " << maximalDistance;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 3u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 3u, height);

				const bool assignFinal = RandomI::boolean(randomGenerator);

				const Frame maskFrame = TestMaskAnalyzer::generateTestMask(randomGenerator, testWidth, testHeight);
				Frame distanceFrame(maskFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const CV::PixelBoundingBox boundingBox(CV::MaskAnalyzer::detectBoundingBox(maskFrame.constdata<uint8_t>(), testWidth, testHeight, nonMaskValue, maskFrame.paddingElements()));

				performance.startIf(performanceIteration);
					CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(distanceFrame.data<uint8_t>(), testWidth, testHeight, distanceFrame.paddingElements(), maximalDistance, assignFinal, boundingBox, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(maskFrame, distanceFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateDetermineDistancesToBorder(maskFrame, distanceFrame, maximalDistance, assignFinal))
				{
					allSucceeded = false;
				}
			}

		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED";
	}

	return allSucceeded;
}

bool TestMaskAnalyzer::testFindBorderPixels4(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Find border pixels in 4-neighborhood test:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {false, true})
			{
				const unsigned int widthToUse = performanceIteration ? width : RandomI::random(randomGenerator, 3u, 1920u);
				const unsigned int heightToUse = performanceIteration ? height : RandomI::random(randomGenerator, 3u, 1080u);

				const Frame mask = generateTestMask(randomGenerator, widthToUse, heightToUse);

				CV::PixelPositions borderPixels;
				CV::PixelBoundingBox boundingBox;

				if (!performanceIteration && RandomI::boolean(randomGenerator))
				{
					const unsigned int left = RandomI::random(randomGenerator, mask.width() - 1u);
					const unsigned int top = RandomI::random(randomGenerator, mask.height() - 1u);

					const unsigned int right = RandomI::random(randomGenerator, left, mask.width() - 1u);
					const unsigned int bottom = RandomI::random(randomGenerator, top, mask.height() - 1u);

					boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
				}

				performance.startIf(performanceIteration);
					CV::Segmentation::MaskAnalyzer::findBorderPixels4(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), borderPixels, boundingBox, useWorker);
				performance.stopIf(performanceIteration);

				if (!validateFindBorderPixels4(mask, boundingBox, borderPixels))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
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

bool TestMaskAnalyzer::testFindBorderPixels8(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Find border pixels in 8-neighborhood test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {false, true})
			{
				const unsigned int widthToUse = performanceIteration ? width : RandomI::random(randomGenerator, 3u, 1920u);
				const unsigned int heightToUse = performanceIteration ? height : RandomI::random(randomGenerator, 3u, 1080u);

				const Frame mask = generateTestMask(randomGenerator, widthToUse, heightToUse);

				CV::PixelPositions borderPixels;
				CV::PixelBoundingBox boundingBox;

				if (!performanceIteration && RandomI::boolean(randomGenerator))
				{
					const unsigned int left = RandomI::random(randomGenerator, mask.width() - 1u);
					const unsigned int top = RandomI::random(randomGenerator, mask.height() - 1u);

					const unsigned int right = RandomI::random(randomGenerator, left, mask.width() - 1u);
					const unsigned int bottom = RandomI::random(randomGenerator, top, mask.height() - 1u);

					boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
				}

				performance.startIf(performanceIteration);
					CV::Segmentation::MaskAnalyzer::findBorderPixels8(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), borderPixels, boundingBox, useWorker);
				performance.stopIf(performanceIteration);

				if (!validateFindBorderPixels8(mask, boundingBox, borderPixels))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
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

bool TestMaskAnalyzer::testFindNonUniquePixels4(const unsigned int width, const unsigned int height, const double testDuration, Worker& /*worker*/)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Find non unique pixels in 4-neighborhood test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int widthToUse = performanceIteration ? width : RandomI::random(randomGenerator, 3u, 1920u);
			const unsigned int heightToUse = performanceIteration ? height : RandomI::random(randomGenerator, 3u, 1080u);

			const Frame mask = generateTestMask(randomGenerator, widthToUse, heightToUse);

			CV::PixelPositions nonUniquePixels;
			CV::PixelBoundingBox boundingBox;

			if (!performanceIteration && RandomI::boolean(randomGenerator))
			{
				const unsigned int left = RandomI::random(randomGenerator, mask.width() - 1u);
				const unsigned int top = RandomI::random(randomGenerator, mask.height() - 1u);

				const unsigned int right = RandomI::random(randomGenerator, left, mask.width() - 1u);
				const unsigned int bottom = RandomI::random(randomGenerator, top, mask.height() - 1u);

				boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
			}

			performance.startIf(performanceIteration);
				CV::Segmentation::MaskAnalyzer::findNonUniquePixels4(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), nonUniquePixels, boundingBox);
			performance.stopIf(performanceIteration);

			if (!validateFindNonUniquePixels4(mask, boundingBox, nonUniquePixels))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Singlecore performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

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

bool TestMaskAnalyzer::testFindNonUniquePixels8(const unsigned int width, const unsigned int height, const double testDuration, Worker& /*worker*/)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Find non unique pixels in 8-neighborhood test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {false, true})
		{
			const unsigned int widthToUse = performanceIteration ? width : RandomI::random(randomGenerator, 3u, 1920u);
			const unsigned int heightToUse = performanceIteration ? height : RandomI::random(randomGenerator, 3u, 1080u);

			const Frame mask = generateTestMask(randomGenerator, widthToUse, heightToUse);

			CV::PixelPositions nonUniquePixels;
			CV::PixelBoundingBox boundingBox;

			if (!performanceIteration && RandomI::boolean(randomGenerator))
			{
				const unsigned int left = RandomI::random(randomGenerator, mask.width() - 1u);
				const unsigned int top = RandomI::random(randomGenerator, mask.height() - 1u);

				const unsigned int right = RandomI::random(randomGenerator, left, mask.width() - 1u);
				const unsigned int bottom = RandomI::random(randomGenerator, top, mask.height() - 1u);

				boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
			}

			performance.startIf(performanceIteration);
				CV::Segmentation::MaskAnalyzer::findNonUniquePixels8(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), nonUniquePixels, boundingBox);
			performance.stopIf(performanceIteration);

			if (!validateFindNonUniquePixels8(mask, boundingBox, nonUniquePixels))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Singlecore performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

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

bool TestMaskAnalyzer::testFindOutlinePixels4(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Find outline pixels in 4-neighborhood test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {false, true})
		{
			const unsigned int widthToUse = performanceIteration ? width : RandomI::random(randomGenerator, 3u, 1920u);
			const unsigned int heightToUse = performanceIteration ? height : RandomI::random(randomGenerator, 3u, 1080u);

			const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
			const uint8_t nonMaskValue = uint8_t(((unsigned int)(maskValue) + RandomI::random(randomGenerator, 1u, 255u)) % 256u);

			const Frame mask = generateTestMask(randomGenerator, widthToUse, heightToUse, maskValue, nonMaskValue);

			CV::PixelPositions outlinePixels;
			CV::PixelBoundingBox boundingBox;

			if (!performanceIteration && RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int left = RandomI::random(randomGenerator, mask.width() - 1u);
				const unsigned int top = RandomI::random(randomGenerator, mask.height() - 1u);

				const unsigned int right = RandomI::random(randomGenerator, left, mask.width() - 1u);
				const unsigned int bottom = RandomI::random(randomGenerator, top, mask.height() - 1u);

				boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
			}

			performance.startIf(performanceIteration);
				CV::Segmentation::MaskAnalyzer::findOutline4(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), outlinePixels, boundingBox, nonMaskValue);
			performance.stopIf(performanceIteration);

			if (!validateFindOutlinePixels4(mask, boundingBox, outlinePixels, nonMaskValue))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Singlecore performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms, median: " << performance.medianMseconds() << "ms";

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

bool TestMaskAnalyzer::testComputeChessboardDistanceTransform8Bit(const uint32_t width, const uint32_t height, const double testDuration)
{
	Log::info() << "Testing distance transform (chessboard):";
	Log::info() << " ";

	return testComputeDistanceTransform8Bit(width, height, computeChessboardDistanceTransform8Bit, distanceVerticalHorizontalC, distanceDiagonalC, testDuration);
}

bool TestMaskAnalyzer::testComputeL1DistanceTransform8Bit(const uint32_t width, const uint32_t height, const double testDuration)
{
	Log::info() << "Testing distance transform (L1):";
	Log::info() << " ";

	return testComputeDistanceTransform8Bit(width, height, computeL1DistanceTransform8Bit, distanceVerticalHorizontalL1, distanceDiagonalL1, testDuration);
}

bool TestMaskAnalyzer::testComputeL2DistanceTransform8Bit(const uint32_t width, const uint32_t height, const double testDuration)
{
	Log::info() << "Testing distance transform (L2):";
	Log::info() << " ";

	return testComputeDistanceTransform8Bit(width, height, computeL2DistanceTransform8Bit, distanceVerticalHorizontalL2, distanceDiagonalL2, testDuration);
}

bool TestMaskAnalyzer::testDetectBoundingBoxes(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Determine bounding boxes test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1000u);

		const unsigned int numberBoxes = RandomI::random(randomGenerator, 1u, 20u);

		CV::PixelBoundingBoxes boxes;

		for (unsigned int n = 0u; n < numberBoxes; ++n)
		{
			const unsigned int left = RandomI::random(randomGenerator, 0u, width - 1u);
			const unsigned int top = RandomI::random(randomGenerator, 0u, height - 1u);

			const unsigned int right = RandomI::random(randomGenerator, left, width - 1u);
			const unsigned int bottom = RandomI::random(randomGenerator, top, height - 1u);

			boxes.emplace_back(left, top, right, bottom);
		}

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		yFrame.setValue(0xFF);

		for (const CV::PixelBoundingBox& box : boxes)
		{
			yFrame.subFrame(box.left(), box.top(), box.width(), box.height(), Frame::CM_USE_KEEP_LAYOUT).setValue(0x00);
		}

		const bool useNeighborhood4 = RandomI::random(randomGenerator, 1u) == 1u;
		const bool useNeighborhood8 = !useNeighborhood4;

		// determine unions of all box groups

		typedef std::vector<IndexSet32> IndexSetGroups;

		IndexSetGroups connectedBoxIndexGroups;
		connectedBoxIndexGroups.reserve(boxes.size());

		for (size_t nOuter = 0; nOuter < boxes.size(); ++nOuter)
		{
			IndexSet32 connectedBoxIndices;

			for (size_t nInner = 0; nInner < boxes.size(); ++nInner)
			{
				if (nInner != nOuter && boxes[nOuter].isTouching(boxes[nInner], useNeighborhood8))
				{
					connectedBoxIndices.emplace(Index32(nInner));
				}
			}

			connectedBoxIndexGroups.emplace_back(std::move(connectedBoxIndices));
		}

		CV::PixelBoundingBoxes mergedBoxes(boxes);

		bool foundMerge = true;

		while (foundMerge)
		{
			foundMerge = false;

			for (size_t n = 0; n < mergedBoxes.size(); ++n)
			{
				for (const Index32& connectedBoxIndex : connectedBoxIndexGroups[n])
				{
					CV::PixelBoundingBox mergedBox(mergedBoxes[n] || mergedBoxes[connectedBoxIndex]);

					if (mergedBox != mergedBoxes[n] || mergedBox != mergedBoxes[connectedBoxIndex])
					{
						foundMerge = true;
					}

					mergedBoxes[n] = mergedBox;
					mergedBoxes[connectedBoxIndex] = mergedBox;
				}
			}
		}

		const std::unordered_set<CV::PixelBoundingBox, CV::PixelBoundingBox> boxesSet(mergedBoxes.cbegin(), mergedBoxes.cend());

		const CV::PixelBoundingBoxes detectedBoxes = CV::Segmentation::MaskAnalyzer::detectBoundingBoxes(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), 0x00, useNeighborhood4);

		const BoundingBoxSet detectedBoxesSet(detectedBoxes.cbegin(), detectedBoxes.cend());

		if (boxesSet.size() == detectedBoxes.size())
		{
			for (const CV::PixelBoundingBox& box : boxesSet)
			{
				if (detectedBoxesSet.find(box) == detectedBoxesSet.cend())
				{
					allSucceeded = false;
				}
			}
		}
		else
		{
			allSucceeded = false;
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

bool TestMaskAnalyzer::testCountMaskPixels(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Count mask pixels test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1000u);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const uint8_t nonMaskValue = uint8_t(255u - maskValue);

		const Frame mask = generateTestMask(randomGenerator, width, height, maskValue, nonMaskValue);

		CV::PixelBoundingBox boundingBox;

		if (RandomI::boolean(randomGenerator))
		{
			const unsigned int left = RandomI::random(randomGenerator, width - 1u);
			const unsigned int top = RandomI::random(randomGenerator, height - 1u);

			const unsigned int right = RandomI::random(randomGenerator, left, width - 1u);
			const unsigned int bottom = RandomI::random(randomGenerator, top, height - 1u);

			boundingBox = CV::PixelBoundingBox(left, top, right, bottom);
		}

		const unsigned int maskPixels = CV::Segmentation::MaskAnalyzer::countMaskPixels(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), boundingBox, nonMaskValue);

		unsigned int testMaskPixels = 0u;

		if (boundingBox.isValid())
		{
			for (unsigned int y = boundingBox.top(); y < boundingBox.bottomEnd(); ++y)
			{
				for (unsigned int x = boundingBox.left(); x < boundingBox.rightEnd(); ++x)
				{
					if (mask.constpixel<uint8_t>(x, y)[0] != nonMaskValue)
					{
						++testMaskPixels;
					}

				}
			}
		}
		else
		{
			for (unsigned int y = 0u; y < mask.height(); ++y)
			{
				for (unsigned int x = 0u; x < mask.width(); ++x)
				{
					if (mask.constpixel<uint8_t>(x, y)[0] != nonMaskValue)
					{
						++testMaskPixels;
					}

				}
			}
		}

		if (testMaskPixels != maskPixels)
		{
			allSucceeded = false;
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

Frame TestMaskAnalyzer::generateTestMask(RandomGenerator& randomGenerator, const unsigned int width, const unsigned int height, const uint8_t maskValue, const uint8_t nonMaskValue)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(maskValue != nonMaskValue);

	Frame mask = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
	mask.setValue(nonMaskValue);

	const unsigned int numberBoxes = RandomI::random(randomGenerator, 1u, 30u);

	for (unsigned int n = 0u; n < numberBoxes; ++n)
	{
		const unsigned int boxLeft = RandomI::random(randomGenerator, 0u, width - 1u);
		const unsigned int boxTop = RandomI::random(randomGenerator, 0u, height - 1u);

		const unsigned int boxWidth = RandomI::random(randomGenerator, 1u, width - boxLeft);
		const unsigned int boxHeight = RandomI::random(randomGenerator, 1u, height - boxTop);

		mask.subFrame(boxLeft, boxTop, boxWidth, boxHeight).setValue(maskValue);
	}

	return mask;
}

template <typename TDistanceType>
bool TestMaskAnalyzer::testComputeDistanceTransform8Bit(const uint32_t width, const uint32_t height, ComputeDistanceTransformFunctionPointer<TDistanceType> computeDistanceTransformPtr, const TDistanceType distanceVerticalHorizontal, const TDistanceType distanceDiagonal, const double testDuration)
{
	static_assert(std::is_arithmetic<TDistanceType>::value, "TDistanceType must be integral or floating-point.");

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(computeDistanceTransformPtr != nullptr);
	ocean_assert(distanceVerticalHorizontal > 0 && distanceDiagonal > 0);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	uint64_t iteration = 0ull;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	RandomGenerator randomGenerator;

	do
	{
		const bool measurePerformance = RandomI::random(randomGenerator, 1u) == 0u; // ~50% chance
		const bool useExternalBuffer = iteration % 2ull == 0ull;

		const uint32_t testWidth = measurePerformance ? width : RandomI::random(randomGenerator, 1u, 3072u);
		const uint32_t testHeight = measurePerformance ? height : RandomI::random(randomGenerator, 1u, 3072u);

		const uint8_t referenceValue = uint8_t(RandomI::random(randomGenerator, 255u));

		Memory buffer;

		if (useExternalBuffer)
		{
			buffer = Memory((testWidth + 2u) * (testHeight + 2u) * sizeof(uint32_t));
		}

		ocean_assert(useExternalBuffer != buffer.isNull());

		const bool createDataWithoutReferenceValue = RandomI::random(randomGenerator, 19u) == 0u; // ~5% chance

		uint8_t maskValue = referenceValue;

		if (createDataWithoutReferenceValue)
		{
			maskValue = referenceValue + uint8_t(RandomI::random(randomGenerator, 1u, 254u)); // intentional overflow!
		}

		const Frame sourceFrame = CV::CVUtilities::randomizedBinaryMask(testWidth, testHeight, maskValue, &randomGenerator);
		ocean_assert(sourceFrame == FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, FrameType::genericPixelFormat<TDistanceType, 1u>()), &randomGenerator);

		const Frame targetFrameClone(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		if (measurePerformance)
		{
			performance.start();
		}

		const bool distanceTransformStatus = computeDistanceTransformPtr(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.data<TDistanceType>(), buffer.data<uint32_t>(), referenceValue, sourceFrame.paddingElements(), targetFrame.paddingElements());

		if (measurePerformance)
		{
			performance.stop();
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameClone))
		{
			ocean_assert(false && "Invalid padding elements!");
			allSucceeded = false;
			break;
		}

		if (validateComputeDistanceTransform8Bit(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.constdata<TDistanceType>(), distanceTransformStatus, distanceVerticalHorizontal, distanceDiagonal, referenceValue, sourceFrame.paddingElements(), targetFrame.paddingElements()) == false)
		{
			allSucceeded = false;
		}

		iteration++;
	}
	while (startTimestamp + testDuration > Timestamp(true) || performance.measurements() == 0u);

	Log::info() << "Performance (at " << width << " x " << height << " pixels) [p05, p50, p95, avg] : [ " << String::toAString(performance.percentileMseconds(0.05), 2u) << ", " << String::toAString(performance.percentileMseconds(0.05), 2u) << ", " << String::toAString(performance.percentileMseconds(0.05), 2u) << ", " << String::toAString(performance.averageMseconds(), 2u) << " ] ms";

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

bool TestMaskAnalyzer::validateDetermineDistancesToBorder(const Frame& mask, const Frame& distance, const unsigned int maximalDistance, const bool assignFinal)
{
	ocean_assert(mask.isValid() && mask.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(mask.isFrameTypeCompatible(distance.frameType(), false /*allowDifferentPixelOrigins*/));

	if (!mask.isValid() || !mask.isPixelFormatCompatible(FrameType::FORMAT_Y8) || !mask.isFrameTypeCompatible(distance.frameType(), false /*allowDifferentPixelOrigins*/))
	{
		return false;
	}

	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		for (unsigned int x = 0u; x < mask.width(); ++x)
		{
			if (mask.constpixel<uint8_t>(x, y)[0] != 0xFF)
			{
				unsigned int testDistance = 1u;

				while (testDistance <= maximalDistance)
				{
					bool nonMaskFound = false;

					// top row
					if (!nonMaskFound && y >= testDistance)
					{
						const unsigned int yy = y - testDistance;
						for (int xx = max(0, int(x) - int(testDistance)); xx <= min(int(x) + int(testDistance), int(mask.width() - 1)); ++xx)
						{
							if (mask.constpixel<uint8_t>(xx, yy)[0] == 0xFF)
							{
								nonMaskFound = true;
								break;
							}
						}
					}

					// bottom row
					if (!nonMaskFound && y + testDistance < mask.height())
					{
						const unsigned int yy = y + testDistance;
						for (int xx = max(0, int(x) - int(testDistance)); xx <= min(int(x) + int(testDistance), int(mask.width() - 1)); ++xx)
						{
							if (mask.constpixel<uint8_t>(xx, yy)[0] == 0xFF)
							{
								nonMaskFound = true;
								break;
							}
						}
					}

					// left column
					if (!nonMaskFound && x >= testDistance)
					{
						const unsigned int xx = x - testDistance;
						for (int yy = max(0, int(y) - int(testDistance)); yy <= min(int(y) + int(testDistance), int(mask.height() - 1)); ++yy)
						{
							if (mask.constpixel<uint8_t>(xx, yy)[0] == 0xFF)
							{
								nonMaskFound = true;
								break;
							}
						}
					}

					// right column
					if (!nonMaskFound && x + testDistance < mask.width())
					{
						const unsigned int xx = x + testDistance;
						for (int yy = max(0, int(y) - int(testDistance)); yy <= min(int(y) + int(testDistance), int(mask.height() - 1)); ++yy)
						{
							if (mask.constpixel<uint8_t>(xx, yy)[0] == 0xFF)
							{
								nonMaskFound = true;
								break;
							}
						}
					}

					if (nonMaskFound)
					{
						break;
					}

					++testDistance;
				}

				if (x + 1u < testDistance)
				{
					testDistance = x + 1u;
				}

				if (y + 1u < testDistance)
				{
					testDistance = y + 1u;
				}

				if (mask.width() - x < testDistance)
				{
					testDistance = mask.width() - x;
				}

				if (mask.height() - y < testDistance)
				{
					testDistance = mask.height() - y;
				}

				if (testDistance > maximalDistance)
				{
					if (assignFinal)
					{
						if (distance.constpixel<uint8_t>(x, y)[0] != maximalDistance)
						{
							return false;
						}
					}
					else
					{
						if (distance.constpixel<uint8_t>(x, y)[0] != 0)
						{
							return false;
						}
					}
				}
				else
				{
					if (distance.constpixel<uint8_t>(x, y)[0] != testDistance)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestMaskAnalyzer::validateFindBorderPixels4(const Frame& mask, const CV::PixelBoundingBox& optionalBoundingBox, const CV::PixelPositions& borderPixels)
{
	ocean_assert(mask.isValid());

	const CV::PixelBoundingBox boundingBox = optionalBoundingBox.isValid() ? optionalBoundingBox : CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), mask.width(), mask.height());

	const PixelPositionSet borderPixelSet(borderPixels.cbegin(), borderPixels.cend());

	ocean_assert(borderPixelSet.size() == borderPixels.size());
	if (borderPixelSet.size() != borderPixels.size())
	{
		return false;
	}

	CV::PixelPositions testPixels;
	testPixels.reserve(1024);

	const unsigned int width_1 = mask.width() - 1u;
	const unsigned int height_1 = mask.height() - 1u;

	// top and bottom row
	for (unsigned int x = max(1u, boundingBox.left()); x < min(boundingBox.rightEnd(), width_1); ++x)
	{
		if (boundingBox.top() == 0u)
		{
			if (mask.constrow<uint8_t>(0u)[x] != 0xFF)
			{
				testPixels.emplace_back(x, 0u);
			}
		}

		if (boundingBox.bottom() == height_1)
		{
			if (mask.constrow<uint8_t>(height_1)[x] != 0xFF)
			{
				testPixels.emplace_back(x, height_1);
			}
		}
	}

	// left and right column
	for (unsigned int y = max(1u, boundingBox.top()); y < min(boundingBox.bottomEnd(), height_1); ++y)
	{
		if (boundingBox.left() == 0u)
		{
			if (mask.constrow<uint8_t>(y)[0] != 0xFF)
			{
				testPixels.emplace_back(0u, y);
			}
		}

		if (boundingBox.right() == width_1)
		{
			if (mask.constrow<uint8_t>(y)[width_1] != 0xFF)
			{
				testPixels.emplace_back(width_1, y);
			}
		}
	}

	// top left corner
	if (boundingBox.left() == 0u && boundingBox.top() == 0u)
	{
		if (mask.constpixel<uint8_t>(0, 0)[0] != 0xFF)
		{
			testPixels.emplace_back(0u, 0u);
		}
	}

	// top right corner
	if (boundingBox.rightEnd() == mask.width() && boundingBox.top() == 0u)
	{
		if (mask.constpixel<uint8_t>(width_1, 0u)[0] != 0xFF)
		{
			testPixels.emplace_back(width_1, 0u);
		}
	}

	// bottom left corner
	if (boundingBox.left() == 0u && boundingBox.bottomEnd() == mask.height())
	{
		if (mask.constpixel<uint8_t>(0u, height_1)[0] != 0xFF)
		{
			testPixels.emplace_back(0u, height_1);
		}
	}

	// bottom right corner
	if (boundingBox.rightEnd() == mask.width() && boundingBox.bottomEnd() == mask.height())
	{
		if (mask.constpixel<uint8_t>(width_1, height_1)[0] != 0xFF)
		{
			testPixels.emplace_back(width_1, height_1);
		}
	}

	for (unsigned int y = 1u; y < height_1; ++y)
	{
		const uint8_t* const row0 = mask.constrow<uint8_t>(y - 1u);
		const uint8_t* const row1 = mask.constrow<uint8_t>(y);
		const uint8_t* const row2 = mask.constrow<uint8_t>(y + 1u);

		for (unsigned int x = 1u; x < width_1; ++x)
		{
			if (row1[x] != 0xFF)
			{
				if (row0[x] == 0xFF || row1[x - 1u] == 0xFF || row1[x + 1u] == 0xFF || row2[x] == 0xFF)
				{
					testPixels.emplace_back(x, y);
				}
			}
		}
	}

	const PixelPositionSet testPixelSet(testPixels.cbegin(), testPixels.cend());

	ocean_assert(testPixelSet.size() == testPixels.size());
	if (testPixelSet.size() != testPixels.size())
	{
		return false;
	}

	if (borderPixels.size() != borderPixelSet.size())
	{
		return false;
	}

	for (const CV::PixelPosition& borderPixel : borderPixels)
	{
		if (testPixelSet.find(borderPixel) == testPixelSet.cend())
		{
			return false;
		}
	}

	return true;
}

bool TestMaskAnalyzer::validateFindBorderPixels8(const Frame& mask, const CV::PixelBoundingBox& optionalBoundingBox, const CV::PixelPositions& borderPixels)
{
	ocean_assert(mask.isValid());

	const CV::PixelBoundingBox boundingBox = optionalBoundingBox.isValid() ? optionalBoundingBox : CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), mask.width(), mask.height());

	const PixelPositionSet borderPixelSet(borderPixels.cbegin(), borderPixels.cend());

	ocean_assert(borderPixelSet.size() == borderPixels.size());
	if (borderPixelSet.size() != borderPixels.size())
	{
		return false;
	}

	CV::PixelPositions testPixels;
	testPixels.reserve(1024);

	const unsigned int width_1 = mask.width() - 1u;
	const unsigned int height_1 = mask.height() - 1u;

	// top and bottom row
	for (unsigned int x = max(1u, boundingBox.left()); x < min(boundingBox.rightEnd(), width_1); ++x)
	{
		if (boundingBox.top() == 0u)
		{
			if (mask.constrow<uint8_t>(0u)[x] != 0xFF)
			{
				testPixels.emplace_back(x, 0u);
			}
		}

		if (boundingBox.bottom() == height_1)
		{
			if (mask.constrow<uint8_t>(height_1)[x] != 0xFF)
			{
				testPixels.emplace_back(x, height_1);
			}
		}
	}

	// left and right column
	for (unsigned int y = max(1u, boundingBox.top()); y < min(boundingBox.bottomEnd(), height_1); ++y)
	{
		if (boundingBox.left() == 0u)
		{
			if (mask.constrow<uint8_t>(y)[0] != 0xFF)
			{
				testPixels.emplace_back(0u, y);
			}
		}

		if (boundingBox.right() == width_1)
		{
			if (mask.constrow<uint8_t>(y)[width_1] != 0xFF)
			{
				testPixels.emplace_back(width_1, y);
			}
		}
	}

	// top left corner
	if (boundingBox.left() == 0u && boundingBox.top() == 0u)
	{
		if (mask.constpixel<uint8_t>(0, 0)[0] != 0xFF)
		{
			testPixels.emplace_back(0u, 0u);
		}
	}

	// top right corner
	if (boundingBox.rightEnd() == mask.width() && boundingBox.top() == 0u)
	{
		if (mask.constpixel<uint8_t>(width_1, 0u)[0] != 0xFF)
		{
			testPixels.emplace_back(width_1, 0u);
		}
	}

	// bottom left corner
	if (boundingBox.left() == 0u && boundingBox.bottomEnd() == mask.height())
	{
		if (mask.constpixel<uint8_t>(0u, height_1)[0] != 0xFF)
		{
			testPixels.emplace_back(0u, height_1);
		}
	}

	// bottom right corner
	if (boundingBox.rightEnd() == mask.width() && boundingBox.bottomEnd() == mask.height())
	{
		if (mask.constpixel<uint8_t>(width_1, height_1)[0] != 0xFF)
		{
			testPixels.emplace_back(width_1, height_1);
		}
	}

	for (unsigned int y = 1u; y < height_1; ++y)
	{
		const uint8_t* const row0 = mask.constrow<uint8_t>(y - 1u);
		const uint8_t* const row1 = mask.constrow<uint8_t>(y);
		const uint8_t* const row2 = mask.constrow<uint8_t>(y + 1u);

		for (unsigned int x = 1u; x < width_1; ++x)
		{
			if (row1[x] != 0xFF)
			{
				if (row0[x - 1u] == 0xFF || row0[x] == 0xFF || row0[x + 1u] == 0xFF
						|| row1[x - 1u] == 0xFF || row1[x + 1u] == 0xFF
						|| row2[x - 1u] == 0xFF || row2[x] == 0xFF || row2[x + 1u] == 0xFF)
				{
					testPixels.emplace_back(x, y);
				}
			}
		}
	}

	const PixelPositionSet testPixelSet(testPixels.cbegin(), testPixels.cend());

	ocean_assert(testPixelSet.size() == testPixels.size());
	if (testPixelSet.size() != testPixels.size())
	{
		return false;
	}

	if (borderPixels.size() != borderPixelSet.size())
	{
		return false;
	}

	for (const CV::PixelPosition& borderPixel : borderPixels)
	{
		if (testPixelSet.find(borderPixel) == testPixelSet.cend())
		{
			return false;
		}
	}

	return true;
}

bool TestMaskAnalyzer::validateFindNonUniquePixels4(const Frame& mask, const CV::PixelBoundingBox& optionalBoundingBox, const CV::PixelPositions& nonUniquePixels)
{
	ocean_assert(mask.isValid());

	const CV::PixelBoundingBox boundingBox = optionalBoundingBox.isValid() ? optionalBoundingBox : CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), mask.width(), mask.height());

	const PixelPositionSet nonUniquePixelsSet(nonUniquePixels.cbegin(), nonUniquePixels.cend());

	ocean_assert(nonUniquePixelsSet.size() == nonUniquePixels.size());
	if (nonUniquePixelsSet.size() != nonUniquePixels.size())
	{
		return false;
	}

	CV::PixelPositions testPixels;
	testPixels.reserve(1024);

	const unsigned int width_1 = mask.width() - 1u;
	const unsigned int height_1 = mask.height() - 1u;

	// top and bottom row
	for (unsigned int x = max(1u, boundingBox.left()); x < min(boundingBox.rightEnd(), width_1); ++x)
	{
		if (boundingBox.top() == 0u)
		{
			const uint8_t value = mask.constpixel<uint8_t>(x, 0u)[0u];

			if (value != mask.constpixel<uint8_t>(x - 1u, 0u)[0u] || value != mask.constpixel<uint8_t>(x + 1u, 0u)[0u] || value != mask.constpixel<uint8_t>(x, 1u)[0])
			{
				testPixels.emplace_back(x, 0u);
			}
		}

		if (boundingBox.bottom() == height_1)
		{
			const uint8_t value = mask.constpixel<uint8_t>(x, height_1)[0u];

			if (value != mask.constpixel<uint8_t>(x - 1u, height_1)[0u] || value != mask.constpixel<uint8_t>(x + 1u, height_1)[0u] || value != mask.constpixel<uint8_t>(x, height_1 - 1u)[0u])
			{
				testPixels.emplace_back(x, height_1);
			}
		}
	}

	// left and right column
	for (unsigned int y = max(1u, boundingBox.top()); y < min(boundingBox.bottomEnd(), height_1); ++y)
	{
		if (boundingBox.left() == 0u)
		{
			const uint8_t value = mask.constpixel<uint8_t>(0u, y)[0];

			if (value != mask.constpixel<uint8_t>(0u, y - 1u)[0] || value != mask.constpixel<uint8_t>(0u, y + 1u)[0] || value != mask.constpixel<uint8_t>(1u, y)[0])
			{
				testPixels.emplace_back(0u, y);
			}
		}

		if (boundingBox.right() == width_1)
		{
			const uint8_t value = mask.constpixel<uint8_t>(width_1, y)[0u];

			if (value != mask.constpixel<uint8_t>(width_1, y - 1u)[0u] || value != mask.constpixel<uint8_t>(width_1, y + 1u)[0u] || value != mask.constpixel<uint8_t>(width_1 - 1u, y)[0u])
			{
				testPixels.emplace_back(width_1, y);
			}
		}
	}

	// top left corner
	if (boundingBox.left() == 0u && boundingBox.top() == 0u)
	{
		const uint8_t value = mask.constpixel<uint8_t>(0u, 0u)[0];

		if (value != mask.constpixel<uint8_t>(1u, 0u)[0] || value != mask.constpixel<uint8_t>(0u, 1u)[0])
		{
			testPixels.emplace_back(0u, 0u);
		}
	}

	// top right corner
	if (boundingBox.rightEnd() == mask.width() && boundingBox.top() == 0u)
	{
		const uint8_t value = mask.constpixel<uint8_t>(width_1, 0u)[0];

		if (value != mask.constpixel<uint8_t>(width_1, 1u)[0] || value != mask.constpixel<uint8_t>(width_1 - 1u, 0u)[0])
		{
			testPixels.emplace_back(width_1, 0u);
		}
	}

	// bottom left corner
	if (boundingBox.left() == 0u && boundingBox.bottomEnd() == mask.height())
	{
		const uint8_t value = mask.constpixel<uint8_t>(0u, height_1)[0];

		if (value != mask.constpixel<uint8_t>(1u, height_1)[0] || value != mask.constpixel<uint8_t>(0u, height_1 - 1u)[0])
		{
			testPixels.emplace_back(0u, height_1);
		}
	}

	// bottom right corner
	if (boundingBox.rightEnd() == mask.width() && boundingBox.bottomEnd() == mask.height())
	{
		const uint8_t value = mask.constpixel<uint8_t>(width_1, height_1)[0];

		if (value != mask.constpixel<uint8_t>(width_1 - 1u, height_1)[0] || value != mask.constpixel<uint8_t>(width_1, height_1 - 1u)[0])
		{
			testPixels.emplace_back(width_1, height_1);
		}
	}

	for (unsigned int y = 1u; y < height_1; ++y)
	{
		const uint8_t* const row0 = mask.constrow<uint8_t>(y - 1u);
		const uint8_t* const row1 = mask.constrow<uint8_t>(y);
		const uint8_t* const row2 = mask.constrow<uint8_t>(y + 1u);

		for (unsigned int x = 1u; x < width_1; ++x)
		{
			const uint8_t value = row1[x];

			if (row0[x] != value || row1[x - 1u] != value || row1[x + 1u] != value || row2[x] != value)
			{
				testPixels.emplace_back(x, y);
			}
		}
	}

	const PixelPositionSet testPixelSet(testPixels.cbegin(), testPixels.cend());

	ocean_assert(testPixelSet.size() == testPixels.size());
	if (testPixelSet.size() != testPixels.size())
	{
		return false;
	}

	if (nonUniquePixels.size() != nonUniquePixelsSet.size())
	{
		return false;
	}

	for (const CV::PixelPosition& nonUniquePixel : nonUniquePixels)
	{
		if (testPixelSet.find(nonUniquePixel) == testPixelSet.cend())
		{
			return false;
		}
	}

	return true;
}

bool TestMaskAnalyzer::validateFindNonUniquePixels8(const Frame& mask, const CV::PixelBoundingBox& optionalBoundingBox, const CV::PixelPositions& nonUniquePixels)
{
	ocean_assert(mask.isValid());

	const CV::PixelBoundingBox boundingBox = optionalBoundingBox.isValid() ? optionalBoundingBox : CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), mask.width(), mask.height());

	const PixelPositionSet nonUniquePixelsSet(nonUniquePixels.cbegin(), nonUniquePixels.cend());

	ocean_assert(nonUniquePixelsSet.size() == nonUniquePixels.size());
	if (nonUniquePixelsSet.size() != nonUniquePixels.size())
	{
		return false;
	}

	CV::PixelPositions testPixels;
	testPixels.reserve(1024);

	for (unsigned int y = boundingBox.top(); y < boundingBox.bottomEnd(); ++y)
	{
		for (unsigned int x = boundingBox.left(); x < boundingBox.rightEnd(); ++x)
		{
			const uint8_t value = mask.constpixel<uint8_t>(x, y)[0u];

			bool isUnique = true;

			for (int yy = -1; isUnique && yy <= 1; ++yy)
			{
				const unsigned int yLookup = (unsigned int)(int(y) + yy);

				if (yLookup < mask.height())
				{
					for (int xx = -1; isUnique && xx <=1; ++xx)
					{
						const unsigned int xLookup = (unsigned int)(int(x) + xx);

						if (xLookup < mask.width())
						{
							if (value != mask.constpixel<uint8_t>(xLookup, yLookup)[0u])
							{
								isUnique = false;
							}
						}
					}
				}
			}

			if (!isUnique)
			{
				testPixels.emplace_back(x, y);
			}
		}
	}

	const PixelPositionSet testPixelSet(testPixels.cbegin(), testPixels.cend());

	ocean_assert(testPixelSet.size() == testPixels.size());
	if (testPixelSet.size() != testPixels.size())
	{
		return false;
	}

	if (nonUniquePixels.size() != nonUniquePixelsSet.size())
	{
		return false;
	}

	for (const CV::PixelPosition& nonUniquePixel : nonUniquePixels)
	{
		if (testPixelSet.find(nonUniquePixel) == testPixelSet.cend())
		{
			return false;
		}
	}

	return true;
}

bool TestMaskAnalyzer::validateFindOutlinePixels4(const Frame& mask, const CV::PixelBoundingBox& optionalBoundingBox, const CV::PixelPositions& outlinePixels, const uint8_t nonMaskValue)
{
	ocean_assert(mask.isValid());

	const CV::PixelBoundingBox boundingBox = optionalBoundingBox.isValid() ? optionalBoundingBox : CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), mask.width(), mask.height());

	{
		PixelPositionSet outlinePixelsSet;
		for (auto a : outlinePixels)
		{
			ocean_assert(outlinePixelsSet.find(a) == outlinePixelsSet.cend());
			outlinePixelsSet.emplace(a);
		}
	}

	const PixelPositionSet outlinePixelsSet(outlinePixels.cbegin(), outlinePixels.cend());

	ocean_assert(outlinePixelsSet.size() == outlinePixels.size());
	if (outlinePixelsSet.size() != outlinePixels.size())
	{
		return false;
	}

	CV::PixelPositions testPixels;
	testPixels.reserve(1024);

	for (int y = int(boundingBox.top()) - 1; y <= int(boundingBox.bottomEnd()); ++y)
	{
		for (int x = int(boundingBox.left() - 1); x <= int(boundingBox.rightEnd()); ++x)
		{
			bool isOutline = false;

			for (int yy = -1; !isOutline && yy <= 1; ++yy)
			{
				const unsigned int yLookup = (unsigned int)(int(y) + yy);

				if (yLookup < mask.height())
				{
					for (int xx = -1; !isOutline && xx <=1; ++xx)
					{
						const unsigned int xLookup = (unsigned int)(int(x) + xx);

						if (xLookup < mask.width())
						{
							if (mask.constpixel<uint8_t>(xLookup, yLookup)[0u] != nonMaskValue)
							{
								isOutline = false;
							}
						}
					}
				}
			}

			if (!isOutline)
			{
				testPixels.emplace_back((unsigned int)(x), (unsigned int)(y));
			}
		}
	}

	const PixelPositionSet testPixelSet(testPixels.cbegin(), testPixels.cend());

	ocean_assert(testPixelSet.size() == testPixels.size());
	if (testPixelSet.size() != testPixels.size())
	{
		return false;
	}

	if (outlinePixels.size() != outlinePixelsSet.size())
	{
		return false;
	}

	for (const CV::PixelPosition& outlinePixel : outlinePixels)
	{
		if (testPixelSet.find(outlinePixel) == testPixelSet.cend())
		{
			return false;
		}
	}

	return true;
}

template <typename TDistanceType>
bool TestMaskAnalyzer::validateComputeDistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, const TDistanceType* validationTarget, const bool validationTargetStatus, const TDistanceType distanceVerticalHorizontal, const TDistanceType distanceDiagonal, const uint8_t referenceValue, const uint32_t sourcePaddingElements, const uint32_t validationTargetPaddingElements)
{
	if (source == nullptr || width == 0u || height == 0u || validationTarget == nullptr)
	{
		ocean_assert(false && "Invalid input data");
		return false;
	}

// Note: there are three different implementations available for this test. However, since the brute-force and "grow" method
//       are slow the test uses an implementation that is similar to be function being tested. The other implementations are
//       left for reference.
//
//#define USE_BRUTE_FORCE_METHOD
//#define USE_GROW_METHOD

#if defined(USE_BRUTE_FORCE_METHOD)

	if (distanceVerticalHorizontal <= TDistanceType(0) || distanceDiagonal <= TDistanceType(0))
	{
		ocean_assert(false && "Distance values must be positive");
		return false;
	}

	const uint32_t sourceStrideElements = width + sourcePaddingElements;
	const uint32_t validationTargetStrideElements = width + validationTargetPaddingElements;

	CV::PixelPositions referencePixels;

	for (uint32_t y = 0u; y < height; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements;

		for (uint32_t x = 0u; x < width; ++x)
		{
			if (sourceRow[x] == referenceValue)
			{
				referencePixels.emplace_back(x, y);
			}
		}
	}

	if (referencePixels.empty())
	{
		return validationTargetStatus == false;
	}

	Memory memory(width * height * sizeof(TDistanceType));
	TDistanceType* buffer = (TDistanceType*)memory.data();

	constexpr TDistanceType boundaryValue = NumericT<TDistanceType>::maxValue() / TDistanceType(2);
	for (uint32_t i = 0u; i < height * width; ++i)
	{
		buffer[i] = boundaryValue;
	}

	bool allSucceeded = true;

	for (uint32_t y = 0u; y < height; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements;
		const TDistanceType* validationTargetRow = validationTarget + y * validationTargetStrideElements;

		for (uint32_t x = 0u; x < width; ++x)
		{
			if (sourceRow[x] == referenceValue)
			{
				buffer[y * width + x] = TDistanceType(0);
			}
			else
			{
				TDistanceType distanceToNearestReferencePixel = NumericT<TDistanceType>::maxValue();

				for (const CV::PixelPosition& referencePixel : referencePixels)
				{
					//                      offsetX
					//   p1---------------------------------------+
					//     \                                      |
					//      \ <- diagonalSteps                    | offsetY
					//       \____________________________________|
					//              ^-- straightSteps             p2
					//
					// This also covers the transposed case.
					const uint32_t offsetX = uint32_t(std::abs(int64_t(referencePixel.x()) - int64_t(x)));
					const uint32_t offsetY = uint32_t(std::abs(int64_t(referencePixel.y()) - int64_t(y)));

					const uint32_t diagonalSteps = std::min(offsetX, offsetY);
					const uint32_t straightSteps = std::max(offsetX, offsetY) - diagonalSteps;

					const TDistanceType distance = TDistanceType(diagonalSteps) * distanceDiagonal + TDistanceType(straightSteps) * distanceVerticalHorizontal;

					if (distance < distanceToNearestReferencePixel)
					{
						distanceToNearestReferencePixel = distance;
					}

					ocean_assert(distanceToNearestReferencePixel < NumericT<TDistanceType>::maxValue());

					buffer[y * width + x] = distanceToNearestReferencePixel;
				}
			}

			const TDistanceType bufferValue = buffer[y * width + x];
			const TDistanceType validationTargetValue = validationTargetRow[x];
			if (NumericT<TDistanceType>::isWeakEqual(validationTargetValue, bufferValue) == false)
			{
				allSucceeded = false;
			}
		}
	}

	if (validationTargetStatus == false)
	{
		allSucceeded = false;
	}

	return allSucceeded;

#elif defined(USE_GROW_METHOD)

	if (distanceVerticalHorizontal <= TDistanceType(0) || distanceDiagonal <= TDistanceType(0))
	{
		ocean_assert(false && "Distance values must be positive");
		return false;
	}

	const uint32_t sourceStrideElements = width + sourcePaddingElements;
	const uint32_t validationTargetStrideElements = width + validationTargetPaddingElements;

	CV::PixelPositions pixelQueue;

	for (uint32_t y = 0u; y < height; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements;

		for (uint32_t x = 0u; x < width; ++x)
		{
			if (sourceRow[x] == referenceValue)
			{
				pixelQueue.emplace_back(x, y);
			}
		}
	}

	if (pixelQueue.empty())
	{
		return validationTargetStatus == false;
	}

	Memory memory(width * height * sizeof(TDistanceType));
	TDistanceType* buffer = (TDistanceType*)memory.data();

	const TDistanceType* bufferEnd = buffer + height * width;

	const TDistanceType invalidDistance = NumericT<TDistanceType>::maxValue();
	for (uint32_t i = 0u; i < height * width; ++i)
	{
		buffer[i] = invalidDistance;
	}

	bool allSucceeded = true;

	const uint8_t* const sourceEnd = source + (height - 1u) * sourceStrideElements + width;
	OCEAN_SUPPRESS_UNUSED_WARNING(sourceEnd);

	std::set<CV::PixelPosition> nextPixelQueue;

	while (pixelQueue.empty() == false)
	{
		std::set<CV::PixelPosition> visitedNeighbors;

		for (const CV::PixelPosition& pixel : pixelQueue)
		{
			ocean_assert(pixel.x() < width && pixel.y() < height);
			const uint32_t sourcePixelIndex = pixel.y() * sourceStrideElements + pixel.x();
			const uint32_t bufferPixelIndex = pixel.y() * width + pixel.x();
			ocean_assert_and_suppress_unused(source + sourcePixelIndex < sourceEnd && buffer + bufferPixelIndex < bufferEnd, bufferEnd);

			CV::PixelPositions neighborsVerticalHorizontal;
			CV::PixelPositions neighborsDiagonal;

			if (pixel.y() > 0u)
			{
				neighborsVerticalHorizontal.emplace_back(pixel.north());

				if (pixel.x() > 0u)
				{
					neighborsDiagonal.emplace_back(pixel.northWest());
				}

				if (pixel.x() < width - 1u)
				{
					neighborsDiagonal.emplace_back(pixel.northEast());
				}
			}

			if (pixel.y() < height - 1u)
			{
				neighborsVerticalHorizontal.emplace_back(pixel.south());

				if (pixel.x() > 0u)
				{
					neighborsDiagonal.emplace_back(pixel.southWest());
				}

				if (pixel.x() < width - 1u)
				{
					neighborsDiagonal.emplace_back(pixel.southEast());
				}
			}

			if (pixel.x() > 0u)
			{
				neighborsVerticalHorizontal.emplace_back(pixel.west());
			}

			if (pixel.x() < width - 1u)
			{
				neighborsVerticalHorizontal.emplace_back(pixel.east());
			}

			ocean_assert(neighborsVerticalHorizontal.size() != 0 && neighborsDiagonal.size() != 0);

			TDistanceType minimumDistance = invalidDistance;

			if (source[sourcePixelIndex] == referenceValue)
			{
				minimumDistance = TDistanceType(0);
			}
			else
			{
				for (const CV::PixelPosition& neighbor : neighborsDiagonal)
				{
					ocean_assert(neighbor.x() < width && neighbor.y() < height);
					const uint32_t bufferNeighborIndex = neighbor.y() * width + neighbor.x();

					if (buffer[bufferNeighborIndex] != invalidDistance)
					{
						const TDistanceType distance = buffer[bufferNeighborIndex] + distanceDiagonal;

						if (distance < minimumDistance)
						{
							minimumDistance = distance;
						}
					}
				}

				for (const CV::PixelPosition& neighbor : neighborsVerticalHorizontal)
				{
					ocean_assert(neighbor.x() < width && neighbor.y() < height);
					const uint32_t bufferNeighborIndex = neighbor.y() * width + neighbor.x();

					if (buffer[bufferNeighborIndex] != invalidDistance)
					{
						const TDistanceType distance = buffer[bufferNeighborIndex] + distanceDiagonal;

						if (distance < minimumDistance)
						{
							minimumDistance = distance;
						}
					}
				}
			}

			ocean_assert(minimumDistance != invalidDistance);
			buffer[bufferPixelIndex] = minimumDistance;

			for (const CV::PixelPosition& neighbor : neighborsDiagonal)
			{
				const uint32_t bufferNeighborIndex = neighbor.y() * width + neighbor.x();

				if (buffer[bufferNeighborIndex] == invalidDistance && visitedNeighbors.find(neighbor) == visitedNeighbors.cend())
				{
					visitedNeighbors.insert(neighbor);
				}
			}

			for (const CV::PixelPosition& neighbor : neighborsVerticalHorizontal)
			{
				const uint32_t bufferNeighborIndex = neighbor.y() * width + neighbor.x();

				if (buffer[bufferNeighborIndex] == invalidDistance && visitedNeighbors.find(neighbor) == visitedNeighbors.cend())
				{
					visitedNeighbors.insert(neighbor);
				}
			}
		}

		pixelQueue = CV::PixelPositions(visitedNeighbors.cbegin(), visitedNeighbors.cend());
	}

#if defined(OCEAN_DEBUG)
	// Make sure all pixels have been processed
	for (uint32_t i = 0u; i < height * width; ++i)
	{
		ocean_assert(buffer[i] < invalidDistance);
	}
#endif

	if (validationTargetStatus == false)
	{
		allSucceeded = false;
	}

	return allSucceeded;

#else

	bool allSucceeded = true;

	const uint32_t bufferWidth = width + 2u;
	const uint32_t bufferHeight = height + 2u;

	Memory memory(bufferWidth * bufferHeight * sizeof(TDistanceType));
	TDistanceType* buffer = (TDistanceType*)memory.data();

	if (distanceVerticalHorizontal <= TDistanceType(0) || distanceDiagonal <= TDistanceType(0))
	{
		ocean_assert(false && "Distance values must be positive");
		return false;
	}

	const TDistanceType* bufferEnd = buffer + bufferHeight * bufferWidth;

	constexpr TDistanceType boundaryValue = NumericT<TDistanceType>::maxValue() / TDistanceType(2);
	for (uint32_t i = 0u; i < bufferHeight * bufferWidth; ++i)
	{
		buffer[i] = boundaryValue;
	}

	const uint32_t sourceStrideElements = width + sourcePaddingElements;
	const uint32_t validationTargetStrideElements = width + validationTargetPaddingElements;

	const uint8_t* const sourceEnd = source + (height - 1u) * sourceStrideElements + width;
	const TDistanceType* const validationTargetEnd = validationTarget + (height - 1u) * validationTargetStrideElements + width;

	// Forward pass
	bool foundReferenceValue = false;
	for (uint32_t y = 0u; y < height; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements;
		TDistanceType* bufferRow = buffer + (y + 1u) * bufferWidth;

		// Initialize the left-most and right-most columns of the current row of the buffer memory
		*bufferRow = boundaryValue;
		*(bufferRow + bufferWidth - 1u) = boundaryValue;

		++bufferRow;

		for (uint32_t x = 0u; x < width; ++x)
		{
			ocean_assert_and_suppress_unused(sourceRow >= source && sourceRow < sourceEnd, sourceEnd);
			ocean_assert_and_suppress_unused(bufferRow - bufferWidth - 1u >= buffer && bufferRow < bufferEnd, bufferEnd);

			if (*sourceRow == referenceValue)
			{
				*bufferRow =  0u;
				foundReferenceValue = true;
			}
			else
			{
				const TDistanceType neighbor0 = TDistanceType(*(bufferRow - bufferWidth - 1u)) + distanceDiagonal;
				const TDistanceType neighbor1 = TDistanceType(*(bufferRow - bufferWidth)) + distanceVerticalHorizontal;
				const TDistanceType neighbor2 = TDistanceType(*(bufferRow - bufferWidth + 1u)) + distanceDiagonal;
				const TDistanceType neighbor3 = TDistanceType(*(bufferRow - 1u)) + distanceVerticalHorizontal;

				*bufferRow = std::min(std::min(neighbor0, neighbor1), std::min(neighbor2, neighbor3));
			}

			++sourceRow;
			++bufferRow;
		}
	}

	if (foundReferenceValue == false)
	{
		return validationTargetStatus == false;
	}

	// Backward pass
	for (uint32_t y = height - 1u; y < height; --y)
	{
		const TDistanceType* validationTargetRow = validationTarget + y * validationTargetStrideElements + width - 1u;
		TDistanceType* bufferRow = buffer + (y + 1u) * bufferWidth + width;

		for (uint32_t x = width - 1u; x < width; --x)
		{
			ocean_assert_and_suppress_unused(validationTargetRow >= validationTarget && validationTargetRow < validationTargetEnd, validationTargetEnd);
			ocean_assert(bufferRow >= buffer && bufferRow + bufferWidth + 1u < bufferEnd);

			const TDistanceType neighbor4 = TDistanceType(*(bufferRow + 1u)) + distanceVerticalHorizontal;
			const TDistanceType neighbor5 = TDistanceType(*(bufferRow + bufferWidth - 1u)) + distanceDiagonal;
			const TDistanceType neighbor6 = TDistanceType(*(bufferRow + bufferWidth)) + distanceVerticalHorizontal;
			const TDistanceType neighbor7 = TDistanceType(*(bufferRow + bufferWidth + 1u)) + distanceDiagonal;

			*bufferRow = std::min(*bufferRow, std::min(std::min(neighbor4, neighbor5), std::min(neighbor6, neighbor7)));

			if (NumericT<TDistanceType>::isWeakEqual(*validationTargetRow, *bufferRow) == false)
			{
				allSucceeded = false;
			}

			--validationTargetRow;
			--bufferRow;
		}
	}

	if (validationTargetStatus == false)
	{
		allSucceeded = false;
	}

	return allSucceeded;
#endif
}

} // namespace TestSegmentation

} // namespace TestCV

} // namespace Test

} // namespace Ocean
