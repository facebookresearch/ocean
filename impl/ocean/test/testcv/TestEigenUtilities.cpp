/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestEigenUtilities.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/EigenUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestEigenUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test EigenUtilities:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFrame2Matrix(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToEigenVector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToOceanVector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToEigenQuaternion(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToOceanQuaternion(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "EigenUtilities test succeeded.";
	}
	else
	{
		Log::info() << "EigenUtilities test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestEigenUtilities, Frame2Matrix)
{
	EXPECT_TRUE(TestEigenUtilities::testFrame2Matrix(GTEST_TEST_DURATION));
}

TEST(TestEigenUtilities, ToEigenVector)
{
	EXPECT_TRUE(TestEigenUtilities::testToEigenVector(GTEST_TEST_DURATION));
}

TEST(TestEigenUtilities, ToOceanVector)
{
	EXPECT_TRUE(TestEigenUtilities::testToOceanVector(GTEST_TEST_DURATION));
}

TEST(TestEigenUtilities, ToEigenQuaternion)
{
	EXPECT_TRUE(TestEigenUtilities::testToEigenQuaternion(GTEST_TEST_DURATION));
}

TEST(TestEigenUtilities, ToOceanQuaternion)
{
	EXPECT_TRUE(TestEigenUtilities::testToOceanQuaternion(GTEST_TEST_DURATION));
}

#endif

bool TestEigenUtilities::testFrame2Matrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing frame2matrix():";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFrame2Matrix<unsigned char, float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFrame2Matrix<unsigned char, double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFrame2Matrix<float, float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFrame2Matrix<float, double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Test succeeded.";
	}
	else
	{
		Log::info() << "Test FAILED!";
	}

	return allSucceeded;
}

bool TestEigenUtilities::testToEigenVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing toEigenVector()";

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 9.0;

	bool allSucceeded = true;

	allSucceeded = testToEigenVector<int, 2, VectorI2>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<int, 3, VectorI3>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<int, 4, VectorI4>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<float, 2, VectorF2>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<float, 3, VectorF3>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<float, 4, VectorF4>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<double, 2, VectorD2>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<double, 3, VectorD3>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenVector<double, 4, VectorD4>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Test succeeded.";
	}
	else
	{
		Log::info() << "Test FAILED!";
	}

	return allSucceeded;
}

bool TestEigenUtilities::testToOceanVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 9.0;

	Log::info() << "Testing toOceanVector()";

	bool allSucceeded = true;

	allSucceeded = testToOceanVector<int, 2, VectorI2>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<int, 3, VectorI3>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<int, 4, VectorI4>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<float, 2, VectorF2>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<float, 3, VectorF3>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<float, 4, VectorF4>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<double, 2, VectorD2>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<double, 3, VectorD3>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanVector<double, 4, VectorD4>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Test succeeded.";
	}
	else
	{
		Log::info() << "Test FAILED!";
	}

	return allSucceeded;
}

bool TestEigenUtilities::testToEigenQuaternion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 2.0;

	Log::info() << "Testing toEigenQuaternion()";

	bool allSucceeded = true;

	allSucceeded = testToEigenQuaternion<float>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToEigenQuaternion<double>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Test succeeded.";
	}
	else
	{
		Log::info() << "Test FAILED!";
	}

	return allSucceeded;
}

bool TestEigenUtilities::testToOceanQuaternion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 2.0;

	Log::info() << "Testing toOceanQuaternion()";

	bool allSucceeded = true;

	allSucceeded = testToOceanQuaternion<float>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testToOceanQuaternion<double>(splitTestDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Test succeeded.";
	}
	else
	{
		Log::info() << "Test FAILED!";
	}

	return allSucceeded;
}

