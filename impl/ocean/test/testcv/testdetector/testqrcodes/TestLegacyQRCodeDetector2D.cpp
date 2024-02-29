// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testdetector/testqrcodes/TestLegacyQRCodeDetector2D.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"

#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"
#include <cstdint>

#ifdef __APPLE__
	#include "ocean/media/imageio/Image.h"
#else
	#include "ocean/media/openimagelibraries/Image.h"
#endif

#ifdef OCEAN_ENABLED_EVERSTORE_CLIENT
	#include "metaonly/ocean/network/everstore/EverstoreClient.h"
#endif // OCEAN_ENABLED_EVERSTORE_CLIENT

#include <random>

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

// #define APPLY_IMAGE_TEST

bool TestLegacyQRCodeDetector2D::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Legacy QR code detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testStressTest(testDuration, worker) && allSucceeded;

#if defined(OCEAN_ENABLED_EVERSTORE_CLIENT) || defined(APPLY_IMAGE_TEST)

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodes_0_qrcodes(testDuration, worker, true) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodes_1_qrcode(testDuration, worker, true) && allSucceeded;

#endif

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Legacy QR code detector test succeeded.";
	}
	else
	{
		Log::info() << "Legacy QR code detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCVDetectorQRCodesQRCodeDetector2D, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestLegacyQRCodeDetector2D::testStressTest(GTEST_TEST_DURATION, worker));
}

#if defined(APPLY_IMAGE_TEST) || defined(OCEAN_ENABLED_EVERSTORE_CLIENT)

TEST(TestCVDetectorQRCodesQRCodeDetector2D, DetectQRCodes_0_qrcodes)
{
	Worker worker;
	EXPECT_TRUE(TestLegacyQRCodeDetector2D::testDetectQRCodes_0_qrcodes(10.0, worker, false));
}

TEST(TestCVDetectorQRCodesQRCodeDetector2D, DetectQRCodes_1_qrcode)
{
	Worker worker;
	EXPECT_TRUE(TestLegacyQRCodeDetector2D::testDetectQRCodes_1_qrcode(10.0, worker, false));
}

#endif // defined(APPLY_IMAGE_TEST) || defined(OCEAN_ENABLED_EVERSTORE_CLIENT)

#endif // OCEAN_USE_GTEST

