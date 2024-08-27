/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/QRCodeEncoderBase.h"
#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

bool QRCodeEncoderBase::Segment::generateSegmentNumeric(const std::string& data, Segments& segments)
{
	if (isNumericData(data) == false)
	{
		return false;
	}

	// Cf. ISO/IEC 18004:2015, Section 7.4.3

	BitBuffer bitBuffer;
	bitBuffer.reserve((data.size() / 3) * 10 + (data.size() % 3 != 0 ? (data.size() % 3) * 3 + 1 : 0));

	for (size_t i = 0; i < data.size(); i += 3)
	{
		const size_t length = (i + 3 <= data.size() ? 3 : data.size() - i);
		ocean_assert(length != 0 && length <= 3);

		int value;
		if (String::isInteger32(data.substr(i, length), &value) == false)
		{
			return false;
		}
		ocean_assert(value >= 0);

		bitBufferAppend((unsigned int)value, length * 3 + 1, bitBuffer);
	}

	segments.emplace_back(QRCode::EM_NUMERIC, (unsigned int)data.size(), bitBuffer);

	return true;
}

bool QRCodeEncoderBase::Segment::generateSegmentAlphanumeric(const std::string& data, Segments& segments)
{
	if (!isAlphanumericData(data))
	{
		return false;
	}

	// Cf. ISO/IEC 18004:2015, Section 7.4.4

	BitBuffer bitBuffer;
	bitBuffer.reserve(data.size() % 2 == 0 ? 11 * data.size() : 11 * (data.size() - 1) + 6);

	const std::string& alphanumericCharset = getAlphanumericCharset();

	unsigned int buffer = 0u;
	unsigned int bufferSize = 0u;
	for (size_t i = 0; i < data.size(); ++i)
	{
		const size_t charsetIndex = alphanumericCharset.find(data[i]);
		ocean_assert(charsetIndex < alphanumericCharset.size());

		const unsigned int encodedCharacter = (unsigned int)charsetIndex;

		buffer = buffer * 45u + encodedCharacter;
		bufferSize += 1u;

		if (bufferSize == 2u)
		{
			bitBufferAppend(buffer, 11, bitBuffer);

			buffer = 0u;
			bufferSize = 0u;
		}
	}

	ocean_assert(bufferSize <= 1u);

	if (bufferSize != 0u)
	{
		bitBufferAppend(buffer, 6, bitBuffer);
	}

	segments.emplace_back(QRCode::EM_ALPHANUMERIC, (unsigned int)data.size(), bitBuffer);

	return true;
}

bool QRCodeEncoderBase::Segment::generateSegmentsBytes(const std::vector<uint8_t>& data, Segments& segments)
{
	ocean_assert(data.empty() == false);

	// Cf. ISO/IEC 18004:2015, Section 7.4.5

	BitBuffer bitBuffer;
	for (size_t i = 0; i < data.size(); ++i)
	{
		bitBufferAppend((unsigned int)data[i], 8, bitBuffer);
	}
	segments.emplace_back(QRCode::EM_BYTE, (unsigned int)data.size(), bitBuffer);

	return true;
}

QRCodeEncoderBase::ReedSolomon::Coefficients QRCodeEncoderBase::ReedSolomon::generateCoefficients(const unsigned int degree)
{
	// Cf. ISO/IEC 18004:2015, Annex B
	ocean_assert(degree != 0u && degree < 256u);

	Coefficients coefficients(degree);

	// Monomial x^0
	coefficients[degree - 1u] = 1u;

	// Compute the product polynomial: (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}).
	// The highest coefficient is dropped. The remaining coefficients are stored in descending order.
	// Note: r = 0x02 is a generator element of GF(2^8/0x11D).

	uint8_t root = 1u;
	for (unsigned int i = 0u; i < coefficients.size(); ++i)
	{
		for (unsigned int j = 0; j < coefficients.size(); ++j)
		{
			coefficients[j] = multiply(coefficients[j], root);

			if (j + 1 < coefficients.size())
			{
				coefficients[j] ^= coefficients[j + 1u];
			}
		}

		root = multiply(root, 0x02u);
	}

	return coefficients;
}

QRCodeEncoderBase::Codewords QRCodeEncoderBase::ReedSolomon::computeRemainders(const Codewords& codewords, const Coefficients& coefficients)
{
	std::deque<Codeword> remaindersDeque(coefficients.size(), 0u);

	for (const Codeword codeword : codewords)
	{
		const uint8_t factor = codeword ^ remaindersDeque[0];
		remaindersDeque.pop_front();
		remaindersDeque.push_back(0u);

		for (size_t i = 0; i < remaindersDeque.size(); ++i)
		{
			remaindersDeque[i] ^= multiply(coefficients[i], factor);
		}
	}

	Codewords remainders(remaindersDeque.begin(), remaindersDeque.end());

	return remainders;
}

uint8_t QRCodeEncoderBase::ReedSolomon::multiply(const uint8_t x, const uint8_t y)
{
	// Russian peasant multiplication
	unsigned int z = 0u;
	for (unsigned int i = 7u; i < 8u; --i)
	{
		z = (z << 1) ^ ((z >> 7) * 0x11D);
		z ^= ((y >> i) & 1) * x;
	}

	ocean_assert(z >> 8u == 0u);

	return uint8_t(z);
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
