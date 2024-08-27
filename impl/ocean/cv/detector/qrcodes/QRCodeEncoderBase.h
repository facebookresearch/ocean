/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"

#include "ocean/math/Vector2.h"

#include <cctype>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * This class implements basic QRCodeEncoder functionality
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT QRCodeEncoderBase
{
	public:

		/// Codeword: sequence of 8 bits
		typedef uint8_t Codeword;

		/// Vector of codewords
		typedef std::vector<Codeword> Codewords;

		/// Typedef for a bit buffer
		typedef std::vector<uint8_t> BitBuffer;

		/// Forward declaration of the segment class
		class Segment;

		/// Typedef for a vector of segments
		typedef std::vector<Segment> Segments;

		/**
		 * Definition of the segment class
		 * A sequence is a sequence of data encoded according to the rules of one ECI or encodation mode
		 */
		class OCEAN_CV_DETECTOR_QRCODES_EXPORT Segment
		{
			public:
				/**
				 * Constructor for segments
				 * @param mode The data encodation mode of this segment
				 * @param characters The number of characters stored in this segment, range: [0, infinity)
				 * @param bitBuffer The bit sequence representing the data stored in this segment, must be valid
				 */
				inline Segment(const QRCode::EncodingMode mode, const unsigned int characters, BitBuffer& bitBuffer);

				/**
				 * Returns the encodation mode set for this segment
				 * @return The encodation mode
				 */
				inline QRCode::EncodingMode encodationMode() const;

				/**
				 * Returns the number of characters stored in this segment
				 * @return The number of characters stored in this segment
				 */
				inline unsigned int characters() const;

				/**
				 * Returns the encoded bits stored in this segment
				 * @return The encoded bits
				 */
				inline const BitBuffer& bitBuffer() const;

				/**
				 * Encode a sequence of digits (0-9) and store it in a segment
				 * @param data The data that will be encoded, must be a sequence of digits, i.e., `isNumeric(data) == true`
				 * @param segments The segment storing the encoded data will be appended to this vector, memory will be initialized internally
				 * @return True if the data was successfully encoded, otherwise false
				 */
				static bool generateSegmentNumeric(const std::string& data, Segments& segments);

				/**
				 * Encode a sequence of alphanumeric characters (cf. ISO/IEC 18004:2015, Table 5) and store it in a segment
				 * @param data The data that will be encoded, must be a sequence of alphanumeric characters, i.e., `isAlphanumeric(data) == true`
				 * @param segments The segment storing the encoded data will be appended to this vector, memory will be initialized internally
				 * @return True if the data was successfully encoded, otherwise false
				 */
				static bool generateSegmentAlphanumeric(const std::string& data, Segments& segments);

				/**
				 * Encode a sequence of bytes and store it in a segment
				 * @param data The data that will be encoded
				 * @param segments The segment storing the encoded data will be appended to this vector, memory will be initialized internally
				 * @return True if the data was successfully encoded, otherwise false
				 */
				static bool generateSegmentsBytes(const std::vector<uint8_t>& data, Segments& segments);

				/**
				 * Helper function to append a certain number of bits of a number to a bit buffer
				 * @param value The value of which the first `N` bits will be appended to the bit buffer
				 * @param bits The number of bits to append, range: [0, sizeof(unsigned int) * 8]
				 * @param bitBuffer The bit buffer to which the `N` bits will be appended
				 */
				static inline void bitBufferAppend(const unsigned int value, size_t bits, BitBuffer& bitBuffer);

				/**
				 * Test to check if data is numeric (consisting of only digits)
				 * @param data The data to be tested
				 * @return True if the data consists of only digits, otherwise false
				 */
				static inline bool isNumericData(const std::string& data);

				/**
				 * Test to check if data contains only alphanumeric characters
				 * @param data The data to be tested
				 * @return True if the data consists of alphanumeric characters, otherwise false
				 * @sa getAlphanumericCharset()
				 */
				static inline bool isAlphanumericData(const std::string& data);

				/**
				 * Returns the character set for the alphanumeric data mode
				 * The character set for the alphanumeric data mode, cf. ISO/IEC 18004:2015, Table 5. The index
				 * of each character in the string corresponds to the value assigned to them in the alphanumeric
				 * encoding/decoding table.
				 * @return The character set for the alphanumeric data mode.
				 */
				static inline const std::string& getAlphanumericCharset();

			protected:

				/// The mode used to encode the data of this segment
				QRCode::EncodingMode encodationMode_;

				/// The number of characters stored in this segment
				unsigned int characters_;

				/// The actual encoded data (sequence of bits)
				BitBuffer bitBuffer_;
		};

		/**
		 * Definition of the ReedSolomon class
		 */
		class ReedSolomon
		{
			public:

				/// Coefficients of the divisor polynomial, stored from highest to lower power (excluding the leading term which is always 1). Example x^3 + 255x^2 + 8x + 93 is stored as {255, 8, 93}.
				typedef std::vector<uint8_t> Coefficients;

			public:

				/**
				 * Generates the Reed-Solomon coefficients for a divisor polynomial of degree `N`.
				 * @param degree The degree of the divisor polynomial, range: [1, infinity)
				 * @return The coeffiencts of the divisor polynomial (will have `N = degree` elements)
				 */
				static Coefficients generateCoefficients(const unsigned int degree);

				/**
				 * Computes the Reed-Solomon error correction codewords for a sequence of data codewords
				 * @param codewords The input codewords for which the error correction will be computed
				 * @param coefficients The coefficients of the Reed-Solomon divisor polynomial of degree `N`
				 * @return The error correction codewords
				 */
				static Codewords computeRemainders(const Codewords& codewords, const Coefficients& coefficients);

			protected:

				/**
				 * Return the product of two fields modulo GF(2^8/0x11D)
				 */
				static uint8_t multiply(const uint8_t a, const uint8_t b);
		};

		/**
		 * Computes the remainder of a polynomial long division for (n, k) BCH codes
		 * Notation:
		 * n : block length in bits
		 * k : number of information/data bits
		 * @param data The data used as the nominator in this division
		 * @return The remainder of the polynomial division
		 * @tparam tBlockLength The length, n, of the BCH code in bits, range: (tDataLength, infinity)
		 * @tparam tDataLength The number the information/data bits, k, range: [1, tBlockLength)
		 * @tparam tGeneratorPolynomial The generator polynomial used by this code for the polynomial division. This is specificied as an integer (or binary number). Range: (0, 2^n - 1) (Important: make sure this value is correct according to the Galois field theory behind it, there are no additional sanity checks)
		 */
		template <uint32_t tBlockLength, uint32_t tDataLength, uint32_t tGeneratorPolynomial>
		static inline uint32_t computePolynomialDivisonRemainderBCH(const uint32_t data);

		/**
		 * Determines the number of 1-bits in an integer value (Hamming weight)
		 * @param value The value of which the 1-bits will be counted
		 * @return The number of 1-bits in the input value
		 */
		static inline uint32_t computeHammingWeight(uint32_t value);
};

