// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/cv/Canvas.h"

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

void Utilities::drawNoisePattern(uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const Vector2& location, RandomGenerator& randomGenerator, const uint8_t foregroundColor, const Scalar extraBorder)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 29u && height >= 29u);

	if (location.x() - extraBorder < Scalar(0) || location.x() + extraBorder >= Scalar(width) ||
		location.y() - extraBorder < Scalar(0) || location.y() + extraBorder >= Scalar(height))
	{
		// Nothing to be drawn.
		return;
	}

	for (unsigned int n = 0u; n < 5u; ++n)
	{
		const Vector2 offset = Random::vector2(randomGenerator, Scalar(-3), Scalar(3));
		CV::Canvas::point8BitPerChannel<1u, 3u>(yFrame, width, height, location + offset, &foregroundColor, paddingElements);
	}
}

std::string Utilities::generateRandomString(RandomGenerator& randomGenerator, const unsigned int minSize, const unsigned int maxSize)
{
	ocean_assert(minSize != 0u && minSize <= maxSize);

	const unsigned int size = RandomI::random(randomGenerator, minSize, maxSize);

	std::string randomString = std::string(size, ' ');
	for (unsigned int index = 0u; index < size; ++index)
	{
		// Using ASCII codes 32-126
		randomString[index] = char(RandomI::random(randomGenerator, 32u, 126u));
	}

	return randomString;
}

bool Utilities::generateRandomNumericData(RandomGenerator& randomGenerator, const unsigned int sizeInBytes, std::vector<uint8_t>& data)
{
	ocean_assert(sizeInBytes != 0u);

	data.resize(sizeInBytes);

	for (unsigned int i = 0u; i < sizeInBytes; ++i)
	{
		data[i] = uint8_t(RandomI::random(randomGenerator, 9u));
	}

	return true;
}

bool Utilities::generateRandomAlphanumericData(RandomGenerator& randomGenerator, const unsigned int sizeInBytes, std::vector<uint8_t>& data)
{
	ocean_assert(sizeInBytes != 0u);

	data.resize(sizeInBytes);

	const std::string alphanumericCharset(QRCodeEncoder::Segment::ALPHANUMERIC_CHARSET);
	ocean_assert(!alphanumericCharset.empty());

	for (size_t i = 0; i < sizeInBytes; ++i)
	{
		data[i] = uint8_t(alphanumericCharset[RandomI::random(randomGenerator, (unsigned int)(alphanumericCharset.size() - 1))]);
	}

	return true;
}

bool Utilities::generateRandomByteData(RandomGenerator& randomGenerator, const unsigned int sizeInBytes, std::vector<uint8_t>& data)
{
	ocean_assert(sizeInBytes != 0u);

	data.resize(sizeInBytes);

	for (unsigned int i = 0u; i < sizeInBytes; ++i)
	{
		data[i] = uint8_t(RandomI::random(randomGenerator, 255u));
	}

	return true;
}

bool Utilities::generateUniqueRandomQRCodes(RandomGenerator& randomGenerator, const unsigned int numberCodes, CV::Detector::QRCodes::QRCodes& codes)
{
	ocean_assert(numberCodes != 0u);

	QRCodes uniqueRandomCodes;
	uniqueRandomCodes.reserve(numberCodes);

	for (size_t codeIndex = 0; codeIndex < numberCodes; ++codeIndex)
	{
		// There is a chance a newly generated code will already exists; so, limit the number of attempts to avoid infinite loops.
		unsigned int numberAttempts = 0u;
		constexpr unsigned int maxNumberAttempts = 100u;

		while (numberAttempts < maxNumberAttempts)
		{
			const unsigned int newRandomPayloadSize = RandomI::random(randomGenerator, 1u, 256u);
			std::vector<uint8_t> newRandomPayload;

			if (!generateRandomByteData(randomGenerator, newRandomPayloadSize, newRandomPayload))
			{
				Log::error() << "Failed to generate random data!";
				ocean_assert(false && "This should never happen!");

				break;
			}

			bool isUniquePayload = true;

			for (const QRCode& code : uniqueRandomCodes)
			{
				const std::vector<uint8_t>& payload = code.data();

				if (newRandomPayload.size() == payload.size())
				{
					if (memcmp(newRandomPayload.data(), payload.data(), newRandomPayload.size()) == 0)
					{
						isUniquePayload = false;
						break;
					}
				}
			}

			if (!isUniquePayload)
			{
				// The random payload is not unique; try again.
				++numberAttempts;

				continue;
			}

			// Add a new QR code to the list of codes
			constexpr QRCode::ErrorCorrectionCapacity errorCorrectionCapacities[4] =
			{
				QRCode::ECC_07,
				QRCode::ECC_15,
				QRCode::ECC_25,
				QRCode::ECC_30,
			};

			const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity = errorCorrectionCapacities[RandomI::random(randomGenerator, 3u)];

			QRCode code;

			if (!QRCodeEncoder::encodeBinary(newRandomPayload, errorCorrectionCapacity, code))
			{
				Log::error() << "Failed to generate QR code!";
				ocean_assert(false && "This should never happen!");

				break;
			}

			ocean_assert(code.isValid());
			uniqueRandomCodes.emplace_back(std::move(code));

			// A new unique code has been found, so no need to re-try.
			break;
		}

		if (numberAttempts >= maxNumberAttempts)
		{
			// Exceeded the maximum number of attempts; there won't be enough codes at the end so abort here.
			break;
		}
	}

	if (uniqueRandomCodes.size() != numberCodes)
	{
		// Failed to generate enough unique random payloads; try again in the next iteration.
		return false;
	}

	codes = std::move(uniqueRandomCodes);

	return true;
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
