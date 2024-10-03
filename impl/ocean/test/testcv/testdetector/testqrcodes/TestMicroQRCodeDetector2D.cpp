/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestMicroQRCodeDetector2D.h"

#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

#include <algorithm>
#include <cctype>

#ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
#include "ocean/io/Directory.h"
#include "ocean/io/image/Image.h"

#include <cstdint>
#include <fstream>
#include <sstream>
#endif

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestQRCodes
{

using namespace CV::Detector::QRCodes;

bool TestMicroQRCodeDetector2D::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Micro QR code detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testStressTest(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesSmallImageSyntheticData(0u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesSmallImageSyntheticData(1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesSmallImageSyntheticData(3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesSmallImageSyntheticData(5u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesSmallImageSyntheticData(7u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesLargeImageSyntheticData(0u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesLargeImageSyntheticData(1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesLargeImageSyntheticData(3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesLargeImageSyntheticData(5u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectMicroQRCodesLargeImageSyntheticData(7u, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Micro QR code detector test succeeded.";
	}
	else
	{
		Log::info() << "Micro QR code detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMicroQRCodeDetector2D, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testStressTest(GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesSmallImageSyntheticDataNoGaussianFilter)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesSmallImageSyntheticData(0u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesSmallImageSyntheticDataGaussianFilter1)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesSmallImageSyntheticData(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesSmallImageSyntheticDataGaussianFilter3)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesSmallImageSyntheticData(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesSmallImageSyntheticDataGaussianFilter5)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesSmallImageSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesSmallImageSyntheticDataGaussianFilter7)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesSmallImageSyntheticData(7u, GTEST_TEST_DURATION, worker));
}

// Disable large image tests on hardware with limited performance.
// Short duration for test (GTEST_TEST_DURATION seconds) does not allow for enough iterations to yield meaningful success rate values and increases noise from frequent failures.
#if !defined(OCEAN_HARDWARE_REDUCED_PERFORMANCE)

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesLargeImageSyntheticDataNoGaussianFilter)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesLargeImageSyntheticData(0u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesLargeImageSyntheticDataGaussianFilter1)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesLargeImageSyntheticData(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesLargeImageSyntheticDataGaussianFilter3)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesLargeImageSyntheticData(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesLargeImageSyntheticDataGaussianFilter5)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesLargeImageSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

TEST(TestMicroQRCodeDetector2D, TestDetectMicroQRCodesLargeImageSyntheticDataGaussianFilter7)
{
	Worker worker;
	EXPECT_TRUE(TestMicroQRCodeDetector2D::testDetectMicroQRCodesLargeImageSyntheticData(7u, GTEST_TEST_DURATION, worker));
}

#endif // !defined(OCEAN_HARDWARE_REDUCED_PERFORMANCE)

#endif // OCEAN_USE_GTEST

bool TestMicroQRCodeDetector2D::testStressTest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool useWorker : {false, true})
		{
			Worker* workerToUse = useWorker ? &worker : nullptr;

			const unsigned int width = RandomI::random(randomGenerator, 15u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 15u, 1920u);

			const SharedAnyCamera anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, Numeric::deg2rad(Scalar(60))));
			ocean_assert(anyCamera != nullptr && anyCamera->isValid());

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			MicroQRCodeDetector2D::Observations observations;
			const MicroQRCodes codes = MicroQRCodeDetector2D::detectMicroQRCodes(*anyCamera, frame, &observations, workerToUse);

			OCEAN_EXPECT_EQUAL(validation, codes.size(), observations.size());
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Stress test: " << validation;

	return validation.succeeded();
}

bool TestMicroQRCodeDetector2D::testDetectMicroQRCodesSmallImageSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker)
{
	static_assert(std::is_same_v<Scalar, double> || std::is_same_v<Scalar, float>);

	const double validationPrecisionThreshold = std::is_same_v<Scalar, double> ? 0.85 : 0.80;

	return testDetectMicroQRCodesSyntheticData_Internal(gaussianFilterSize, testDuration, worker, "SmallImage", validationPrecisionThreshold);
}

bool TestMicroQRCodeDetector2D::testDetectMicroQRCodesLargeImageSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker)
{
	static_assert(std::is_same_v<Scalar, double> || std::is_same_v<Scalar, float>);

	const double validationPrecisionThreshold = std::is_same_v<Scalar, double> ? 0.85 : 0.80;

	return testDetectMicroQRCodesSyntheticData_Internal(gaussianFilterSize, testDuration, worker, "LargeImage", validationPrecisionThreshold, 6u, 20u, 2048u, 4096u);
}

bool TestMicroQRCodeDetector2D::testDetectMicroQRCodesSyntheticData_Internal(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker, const std::string& testLabel, const double validationPrecisionThreshold, const unsigned int moduleSizePixelsMin, const unsigned int moduleSizePixelsMax, const unsigned int imageDimPixelsMin, const unsigned int imageDimPixelsMax)
{
	ocean_assert(gaussianFilterSize == 0u || gaussianFilterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);
	ocean_assert(testLabel.end() == std::find_if(testLabel.begin(), testLabel.end(), [](char c) { return !std::isalnum(c); }) && "testLabel must be alphanumeric");
	ocean_assert(validationPrecisionThreshold >= 0.0 && validationPrecisionThreshold <= 1.0);
	ocean_assert(moduleSizePixelsMin >= 1u);
	ocean_assert(moduleSizePixelsMax >= moduleSizePixelsMin);
	ocean_assert(imageDimPixelsMin >= 0u);
	ocean_assert(imageDimPixelsMax >= 0u);

	Log::info() << "Detect Micro QR codes test using synthetic data (" << testLabel << ", " << (gaussianFilterSize == 0u ? "no Gaussian filter" : "Gaussian filter:" + String::toAString(gaussianFilterSize)) << ")";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(validationPrecisionThreshold, randomGenerator);

	Timestamp startTimestamp(true);

#ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
	std::uint64_t testImageIndex = 0u;
#endif

	const std::array<EncodingParameters, 20> encodingParameters
	{{
		{MicroQRCode::EM_NUMERIC, 1u, 5u},
		{MicroQRCode::EM_NUMERIC, 9u, 10u},
		{MicroQRCode::EM_ALPHANUMERIC, 6u, 6u},
		{MicroQRCode::EM_NUMERIC, 6u, 8u},
		{MicroQRCode::EM_ALPHANUMERIC, 1u, 5u},
		{MicroQRCode::EM_NUMERIC, 19u, 23u},
		{MicroQRCode::EM_ALPHANUMERIC, 12u, 14u},
		{MicroQRCode::EM_BYTE, 8u, 9u},
		{MicroQRCode::EM_NUMERIC, 11u, 18u},
		{MicroQRCode::EM_ALPHANUMERIC, 7u, 11u},
		{MicroQRCode::EM_BYTE, 1u, 7u},
		{MicroQRCode::EM_NUMERIC, 31u, 35u},
		{MicroQRCode::EM_ALPHANUMERIC, 19u, 21u},
		{MicroQRCode::EM_BYTE, 14u, 15u},
		{MicroQRCode::EM_NUMERIC, 24u, 30u},
		{MicroQRCode::EM_ALPHANUMERIC, 15u, 18u},
		{MicroQRCode::EM_BYTE, 10u, 13u},
		{MicroQRCode::EM_NUMERIC, 1u, 21u, MicroQRCode::ECC_25},
		{MicroQRCode::EM_ALPHANUMERIC, 1u, 13u, MicroQRCode::ECC_25},
		{MicroQRCode::EM_BYTE, 1u, 9u, MicroQRCode::ECC_25},
	}};
	
	do
	{
		// Create a synthetic Micro QR code
		const EncodingParameters& params = encodingParameters[RandomI::random(randomGenerator, (unsigned int)(encodingParameters.size() - 1))];


		MicroQRCode groundtruthCode;
		std::vector<uint8_t> data;
		const unsigned int messageLength = RandomI::random(randomGenerator, params.minimumMessageLength, params.maximumMessageLength);

		switch (params.encodingMode)
		{
			case MicroQRCode::EM_NUMERIC:
				MicroQRCodeEncoder::encodeText(Utilities::generateRandomNumericString(randomGenerator, messageLength), params.errorCorrectionCapacity, groundtruthCode);
				break;

			case MicroQRCode::EM_ALPHANUMERIC:
				MicroQRCodeEncoder::encodeText(Utilities::generateRandomAlphanumericString(randomGenerator, messageLength), params.errorCorrectionCapacity, groundtruthCode);
				break;

			case MicroQRCode::EM_BYTE:
				Utilities::generateRandomByteData(randomGenerator, messageLength, data);
				MicroQRCodeEncoder::encodeBinary(data, params.errorCorrectionCapacity, groundtruthCode);
				break;

			default:
				ocean_assert(false && "Never be here!");
				continue;
		}

		if (!groundtruthCode.isValid())
		{
			ocean_assert(false && "Never be here!");
			continue;
		}

		// Draw the ground truth code
		const uint8_t minimumContrast = gaussianFilterSize < 3u ? 50u : 70u;

		const uint8_t lowIntensity = uint8_t(RandomI::random(randomGenerator, 150u));
		ocean_assert(lowIntensity + minimumContrast <= 255u);

		const uint8_t highIntensity = uint8_t(RandomI::random(randomGenerator, (unsigned int)(lowIntensity + minimumContrast), 255u));
		ocean_assert(highIntensity > lowIntensity && highIntensity - lowIntensity >= minimumContrast);

		constexpr bool isNormalReflectance = true; // TODO Enable random reflectance once the finder pattern detector supports it

		const uint8_t foregroundValue = isNormalReflectance ? lowIntensity : highIntensity;
		const uint8_t backgroundValue = isNormalReflectance ? highIntensity : lowIntensity;

		const unsigned int codeBorderModules = 4u;
		const unsigned int codeWithBorderModulesPerSide = 2u * codeBorderModules + groundtruthCode.modulesPerSide();
		const unsigned int targetFrameWithCodeSize = RandomI::random(randomGenerator, moduleSizePixelsMin * codeWithBorderModulesPerSide, moduleSizePixelsMax * codeWithBorderModulesPerSide);

		const Frame frameWithCode = CV::Detector::QRCodes::Utilities::draw(groundtruthCode, targetFrameWithCodeSize, /* allowTrueMultiple */ true, /* border */ codeBorderModules, &worker, foregroundValue, backgroundValue);

		ocean_assert(frameWithCode.isValid() && frameWithCode.width() >= targetFrameWithCodeSize && frameWithCode.height() >= targetFrameWithCodeSize);
		ocean_assert(frameWithCode.width() % codeWithBorderModulesPerSide == 0);

		const unsigned int codeSizeInFramePixelsPerSide = frameWithCode.width() / codeWithBorderModulesPerSide * groundtruthCode.modulesPerSide();

		// Draw a randomly rotated version of the Micro QR code into the center of the final frame
		const Scalar rotation = Random::scalar(randomGenerator, Scalar(0), Numeric::pi2());
		const Scalar qrcodeCenterToCornerDistancePixels = codeSizeInFramePixelsPerSide * Scalar(0.5) * Numeric::sqrt(Scalar(2));
		const Scalar qrcodeDiagonalOrientation = Numeric::pi_4() + rotation;
		const Scalar maxQRCodeImageDeviationXY = std::max(Numeric::abs(Numeric::cos(qrcodeDiagonalOrientation)), Numeric::abs(Numeric::sin(qrcodeDiagonalOrientation))) * qrcodeCenterToCornerDistancePixels;

		// The Micro QR code is rotated by a random angle and drawn.  Set minimum frame size to be able to accommodate rotated image with room for margin required by detector.
		constexpr unsigned int imageMarginPixels = 11u;
		const unsigned int minFrameWidth = std::max(imageDimPixelsMin, (unsigned int)Numeric::round32(maxQRCodeImageDeviationXY * 2) + 2u + 2u * imageMarginPixels);
		const unsigned int minFrameHeight = std::max(imageDimPixelsMin, (unsigned int)Numeric::round32(maxQRCodeImageDeviationXY * 2) + 2u + 2u * imageMarginPixels);

		const unsigned int maxFrameWidth = std::max(imageDimPixelsMax, minFrameWidth);
		const unsigned int maxFrameHeight = std::max(imageDimPixelsMax, minFrameHeight);

		const unsigned int frameWidth = RandomI::random(randomGenerator, minFrameWidth, maxFrameWidth);
		const unsigned int frameHeight = RandomI::random(randomGenerator, minFrameHeight, maxFrameHeight);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth, frameHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		frame.setValue(backgroundValue);

		const Scalar maxOffsetMagnitudeX = frameWidth / 2 - maxQRCodeImageDeviationXY - imageMarginPixels;
		const Scalar maxOffsetMagnitudeY = frameHeight / 2 - maxQRCodeImageDeviationXY - imageMarginPixels;

		ocean_assert(maxOffsetMagnitudeX >= 0.0 && maxOffsetMagnitudeY >= 0.0);

		const Scalar offsetX = Random::scalar(randomGenerator, -maxOffsetMagnitudeX, maxOffsetMagnitudeX);
		const Scalar offsetY = Random::scalar(randomGenerator, -maxOffsetMagnitudeY, maxOffsetMagnitudeY);
		const Vector2 frameCenterOffset(frame.width() * Scalar(0.5) + offsetX, frame.height() * Scalar(0.5) + offsetY);


		const SquareMatrix3 frameWithCodeCenteredRotated_T_frame = SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(-frameCenterOffset, 1));
		const SquareMatrix3 frameWithCodeCentered_R_frameWithCodeCenteredRotated = SquareMatrix3(Rotation(Vector3(0, 0, 1), rotation));
		const SquareMatrix3 frameWithCode_T_frameWithCodeOffsetFromCenter = SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(frameWithCode.width() * Scalar(0.5), frameWithCode.height() * Scalar(0.5), 1));
		const SquareMatrix3 frameWithCode_T_frame = frameWithCode_T_frameWithCodeOffsetFromCenter * frameWithCodeCentered_R_frameWithCodeCenteredRotated * frameWithCodeCenteredRotated_T_frame;

		if (!CV::FrameInterpolatorBilinear::Comfort::affine(frameWithCode, frame, frameWithCode_T_frame, &backgroundValue, &worker))
		{
			ocean_assert(false && "This should never happen!");

			OCEAN_SET_FAILED(validation);
		}

		if (gaussianFilterSize != 0u)
		{
			CV::FrameFilterGaussian::filter(frame, gaussianFilterSize , &worker);
		}

		const SharedAnyCamera anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera(frame.width(), frame.height(), Numeric::deg2rad(Scalar(60))));
		ocean_assert(anyCamera != nullptr && anyCamera->isValid());

#ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
		unsigned failedDetectionCount = 0u;
		unsigned failedGroundTruthComparisonCount = 0u;
		++testImageIndex;
#endif

		for (const bool useWorker : {true, false})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			Worker* workerToUse = useWorker ? &worker : nullptr;

			MicroQRCodeDetector2D::Observations observations;
			const MicroQRCodes codes = MicroQRCodeDetector2D::detectMicroQRCodes(*anyCamera, frame, &observations, workerToUse);

			bool detectionSuccess = codes.size() == 1;
			bool groundTruthComparisonSuccess = detectionSuccess && codes[0].isSame(groundtruthCode, false);

			if (!detectionSuccess || !groundTruthComparisonSuccess)
			{
				scopedIteration.setInaccurate();
			}

#ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
			bool failedAtDetection = !detectionSuccess;
			bool failedAtGroundTruthComparison = detectionSuccess && !groundTruthComparisonSuccess;

			// Differentiate cases of ground truth comparison failure where extracted modules in detected Micro QR code is wrong but otherwise decoded data is correct due to error correction.
			bool failedAtGroundTruthModulesComparison = failedAtGroundTruthComparison && codes[0].isSame(groundtruthCode, true);

			if (failedAtDetection)
			{
				if (codes.size() == 0)
				{
					Log::error() << "Did not detect any Micro QR codes (seed:" << randomGenerator.initialSeed() << ", #" << testImageIndex << ", useWorker:" << useWorker << ")";
				}

				if (codes.size() > 1)
				{
					Log::error() << "Detected " << codes.size() << " Micro QR codes (seed:" << randomGenerator.initialSeed() << ", #" << testImageIndex << ", useWorker:" << useWorker << ")";
				}
			}

			if (failedAtGroundTruthComparison)
			{
				Log::error() << "Detected 1 Micro QR code as expected but decoded value differs from ground truth value " << (failedAtGroundTruthModulesComparison ? "solely due to difference in modules extracted" : "") << " (seed:" << randomGenerator.initialSeed() << ", #" << testImageIndex << ", useWorker:" << useWorker << ")";
			}

			failedDetectionCount += failedAtDetection ? 1 : 0;
			failedGroundTruthComparisonCount += failedAtGroundTruthComparison ? 1 : 0;

			if ((failedAtDetection && failedDetectionCount == 1) || (failedAtGroundTruthComparison && failedGroundTruthComparisonCount == 1))
			{
				IO::Directory failureDataDirectory;
				ocean_assert(!failureDataDirectory.isNull() && "Destination directory for failure data must be specified.");

				if (!failureDataDirectory.exists())
				{
					static bool directoryCreationPreviouslyFailed = false;

					if (directoryCreationPreviouslyFailed)
					{
						continue;
					}

					if (!failureDataDirectory.create())
					{
						directoryCreationPreviouslyFailed = true;
						continue;
					}
				}

				ocean_assert(failureDataDirectory.exists() && "Destination directory for failure data must exist.");

				std::stringstream filenameprefix;

				if (failedAtGroundTruthComparison)
				{
					if (failedAtGroundTruthModulesComparison)
					{
						// Detected QRCode and ground truth QRCode represent same data, but modules extracted by detector does not match ground truth values.
						filenameprefix << "microqrdetect2d_failed_module-comparison_" << testLabel << "_gfs-";
					}
					else
					{
						filenameprefix << "microqrdetect2d_failed_comparison_" << testLabel << "_gfs-";
					}
				}
				else
				{
					ocean_assert(failedAtDetection);
					filenameprefix << "microqrdetect2d_failed_detect_" << testLabel << "_gfs-";
				}

				filenameprefix << gaussianFilterSize << "_" << randomGenerator.initialSeed() << "_" << testImageIndex;

				IO::Image::Comfort::writeImage(frame, (failureDataDirectory + IO::File(filenameprefix.str() + ".png"))(), true);

				Frame rgbFrame;
				if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, true, &worker))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}
				CV::Detector::QRCodes::Utilities::drawObservations(*anyCamera, rgbFrame, observations, codes);
				if (observations.empty())
				{
					// Draw finder patterns and potential timing patterns
					const FinderPatterns finderPatterns = FinderPatternDetector::detectFinderPatterns(frame.constdata<uint8_t>(), frame.width(), frame.height(), /* minimumDistance */ 10u, frame.paddingElements(), worker);
					for (const FinderPattern& finderPattern : finderPatterns)
					{
						CV::Detector::QRCodes::Utilities::drawFinderPattern(rgbFrame, finderPattern, CV::Canvas::red(FrameType::FORMAT_RGB24));
						for (unsigned int corner = 0u; corner < 4u; corner++)
						{
							bool isCandidate = true;
							for (unsigned int direction = 0u; isCandidate && direction < 2u; direction++)
							{
								unsigned int offset = 3u - direction * 2u;
								Vectors2 timingPattern;
								if (MicroQRCodeDetector::getTimingPatternModules(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), finderPattern, corner, (corner + offset) % 4u, timingPattern))
								{
									for (unsigned int i = 0u; i < timingPattern.size(); i++)
									{
										CV::Canvas::point<5u>(rgbFrame, timingPattern[i], CV::Canvas::yellow(FrameType::FORMAT_RGB24));
										if (i > 0u)
										{
											CV::Detector::QRCodes::Utilities::drawLine<1u>(*anyCamera, rgbFrame, timingPattern[i - 1u], timingPattern[i], CV::Canvas::yellow(FrameType::FORMAT_RGB24));
										}
									}
								}
							}
						}
					}
				}
				IO::Image::Comfort::writeImage(rgbFrame, (failureDataDirectory + IO::File(filenameprefix.str() + "_observations.png"))(), true);

				std::ofstream groundtruthFile((failureDataDirectory + IO::File(filenameprefix.str() + "_groundtruth.txt"))());
				groundtruthFile << Utilities::translateQRCodeToString(groundtruthCode);

				if (failedAtGroundTruthComparison)
				{
					std::ofstream detectedCodeFile((failureDataDirectory + IO::File(filenameprefix.str() + "_detected.txt"))());
					detectedCodeFile << Utilities::translateQRCodeToString(codes[0]);
				}
			}
#endif // #ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
