/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrequencyAnalysis.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrequencyAnalysis.h"

#include "ocean/math/Complex.h"
#include "ocean/math/Numeric.h"

#include <vector>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrequencyAnalysis::test(const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("FrequencyAnalysis test");
	Log::info() << " ";

	if (selector.shouldRun("roundtripidentity"))
	{
		testResult = testRoundTripIdentity(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("dccomponent"))
	{
		testResult = testDCComponent(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("parseval"))
	{
		testResult = testParseval(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("magnitudeframe"))
	{
		testResult = testMagnitudeFrame(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("workerequivalencestress"))
	{
		testResult = testWorkerEquivalenceStress(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("roundtripstress"))
	{
		testResult = testRoundTripStress(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("constantinputspectrumstress"))
	{
		testResult = testConstantInputSpectrumStress(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("paddinginvariancestress"))
	{
		testResult = testPaddingInvarianceStress(testDuration, worker);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << testResult;
	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrequencyAnalysis, RoundTripIdentity)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testRoundTripIdentity(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, DCComponent)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testDCComponent(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, Parseval)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testParseval(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, MagnitudeFrame)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testMagnitudeFrame(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, WorkerEquivalenceStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testWorkerEquivalenceStress(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, RoundTripStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testRoundTripStress(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, ConstantInputSpectrumStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testConstantInputSpectrumStress(GTEST_TEST_DURATION, worker));
}

TEST(TestFrequencyAnalysis, PaddingInvarianceStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrequencyAnalysis::testPaddingInvarianceStress(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

namespace
{

/**
 * Helper that fills a frame's elements with random uint8_t values.
 */
void fillRandomFrame(RandomGenerator& randomGenerator, Frame& frame)
{
	const unsigned int channels = frame.channels();
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		uint8_t* row = frame.row<uint8_t>(y);
		for (unsigned int x = 0u; x < frame.width() * channels; ++x)
		{
			row[x] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}
	}
}

/**
 * Returns one of {Y8, RGB24, RGBA32}.
 */
FrameType::PixelFormat randomPixelFormat(RandomGenerator& randomGenerator)
{
	const unsigned int sel = RandomI::random(randomGenerator, 0u, 2u);
	if (sel == 0u) return FrameType::FORMAT_Y8;
	if (sel == 1u) return FrameType::FORMAT_RGB24;
	return FrameType::FORMAT_RGBA32;
}

} // namespace

bool TestFrequencyAnalysis::testRoundTripIdentity(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing image2frequencies -> frequencies2image identity:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 4u, 32u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 32u);

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Complexes frequencies;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		const bool ok1 = CV::FrequencyAnalysis::image2frequencies(source, frequencies, w);
		OCEAN_EXPECT_TRUE(validation, ok1);
		OCEAN_EXPECT_EQUAL(validation, (unsigned int)frequencies.size(), width * height * source.channels());

		Frame target(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		target.setValue(0u);

		const bool ok2 = CV::FrequencyAnalysis::frequencies2image(frequencies.data(), target, w);
		OCEAN_EXPECT_TRUE(validation, ok2);

		// Round-trip should reproduce the original to within 1 LSB (uint8 quantisation + FFT
		// numerical error).
		const unsigned int channels = source.channels();
		for (unsigned int y = 0u; y < height; ++y)
		{
			const uint8_t* rowS = source.constrow<uint8_t>(y);
			const uint8_t* rowT = target.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < width * channels; ++x)
			{
				const int diff = int(rowS[x]) - int(rowT[x]);
				OCEAN_EXPECT_TRUE(validation, diff >= -1 && diff <= 1);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testDCComponent(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing DC component equals sum of pixel values:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 2u, 24u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 24u);

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		const unsigned int channels = source.channels();

		Complexes frequencies;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrequencyAnalysis::image2frequencies(source, frequencies, w);
		OCEAN_EXPECT_TRUE(validation, ok);

		const unsigned int channelStride = width * height;

		// Each channel's DC == sum of its pixel values (real); imaginary part should be ~0.
		for (unsigned int c = 0u; c < channels; ++c)
		{
			Scalar pixelSum = Scalar(0);
			for (unsigned int y = 0u; y < height; ++y)
			{
				const uint8_t* row = source.constrow<uint8_t>(y);
				for (unsigned int x = 0u; x < width; ++x)
				{
					pixelSum += Scalar(row[x * channels + c]);
				}
			}

			const Complex dc = frequencies[c * channelStride + 0u];

			// For these small frames, sums are at most 24*24*255 ~ 147000; tolerance reflects FFT
			// double-precision error.
			const Scalar tolerance = Scalar(0.01) + Numeric::abs(pixelSum) * Scalar(1e-5);

			OCEAN_EXPECT_TRUE(validation, Numeric::abs(dc.real() - pixelSum) <= tolerance);
			OCEAN_EXPECT_TRUE(validation, Numeric::abs(dc.imag()) <= tolerance);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testParseval(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing Parseval's theorem N*sum(|x|^2) == sum(|X|^2):";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 2u, 16u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 16u);

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		const unsigned int channels = source.channels();
		const unsigned int N = width * height;

		Complexes frequencies;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrequencyAnalysis::image2frequencies(source, frequencies, w);
		OCEAN_EXPECT_TRUE(validation, ok);

		// For the unnormalised DFT used here (DC == sum of inputs), Parseval's identity is:
		//     N * sum(|x[n]|^2) == sum(|X[k]|^2)
		for (unsigned int c = 0u; c < channels; ++c)
		{
			Scalar timeEnergy = Scalar(0);
			for (unsigned int y = 0u; y < height; ++y)
			{
				const uint8_t* row = source.constrow<uint8_t>(y);
				for (unsigned int x = 0u; x < width; ++x)
				{
					const Scalar v = Scalar(row[x * channels + c]);
					timeEnergy += v * v;
				}
			}

			Scalar freqEnergy = Scalar(0);
			for (unsigned int k = 0u; k < N; ++k)
			{
				const Complex& f = frequencies[c * N + k];
				freqEnergy += f.real() * f.real() + f.imag() * f.imag();
			}

			const Scalar lhs = Scalar(N) * timeEnergy;
			const Scalar tolerance = Scalar(1e-3) + Numeric::abs(lhs) * Scalar(1e-5);

			OCEAN_EXPECT_TRUE(validation, Numeric::abs(lhs - freqEnergy) <= tolerance);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testMagnitudeFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing magnitudeFrame() basic shape/validity:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 4u, 32u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 32u);

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		const Scalar octaves = Scalar(RandomI::random(randomGenerator, 1u, 8u));
		const bool shift = RandomI::random(randomGenerator, 1u) == 0u;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		const Frame magnitudeFrame = CV::FrequencyAnalysis::magnitudeFrame(source, octaves, shift, w);

		OCEAN_EXPECT_TRUE(validation, magnitudeFrame.isValid());
		OCEAN_EXPECT_EQUAL(validation, magnitudeFrame.width(), width);
		OCEAN_EXPECT_EQUAL(validation, magnitudeFrame.height(), height);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testWorkerEquivalenceStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: image2frequencies worker vs single-thread output equivalence:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 8u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 8u, 64u);

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Complexes freqSingle;
		Complexes freqWorker;

		const bool ok1 = CV::FrequencyAnalysis::image2frequencies(source, freqSingle, nullptr);
		const bool ok2 = CV::FrequencyAnalysis::image2frequencies(source, freqWorker, &worker);

		OCEAN_EXPECT_TRUE(validation, ok1);
		OCEAN_EXPECT_TRUE(validation, ok2);
		OCEAN_EXPECT_EQUAL(validation, freqSingle.size(), freqWorker.size());

		const Scalar tolerance = Scalar(1e-3);
		for (size_t i = 0u; i < freqSingle.size(); ++i)
		{
			OCEAN_EXPECT_TRUE(validation, Numeric::abs(freqSingle[i].real() - freqWorker[i].real()) <= tolerance);
			OCEAN_EXPECT_TRUE(validation, Numeric::abs(freqSingle[i].imag() - freqWorker[i].imag()) <= tolerance);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testRoundTripStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: round-trip identity across many parameter combinations:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 2u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 64u);
		const unsigned int sourcePadding = RandomI::random(randomGenerator, 0u, 32u);

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePadding);
		fillRandomFrame(randomGenerator, source);

		Complexes frequencies;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		const bool ok1 = CV::FrequencyAnalysis::image2frequencies(source, frequencies, w);
		OCEAN_EXPECT_TRUE(validation, ok1);

		Frame target(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		const bool ok2 = CV::FrequencyAnalysis::frequencies2image(frequencies.data(), target, w);
		OCEAN_EXPECT_TRUE(validation, ok2);

		const unsigned int channels = source.channels();
		for (unsigned int y = 0u; y < height; ++y)
		{
			const uint8_t* rowS = source.constrow<uint8_t>(y);
			const uint8_t* rowT = target.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < width * channels; ++x)
			{
				const int diff = int(rowS[x]) - int(rowT[x]);
				OCEAN_EXPECT_TRUE(validation, diff >= -1 && diff <= 1);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testConstantInputSpectrumStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: constant-input spectrum: only DC is non-zero:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 4u, 24u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 24u);
		const uint8_t value = uint8_t(RandomI::random(randomGenerator, 0u, 255u));

		Frame source(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		source.setValue(value);

		const unsigned int channels = source.channels();

		Complexes frequencies;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrequencyAnalysis::image2frequencies(source, frequencies, w);
		OCEAN_EXPECT_TRUE(validation, ok);

		const unsigned int channelStride = width * height;
		const Scalar tolerance = Scalar(1e-3);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			// DC == width * height * value (real, imag ~0)
			const Complex dc = frequencies[c * channelStride + 0u];
			const Scalar expectedDC = Scalar(width) * Scalar(height) * Scalar(value);

			OCEAN_EXPECT_TRUE(validation, Numeric::abs(dc.real() - expectedDC) <= tolerance + Numeric::abs(expectedDC) * Scalar(1e-5));
			OCEAN_EXPECT_TRUE(validation, Numeric::abs(dc.imag()) <= tolerance);

			// All other components ~ 0.
			for (unsigned int k = 1u; k < channelStride; ++k)
			{
				const Complex& f = frequencies[c * channelStride + k];
				OCEAN_EXPECT_TRUE(validation, Numeric::abs(f.real()) <= tolerance);
				OCEAN_EXPECT_TRUE(validation, Numeric::abs(f.imag()) <= tolerance);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrequencyAnalysis::testPaddingInvarianceStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: source padding does not change image2frequencies output:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = randomPixelFormat(randomGenerator);
		const unsigned int width = RandomI::random(randomGenerator, 4u, 24u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 24u);

		Frame source0(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), 0u);
		fillRandomFrame(randomGenerator, source0);

		const unsigned int sourcePaddingP = RandomI::random(randomGenerator, 1u, 32u);
		Frame sourceP(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingP);

		const unsigned int channels = source0.channels();
		for (unsigned int y = 0u; y < height; ++y)
		{
			memcpy(sourceP.row<uint8_t>(y), source0.constrow<uint8_t>(y), width * channels);
		}

		Complexes freq0;
		Complexes freqP;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		OCEAN_EXPECT_TRUE(validation, CV::FrequencyAnalysis::image2frequencies(source0, freq0, w));
		OCEAN_EXPECT_TRUE(validation, CV::FrequencyAnalysis::image2frequencies(sourceP, freqP, w));
		OCEAN_EXPECT_EQUAL(validation, freq0.size(), freqP.size());

		const Scalar tolerance = Scalar(1e-3);
		for (size_t i = 0u; i < freq0.size(); ++i)
		{
			OCEAN_EXPECT_TRUE(validation, Numeric::abs(freq0[i].real() - freqP[i].real()) <= tolerance);
			OCEAN_EXPECT_TRUE(validation, Numeric::abs(freq0[i].imag() - freqP[i].imag()) <= tolerance);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
