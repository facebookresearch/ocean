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
class MicroQRCode;

/// Definition of a vector of Micro QR codes
typedef std::vector<MicroQRCode> MicroQRCodes;

/**
 * Definition of a Micro QR code
 * @ingroup cvdetectorqrcodes
 */
class MicroQRCode final : public QRCodeBase
{
	friend class MicroQRCodeEncoder;

	public:

		/// Indicates the smallest valid version number of Micro QR codes.
		static constexpr unsigned int MIN_VERSION = 1u;
		
		/// Indicates the largest valid version number of Micro QR codes.
		static constexpr unsigned int MAX_VERSION = 4u;

	public:

		/**
		 * Creates an invalid Micro QR code instance
		 */
		MicroQRCode() = default;

		/**
		 * Returns whether this is a valid QR code instance
		 * @return True if this is a valid QR code instance, otherwise false
		 */
		inline bool isValid() const override;

		/**
		 * Returns the version of the Micro QR code as a string
		 * @return The version string
		 */
		inline std::string versionString() const override;

		/**
		 * Returns the number of modules per side of the Micro QR code
		 * @return The number of modules per side
		 */
		inline unsigned int modulesPerSide() const override;

		/**
		 * Computes the number of modules per side of a Micro QR code given its version
		 * @param version The version number for which the number of modules per side will be computed, range: [MIN_VERSION, MAX_VERSION]
		 * @return The number of modules per side
		 */
		static inline unsigned int modulesPerSide(const unsigned int version);
	
	protected:

		/**
		 * Creates a Micro QR code instance
		 * @param data The plain data of this QR code, must be valid
		 * @param encodingMode The encoding mode that was used to encode the data, must not be `EM_INVALID_ENCODING_MODE`
		 * @param errorCorrectionCapacity The error correction capacity that was used to generate this Micro QR code, must not be `ECC_INVALID`
		 * @param modules The modules of the Micro QR code that store the data, must be valid
		 * @param version The version of the Micro QR code, range: [MIN_VERSION, MAX_VERSION]
		 * @sa MicroQRCodeEncoder, QRCodeDecoder
		 */
		inline explicit MicroQRCode(std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version);
};

inline MicroQRCode::MicroQRCode(std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version) :
	QRCodeBase(CT_MICRO, std::move(data), encodingMode, errorCorrectionCapacity, std::move(modules), version)
{
	ocean_assert(isValid());
}

inline bool MicroQRCode::isValid() const
{
	if (codeType_ != CT_MICRO)
	{
		return false;
	}

	if (encodingMode_ == EM_INVALID_ENCODING_MODE || errorCorrectionCapacity_ == ECC_INVALID)
	{
		return false;
	}

	if (data_.empty() || modules_.empty())
	{
		return false;
	}

	// Version and version-specific format checks
	if (version_ == 1u && encodingMode_ != EM_NUMERIC)
	{
		return false;
	}

	if (version_ == 2u && encodingMode_ != EM_NUMERIC && encodingMode_ != EM_ALPHANUMERIC)
	{
		return false;
	}

	if (encodingMode_ != EM_NUMERIC && encodingMode_ != EM_ALPHANUMERIC && encodingMode_ != EM_BYTE)
	{
		return false;
	}

	if (errorCorrectionCapacity_ == ECC_DETECTION_ONLY && version_ != 1u)
	{
		return false;
	}

	if (errorCorrectionCapacity_ == ECC_25 && version_ != 4u)
	{
		return false;
	}

	if (errorCorrectionCapacity_ == ECC_30)
	{
		return false;
	}

	if (version_ < MIN_VERSION || version_ > MAX_VERSION)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (modules_.size() != modulesPerSide() * modulesPerSide())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

inline std::string MicroQRCode::versionString() const
{
	return "M" + QRCodeBase::versionString();
}

inline unsigned int MicroQRCode::modulesPerSide() const
{
	return modulesPerSide(version());
}

inline unsigned int MicroQRCode::modulesPerSide(const unsigned int version)
{
	if (version >= MIN_VERSION && version <= MAX_VERSION)
	{
		return 2u * version + 9u;
	}

	return 0u;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
