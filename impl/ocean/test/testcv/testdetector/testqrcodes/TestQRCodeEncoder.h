/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

#include "ocean/test/TestDataCollection.h"
#include "ocean/test/TestDataManager.h"

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
	#ifndef OCEAN_USE_TEST_DATA_COLLECTION
		#define OCEAN_USE_TEST_DATA_COLLECTION
	#endif
#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

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
 * This class implements tests for the QR code features
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestQRCodeEncoder
{
	friend class TestQRCodeDecoder;

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
		 * Definition of a helper data structure that's used to verify the QR code encoding functionality
		 * This will contain the raw message and the values of corresponding QR code that were generated with a reference QR code generator and which are assumed to be correct.
		 */
		class QRCodeVerificationItem
		{
			public:

				/**
				 * Constructor - creates an invalid instance
				 */
				QRCodeVerificationItem() = default;

				/**
				 * Constructor
				 * @param version The version of the QR code
				 * @param errorCorrectionCapacity The level of error correction that the QR code has
				 * @param maskingPattern The masking pattern that was used to generate the QR code
				 * @param message The raw input message that was used to generate the QR code
				 * @param modules The modules of a QR code as a string ("1", "0" only), must have `(4 * version + 17)^2` elements
				 * @sa loadCSVTestQRCodeEncoding()
				 */
				inline QRCodeVerificationItem(const unsigned int version, const CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const CV::Detector::QRCodes::QRCodeEncoder::MaskingPattern maskingPattern, const std::string& message, const std::string& modules);

			public:

				/// Version number
				unsigned int version_ = 0u;

				/// Error correction capacity
				CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity_ = CV::Detector::QRCodes::QRCode::ECC_INVALID;

				/// Masking pattern
				CV::Detector::QRCodes::QRCodeEncoder::MaskingPattern maskingPattern_ =CV::Detector::QRCodes::QRCodeEncoder::MP_PATTERN_UNKNOWN;

				/// The raw message
				std::string message_;

				/// The modules (bit matrix)
				std::string modules_;
		};

		/// Vector of verification items
		typedef std::vector<QRCodeVerificationItem> QRCodeVerificationItems;

	public:

		/**
		 * Tests the QR code functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests encoding (generation) of QR codes
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @return True, if succeeded
		 */
		static bool testQRCodeEncoding(const double testDuration);

		/**
		 * Tests the encoding/decoding of the version information
		 * @return True, if succeeded
		 */
		static bool testQRCodeVersionEncodingDecoding();

		/**
		 * Tests the encoding/decoding of the format information
		 * @return True, if succeeded
		 */
		static bool testQRCodeFormatEncodingDecoding();

	protected:

		/**
		 * Provides verification data for the QR code encoding test.
		 * The source is either a test data collection, or a minimal dataset
		 * @return The verification data, empty if the data could not be loaded
		 */
		static QRCodeVerificationItems loadDataTestQRCodeEncoding();

		/**
		 * Loads the verification data for the QR code encoding test from a buffer containing a CSV file.
		 * @param buffer The pointer to the buffer, must be valid
		 * @param size The size of the buffer, in bytes, with range [1, infinity)
		 * @return The verification data
		 */
		static QRCodeVerificationItems loadCSVTestQRCodeEncoding(const void* buffer, const size_t size);

		/**
		 * Converts a line from a CSV file into a helper data structure that is subsequently used for testing
		 * @param lineCSV A line for a CSV file, cf. `loadDataTestQRCodeEncoding()`
		 * @param qrcodeVerificationItem The converted line from a CSV that will be stored here
		 * @return True on success, otherwise false
		 */
		static bool convertCSVToQRCodeVerificationItem(const std::string& lineCSV, QRCodeVerificationItem& qrcodeVerificationItem);
};

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

/**
 * Registers the data collections for the QRCodeEncoder test.
 * @return The scoped subscription for the registered data collection
 */
TestDataManager::ScopedSubscription TestQRCodeEncoder_registerTestDataCollection();

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

inline TestQRCodeEncoder::QRCodeVerificationItem::QRCodeVerificationItem(const unsigned int version, const CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const CV::Detector::QRCodes::QRCodeEncoder::MaskingPattern maskingPattern, const std::string& message, const std::string& modules) :
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
