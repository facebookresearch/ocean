/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestLegacyQRCodeDetector2D.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/image/Image.h"

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

TestLegacyQRCodeDetector2D::FileDataCollection::FileDataCollection(std::vector<std::string>&& filenames) :
	filenames_(std::move(filenames))
{
	ocean_assert(!filenames_.empty());
}

SharedTestData TestLegacyQRCodeDetector2D::FileDataCollection::data(const size_t index)
{
	ocean_assert(index < filenames_.size());

	if (index >= filenames_.size())
	{
		return nullptr;
	}

	Frame image = IO::Image::readImage(filenames_[index]);

	if (image.isValid())
	{
		if (CV::FrameConverter::Comfort::change(image, FrameType::FORMAT_Y8))
		{
			return std::make_shared<TestData>(std::move(image), Value());
		}
	}

	ocean_assert(false && "Failed to load the image!");
	return nullptr;
}

size_t TestLegacyQRCodeDetector2D::FileDataCollection::size()
{
	return filenames_.size();
}

bool TestLegacyQRCodeDetector2D::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Legacy QR code detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testStressTest(testDuration, worker) && allSucceeded;

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

	const TestDataManager::ScopedSubscriptions scopedSubscriptions = TestLegacyQRCodeDetector2D_registerTestDataCollections();

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodes_0_qrcodes(testDuration, worker, true) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectQRCodes_1_qrcode(testDuration, worker, true) && allSucceeded;

#endif // OCEAN_USE_TEST_DATA_COLLECTION

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

} // namespace TestQRCodes

/**
 * This class implements a simple instance for the GTest ensuring test data collections are registered.
 */
class TestLegacyQRCodeDetector2D : public ::testing::Test
{
	protected:

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_USE_TEST_DATA_COLLECTION
			scopedSubscriptions_ = TestDetector::TestQRCodes::TestLegacyQRCodeDetector2D_registerTestDataCollections();
#endif // OCEAN_USE_TEST_DATA_COLLECTION
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
			scopedSubscriptions_.clear();
		}

	protected:

		/// The subscriptions to all registered data collections.
		TestDataManager::ScopedSubscriptions scopedSubscriptions_;
};

TEST_F(TestLegacyQRCodeDetector2D, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestQRCodes::TestLegacyQRCodeDetector2D::testStressTest(GTEST_TEST_DURATION, worker));
}

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

TEST_F(TestLegacyQRCodeDetector2D, DetectQRCodes_0_qrcodes)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestQRCodes::TestLegacyQRCodeDetector2D::testDetectQRCodes_0_qrcodes(10.0, worker, false));
}

TEST_F(TestLegacyQRCodeDetector2D, DetectQRCodes_1_qrcode)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestQRCodes::TestLegacyQRCodeDetector2D::testDetectQRCodes_1_qrcode(10.0, worker, false));
}

#endif // OCEAN_USE_TEST_DATA_COLLECTION

namespace TestQRCodes
{

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

			const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

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

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	const SharedTestDataCollection dataCollection = TestDataManager::get().testDataCollection("legacyqrCodedetector2d_0code");

