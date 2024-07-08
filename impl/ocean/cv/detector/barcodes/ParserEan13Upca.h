/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/barcodes/Barcodes.h"
#include "ocean/cv/detector/barcodes/Barcode.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Barcodes
{

/**
 * A parser for EAN-13 and UPC-A barcodes.
 * @ingroup cvdetectorbarcodes
 */
class OCEAN_CV_DETECTOR_BARCODES_EXPORT ParserEan13Upca
{
	protected:

		/// Definition of a digit pattern, i.e. any digit is encoded by the widths of two light and dark bars
		typedef std::array<uint32_t, 4> DigitPattern;

	public:

		/**
		 * Parses segment data for an EAN-13 or UPC-A barcode
		 * @param segmentData The pointer to the segment data, must be valid
		 * @param size The size of the segment data in elements, range: [59, infinity)
		 * @param barcode The resulting barcode if one has been found
		 * @param xCoordinates The resulting start and end x-coordinates of the detected barcode, the end is inclusive
		 * @return True if a barcode has been detected, otherwise false
		 * @tparam tReportUpcASeparately True to report UPC-A codes separately, otherwise UPC-A will be reported as EAN-13 (of which it is a sub-type)
		 */
		template <bool tReportUpcASeparately = true>
		static bool parse(const uint32_t* segmentData, const size_t size, Barcode& barcode, IndexPair32& xCoordinates);

	protected:

		/**
		 * Returns if the given segment data is the start of the left guard of a barcode
		 * @param segmentData The pointer to the segment data, must be valid
		 * @param size The size of the segment data in elements, range: [59, infinity)
		 * @param moduleSize The estimated module size in pixels, range: [1, infinity)
		 * @param minModuleSize The minimum size of what can be accepted as a single module, range: [1, moduleSize
		 * @param maxModuleSize The maximum size of what can be accepted as a single module, range: [moduleSize, infinity)
		 * @return True if the given segment data is the start of the left guard of a barcode, otherwise false
		 */
		static bool isLeftGuard(const uint32_t* segmentData, const size_t size, uint32_t& moduleSize, uint32_t& minModuleSize, uint32_t& maxModuleSize);

		/**
		 * Returns if the given segment data is the start of the middle guard of a barcode
		 * @param segmentData The pointer to the segment data, must be valid
		 * @param size The size of the segment data in elements, range: [59, infinity)
		 * @param minModuleSize The minimum size of what can be accepted as a single module, range: [1, maxModuleSize
		 * @param maxModuleSize The maximum size of what can be accepted as a single module, range: [minModuleSize, infinity)
		 * @return True if the given segment data is the start of the middle guard of a barcode, otherwise false
		 */
		static bool isMiddleGuard(const uint32_t* segmentData, const size_t size, const uint32_t& minModuleSize, const uint32_t& maxModuleSize);

		/**
		 * Returns if the given segment data is the start of the right guard of a barcode
		 * @param segmentData The pointer to the segment data, must be valid
		 * @param size The size of the segment data in elements, range: [59, infinity)
		 * @param minModuleSize The minimum size of what can be accepted as a single module, range: [1, maxModuleSize
		 * @param maxModuleSize The maximum size of what can be accepted as a single module, range: [minModuleSize, infinity)
		 * @return True if the given segment data is the start of the right guard of a barcode, otherwise false
		 */
		static bool isRightGuard(const uint32_t* segmentData, const size_t size, const uint32_t& minModuleSize, const uint32_t& maxModuleSize);

		/**
		 * Decodes the left and right digits of a barcode given their respective start segments
		 * @param leftDigitsSegmentData The pointer to the first element of the segment data where the left block of digits starts, must be valid
		 * @param rightDigitsSegmentData The pointer to the first element of the segment data where the right block of digits start, must be valid
		 * @param moduleSize The estimated module size in pixels, range: [1, infinity)
		 * @param minModuleSize The minimum size of what can be accepted as a single module, range: [1, moduleSize
		 * @param maxModuleSize The maximum size of what can be accepted as a single module, range: [moduleSize, infinity)
		 * @param decodedDigits The resulting, decoded digits in the order as they appear under the barcode (if printed)
		 * @return True if the digits have been decoded successfully, otherwise false
		 */
		static bool decodeDigits(const uint32_t* leftDigitsSegmentData, const uint32_t* rightDigitsSegmentData, const uint32_t moduleSize, const uint32_t& minModuleSize, const uint32_t& maxModuleSize, std::vector<uint8_t>& decodedDigits);

		/**
		 * Decodes a single digit given its segment data
		 * @param digitSegmentData The pointer to the first element of the segment data where the digits starts, must be valid
		 * @param moduleSize The estimated module size in pixels, range: [1, infinity)
		 * @param minModuleSize The minimum size of what can be accepted as a single module, range: [1, moduleSize
		 * @param maxModuleSize The maximum size of what can be accepted as a single module, range: [moduleSize, infinity)
		 * @param decodedDigit The resulting decoded digit
		 * @param parityBit The parity bit of the decoded digit from the underlying alphabet (required to decode the first digit in case of EAN-13 from the parity of the left digit block).
		 * @tparam tUseCodesLAndG Indicates the alphabet that should be used to decode this digit (left block: `true`, right block: `false`)
		 */
		template <bool tUseCodesLAndG>
		static bool decodeDigit(const uint32_t* digitSegmentData, const uint32_t moduleSize, const uint32_t& minModuleSize, const uint32_t& maxModuleSize, uint8_t& decodedDigit, unsigned int& parityBit);

		/**
		 * Computes a score for digit pattern
		 * The score indicates how well a certain digit pattern fits with a pre-defined pattern from one of the alphabets; the lower the score, the better (range: [0, infinity))
		 * @param digitSegmentData The pointer to the first element of the segment data where the digits starts, must be valid, TODO This should be a DigitPattern at some point
		 * @param digitPattern The pattern against which the segment above will be scored, must be valid
		 * @param moduleSize The estimated module size in pixels, range: [1, infinity)
		 * @param minModuleSize The minimum size of what can be accepted as a single module, range: [1, moduleSize
		 * @param maxModuleSize The maximum size of what can be accepted as a single module, range: [moduleSize, infinity)
		 * @return The value of the score
		 */
		static uint32_t computePatternScore(const uint32_t* digitSegmentData, const DigitPattern& digitPattern, const uint32_t moduleSize, const uint32_t& minModuleSize, const uint32_t& maxModuleSize);

		/**
		 * Checks the verification digits of a barcode
		 * @param barcodeDigits The pointer to the decoded digits of a barcode, must be valid
		 * @param numberDigits The number of digits provided, range: [12, 13]
		 * @return True if the verification was successful, otherwise false
		 */
		static bool verifyCheckDigit(const uint8_t* barcodeDigits, const size_t numberDigits);
};

} // namespace Barcodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
