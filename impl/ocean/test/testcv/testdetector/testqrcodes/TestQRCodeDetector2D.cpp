/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestQRCodeDetector2D.h"

#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"
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

bool TestQRCodeDetector2D::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   QR code detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testStressTest(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSmallImageSyntheticData(0u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSmallImageSyntheticData(1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSmallImageSyntheticData(3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSmallImageSyntheticData(5u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSmallImageSyntheticData(7u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesLargeImageSyntheticData(0u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesLargeImageSyntheticData(1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesLargeImageSyntheticData(3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesLargeImageSyntheticData(5u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesLargeImageSyntheticData(7u, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "QR code detector test succeeded.";
	}
	else
	{
		Log::info() << "QR code detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestQRCodeDetector2D, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testStressTest(GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSmallImageSyntheticDataNoGaussianFilter)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSmallImageSyntheticData(0u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSmallImageSyntheticDataGaussianFilter1)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSmallImageSyntheticData(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSmallImageSyntheticDataGaussianFilter3)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSmallImageSyntheticData(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSmallImageSyntheticDataGaussianFilter5)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSmallImageSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSmallImageSyntheticDataGaussianFilter7)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSmallImageSyntheticData(7u, GTEST_TEST_DURATION, worker));
}

// Disable large image tests on hardware with limited performance.
// Short duration for test (GTEST_TEST_DURATION seconds) does not allow for enough iterations to yield meaningful success rate values and increases noise from frequent failures.
#if !defined(OCEAN_HARDWARE_REDUCED_PERFORMANCE)

TEST(TestQRCodeDetector2D, TestDetectQRCodesLargeImageSyntheticDataNoGaussianFilter)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesLargeImageSyntheticData(0u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesLargeImageSyntheticDataGaussianFilter1)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesLargeImageSyntheticData(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesLargeImageSyntheticDataGaussianFilter3)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesLargeImageSyntheticData(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesLargeImageSyntheticDataGaussianFilter5)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesLargeImageSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesLargeImageSyntheticDataGaussianFilter7)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesLargeImageSyntheticData(7u, GTEST_TEST_DURATION, worker));
}

#endif // !defined(OCEAN_HARDWARE_REDUCED_PERFORMANCE)

#endif // OCEAN_USE_GTEST

bool TestQRCodeDetector2D::testStressTest(const double testDuration, Worker& worker)
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

			const unsigned int width = RandomI::random(randomGenerator, 29u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 29u, 1920u);

			const SharedAnyCamera anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, Numeric::deg2rad(Scalar(60))));
			ocean_assert(anyCamera != nullptr && anyCamera->isValid());

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			QRCodeDetector2D::Observations observations;
			const QRCodes codes = QRCodeDetector2D::detectQRCodes(*anyCamera, frame, &observations, workerToUse);

			OCEAN_EXPECT_EQUAL(validation, codes.size(), observations.size());
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Stress test: " << validation;

	return validation.succeeded();
}

bool TestQRCodeDetector2D::testDetectQRCodesSmallImageSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker)
{
	static_assert(std::is_same_v<Scalar, double> || std::is_same_v<Scalar, float>);

	const double detectionValidationThreshold = std::is_same_v<Scalar, double> ? 0.95 : 0.9;
	const double groundtruthComparisonValidationThreshold = std::is_same_v<Scalar, double> ? 0.80 : 0.75;

	return testDetectQRCodesSyntheticData_Internal(gaussianFilterSize, testDuration, worker, "SmallImage", detectionValidationThreshold, groundtruthComparisonValidationThreshold);
}

bool TestQRCodeDetector2D::testDetectQRCodesLargeImageSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker)
{
	static_assert(std::is_same_v<Scalar, double> || std::is_same_v<Scalar, float>);

	const double detectionValidationThreshold = std::is_same_v<Scalar, double> ? 0.90 : 0.85;
	const double groundtruthComparisonValidationThreshold = std::is_same_v<Scalar, double> ? 0.75 : 0.70;

	return testDetectQRCodesSyntheticData_Internal(gaussianFilterSize, testDuration, worker, "LargeImage", detectionValidationThreshold, groundtruthComparisonValidationThreshold, 6u, 20u, 2048u, 4096u);
}

bool TestQRCodeDetector2D::testDetectQRCodesSyntheticData_Internal(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker, const std::string& testLabel, const double detectionValidationThreshold, const double groundtruthComparisonValidationThreshold, const unsigned int moduleSizePixelsMin, const unsigned int moduleSizePixelsMax, const unsigned int imageDimPixelsMin, const unsigned int imageDimPixelsMax)
{
	ocean_assert(gaussianFilterSize == 0u || gaussianFilterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);
	ocean_assert(testLabel.end() == std::find_if(testLabel.begin(), testLabel.end(), [](char c) { return !std::isalnum(c); }) && "testLabel must be alphanumeric");
	ocean_assert(detectionValidationThreshold >= 0.0 && detectionValidationThreshold <= 1.0);
	ocean_assert(groundtruthComparisonValidationThreshold >= 0.0 && groundtruthComparisonValidationThreshold <= detectionValidationThreshold);
	ocean_assert(moduleSizePixelsMin >= 1u);
	ocean_assert(moduleSizePixelsMax >= moduleSizePixelsMin);
	ocean_assert(imageDimPixelsMin >= 0u);
	ocean_assert(imageDimPixelsMax >= 0u);

	Log::info() << "Detect QR codes test using synthetic data (" << testLabel << ", " << (gaussianFilterSize == 0u ? "no Gaussian filter" : "Gaussian filter:" + String::toAString(gaussianFilterSize)) << ")";

	RandomGenerator randomGenerator;
	ValidationPrecision detectionValidation(detectionValidationThreshold, randomGenerator, 50u);
	ValidationPrecision groundtruthComparisonValidation(groundtruthComparisonValidationThreshold, randomGenerator, 50u);

	Timestamp startTimestamp(true);

#ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
	std::uint64_t testImageIndex = 0u;
#endif

	do
	{
		// Create a synthetic QR code
		constexpr QRCode::ErrorCorrectionCapacity errorCorrectionCapacities[4] =
		{
			QRCode::ECC_07,
			QRCode::ECC_15,
			QRCode::ECC_25,
			QRCode::ECC_30,
		};

		const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity = errorCorrectionCapacities[RandomI::random(randomGenerator, 3u)];

		constexpr QRCode::EncodingMode encodingModes[3] =
		{
			QRCode::EM_NUMERIC,
			QRCode::EM_ALPHANUMERIC,
			QRCode::EM_BYTE,
		};

		const QRCode::EncodingMode encodingMode = encodingModes[RandomI::random(randomGenerator, 2u)];

		QRCode groundtruthCode;
		std::vector<uint8_t> data;
		const unsigned int dataSizeInBytes = RandomI::random(randomGenerator, 1u, 128u);

		switch (encodingMode)
		{
			case QRCode::EM_NUMERIC:
				QRCodeEncoder::encodeText(Utilities::generateRandomNumericString(randomGenerator, dataSizeInBytes), errorCorrectionCapacity, groundtruthCode);
				break;

			case QRCode::EM_ALPHANUMERIC:
				QRCodeEncoder::encodeText(Utilities::generateRandomAlphanumericString(randomGenerator, dataSizeInBytes), errorCorrectionCapacity, groundtruthCode);
				break;

			case QRCode::EM_BYTE:
				Utilities::generateRandomByteData(randomGenerator, dataSizeInBytes, data);
				QRCodeEncoder::encodeBinary(data, errorCorrectionCapacity, groundtruthCode);
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

		// Draw a randomly rotated version of the QR code into the center of the final frame
		const Scalar rotation = Random::scalar(randomGenerator, Scalar(0), Numeric::pi2());
		const Scalar qrcodeCenterToCornerDistancePixels = codeSizeInFramePixelsPerSide * Scalar(0.5) * Numeric::sqrt(Scalar(2));
		const Scalar qrcodeDiagonalOrientation = Numeric::pi_4() + rotation;
		const Scalar maxQRCodeImageDeviationXY = std::max(Numeric::abs(Numeric::cos(qrcodeDiagonalOrientation)), Numeric::abs(Numeric::sin(qrcodeDiagonalOrientation))) * qrcodeCenterToCornerDistancePixels;

		// The QR code is rotated by a random angle and drawn.  Set minimum frame size to be able to accommodate rotated image with room for margin required by detector.
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

			OCEAN_SET_FAILED(detectionValidation);
			OCEAN_SET_FAILED(groundtruthComparisonValidation);
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
			ValidationPrecision::ScopedIteration detectionScopedIteration(detectionValidation);
			ValidationPrecision::ScopedIteration groundtruthComparisonScopedIteration(groundtruthComparisonValidation);

			Worker* workerToUse = useWorker ? &worker : nullptr;

			QRCodeDetector2D::Observations observations;
			const QRCodes codes = QRCodeDetector2D::detectQRCodes(*anyCamera, frame, &observations, workerToUse);

			const bool detectionSuccess = codes.size() == 1 && codes[0].isSame(groundtruthCode, true);
			const bool groundtruthComparisonSuccess = detectionSuccess && codes[0].isSame(groundtruthCode, false);

			if (!detectionSuccess)
			{
				detectionScopedIteration.setInaccurate();
			}

			if (!groundtruthComparisonSuccess)
			{
				groundtruthComparisonScopedIteration.setInaccurate();
			}

#ifdef OCEAN_TEST_QRCODES_DETECTOR2D_ENABLE_VERBOSE_LOGGING
			bool failedAtDetection = !detectionSuccess;
			bool failedAtGroundTruthComparison = detectionSuccess && !groundtruthComparisonSuccess;

			// Differentiate cases of ground truth comparison failure where extracted modules in detected QR code is wrong but otherwise decoded data is correct due to error correction.
			bool failedAtGroundTruthModulesComparison = failedAtGroundTruthComparison && codes[0].isSame(groundtruthCode, true);

			if (failedAtDetection)
			{
				if (codes.size() == 0)
				{
					Log::error() << "Did not detect any QR codes (seed:" << randomGenerator.initialSeed() << ", #" << testImageIndex << ", useWorker:" << useWorker << ")";
				}

				if (codes.size() > 1)
				{
					Log::error() << "Detected " << codes.size() << " QR codes (seed:" << randomGenerator.initialSeed() << ", #" << testImageIndex << ", useWorker:" << useWorker << ")";
				}
			}

			if (failedAtGroundTruthComparison)
			{
				Log::error() << "Detected 1 QR code as expected but decoded value differs from ground truth value " << (failedAtGroundTruthModulesComparison ? "solely due to difference in modules extracted" : "") << " (seed:" << randomGenerator.initialSeed() << ", #" << testImageIndex << ", useWorker:" << useWorker << ")";
			}

			failedDetectionCount += failedAtDetection ? 1 : 0;
			failedGroundTruthComparisonCount += failedAtGroundTruthComparison ? 1 : 0;

			if (failedAtDetection && failedDetectionCount == 1 || failedAtGroundTruthComparison && failedGroundTruthComparisonCount == 1)
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
						filenameprefix << "qrdetect2d_failed_module-comparison_" << testLabel << "_gfs-";
					}
					else
					{
						filenameprefix << "qrdetect2d_failed_comparison_" << testLabel << "_gfs-";
					}
				}
				else
				{
					ocean_assert(failedAtDetection);
					filenameprefix << "qrdetect2d_failed_detect_" << testLabel << "_gfs-";
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
	while (detectionValidation.needMoreIterations() || groundtruthComparisonValidation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << " ";

	Log::info() << "QR Code Detection Validation: " << detectionValidation;
	Log::info() << "Groundtruth Comparison Validation: " << groundtruthComparisonValidation;

	bool success = detectionValidation.succeeded();
	success = groundtruthComparisonValidation.succeeded() && success;

	return success;
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
