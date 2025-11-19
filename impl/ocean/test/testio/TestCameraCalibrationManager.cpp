/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestCameraCalibrationManager.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestCameraCalibrationManager::test()
{
	Log::info() << "Camera Calibration Manager test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testBasicRegistration() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCameraLookup() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAliasLookup() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testResolutionMatching() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPriorityHandling() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCameraModels() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testErrorHandling() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDeviceContext() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDeviceContextHierarchy() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDeviceContextIsolation() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire Camera Calibration Manager test succeeded.";
	}
	else
	{
		Log::info() << "Camera Calibration Manager test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCameraCalibrationManager, BasicRegistration)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testBasicRegistration());
}

TEST(TestCameraCalibrationManager, CameraLookup)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testCameraLookup());
}

TEST(TestCameraCalibrationManager, AliasLookup)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testAliasLookup());
}

TEST(TestCameraCalibrationManager, ResolutionMatching)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testResolutionMatching());
}

TEST(TestCameraCalibrationManager, PriorityHandling)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testPriorityHandling());
}

TEST(TestCameraCalibrationManager, CameraModels)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testCameraModels());
}

TEST(TestCameraCalibrationManager, ErrorHandling)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testErrorHandling());
}

TEST(TestCameraCalibrationManager, DeviceContext)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testDeviceContext());
}

TEST(TestCameraCalibrationManager, DeviceContextHierarchy)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testDeviceContextHierarchy());
}

TEST(TestCameraCalibrationManager, DeviceContextIsolation)
{
	EXPECT_TRUE(TestCameraCalibrationManager::testDeviceContextIsolation());
}

#endif // OCEAN_USE_GTEST

