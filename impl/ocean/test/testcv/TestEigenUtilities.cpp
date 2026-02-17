/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestEigenUtilities.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

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

bool TestEigenUtilities::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Test EigenUtilities");

	if (selector.shouldRun("frame2matrix"))
	{
		testResult = testFrame2Matrix(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("toeigenvector"))
	{
		testResult = testToEigenVector(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("tooceanvector"))
	{
		testResult = testToOceanVector(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("toeigenquaternion"))
	{
		testResult = testToEigenQuaternion(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("tooceanquaternion"))
	{
		testResult = testToOceanQuaternion(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testFrame2Matrix<unsigned char, float>(testDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testFrame2Matrix<unsigned char, double>(testDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testFrame2Matrix<float, float>(testDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testFrame2Matrix<float, double>(testDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestEigenUtilities::testToEigenVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing toEigenVector()";

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 9.0;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<int, 2, VectorI2>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<int, 3, VectorI3>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<int, 4, VectorI4>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<float, 2, VectorF2>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<float, 3, VectorF3>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<float, 4, VectorF4>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<double, 2, VectorD2>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<double, 3, VectorD3>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenVector<double, 4, VectorD4>(splitTestDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestEigenUtilities::testToOceanVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 9.0;

	Log::info() << "Testing toOceanVector()";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<int, 2, VectorI2>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<int, 3, VectorI3>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<int, 4, VectorI4>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<float, 2, VectorF2>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<float, 3, VectorF3>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<float, 4, VectorF4>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<double, 2, VectorD2>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<double, 3, VectorD3>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanVector<double, 4, VectorD4>(splitTestDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestEigenUtilities::testToEigenQuaternion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 2.0;

	Log::info() << "Testing toEigenQuaternion()";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testToEigenQuaternion<float>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToEigenQuaternion<double>(splitTestDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestEigenUtilities::testToOceanQuaternion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	// These tests are very simple and are not required to run for the full test duration
	const double splitTestDuration = testDuration / 2.0;

	Log::info() << "Testing toOceanQuaternion()";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testToOceanQuaternion<float>(splitTestDuration));

	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testToOceanQuaternion<double>(splitTestDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename TSource, typename TTarget>
bool TestEigenUtilities::testFrame2Matrix(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing " << TypeNamer::name<TSource>() << " -> " << TypeNamer::name<TTarget>() << ":";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<TSource, 1u>();

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

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

				OCEAN_SET_FAILED(validation);
				continue;
			}

			if ((unsigned int)(columnMajorMatrix.cols()) != width || (unsigned int)(columnMajorMatrix.rows()) != height)
			{
				ocean_assert(false && "This must never happen!");

				OCEAN_SET_FAILED(validation);
				continue;
			}

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					const TTarget testValue = TTarget(frame.constpixel<TSource>(x, y)[0]);

					OCEAN_EXPECT_EQUAL(validation, testValue, rowMajorMatrix(y, x));

					OCEAN_EXPECT_EQUAL(validation, testValue, columnMajorMatrix(y, x));
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
	Validation validation(randomGenerator);

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
			OCEAN_EXPECT_EQUAL(validation, ocnVector[i] - eigenVector[i], TType(0));
			ocean_assert(validation.succeededSoFar()); // DEBUG
		}
	}
	while(!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
	Validation validation(randomGenerator);

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
			OCEAN_EXPECT_EQUAL(validation, ocnVector[(unsigned int)i] - eigenVector[i], TType(0));
			ocean_assert(validation.succeededSoFar()); // DEBUG
		}
	}
	while(startTime + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
	Validation validation(randomGenerator);

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

		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.w(), eigenQuaternion.w());
		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.x(), eigenQuaternion.x());
		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.y(), eigenQuaternion.y());
		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.z(), eigenQuaternion.z());
	}
	while(startTime + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
	Validation validation(randomGenerator);

	const Timestamp startTime(true);

	do
	{
		const TType w = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType x = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType y = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const TType z = RandomT<TType>::scalar(randomGenerator, lowestValue, maxValue);
		const Eigen::Quaternion<TType> eigenQuaternion(w, x, y, z);

		const QuaternionT<TType> ocnQuaternion = CV::EigenUtilities::toOceanQuaternion(eigenQuaternion);

		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.w(), eigenQuaternion.w());
		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.x(), eigenQuaternion.x());
		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.y(), eigenQuaternion.y());
		OCEAN_EXPECT_EQUAL(validation, ocnQuaternion.z(), eigenQuaternion.z());
	}
	while(startTime + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

} // namespace CV

} // namespace Test

} // namespace Ocean
