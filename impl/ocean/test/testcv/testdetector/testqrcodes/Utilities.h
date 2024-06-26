/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"

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

/**
 * This class implements utility functions for the QR code tests
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT Utilities
{
	public:

		/**
		 * Paint randomized noise into a grayscale image
		 * @param yFrame Pointer to the data of a grayscale image. Must be valid.
		 * @param width The width of the frame pointed to by `yFrame`, range: [29, infinity)
		 * @param height The height of the frame pointer to by `yFrame`, range: [29, infinity)
		 * @param paddingElements The number of padding elements of the input frame `yFrame`, range: [0, infinity)
		 * @param location The location of the center of the noise that will be drawn.
		 * @param randomGenerator Random generator that is used to generate random noise
		 * @param foregroundColor Color that is used for the finder pattern, range: [0, 255]
		 * @param extraBorder An optional distance to the image border that all noise samples must be away from in order to be drawn
		 */
		static void drawNoisePattern(uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const Vector2& location, RandomGenerator& randomGenerator, const uint8_t foregroundColor, const Scalar extraBorder = Scalar(14));

		/**
		 * Generates a random string
		 * @param randomGenerator The random generator that will be used to generate the random string
		 * @param minSize The minimum size of the random string, range: [1, infinity)
		 * @param maxSize The maximum size of the random string, range: [minSize, infinity)
		 * @return The generated random string
		 */
		static std::string generateRandomString(RandomGenerator& randomGenerator, const unsigned int minSize = 1u, const unsigned int maxSize = 15u);

		/**
		 * Generates random string consisting of decimal digit characters ('0' to '9' only)
		 * @param randomGenerator The random generator that will be used to generate the random data
		 * @param size The size of string that will be generated in number of characters, range: [1, infinity)
		 * @return The generated random numeric string
		 */
		static std::string generateRandomNumericString(RandomGenerator& randomGenerator, const unsigned int size);

		/**
		 * Generates random string consisting of characters in alphanumeric character set defined by the QR code standard
		 * @param randomGenerator The random generator that will be used to generate the random data
		 * @param size The size of string that will be generated in number of characters, range: [1, infinity)
		 * @return The generated random alphanumeric string
		 */
		static std::string generateRandomAlphanumericString(RandomGenerator& randomGenerator, const unsigned int size);

		/**
		 * Generates a random data that consists of bytes (value range: 0-255)
		 * @param randomGenerator The random generator that will be used to generate the random data
		 * @param length The length of the data that will be generated in bytes, range: [1, infinity)
		 * @param data The memory where the generated data will be stored. Will be initialized internally, if necessary.
		 * @return True if the data was successfully generated, otherwise false
		 */
		static bool generateRandomByteData(RandomGenerator& randomGenerator, const unsigned int length, std::vector<uint8_t>& data);

		/**
		 * Generates random data that consists of values that are decimal digits (between 0 and 9, inclusive)
		 * @param randomGenerator The random generator that will be used to generate the random data
		 * @param sizeInBytes The of the data that will be generated in bytes, range: [1, infinity)
		 * @param data The memory where the generated data will be stored. Will be initialized internally, if necessary.
		 * @return True if the data was successfully generated, otherwise false
		 */
		static bool generateRandomDecimalDigitSequenceData(RandomGenerator& randomGenerator, const unsigned int sizeInBytes, std::vector<uint8_t>& data);

		/**
		 * Generates a list of unique and random QR codes
		 * @param randomGenerator The random generator that will be used to generate the QR codes
		 * @param numberCodes The number of QR codes that will be generated, range: [1, infinity)
		 * @param codes The resulting list of QR codes
		 * @return True if the requested number of QR codes were successfully generated, otherwise false
		 */
		static bool generateUniqueRandomQRCodes(RandomGenerator& randomGenerator, const unsigned int numberCodes, CV::Detector::QRCodes::QRCodes& codes);

		/**
		 * Returns the numeric charset used for encoding numeric data
		 * @return The numeric charset
		 */
		static const std::string& getNumericCharset();
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
