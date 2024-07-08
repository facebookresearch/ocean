/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/barcodes/Barcodes.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Barcodes
{

/// Forward declaration
class Barcode;

/// Definition of a vector of barcodes
typedef std::vector<Barcode> Barcodes;

/**
 * Definition of barcode types
 * @ingroup cvdetectorbarcodes
 */
enum class BarcodeType : uint32_t
{
	/// An invalid barcode type
	INVALID = 0u,
	/// A barcode of type EAN-13
	EAN_13 = 1u << 0u,
	/// A barcode of type UPC-A
	UPC_A = 1u << 1u,
};

/// Definition of a set of barcode types
typedef std::unordered_set<BarcodeType> BarcodeTypeSet;

/**
 * Definition of a barcode
 * @ingroup cvdetectorbarcodes
 */
class Barcode
{
	public:

		/**
		 * Creates an invalid barcode instance
		 */
		Barcode() = default;

		/**
		 * Creates a barcode object
		 * @param barcodeType The type of the barcode, must not be `BarcodeType::INVALID`
		 * @param data The data that will be stored for this barcode, must not be empty
		 */
		inline Barcode(const BarcodeType barcodeType, const std::string& data);

		/**
		 * Creates a barcode object
		 * @param barcodeType The type of the barcode, must not be `BarcodeType::INVALID`
		 * @param data The data that will be stored for this barcode, must not be empty
		 */
		inline Barcode(const BarcodeType barcodeType, std::string&& data);

		/**
		 * Returns whether this is a valid barcode instance
		 * @return True if this is a valid barcode instance, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Returns the type of this barcode
		 * @return The type of this barcode
		 */
		inline BarcodeType barcodeType() const;

		/**
		 * Returns the payload of this barcode
		 * @return The payload
		 */
		inline const std::string& data() const;

		/**
		 * Translates a barcode type into a human-readable string
		 * @param barcodeType The barcode type that will be converted into a string
		 * @return The translated barcode type
		 */
		static inline std::string translateBarcodeType(const BarcodeType barcodeType);

	protected:

		/// The type of this barcode
		BarcodeType barcodeType_ = BarcodeType::INVALID;

		/// The payload of barcode
		std::string data_;
};

inline Barcode::Barcode(const BarcodeType barcodeType, const std::string& data) :
	barcodeType_(barcodeType),
	data_(data)
{
	ocean_assert(barcodeType_ != BarcodeType::INVALID);
	ocean_assert(!data_.empty());
}

inline Barcode::Barcode(const BarcodeType barcodeType, std::string&& data) :
	barcodeType_(barcodeType),
	data_(std::move(data))
{
	ocean_assert(barcodeType_ != BarcodeType::INVALID);
	ocean_assert(!data_.empty());
}

inline bool Barcode::isValid() const
{
	return barcodeType() != BarcodeType::INVALID;
}

inline BarcodeType Barcode::barcodeType() const
{
	return barcodeType_;
}

inline const std::string& Barcode::data() const
{
	return data_;
}

inline std::string Barcode::translateBarcodeType(const BarcodeType barcodeType)
{
	switch (barcodeType)
	{
		case BarcodeType::INVALID:
			return "INVALID";

		case BarcodeType::EAN_13:
			return "EAN-13";

		case BarcodeType::UPC_A:
			return "UPC-A";
	}

	ocean_assert(false && "Never be here!");
	return "Unknown barcode type";
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
