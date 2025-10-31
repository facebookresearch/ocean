/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodeBase.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/// Forward declaration
class QRCode;

/// Definition of a vector of QR codes
using QRCodes = std::vector<QRCode>;

/**
 * Definition of a QR code
 * @ingroup cvdetectorqrcodes
 */
class QRCode final : public QRCodeBase
{
	friend class QRCodeEncoder;
	friend class QRCodeDecoder;

	public:

		/// Indicates the smallest valid version number of QR codes.
		static constexpr unsigned int MIN_VERSION = 1u;
		
		/// Indicates the largest valid version number of QR codes.
		static constexpr unsigned int MAX_VERSION = 40u;

	public:

		/**
		 * Creates an invalid QR code instance
		 */
		QRCode() = default;

		/**
		 * Returns whether this is a valid QR code instance
		 * @return True if this is a valid QR code instance, otherwise false
		 */
		inline bool isValid() const override;

		/**
		 * Returns the number of modules per side of the QR code
		 * @return The number of modules per side
		 */
		inline unsigned int modulesPerSide() const override;

		/**
		 * Computes the number of modules per side of a QR code given its version
		 * @param version The version number for which the number of modules per side will be computed, range: [MIN_VERSION, MAX_VERSION]
		 * @return The number of modules per side
		 */
		static inline unsigned int modulesPerSide(const unsigned int version);
	
	protected:

		/**
		 * Creates an QR code instance
		 * @param data The plain data of this QR code, must be valid
		 * @param encodingMode The encoding mode that was used to encode the data, must not be `EM_INVALID_ENCODING_MODE`
		 * @param errorCorrectionCapacity The error correction capacity that was used to generate this QR code, must not be `ECC_INVALID`
		 * @param modules The modules of the QR code that store the data, must be valid
		 * @param version The version of the QR code, range: [MIN_VERSION, MAX_VERSION]
		 * @sa QRCodeEncoder, QRCodeDecoder
		 */
		inline explicit QRCode(std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version);
};

inline QRCode::QRCode(std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version) :
	QRCodeBase(CT_STANDARD, std::move(data), encodingMode, errorCorrectionCapacity, std::move(modules), version)
{
	ocean_assert(isValid());
}

inline bool QRCode::isValid() const
{
	if (codeType_ != CT_STANDARD)
	{
		return false;
	}

	if (encodingMode_ == EM_INVALID_ENCODING_MODE)
	{
		return false;
	}

	if (errorCorrectionCapacity_ == ECC_INVALID || errorCorrectionCapacity_ == ECC_DETECTION_ONLY)
	{
		return false;
	}

	if (data_.empty() || modules_.empty())
	{
		return false;
	}

	if (version_ < MIN_VERSION || version_ > MAX_VERSION)
	{
		return false;
	}

	if (modules_.size() != modulesPerSide() * modulesPerSide())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

inline unsigned int QRCode::modulesPerSide() const
{
	return modulesPerSide(version());
}

inline unsigned int QRCode::modulesPerSide(const unsigned int version)
{
	if (version >= MIN_VERSION && version <= MAX_VERSION)
	{
		return 4u * version + 17u;
	}

	return 0u;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
