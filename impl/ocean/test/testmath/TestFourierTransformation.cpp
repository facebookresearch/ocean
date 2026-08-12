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

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFourierTransformation::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Fourier Transformation test");

	Log::info() << " ";

	if (selector.shouldRun("fouriertransform"))
	{
		testResult = testFourierTransform<float, false>(testDuration);
		Log::info() << " ";
		testResult = testFourierTransform<float, true>(testDuration);
		Log::info() << " ";
		testResult = testFourierTransform<double, false>(testDuration);
		Log::info() << " ";
		testResult = testFourierTransform<double, true>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("elementwisemultiplication2"))
	{
		testResult = testElementwiseMultiplication2<float>(testDuration);
		Log::info() << " ";
		testResult = testElementwiseMultiplication2<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("elementwisemultiplicationccs"))
	{
		testResult = testElementwiseMultiplicationCCS<float>(testDuration);
		Log::info() << " ";
		testResult = testElementwiseMultiplicationCCS<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("elementwisedivision2"))
	{
		testResult = testElementwiseDivision2<float>(testDuration);
		Log::info() << " ";
		testResult = testElementwiseDivision2<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

TEST(TestFourierTransformation, ElementwiseMultiplicationCCSFloat)
{
	EXPECT_TRUE(TestFourierTransformation::testElementwiseMultiplicationCCS<float>(GTEST_TEST_DURATION));
}

TEST(TestFourierTransformation, ElementwiseMultiplicationCCSDouble)
{
	EXPECT_TRUE(TestFourierTransformation::testElementwiseMultiplicationCCS<double>(GTEST_TEST_DURATION));
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

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int maxSpatialPaddingElements = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int spatialPaddingElements = maxSpatialPaddingElements * RandomI::random(randomGenerator, 1u);

		const unsigned int maxFrequencyPaddingElements = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int frequencyPaddingElements = maxFrequencyPaddingElements * RandomI::random(randomGenerator, 1u);

		const unsigned int maxReverseSpatialPaddingElements = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int reverseSpatialPaddingElements = maxReverseSpatialPaddingElements * RandomI::random(randomGenerator, 1u);

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
					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(((const std::complex<T>*)spatialRow)[x], ((const std::complex<T>*)reverseSpatialRow)[x], epsilon));
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
					OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(spatialRow[x], reverseSpatialRow[x], epsilon));
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
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFourierTransformation::testElementwiseMultiplication2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Elementwise-multiplication test for " << TypeNamer::name<T>() << ":";

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	using ComplexT = std::complex<T>;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 100u);

		const unsigned int maxHorizontalPaddingSource0 = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingSource0 = maxHorizontalPaddingSource0 * RandomI::random(randomGenerator, 1u);

		const unsigned int maxHorizontalPaddingSource1 = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingSource1 = maxHorizontalPaddingSource1 * RandomI::random(randomGenerator, 1u);

		const unsigned int maxHorizontalPaddingTarget = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingTarget = maxHorizontalPaddingTarget * RandomI::random(randomGenerator, 1u);

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
				OCEAN_SET_FAILED(validation);
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

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(groundTruthResult.real(), calculatedResult.real(), epsilon)
						&& NumericT<T>::isEqual(groundTruthResult.imag(), calculatedResult.imag(), epsilon));
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
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFourierTransformation::testElementwiseMultiplicationCCS(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Elementwise-multiplication test for CCS-packed spectrums for " << TypeNamer::name<T>() << ":";

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	constexpr Index32 invalidIndex = Index32(-1);

	// additional elements behind the spectrum, any modification of these elements is a buffer overrun
	constexpr unsigned int guardElements = 8u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		unsigned int width = 1u;
		unsigned int height = 1u;

		switch (RandomI::random(randomGenerator, 2u))
		{
			case 0u:
				// one-dimensional spectrum stored in a single row
				width = RandomI::random(randomGenerator, 1u, 64u);
				break;

			case 1u:
				// one-dimensional spectrum stored in a single column
				height = RandomI::random(randomGenerator, 1u, 64u);
				break;

			default:
				width = RandomI::random(randomGenerator, 2u, 64u);
				height = RandomI::random(randomGenerator, 2u, 64u);
				break;
		}

		const unsigned int paddingSourceA = RandomI::random(randomGenerator, 0u, 16u);
		const unsigned int paddingSourceB = RandomI::random(randomGenerator, 0u, 16u);
		const unsigned int paddingTarget = RandomI::random(randomGenerator, 0u, 16u);

		std::vector<T> sourceA((width + paddingSourceA) * height + guardElements);
		std::vector<T> sourceB((width + paddingSourceB) * height + guardElements);
		std::vector<T> target((width + paddingTarget) * height + guardElements);

		for (T& value : sourceA)
		{
			value = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		for (T& value : sourceB)
		{
			value = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		for (T& value : target)
		{
			value = RandomT<T>::scalar(randomGenerator, T(-1), T(1));
		}

		const std::vector<T> targetCopy(target);

		bool complexConjugateA = false;
		bool complexConjugateB = false;

		switch (RandomI::random(randomGenerator, 3u))
		{
			case 0u:
				FourierTransformation::elementwiseMultiplicationCCS<T, false, false>(sourceA.data(), sourceB.data(), target.data(), width, height, paddingSourceA, paddingSourceB, paddingTarget);
				break;

			case 1u:
				complexConjugateA = true;
				FourierTransformation::elementwiseMultiplicationCCS<T, true, false>(sourceA.data(), sourceB.data(), target.data(), width, height, paddingSourceA, paddingSourceB, paddingTarget);
				break;

			case 2u:
				complexConjugateB = true;
				FourierTransformation::elementwiseMultiplicationCCS<T, false, true>(sourceA.data(), sourceB.data(), target.data(), width, height, paddingSourceA, paddingSourceB, paddingTarget);
				break;

			case 3u:
				complexConjugateA = true;
				complexConjugateB = true;
				FourierTransformation::elementwiseMultiplicationCCS<T, true, true>(sourceA.data(), sourceB.data(), target.data(), width, height, paddingSourceA, paddingSourceB, paddingTarget);
				break;

			default:
				ocean_assert(false && "Invalid combination!");
				OCEAN_SET_FAILED(validation);
				break;
		}

		const IndexPairs32 valuesSourceA = ccsSpectrumValues(width, height, paddingSourceA);
		const IndexPairs32 valuesSourceB = ccsSpectrumValues(width, height, paddingSourceB);
		const IndexPairs32 valuesTarget = ccsSpectrumValues(width, height, paddingTarget);

		if (valuesSourceA.size() != valuesTarget.size() || valuesSourceB.size() != valuesTarget.size())
		{
			ocean_assert(false && "This should never happen!");
			OCEAN_SET_FAILED(validation);

			break;
		}

		std::vector<bool> isSpectrumElement(target.size(), false);

		for (size_t n = 0; n < valuesTarget.size(); ++n)
		{
			const IndexPair32& indicesSourceA = valuesSourceA[n];
			const IndexPair32& indicesSourceB = valuesSourceB[n];
			const IndexPair32& indicesTarget = valuesTarget[n];

			const T imaginarySourceA = indicesSourceA.second == invalidIndex ? T(0) : sourceA[indicesSourceA.second];
			const T imaginarySourceB = indicesSourceB.second == invalidIndex ? T(0) : sourceB[indicesSourceB.second];

			const std::complex<T> valueSourceA(sourceA[indicesSourceA.first], complexConjugateA ? -imaginarySourceA : imaginarySourceA);
			const std::complex<T> valueSourceB(sourceB[indicesSourceB.first], complexConjugateB ? -imaginarySourceB : imaginarySourceB);

			const std::complex<T> groundTruthResult = valueSourceA * valueSourceB;

			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(groundTruthResult.real(), target[indicesTarget.first], epsilon));

			isSpectrumElement[indicesTarget.first] = true;

			if (indicesTarget.second != invalidIndex)
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(groundTruthResult.imag(), target[indicesTarget.second], epsilon));

				isSpectrumElement[indicesTarget.second] = true;
			}
		}

		// the elements which do not belong to the spectrum - the padding and the guard elements behind the spectrum - must be untouched

		for (size_t n = 0; n < target.size(); ++n)
		{
			if (!isSpectrumElement[n] && target[n] != targetCopy[n])
			{
				ocean_assert(false && "The function has written outside of the spectrum!");
				OCEAN_SET_FAILED(validation);

				break;
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	{
		// a fixed vector for a spectrum stored in a single column, verified by hand so that this case does not depend on ccsSpectrumValues()
		// four elements with two elements of padding per row, so the spectrum sits at the offsets 0, 3, 6 and 9
		// CCS packs [dc, real, imaginary, nyquist], so the middle pair is one complex number: (2+3i) * (10+10i) = -10+50i

		// the two buffers use distinct fillers, so that a value read from the padding cannot be mistaken for a spectrum value
		constexpr T paddingSourceA = T(-2);
		constexpr T paddingSourceB = T(-3);
		constexpr T untouched = T(-1);

		const std::vector<T> sourceA = {T(1), paddingSourceA, paddingSourceA, T(2), paddingSourceA, paddingSourceA, T(3), paddingSourceA, paddingSourceA, T(4), paddingSourceA, paddingSourceA};
		const std::vector<T> sourceB = {T(10), paddingSourceB, paddingSourceB, T(10), paddingSourceB, paddingSourceB, T(10), paddingSourceB, paddingSourceB, T(10), paddingSourceB, paddingSourceB};

		const std::vector<T> expected = {T(10), untouched, untouched, T(-10), untouched, untouched, T(50), untouched, untouched, T(40), untouched, untouched};

		std::vector<T> target(expected.size(), untouched);

		FourierTransformation::elementwiseMultiplicationCCS<T, false, false>(sourceA.data(), sourceB.data(), target.data(), 1u /*width*/, 4u /*height*/, 2u, 2u, 2u);

		for (size_t n = 0; n < expected.size(); ++n)
		{
			if (target[n] != expected[n])
			{
				Log::error() << "Offset " << n << ": expected " << double(expected[n]) << ", got " << double(target[n]);
			}

			OCEAN_EXPECT_EQUAL(validation, target[n], expected[n]);
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFourierTransformation::testElementwiseDivision2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Elementwise-division test for " << TypeNamer::name<T>() << ":";

	constexpr T epsilon = std::is_same<T, double>::value ? T(0.00001) : T(0.001);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	using ComplexT = std::complex<T>;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 100u);

		const unsigned int maxHorizontalPaddingSource0 = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingSource0 = maxHorizontalPaddingSource0 * RandomI::random(randomGenerator, 1u);

		const unsigned int maxHorizontalPaddingSource1 = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingSource1 = maxHorizontalPaddingSource1 * RandomI::random(randomGenerator, 1u);

		const unsigned int maxHorizontalPaddingTarget = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int horizontalPaddingTarget = maxHorizontalPaddingTarget * RandomI::random(randomGenerator, 1u);

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

				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(groundTruthResult.real(), calculatedResult.real(), epsilon)
						&& NumericT<T>::isEqual(groundTruthResult.imag(), calculatedResult.imag(), epsilon));
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
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

IndexPairs32 TestFourierTransformation::ccsSpectrumValues(const unsigned int width, const unsigned int height, const unsigned int paddingElements)
{
	ocean_assert(width != 0u && height != 0u);

	constexpr Index32 invalidIndex = Index32(-1);

	const unsigned int strideElements = width + paddingElements;

	IndexPairs32 values;

	if (width == 1u || height == 1u)
	{
		// the first element and, for an even length, the last element are real-only, all remaining elements form real/imaginary pairs

		const unsigned int elements = std::max(width, height);

		// the layout of a one-dimensional spectrum is defined by elementwiseMultiplicationCCS(), see FourierTransformation.h
		const unsigned int step = width == 1u ? strideElements : 1u;

		values.emplace_back(0u, invalidIndex);

		for (Index32 element = 1u; element + 1u < elements; element += 2u)
		{
			values.emplace_back(element * step, (element + 1u) * step);
		}

		if (elements % 2u == 0u)
		{
			values.emplace_back((elements - 1u) * step, invalidIndex);
		}

		return values;
	}

	// the left-most column and, for an even width, the right-most column are packed along the vertical direction

	const bool isWidthEven = width % 2u == 0u;

	Indices32 verticalColumns(1, 0u);

	if (isWidthEven)
	{
		verticalColumns.emplace_back(width - 1u);
	}

	for (const Index32 column : verticalColumns)
	{
		values.emplace_back(column, invalidIndex);

		for (Index32 row = 1u; row + 1u < height; row += 2u)
		{
			values.emplace_back(row * strideElements + column, (row + 1u) * strideElements + column);
		}

		if (height % 2u == 0u)
		{
			values.emplace_back((height - 1u) * strideElements + column, invalidIndex);
		}
	}

	// all remaining columns are packed along the horizontal direction, in every row

	const unsigned int lastHorizontalColumn = isWidthEven ? width - 2u : width - 1u;

	for (Index32 row = 0u; row < height; ++row)
	{
		for (Index32 column = 1u; column < lastHorizontalColumn; column += 2u)
		{
			values.emplace_back(row * strideElements + column, row * strideElements + column + 1u);
		}
	}

	return values;
}

}

}

}
