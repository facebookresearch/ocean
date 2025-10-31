/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"

#include "ocean/test/TestDataCollection.h"
#include "ocean/test/TestDataManager.h"

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
 * This class implements tests for the Micro QR code features
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestMicroQRCodeEncoder
{
	friend class TestMicroQRCodeDecoder;

	public:

		/**
		 * This class implements a test data collection based on a local file.
		 */
		class FileDataCollection : public TestDataCollection
		{
			public:

				/**
				 * Creates a new test data collection object.
				 * @param filename The filename of the file containing the test data
				 */
				explicit FileDataCollection(const std::string& filename);

				/**
				 * Returns the test data object associated with a specified index.
				 * @see TestDataCollection::data().
				 */
				SharedTestData data(const size_t index) override;

				/**
				 * Returns the number of data object objects this collection holds.
				 * @see TestDataCollection::size().
				 */
				size_t size() override;

			protected:

				/// The filename of the test data belonging to this test collection.
				std::string filename_;
		};

	protected:

		/**
		 * Definition of a helper data structure that's used to verify the Micro QR code encoding functionality
		 * This will contain the raw message and the values of corresponding Micro QR code that were generated with a reference Micro QR code generator and which are assumed to be correct.
		 */
		class MicroQRCodeVerificationItem
		{
			public:

				/**
				 * Constructor - creates an invalid instance
				 */
				MicroQRCodeVerificationItem() = default;

				/**
				 * Constructor
				 * @param version The version of the Micro QR code
				 * @param errorCorrectionCapacity The level of error correction that the Micro QR code has
				 * @param maskingPattern The masking pattern that was used to generate the Micro QR code
				 * @param message The raw input message that was used to generate the Micro QR code
				 * @param modules The modules of a Micro QR code as a string ("1", "0" only), must have `(2 * version + 9)^2` elements
				 * @sa loadCSVTestMicroQRCodeEncoding()
				 */
				inline MicroQRCodeVerificationItem(const unsigned int version, const CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const CV::Detector::QRCodes::MicroQRCodeEncoder::MaskingPattern maskingPattern, const std::string& message, const std::string& modules);

			public:

				/// Version number
				unsigned int version_ = 0u;

				/// Error correction capacity
				CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity_ = CV::Detector::QRCodes::QRCode::ECC_INVALID;

				/// Masking pattern
				CV::Detector::QRCodes::MicroQRCodeEncoder::MaskingPattern maskingPattern_ =CV::Detector::QRCodes::MicroQRCodeEncoder::MP_PATTERN_UNKNOWN;

				/// The raw message
				std::string message_;

				/// The modules (bit matrix)
				std::string modules_;
		};

		/// Vector of verification items
		using MicroQRCodeVerificationItems = std::vector<MicroQRCodeVerificationItem>;

	public:

		/**
		 * Tests the Micro QR code functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests encoding (generation) of Micro QR codes
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @return True, if succeeded
		 */
		static bool testMicroQRCodeEncoding(const double testDuration);

		/**
		 * Tests the encoding/decoding of the format information
		 * @return True, if succeeded
		 */
		static bool testMicroQRCodeFormatEncodingDecoding();

	protected:

		/**
		 * Provides verification data for the Micro QR code encoding test.
		 * The source is either a test data collection, or a minimal dataset
		 * @return The verification data, empty if the data could not be loaded
		 */
		static MicroQRCodeVerificationItems loadDataTestMicroQRCodeEncoding();

		/**
		 * Loads the verification data for the Micro QR code encoding test from a buffer containing a CSV file.
		 * @param buffer The pointer to the buffer, must be valid
		 * @param size The size of the buffer, in bytes, with range [1, infinity)
		 * @return The verification data
		 */
		static MicroQRCodeVerificationItems loadCSVTestMicroQRCodeEncoding(const void* buffer, const size_t size);

		/**
		 * Converts a line from a CSV file into a helper data structure that is subsequently used for testing
		 * @param lineCSV A line for a CSV file, cf. `loadDataTestMicroQRCodeEncoding()`
		 * @param qrcodeVerificationItem The converted line from a CSV that will be stored here
		 * @return True on success, otherwise false
		 */
		static bool convertCSVToMicroQRCodeVerificationItem(const std::string& lineCSV, MicroQRCodeVerificationItem& qrcodeVerificationItem);
};

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

/**
 * Registers the data collections for the MicroQRCodeEncoder test.
 * @return The scoped subscription for the registered data collection
 */
TestDataManager::ScopedSubscription TestMicroQRCodeEncoder_registerTestDataCollection();

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

inline TestMicroQRCodeEncoder::MicroQRCodeVerificationItem::MicroQRCodeVerificationItem(const unsigned int version, const CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const CV::Detector::QRCodes::MicroQRCodeEncoder::MaskingPattern maskingPattern, const std::string& message, const std::string& modules) :
	version_(version),
	errorCorrectionCapacity_(errorCorrectionCapacity),
	maskingPattern_(maskingPattern),
	message_(message),
	modules_(modules)
{
	// Nothing else to do.
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
