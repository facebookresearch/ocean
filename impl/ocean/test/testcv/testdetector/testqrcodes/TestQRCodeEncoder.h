// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

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

	protected:

		/// Handle for test images, first: filename, second: Everstore handle
		typedef std::pair<std::string, std::string> HandlePair;

		/// Vector of handle pairs
		typedef std::vector<HandlePair> HandlePairs;

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
				inline QRCodeVerificationItem();

				/**
				 * Constructor
				 * @param version The version of the QR code
				 * @param errorCorrectionCapacity The level of error correction that the QR code has
				 * @param maskingPattern The masking pattern that was used to generate the QR code
				 * @param message The raw input message that was used to generate the QR code
				 * @param modules The modules of a QR code as a string ("1", "0" only), must have `(4 * version + 17)^2` elements
				 * @sa loadCSVEverstoreTestQRCodeEncoding(), loadCSVTestQRCodeEncoding()
				 */
				inline QRCodeVerificationItem(const unsigned int version, const CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const CV::Detector::QRCodes::QRCodeEncoder::MaskingPattern maskingPattern, const std::string& message, const std::string& modules);

			public:

				/// Version number
				unsigned int version_;

				/// Error correction capacity
				CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity_;

				/// Masking pattern
				CV::Detector::QRCodes::QRCodeEncoder::MaskingPattern maskingPattern_;

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
		 * Provides verification data for the QR code encoding test
		 * @return The verification data, the source is either 1. Everstore, 2. a file, or 3. a minimal dataset
		 */
		static QRCodeVerificationItems loadDataTestQRCodeEncoding();

		/**
		 * Loads the verification data for the QR code encoding test from a CSV file
		 * For details checkout the Dropbox folder:
		 *
		 *   https://fburl.com/ocean_everstore_test_data (and then: qrcode/encoding_decoding_data/\*.csv)
		 *
		 * @return The verification data
		 */
		static QRCodeVerificationItems loadCSVTestQRCodeEncoding(const HandlePair& handlePair);

#ifdef OCEAN_ENABLED_EVERSTORE_CLIENT

		/**
		 * Loads the verification data for the QR code encoding test from directly from Everstore
		 * For details checkout the Dropbox folder:
		 *
		 *   https://fburl.com/ocean_everstore_test_data (and then: qrcode/encoding_decoding_data/\*.csv)
		 *
		 * @return The verification data
		 */
		static QRCodeVerificationItems loadCSVEverstoreTestQRCodeEncoding(const HandlePair& handlePair);

#endif // OCEAN_ENABLED_EVERSTORE_CLIENT

		/**
		 * Converts a line from a CSV file into a helper data structure that is subsequently used for testing
		 * @param lineCSV A line for a CSV file, cf. `loadDataTestQRCodeEncoding()`
		 * @param qrcodeVerificationItem The converted line from a CSV that will be stored here
		 * @return True on success, otherwise false
		 */
		static bool convertCSVToQRCodeVerificationItem(const std::string& lineCSV, QRCodeVerificationItem& qrcodeVerificationItem);

		/**
		 * Returns the pair of filename and Everstore handle for the QR code encoding test
		 * @return The handle pair
		 */
		static HandlePair handlePairTestQRCodeEncoding();

		/**
		 * Return the absolute path to the directory containing the test data
		 * Note: in order to use this function you will have define the macro `RUN_TESTS_WITH_FILES_ACCESS` and to set the path in the implementation to your copy of
		 *
 		 *   https://fburl.com/ocean_everstore_test_data (and then: qrcode/encoding_decoding_data/)
 		 *
		 * @return The absolute path of the data directory
		 */
		static std::string testDataDirectory();
};


inline TestQRCodeEncoder::QRCodeVerificationItem::QRCodeVerificationItem()
{
	// Nothing else to do.
}

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

} // namespace Test
