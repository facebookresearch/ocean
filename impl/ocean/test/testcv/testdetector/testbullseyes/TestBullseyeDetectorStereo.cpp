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

#include "ocean/test/Validation.h"

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

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test 1: Default constructor (verify inherits mono parameters correctly)
		{
			const BullseyeDetectorStereo::Parameters defaultParams;

			// Verify the object is valid
			OCEAN_EXPECT_TRUE(validation, defaultParams.isValid());

			// Verify inherited mono parameters have default values
			OCEAN_EXPECT_EQUAL(validation, defaultParams.framePyramidPixelThreshold(), 640u * 480u);

			OCEAN_EXPECT_EQUAL(validation, defaultParams.framePyramidLayers(), 3u);

			OCEAN_EXPECT_TRUE(validation, defaultParams.useAdaptiveRowSpacing());
		}

		// Test 2: Static factory method (Parameters::defaultParameters())
		{
			const BullseyeDetectorStereo::Parameters factoryParams = BullseyeDetectorStereo::Parameters::defaultParameters();

			// Verify the object is valid
			OCEAN_EXPECT_TRUE(validation, factoryParams.isValid());

			// Verify it has the same values as default constructor
			OCEAN_EXPECT_EQUAL(validation, factoryParams.framePyramidPixelThreshold(), 640u * 480u);

			OCEAN_EXPECT_EQUAL(validation, factoryParams.framePyramidLayers(), 3u);

			OCEAN_EXPECT_TRUE(validation, factoryParams.useAdaptiveRowSpacing());
		}

		// Test 3: Parameter modification (modify inherited mono parameters, verify changes persist)
		{
			BullseyeDetectorStereo::Parameters params;

			// Modify framePyramidPixelThreshold
			const unsigned int newPixelThreshold = RandomI::random(randomGenerator, 100u, 1000000u);
			params.setFramePyramidPixelThreshold(newPixelThreshold);

			OCEAN_EXPECT_EQUAL(validation, params.framePyramidPixelThreshold(), newPixelThreshold);

			// Modify framePyramidLayers
			const unsigned int newLayers = RandomI::random(randomGenerator, 1u, 10u);
			params.setFramePyramidLayers(newLayers);

			OCEAN_EXPECT_EQUAL(validation, params.framePyramidLayers(), newLayers);

			// Modify useAdaptiveRowSpacing
			const bool newAdaptiveSpacing = RandomI::boolean(randomGenerator);
			params.setUseAdaptiveRowSpacing(newAdaptiveSpacing);

			OCEAN_EXPECT_EQUAL(validation, params.useAdaptiveRowSpacing(), newAdaptiveSpacing);

			// Verify still valid after modifications
			OCEAN_EXPECT_TRUE(validation, params.isValid());
		}

		// Test 4: Inheritance verification (verify all BullseyeDetectorMono::Parameters members accessible)
		{
			BullseyeDetectorStereo::Parameters stereoParams;

			// Test that we can use it as a BullseyeDetectorMono::Parameters
			const BullseyeDetectorMono::Parameters& monoParamsRef = stereoParams;

			// Verify mono parameters are accessible through the reference
			OCEAN_EXPECT_TRUE(validation, monoParamsRef.isValid());

			OCEAN_EXPECT_EQUAL(validation, monoParamsRef.framePyramidPixelThreshold(), stereoParams.framePyramidPixelThreshold());

			OCEAN_EXPECT_EQUAL(validation, monoParamsRef.framePyramidLayers(), stereoParams.framePyramidLayers());

			OCEAN_EXPECT_EQUAL(validation, monoParamsRef.useAdaptiveRowSpacing(), stereoParams.useAdaptiveRowSpacing());
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::testInvalidMatchingCost()
{
	Log::info() << "invalidMatchingCost() function test:";

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, invalidMatchingCost(), Scalar(1000));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::testCandidate(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Candidate class test:";

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test 1: Default Constructor
		{
			const Candidate defaultCandidate;

			// Verify default candidate is invalid
			OCEAN_EXPECT_FALSE(validation, defaultCandidate.isValid());

			// Verify center is sentinel value
			OCEAN_EXPECT_EQUAL(validation, defaultCandidate.center(), Candidate::invalidBullseyeCenter());

			// Verify reprojection errors are negative (Numeric::minValue())
			OCEAN_EXPECT_LESS(validation, defaultCandidate.reprojectionErrorA(), Scalar(0));
			OCEAN_EXPECT_LESS(validation, defaultCandidate.reprojectionErrorB(), Scalar(0));
		}

		// Test 2: Parameterized Constructor with valid data
		{
			const Vector3 center = Random::vector3(randomGenerator, Scalar(-10), Scalar(10), Scalar(-10), Scalar(10), Scalar(0.1), Scalar(10));
			const Scalar errorA = Random::scalar(randomGenerator, 0, 10);
			const Scalar errorB = Random::scalar(randomGenerator, 0, 10);

			const Candidate candidate(center, errorA, errorB);

			// Verify candidate is valid
			OCEAN_EXPECT_TRUE(validation, candidate.isValid());

			// Verify center is stored correctly
			OCEAN_EXPECT_EQUAL(validation, candidate.center(), center);

			// Verify reprojection errors are stored correctly
			OCEAN_EXPECT_EQUAL(validation, candidate.reprojectionErrorA(), errorA);

			OCEAN_EXPECT_EQUAL(validation, candidate.reprojectionErrorB(), errorB);
		}

		// Test 3: Parameterized Constructor with zero errors
		{
			const Vector3 center(Scalar(1), Scalar(2), Scalar(3));
			const Scalar errorA = Scalar(0);
			const Scalar errorB = Scalar(0);

			const Candidate candidate(center, errorA, errorB);

			// Verify candidate is valid (zero errors are valid)
			OCEAN_EXPECT_TRUE(validation, candidate.isValid());

			// Verify values
			OCEAN_EXPECT_EQUAL(validation, candidate.center(), center);
			OCEAN_EXPECT_EQUAL(validation, candidate.reprojectionErrorA(), errorA);
			OCEAN_EXPECT_EQUAL(validation, candidate.reprojectionErrorB(), errorB);
		}

		// Test 4: Invalid candidate with sentinel center value (cannot test directly as constructor asserts isValid())
		// Instead, verify that default constructed candidate has invalid center
		{
			const Candidate defaultCandidate;
			const Vector3 invalidCenter = Candidate::invalidBullseyeCenter();

			OCEAN_EXPECT_EQUAL(validation, defaultCandidate.center(), invalidCenter);

			// Verify sentinel value is Vector3::minValue()
			OCEAN_EXPECT_EQUAL(validation, invalidCenter, Vector3::minValue());
		}

		// Test 5: Static method invalidBullseyeCenter()
		{
			const Vector3 invalidCenter1 = Candidate::invalidBullseyeCenter();
			const Vector3 invalidCenter2 = Candidate::invalidBullseyeCenter();

			// Verify consistency
			OCEAN_EXPECT_EQUAL(validation, invalidCenter1, invalidCenter2);

			// Verify it returns Vector3::minValue()
			OCEAN_EXPECT_EQUAL(validation, invalidCenter1, Vector3::minValue());
		}

		// Test 6: Copy semantics
		{
			const Vector3 center = Random::vector3(randomGenerator, Scalar(-10), Scalar(10), Scalar(-10), Scalar(10), Scalar(0.1), Scalar(10));
			const Scalar errorA = Random::scalar(randomGenerator, 0, 5);
			const Scalar errorB = Random::scalar(randomGenerator, 0, 5);

			const Candidate original(center, errorA, errorB);

			// Test copy constructor
			const Candidate copied(original);

			OCEAN_EXPECT_TRUE(validation, copied.isValid());

			OCEAN_EXPECT_EQUAL(validation, copied.center(), original.center());

			OCEAN_EXPECT_EQUAL(validation, copied.reprojectionErrorA(), original.reprojectionErrorA());

			OCEAN_EXPECT_EQUAL(validation, copied.reprojectionErrorB(), original.reprojectionErrorB());

			// Test assignment operator
			Candidate assigned;
			assigned = original;

			OCEAN_EXPECT_TRUE(validation, assigned.isValid());

			OCEAN_EXPECT_EQUAL(validation, assigned.center(), original.center());

			OCEAN_EXPECT_EQUAL(validation, assigned.reprojectionErrorA(), original.reprojectionErrorA());

			OCEAN_EXPECT_EQUAL(validation, assigned.reprojectionErrorB(), original.reprojectionErrorB());
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
				OCEAN_SET_FAILED(validation);
			}

			// Verify values match exactly
			OCEAN_EXPECT_TRUE(validation, centerRef.isEqual(center, Numeric::weakEps()));

			OCEAN_EXPECT_LESS_EQUAL(validation, std::abs(candidate.reprojectionErrorA() - errorA), Numeric::weakEps());

			OCEAN_EXPECT_LESS_EQUAL(validation, std::abs(candidate.reprojectionErrorB() - errorB), Numeric::weakEps());
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

			OCEAN_EXPECT_TRUE(validation, valid);
			OCEAN_EXPECT_EQUAL(validation, constCenter, center);
			OCEAN_EXPECT_EQUAL(validation, constErrorA, errorA);
			OCEAN_EXPECT_EQUAL(validation, constErrorB, errorB);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::testTriangulateBullseye(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "triangulateBullseye() function test:";

	Validation validation(randomGenerator);

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

		// Random camera pose - uses y/z offsets for more varied stereo configurations
		const Vector3 randomTranslation = Random::vector3(randomGenerator, Scalar(-1), Scalar(1));
		const Quaternion randomRotation = Random::quaternion(randomGenerator);
		const HomogenousMatrix4 world_T_device(randomTranslation, randomRotation);

		const Scalar baseline = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5));
		const Scalar yOffset = Random::scalar(randomGenerator, Scalar(-0.1), Scalar(0.1));
		const Scalar zOffset = Random::scalar(randomGenerator, Scalar(-0.1), Scalar(0.1));
		const HomogenousMatrix4 device_T_camera0(Vector3(-baseline / 2, yOffset, zOffset));
		const HomogenousMatrix4 device_T_camera1(Vector3(baseline / 2, yOffset, zOffset));

		const HomogenousMatrix4 world_T_camera0 = world_T_device * device_T_camera0;
		const HomogenousMatrix4 world_T_camera1 = world_T_device * device_T_camera1;

		// Test random points (in front of 0, 1, or both cameras)
		for (size_t i = 0; i < 10; ++i)
		{
			const Vector3 groundtruthDevicePoint = Random::vector3(randomGenerator, Scalar(0.1), Scalar(0.5), Scalar(0.1), Scalar(0.5), Scalar(-0.5), Scalar(-0.1));
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
			const Bullseye bullseye0 = createRandomBullseyeAtPosition(randomGenerator, imagePoint0);
			const Bullseye bullseye1 = createRandomBullseyeAtPosition(randomGenerator, imagePoint1);

			// Triangulate the bullseye pair
			Vector3 triangulatedCenter;
			Scalar reprojectionError0;
			Scalar reprojectionError1;

			const bool triangulationSucceeded = triangulateBullseye(*camera0, *camera1, world_T_camera0, world_T_camera1, bullseye0, bullseye1, triangulatedCenter, reprojectionError0, reprojectionError1);

			if (triangulationSucceeded != expectedSuccess)
			{
				OCEAN_SET_FAILED(validation);
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
						OCEAN_SET_FAILED(validation);
					}

					constexpr Scalar maxReprojectionError = Scalar(1.5); // in pixels
					if (reprojectionError0 > maxReprojectionError || reprojectionError1 > maxReprojectionError)
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::stressTestDetectBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "BullseyeDetectorStereo::detectBullseyes() stress test:";

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};

		// Create two realistic cameras for stereo setup
		const AnyCameraType cameraType = anyCameraTypes[RandomI::random(randomGenerator, 1u)];
		const unsigned int cameraIndex = RandomI::random(randomGenerator, 1u);
		const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(cameraType, cameraIndex);
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
		const Vector3 randomTranslation = Random::vector3(randomGenerator, -10, 10);
		const Euler randomEuler = Random::euler(randomGenerator);
		const HomogenousMatrix4 world_T_device(randomTranslation, randomEuler);

		HomogenousMatrices4 device_T_cameras(2);
		const Vector3 randomTranslation0 = Random::vector3(randomGenerator, -1, 1);
		const Euler randomEuler0 = Random::euler(randomGenerator);
		device_T_cameras[0] = HomogenousMatrix4(randomTranslation0, randomEuler0);

		const Vector3 randomTranslation1 = Random::vector3(randomGenerator, -1, 1);
		const Euler randomEuler1 = Random::euler(randomGenerator);
		device_T_cameras[1] = HomogenousMatrix4(randomTranslation1, randomEuler1);

		BullseyeDetectorStereo::Parameters parameters = BullseyeDetectorStereo::Parameters::defaultParameters();
		parameters.setFramePyramidPixelThreshold((unsigned int)(Scalar(yFrames[0].pixels()) * Random::scalar(randomGenerator, 0.1, 2.0) + Scalar(0.5)));
		parameters.setFramePyramidLayers(RandomI::random(randomGenerator, 1u, 5u));
		parameters.setUseAdaptiveRowSpacing(RandomI::boolean(randomGenerator));
		ocean_assert(parameters.isValid());

		const bool useWorker = RandomI::boolean(randomGenerator);

		BullseyeDetectorStereo::BullseyePairs bullseyePairs;
		Vectors3 bullseyeCenters;
		if (!BullseyeDetectorStereo::detectBullseyes(cameras, yFrames, world_T_device, device_T_cameras, bullseyePairs, bullseyeCenters, parameters, (useWorker ? WorkerPool::get().scopedWorker()() : nullptr)))
		{
			OCEAN_SET_FAILED(validation);
			break;
		}

		// Validate that the output is consistent
		if (bullseyePairs.size() != bullseyeCenters.size())
		{
			OCEAN_SET_FAILED(validation);
			break;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::testExtractBullseyeCandidates(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "extractBullseyeCandidates() function test:";

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const StereoTestSetup setup = createRandomStereoSetup(randomGenerator);

		// Test 1: Empty bullseye lists should return empty map
		{
			Bullseyes bullseyesA;
			Bullseyes bullseyesB;

			CandidateMap candidateMap = extractBullseyeCandidates(*setup.cameraA, *setup.cameraB, setup.world_T_cameraA, setup.world_T_cameraB, bullseyesA, bullseyesB);

			OCEAN_EXPECT_TRUE(validation, candidateMap.empty());
		}

		// Test 2: One empty, one non-empty should return empty map
		{
			const Vector2 randomPoint = Random::vector2(randomGenerator, Scalar(100), Scalar(setup.cameraA->width() - 100), Scalar(100), Scalar(setup.cameraA->height() - 100));

			Bullseyes bullseyesA;
			Bullseyes bullseyesB;

			// Randomly add bullseye to either A or B, leaving the other empty
			if (RandomI::boolean(randomGenerator))
			{
				bullseyesA.emplace_back(createRandomBullseyeAtPosition(randomGenerator, randomPoint));
			}
			else
			{
				bullseyesB.emplace_back(createRandomBullseyeAtPosition(randomGenerator, randomPoint));
			}

			CandidateMap candidateMap = extractBullseyeCandidates(*setup.cameraA, *setup.cameraB, setup.world_T_cameraA, setup.world_T_cameraB, bullseyesA, bullseyesB);

			OCEAN_EXPECT_TRUE(validation, candidateMap.empty());
		}

		// Test 3: Non-empty bullseye lists should produce candidates
		{
			Vector3 worldPoint;
			Vector2 imagePointA;
			Vector2 imagePointB;

			if (createRandomVisibleWorldPoint(randomGenerator, setup, worldPoint, imagePointA, imagePointB))
			{
				Bullseyes bullseyesA;
				bullseyesA.emplace_back(createRandomBullseyeAtPosition(randomGenerator, imagePointA));

				Bullseyes bullseyesB;
				bullseyesB.emplace_back(createRandomBullseyeAtPosition(randomGenerator, imagePointB));

				CandidateMap candidateMap = extractBullseyeCandidates(*setup.cameraA, *setup.cameraB, setup.world_T_cameraA, setup.world_T_cameraB, bullseyesA, bullseyesB);

				// Should have at least one candidate if triangulation succeeds
				if (!candidateMap.empty())
				{
					// Verify the candidate data is valid
					for (const auto& entry : candidateMap)
					{
						const Candidate& candidate = entry.second;
						OCEAN_EXPECT_TRUE(validation, candidate.isValid());
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::testExtractBullseyes(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "extractBullseyes() function test:";

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const StereoTestSetup setup = createRandomStereoSetup(randomGenerator);

		// Test 1: Empty inputs should return false
		{
			Bullseyes bullseyesA;
			Bullseyes bullseyesB;
			CandidateMap candidateMap;

			BullseyePairs bullseyePairs;
			Vectors3 bullseyeCenters;

			const bool result = extractBullseyes(*setup.cameraA, *setup.cameraB, bullseyesA, bullseyesB, candidateMap, bullseyePairs, bullseyeCenters);

			OCEAN_EXPECT_FALSE(validation, result);
		}

		// Test 2: Non-empty bullseyes but empty candidate map should return false
		{
			const Vector2 randomPointA = Random::vector2(randomGenerator, Scalar(100), Scalar(setup.cameraA->width() - 100), Scalar(100), Scalar(setup.cameraA->height() - 100));

			const Vector2 randomPointB = Random::vector2(randomGenerator, Scalar(100), Scalar(setup.cameraB->width() - 100), Scalar(100), Scalar(setup.cameraB->height() - 100));

			Bullseyes bullseyesA;
			bullseyesA.emplace_back(createRandomBullseyeAtPosition(randomGenerator, randomPointA));

			Bullseyes bullseyesB;
			bullseyesB.emplace_back(createRandomBullseyeAtPosition(randomGenerator, randomPointB));

			CandidateMap candidateMap;

			BullseyePairs bullseyePairs;
			Vectors3 bullseyeCenters;

			const bool result = extractBullseyes(*setup.cameraA, *setup.cameraB, bullseyesA, bullseyesB, candidateMap, bullseyePairs, bullseyeCenters);

			OCEAN_EXPECT_FALSE(validation, result);
		}

		// Test 3: Valid input with candidate map generated from extractBullseyeCandidates should succeed
		{
			Vector3 worldPoint;
			Vector2 imagePointA;
			Vector2 imagePointB;

			if (createRandomVisibleWorldPoint(randomGenerator, setup, worldPoint, imagePointA, imagePointB))
			{
				Bullseyes bullseyesA;
				bullseyesA.emplace_back(createRandomBullseyeAtPosition(randomGenerator, imagePointA));

				Bullseyes bullseyesB;
				bullseyesB.emplace_back(createRandomBullseyeAtPosition(randomGenerator, imagePointB));

				// Use extractBullseyeCandidates to generate the candidate map
				CandidateMap candidateMap = extractBullseyeCandidates(*setup.cameraA, *setup.cameraB, setup.world_T_cameraA, setup.world_T_cameraB, bullseyesA, bullseyesB);

				if (!candidateMap.empty())
				{
					BullseyePairs bullseyePairs;
					Vectors3 bullseyeCenters;

					const bool result = extractBullseyes(*setup.cameraA, *setup.cameraB, bullseyesA, bullseyesB, candidateMap, bullseyePairs, bullseyeCenters);

					OCEAN_EXPECT_TRUE(validation, result);

					if (result)
					{
						// Verify output sizes match
						OCEAN_EXPECT_EQUAL(validation, bullseyePairs.size(), bullseyeCenters.size());
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseyeDetectorStereo::testComputeCostMatrix(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "computeCostMatrix() function test:";

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const StereoTestSetup setup = createRandomStereoSetup(randomGenerator);

		// Test 1: Empty inputs should return false
		{
			Bullseyes bullseyesA;
			Bullseyes bullseyesB;
			CandidateMap candidateMap;

			Matrix costMatrix;

			const bool result = computeCostMatrix(*setup.cameraA, *setup.cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix);

			OCEAN_EXPECT_FALSE(validation, result);
		}

		// Test 2: Non-empty bullseyes but empty candidate map should return false
		{
			const Vector2 randomPointA = Random::vector2(randomGenerator, Scalar(100), Scalar(setup.cameraA->width() - 100), Scalar(100), Scalar(setup.cameraA->height() - 100));

			const Vector2 randomPointB = Random::vector2(randomGenerator, Scalar(100), Scalar(setup.cameraB->width() - 100), Scalar(100), Scalar(setup.cameraB->height() - 100));

			Bullseyes bullseyesA;
			bullseyesA.emplace_back(createRandomBullseyeAtPosition(randomGenerator, randomPointA));

			Bullseyes bullseyesB;
			bullseyesB.emplace_back(createRandomBullseyeAtPosition(randomGenerator, randomPointB));

			CandidateMap candidateMap;

			Matrix costMatrix;

			const bool result = computeCostMatrix(*setup.cameraA, *setup.cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix);

			OCEAN_EXPECT_FALSE(validation, result);
		}

		// Test 3: Valid input should return correct cost matrix dimensions
		{
			const size_t numBullseyes = RandomI::random(randomGenerator, 1u, 5u);

			Bullseyes bullseyesA;
			Bullseyes bullseyesB;

			// Generate bullseyes by projecting random 3D points to both cameras
			for (size_t i = 0; i < numBullseyes; ++i)
			{
				Vector3 worldPoint;
				Vector2 imagePointA;
				Vector2 imagePointB;

				if (createRandomVisibleWorldPoint(randomGenerator, setup, worldPoint, imagePointA, imagePointB))
				{
					bullseyesA.emplace_back(createRandomBullseyeAtPosition(randomGenerator, imagePointA));
					bullseyesB.emplace_back(createRandomBullseyeAtPosition(randomGenerator, imagePointB));
				}
			}

			if (!bullseyesA.empty() && !bullseyesB.empty())
			{
				// Use extractBullseyeCandidates to generate the candidate map
				CandidateMap candidateMap = extractBullseyeCandidates(*setup.cameraA, *setup.cameraB, setup.world_T_cameraA, setup.world_T_cameraB, bullseyesA, bullseyesB);

				if (!candidateMap.empty())
				{
					Matrix costMatrix;
					const bool result = computeCostMatrix(*setup.cameraA, *setup.cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix);

					OCEAN_EXPECT_TRUE(validation, result);

					if (result)
					{
						// Verify matrix dimensions
						OCEAN_EXPECT_EQUAL(validation, costMatrix.rows(), bullseyesA.size());
						OCEAN_EXPECT_EQUAL(validation, costMatrix.columns(), bullseyesB.size());

						// Verify cost values are in valid range [0, invalidMatchingCost()]
						for (size_t row = 0; row < costMatrix.rows(); ++row)
						{
							for (size_t col = 0; col < costMatrix.columns(); ++col)
							{
								const Scalar cost = costMatrix(row, col);
								OCEAN_EXPECT_GREATER_EQUAL(validation, cost, Scalar(0));
								OCEAN_EXPECT_LESS_EQUAL(validation, cost, invalidMatchingCost());
							}
						}
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

TestBullseyeDetectorStereo::StereoTestSetup TestBullseyeDetectorStereo::createRandomStereoSetup(RandomGenerator& randomGenerator)
{
	StereoTestSetup setup;

	// Create random camera type
	constexpr std::array<AnyCameraType, 2> anyCameraTypes = {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
	const AnyCameraType cameraType = anyCameraTypes[RandomI::random(randomGenerator, 1u)];

	const SharedAnyCamera realisticCamera = TestGeometry::Utilities::realisticAnyCamera(cameraType, RandomI::random(randomGenerator, 1u));
	ocean_assert(realisticCamera != nullptr && realisticCamera->isValid());

	setup.cameraA = realisticCamera;
	setup.cameraB = realisticCamera;

	// Create random device pose
	const Vector3 randomTranslation = Random::vector3(randomGenerator, Scalar(-1), Scalar(1));
	const Quaternion randomRotation = Random::quaternion(randomGenerator);
	setup.world_T_device = HomogenousMatrix4(randomTranslation, randomRotation);

	// Create random stereo baseline
	const Scalar baseline = Random::scalar(randomGenerator, Scalar(0.1), Scalar(0.5));
	const HomogenousMatrix4 device_T_cameraA(Vector3(-baseline / 2, Scalar(0), Scalar(0)));
	const HomogenousMatrix4 device_T_cameraB(Vector3(baseline / 2, Scalar(0), Scalar(0)));

	setup.world_T_cameraA = setup.world_T_device * device_T_cameraA;
	setup.world_T_cameraB = setup.world_T_device * device_T_cameraB;

	return setup;
}

bool TestBullseyeDetectorStereo::createRandomVisibleWorldPoint(RandomGenerator& randomGenerator, const StereoTestSetup& setup, Vector3& worldPoint, Vector2& imagePointA, Vector2& imagePointB)
{
	// Create a random 3D point in front of both cameras (negative z in device frame = in front)
	const Vector3 randomDevicePoint = Random::vector3(randomGenerator, Scalar(-0.3), Scalar(0.3), Scalar(-0.3), Scalar(0.3), Scalar(-2), Scalar(-0.5));

	worldPoint = setup.world_T_device * randomDevicePoint;

	// Verify point is in front of both cameras
	const bool isInFrontA = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(setup.world_T_cameraA), worldPoint);
	const bool isInFrontB = AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(setup.world_T_cameraB), worldPoint);

	if (!isInFrontA || !isInFrontB)
	{
		return false;
	}

	// Project to both cameras
	imagePointA = setup.cameraA->projectToImage(setup.world_T_cameraA, worldPoint);
	imagePointB = setup.cameraB->projectToImage(setup.world_T_cameraB, worldPoint);

	// Check if inside both images
	return setup.cameraA->isInside(imagePointA) && setup.cameraB->isInside(imagePointB);
}

Bullseye TestBullseyeDetectorStereo::createRandomBullseyeAtPosition(RandomGenerator& randomGenerator, const Vector2& position)
{
	const Scalar randomDiameter = Random::scalar(randomGenerator, Scalar(10), Scalar(30));
	const uint8_t randomThreshold = static_cast<uint8_t>(RandomI::random(randomGenerator, 50u, 200u));

	return Bullseye(position, randomDiameter, randomThreshold);
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
