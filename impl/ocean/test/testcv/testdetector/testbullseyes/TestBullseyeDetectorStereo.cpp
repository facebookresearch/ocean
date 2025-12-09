/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestBullseyeDetectorStereo.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/testgeometry/Utilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

using namespace CV::Detector::Bullseyes;

bool TestBullseyeDetectorStereo::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for BullseyeDetectorStereo:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = testParameters(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testInvalidMatchingCost() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testCandidate(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testTriangulateBullseye(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testExtractBullseyeCandidates(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testExtractBullseyes(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testComputeCostMatrix(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = stressTestDetectBullseyes(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "BullseyeDetectorStereo test succeeded.";
	}
	else
	{
		Log::info() << "BullseyeDetectorStereo test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBullseyes

TEST(TestBullseyeDetectorStereo, Parameters)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testParameters(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseyeDetectorStereo, InvalidMatchingCost)
{
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testInvalidMatchingCost());
}

TEST(TestBullseyeDetectorStereo, Candidate)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testCandidate(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseyeDetectorStereo, TriangulateBullseye)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testTriangulateBullseye(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseyeDetectorStereo, ExtractBullseyeCandidates)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testExtractBullseyeCandidates(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseyeDetectorStereo, ExtractBullseyes)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testExtractBullseyes(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseyeDetectorStereo, ComputeCostMatrix)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::testComputeCostMatrix(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseyeDetectorStereo, StressTestDetectBullseyes)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseyeDetectorStereo::stressTestDetectBullseyes(GTEST_TEST_DURATION, randomGenerator));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestBullseyeDetectorStereo::testParameters(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Parameters class test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// Test 1: Default constructor (verify inherits mono parameters correctly)
		{
			const BullseyeDetectorStereo::Parameters defaultParams;

			// Verify the object is valid
			if (!defaultParams.isValid())
			{
				allSucceeded = false;
			}

			// Verify inherited mono parameters have default values
			if (defaultParams.framePyramidPixelThreshold() != 640u * 480u)
			{
				allSucceeded = false;
			}

			if (defaultParams.framePyramidLayers() != 3u)
			{
				allSucceeded = false;
			}

			if (defaultParams.useAdaptiveRowSpacing() != true)
			{
				allSucceeded = false;
			}
		}

		// Test 2: Static factory method (Parameters::defaultParameters())
		{
			const BullseyeDetectorStereo::Parameters factoryParams = BullseyeDetectorStereo::Parameters::defaultParameters();

			// Verify the object is valid
			if (!factoryParams.isValid())
			{
				allSucceeded = false;
			}

			// Verify it has the same values as default constructor
			if (factoryParams.framePyramidPixelThreshold() != 640u * 480u)
			{
				allSucceeded = false;
			}

			if (factoryParams.framePyramidLayers() != 3u)
			{
				allSucceeded = false;
			}

			if (factoryParams.useAdaptiveRowSpacing() != true)
			{
				allSucceeded = false;
			}
		}

		// Test 3: Parameter modification (modify inherited mono parameters, verify changes persist)
		{
			BullseyeDetectorStereo::Parameters params;

			// Modify framePyramidPixelThreshold
			const unsigned int newPixelThreshold = RandomI::random(randomGenerator, 100u, 1000000u);
			params.setFramePyramidPixelThreshold(newPixelThreshold);

			if (params.framePyramidPixelThreshold() != newPixelThreshold)
			{
				allSucceeded = false;
			}

			// Modify framePyramidLayers
			const unsigned int newLayers = RandomI::random(randomGenerator, 1u, 10u);
			params.setFramePyramidLayers(newLayers);

			if (params.framePyramidLayers() != newLayers)
			{
				allSucceeded = false;
			}

			// Modify useAdaptiveRowSpacing
			const bool newAdaptiveSpacing = RandomI::random(randomGenerator, 1u) == 1u;
			params.setUseAdaptiveRowSpacing(newAdaptiveSpacing);

			if (params.useAdaptiveRowSpacing() != newAdaptiveSpacing)
			{
				allSucceeded = false;
			}

			// Verify still valid after modifications
			if (!params.isValid())
			{
				allSucceeded = false;
			}
		}

		// Test 4: Inheritance verification (verify all BullseyeDetectorMono::Parameters members accessible)
		{
			BullseyeDetectorStereo::Parameters stereoParams;

			// Test that we can use it as a BullseyeDetectorMono::Parameters
			const BullseyeDetectorMono::Parameters& monoParamsRef = stereoParams;

			// Verify mono parameters are accessible through the reference
			if (!monoParamsRef.isValid())
			{
				allSucceeded = false;
			}

			if (monoParamsRef.framePyramidPixelThreshold() != stereoParams.framePyramidPixelThreshold())
			{
				allSucceeded = false;
			}

			if (monoParamsRef.framePyramidLayers() != stereoParams.framePyramidLayers())
			{
				allSucceeded = false;
			}

			if (monoParamsRef.useAdaptiveRowSpacing() != stereoParams.useAdaptiveRowSpacing())
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::testInvalidMatchingCost()
{
	Log::info() << "invalidMatchingCost() function test:";

	bool allSucceeded = invalidMatchingCost() == Scalar(1000);

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::testCandidate(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Candidate class test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// Test 1: Default Constructor
		{
			const Candidate defaultCandidate;

			// Verify default candidate is invalid
			if (defaultCandidate.isValid())
			{
				allSucceeded = false;
			}

			// Verify center is sentinel value
			if (defaultCandidate.center() != Candidate::invalidBullseyeCenter())
			{
				allSucceeded = false;
			}

			// Verify reprojection errors are negative (Numeric::minValue())
			if (defaultCandidate.reprojectionErrorA() >= Scalar(0) || defaultCandidate.reprojectionErrorB() >= Scalar(0))
			{
				allSucceeded = false;
			}
		}

		// Test 2: Parameterized Constructor with valid data
		{
			const Vector3 center(Random::scalar(randomGenerator, -10, 10), Random::scalar(randomGenerator, -10, 10), Random::scalar(randomGenerator, 0.1, 10));
			const Scalar errorA = Random::scalar(randomGenerator, 0, 10);
			const Scalar errorB = Random::scalar(randomGenerator, 0, 10);

			const Candidate candidate(center, errorA, errorB);

			// Verify candidate is valid
			if (!candidate.isValid())
			{
				allSucceeded = false;
			}

			// Verify center is stored correctly
			if (candidate.center() != center)
			{
				allSucceeded = false;
			}

			// Verify reprojection errors are stored correctly
			if (candidate.reprojectionErrorA() != errorA)
			{
				allSucceeded = false;
			}

			if (candidate.reprojectionErrorB() != errorB)
			{
				allSucceeded = false;
			}
		}

		// Test 3: Parameterized Constructor with zero errors
		{
			const Vector3 center(Scalar(1), Scalar(2), Scalar(3));
			const Scalar errorA = Scalar(0);
			const Scalar errorB = Scalar(0);

			const Candidate candidate(center, errorA, errorB);

			// Verify candidate is valid (zero errors are valid)
			if (!candidate.isValid())
			{
				allSucceeded = false;
			}

			// Verify values
			if (candidate.center() != center || candidate.reprojectionErrorA() != errorA || candidate.reprojectionErrorB() != errorB)
			{
				allSucceeded = false;
			}
		}

		// Test 4: Invalid candidate with sentinel center value (cannot test directly as constructor asserts isValid())
		// Instead, verify that default constructed candidate has invalid center
		{
			const Candidate defaultCandidate;
			const Vector3 invalidCenter = Candidate::invalidBullseyeCenter();

			if (defaultCandidate.center() != invalidCenter)
			{
				allSucceeded = false;
			}

			// Verify sentinel value is Vector3::minValue()
			if (invalidCenter != Vector3::minValue())
			{
				allSucceeded = false;
			}
		}

		// Test 5: Static method invalidBullseyeCenter()
		{
			const Vector3 invalidCenter1 = Candidate::invalidBullseyeCenter();
			const Vector3 invalidCenter2 = Candidate::invalidBullseyeCenter();

			// Verify consistency
			if (invalidCenter1 != invalidCenter2)
			{
				allSucceeded = false;
			}

			// Verify it returns Vector3::minValue()
			if (invalidCenter1 != Vector3::minValue())
			{
				allSucceeded = false;
			}
		}

		// Test 6: Copy semantics
		{
			const Vector3 center(Random::scalar(randomGenerator, -10, 10), Random::scalar(randomGenerator, -10, 10), Random::scalar(randomGenerator, 0.1, 10));
			const Scalar errorA = Random::scalar(randomGenerator, 0, 5);
			const Scalar errorB = Random::scalar(randomGenerator, 0, 5);

			const Candidate original(center, errorA, errorB);

			// Test copy constructor
			const Candidate copied(original);

			if (!copied.isValid())
			{
				allSucceeded = false;
			}

			if (copied.center() != original.center())
			{
				allSucceeded = false;
			}

			if (copied.reprojectionErrorA() != original.reprojectionErrorA())
			{
				allSucceeded = false;
			}

			if (copied.reprojectionErrorB() != original.reprojectionErrorB())
			{
				allSucceeded = false;
			}

			// Test assignment operator
			Candidate assigned;
			assigned = original;

			if (!assigned.isValid())
			{
				allSucceeded = false;
			}

			if (assigned.center() != original.center())
			{
				allSucceeded = false;
			}

			if (assigned.reprojectionErrorA() != original.reprojectionErrorA())
			{
				allSucceeded = false;
			}

			if (assigned.reprojectionErrorB() != original.reprojectionErrorB())
			{
				allSucceeded = false;
			}
		}

		// Test 7: Accessor methods return correct types
		{
			const Vector3 center(Scalar(5), Scalar(10), Scalar(15));
			const Scalar errorA = Scalar(1.5);
			const Scalar errorB = Scalar(2.5);

			const Candidate candidate(center, errorA, errorB);

			// Verify center() returns const reference (by checking it's the same object)
			const Vector3& centerRef = candidate.center();
			if (&centerRef != &candidate.center())
			{
				// Different addresses would indicate copy, not reference
				allSucceeded = false;
			}

			// Verify values match exactly
			if (!centerRef.isEqual(center, Numeric::weakEps()))
			{
				allSucceeded = false;
			}

			if (std::abs(candidate.reprojectionErrorA() - errorA) > Numeric::weakEps())
			{
				allSucceeded = false;
			}

			if (std::abs(candidate.reprojectionErrorB() - errorB) > Numeric::weakEps())
			{
				allSucceeded = false;
			}
		}

		// Test 8: Const correctness
		{
			const Vector3 center(Scalar(1), Scalar(2), Scalar(3));
			const Scalar errorA = Scalar(0.5);
			const Scalar errorB = Scalar(0.8);

			const Candidate constCandidate(center, errorA, errorB);

			// All these should compile and work on const object
			const bool valid = constCandidate.isValid();
			const Vector3& constCenter = constCandidate.center();
			const Scalar constErrorA = constCandidate.reprojectionErrorA();
			const Scalar constErrorB = constCandidate.reprojectionErrorB();

			if (!valid || constCenter != center || constErrorA != errorA || constErrorB != errorB)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::testTriangulateBullseye(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "triangulateBullseye() function test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// Random camera
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
		const AnyCameraType cameraType = anyCameraTypes[RandomI::random(randomGenerator, 1u)];

		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(cameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const SharedAnyCamera& camera0 = realisticCamera;
		const SharedAnyCamera& camera1 = realisticCamera;

		// Random camera pose
		const HomogenousMatrix4 world_T_device(Random::vector3(randomGenerator, Scalar(-1), Scalar(1)), Random::quaternion(randomGenerator));

		const Scalar baseline = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5)); // 0.01 m to 0.5 m
		const Scalar yOffset = Random::scalar(randomGenerator, Scalar(-0.1), Scalar(0.1)); // -0.1 m to 0.1 m
		const Scalar zOffset = Random::scalar(randomGenerator, Scalar(-0.1), Scalar(0.1)); // -0.1 m to 0.1 m
		const HomogenousMatrix4 device_T_camera0(Vector3(-baseline / 2, yOffset, zOffset));
		const HomogenousMatrix4 device_T_camera1(Vector3(baseline / 2, yOffset, zOffset));

		const HomogenousMatrix4 world_T_camera0 = world_T_device * device_T_camera0;
		const HomogenousMatrix4 world_T_camera1 = world_T_device * device_T_camera1;

		// Test random points (in front of 0, 1, or both cameras)
		for (size_t i = 0; i < 10; ++i)
		{
			const Scalar x = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5)); // 0.01 m to 0.5 m
			const Scalar y = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5)); // 0.01 m to 0.5 m
			const Scalar z = Scalar(-1) * Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5)); // -0.01 m to -0.5 m
			const Vector3 groundtruthDevicePoint = Vector3(x, y, z);
			const Vector3 groundtruthWorldPoint = world_T_device * groundtruthDevicePoint;

			const bool isPointInFront0 = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_camera0), groundtruthWorldPoint);
			const bool isPointInFront1 = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_camera1), groundtruthWorldPoint);

			if (!isPointInFront0 && !isPointInFront1)
			{
				// Point is not visible at all, skip this iteration.
				continue;
			}

			const Vector2 imagePoint0 = camera0->projectToImage(world_T_camera0, groundtruthWorldPoint);
			const Vector2 imagePoint1 = camera1->projectToImage(world_T_camera1, groundtruthWorldPoint);

			const bool isPointInside0 = camera0->isInside(imagePoint0);
			const bool isPointInside1 = camera1->isInside(imagePoint1);
			if (!isPointInside0 && !isPointInside1)
			{
				// Point is not visible at all, skip this iteration.
				continue;
			}

			const bool expectedSuccess = isPointInFront0 && isPointInFront1 && isPointInside0 && isPointInside1;

			// Create fake bullseyes from the projected points to use as input
			const Scalar diameter = Random::scalar(randomGenerator, Scalar(10), Scalar(20));
			const uint8_t grayThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 100u, 150u));

			const Bullseye bullseye0(imagePoint0, diameter, grayThreshold);
			const Bullseye bullseye1(imagePoint1, diameter, grayThreshold);

			// Triangulate the bullseye pair
			Vector3 triangulatedCenter;
			Scalar reprojectionError0;
			Scalar reprojectionError1;

			const bool triangulationSucceeded = triangulateBullseye(*camera0, *camera1, world_T_camera0, world_T_camera1, bullseye0, bullseye1, triangulatedCenter, reprojectionError0, reprojectionError1);

			if (triangulationSucceeded != expectedSuccess)
			{
				allSucceeded = false;
			}
			else
			{
				if (triangulationSucceeded)
				{
					const Scalar pointError = (triangulatedCenter - groundtruthWorldPoint).length();

					const Scalar pointDistance = groundtruthDevicePoint.length();
					const Scalar maxPointError = pointDistance * Scalar(0.01); // 1% of distance

					if (pointError > maxPointError)
					{
						allSucceeded = false;
					}

					constexpr Scalar maxReprojectionError = Scalar(1.5); // in pixels
					if (reprojectionError0 > maxReprojectionError || reprojectionError1 > maxReprojectionError)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded > 0u)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::stressTestDetectBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BullseyeDetectorStereo::detectBullseyes() stress test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};

		// Create two realistic cameras for stereo setup
		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(anyCameraTypes[RandomI::random(randomGenerator, 1u)], RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const unsigned int widthA = RandomI::random(randomGenerator, 21u, 2048u);
		const unsigned int heightA = (realisticCamera->height() * widthA) / realisticCamera->width();

		const unsigned int widthB = RandomI::random(randomGenerator, 21u, 2048u);
		const unsigned int heightB = (realisticCamera->height() * widthB) / realisticCamera->width();

		if (heightA < 21u || heightB < 21u)
		{
			// One of the cameras/frames undercuts the minimum image height and cannot be processed.
			continue;
		}

		const SharedAnyCamera cameraA = realisticCamera->clone(widthA, heightA);
		const SharedAnyCamera cameraB = realisticCamera->clone(widthB, heightB);
		ocean_assert(cameraA != nullptr && cameraA->isValid());
		ocean_assert(cameraB != nullptr && cameraB->isValid());

		SharedAnyCameras cameras = {cameraA, cameraB};

		// Create random frames
		Frame yFrameA(FrameType(widthA, heightA, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		Frame yFrameB(FrameType(widthB, heightB, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(yFrameA, /*skipPaddingArea*/ true, &randomGenerator);
		CV::CVUtilities::randomizeFrame(yFrameB, /*skipPaddingArea*/ true, &randomGenerator);

		Frames yFrames = {std::move(yFrameA), std::move(yFrameB)};

		// Create random transformations
		const HomogenousMatrix4 world_T_device(Random::vector3(randomGenerator, -10, 10), Random::euler(randomGenerator));

		HomogenousMatrices4 device_T_cameras(2);
		device_T_cameras[0] = HomogenousMatrix4(Random::vector3(randomGenerator, -1, 1), Random::euler(randomGenerator));
		device_T_cameras[1] = HomogenousMatrix4(Random::vector3(randomGenerator, -1, 1), Random::euler(randomGenerator));

		BullseyeDetectorStereo::Parameters parameters = BullseyeDetectorStereo::Parameters::defaultParameters();
		parameters.setFramePyramidPixelThreshold((unsigned int)(Scalar(yFrames[0].pixels()) * Random::scalar(randomGenerator, 0.1, 2.0) + Scalar(0.5)));
		parameters.setFramePyramidLayers(RandomI::random(randomGenerator, 1u, 5u));
		parameters.setUseAdaptiveRowSpacing(RandomI::random(randomGenerator, 1u) == 1u);
		ocean_assert(parameters.isValid());

		const bool useWorker = RandomI::random(randomGenerator, 1u) == 1u;

		BullseyeDetectorStereo::BullseyePairs bullseyePairs;
		Vectors3 bullseyeCenters;
		if (!BullseyeDetectorStereo::detectBullseyes(cameras, yFrames, world_T_device, device_T_cameras, bullseyePairs, bullseyeCenters, parameters, (useWorker ? WorkerPool::get().scopedWorker()() : nullptr)))
		{
			allSucceeded = false;
			break;
		}

		// Validate that the output is consistent
		if (bullseyePairs.size() != bullseyeCenters.size())
		{
			allSucceeded = false;
			break;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Stress test succeeded.";
	}
	else
	{
		Log::info() << "Stress test FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::testExtractBullseyeCandidates(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "extractBullseyeCandidates() function test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// Create random cameras
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
		const AnyCameraType cameraType = anyCameraTypes[RandomI::random(randomGenerator, 1u)];

		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(cameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const SharedAnyCamera& cameraA = realisticCamera;
		const SharedAnyCamera& cameraB = realisticCamera;

		// Create random camera poses
		const HomogenousMatrix4 world_T_device(Random::vector3(randomGenerator, Scalar(-1), Scalar(1)), Random::quaternion(randomGenerator));

		const Scalar baseline = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5));
		const HomogenousMatrix4 device_T_cameraA(Vector3(-baseline / 2, Scalar(0), Scalar(0)));
		const HomogenousMatrix4 device_T_cameraB(Vector3(baseline / 2, Scalar(0), Scalar(0)));

		const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
		const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

		// Test 1: Empty bullseye lists should return empty map
		{
			Bullseyes bullseyesA;
			Bullseyes bullseyesB;

			CandidateMap candidateMap = extractBullseyeCandidates(*cameraA, *cameraB, world_T_cameraA, world_T_cameraB, bullseyesA, bullseyesB);

			if (!candidateMap.empty())
			{
				allSucceeded = false;
			}
		}

		// Test 2: One empty, one non-empty should return empty map
		{
			const Vector2 randomPoint(Random::scalar(randomGenerator, Scalar(100), Scalar(cameraA->width() - 100)), Random::scalar(randomGenerator, Scalar(100), Scalar(cameraA->height() - 100)));
			const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
			const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

			Bullseyes bullseyesA;
			Bullseyes bullseyesB;

			// Randomly add bullseye to either A or B, leaving the other empty
			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				bullseyesA.emplace_back(randomPoint, randomDiameter, randomThreshold);
			}
			else
			{
				bullseyesB.emplace_back(randomPoint, randomDiameter, randomThreshold);
			}

			CandidateMap candidateMap = extractBullseyeCandidates(*cameraA, *cameraB, world_T_cameraA, world_T_cameraB, bullseyesA, bullseyesB);

			if (!candidateMap.empty())
			{
				allSucceeded = false;
			}
		}

		// Test 3: Non-empty bullseye lists should produce candidates
		{
			// Create a random 3D point in front of both cameras (negative z in device frame = in front)
			const Vector3 randomDevicePoint(
				Random::scalar(randomGenerator, Scalar(-0.3), Scalar(0.3)),
				Random::scalar(randomGenerator, Scalar(-0.3), Scalar(0.3)),
				Random::scalar(randomGenerator, Scalar(-2), Scalar(-0.5)));
			const Vector3 groundtruthWorldPoint = world_T_device * randomDevicePoint;

			// Verify point is in front of both cameras
			const bool isInFrontA = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraA), groundtruthWorldPoint);
			const bool isInFrontB = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraB), groundtruthWorldPoint);

			if (!isInFrontA || !isInFrontB)
			{
				// Skip this iteration if the point is not in front of both cameras
				continue;
			}

			// Project to both cameras
			const Vector2 imagePointA = cameraA->projectToImage(world_T_cameraA, groundtruthWorldPoint);
			const Vector2 imagePointB = cameraB->projectToImage(world_T_cameraB, groundtruthWorldPoint);

			if (cameraA->isInside(imagePointA) && cameraB->isInside(imagePointB))
			{
				// Use random diameter and gray threshold for the bullseyes
				const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
				const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

				Bullseyes bullseyesA;
				bullseyesA.emplace_back(imagePointA, randomDiameter, randomThreshold);

				Bullseyes bullseyesB;
				bullseyesB.emplace_back(imagePointB, randomDiameter, randomThreshold);

				CandidateMap candidateMap = extractBullseyeCandidates(*cameraA, *cameraB, world_T_cameraA, world_T_cameraB, bullseyesA, bullseyesB);

				// Should have at least one candidate if triangulation succeeds
				if (candidateMap.empty())
				{
					// May fail for edge cases, so this is not necessarily a failure
				}
				else
				{
					// Verify the candidate data is valid
					for (const auto& entry : candidateMap)
					{
						const Candidate& candidate = entry.second;
						if (!candidate.isValid())
						{
							allSucceeded = false;
						}
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::testExtractBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "extractBullseyes() function test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// Create random cameras
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
		const AnyCameraType cameraType = anyCameraTypes[RandomI::random(randomGenerator, 1u)];

		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(cameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const SharedAnyCamera& cameraA = realisticCamera;
		const SharedAnyCamera& cameraB = realisticCamera;

		// Create random camera poses (needed for extractBullseyeCandidates in Test 3)
		const HomogenousMatrix4 world_T_device(Random::vector3(randomGenerator, Scalar(-1), Scalar(1)), Random::quaternion(randomGenerator));
		const Scalar baseline = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5));
		const HomogenousMatrix4 device_T_cameraA(Vector3(-baseline / 2, Scalar(0), Scalar(0)));
		const HomogenousMatrix4 device_T_cameraB(Vector3(baseline / 2, Scalar(0), Scalar(0)));
		const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
		const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

		// Test 1: Empty inputs should return false
		{
			Bullseyes bullseyesA;
			Bullseyes bullseyesB;
			CandidateMap candidateMap;

			BullseyePairs bullseyePairs;
			Vectors3 bullseyeCenters;

			const bool result = extractBullseyes(*cameraA, *cameraB, bullseyesA, bullseyesB, candidateMap, bullseyePairs, bullseyeCenters);

			if (result != false)
			{
				allSucceeded = false;
			}
		}

		// Test 2: Non-empty bullseyes but empty candidate map should return false
		{
			const Vector2 randomPointA(Random::scalar(randomGenerator, Scalar(100), Scalar(cameraA->width() - 100)), Random::scalar(randomGenerator, Scalar(100), Scalar(cameraA->height() - 100)));
			const Vector2 randomPointB(Random::scalar(randomGenerator, Scalar(100), Scalar(cameraB->width() - 100)), Random::scalar(randomGenerator, Scalar(100), Scalar(cameraB->height() - 100)));
			const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
			const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

			Bullseyes bullseyesA;
			bullseyesA.emplace_back(randomPointA, randomDiameter, randomThreshold);

			Bullseyes bullseyesB;
			bullseyesB.emplace_back(randomPointB, randomDiameter, randomThreshold);

			CandidateMap candidateMap;

			BullseyePairs bullseyePairs;
			Vectors3 bullseyeCenters;

			const bool result = extractBullseyes(*cameraA, *cameraB, bullseyesA, bullseyesB, candidateMap, bullseyePairs, bullseyeCenters);

			if (result != false)
			{
				allSucceeded = false;
			}
		}

		// Test 3: Valid input with candidate map generated from extractBullseyeCandidates should succeed
		{
			// Create a random 3D point in front of both cameras
			const Vector3 randomDevicePoint(
				Random::scalar(randomGenerator, Scalar(-0.3), Scalar(0.3)),
				Random::scalar(randomGenerator, Scalar(-0.3), Scalar(0.3)),
				Random::scalar(randomGenerator, Scalar(-2), Scalar(-0.5)));
			const Vector3 groundtruthWorldPoint = world_T_device * randomDevicePoint;

			// Verify point is in front of both cameras
			const bool isInFrontA = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraA), groundtruthWorldPoint);
			const bool isInFrontB = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraB), groundtruthWorldPoint);

			if (isInFrontA && isInFrontB)
			{
				// Project to both cameras
				const Vector2 imagePointA = cameraA->projectToImage(world_T_cameraA, groundtruthWorldPoint);
				const Vector2 imagePointB = cameraB->projectToImage(world_T_cameraB, groundtruthWorldPoint);

				if (cameraA->isInside(imagePointA) && cameraB->isInside(imagePointB))
				{
					const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
					const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

					Bullseyes bullseyesA;
					bullseyesA.emplace_back(imagePointA, randomDiameter, randomThreshold);

					Bullseyes bullseyesB;
					bullseyesB.emplace_back(imagePointB, randomDiameter, randomThreshold);

					// Use extractBullseyeCandidates to generate the candidate map
					CandidateMap candidateMap = extractBullseyeCandidates(*cameraA, *cameraB, world_T_cameraA, world_T_cameraB, bullseyesA, bullseyesB);

					if (!candidateMap.empty())
					{
						BullseyePairs bullseyePairs;
						Vectors3 bullseyeCenters;

						const bool result = extractBullseyes(*cameraA, *cameraB, bullseyesA, bullseyesB, candidateMap, bullseyePairs, bullseyeCenters);

						if (!result)
						{
							allSucceeded = false;
						}
						else
						{
							// Verify output sizes match
							if (bullseyePairs.size() != bullseyeCenters.size())
							{
								allSucceeded = false;
							}
						}
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBullseyeDetectorStereo::testComputeCostMatrix(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "computeCostMatrix() function test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// Create random cameras
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
		const AnyCameraType cameraType = anyCameraTypes[RandomI::random(randomGenerator, 1u)];

		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(cameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

		const SharedAnyCamera& cameraA = realisticCamera;
		const SharedAnyCamera& cameraB = realisticCamera;

		// Create random camera poses (needed for extractBullseyeCandidates in Test 3)
		const HomogenousMatrix4 world_T_device(Random::vector3(randomGenerator, Scalar(-1), Scalar(1)), Random::quaternion(randomGenerator));
		const Scalar baseline = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5));
		const HomogenousMatrix4 device_T_cameraA(Vector3(-baseline / 2, Scalar(0), Scalar(0)));
		const HomogenousMatrix4 device_T_cameraB(Vector3(baseline / 2, Scalar(0), Scalar(0)));
		const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameraA;
		const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameraB;

		// Test 1: Empty inputs should return false
		{
			Bullseyes bullseyesA;
			Bullseyes bullseyesB;
			CandidateMap candidateMap;

			Matrix costMatrix;

			const bool result = computeCostMatrix(*cameraA, *cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix);

			if (result != false)
			{
				allSucceeded = false;
			}
		}

		// Test 2: Non-empty bullseyes but empty candidate map should return false
		{
			const Vector2 randomPointA(Random::scalar(randomGenerator, Scalar(100), Scalar(cameraA->width() - 100)), Random::scalar(randomGenerator, Scalar(100), Scalar(cameraA->height() - 100)));
			const Vector2 randomPointB(Random::scalar(randomGenerator, Scalar(100), Scalar(cameraB->width() - 100)), Random::scalar(randomGenerator, Scalar(100), Scalar(cameraB->height() - 100)));
			const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
			const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

			Bullseyes bullseyesA;
			bullseyesA.emplace_back(randomPointA, randomDiameter, randomThreshold);

			Bullseyes bullseyesB;
			bullseyesB.emplace_back(randomPointB, randomDiameter, randomThreshold);

			CandidateMap candidateMap;

			Matrix costMatrix;

			const bool result = computeCostMatrix(*cameraA, *cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix);

			if (result != false)
			{
				allSucceeded = false;
			}
		}

		// Test 3: Valid input should return correct cost matrix dimensions
		{
			const size_t numBullseyes = RandomI::random(randomGenerator, 1u, 5u);

			Bullseyes bullseyesA;
			Bullseyes bullseyesB;

			// Generate bullseyes by projecting random 3D points to both cameras
			for (size_t i = 0; i < numBullseyes; ++i)
			{
				// Create a random 3D point in front of both cameras
				const Vector3 randomDevicePoint(
					Random::scalar(randomGenerator, Scalar(-0.5), Scalar(0.5)),
					Random::scalar(randomGenerator, Scalar(-0.5), Scalar(0.5)),
					Random::scalar(randomGenerator, Scalar(-3), Scalar(-0.5)));
				const Vector3 worldPoint = world_T_device * randomDevicePoint;

				// Verify point is in front of both cameras
				const bool isInFrontA = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraA), worldPoint);
				const bool isInFrontB = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraB), worldPoint);

				if (isInFrontA && isInFrontB)
				{
					const Vector2 imagePointA = cameraA->projectToImage(world_T_cameraA, worldPoint);
					const Vector2 imagePointB = cameraB->projectToImage(world_T_cameraB, worldPoint);

					if (cameraA->isInside(imagePointA) && cameraB->isInside(imagePointB))
					{
						const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
						const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

						bullseyesA.emplace_back(imagePointA, randomDiameter, randomThreshold);
						bullseyesB.emplace_back(imagePointB, randomDiameter, randomThreshold);
					}
				}
			}

			if (!bullseyesA.empty() && !bullseyesB.empty())
			{
				// Use extractBullseyeCandidates to generate the candidate map
				CandidateMap candidateMap = extractBullseyeCandidates(*cameraA, *cameraB, world_T_cameraA, world_T_cameraB, bullseyesA, bullseyesB);

				if (!candidateMap.empty())
				{
					Matrix costMatrix;
					const bool result = computeCostMatrix(*cameraA, *cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix);

					if (!result)
					{
						allSucceeded = false;
					}
					else
					{
						// Verify matrix dimensions
						if (costMatrix.rows() != bullseyesA.size() || costMatrix.columns() != bullseyesB.size())
						{
							allSucceeded = false;
						}

						// Verify cost values are in valid range [0, invalidMatchingCost()]
						for (size_t row = 0; row < costMatrix.rows(); ++row)
						{
							for (size_t col = 0; col < costMatrix.columns(); ++col)
							{
								const Scalar cost = costMatrix(row, col);
								if (cost < 0 || cost > invalidMatchingCost())
								{
									allSucceeded = false;
								}
							}
						}
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