bool TestLegacyQRCodeDetector2D::testStressTest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool useWorker : {false, true})
		{
			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1920u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);

			const QRCodes qrCodes = LegacyQRCodeDetector2D::detectQRCodes(yFrame, useWorker ? &worker : nullptr);

			if (qrCodes.size() >= size_t(width * height))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestLegacyQRCodeDetector2D::testDetectQRCodes_0_qrcodes(const double testDuration, Worker& worker, const bool forceFullTest)
{
	Log::info() << "Detection of QR codes (on images with 0 QR codes):";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const HandlePairs handlePairs = getHandlePairs_0_qrcodes();

	Indices32 randomHandleIndices(handlePairs.size());
	for (size_t i = 0; i < handlePairs.size(); ++i)
	{
		randomHandleIndices[i] = (unsigned int)i;
	}

	std::random_device randomDevice;
	std::mt19937 randomNumberGenerator(randomDevice());
	std::shuffle(randomHandleIndices.begin(), randomHandleIndices.end(), randomNumberGenerator);

	const Timestamp startTimestamp(true);

	for (Index32 i : randomHandleIndices)
	{
		const HandlePair& handlePair = handlePairs[i];

		const Frame testImageY8 = loadTestImage(handlePair);
		ocean_assert(testImageY8.pixelFormat() == FrameType::FORMAT_Y8);

		if (testImageY8.isValid() == false)
		{
			Log::info() << "Failed to download image: " << handlePair.first;

			allSucceeded = false;
			continue;
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			const bool useFrameInterface = RandomI::random(1u) == 1u;

			QRCodes qrcodes;

			if (useFrameInterface)
			{
				qrcodes = LegacyQRCodeDetector2D::detectQRCodes(testImageY8, useWorker);
			}
			else
			{
				qrcodes = LegacyQRCodeDetector2D::detectQRCodes(testImageY8.constdata<uint8_t>(), testImageY8.width(), testImageY8.height(), testImageY8.paddingElements(), useWorker);
			}

			if (qrcodes.size() == 0)
			{
				++validIterations;
			}

			++iterations;
		}

		if (forceFullTest == false && iterations != 0ull && startTimestamp + testDuration < Timestamp(true))
		{
			break;
		}
	}

	ocean_assert(allSucceeded == false || iterations != 0ull);
	if (iterations != 0u)
	{
		if (validIterations != iterations)
		{
			const double percent = double(validIterations) / double(iterations);
			Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";
			allSucceeded = false;
		}
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

bool TestLegacyQRCodeDetector2D::testDetectQRCodes_1_qrcode(const double testDuration, Worker& worker, const bool forceFullTest)
{
	Log::info() << "Detection of QR codes (on images with 1 QR code):";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const HandlePairs handlePairs1Code = getHandlePairs_1_qrcode();

	Indices32 randomHandleIndices(handlePairs1Code.size());
	for (size_t i = 0; i < handlePairs1Code.size(); ++i)
	{
		randomHandleIndices[i] = (unsigned int)i;
	}

	std::random_device randomDevice;
	std::mt19937 randomNumberGenerator(randomDevice());
	std::shuffle(randomHandleIndices.begin(), randomHandleIndices.end(), randomNumberGenerator);

	const Timestamp startTimestamp(true);

	for (Index32 i : randomHandleIndices)
	{
		const HandlePair& handlePair = handlePairs1Code[i];

		const Frame testImageY8 = loadTestImage(handlePair);
		ocean_assert(testImageY8.pixelFormat() == FrameType::FORMAT_Y8);

		if (testImageY8.isValid() == false)
		{
			Log::info() << "Failed to download image: " << handlePair.first;

			allSucceeded = false;
			continue;
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			const bool useFrameInterface = RandomI::random(1u) == 1u;

			QRCodes qrcodes;

			if (useFrameInterface)
			{
				qrcodes = LegacyQRCodeDetector2D::detectQRCodes(testImageY8, useWorker);
			}
			else
			{
				qrcodes = LegacyQRCodeDetector2D::detectQRCodes(testImageY8.constdata<uint8_t>(), testImageY8.width(), testImageY8.height(), testImageY8.paddingElements(), useWorker);
			}

			if (qrcodes.size() == 1)
			{
				++validIterations;
			}

			++iterations;
		}

		if (forceFullTest == false && iterations != 0ull && startTimestamp + testDuration < Timestamp(true))
		{
			break;
		}
	}

	ocean_assert(allSucceeded == false || iterations != 0ull);
	if (iterations != 0u)
	{
		const double percent = double(validIterations) / double(iterations);

		if (percent < 0.95)
		{
			Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";
			allSucceeded = false;
		}
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

Frame TestLegacyQRCodeDetector2D::loadTestImage(const HandlePair& handlePair)
{
#if defined(OCEAN_ENABLED_EVERSTORE_CLIENT)

	Network::EverstoreClient::Buffer buffer;
	if (!Network::EverstoreClient::download(handlePair.second, buffer))
	{
		ocean_assert(false && "Failed to download image from everstore!");
		return Frame();
	}

	const Frame image = Media::OpenImageLibraries::Image::decodeImage(buffer.data(), buffer.size());

#else

#ifdef __APPLE__
	const Frame image = Media::ImageIO::Image::readImage(handlePair.first);
#else
	const Frame image = Media::OpenImageLibraries::Image::readImage(handlePair.first);
#endif

#endif

	if (!image.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	Frame imageY8;
	if (!CV::FrameConverter::Comfort::convert(image, FrameType(image, FrameType::FORMAT_Y8), imageY8, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		Frame();
	}

	return imageY8;
}

std::string TestLegacyQRCodeDetector2D::testImagesDirectory()
{
	// Note: When `APPLY_IMAGE_TEST` if defined, set to this to the absolute path of your copy of https://fburl.com/ocean_everstore_test_data
	const std::string absolutePath = "";

#if defined(APPLY_IMAGE_TEST)

#ifdef OCEAN_ENABLED_EVERSTORE_CLIENT
	#error Either Everstore or local images.
#endif

	ocean_assert(absolutePath.empty() == false && "Define a valid absolute path");

#endif

	return absolutePath;
}

TestLegacyQRCodeDetector2D::HandlePairs TestLegacyQRCodeDetector2D::getHandlePairs_0_qrcodes()
{
	const std::string absolutePath = testImagesDirectory();

	const HandlePairs handlePairs =
	{
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0001.png", std::string("GICWmAA2SQNNsigBAJmD_x_mwHpYbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0002.png", std::string("GICWmAA_rCiE0xwCALwKXimHHFtrbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0003.png", std::string("GICWmAAHTSC37RgIAPzd3MrwJL13buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0004.png", std::string("GICWmAD69MNz1nsBAPjeBWKznn9gbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0005.png", std::string("GICWmADd-PB_t0oEABka923tngN0buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0006.png", std::string("GICWmACdaugOs3MFABFWA7YqBvFibuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0007.png", std::string("GICWmABrenKraKQIAHHUjHr0OfEQbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0008.png", std::string("GICWmACwRK9bHTIBAAYlS3YFkRMVbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0009.png", std::string("GICWmAAdUpeGfpIAAIbWXIZxtY1YbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0010.png", std::string("GICWmADYdUQh-oUAABetgtNmw_hibuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0011.png", std::string("GICWmADrYDkLi34AAK8JYp31jhUtbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0012.png", std::string("GICWmACENyskkDQBAPY0ZdTMKo0bbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0013.png", std::string("GICWmAA7d6Cz3y0JAIkSqncnL-MbbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0014.png", std::string("GDUUngM31zxDlbYIAMn6l6dph8k4buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0015.png", std::string("GNmyoAPrLgseQIQIACF6sG-YvRl6buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0016.png", std::string("GCHGnAOp5ktwLkUBAFpgb6WYa_89buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0017.png", std::string("GGtpnQPqMXKxW0YCAKmb8QMglXF3buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0018.png", std::string("GICWmACd7qHaChMEAGgIpIubDZkhbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0019.png", std::string("GICWmACM_aLZ4S8CAJ_lWIEPWv8xbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0020.png", std::string("GICWmAA_BPphuisBANIpOc2xhxwibuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0021.png", std::string("GICWmACe8HvSoEEBADqBb9Cav4NibuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0022.png", std::string("GICWmAD19wO5axoCAEnoSz-ZQjZ2buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0023.png", std::string("GICWmADg8-eIl4MBAJw6PrCwv1ErbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0024.png", std::string("GICWmABxQ7IDR4MBADq3yFcOZkFrbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0025.png", std::string("GICWmADRVU5xL4cBAEc2s-SpMIZSbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0026.png", std::string("GICWmADx5TYJRgYBAEdjf54KFChCbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0027.png", std::string("GICWmABDfKZvUH0AALvKnSjXcT5JbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_0_qrcodes/0028.png", std::string("GICWmAB8F3OT9qEDAMwFCpbhMu0ybuZcAAAD")),
    };

	return handlePairs;
}

TestLegacyQRCodeDetector2D::HandlePairs TestLegacyQRCodeDetector2D::getHandlePairs_1_qrcode()
{
	const std::string absolutePath = testImagesDirectory();

	const HandlePairs handlePairs =
	{
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0001.png", std::string("GICWmABUPDVzHJgHAEA66TqfHg0pbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0002.png", std::string("GICWmACjuQPcG4AAAH3Da_TQYNo-buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0003.png", std::string("GICWmAAvoUDvcrIJALxCDHWEU-FubuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0004.png", std::string("GICWmAAATDy7aREDAO8gqHgg9JURbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0005.png", std::string("GICWmACefYWYnyYIAMc5jWTp1OkebuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0006.png", std::string("GICWmADRuyUsNnwAAFU8ff1kCC0FbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0008.png", std::string("GICWmAB40Jstn7MHADOxNZMTagBubuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0010.png", std::string("GICWmAD5Jv-jH3oAAPLcePpfRFYdbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0011.png", std::string("GICWmACzxh6ibTwBAN3PxF1q5qsTbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0012.png", std::string("GICWmACM-MH1340BAJFFRrGuj_UmbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0013.png", std::string("GICWmACrqS_TQGgIAIA5-e-iOjcBbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0016.png", std::string("GICWmADzD36_p34AABBf5jkOboogbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0017.png", std::string("GICWmABpiImHdGkBAC3sp9OSM5NlbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0018.png", std::string("GICWmADuH7rOUdMCABElYkyf4WlubuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0019.png", std::string("GICWmAD9tqRKC2QCAB31DfE3FPYcbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0020.png", std::string("GICWmABdJqpeQxgBALphb9MMq7FJbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0021.png", std::string("GICWmAArYPORlzkIANYOsh4vap97buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0022.png", std::string("GICWmACAEULSrYEAAMwepfp4oPssbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0024.png", std::string("GICWmAAOlu4KHV0BAHEljsKEFnltbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0025.png", std::string("GICWmADNtVJJ_j0IAL0FcTAGnzRGbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0026.png", std::string("GICWmACBIE_NN18BANgE5pcFzkNFbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0027.png", std::string("GICWmAA8OXBSQSwBADrvs71rMTtWbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0028.png", std::string("GICWmAD_qZm6AgMDAOw7coVtuV9rbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0030.png", std::string("GICWmAAyXCliZ1MHAP77ltge5OlKbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0031.png", std::string("GICWmAAKs3xehaQAAGNNjUt91lQabuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0035.png", std::string("GICWmABeAu-ONoIBAB4zuBVoAdQNbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0036.png", std::string("GICWmABaEJasZ0oBABk8mUBZepYBbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0037.png", std::string("GICWmAB9VjUXnrwAABKsuvzVFegybuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0038.png", std::string("GICWmAA3P1_25WYAAJjh4BeliONjbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0039.png", std::string("GICWmADwqvGRlZQBAI6KTYXgg8NjbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0040.png", std::string("GICWmAC6BLR6a5QCACzQ-TAxTP44buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0041.png", std::string("GICWmACnTqPfQRABAMG_0Awh2GUtbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0042.png", std::string("GICWmAAUtCVX4yYBACugNCKWBn9FbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0043.png", std::string("GICWmAAPC2sAKMQJALiRPzfzrSENbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0044.png", std::string("GICWmAB35ebuc_UCAIBJK1xQ98gDbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0045.png", std::string("GICWmADdTqAS5G8IAFZcGd69zFcYbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0052.png", std::string("GICWmAC1-iPRISwBAKuKsh_Ia8E9buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0053.png", std::string("GICWmAC5i7YaS4wCAGnsnn2-dlgTbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0054.png", std::string("GICWmAAHUoobVKQAABGwvbOi3EtmbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0055.png", std::string("GICWmABHWR9IHmABAIzPoHNkeyQvbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0056.png", std::string("GICWmAAfoYvujqsIAJEzu7EiMgZZbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0057.png", std::string("GICWmAAxb-kZfvMCAE6fz_JvuQRQbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0058.png", std::string("GICWmAA0cIRaZWMBAK5_E_bNK6FsbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0059.png", std::string("GICWmABSXaq5mpcBANQKrpjPw59DbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0060.png", std::string("GICWmAD3Adbye7AAAJ1iiT_vU78UbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0061.png", std::string("GICWmABoDSa4t-EEAPs4KjiAxG5ZbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0062.png", std::string("GICWmACTL2iSZBMBAJSXnlcYIjp5buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0063.png", std::string("GICWmABA68hFejYCALKzdh2IzCFJbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0064.png", std::string("GICWmACO_T2pbkUBAImFEGleq4lRbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0065.png", std::string("GICWmAAP7eRekjACAK0xM7tVOpxkbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0066.png", std::string("GICWmADxiq47nEcJAOeR-kJx9u4ybuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0070.png", std::string("GICWmAAB0ZlbtP0CABy0EgwFPnl2buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0071.png", std::string("GICWmAB5j8wwJi8BAIMwq0DiJEZpbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0073.png", std::string("GICWmAB0q55s4ocJAFtq8MgQbbtXbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0075.png", std::string("GICWmAAdpMvFjEoBABF9L6Pz3T5mbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0080.png", std::string("GICWmAAcu3YDGwQBAClffBy3DNVNbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0081.png", std::string("GICWmAD6wbO9XjEIAJLEZlRfdi4jbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0082.png", std::string("GICWmAD6y3ZHBCQIALP192JBr3xZbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0084.png", std::string("GICWmAA3g8s_qZoCAISQBJgaJ3JcbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0087.png", std::string("GICWmADR0TT1kjcIAOMUyGcxlw0BbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0088.png", std::string("GICWmAB8LTIZ40wBAB417f5-0T52buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0089.png", std::string("GICWmAC3v0oaA7sAAD35ZIHysihwbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0090.png", std::string("GICWmABlq8hkzf4CAPHBnkpJnmVFbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0091.png", std::string("GICWmADrLw--EX4BAMvML9f1V7VobuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0094.png", std::string("GICWmABy-on0UlIBALunDV9G1VcfbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0095.png", std::string("GICWmACHefu_NWUHAHR5gkwZG4xvbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0096.png", std::string("GICWmAC75u5p9_YCAH9x_W6GcO8ubuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0097.png", std::string("GBawmgNkGsqmcZ8BALnG3N6ZUfxobuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0103.png", std::string("GICWmACvkGfXFlMCALFUobv6h500buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0104.png", std::string("GICWmADnFupIof4IAL5q87g6ickQbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0105.png", std::string("GICWmADEkU-HkxcCANwYi8sQ1XpTbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0106.png", std::string("GICWmACGValgsUMCADbdgzdS8igZbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0107.png", std::string("GICWmACpxcimHzoHAIv1AAHWRwVtbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0108.png", std::string("GICWmACNRzgfA-gHAPvRguBj1BV0buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0109.png", std::string("GICWmADhZqggwm8AAN4lqETmKXIAbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0110.png", std::string("GICWmAA7_aedMiECAGo-xmbxne40buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0111.png", std::string("GICWmABGITbqBXkIAPLo-5KUBBoJbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0112.png", std::string("GICWmAA2_s7t43gBAEIhJuTxB505buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0113.png", std::string("GICWmAC8fUGP2GsBANX5P2qIwmIabuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0114.png", std::string("GICWmAAn2xfrcn0AAOcEgTjeITMEbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0115.png", std::string("GICWmADspLlEgjQBAATUnK54newQbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0116.png", std::string("GICWmABvHZbIAmUCABWUvSsK8xFnbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0117.png", std::string("GICWmACPsLjxknsAAMmCVkk9LytIbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0118.png", std::string("GICWmACDJvKJIG0HAA5Xdi0MP0QEbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0119.png", std::string("GICWmACJqxAg1eQDAEiLCgJuAMhXbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0120.png", std::string("GICWmADkMs0tyW4IAD9NNn4denNabuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0121.png", std::string("GICWmADbbcq0mmoBABK2ZqC36HAibuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0124.png", std::string("GICWmABSQ3bWjIEAAPLKJcGcxKJ0buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0125.png", std::string("GICWmADaYDy3SSADAPuylCtn-v4JbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0126.png", std::string("GICWmADHD4D4MagIAE3NAwi0UkdFbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0128.png", std::string("GICWmADGUNqaYp0AALCpmLr_ytV-buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0129.png", std::string("GICWmAA3dnJB1QEIAMX59J7lLuMRbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0131.png", std::string("GICWmAAPpSWlmaIDAAUsv-_QnVV3buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0136.png", std::string("GICWmABZZxhY84IAAOyZiv8dyEZxbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0137.png", std::string("GICWmABXVI_jy04IALYoiPL3GF9wbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0143.png", std::string("GICWmACue3wsrE8IAAoQVmwziU11buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0144.png", std::string("GICWmABy_OawZLYAAHGhVOpVjhl2buZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0145.png", std::string("GICWmABD_9KV_mcAAPON1a5OhrJTbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0146.png", std::string("GICWmABn8OgH3n4AAJ-vZ0jsMhRTbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0148.png", std::string("GICWmAD1jOcjPkwBAB1Rn1q42usNbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0149.png", std::string("GICWmAC6UUAgJJsBAHWSLA6QIjhFbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0150.png", std::string("GICWmAB-nxm1iTEEAKXMr5SXF6VLbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0151.png", std::string("GICWmAAaNM7TVTABAIAiFLAiYOpJbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0152.png", std::string("GICWmACHaCrebtcEALpK32hqNCEQbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0154.png", std::string("GICWmACSXF5sU0sEAIZK9l9NdIdIbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0157.png", std::string("GICWmAA492ewl_EEANQHqFGieFBFbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0159.png", std::string("GACwnAOw2CGHw6UBAAnRpuSmgHxQbuZcAAAD")),
		std::make_pair(absolutePath + "qrcode/detection_1_qrcode/0162.png", std::string("GLY69wP-gbCVHlUKABf_H9P2-80HbuZcAAAD")), // T46956176 ECI mode was disabled
	};

	return handlePairs;
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namepace Test
