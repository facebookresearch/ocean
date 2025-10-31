/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestMicroQRCodeEncoder.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

#include "ocean/cv/detector/qrcodes/MicroQRCode.h"
#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"

#include "ocean/io/Base64.h"
#include "ocean/io/Utilities.h"

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

TestMicroQRCodeEncoder::FileDataCollection::FileDataCollection(const std::string& filename) :
	filename_(filename)
{
	ocean_assert(!filename_.empty());
}

SharedTestData TestMicroQRCodeEncoder::FileDataCollection::data(const size_t index)
{
	if (index >= size())
	{
		ocean_assert(false && "Invalid index");
		return nullptr;
	}

	IO::Utilities::Buffer buffer;
	if (!IO::Utilities::readFile(filename_, buffer))
	{
		return nullptr;
	}

	if (buffer.empty())
	{
		return nullptr;
	}

	Value value(buffer.data(), buffer.size());

	return std::make_shared<TestData>(std::move(value));
}

size_t TestMicroQRCodeEncoder::FileDataCollection::size()
{
	return filename_.empty() ? 0 : 1;
}

bool TestMicroQRCodeEncoder::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for Micro QR code encoding:   ---";
	Log::info() << " ";

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
	const TestDataManager::ScopedSubscription scopedSubscription = TestMicroQRCodeEncoder_registerTestDataCollection();