bool TestCameraCalibrationManager::testBasicRegistration()
{
	Log::info() << "Basic registration test:";

	Validation validation;

	{
		// Test registering a simple Pinhole camera from JSON string
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Test Camera",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"configuration": "8_PARAMETERS",
							"parameters": [500.0, 500.0, 320.0, 240.0, 0.0, 0.0, 0.0, 0.0]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		OCEAN_EXPECT_TRUE(validation, registered);

		if (registered)
		{
			IO::CameraCalibrationManager::CalibrationQuality quality = IO::CameraCalibrationManager::CQ_UNKNOWN;
			SharedAnyCamera camera = manager.camera("Test Camera", 640, 480, &quality);

			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
			OCEAN_EXPECT_TRUE(validation, camera->isValid());
			OCEAN_EXPECT_EQUAL(validation, quality, IO::CameraCalibrationManager::CQ_EXACT);

			if (camera && camera->isValid())
			{
				OCEAN_EXPECT_EQUAL(validation, camera->width(), 640u);
				OCEAN_EXPECT_EQUAL(validation, camera->height(), 480u);
			}
		}
	}

	{
		// Test registering multiple cameras
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Camera 1",
					"calibrations": [
						{
							"resolution": {"width": 1920, "height": 1080},
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				},
				{
					"name": "Camera 2",
					"calibrations": [
						{
							"resolution": {"width": 1280, "height": 720},
							"model": "Ocean Pinhole",
							"fovx": 1.2
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		OCEAN_EXPECT_TRUE(validation, registered);

		if (registered)
		{
			SharedAnyCamera camera1 = manager.camera("Camera 1", 1920, 1080);
			SharedAnyCamera camera2 = manager.camera("Camera 2", 1280, 720);

			OCEAN_EXPECT_TRUE(validation, camera1 != nullptr);
			OCEAN_EXPECT_TRUE(validation, camera2 != nullptr);

			if (camera1)
			{
				OCEAN_EXPECT_EQUAL(validation, camera1->width(), 1920u);
				OCEAN_EXPECT_EQUAL(validation, camera1->height(), 1080u);
			}

			if (camera2)
			{
				OCEAN_EXPECT_EQUAL(validation, camera2->width(), 1280u);
				OCEAN_EXPECT_EQUAL(validation, camera2->height(), 720u);
			}
		}
	}

	{
		// Test registering camera with multiple resolutions
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Multi-Res Camera",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"fovx": 0.9
						},
						{
							"resolution": {"width": 1280, "height": 720},
							"model": "Ocean Pinhole",
							"fovx": 0.9
						},
						{
							"resolution": {"width": 1920, "height": 1080},
							"model": "Ocean Pinhole",
							"fovx": 0.9
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		OCEAN_EXPECT_TRUE(validation, registered);

		if (registered)
		{
			SharedAnyCamera camera1 = manager.camera("Multi-Res Camera", 640, 480);
			SharedAnyCamera camera2 = manager.camera("Multi-Res Camera", 1280, 720);
			SharedAnyCamera camera3 = manager.camera("Multi-Res Camera", 1920, 1080);

			OCEAN_EXPECT_TRUE(validation, camera1 != nullptr && camera1->width() == 640u);
			OCEAN_EXPECT_TRUE(validation, camera2 != nullptr && camera2->width() == 1280u);
			OCEAN_EXPECT_TRUE(validation, camera3 != nullptr && camera3->width() == 1920u);
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testCameraLookup()
{
	Log::info() << "Camera lookup test:";

	Validation validation;

	{
		// Test basic lookup by name
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Logitech C920",
					"calibrations": [
						{
							"resolution": {"width": 1920, "height": 1080},
							"model": "Ocean Pinhole",
							"configuration": "8_PARAMETERS",
							"parameters": [1000.0, 1000.0, 960.0, 540.0, 0.01, -0.02, 0.0, 0.0]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		SharedAnyCamera camera = manager.camera("Logitech C920", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);

		if (camera && camera->isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, camera->width(), 1920u);
			OCEAN_EXPECT_EQUAL(validation, camera->height(), 1080u);
		}
	}

	{
		// Test lookup with non-existent camera
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Camera A",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		SharedAnyCamera camera = manager.camera("Camera B", 640, 480);
		OCEAN_EXPECT_TRUE(validation, camera == nullptr);
	}

	{
		// Test lookup with non-existent resolution
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Fixed Resolution Camera",
					"calibrations": [
						{
							"resolution": {"width": 1920, "height": 1080},
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Requesting a different resolution with different aspect ratio should fail
		SharedAnyCamera camera = manager.camera("Fixed Resolution Camera", 640, 480);
		OCEAN_EXPECT_TRUE(validation, camera == nullptr);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testAliasLookup()
{
	Log::info() << "Alias lookup test:";

	Validation validation;

	{
		// Test lookup using alias
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Samsung S21 Camera",
					"aliases": ["S21 Main", "Galaxy S21 Camera"],
					"calibrations": [
						{
							"resolution": {"width": 1920, "height": 1080},
							"model": "Ocean Pinhole",
							"fovx": 1.1
						}
					]
				}
			]
		})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			// Lookup by original name
			SharedAnyCamera camera1 = manager.camera("Samsung S21 Camera", 1920, 1080);
			OCEAN_EXPECT_TRUE(validation, camera1 != nullptr);

			// Lookup by first alias
			SharedAnyCamera camera2 = manager.camera("S21 Main", 1920, 1080);
			OCEAN_EXPECT_TRUE(validation, camera2 != nullptr);

			// Lookup by second alias
			SharedAnyCamera camera3 = manager.camera("Galaxy S21 Camera", 1920, 1080);
			OCEAN_EXPECT_TRUE(validation, camera3 != nullptr);

			// All should return valid cameras with same resolution
			if (camera1 && camera2 && camera3)
			{
				OCEAN_EXPECT_EQUAL(validation, camera1->width(), 1920u);
				OCEAN_EXPECT_EQUAL(validation, camera2->width(), 1920u);
				OCEAN_EXPECT_EQUAL(validation, camera3->width(), 1920u);
			}
		}

		{
			// Test camera with no aliases
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Simple Camera",
						"calibrations": [
							{
								"resolution": {"width": 640, "height": 480},
								"model": "Ocean Pinhole",
								"fovx": 1.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Simple Camera", 640, 480);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
		}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testResolutionMatching()
{
	Log::info() << "Resolution matching test:";

	Validation validation;

		{
			// Test exact resolution match
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Test Camera",
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"fovx": 1.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			IO::CameraCalibrationManager::CalibrationQuality quality = IO::CameraCalibrationManager::CQ_UNKNOWN;
			SharedAnyCamera camera = manager.camera("Test Camera", 1920, 1080, &quality);

			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
			OCEAN_EXPECT_EQUAL(validation, quality, IO::CameraCalibrationManager::CQ_EXACT);
		}

		{
			// Test interpolated resolution match (same aspect ratio)
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Scalable Camera",
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"fovx": 1.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			// Request half resolution with same aspect ratio (16:9)
			IO::CameraCalibrationManager::CalibrationQuality quality = IO::CameraCalibrationManager::CQ_UNKNOWN;
			SharedAnyCamera camera = manager.camera("Scalable Camera", 960, 540, &quality);

			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
			OCEAN_EXPECT_EQUAL(validation, quality, IO::CameraCalibrationManager::CQ_INTERPOLATED);

			if (camera)
			{
				OCEAN_EXPECT_EQUAL(validation, camera->width(), 960u);
				OCEAN_EXPECT_EQUAL(validation, camera->height(), 540u);
			}
		}

		{
			// Test no match for different aspect ratio
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "16:9 Camera",
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"fovx": 1.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			// Request 4:3 aspect ratio - should not match
			SharedAnyCamera camera = manager.camera("16:9 Camera", 640, 480);
			OCEAN_EXPECT_TRUE(validation, camera == nullptr);
		}

		{
			// Test multiple resolutions - exact match preferred
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Multi Camera",
						"calibrations": [
							{
								"resolution": {"width": 640, "height": 480},
								"model": "Ocean Pinhole",
								"fovx": 0.8
							},
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"fovx": 1.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			// Request exact match for 640x480
			IO::CameraCalibrationManager::CalibrationQuality quality = IO::CameraCalibrationManager::CQ_UNKNOWN;
			SharedAnyCamera camera = manager.camera("Multi Camera", 640, 480, &quality);

			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
			OCEAN_EXPECT_EQUAL(validation, quality, IO::CameraCalibrationManager::CQ_EXACT);
		}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testPriorityHandling()
{
	Log::info() << "Priority handling test:";

	Validation validation;

		{
			// Test that higher priority calibration is selected
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Dual Priority Camera",
						"priority": 5,
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"configuration": "4_PARAMETERS",
								"parameters": [1000.0, 1000.0, 960.0, 540.0]
							}
						]
					},
					{
						"name": "Dual Priority Camera",
						"priority": 10,
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"configuration": "8_PARAMETERS",
								"parameters": [1100.0, 1100.0, 960.0, 540.0, 0.01, -0.02, 0.0, 0.0]
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Dual Priority Camera", 1920, 1080);

			OCEAN_EXPECT_TRUE(validation, camera != nullptr);

			// The higher priority (10) should be selected
			// We can verify by checking that camera exists and is valid
			if (camera && camera->isValid())
			{
				OCEAN_EXPECT_EQUAL(validation, camera->width(), 1920u);
				OCEAN_EXPECT_EQUAL(validation, camera->height(), 1080u);
			}
		}

		{
			// Test default priority (0)
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Default Priority Camera",
						"calibrations": [
							{
								"resolution": {"width": 640, "height": 480},
								"model": "Ocean Pinhole",
								"fovx": 1.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Default Priority Camera", 640, 480);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
		}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testCameraModels()
{
	Log::info() << "Camera models test:";

	Validation validation;

		{
			// Test Ocean Pinhole with 4 parameters
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Pinhole 4 Params",
						"calibrations": [
							{
								"resolution": {"width": 640, "height": 480},
								"model": "Ocean Pinhole",
								"configuration": "4_PARAMETERS",
								"parameters": [500.0, 500.0, 320.0, 240.0]
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Pinhole 4 Params", 640, 480);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);

			if (camera)
			{
				OCEAN_EXPECT_EQUAL(validation, camera->width(), 640u);
				OCEAN_EXPECT_EQUAL(validation, camera->height(), 480u);
			}
		}

		{
			// Test Ocean Pinhole with 8 parameters (with distortion)
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Pinhole 8 Params",
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Pinhole",
								"configuration": "8_PARAMETERS",
								"parameters": [1000.0, 1000.0, 960.0, 540.0, 0.05, -0.1, 0.001, -0.001]
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Pinhole 8 Params", 1920, 1080);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
		}

		{
			// Test Ocean Pinhole with fovx (field of view)
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Pinhole FOV",
						"calibrations": [
							{
								"resolution": {"width": 1280, "height": 720},
								"model": "Ocean Pinhole",
								"fovx": 1.0471975512
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Pinhole FOV", 1280, 720);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
		}

		{
			// Test Ocean Fisheye with 12 parameters
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Fisheye Camera",
						"calibrations": [
							{
								"resolution": {"width": 1920, "height": 1080},
								"model": "Ocean Fisheye",
								"configuration": "12_PARAMETERS",
								"parameters": [600.0, 600.0, 960.0, 540.0, 0.1, 0.5, -1.0, 2.0, -1.5, 0.5, 0.001, -0.001]
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Fisheye Camera", 1920, 1080);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);

			if (camera)
			{
				OCEAN_EXPECT_EQUAL(validation, camera->width(), 1920u);
				OCEAN_EXPECT_EQUAL(validation, camera->height(), 1080u);
			}
		}

		{
			// Test Ocean Fisheye with fovx
			const std::string jsonData = R"(
			{
				"cameras": [
					{
						"name": "Fisheye FOV",
						"calibrations": [
							{
								"resolution": {"width": 640, "height": 480},
								"model": "Ocean Fisheye",
								"fovx": 2.0
							}
						]
					}
				]
			})";

			TestableCalibrationManager manager;
			OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

			SharedAnyCamera camera = manager.camera("Fisheye FOV", 640, 480);
			OCEAN_EXPECT_TRUE(validation, camera != nullptr);
		}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testErrorHandling()
{
	Log::info() << "Error handling test:";

	Validation validation;

	{
		// Test invalid JSON
		const std::string jsonData = "{ invalid json }";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test empty JSON object
		const std::string jsonData = "{}";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		// Should fail because no cameras array present
		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test missing camera name
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		// Should fail because no valid camera was registered
		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test missing calibrations array
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "No Calibrations Camera"
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		// Should fail because no valid camera was registered
		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test invalid camera model
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Invalid Model Camera",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Unknown Camera Model",
							"fovx": 1.0
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		// Should fail because no valid camera was registered
		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test missing resolution
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "No Resolution Camera",
					"calibrations": [
						{
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		// Should fail because no valid camera was registered
		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test invalid parameter count
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Wrong Param Count",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"configuration": "8_PARAMETERS",
							"parameters": [500.0, 500.0, 320.0]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(jsonData.c_str(), jsonData.size());

		// Should fail because no valid camera was registered
		OCEAN_EXPECT_FALSE(validation, registered);
	}

	{
		// Test null buffer
		TestableCalibrationManager manager;
		const bool registered = manager.registerCalibrations(nullptr, 0);

		OCEAN_EXPECT_FALSE(validation, registered);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testDeviceContext()
{
	Log::info() << "Device context test:";

	Validation validation;

	{
		// Test setting device product
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set device product context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Samsung S21 5G"));

		// Should find camera with simple name
		SharedAnyCamera camera = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);

		if (camera)
		{
			OCEAN_EXPECT_EQUAL(validation, camera->width(), 1920u);
			OCEAN_EXPECT_EQUAL(validation, camera->height(), 1080u);
		}
	}

	{
		// Test setting device version
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"configuration": "8_PARAMETERS",
									"parameters": [1100.0, 1100.0, 960.0, 540.0, 0.01, -0.02, 0.0, 0.0]
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set device version context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceVersion("SM-G991U"));

		// Should find camera
		SharedAnyCamera camera = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);
	}

	{
		// Test setting device serial
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"serial": "ABC123456",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.1
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set device serial context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceSerial("ABC123456"));

		// Should find camera
		SharedAnyCamera camera = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);
	}

	{
		// Test clearing device context
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Generic Webcam",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Should find global camera without context
		SharedAnyCamera camera1 = manager.camera("Generic Webcam", 640, 480);
		OCEAN_EXPECT_TRUE(validation, camera1 != nullptr);

		// Set device context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Some Product"));

		// Should not find global camera with device context
		SharedAnyCamera camera2 = manager.camera("Generic Webcam", 640, 480);
		OCEAN_EXPECT_TRUE(validation, camera2 == nullptr);

		// Clear context
		manager.clearDeviceContext();

		// Should find global camera again
		SharedAnyCamera camera3 = manager.camera("Generic Webcam", 640, 480);
		OCEAN_EXPECT_TRUE(validation, camera3 != nullptr);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testDeviceContextHierarchy()
{
	Log::info() << "Device context hierarchy test:";

	Validation validation;

	{
		// Test that product context only matches product-level cameras
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"priority": 5,
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 0.9
								}
							]
						}
					]
				},
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"priority": 10,
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set product context - should only see product-level camera (priority 5)
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Samsung S21 5G"));

		SharedAnyCamera camera = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);

		// The product-level camera should be returned (not the version-level one)
		// We can't directly verify the priority, but we verify a camera was found
	}

	{
		// Test that version context only matches version-level cameras (not serial)
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"priority": 8,
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				},
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"serial": "ABC123",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"priority": 15,
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.1
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set version context - should only see version-level camera (priority 8, not serial)
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceVersion("SM-G991U"));

		SharedAnyCamera camera = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);
	}

	{
		// Test that serial context matches serial-level cameras
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"priority": 8,
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				},
				{
					"product": "Samsung S21 5G",
					"version": "SM-G991U",
					"serial": "XYZ789",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"priority": 20,
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.2
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set serial context - should only see serial-level camera
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceSerial("XYZ789"));

		SharedAnyCamera camera = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera != nullptr);

		// Try different serial - should not find camera
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceSerial("DIFFERENT"));
		SharedAnyCamera camera2 = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, camera2 == nullptr);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestCameraCalibrationManager::testDeviceContextIsolation()
{
	Log::info() << "Device context isolation test:";

	Validation validation;

	{
		// Test that global cameras are not accessible with device context set
		const std::string jsonData = R"(
		{
			"cameras": [
				{
					"name": "Generic Webcam",
					"calibrations": [
						{
							"resolution": {"width": 640, "height": 480},
							"model": "Ocean Pinhole",
							"fovx": 1.0
						}
					]
				}
			],
			"devices": [
				{
					"product": "Samsung S21 5G",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Without context, can access global camera but not device camera
		SharedAnyCamera globalCam = manager.camera("Generic Webcam", 640, 480);
		OCEAN_EXPECT_TRUE(validation, globalCam != nullptr);

		SharedAnyCamera deviceCam1 = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, deviceCam1 == nullptr);

		// With device context, can access device camera but not global camera
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Samsung S21 5G"));

		SharedAnyCamera globalCam2 = manager.camera("Generic Webcam", 640, 480);
		OCEAN_EXPECT_TRUE(validation, globalCam2 == nullptr);

		SharedAnyCamera deviceCam2 = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, deviceCam2 != nullptr);
	}

	{
		// Test switching between different device contexts
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Samsung S21 5G",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				},
				{
					"product": "iPhone 13 Pro",
					"cameras": [
						{
							"name": "Back-facing Camera 0",
							"calibrations": [
								{
									"resolution": {"width": 1920, "height": 1080},
									"model": "Ocean Pinhole",
									"fovx": 1.1
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Set Samsung context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Samsung S21 5G"));
		SharedAnyCamera samsungCam = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, samsungCam != nullptr);

		// Switch to iPhone context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("iPhone 13 Pro"));
		SharedAnyCamera iphoneCam = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, iphoneCam != nullptr);

		// Set non-existent context
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Non-existent Product"));
		SharedAnyCamera noCam = manager.camera("Back-facing Camera 0", 1920, 1080);
		OCEAN_EXPECT_TRUE(validation, noCam == nullptr);
	}

	{
		// Test that cameras from different hierarchy levels don't interfere
		const std::string jsonData = R"(
		{
			"devices": [
				{
					"product": "Test Product",
					"cameras": [
						{
							"name": "Camera A",
							"calibrations": [
								{
									"resolution": {"width": 640, "height": 480},
									"model": "Ocean Pinhole",
									"fovx": 0.9
								}
							]
						}
					]
				},
				{
					"product": "Test Product",
					"version": "V1",
					"cameras": [
						{
							"name": "Camera B",
							"calibrations": [
								{
									"resolution": {"width": 640, "height": 480},
									"model": "Ocean Pinhole",
									"fovx": 1.0
								}
							]
						}
					]
				},
				{
					"product": "Test Product",
					"version": "V1",
					"serial": "S123",
					"cameras": [
						{
							"name": "Camera C",
							"calibrations": [
								{
									"resolution": {"width": 640, "height": 480},
									"model": "Ocean Pinhole",
									"fovx": 1.1
								}
							]
						}
					]
				}
			]
		})";

		TestableCalibrationManager manager;
		OCEAN_EXPECT_TRUE(validation, manager.registerCalibrations(jsonData.c_str(), jsonData.size()));

		// Product context: should see Camera A, not B or C
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceProduct("Test Product"));
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera A", 640, 480) != nullptr);
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera B", 640, 480) == nullptr);
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera C", 640, 480) == nullptr);

		// Version context: should see Camera B, not A or C
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceVersion("V1"));
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera A", 640, 480) == nullptr);
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera B", 640, 480) != nullptr);
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera C", 640, 480) == nullptr);

		// Serial context: should see Camera C, not A or B
		OCEAN_EXPECT_TRUE(validation, manager.setDeviceSerial("S123"));
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera A", 640, 480) == nullptr);
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera B", 640, 480) == nullptr);
		OCEAN_EXPECT_TRUE(validation, manager.camera("Camera C", 640, 480) != nullptr);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

} // namespace TestIO

} // namespace Test

} // namespace Ocean
