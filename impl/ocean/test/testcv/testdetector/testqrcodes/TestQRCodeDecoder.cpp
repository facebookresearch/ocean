/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestQRCodeDecoder.h"

#include "ocean/test/testcv/testdetector/testqrcodes/TestQRCodeEncoder.h"
#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeDecoder.h"

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

bool TestQRCodeDecoder::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for QR code decoding:   ---";
	Log::info() << " ";

#ifdef OCEAN_USE_TEST_DATA_COLLECTION
	const TestDataManager::ScopedSubscription scopedSubscription = TestQRCodeEncoder_registerTestDataCollection();
#endif

	bool allSucceeded = true;

	allSucceeded = testQRCodeDecoding(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "QR code decoding test succeeded.";
	}
	else
	{
		Log::info() << "QR code decoding test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestQRCodes

/**
 * This class implements a simple instance for the GTest ensuring test data collections are registered.
 */
class TestQRCodeDecoder : public ::testing::Test
{
	protected:

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_USE_TEST_DATA_COLLECTION
			scopedSubscription_ = TestDetector::TestQRCodes::TestQRCodeEncoder_registerTestDataCollection();
#endif // OCEAN_USE_TEST_DATA_COLLECTION
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
			scopedSubscription_.release();
		}

	protected:

		/// The subscriptions to all registered data collections.
		TestDataManager::ScopedSubscription scopedSubscription_;
};

TEST_F(TestQRCodeDecoder, QRCodeDecoding)
{
	EXPECT_TRUE(TestDetector::TestQRCodes::TestQRCodeDecoder::testQRCodeDecoding(GTEST_TEST_DURATION));
}

namespace TestQRCodes
{

#endif // OCEAN_USE_GTEST

bool TestQRCodeDecoder::testQRCodeDecoding(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "QR code decoding test:";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	// Testing

	{
		Timestamp start(true);

		constexpr QRCode::EncodingMode encodingModes[3] =
		{
			QRCode::EM_NUMERIC,
			QRCode::EM_ALPHANUMERIC,
			QRCode::EM_BYTE
		};

		do
		{
			const QRCode::EncodingMode encodationMode = encodingModes[RandomI::random(randomGenerator, 2u)];
			const unsigned int messageLength = RandomI::random(randomGenerator, 1u, 1024u);
			std::vector<uint8_t> message;

			QRCode code;
			bool qrcodeGenerated = false;
			switch (encodationMode)
			{
				case QRCode::EM_NUMERIC:
					qrcodeGenerated = QRCodeEncoder::encodeText(Utilities::generateRandomNumericString(randomGenerator, messageLength), QRCode::ECC_07, code) && allSucceeded;
					break;

				case QRCode::EM_ALPHANUMERIC:
					qrcodeGenerated = QRCodeEncoder::encodeText(Utilities::generateRandomAlphanumericString(randomGenerator, messageLength), QRCode::ECC_07, code) && allSucceeded;
					break;

				case QRCode::EM_BYTE:
					allSucceeded = Utilities::generateRandomByteData(randomGenerator, messageLength, message) && allSucceeded;
					qrcodeGenerated = QRCodeEncoder::encodeBinary(message, QRCode::ECC_07, code) && allSucceeded;
					break;

				default:
					ocean_assert(false && "Never be here");
					allSucceeded = false;
					break;
			}

			ocean_assert_and_suppress_unused(qrcodeGenerated, qrcodeGenerated);

			QRCode testCode;
			if (!QRCodeDecoder::decodeQRCode(code.modules(), testCode))
			{
				allSucceeded = false;
			}

			allSucceeded = code == testCode;
		}
		while (Timestamp(true) < start + testDuration);
	}

	// Validation - reuse verification data of the encoding test for QR codes

	{
		const TestQRCodeEncoder::QRCodeVerificationItems verificationItems = TestQRCodeEncoder::loadDataTestQRCodeEncoding();

		if (verificationItems.empty())
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		Timestamp start(true);

		do
		{
			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(verificationItems.size() - 1));
			const TestQRCodeEncoder::QRCodeVerificationItem& verificationItem = verificationItems[randomIndex];

			std::vector<uint8_t> modules(verificationItem.modules_.size());

			for (size_t i = 0; i < verificationItem.modules_.size(); ++i)
			{
				modules[i] = verificationItem.modules_[i] == '1' ? 1u : 0u;
			}

			QRCode testCode;
			if (!QRCodeDecoder::decodeQRCode(modules, testCode) || !testCode.isValid())
			{
				allSucceeded = false;
			}

			allSucceeded = testCode.version() == verificationItem.version_
				&& testCode.errorCorrectionCapacity() == verificationItem.errorCorrectionCapacity_
				&& allSucceeded;

			const std::vector<uint8_t>& testData = testCode.data();
			const std::string testDataString(testData.begin(), testData.end());

			if (testDataString != verificationItem.message_)
			{
				allSucceeded = false;
			}
		}
		while (Timestamp(true) < start + testDuration);
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
		Log::info() << "Random generator seed: " << randomGenerator.seed();
	}

	return allSucceeded;
}


} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
