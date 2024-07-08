/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"

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
 * This class implements a test for the QR code detector.
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestLegacyQRCodeDetector2D : protected CV::Detector::QRCodes::LegacyQRCodeDetector2D
{
	public:

		/**
		 * This class implements a test data collection based on local files.
		 */
		class FileDataCollection : public TestDataCollection
		{
			public:

				/**
				 * Creates a new test data collection object.
				 * @param imageFilenames The filenames of all local images which will be part of the test collection
				 */
				explicit FileDataCollection(std::vector<std::string>&& imageFilenames);

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

				/// The filenames of all local images belonging to this test collection.
				std::vector<std::string> filenames_;
		};

	public:

		/**
		 * Invokes all test for the QR code detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test with random input data just ensuring that the detector does not crash.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration, Worker& worker);

		/**
		 * Tests the detection of QR codes in images without QR codes (avoid false-positive detections).
		 * The images needs to be provided via a test data collection with name "legacyqrCodedetector2d_0code".
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param forceFullTest If true, the test will ignore the test duration and will continue until all images have been tested, otherwise test success/failure will be determined after the test duration has been reached.
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodes_0_qrcodes(const double testDuration, Worker& worker, const bool forceFullTest);

		/**
		 * Tests the detection of QR codes in images.
		 * The images needs to be provided via a test data collection with name "legacyqrCodedetector2d_1code".
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param forceFullTest If true, the test will ignore the test duration and will continue until all images have been tested, otherwise test success/failure will be determined after the test duration has been reached.
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodes_1_qrcode(const double testDuration, Worker& worker, const bool forceFullTest);
};

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

/**
 * Registers the data collections for the LegacyQRCodeDetector2D test.
 * @return The scoped subscriptions for the registered data collections
 */
TestDataManager::ScopedSubscriptions TestLegacyQRCodeDetector2D_registerTestDataCollections();

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
