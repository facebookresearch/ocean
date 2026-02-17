/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestMicroQRCodeDecoder.h"

#include "ocean/test/testcv/testdetector/testqrcodes/TestMicroQRCodeEncoder.h"
#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomI.h"

#include "ocean/test/Validation.h"

#include "ocean/cv/detector/qrcodes/MicroQRCode.h"
#include "ocean/cv/detector/qrcodes/MicroQRCodeDecoder.h"

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

bool TestMicroQRCodeDecoder::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for Micro QR code decoding:   ---";
	Log::info() << " ";

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
	const TestDataManager::ScopedSubscription scopedSubscription = TestMicroQRCodeEncoder_registerTestDataCollection();
#endif

	bool allSucceeded = true;

	allSucceeded = testMicroQRCodeDecoding(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Micro QR code decoding test succeeded.";
	}
	else
	{
		Log::info() << "Micro QR code decoding test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestQRCodes

/**
 * This class implements a simple instance for the GTest ensuring test data collections are registered.
 */
class TestMicroQRCodeDecoder : public ::testing::Test
{
	protected:

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
			scopedSubscription_ = TestDetector::TestQRCodes::TestMicroQRCodeEncoder_registerTestDataCollection();
#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
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

TEST_F(TestMicroQRCodeDecoder, MicroQRCodeDecoding)
{
	EXPECT_TRUE(TestDetector::TestQRCodes::TestMicroQRCodeDecoder::testMicroQRCodeDecoding(GTEST_TEST_DURATION));
}

namespace TestQRCodes
{

#endif // OCEAN_USE_GTEST

bool TestMicroQRCodeDecoder::testMicroQRCodeDecoding(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Micro QR code decoding test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// Testing

	{
		Timestamp start(true);

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
			const EncodingParameters& params = encodingParameters[RandomI::random(randomGenerator, (unsigned int)(encodingParameters.size() - 1))];

			std::vector<uint8_t> message;
			std::string messageString;

			unsigned int messageLength = RandomI::random(randomGenerator, params.minSize, params.maxSize);

			MicroQRCode code;
			bool qrcodeGenerated = false;
			switch (params.mode)
			{
				case MicroQRCode::EM_NUMERIC:
					messageString = Utilities::generateRandomNumericString(randomGenerator, messageLength);
					qrcodeGenerated = (MicroQRCodeEncoder::encodeText(messageString, params.eccMin, code) == MicroQRCodeEncoder::SC_SUCCESS) && code.isValid();
					break;

				case MicroQRCode::EM_ALPHANUMERIC:
					do
					{
						messageString = Utilities::generateRandomAlphanumericString(randomGenerator, messageLength);
					}
					while (QRCodeEncoderBase::Segment::isNumericData(messageString));
					qrcodeGenerated = (MicroQRCodeEncoder::encodeText(messageString, params.eccMin, code) == MicroQRCodeEncoder::SC_SUCCESS) && code.isValid();
					break;

				case MicroQRCode::EM_BYTE:
					OCEAN_EXPECT_TRUE(validation, Utilities::generateRandomByteData(randomGenerator, messageLength, message));
					qrcodeGenerated = (MicroQRCodeEncoder::encodeBinary(message, params.eccMin, code) == MicroQRCodeEncoder::SC_SUCCESS) && code.isValid();
					break;

				default:
					ocean_assert(false && "Never be here");
					OCEAN_SET_FAILED(validation);
					return validation.succeeded();
			}

			OCEAN_EXPECT_TRUE(validation, qrcodeGenerated);

			MicroQRCode testCode;
			const bool decoded = MicroQRCodeDecoder::decodeMicroQRCode(code.modules(), testCode);
			OCEAN_EXPECT_TRUE(validation, decoded && testCode.isValid());

			OCEAN_EXPECT_TRUE(validation, code == testCode);
		}
		while (Timestamp(true) < start + testDuration);
	}

	// Validation - reuse verification data of the encoding test for Micro QR codes

	{
		const TestMicroQRCodeEncoder::MicroQRCodeVerificationItems verificationItems = TestMicroQRCodeEncoder::loadDataTestMicroQRCodeEncoding();

		if (verificationItems.empty())
		{
			ocean_assert(false && "This should never happen!");
			OCEAN_SET_FAILED(validation);
			return validation.succeeded();
		}

		Timestamp start(true);

		do
		{
			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(verificationItems.size() - 1));
			const TestMicroQRCodeEncoder::MicroQRCodeVerificationItem& verificationItem = verificationItems[randomIndex];

			std::vector<uint8_t> modules(verificationItem.modules_.size());

			for (size_t i = 0; i < verificationItem.modules_.size(); ++i)
			{
				modules[i] = verificationItem.modules_[i] == '1' ? 1u : 0u;
			}

			MicroQRCode testCode;
			const bool decoded = MicroQRCodeDecoder::decodeMicroQRCode(modules, testCode);
			OCEAN_EXPECT_TRUE(validation, decoded && testCode.isValid());

			OCEAN_EXPECT_EQUAL(validation, testCode.version(), verificationItem.version_);
			OCEAN_EXPECT_EQUAL(validation, testCode.errorCorrectionCapacity(), verificationItem.errorCorrectionCapacity_);

			const std::vector<uint8_t>& testData = testCode.data();
			const std::string testDataString(testData.begin(), testData.end());

			OCEAN_EXPECT_EQUAL(validation, testDataString, verificationItem.message_);
		}
		while (Timestamp(true) < start + testDuration);
	}

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}


} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