#endif

	bool allSucceeded = true;

	allSucceeded = testMicroQRCodeEncoding(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMicroQRCodeFormatEncodingDecoding() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Micro QR code encoding test succeeded.";
	}
	else
	{
		Log::info() << "Micro QR code encoding test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestQRCodes

/**
 * This class implements a simple instance for the GTest ensuring test data collections are registered.
 */
class TestMicroQRCodeEncoder : public ::testing::Test
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

TEST_F(TestMicroQRCodeEncoder, MicroQRCodeEncoding)
{
	EXPECT_TRUE(TestDetector::TestQRCodes::TestMicroQRCodeEncoder::testMicroQRCodeEncoding(GTEST_TEST_DURATION));
}

TEST_F(TestMicroQRCodeEncoder, MicroQRCodeFormatEncodingDecoding)
{
	EXPECT_TRUE(TestDetector::TestQRCodes::TestMicroQRCodeEncoder::testMicroQRCodeFormatEncodingDecoding());
}

namespace TestQRCodes
{

#endif // OCEAN_USE_GTEST

bool TestMicroQRCodeEncoder::testMicroQRCodeEncoding(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Micro QR code encoding test:";

	bool allSucceeded = true;

	const MicroQRCodeVerificationItems verificationItems = TestMicroQRCodeEncoder::loadDataTestMicroQRCodeEncoding();

	ocean_assert(!verificationItems.empty());
	if (!verificationItems.empty())
	{
		RandomGenerator randomGenerator;

		Timestamp start(true);

		do
		{
			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(verificationItems.size() - 1));
			const MicroQRCodeVerificationItem& verificationItem = verificationItems[randomIndex];

			MicroQRCode testCode;
			if (MicroQRCodeEncoder::encodeText(verificationItem.message_, verificationItem.errorCorrectionCapacity_, testCode) != QRCodeEncoderBase::SC_SUCCESS || !testCode.isValid())
			{
				allSucceeded = false;
			}

			allSucceeded = testCode.version() == verificationItem.version_
				&& testCode.errorCorrectionCapacity() == (unsigned int)verificationItem.errorCorrectionCapacity_
				&& allSucceeded;

			std::vector<char> modulesString(testCode.modules().size());
			for (unsigned int i = 0u; i < testCode.modules().size(); ++i)
			{
				modulesString[i] = testCode.modules()[i] == 0u ? '0' : '1';
			}

			allSucceeded = std::equal(modulesString.begin(), modulesString.end(), verificationItem.modules_.begin()) && allSucceeded;
		}
		while (Timestamp(true) < start + testDuration);
	}
	else
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestMicroQRCodeEncoder::testMicroQRCodeFormatEncodingDecoding()
{
	Log::info() << "Format encoding/decoding test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const std::array<MicroQRCodeEncoder::MaskingPattern, 4> maskingPatterns =
	{
		MicroQRCodeEncoder::MP_PATTERN_0,
		MicroQRCodeEncoder::MP_PATTERN_1,
		MicroQRCodeEncoder::MP_PATTERN_2,
		MicroQRCodeEncoder::MP_PATTERN_3,
	};

	for (unsigned int symbolNumber = 0u; symbolNumber < 8; symbolNumber++)
	{
		unsigned int version;
		MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity;
		if (!MicroQRCode::unpackSymbolNumber(symbolNumber, version, errorCorrectionCapacity))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		for (const MicroQRCodeEncoder::MaskingPattern& maskingPattern : maskingPatterns)
		{
			const uint32_t encodedFormat = MicroQRCodeEncoder::encodeFormat(version, errorCorrectionCapacity, maskingPattern);

			if (encodedFormat >> 15u != 0u)
			{
				allSucceeded = false;
			}

			unsigned int decodedVersion;
			MicroQRCode::ErrorCorrectionCapacity decodedErrorCorrectionCapacity;
			MicroQRCodeEncoder::MaskingPattern decodedMaskingPattern;

			if (MicroQRCodeEncoder::decodeFormatBits(encodedFormat, decodedVersion, decodedErrorCorrectionCapacity, decodedMaskingPattern) == false || version != decodedVersion || errorCorrectionCapacity != decodedErrorCorrectionCapacity || maskingPattern != decodedMaskingPattern)
			{
				allSucceeded = false;
			}

			// The encoded format must be recoverable with up to 3 incorrect bits

			uint32_t xorMask = 0u;

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				unsigned int bit;

				do
				{
					bit = RandomI::random(randomGenerator, 14u);
				}
				while (((xorMask >> bit) & 1u) == 1u);

				xorMask |= 1u << bit;
			}

			ocean_assert(xorMask >> 15u == 0u);
			const uint32_t encodedFormatWithErrors = encodedFormat ^ xorMask;

			if (MicroQRCodeEncoder::decodeFormatBits(encodedFormatWithErrors, decodedVersion, decodedErrorCorrectionCapacity, decodedMaskingPattern) == false || version != decodedVersion || errorCorrectionCapacity != decodedErrorCorrectionCapacity || maskingPattern != decodedMaskingPattern)
			{
				allSucceeded = false;
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation successful";
	}
	else
	{
		Log::info() << "Validation FAILED";
	}

	return allSucceeded;
}

TestMicroQRCodeEncoder::MicroQRCodeVerificationItems TestMicroQRCodeEncoder::loadDataTestMicroQRCodeEncoding()
{

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

	const SharedTestDataCollection dataCollection = TestDataManager::get().testDataCollection("microqrcodeencoder_encoding_decoding_1000");

	if (!dataCollection || dataCollection->size() != 1)
	{
		ocean_assert(false && "Data collection not registered!");
		return MicroQRCodeVerificationItems();
	}

	const SharedTestData data = dataCollection->data(0);

	if (!data || data->dataType() != TestData::DT_VALUE || !data->value().isBuffer())
	{
		ocean_assert(false && "Invalid data!");
		return MicroQRCodeVerificationItems();
	}

	size_t size = 0;
	const void* buffer = data->value().bufferValue(size);

	if (buffer == nullptr || size == 0)
	{
		ocean_assert(false && "Invalid data!");
		return MicroQRCodeVerificationItems();
	}

	return loadCSVTestMicroQRCodeEncoding(buffer, size);

#else // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

	// A minimal set of vetted verification data in case not test collection is used

	const MicroQRCodeVerificationItems verificationItems =
	{
		// clang-format off
		MicroQRCodeVerificationItem(1u, MicroQRCode::ECC_DETECTION_ONLY, MicroQRCodeEncoder::MP_PATTERN_0, "17", "1111111010110000010100101110100111011101010010111010011100000100001111111000100000000100110001000010010001000011111011101"),
		MicroQRCodeVerificationItem(2u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_0, "87797239", "1111111010101100000101000110111010110011011101001001101110100000010000010101111111111001110000000000001011100111110000110110111001110000110100100011011100001011111111001"),
		MicroQRCodeVerificationItem(2u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_2, "7555364080", "1111111010101100000100000110111010000101011101000101101110100011110000010001001111111001101000000001000011011111111100000010001111111000110000101011010011011001010111101"),
		MicroQRCodeVerificationItem(3u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_3, "72359823576", "111111101010101100000101010000101110101010011101110101100011101110100101110100000100111111111111100010110000000000010110100010011111101011000110101011111101111000101000011001100000111000001100111000110101101010111101110101100"),
		MicroQRCodeVerificationItem(3u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_0, "7001041861263744235", "111111101010101100000100001000101110100110111101110100001110101110101110100100000101000000111111101111111000000001011101111101100100000000011010011110110111011001101010000010000111110110101010001011011101110101111000001110011"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_0, "4785359018776414284894656", "1111111010101010110000010000000011101110100111111001011101000011011110111010111100010100000100110001001111111001010001100000000011011010101001010110010000111101001101111111001101111110110000011000011000111010000111100111000010010101110000111111100001110000000010001101110011011000010010111"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_1, "5901142436527132823831518543623", "1111111010101010110000010011000010101110101111101111011101001011100110111010011100001100000100101001111111111000111001100000000011101011100100100101110110100011110100011010000001111011010000101001110101111101100100010011100001011000001111111100111111001000000100101100011111010110000101100"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_25, MicroQRCodeEncoder::MP_PATTERN_0, "2319", "1111111010101010110000010100011100101110101001110111011101001100110010111010001001111100000100110000011111111010010011100000000100100011101101001000101000000100001010110011111111110111000001011010001011101110110011111111100101101001000001110101000100001110101101000001000011001101110000111"),
		MicroQRCodeVerificationItem(2u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_2, " H", "1111111010101100000101110110111010010011011101011001101110101101110000010111101111111011110000000001000111101101111100101100010101101101110110001001010111101101001111110"),
		MicroQRCodeVerificationItem(2u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_3, "A5FVYM", "1111111010101100000101101010111010110111011101011001101110100000010000010111011111111010110000000001111011011010101100111000100001101110101010100011110000011100111111010"),
		MicroQRCodeVerificationItem(3u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_3, "FZ.TO4L8%HZ", "111111101010101100000101011111101110101111111101110101000001101110100101001100000100000100111111100010111000000000010111100010011011000010001111110111101100001100001011011101101001111010001101010000011011000001111111110100110"),
		MicroQRCodeVerificationItem(3u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_0, "6IIMA7ZO*A .FV", "111111101010101100000100000000101110100010011101110100000100101110101100101100000101100011111111101010011000000001010100111101100001111010011111111110101100110010010011111000011100100000001000010010101010011011111101100110001"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_3, "Z/BBHR.A0S:BV3XFZ", "1111111010101010110000010110100101101110100010000001011101000100011110111010000101111100000101101001011111111000111101100000000111011011101010100011001110010101111101011111101100110001101011110100100111101111010100110011000110110101000001100010101001111010100101001101110111111111011100100"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_0, ".4BC1E1+8.2Q7FF5  Z", "1111111010101010110000010111000000101110100111001001011101011101110110111010000100100100000101000101111111111010111110100000000000101000100101110010101000010011110101100011000101111100100000111100000000011110011111111000000101001000100011101011000110010110110011010010101111000100111111111"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_25, MicroQRCodeEncoder::MP_PATTERN_3, ":+9+-QUD", "1111111010101010110000010011100100101110101110101111011101001100000110111010110000011100000101101100111111111010011111100000000011101011101110111110010000111111111100101011010001100010011010010101111011011100011111001011001100101000100011101011000100000000000000010111101110010000111101100"),
		MicroQRCodeVerificationItem(3u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_2, "$&3", "111111101010101100000100001011101110100000101101110100010101101110100101100100000101011110111111101001110000000000111011100011001000010010100000011011111011101101100011101101011100101111111000100001111101010111101011101001111"),
		MicroQRCodeVerificationItem(3u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_2, "Q8dIMUT&B", "111111101010101100000100000101101110101111111101110101010111101110100111001100000101010101111111100110100000000000100111111111000101000011001110111101111011100110111010000001010110110101111110101011001000101110100011101011111"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_15, MicroQRCodeEncoder::MP_PATTERN_0, "hR#\\'&bk/rS", "1111111010101010110000010000111011101110100111101111011101000010110110111010111011111100000100000100011111111001111101100000000000111010101001010110100010110011110011000011100010001001010000110110110110011011100001111100101111000010111011100111000111010100101010010001000011011100110011001"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_07, MicroQRCodeEncoder::MP_PATTERN_0, "Xj~ln2FtMs7'Q3", "1111111010101010110000010100111001101110100100110001011101011011111110111010000111101100000101101110011111111010111101000000000000111101100101110010110010101011110010000011011000100000010010010011000000101010110000001100100001101000001001101110111110111000101001100011100011000110011011001"),
		MicroQRCodeVerificationItem(4u, MicroQRCode::ECC_25, MicroQRCodeEncoder::MP_PATTERN_0, "#RTJ_", "1111111010101010110000010111101000101110101110111011011101001011011010111010010001001100000100010110011111111010010011100000000111101110101101001110000010000110111110001110101010001111111010011110101000011001011101111111101001001011000010110100001100011010111110111101000011111110011001101"),
		// clang-format on
	};

	ocean_assert(verificationItems.empty() == false);
	return verificationItems;

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
}

TestMicroQRCodeEncoder::MicroQRCodeVerificationItems TestMicroQRCodeEncoder::loadCSVTestMicroQRCodeEncoding(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size != 0);

	if (buffer == nullptr || size == 0)
	{
		ocean_assert(false && "Invalid data");
		return MicroQRCodeVerificationItems();
	}

	// Tokenize the network data (split into lines, delimiter: \n)

	MicroQRCodeVerificationItems verificationItems;

	size_t start = 0;
	const char* const bufferData = (const char*)(buffer);

	while (start < size)
	{
		size_t end = start;
		while (end < size && bufferData[end] != '\n')
		{
			end++;
		}

		ocean_assert(start <= end && end <= size);

		if (end > start && bufferData[start] != '#')
		{
			const std::string line = std::string(bufferData + start, end - start);
			ocean_assert(line.empty() == false);

			MicroQRCodeVerificationItem verificationItem;
			if (convertCSVToMicroQRCodeVerificationItem(line, verificationItem) == false)
			{
				ocean_assert(false && "The format of input data seems to be corrupted.");
				return MicroQRCodeVerificationItems();
			}

			verificationItems.emplace_back(std::move(verificationItem));
		}

		start = end + 1u;
	}

	return verificationItems;
}

bool TestMicroQRCodeEncoder::convertCSVToMicroQRCodeVerificationItem(const std::string& lineCSV, MicroQRCodeVerificationItem& verificationItem)
{
	if (lineCSV.empty() || lineCSV[0] == '#')
	{
		return false;
	}

	using TokenStringToErrorCorrectionCapacityMap = std::unordered_map<std::string, MicroQRCode::ErrorCorrectionCapacity>;

	const TokenStringToErrorCorrectionCapacityMap tokenStringToErrorCorrectionCapacityMap =
	{
		std::make_pair(std::string("ECC_DETECTION_ONLY"), MicroQRCode::ECC_DETECTION_ONLY),
		std::make_pair(std::string("ECC_LOW_7"), MicroQRCode::ECC_07),
		std::make_pair(std::string("ECC_MEDIUM_15"), MicroQRCode::ECC_15),
		std::make_pair(std::string("ECC_QUARTILE_25"), MicroQRCode::ECC_25),
	};

	using TokenStringToMaskingPatternMap = std::unordered_map<std::string, MicroQRCodeEncoder::MaskingPattern>;

	const TokenStringToMaskingPatternMap tokenStringToMaskingPatternMap =
	{
		std::make_pair(std::string("MASKING_PATTERN_0"), MicroQRCodeEncoder::MP_PATTERN_0),
		std::make_pair(std::string("MASKING_PATTERN_1"), MicroQRCodeEncoder::MP_PATTERN_1),
		std::make_pair(std::string("MASKING_PATTERN_2"), MicroQRCodeEncoder::MP_PATTERN_2),
		std::make_pair(std::string("MASKING_PATTERN_3"), MicroQRCodeEncoder::MP_PATTERN_3),
	};

	// Split current line at ','

	std::vector<std::string> tokens;
	std::string::size_type begin = lineCSV.find_first_not_of(' ');

	while (begin < lineCSV.size())
	{
		const std::string::size_type end = lineCSV.find(',', begin);
		const std::string token = lineCSV.substr(begin, end - begin);

		if (token.empty() == false)
		{
			tokens.push_back(token);
		}

		if (end == std::string::npos)
		{
			break;
		}

		begin = end + 1;
	}

	if (tokens.size() != 5)
	{
		ocean_assert(false && "The file data is invalid");
		return false;
	}

	// Token 1: version

	ocean_assert(tokens[0].empty() == false);

	int versionSigned = -1;
	if (String::isInteger32(tokens[0], &versionSigned) == false || versionSigned < 0)
	{
		ocean_assert(false && "The format of the input data seems to be corrupted.");
		return false;
	}

	const unsigned int version = (unsigned int)(versionSigned);

	if (version == 0u || version > 4u)
	{
		ocean_assert(false && "The format of the input data seems to be corrupted.");
		return false;
	}

	// Token 2: error correction level

	const TokenStringToErrorCorrectionCapacityMap::const_iterator errorCorrectionCapacityIter = tokenStringToErrorCorrectionCapacityMap.find(tokens[1]);

	if (errorCorrectionCapacityIter == tokenStringToErrorCorrectionCapacityMap.end())
	{
		ocean_assert(false && "The format of the input data seems to be corrupted.");
		return false;
	}

	const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity = errorCorrectionCapacityIter->second;

	// Token 3: masking pattern

	TokenStringToMaskingPatternMap::const_iterator maskingPatternIter = tokenStringToMaskingPatternMap.find(tokens[2]);

	if (maskingPatternIter == tokenStringToMaskingPatternMap.end())
	{
		ocean_assert(false && "The format of input data seems to be corrupted.");
		return false;
	}

	const MicroQRCodeEncoder::MaskingPattern maskingPattern = maskingPatternIter->second;

	// Token 4: raw message (base64-encoded)

	IO::Base64::Buffer buffer;

	if (IO::Base64::decode((unsigned char*)(tokens[3].c_str()), tokens[3].size(), buffer) == false)
	{
		ocean_assert(false && "The format of input data seems to be corrupted.");
		return false;
	}

	const std::string message = std::string((char*)buffer.data(), buffer.size());
	ocean_assert(message.size() == buffer.size() && message.empty() == false && message[0] != '\0');

	// Token 5: modules of the QR code as a string consisting of ones and zeros

	if (tokens[4].size() != MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version))
	{
		ocean_assert(false && "The format of input data seems to be corrupted.");
		return false;
	}

	verificationItem = MicroQRCodeVerificationItem(version, errorCorrectionCapacity, maskingPattern, message, /* modules */ tokens[4]);

	return true;
}

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

TestDataManager::ScopedSubscription TestMicroQRCodeEncoder_registerTestDataCollection()
{
	std::string absolutePath = "";
	ocean_assert(!absolutePath.empty());

	const std::string filename = absolutePath + "/encoding_decoding_data/micro_encoding_decoding_data_1000.csv";

	return TestDataManager::get().registerTestDataCollection("microqrcodeencoder_encoding_decoding_1000", std::make_unique<TestMicroQRCodeEncoder::FileDataCollection>(filename));
}

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