inline QRCodeEncoderBase::Segment::Segment(const QRCode::EncodingMode mode, const unsigned int characters, BitBuffer& bitBuffer) :
	encodationMode_(mode),
	characters_(characters),
	bitBuffer_(bitBuffer)
{
	// Nothing else to do.
}

inline QRCode::EncodingMode QRCodeEncoderBase::Segment::encodationMode() const
{
	return encodationMode_;
}

inline unsigned int QRCodeEncoderBase::Segment::characters() const
{
	return characters_;
}

inline const typename QRCodeEncoderBase::BitBuffer& QRCodeEncoderBase::Segment::bitBuffer() const
{
	return bitBuffer_;
}

inline bool QRCodeEncoderBase::Segment::isNumericData(const std::string& data)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (std::isdigit((unsigned char)data[i]) == false)
		{
			return false;
		}
	}

	return data.size() != 0;
}

inline bool QRCodeEncoderBase::Segment::isAlphanumericData(const std::string& data)
{
	if (data.empty())
	{
		return false;
	}

	const std::string& alphanumericCharset = getAlphanumericCharset();

	for (const char character : data)
	{
		if (alphanumericCharset.find(character) == std::string::npos)
		{
			return false;
		}
	}

	return true;
}

inline const std::string& QRCodeEncoderBase::Segment::getAlphanumericCharset()
{
	const static std::string alphanumericCharset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
	ocean_assert(alphanumericCharset.size() == 45);

	return alphanumericCharset;
}

inline void QRCodeEncoderBase::Segment::bitBufferAppend(const unsigned int value, size_t bits, BitBuffer& bitBuffer)
{
	ocean_assert(bits <= sizeof(unsigned int) * 8);

	if (bits == 0u)
	{
		return;
	}

	for (size_t i = bits - 1; i < bits; --i)
	{
		bitBuffer.push_back((value >> i) & 1);
	}
}

template <uint32_t tBlockLength, uint32_t tDataLength, uint32_t tGeneratorPolynomial>
inline uint32_t QRCodeEncoderBase::computePolynomialDivisonRemainderBCH(const uint32_t data)
{
	static_assert(tBlockLength != 0u && tDataLength != 0u && tBlockLength > tDataLength, "The block length must be larger than the number of data bits and both must be nonzero.");
	static_assert(tGeneratorPolynomial != 0u && tGeneratorPolynomial >> tBlockLength == 0u, "The generator polynomial cannot be zero and must fit into the block length");
	ocean_assert(data >> tBlockLength == 0u);

	// Example for a (15, 5) BCH code:
	//
	//  * block length: 15
	//  * data length: 5
	//  * generator polynomial = 10100110111 ~ G(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1
	//  * data = 000111101011001, i.e. 00011|1101011001
	//                                 data | error correction
	//
	// remainder0 = 000111101011001
	//            ^ 10100110111     skip step - left-most bit of the remainder is zero
	//            -----------------
	// remainder1 = 000111101011001
	//            ^  10100110111    skip step - left-most bit of the remainder is zero
	//            -----------------
	// remainder2 = 000111101011001
	//            ^   10100110111   skip step - left-most bit of the remainder is zero
	//            -----------------
	// remainder3 = 000111101011001
	//            ^    10100110111
	//            -----------------
	// remainder4 = 000010100110111
	//            ^     10100110111
	//            -----------------
	// remainder  = 000000000000000

	constexpr uint32_t errorCorrectionBits = tBlockLength - tDataLength;

	uint32_t remainder = data;

	for (uint32_t i = 0u; i < tDataLength; ++i)
	{
		ocean_assert(tDataLength >= i + 1u);
		const uint32_t shift = tDataLength - i - 1u;

		if (remainder & (1u << (shift + errorCorrectionBits)))
		{
			remainder ^= tGeneratorPolynomial << shift;
		}
	}

	ocean_assert(tGeneratorPolynomial > remainder);
	return remainder;
}

inline uint32_t QRCodeEncoderBase::computeHammingWeight(uint32_t value)
{
	uint32_t weight = 0u;

	while (value > 0u)
	{
		value = value & (value - 1u);
		weight += 1u;
	}

	return weight;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
