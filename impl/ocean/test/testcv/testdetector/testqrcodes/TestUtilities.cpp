/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestUtilities.h"
#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

#include "ocean/math/Random.h"

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

bool TestUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for the utility functions:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testContainsCode(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = stressTestParseWifiConfig(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testParseWifiConfig(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testEscapeSpecialCharacters(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testUnescapeSpecialCharacters(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Utility functions test succeeded.";
	}
	else
	{
		Log::info() << "Utility functions test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestUtilities, TestContainsCode)
{
	EXPECT_TRUE(TestUtilities::testContainsCode(GTEST_TEST_DURATION));
}

TEST(TestUtilities, StressTestParseWifiConfig)
{
	EXPECT_TRUE(TestUtilities::stressTestParseWifiConfig(GTEST_TEST_DURATION));
}

TEST(TestUtilities, TestParseWifiConfig)
{
	EXPECT_TRUE(TestUtilities::testParseWifiConfig(GTEST_TEST_DURATION));
}

TEST(TestUtilities, TestEscapeSpecialCharacters)
{
	EXPECT_TRUE(TestUtilities::testEscapeSpecialCharacters(GTEST_TEST_DURATION));
}

TEST(TestUtilities, TestUnescapeSpecialCharacters)
{
	EXPECT_TRUE(TestUtilities::testUnescapeSpecialCharacters(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUtilities::testContainsCode(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for the check if a QR code is contained in a list of existing QR codes (without poses):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	Timestamp start(true);

	do
	{
		// Indicates whether the code that will be tested against a list of reference codes should be unique or contained in the list of existing codes.
		const bool useUniqueTestCode = RandomI::random(randomGenerator, 1u);

		// The number of reference codes that will be tested against; will be 0 in 10% of the cases, if the test code is unique (i.e. not in the list of existing codes)
		const size_t numberReferenceCodes = size_t(useUniqueTestCode && RandomI::random(randomGenerator, 0u, 9u) == 0u ? 0u : RandomI::random(randomGenerator, 1u, 100u));

		// The total number of unique and random codes required for this test iteration.
		const size_t totalNumberCodes = numberReferenceCodes + (useUniqueTestCode ? 1 : 0);
		ocean_assert(totalNumberCodes != 0u);

		// Generate all required unique and random codes.
		QRCodes  uniqueRandomCodes;

		if (!TestQRCodes::Utilities::generateUniqueRandomQRCodes(randomGenerator, (unsigned int)(totalNumberCodes), uniqueRandomCodes))
		{
			// Something went wrong; retry in the next iteration.
			continue;
		}

		if (uniqueRandomCodes.size() != totalNumberCodes)
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		QRCode testCode;

		if (useUniqueTestCode)
		{
			ocean_assert(uniqueRandomCodes.size() >= numberReferenceCodes + 1);

			testCode = uniqueRandomCodes.back();

			uniqueRandomCodes.pop_back();
		}
		else
		{
			const unsigned int codeIndex = RandomI::random(randomGenerator, (unsigned int)(uniqueRandomCodes.size() - 1));

			testCode = uniqueRandomCodes[codeIndex];
		}

		const QRCodes & referenceCodes = uniqueRandomCodes;

		const bool containsCode = CV::Detector::QRCodes::Utilities::containsCode(referenceCodes, testCode);

		if (useUniqueTestCode == containsCode)
		{
			allSucceeded = false;
		}

	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		Log::info() << "Random generator seed: " << randomGenerator.seed();
	}

	return allSucceeded;
}

bool TestUtilities::stressTestParseWifiConfig(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test for parsing Wi-Fi configurations stored as a string:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	Timestamp start(true);

	do
	{
		// A V40 code with binary encoding and low error correction can hold up to 2953 bytes; choosing a maximum of ~2x for this test.
		const std::string randomData = Test::TestCV::TestDetector::TestQRCodes::Utilities::generateRandomString(randomGenerator, 1u, 6000u);

		std::string parsedSsid;
		std::string parsedPassword;
		std::string parsedEncryption;
		bool parsedIsHiddenSsid = false;

		const CV::Detector::QRCodes::Utilities::ParsingStatus parsingStatus = CV::Detector::QRCodes::Utilities::parseWifiConfig(randomData, parsedSsid, parsedPassword, &parsedEncryption, &parsedIsHiddenSsid);

		switch (parsingStatus)
		{
			case CV::Detector::QRCodes::Utilities::PS_SUCCESS:
			case CV::Detector::QRCodes::Utilities::PS_ERROR_INVALID_PREFIX:
			case CV::Detector::QRCodes::Utilities::PS_ERROR_INVALID_FIELD_FORMAT:
			case CV::Detector::QRCodes::Utilities::PS_ERROR_INVALID_FIELD_DATA:
			case CV::Detector::QRCodes::Utilities::PS_ERROR_INVALID_FIELD_TYPE:
			case CV::Detector::QRCodes::Utilities::PS_ERROR_DUPLICATE_FIELD_TYPE:
			case CV::Detector::QRCodes::Utilities::PS_ERROR_INVALID_TERMINATION:
				// Nothing to do; just ensuring all cases are covered.
				break;
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Stress test: Succeeded.";
	}
	else
	{
		Log::info() << "Stress test: FAILED!";
	}

	return allSucceeded;
}

bool TestUtilities::testParseWifiConfig(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for parsing Wi-Fi configurations stored as a string:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	Timestamp start(true);

	do
	{
		const std::string ssid = Test::TestCV::TestDetector::TestQRCodes::Utilities::generateRandomString(randomGenerator, 1u, 30u);
		const std::string escapedSsid = CV::Detector::QRCodes::Utilities::escapeSpecialCharacters(ssid);

		const std::vector<std::string> encryptionTypes =
		{
			"WEP",
			"WPA",
			""
		};

		const std::string encryptionType = encryptionTypes[RandomI::random(randomGenerator, (unsigned int)(encryptionTypes.size() - 1))];

		std::string password;
		std::string escapedPassword;

		if (!encryptionType.empty())
		{
			password = Test::TestCV::TestDetector::TestQRCodes::Utilities::generateRandomString(randomGenerator, 1u, 30u);
			escapedPassword = CV::Detector::QRCodes::Utilities::escapeSpecialCharacters(password);
		}

		ocean_assert(encryptionType.empty() || !escapedPassword.empty());

		const bool isHiddenSsid = RandomI::random(randomGenerator, 1u) == 0u;

		// The types of fields that the Wi-Fi config may contain
		enum FieldType : uint32_t
		{
			FT_SSID = 0u,
			FT_PASSWORD,
			FT_ENCRYPTION,
			FT_HIDDEN,
		};

		// A vector of fields that will be added to the Wi-Fi configuration. The vector may contain duplicate fields if the
		// resulting configuration should contain the corresponding field multiple times. The order of elements in this vector
		// is ignored; the elements will be randomly selected later.
		std::vector<FieldType> fieldTypes;

		// The resulting Wi-Fi configuration for testing.
		std::string wifiConfig;

		// Create 80% correct Wi-Fi configurations, 20% invalid ones
		const bool createValidWifiConfig = RandomI::random(randomGenerator, 9u) < 8u;

		if (createValidWifiConfig)
		{
			wifiConfig = "WIFI:";
			fieldTypes = {FT_SSID, FT_PASSWORD, FT_ENCRYPTION, FT_HIDDEN};
		}
		else
		{
			const bool useValidPrefix = RandomI::random(randomGenerator, 1u) == 1u;
			if (useValidPrefix)
			{
				wifiConfig = "WIFI:";
			}
			else
			{
				const bool useNoPrefixAtAll = RandomI::random(randomGenerator, 1u) == 1u;

				if (useNoPrefixAtAll)
				{
					wifiConfig = "";
				}
				else
				{
					wifiConfig = Test::TestCV::TestDetector::TestQRCodes::Utilities::generateRandomString(randomGenerator, 1u, 15u) + ":";
				}
			}

			// All fields: either omit a field entirely or add it multiple times.
			const unsigned int numberSsidFields = RandomI::random(randomGenerator, 4u) < 4u ? 0u : RandomI::random(randomGenerator, 2u, 5u);
			for (unsigned int iteration = 0u; iteration < numberSsidFields; ++iteration)
			{
				fieldTypes.push_back(FT_SSID);
			}

			const unsigned int numberPasswordFields = RandomI::random(randomGenerator, 4u) < 4u ? 0u : RandomI::random(randomGenerator, 2u, 5u);
			for (unsigned int iteration = 0u; iteration < numberPasswordFields; ++iteration)
			{
				fieldTypes.push_back(FT_PASSWORD);
			}

			const unsigned int numberEncryptionFields = RandomI::random(randomGenerator, 4u) < 4u ? 0u : RandomI::random(randomGenerator, 2u, 5u);
			for (unsigned int iteration = 0u; iteration < numberEncryptionFields; ++iteration)
			{
				fieldTypes.push_back(FT_ENCRYPTION);
			}

			const unsigned int numberIsHiddenSsidFields = RandomI::random(randomGenerator, 4u) < 4u ? 0u : RandomI::random(randomGenerator, 2u, 5u);
			for (unsigned int iteration = 0u; iteration < numberIsHiddenSsidFields; ++iteration)
			{
				fieldTypes.push_back(FT_HIDDEN);
			}
		}

		while (!fieldTypes.empty())
		{
			const unsigned int fieldIndex = RandomI::random(randomGenerator, (unsigned int)(fieldTypes.size() - 1));
			const FieldType fieldType = fieldTypes[fieldIndex];

			switch (fieldType)
			{
				case FT_SSID:
					wifiConfig += "S:" + escapedSsid + ";";
					break;

				case FT_PASSWORD:
					wifiConfig += "P:" + escapedPassword + ";";
					break;

				case FT_ENCRYPTION:
					wifiConfig += "T:" + encryptionType + ";";
					break;

				case FT_HIDDEN:
					wifiConfig += "H:" + std::string(isHiddenSsid ? "true" : "false") + ";";
					break;

				// Intentionally no default case
			}

			std::swap(fieldTypes[fieldIndex], fieldTypes.back());
			fieldTypes.pop_back();
		}

		wifiConfig += ";";

		std::string parsedSsid;
		std::string parsedPassword;
		std::string parsedEncryption;
		bool parsedIsHiddenSsid = false;

		const CV::Detector::QRCodes::Utilities::ParsingStatus parsingStatus = CV::Detector::QRCodes::Utilities::parseWifiConfig(wifiConfig, parsedSsid, parsedPassword, &parsedEncryption, &parsedIsHiddenSsid);
		const bool parsingSucceeded = parsingStatus == CV::Detector::QRCodes::Utilities::PS_SUCCESS;

		if (parsingSucceeded != createValidWifiConfig)
		{
			allSucceeded = false;
		}
		else
		{
			if (parsingSucceeded)
			{
				if (parsedSsid != ssid || parsedPassword != password || parsedEncryption != encryptionType || parsedIsHiddenSsid != isHiddenSsid)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

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

bool TestUtilities::testEscapeSpecialCharacters(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for escaping special characters in a string:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	Timestamp start(true);

	do
	{
		// A random selection of characters that will be escaped (may contain duplicates!)
		const std::string specialCharacters = TestQRCodes::Utilities::generateRandomString(randomGenerator, 5u, 15u);
		ocean_assert(!specialCharacters.empty());

		const unsigned int minimumSize = RandomI::random(randomGenerator, 1u, 50u);
		const unsigned int maximumSize = minimumSize + RandomI::random(randomGenerator, 0u, 100u);
		const std::string rawString = TestQRCodes::Utilities::generateRandomString(randomGenerator, minimumSize, maximumSize);
		ocean_assert(!rawString.empty());

		const std::string escapedString = CV::Detector::QRCodes::Utilities::escapeSpecialCharacters(rawString, specialCharacters);

		if (!validateEscapeSpecialCharacters(rawString, specialCharacters, escapedString))
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

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

bool TestUtilities::testUnescapeSpecialCharacters(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test for unescaping special characters in a string:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	Timestamp start(true);

	do
	{
		// A random selection of character that will be escaped (may contain duplicates!)
		const std::string specialCharacters = TestQRCodes::Utilities::generateRandomString(randomGenerator, 5u, 15u);
		ocean_assert(!specialCharacters.empty());

		// A random string that should be escaped.
		const unsigned int minimumSize = RandomI::random(randomGenerator, 1u, 50u);
		const unsigned int maximumSize = minimumSize + RandomI::random(randomGenerator, 0u, 100u);
		const std::string rawString = TestQRCodes::Utilities::generateRandomString(randomGenerator, minimumSize, maximumSize);
		ocean_assert(!rawString.empty());

		const std::string escapedString = CV::Detector::QRCodes::Utilities::escapeSpecialCharacters(rawString, specialCharacters);

		std::string unescapedString;
		if (!CV::Detector::QRCodes::Utilities::unescapeSpecialCharacters(escapedString, unescapedString, specialCharacters))
		{
			allSucceeded = false;
		}

		if (unescapedString != rawString)
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

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

bool TestUtilities::validateEscapeSpecialCharacters(const std::string& rawString, const std::string& specialCharacters, const std::string& testEscapedString)
{
	ocean_assert(!rawString.empty());
	ocean_assert(!specialCharacters.empty());
	ocean_assert(!testEscapedString.empty());

	std::string validationEscapedString = rawString;

	const std::unordered_set<char> specialCharactersSet(specialCharacters.begin(), specialCharacters.end());

	for (size_t index = validationEscapedString.size() - 1; index < validationEscapedString.size(); --index)
	{
		const char character = validationEscapedString[index];

		if (specialCharactersSet.find(character) != specialCharactersSet.cend())
		{
			validationEscapedString.replace(index, 1, std::string(1, '\\') + character);
		}
	}

	return testEscapedString == validationEscapedString;
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
