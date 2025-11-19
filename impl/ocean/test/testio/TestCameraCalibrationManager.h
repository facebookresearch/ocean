/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_CAMERA_CALIBRATION_MANAGER_H
#define META_OCEAN_TEST_TESTIO_TEST_CAMERA_CALIBRATION_MANAGER_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/io/CameraCalibrationManager.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements a camera calibration manager test.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestCameraCalibrationManager
{
	protected:

		/**
		 * Helper class that allows creating instances of CameraCalibrationManager for testing.
		 * This class derives from CameraCalibrationManager to access the protected constructor.
		 */
		class TestableCalibrationManager : public IO::CameraCalibrationManager
		{
			public:

				/**
				 * Default constructor.
				 */
				TestableCalibrationManager() = default;
		};

	public:

		/**
		 * Tests all camera calibration manager functions.
		 * @return True, if succeeded
		 */
		static bool test();

		/**
		 * Tests basic camera registration from JSON string.
		 * @return True, if succeeded
		 */
		static bool testBasicRegistration();

		/**
		 * Tests camera lookup by name.
		 * @return True, if succeeded
		 */
		static bool testCameraLookup();

		/**
		 * Tests camera lookup using aliases.
		 * @return True, if succeeded
		 */
		static bool testAliasLookup();

		/**
		 * Tests exact and interpolated resolution matching.
		 * @return True, if succeeded
		 */
		static bool testResolutionMatching();

		/**
		 * Tests priority handling when multiple calibrations are available.
		 * @return True, if succeeded
		 */
		static bool testPriorityHandling();

		/**
		 * Tests different camera models (Pinhole and Fisheye).
		 * @return True, if succeeded
		 */
		static bool testCameraModels();

		/**
		 * Tests error handling with invalid JSON and missing data.
		 * @return True, if succeeded
		 */
		static bool testErrorHandling();

		/**
		 * Tests basic device context functionality (product, version, serial).
		 * @return True, if succeeded
		 */
		static bool testDeviceContext();

		/**
		 * Tests hierarchical device context matching.
		 * @return True, if succeeded
		 */
		static bool testDeviceContextHierarchy();

		/**
		 * Tests context isolation (global vs device-specific cameras).
		 * @return True, if succeeded
		 */
		static bool testDeviceContextIsolation();
};

} // namespace TestIO

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTIO_TEST_CAMERA_CALIBRATION_MANAGER_H
