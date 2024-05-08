/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFourierTransform.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/math/FourierTransformation.h"

#include <algorithm>
#include <numeric>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool TestFourierTransform::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Fourier Transform test:   ---";
	Log::info() << " ";

	allSucceeded = testFFT<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFFT<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFFTPadding<float>(testDuration) && allSucceeded;

	 Log::info() << " ";
	 Log::info() << "-";
	 Log::info() << " ";

	 allSucceeded = testFFTPadding<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testElementwiseMultiplicationCCS(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Fourier Transform test passed successfully";
	}
	else
	{
		Log::info() << "Fourier Transform test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestOpenCV, TestFourierTransformFFTFloat)
{
	EXPECT_TRUE(TestFourierTransform::testFFT<float>(GTEST_TEST_DURATION));
}

TEST(TestOpenCV, TestFourierTransformFFTDouble)
{
	EXPECT_TRUE(TestFourierTransform::testFFT<double>(GTEST_TEST_DURATION));
}

TEST(TestOpenCV, TestFourierTransformFFTPaddingFloat)
{
	EXPECT_TRUE(TestFourierTransform::testFFTPadding<float>(GTEST_TEST_DURATION));
}

TEST(TestOpenCV, TestFourierTransformFFTPaddingDouble)
{
	EXPECT_TRUE(TestFourierTransform::testFFTPadding<double>(GTEST_TEST_DURATION));
}

TEST(TestOpenCV, TestFourierTransformSpectraMultiplicationCcsPacked)
{
	EXPECT_TRUE(TestFourierTransform::testElementwiseMultiplicationCCS(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename TDataType>
bool TestFourierTransform::testFFT(const double testDuration)
{
	static_assert(std::is_same<TDataType, float>() || std::is_same<TDataType, double>(), "This is only defined for float and double");

	constexpr FrameType::DataType dataType = FrameType::dataType<TDataType>();

	ocean_assert(testDuration > 0.0);

	Log::info() << "FFT test for type \'" << TypeNamer::name<TDataType>() << "\':";

	constexpr uint32_t width = 1920u;
	constexpr uint32_t height = 1080u;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	uint32_t totalIteration = 0u;
	TDataType maxAbsErrors[2] = { TDataType(0), TDataType(0) };
	bool validationSuccessful = true;
	constexpr TDataType maxAllowedAbsDifference = std::is_same<TDataType, float>::value ? TDataType(100) : TDataType(0.1);

	const Timestamp startTimestamp(true);

	do
	{
		const bool measurePerformance = RandomI::random(randomGenerator, 1u) == 0u;

		const uint32_t sourceWidth = measurePerformance ? width : RandomI::random(randomGenerator, 2u, width);
		const uint32_t sourceHeight = measurePerformance ? height : RandomI::random(randomGenerator, 2u, height);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat(dataType, 1u), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame frequencyFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, FrameType::genericPixelFormat(dataType, 2u)), &randomGenerator);

		const Frame copyFrequencyFrame(frequencyFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		performanceOcean.startIf(measurePerformance);
			FourierTransformation::spatialToFrequency2(sourceFrame.constdata<TDataType>(), sourceFrame.width(), sourceFrame.height(), frequencyFrame.data<TDataType>(), sourceFrame.paddingElements(), frequencyFrame.paddingElements());
		performanceOcean.stopIf(measurePerformance);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(frequencyFrame, copyFrequencyFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		// OpenCV
		Frame ocvFrequencyFrame(frequencyFrame.frameType());

		performanceOpenCV.startIf(measurePerformance);
			cv::dft(CV::OpenCVUtilities::toCvMat(sourceFrame, /* copy */ false), CV::OpenCVUtilities::toCvMat(ocvFrequencyFrame, /* copy */ false), cv::DFT_COMPLEX_OUTPUT);
		performanceOpenCV.stopIf(measurePerformance);

		// Validation
		ocean_assert(frequencyFrame.frameType() == ocvFrequencyFrame.frameType());
		ocean_assert(frequencyFrame.channels() == 2u);

		TDataType currentMaxAbsErrors[2] = { TDataType(0), TDataType(0) };

		for (uint32_t y = 0u; y < frequencyFrame.height(); ++y)
		{
			const TDataType* frequencyFrameRow = frequencyFrame.constrow<TDataType>(y);
			const TDataType* ocvFrequencyFrameRow = ocvFrequencyFrame.constrow<TDataType>(y);

			for (uint32_t x = 0u; x < frequencyFrame.width(); ++x)
			{
				for (uint32_t c = 0u; c < 2u; ++c)
				{
					const TDataType oceanValue = frequencyFrameRow[2u * x + c];
					const TDataType opencvValue = ocvFrequencyFrameRow[2u * x + c];

					const TDataType absAbsoluteError = NumericT<TDataType>::abs(oceanValue - opencvValue);

					currentMaxAbsErrors[c] = std::max(currentMaxAbsErrors[c], absAbsoluteError);
				}
			}
		}

		for (size_t i = 0; i < 2; ++i)
		{
			maxAbsErrors[i] = std::max(maxAbsErrors[i], currentMaxAbsErrors[i]);

			if (currentMaxAbsErrors[i] >= maxAllowedAbsDifference)
			{
				validationSuccessful = false;
			}
		}

		totalIteration++;
	}
	while (performanceOcean.measurements() == 0u || performanceOpenCV.measurements() == 0u || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance factor: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 1u) << "] x";

	Log::info() << "Iterations: " << totalIteration;
	Log::info() << "Max. absolute error, real: " << String::toAString(maxAbsErrors[0], 3u) << ", imaginary: " << String::toAString(maxAbsErrors[1], 3u);
	Log::info() << "Validation: " << (validationSuccessful ? "passed" : "FAILED");

	return validationSuccessful;
}

template bool TestFourierTransform::testFFT<float>(const double);
template bool TestFourierTransform::testFFT<double>(const double);

template <typename TDataType>
bool TestFourierTransform::testFFTPadding(const double testDuration)
{
	static_assert(std::is_same<TDataType, float>() || std::is_same<TDataType, double>(), "This is only defined for float and double");

	constexpr FrameType::DataType dataType = FrameType::dataType<TDataType>();

	ocean_assert(testDuration > 0.0);

	Log::info() << "FFT test for type \'" << TypeNamer::name<TDataType>() << "\' and with padding:";

	constexpr uint32_t width = 1920u;
	constexpr uint32_t height = 1080u;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	uint32_t totalIteration = 0u;
	TDataType maxAbsErrors[2] = {TDataType(0), TDataType(0)};
	bool validationSuccessful = true;

	constexpr TDataType maxAllowedAbsDifference = std::is_same<TDataType, float>::value ? TDataType(100) : TDataType(0.1);

	const Timestamp startTimestamp(true);

	do
	{
		const bool measurePerformance = RandomI::random(randomGenerator, 1u) == 0u;

		const uint32_t sourceWidth = measurePerformance ? width : RandomI::random(randomGenerator, 2u, width);
		const uint32_t sourceHeight = measurePerformance ? height : RandomI::random(randomGenerator, 2u, height);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat(dataType, 1u), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame frequencyFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, FrameType::genericPixelFormat(dataType, 2u)), &randomGenerator);

		const Frame copyFrequencyFrame(frequencyFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		performanceOcean.startIf(measurePerformance);
			FourierTransformation::dft0(sourceFrame.constdata<void>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), frequencyFrame.data<void>(), frequencyFrame.channels(), dataType, cv::DFT_COMPLEX_OUTPUT, 0, sourceFrame.paddingElements(), frequencyFrame.paddingElements());
		performanceOcean.stopIf(measurePerformance);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(frequencyFrame, copyFrequencyFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		// OpenCV
		Frame ocvFrequencyFrame(frequencyFrame.frameType());

		performanceOpenCV.startIf(measurePerformance);
			cv::dft(CV::OpenCVUtilities::toCvMat(sourceFrame, /* copy */ false), CV::OpenCVUtilities::toCvMat(ocvFrequencyFrame, /* copy */ false), cv::DFT_COMPLEX_OUTPUT);
		performanceOpenCV.stopIf(measurePerformance);

		// Validation
		ocean_assert(frequencyFrame.frameType() == ocvFrequencyFrame.frameType());
		ocean_assert(frequencyFrame.channels() == 2u);

		TDataType currentMaxAbsErrors[2] = {TDataType(0), TDataType(0)};

		for (uint32_t y = 0u; y < frequencyFrame.height(); ++y)
		{
			const TDataType* frequencyFrameRow = frequencyFrame.constrow<TDataType>(y);
			const TDataType* ocvFrequencyFrameRow = ocvFrequencyFrame.constrow<TDataType>(y);

			for (uint32_t x = 0u; x < frequencyFrame.width(); ++x)
			{
				for (uint32_t c = 0u; c < 2u; ++c)
				{
					const TDataType oceanValue = frequencyFrameRow[2u * x + c];
					const TDataType opencvValue = ocvFrequencyFrameRow[2u * x + c];

					const TDataType absAbsoluteError = NumericT<TDataType>::abs(oceanValue - opencvValue);

					currentMaxAbsErrors[c] = std::max(currentMaxAbsErrors[c], absAbsoluteError);
				}
			}
		}

		for (size_t i = 0; i < 2; ++i)
		{
			maxAbsErrors[i] = std::max(maxAbsErrors[i], currentMaxAbsErrors[i]);

			if (currentMaxAbsErrors[i] >= maxAllowedAbsDifference)
			{
				validationSuccessful = false;
			}
		}

		totalIteration++;
	}
	while (performanceOcean.measurements() == 0u || performanceOpenCV.measurements() == 0u || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance factor: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 1u) << "] x";

	Log::info() << "Iterations: " << totalIteration;
	Log::info() << "Max. absolute error, real: " << String::toAString(maxAbsErrors[0], 3u) << ", imaginary: " << String::toAString(maxAbsErrors[1], 3u);
	Log::info() << "Validation: " << (validationSuccessful ? "passed" : "FAILED");

	return validationSuccessful;
}

template bool TestFourierTransform::testFFTPadding<float>(const double);
template bool TestFourierTransform::testFFTPadding<double>(const double);

bool TestFourierTransform::testElementwiseMultiplicationCCS(const double testDuration)
{
	bool allSucceeded = true;

	allSucceeded = testElementwiseMultiplicationCCS<float, false>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testElementwiseMultiplicationCCS<float, true>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testElementwiseMultiplicationCCS<double, false>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testElementwiseMultiplicationCCS<double, true>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "Test for the multiplication of DFT spectra " << (allSucceeded ? "was successful" : "FAILED");
	Log::info() << " ";


	return allSucceeded;
}

template <typename TElementType, bool tConjugateB>
bool TestFourierTransform::testElementwiseMultiplicationCCS(const double testDuration)
{
	static_assert(std::is_same<TElementType, float>() || std::is_same<TElementType, double>(), "TElementType must be float xor double");
	ocean_assert(testDuration > 0.0);

	Log::info() << "Multiplication of Fourier spectra (" << TypeNamer::name<TElementType>() << (tConjugateB ? ", conjugate B" : "") << "):";

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<TElementType, 1u>();

	auto cvSpatialToFrequency = [](const Frame& frameY8C1) -> cv::Mat
	{
		ocean_assert(frameY8C1.channels() == 1u && frameY8C1.isValid());

		Frame frameFloat(FrameType(frameY8C1, pixelFormat));

		if (FrameType::dataType(pixelFormat) == FrameType::DT_SIGNED_FLOAT_32)
		{
			CV::FrameConverter::cast(frameY8C1.constdata<uint8_t>(), frameFloat.data<float>(), frameY8C1.width(), frameY8C1.height(), 1u, frameY8C1.paddingElements(), frameFloat.paddingElements());
		}
		else
		{
			ocean_assert(FrameType::dataType(pixelFormat) == FrameType::DT_SIGNED_FLOAT_64);
			CV::FrameConverter::cast(frameY8C1.constdata<uint8_t>(), frameFloat.data<double>(), frameY8C1.width(), frameY8C1.height(), 1u, frameY8C1.paddingElements(), frameFloat.paddingElements());
		}

		const cv::Mat cvFrameFloat = CV::OpenCVUtilities::toCvMat(frameFloat, false);
		cv::Mat cvDftSpectrum;
		cv::dft(cvFrameFloat, cvDftSpectrum, 0, 0 /* = make OpenCV use the CCS-packed format */);

		return cvDftSpectrum;
	};

	unsigned int counter = 0u;
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 640u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 480u, 1080u);

		Frame frameY8C1(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(frameY8C1, false, &randomGenerator);
		const cv::Mat cvSourceA = cvSpatialToFrequency(frameY8C1);

		CV::CVUtilities::randomizeFrame(frameY8C1, false, &randomGenerator);
		const cv::Mat cvSourceB = cvSpatialToFrequency(frameY8C1);

		cv::Mat cvProduct;
		const Frame sourceA = CV::OpenCVUtilities::toOceanFrame(cvSourceA, false, pixelFormat);
		const Frame sourceB = CV::OpenCVUtilities::toOceanFrame(cvSourceB, false, pixelFormat);
		Frame product(sourceA.frameType());

		if (counter % 2u == 0u)
		{
			// OpenCV runs first, Ocean runs second
			performanceOpenCV.start();
			cv::mulSpectrums(cvSourceA, cvSourceB, cvProduct, 0, tConjugateB);
			performanceOpenCV.stop();

			performanceOcean.start();
			FourierTransformation::elementwiseMultiplicationCCS<TElementType, false, tConjugateB, double>(sourceA.constdata<TElementType>(), sourceB.constdata<TElementType>(), product.data<TElementType>(), sourceA.width(), sourceA.height(), sourceA.paddingElements(), sourceB.paddingElements(), product.paddingElements());
			performanceOcean.stop();
		}
		else
		{
			// Ocean runs first, OpenCV runs second
			performanceOcean.start();
			FourierTransformation::elementwiseMultiplicationCCS<TElementType, false, tConjugateB, double>(sourceA.constdata<TElementType>(), sourceB.constdata<TElementType>(), product.data<TElementType>(), sourceA.width(), sourceA.height(), sourceA.paddingElements(), sourceB.paddingElements(), product.paddingElements());
			performanceOcean.stop();

			performanceOpenCV.start();
			cv::mulSpectrums(cvSourceA, cvSourceB, cvProduct, 0, tConjugateB);
			performanceOpenCV.stop();
		}

		counter++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance factor: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 1u) << "] x";

	// Validation
	bool validationSuccessful = true;
	const Timestamp validationStartTimestamp(true);
	std::vector<double> maxAbsoluteErrors;

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 640u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 480u, 1080u);

		Frame frameY8C1(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(frameY8C1, false, &randomGenerator);
		const cv::Mat cvSourceA = cvSpatialToFrequency(frameY8C1);

		CV::CVUtilities::randomizeFrame(frameY8C1, false, &randomGenerator);
		const cv::Mat cvSourceB = cvSpatialToFrequency(frameY8C1);

		cv::Mat cvProduct;
		const Frame sourceA = CV::OpenCVUtilities::toOceanFrame(cvSourceA, false, pixelFormat);
		const Frame sourceB = CV::OpenCVUtilities::toOceanFrame(cvSourceB, false, pixelFormat);
		Frame product(sourceA.frameType());

		performanceOpenCV.start();
		cv::mulSpectrums(cvSourceA, cvSourceB, cvProduct, 0, tConjugateB);
		performanceOpenCV.stop();

		performanceOcean.start();
		FourierTransformation::elementwiseMultiplicationCCS<TElementType, false, tConjugateB, double>(sourceA.constdata<TElementType>(), sourceB.constdata<TElementType>(), product.data<TElementType>(), sourceA.width(), sourceA.height(), sourceA.paddingElements(), sourceB.paddingElements(), product.paddingElements());
		performanceOcean.stop();

		double maxAbsoluteError = 0.0;
		const TElementType* cvProductData = (const TElementType*)cvProduct.data;
		const TElementType* productData = product.constdata<TElementType>();
		for (unsigned int i = 0u; i < product.pixels(); ++i)
		{
			const double absoluteError = std::abs((double)cvProductData[i] - (double)productData[i]);

			if (absoluteError > maxAbsoluteError)
			{
				maxAbsoluteError = absoluteError;
			}
		}

		maxAbsoluteErrors.push_back(maxAbsoluteError);

		counter++;
	}
	while (validationStartTimestamp + testDuration > Timestamp(true));

	if (!maxAbsoluteErrors.empty())
	{
		std::sort(maxAbsoluteErrors.begin(), maxAbsoluteErrors.end());

		const double minDifference = maxAbsoluteErrors[0];
		const double maxDifference = maxAbsoluteErrors.back();
		const double medianDifference = maxAbsoluteErrors[maxAbsoluteErrors.size() / 2];
		const double averageDifference = std::accumulate(maxAbsoluteErrors.begin(), maxAbsoluteErrors.end(), 0.0) / maxAbsoluteErrors.size();

		Log::info() << "Difference to OpenCV (min/median/avg/max): [" << String::toAString(minDifference, 6u) << ", " << String::toAString(medianDifference, 6u) << ", " << String::toAString(averageDifference, 6u) << ", " << String::toAString(maxDifference, 6u) << "]";

		validationSuccessful = maxDifference < (std::is_same<TElementType, float>() ? 0.01 : 0.0001);
	}
	else
	{
		Log::error() << "Not enough data for validation - check the value of the test duration";

		validationSuccessful = false;
	}

	Log::info() << "Validation: " << (validationSuccessful ? "pass" : "FAIL");

	return validationSuccessful;
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