	if (dataCollection && dataCollection->size() > 0)
	{
		Indices32 randomIndices = createIndices(dataCollection->size(), 0u);

		std::random_device randomDevice;
		std::mt19937 randomNumberGenerator(randomDevice());
		std::shuffle(randomIndices.begin(), randomIndices.end(), randomNumberGenerator);

		for (const Index32 randomIndex : randomIndices)
		{
			const SharedTestData data = dataCollection->data(size_t(randomIndex));

			if (data && data->dataType() == TestData::DT_IMAGE)
			{
				const Frame yTestImage = data->image();

				ocean_assert(yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8));
				if (yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8))
				{
					for (const bool useWorker : {false, true})
					{
						QRCodes qrcodes;

						if (RandomI::boolean())
						{
							qrcodes = LegacyQRCodeDetector2D::detectQRCodes(yTestImage, useWorker ? &worker : nullptr);
						}
						else
						{
							qrcodes = LegacyQRCodeDetector2D::detectQRCodes(yTestImage.constdata<uint8_t>(), yTestImage.width(), yTestImage.height(), yTestImage.paddingElements(), useWorker ? &worker : nullptr);
						}

						if (qrcodes.size() != 0)
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}

			if (!forceFullTest == false && startTimestamp + testDuration <= Timestamp(true))
			{
				break;
			}
		}
	}
	else
	{
		Log::error() << "Failed to access test data";

		allSucceeded = false;
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

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	const SharedTestDataCollection dataCollection = TestDataManager::get().testDataCollection("legacyqrCodedetector2d_1code");

	if (dataCollection && dataCollection->size() > 0)
	{
		Indices32 randomIndices = createIndices(dataCollection->size(), 0u);

		std::random_device randomDevice;
		std::mt19937 randomNumberGenerator(randomDevice());
		std::shuffle(randomIndices.begin(), randomIndices.end(), randomNumberGenerator);

		for (const Index32 randomIndex : randomIndices)
		{
			const SharedTestData data = dataCollection->data(size_t(randomIndex));

			if (data && data->dataType() == TestData::DT_IMAGE)
			{
				const Frame yTestImage = data->image();

				ocean_assert(yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8));
				if (yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8))
				{
					for (const bool useWorker : {false, true})
					{
						QRCodes qrcodes;

						if (RandomI::boolean())
						{
							qrcodes = LegacyQRCodeDetector2D::detectQRCodes(yTestImage, useWorker ? &worker : nullptr);
						}
						else
						{
							qrcodes = LegacyQRCodeDetector2D::detectQRCodes(yTestImage.constdata<uint8_t>(), yTestImage.width(), yTestImage.height(), yTestImage.paddingElements(), useWorker ? &worker : nullptr);
						}

						if (qrcodes.size() == 1)
						{
							++validIterations;
						}

						++iterations;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}

			if (!forceFullTest == false && startTimestamp + testDuration <= Timestamp(true))
			{
				break;
			}
		}
	}
	else
	{
		Log::error() << "Failed to access test data";

		allSucceeded = false;
	}

	double percent = 0.0;

	if (iterations != 0ull)
	{
		percent = double(validIterations) / double(iterations);
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded, with " << String::toAString(percent, 1u) << "%.";
	}
	else
	{
		Log::info() << "Validation: FAILED, with " << String::toAString(percent, 1u) << "%!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

TestDataManager::ScopedSubscriptions TestLegacyQRCodeDetector2D_registerTestDataCollections()
{
	std::string absolutePath = "";
	ocean_assert(!absolutePath.empty());

	TestDataManager::ScopedSubscriptions scopedSubscriptions;

	{
		std::vector<std::string> filenames0Code =
		{
			absolutePath + "qrcode/detection_0_qrcodes/0001.png",
			absolutePath + "qrcode/detection_0_qrcodes/0002.png",
			absolutePath + "qrcode/detection_0_qrcodes/0003.png",
			absolutePath + "qrcode/detection_0_qrcodes/0004.png",
			absolutePath + "qrcode/detection_0_qrcodes/0005.png",
			absolutePath + "qrcode/detection_0_qrcodes/0006.png",
			absolutePath + "qrcode/detection_0_qrcodes/0007.png",
			absolutePath + "qrcode/detection_0_qrcodes/0008.png",
			absolutePath + "qrcode/detection_0_qrcodes/0009.png",
			absolutePath + "qrcode/detection_0_qrcodes/0010.png",
			absolutePath + "qrcode/detection_0_qrcodes/0011.png",
			absolutePath + "qrcode/detection_0_qrcodes/0012.png",
			absolutePath + "qrcode/detection_0_qrcodes/0013.png",
			absolutePath + "qrcode/detection_0_qrcodes/0014.png",
			absolutePath + "qrcode/detection_0_qrcodes/0015.png",
			absolutePath + "qrcode/detection_0_qrcodes/0016.png",
			absolutePath + "qrcode/detection_0_qrcodes/0017.png",
			absolutePath + "qrcode/detection_0_qrcodes/0018.png",
			absolutePath + "qrcode/detection_0_qrcodes/0019.png",
			absolutePath + "qrcode/detection_0_qrcodes/0020.png",
			absolutePath + "qrcode/detection_0_qrcodes/0021.png",
			absolutePath + "qrcode/detection_0_qrcodes/0022.png",
			absolutePath + "qrcode/detection_0_qrcodes/0023.png",
			absolutePath + "qrcode/detection_0_qrcodes/0024.png",
			absolutePath + "qrcode/detection_0_qrcodes/0025.png",
			absolutePath + "qrcode/detection_0_qrcodes/0026.png",
			absolutePath + "qrcode/detection_0_qrcodes/0027.png",
			absolutePath + "qrcode/detection_0_qrcodes/0028.png"
		};

		scopedSubscriptions.emplace_back(TestDataManager::get().registerTestDataCollection("legacyqrCodedetector2d_0code", std::make_unique<TestLegacyQRCodeDetector2D::FileDataCollection>(std::move(filenames0Code))));
	}

	{
		std::vector<std::string> filenames1Code =
		{
			absolutePath + "qrcode/detection_1_qrcode/0001.png",
			absolutePath + "qrcode/detection_1_qrcode/0002.png",
			absolutePath + "qrcode/detection_1_qrcode/0003.png",
			absolutePath + "qrcode/detection_1_qrcode/0004.png",
			absolutePath + "qrcode/detection_1_qrcode/0005.png",
			absolutePath + "qrcode/detection_1_qrcode/0006.png",
			absolutePath + "qrcode/detection_1_qrcode/0008.png",
			absolutePath + "qrcode/detection_1_qrcode/0010.png",
			absolutePath + "qrcode/detection_1_qrcode/0011.png",
			absolutePath + "qrcode/detection_1_qrcode/0012.png",
			absolutePath + "qrcode/detection_1_qrcode/0013.png",
			absolutePath + "qrcode/detection_1_qrcode/0016.png",
			absolutePath + "qrcode/detection_1_qrcode/0017.png",
			absolutePath + "qrcode/detection_1_qrcode/0018.png",
			absolutePath + "qrcode/detection_1_qrcode/0019.png",
			absolutePath + "qrcode/detection_1_qrcode/0020.png",
			absolutePath + "qrcode/detection_1_qrcode/0021.png",
			absolutePath + "qrcode/detection_1_qrcode/0022.png",
			absolutePath + "qrcode/detection_1_qrcode/0024.png",
			absolutePath + "qrcode/detection_1_qrcode/0025.png",
			absolutePath + "qrcode/detection_1_qrcode/0026.png",
			absolutePath + "qrcode/detection_1_qrcode/0027.png",
			absolutePath + "qrcode/detection_1_qrcode/0028.png",
			absolutePath + "qrcode/detection_1_qrcode/0030.png",
			absolutePath + "qrcode/detection_1_qrcode/0031.png",
			absolutePath + "qrcode/detection_1_qrcode/0035.png",
			absolutePath + "qrcode/detection_1_qrcode/0036.png",
			absolutePath + "qrcode/detection_1_qrcode/0037.png",
			absolutePath + "qrcode/detection_1_qrcode/0038.png",
			absolutePath + "qrcode/detection_1_qrcode/0039.png",
			absolutePath + "qrcode/detection_1_qrcode/0040.png",
			absolutePath + "qrcode/detection_1_qrcode/0041.png",
			absolutePath + "qrcode/detection_1_qrcode/0042.png",
			absolutePath + "qrcode/detection_1_qrcode/0043.png",
			absolutePath + "qrcode/detection_1_qrcode/0044.png",
			absolutePath + "qrcode/detection_1_qrcode/0045.png",
			absolutePath + "qrcode/detection_1_qrcode/0052.png",
			absolutePath + "qrcode/detection_1_qrcode/0053.png",
			absolutePath + "qrcode/detection_1_qrcode/0054.png",
			absolutePath + "qrcode/detection_1_qrcode/0055.png",
			absolutePath + "qrcode/detection_1_qrcode/0056.png",
			absolutePath + "qrcode/detection_1_qrcode/0057.png",
			absolutePath + "qrcode/detection_1_qrcode/0058.png",
			absolutePath + "qrcode/detection_1_qrcode/0059.png",
			absolutePath + "qrcode/detection_1_qrcode/0060.png",
			absolutePath + "qrcode/detection_1_qrcode/0061.png",
			absolutePath + "qrcode/detection_1_qrcode/0062.png",
			absolutePath + "qrcode/detection_1_qrcode/0063.png",
			absolutePath + "qrcode/detection_1_qrcode/0064.png",
			absolutePath + "qrcode/detection_1_qrcode/0065.png",
			absolutePath + "qrcode/detection_1_qrcode/0066.png",
			absolutePath + "qrcode/detection_1_qrcode/0070.png",
			absolutePath + "qrcode/detection_1_qrcode/0071.png",
			absolutePath + "qrcode/detection_1_qrcode/0073.png",
			absolutePath + "qrcode/detection_1_qrcode/0075.png",
			absolutePath + "qrcode/detection_1_qrcode/0080.png",
			absolutePath + "qrcode/detection_1_qrcode/0081.png",
			absolutePath + "qrcode/detection_1_qrcode/0082.png",
			absolutePath + "qrcode/detection_1_qrcode/0084.png",
			absolutePath + "qrcode/detection_1_qrcode/0087.png",
			absolutePath + "qrcode/detection_1_qrcode/0088.png",
			absolutePath + "qrcode/detection_1_qrcode/0089.png",
			absolutePath + "qrcode/detection_1_qrcode/0090.png",
			absolutePath + "qrcode/detection_1_qrcode/0091.png",
			absolutePath + "qrcode/detection_1_qrcode/0094.png",
			absolutePath + "qrcode/detection_1_qrcode/0095.png",
			absolutePath + "qrcode/detection_1_qrcode/0096.png",
			absolutePath + "qrcode/detection_1_qrcode/0097.png",
			absolutePath + "qrcode/detection_1_qrcode/0103.png",
			absolutePath + "qrcode/detection_1_qrcode/0104.png",
			absolutePath + "qrcode/detection_1_qrcode/0105.png",
			absolutePath + "qrcode/detection_1_qrcode/0106.png",
			absolutePath + "qrcode/detection_1_qrcode/0107.png",
			absolutePath + "qrcode/detection_1_qrcode/0108.png",
			absolutePath + "qrcode/detection_1_qrcode/0109.png",
			absolutePath + "qrcode/detection_1_qrcode/0110.png",
			absolutePath + "qrcode/detection_1_qrcode/0111.png",
			absolutePath + "qrcode/detection_1_qrcode/0112.png",
			absolutePath + "qrcode/detection_1_qrcode/0113.png",
			absolutePath + "qrcode/detection_1_qrcode/0114.png",
			absolutePath + "qrcode/detection_1_qrcode/0115.png",
			absolutePath + "qrcode/detection_1_qrcode/0116.png",
			absolutePath + "qrcode/detection_1_qrcode/0117.png",
			absolutePath + "qrcode/detection_1_qrcode/0118.png",
			absolutePath + "qrcode/detection_1_qrcode/0119.png",
			absolutePath + "qrcode/detection_1_qrcode/0120.png",
			absolutePath + "qrcode/detection_1_qrcode/0121.png",
			absolutePath + "qrcode/detection_1_qrcode/0124.png",
			absolutePath + "qrcode/detection_1_qrcode/0125.png",
			absolutePath + "qrcode/detection_1_qrcode/0126.png",
			absolutePath + "qrcode/detection_1_qrcode/0128.png",
			absolutePath + "qrcode/detection_1_qrcode/0129.png",
			absolutePath + "qrcode/detection_1_qrcode/0131.png",
			absolutePath + "qrcode/detection_1_qrcode/0136.png",
			absolutePath + "qrcode/detection_1_qrcode/0137.png",
			absolutePath + "qrcode/detection_1_qrcode/0143.png",
			absolutePath + "qrcode/detection_1_qrcode/0144.png",
			absolutePath + "qrcode/detection_1_qrcode/0145.png",
			absolutePath + "qrcode/detection_1_qrcode/0146.png",
			absolutePath + "qrcode/detection_1_qrcode/0148.png",
			absolutePath + "qrcode/detection_1_qrcode/0149.png",
			absolutePath + "qrcode/detection_1_qrcode/0150.png",
			absolutePath + "qrcode/detection_1_qrcode/0151.png",
			absolutePath + "qrcode/detection_1_qrcode/0152.png",
			absolutePath + "qrcode/detection_1_qrcode/0154.png",
			absolutePath + "qrcode/detection_1_qrcode/0157.png",
			absolutePath + "qrcode/detection_1_qrcode/0159.png",
			absolutePath + "qrcode/detection_1_qrcode/0162.png"
		};

		scopedSubscriptions.emplace_back(TestDataManager::get().registerTestDataCollection("legacyqrCodedetector2d_1code", std::make_unique<TestLegacyQRCodeDetector2D::FileDataCollection>(std::move(filenames1Code))));
	}

	return scopedSubscriptions;
}

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
