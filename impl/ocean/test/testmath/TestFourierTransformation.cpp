/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestFourierTransformation.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/FourierTransformation.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFourierTransformation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Fourier Transformation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFourierTransform<float, false>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testFourierTransform<float, true>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testFourierTransform<double, false>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testFourierTransform<double, true>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testElementwiseMultiplication2<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testElementwiseMultiplication2<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testElementwiseDivision2<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testElementwiseDivision2<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Fourier Transformation test succeeded.";
	}
	else
	{
		Log::info() << "Fourier Transformation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFourierTransformation, FourierTransformFloatReal)
{
	EXPECT_TRUE((TestFourierTransformation::testFourierTransform<float, false>(GTEST_TEST_DURATION)));
}

TEST(TestFourierTransformation, FourierTransformFloatComplex)
{
	EXPECT_TRUE((TestFourierTransformation::testFourierTransform<float, true>(GTEST_TEST_DURATION)));
}

TEST(TestFourierTransformation, FourierTransformDoubleReal)
{
	EXPECT_TRUE((TestFourierTransformation::testFourierTransform<double, false>(GTEST_TEST_DURATION)));
}

TEST(TestFourierTransformation, FourierTransformDoubleComplex)
{
	EXPECT_TRUE((TestFourierTransformation::testFourierTransform<double, true>(GTEST_TEST_DURATION)));
}

TEST(TestFourierTransformation, ElementwiseMultiplication2Float)
{
	EXPECT_TRUE(TestFourierTransformation::testElementwiseMultiplication2<float>(GTEST_TEST_DURATION));
}

TEST(TestFourierTransformation, ElementwiseMultiplication2Double)
{
	EXPECT_TRUE(TestFourierTransformation::testElementwiseMultiplication2<double>(GTEST_TEST_DURATION));
}

TEST(TestFourierTransformation, ElementwiseDivision2Float)
{
	EXPECT_TRUE(TestFourierTransformation::testElementwiseDivision2<float>(GTEST_TEST_DURATION));
}

TEST(TestFourierTransformation, ElementwiseDivision2Double)
{
	EXPECT_TRUE(TestFourierTransformation::testElementwiseDivision2<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T, bool tSourceIsComplex>
bool TestFourierTransformation::testFourierTransform(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Forward and backward Fourier transform test for " << TypeNamer::name<T>() << ", with " << (tSourceIsComplex ? "complex" : "real") << " source signal:";

	bool allSucceeded = true;

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int spatialPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int frequencyPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int reverseSpatialPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		if constexpr (tSourceIsComplex)
		{
			const unsigned int spatialStrideElements = width * 2u + spatialPaddingElements;
			const unsigned int frequencyStrideElements = width * 2u + frequencyPaddingElements;
			const unsigned int reverseSpatialStrideElements = width * 2u + reverseSpatialPaddingElements;

			std::vector<T> spatial(spatialStrideElements * height);
			std::vector<T> frequency(frequencyStrideElements * height);
			std::vector<T> reverseSpatial(reverseSpatialStrideElements * height);

			for (T& value : spatial)
			{
				value = RandomT<T>::scalar(randomGenerator, -1, 1);
			}

			for (T& value : frequency)
			{
				value = RandomT<T>::scalar(randomGenerator, -1, 1);
			}

			for (T& value : reverseSpatial)
			{
				value = RandomT<T>::scalar(randomGenerator, -1, 1);
			}

			const std::vector<T> copyFrequency(frequency);
			const std::vector<T> copyReverseSpatial(reverseSpatial);

			FourierTransformation::complexSpatialToFrequency2(spatial.data(), width, height, frequency.data(), spatialPaddingElements, frequencyPaddingElements);

			if (frequencyPaddingElements != 0u)
			{
				// we check whether the padding memory is untouched

				for (unsigned int y = 0u; y < height; ++y)
				{
					if (memcmp(frequency.data() + y * frequencyStrideElements + width * 2u, copyFrequency.data() + y * frequencyStrideElements + width * 2u, frequencyPaddingElements * sizeof(T)) != 0)
					{
						ocean_assert(false && "Invalid padding data!");
						return false;
					}
				}
			}

			FourierTransformation::frequencyToComplexSpatial2(frequency.data(), width, height, reverseSpatial.data(), frequencyPaddingElements, reverseSpatialPaddingElements);

			for (unsigned int y = 0u; y < height; ++y)
			{
				const T* spatialRow = spatial.data() + y * spatialStrideElements;
				const T* reverseSpatialRow = reverseSpatial.data() + y * reverseSpatialStrideElements;
				const T* copyReverseSpatialRow = copyReverseSpatial.data() + y * reverseSpatialStrideElements;

				for (unsigned int x = 0u; x < width; ++x)
				{
					if (NumericT<T>::isNotEqual(((const std::complex<T>*)spatialRow)[x], ((const std::complex<T>*)reverseSpatialRow)[x], epsilon))
					{
						allSucceeded = false;
					}
				}

				if (reverseSpatialPaddingElements != 0u)
				{
					// we check whether the padding memory is untouched

					if (memcmp(reverseSpatialRow + width * 2u, copyReverseSpatialRow + width * 2u, reverseSpatialPaddingElements * sizeof(T)) != 0)
					{
						ocean_assert(false && "Invalid padding data!");
						return false;
					}
				}
			}
		}
		else
		{
			const unsigned int spatialStrideElements = width * 1u + spatialPaddingElements;
			const unsigned int frequencyStrideElements = width * 2u + frequencyPaddingElements;
			const unsigned int reverseSpatialStrideElements = width * 1u + reverseSpatialPaddingElements;

			std::vector<T> spatial(spatialStrideElements * height);
			std::vector<T> frequency(frequencyStrideElements * height);
			std::vector<T> reverseSpatial(reverseSpatialStrideElements * height);

			for (T& value : spatial)
			{
				value = RandomT<T>::scalar(randomGenerator, -1, 1);
			}

			for (T& value : frequency)
			{
				value = RandomT<T>::scalar(randomGenerator, -1, 1);
			}

			for (T& value : reverseSpatial)
			{
				value = RandomT<T>::scalar(randomGenerator, -1, 1);
			}

			const std::vector<T> copyFrequency(frequency);
			const std::vector<T> copyReverseSpatial(reverseSpatial);

			FourierTransformation::spatialToFrequency2(spatial.data(), width, height, frequency.data(), spatialPaddingElements, frequencyPaddingElements);

			if (frequencyPaddingElements != 0u)
			{
				// we check whether the padding memory is untouched

				for (unsigned int y = 0u; y < height; ++y)
				{
					if (memcmp(frequency.data() + y * frequencyStrideElements + width * 2u, copyFrequency.data() + y * frequencyStrideElements + width * 2u, frequencyPaddingElements * sizeof(T)) != 0)
					{
						ocean_assert(false && "Invalid padding data!");
						return false;
					}
				}
			}

			FourierTransformation::frequencyToSpatial2(frequency.data(), width, height, reverseSpatial.data(), frequencyPaddingElements, reverseSpatialPaddingElements);

			for (unsigned int y = 0u; y < height; ++y)
			{
				const T* spatialRow = spatial.data() + y * spatialStrideElements;
				const T* reverseSpatialRow = reverseSpatial.data() + y * reverseSpatialStrideElements;
				const T* copyReverseSpatialRow = copyReverseSpatial.data() + y * reverseSpatialStrideElements;

				for (unsigned int x = 0u; x < width; ++x)
				{
					if (NumericT<T>::isNotEqual(spatialRow[x], reverseSpatialRow[x], epsilon))
					{
						allSucceeded = false;
					}
				}

				if (reverseSpatialPaddingElements != 0u)
				{
					// we check whether the padding memory is untouched

					if (memcmp(reverseSpatialRow + width, copyReverseSpatialRow + width, reverseSpatialPaddingElements * sizeof(T)) != 0)
					{
						ocean_assert(false && "Invalid padding data!");
						return false;
					}
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

template <typename T>
bool TestFourierTransformation::testElementwiseMultiplication2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Elementwise-multiplication test for " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	RandomGenerator randomGenerator;

	typedef std::complex<T> ComplexT;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 100u);

		const unsigned int horizontalPaddingSource0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int horizontalPaddingSource1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int horizontalPaddingTarget = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const unsigned int strideSource0 = width * 2u + horizontalPaddingSource0;
		const unsigned int strideSource1 = width * 2u + horizontalPaddingSource1;
		const unsigned int strideTarget = width * 2u + horizontalPaddingTarget;

		std::vector<T> source0(strideSource0 * height);
		std::vector<T> source1(strideSource1 * height);
		std::vector<T> target(strideTarget * height);

		for (size_t n = 0; n < source0.size(); ++n)
		{
			source0[n] = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		for (size_t n = 0; n < source1.size(); ++n)
		{
			source1[n] = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		for (size_t n = 0; n < target.size(); ++n)
		{
			target[n] = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		const std::vector<T> targetCopy(target);

		const unsigned int complexConjugate = RandomI::random(randomGenerator, 3u);

		bool complexConjugate0 = false;
		bool complexConjugate1 = false;

		switch (complexConjugate)
		{
			case 0u:
				complexConjugate0 = false;
				complexConjugate1 = false;
				FourierTransformation::elementwiseMultiplication2<T, false, false>(source0.data(), source1.data(), target.data(), width, height, horizontalPaddingSource0, horizontalPaddingSource1, horizontalPaddingTarget);
				break;

			case 1u:
				complexConjugate0 = true;
				complexConjugate1 = false;
				FourierTransformation::elementwiseMultiplication2<T, true, false>(source0.data(), source1.data(), target.data(), width, height, horizontalPaddingSource0, horizontalPaddingSource1, horizontalPaddingTarget);
				break;

			case 2u:
				complexConjugate0 = false;
				complexConjugate1 = true;
				FourierTransformation::elementwiseMultiplication2<T, false, true>(source0.data(), source1.data(), target.data(), width, height, horizontalPaddingSource0, horizontalPaddingSource1, horizontalPaddingTarget);
				break;

			case 3u:
				complexConjugate0 = true;
				complexConjugate1 = true;
				FourierTransformation::elementwiseMultiplication2<T, true, true>(source0.data(), source1.data(), target.data(), width, height, horizontalPaddingSource0, horizontalPaddingSource1, horizontalPaddingTarget);
				break;

			default:
				ocean_assert(false && "Invalid combination!");
				allSucceeded = false;
				break;
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			const ComplexT* complexSourceRow0 = (const ComplexT*)(source0.data() + y * strideSource0);
			const ComplexT* complexSourceRow1 = (const ComplexT*)(source1.data() + y * strideSource1);
			const ComplexT* complexTargetRow = (ComplexT*)(target.data() + y * strideTarget);

			for (unsigned int x = 0u; x < width; ++x)
			{
				ComplexT elementSource0 = complexSourceRow0[x];
				ComplexT elementSource1 = complexSourceRow1[x];

				if (complexConjugate0)
				{
					elementSource0 = std::conj(elementSource0);
				}

				if (complexConjugate1)
				{
					elementSource1 = std::conj(elementSource1);
				}

				const ComplexT groundTruthResult = elementSource0 * elementSource1;
				const ComplexT& calculatedResult = complexTargetRow[x];

				if (NumericT<T>::isNotEqual(groundTruthResult.real(), calculatedResult.real(), epsilon)
						&& NumericT<T>::isNotEqual(groundTruthResult.imag(), calculatedResult.imag(), epsilon))
				{
					allSucceeded = false;
				}
			}

			// the padding content must be untouched

			const T* targetRowPadding = target.data() + y * strideTarget + width * 2u;
			const T* targetRowCopyPadding = targetCopy.data() + y * strideTarget + width * 2u;

			for (unsigned int n = 0u; n < horizontalPaddingTarget; ++n)
			{
				if (targetRowCopyPadding[n] != targetRowPadding[n])
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
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

template <typename T>
bool TestFourierTransformation::testElementwiseDivision2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Elementwise-division test for " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	RandomGenerator randomGenerator;

	typedef std::complex<T> ComplexT;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingSource0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int horizontalPaddingSource1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int horizontalPaddingTarget = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const unsigned int strideSource0 = width * 2u + horizontalPaddingSource0;
		const unsigned int strideSource1 = width * 2u + horizontalPaddingSource1;
		const unsigned int strideTarget = width * 2u + horizontalPaddingTarget;

		std::vector<T> source0(strideSource0 * height);
		std::vector<T> source1(strideSource1 * height);
		std::vector<T> target(strideTarget * height);

		for (size_t n = 0; n < source0.size(); ++n)
		{
			source0[n] = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			T* const source1RowDataBegin = &source1[y * strideSource1];
			T* const source1RowDataEnd = source1RowDataBegin + 2 * width;

			for (T* complexNumberBegin = source1RowDataBegin;  complexNumberBegin < source1RowDataEnd; complexNumberBegin += 2)
			{
				T& realValue = *complexNumberBegin;
				T& imaginaryValue = *(complexNumberBegin + 1);

				do
				{
					realValue = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
					imaginaryValue = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
				}
				while (NumericT<T>::isEqualEps(realValue * realValue + imaginaryValue * imaginaryValue));
			}

			std::uint32_t* const paddingBegin = reinterpret_cast<std::uint32_t*>(source1RowDataEnd);
			std::uint32_t* const paddingEnd = reinterpret_cast<std::uint32_t*>(source1RowDataBegin + strideSource1);

			for (std::uint32_t* padding = paddingBegin; padding < paddingEnd; ++padding)
			{
				*padding = RandomI::random32(randomGenerator);
			}
		}

		for (size_t n = 0; n < target.size(); ++n)
		{
			target[n] = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		const std::vector<T> targetCopy(target);

		FourierTransformation::elementwiseDivision2<T>(source0.data(), source1.data(), target.data(), width, height, horizontalPaddingSource0, horizontalPaddingSource1, horizontalPaddingTarget);

		for (unsigned int y = 0u; y < height; ++y)
		{
			const ComplexT* complexSourceRow0 = (const ComplexT*)(source0.data() + y * strideSource0);
			const ComplexT* complexSourceRow1 = (const ComplexT*)(source1.data() + y * strideSource1);
			const ComplexT* complexTargetRow = (ComplexT*)(target.data() + y * strideTarget);

			for (unsigned int x = 0u; x < width; ++x)
			{
				ocean_assert(NumericT<T>::isNotEqualEps(complexSourceRow1[x]));

				const ComplexT groundTruthResult = complexSourceRow0[x] / complexSourceRow1[x];
				const ComplexT& calculatedResult = complexTargetRow[x];

				if (NumericT<T>::isNotEqual(groundTruthResult.real(), calculatedResult.real(), epsilon)
						&& NumericT<T>::isNotEqual(groundTruthResult.imag(), calculatedResult.imag(), epsilon))
				{
					allSucceeded = false;
				}
			}

			// the padding content must be untouched

			const T* targetRowPadding = target.data() + y * strideTarget + width * 2u;
			const T* targetRowCopyPadding = targetCopy.data() + y * strideTarget + width * 2u;

			for (unsigned int n = 0u; n < horizontalPaddingTarget; ++n)
			{
				if (targetRowCopyPadding[n] != targetRowPadding[n])
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
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

}

}

}
