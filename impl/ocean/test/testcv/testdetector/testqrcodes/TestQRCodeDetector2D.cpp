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

	allSucceeded = testDetectQRCodesSyntheticData(0u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSyntheticData(1u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSyntheticData(3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSyntheticData(5u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodesSyntheticData(7u, testDuration, worker) && allSucceeded;

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

TEST(TestQRCodeDetector2D, TestDetectQRCodesSyntheticDataNoGaussianFilter)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSyntheticData(0u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSyntheticDataGaussianFilter1)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSyntheticData(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSyntheticDataGaussianFilter3)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSyntheticData(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSyntheticDataGaussianFilter5)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

TEST(TestQRCodeDetector2D, TestDetectQRCodesSyntheticDataGaussianFilter7)
{
	Worker worker;
	EXPECT_TRUE(TestQRCodeDetector2D::testDetectQRCodesSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

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
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Stress test: " << validation;

	return validation.succeeded();
}

bool TestQRCodeDetector2D::testDetectQRCodesSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker)
{
	ocean_assert(gaussianFilterSize == 0u || gaussianFilterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Detect QR codes test using synthetic data (" << (gaussianFilterSize == 0u ? "no Gaussian filter" : "Gaussian filter: " + String::toAString(gaussianFilterSize)) + ")";

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.90;

	ValidationPrecision validation(threshold, randomGenerator);

	Timestamp start(true);

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

		const unsigned int frameWithCodeSize = RandomI::random(randomGenerator, 6u * groundtruthCode.modulesPerSide(), 8u * groundtruthCode.modulesPerSide()); // ~6-8 pixels per module

		const Frame frameWithCode = CV::Detector::QRCodes::Utilities::draw(groundtruthCode, frameWithCodeSize, /* allowTrueMultiple */ true, /* border */ 4u, &worker, foregroundValue, backgroundValue);
		ocean_assert(frameWithCode.isValid() && frameWithCode.width() >= frameWithCodeSize && frameWithCode.height() >= frameWithCodeSize);

		// Draw a randomly rotated version of the QR code into the center of the final frame
		const unsigned int frameWidth = 2u * frameWithCode.width();
		const unsigned int frameHeight = 2u * frameWithCode.height();

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(frameWidth, frameHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		frame.setValue(backgroundValue);

		const Vector2 frameCenterOffset(Scalar(frameWidth) * Scalar(0.5), Scalar(frameHeight) * Scalar(0.5));
		const Scalar rotation = Random::scalar(randomGenerator, Scalar(0), Numeric::pi2());

		const SquareMatrix3 frameWithCodeCenteredRotated_T_frame = SquareMatrix3(Vector3(Scalar(1), Scalar(0), Scalar(0)), Vector3(Scalar(0), Scalar(1), Scalar(0)), Vector3(-frameCenterOffset, Scalar(1)));
		const SquareMatrix3 frameWithCodeCentered_R_frameWithCodeCenteredRotated = SquareMatrix3(Rotation(Vector3(Scalar(0), Scalar(0), Scalar(1)), rotation));
		const SquareMatrix3 frameWithCode_T_frameWithCodeCentered = SquareMatrix3(Vector3(Scalar(1), Scalar(0), Scalar(0)), Vector3(Scalar(0), Scalar(1), Scalar(0)), Vector3(Vector2(Scalar(frameWithCode.width() / 2u), Scalar(frameWithCode.height() / 2u)), Scalar(1)));
		const SquareMatrix3 frameWithCode_T_frame = frameWithCode_T_frameWithCodeCentered * frameWithCodeCentered_R_frameWithCodeCenteredRotated * frameWithCodeCenteredRotated_T_frame;

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

		for (const bool useWorker : {true, false})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			Worker* workerToUse = useWorker ? &worker : nullptr;

			QRCodeDetector2D::Observations observations;
			const QRCodes codes = QRCodeDetector2D::detectQRCodes(*anyCamera, frame, &observations, workerToUse);

			if (codes.size() != 1 || groundtruthCode != codes[0])
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