template <typename TSource, typename TTarget>
bool TestEigenUtilities::testFrame2Matrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing " << TypeNamer::name<TSource>() << " -> " << TypeNamer::name<TTarget>() << ":";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<TSource, 1u>();

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 500u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 500u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Eigen::Matrix<TTarget, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMajorMatrix;
		Eigen::Matrix<TTarget, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> columnMajorMatrix;

		if (CV::EigenUtilities::frame2matrix<TTarget>(frame, rowMajorMatrix) && CV::EigenUtilities::frame2matrix<TTarget>(frame, columnMajorMatrix))
		{
			if ((unsigned int)(rowMajorMatrix.cols()) != width || (unsigned int)(rowMajorMatrix.rows()) != height)
			{
				ocean_assert(false && "This must never happen!");

				allSucceeded = false;
				continue;
			}

			if ((unsigned int)(columnMajorMatrix.cols()) != width || (unsigned int)(columnMajorMatrix.rows()) != height)
			{
				ocean_assert(false && "This must never happen!");

				allSucceeded = false;
				continue;
			}

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					const TTarget testValue = TTarget(frame.constpixel<TSource>(x, y)[0]);

					if (testValue != rowMajorMatrix(y, x))
					{
						allSucceeded = false;
					}

					if (testValue != columnMajorMatrix(y, x))
					{
						allSucceeded = false;
					}
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

template <typename TType, size_t tElements, typename TOceanVector>
bool TestEigenUtilities::testToEigenVector(const double testDuration)
{
	static_assert(std::is_same<TType, int>::value || std::is_same<TType, float>::value || std::is_same<TType, double>::value, "Value of TType is not supported");
	static_assert(tElements >= 1 && tElements <= 4, "Value of tElements is out of range");
	static_assert((tElements == 2 && std::is_same<TOceanVector, VectorT2<TType>>::value) || (tElements == 3 && std::is_same<TOceanVector, VectorT3<TType>>::value) || (tElements == 4 && std::is_same<TOceanVector, VectorT4<TType>>::value), "Mismatch of tElements and TOceanVector");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing " << TypeNamer::name<TType>() << " with " << tElements << " elements:";

	Log::info() << " ";

	// Using only half of the possible value range to avoid numerical problems
	constexpr TType lowestValue = NumericT<TType>::minValue() / TType(2);
	constexpr TType maxValue = NumericT<TType>::maxValue() / TType(2);
	static_assert(lowestValue < maxValue, "Invalid data type");

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		TOceanVector ocnVector;

		for (unsigned int i = 0u; i < (unsigned int)tElements; ++i)
		{
			if constexpr (std::is_floating_point<TType>::value)
			{
				ocnVector[i] = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
			}
			else
			{
				ocnVector[i] = RandomI::random(randomGenerator, lowestValue, maxValue);
			}
		}

		const Eigen::Matrix<TType, tElements, 1> eigenVector = CV::EigenUtilities::toEigenVector(ocnVector);

		for (unsigned int i = 0u; i < (unsigned int)tElements; ++i)
		{
			allSucceeded = (ocnVector[i] - eigenVector[i] == TType(0)) && allSucceeded;
			ocean_assert(allSucceeded); // DEBUG
		}
	}
	while(startTimestamp + testDuration > Timestamp(true));

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

template <typename TType, size_t tElements, typename TOceanVector>
bool TestEigenUtilities::testToOceanVector(const double testDuration)
{
	static_assert(std::is_same<TType, int>::value || std::is_same<TType, float>::value || std::is_same<TType, double>::value, "Value of TType is not supported");
	static_assert(tElements >= 1 && tElements <= 4, "Value of tElements is out of range");
	static_assert((tElements == 2 && std::is_same<TOceanVector, VectorT2<TType>>::value) || (tElements == 3 && std::is_same<TOceanVector, VectorT3<TType>>::value) || (tElements == 4 && std::is_same<TOceanVector, VectorT4<TType>>::value), "Mismatch of tElements and TOceanVector");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing " << TypeNamer::name<TType>() << " with " << tElements << " elements:";

	Log::info() << " ";

	// Using only half of the possible value range to avoid numerical problems
	constexpr TType lowestValue = NumericT<TType>::minValue() / TType(2);
	constexpr TType maxValue = NumericT<TType>::maxValue() / TType(2);

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		Eigen::Matrix<TType, tElements, 1> eigenVector;

		for (size_t i = 0; i < tElements; ++i)
		{
			if constexpr (std::is_floating_point<TType>::value)
			{
				eigenVector[i] = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
			}
			else
			{
				eigenVector[i] = RandomI::random(randomGenerator, lowestValue, maxValue);
			}
		}

		const TOceanVector ocnVector = CV::EigenUtilities::toOceanVector(eigenVector);

		for (size_t i = 0; i < tElements; ++i)
		{
			allSucceeded = (ocnVector[(unsigned int)i] - eigenVector[i] == TType(0)) && allSucceeded;
			ocean_assert(allSucceeded); // DEBUG
		}
	}
	while(startTime + testDuration > Timestamp(true));

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

template <typename TType>
bool TestEigenUtilities::testToEigenQuaternion(const double testDuration)
{
	static_assert(std::is_same<TType, float>::value || std::is_same<TType, double>::value, "Value of TType is not supported");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing " << TypeNamer::name<TType>() << ":";

	Log::info() << " ";

	// Using only half of the possible value range to avoid numerical problems
	constexpr TType lowestValue = NumericT<TType>::minValue() / TType(2);
	constexpr TType maxValue = NumericT<TType>::maxValue() / TType(2);

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		// Not using RandomT<TType>::quaternion(randomGenerator) because this function generates only normalized quaternions
		const TType w = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType x = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType y = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType z = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const QuaternionT<TType> ocnQuaternion(w, x, y, z);

		const Eigen::Quaternion<TType> eigenQuaternion = CV::EigenUtilities::toEigenQuaternion(ocnQuaternion);

		allSucceeded = ocnQuaternion.w() == eigenQuaternion.w()
			&& ocnQuaternion.x() == eigenQuaternion.x()
			&& ocnQuaternion.y() == eigenQuaternion.y()
			&& ocnQuaternion.z() == eigenQuaternion.z()
			&& allSucceeded;
	}
	while(startTime + testDuration > Timestamp(true));

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

template <typename TType>
bool TestEigenUtilities::testToOceanQuaternion(const double testDuration)
{
	static_assert(std::is_same<TType, float>::value || std::is_same<TType, double>::value, "Value of TType is not supported");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing " << TypeNamer::name<TType>() << ":";

	Log::info() << " ";

	// Using only half of the possible value range to avoid numerical problems
	constexpr TType lowestValue = NumericT<TType>::minValue() / TType(2);
	constexpr TType maxValue = NumericT<TType>::maxValue() / TType(2);

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTime(true);

	do
	{
		const TType w = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType x = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType y = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType z = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const Eigen::Quaternion<TType> eigenQuaternion(w, x, y, z);

		const QuaternionT<TType> ocnQuaternion = CV::EigenUtilities::toOceanQuaternion(eigenQuaternion);

		allSucceeded = ocnQuaternion.w() == eigenQuaternion.w()
			&& ocnQuaternion.x() == eigenQuaternion.x()
			&& ocnQuaternion.y() == eigenQuaternion.y()
			&& ocnQuaternion.z() == eigenQuaternion.z()
			&& allSucceeded;
	}
	while(startTime + testDuration > Timestamp(true));

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

} // namespace CV

} // namespace Test

} // namespace Ocean
