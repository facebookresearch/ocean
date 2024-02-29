// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testdetector/TestMessengerCodeDetector.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#ifdef OCEAN_ENABLED_EVERSTORE_CLIENT
	#include "ocean/media/openimagelibraries/Image.h"

	#include "metaonly/ocean/network/everstore/EverstoreClient.h"
#else
	#ifdef __APPLE__
		#include "ocean/media/imageio/Image.h"
	#endif
#endif // OCEAN_ENABLED_EVERSTORE_CLIENT

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using namespace CV::Detector;

// #define APPLY_IMAGE_TEST

bool TestMessengerCodeDetector::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Messenger Code detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testExtractCodeCandidates(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBullseyeDetectionArtificial(0u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testBullseyeDetectionArtificial(3u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testBullseyeDetectionArtificial(5u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testBullseyeDetectionArtificial(7u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStressTest(testDuration, worker) && allSucceeded;

#if defined(APPLY_IMAGE_TEST) || defined(OCEAN_ENABLED_EVERSTORE_CLIENT)

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetect1Bullseye(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetect0Code(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetect1Code(worker) && allSucceeded;

#endif

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Messenger Code detector test succeeded.";
	}
	else
	{
		Log::info() << "Messenger Code detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMessengerCodeDetector, ExtractCodeCandidates)
{
	EXPECT_TRUE(TestMessengerCodeDetector::testExtractCodeCandidates(GTEST_TEST_DURATION));
}

TEST(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize0)
{
	EXPECT_TRUE(TestMessengerCodeDetector::testBullseyeDetectionArtificial(0u, GTEST_TEST_DURATION));
}

TEST(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize3)
{
	EXPECT_TRUE(TestMessengerCodeDetector::testBullseyeDetectionArtificial(3u, GTEST_TEST_DURATION));
}

TEST(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize5)
{
	EXPECT_TRUE(TestMessengerCodeDetector::testBullseyeDetectionArtificial(5u, GTEST_TEST_DURATION));
}

TEST(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize7)
{
	EXPECT_TRUE(TestMessengerCodeDetector::testBullseyeDetectionArtificial(7u, GTEST_TEST_DURATION));
}

TEST(TestMessengerCodeDetector, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestMessengerCodeDetector::testStressTest(GTEST_TEST_DURATION, worker));
}

#if defined(OCEAN_ENABLED_EVERSTORE_CLIENT)

TEST(TestMessengerCodeDetector, Detect1Bullseye)
{
	Worker worker;
	EXPECT_TRUE(TestMessengerCodeDetector::testDetect1Bullseye(worker));
}

TEST(TestMessengerCodeDetector, Detect0CodeEverstoreTest)
{
	Worker worker;
	EXPECT_TRUE(TestMessengerCodeDetector::testDetect0Code(worker));
}

TEST(TestMessengerCodeDetector, Detect1CodeEverstoreTest)
{
	Worker worker;
	EXPECT_TRUE(TestMessengerCodeDetector::testDetect1Code(worker));
}

#endif // OCEAN_ENABLED_EVERSTORE_CLIENT

#endif // OCEAN_USE_GTEST

bool TestMessengerCodeDetector::testExtractCodeCandidates(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Extract code candidates test:";

	constexpr Scalar minRadius = Scalar(5);
	constexpr Scalar maxRadius = Scalar(15);

	bool allSucceeded = true;

	// CodePair stores the center of a code and the radius of the bullseyes
	typedef std::pair<Vector2, Scalar> CodePair;
	typedef std::vector<CodePair> CodePairs;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		// we create code corners fair enough away from each other

		const unsigned int numberCodes = RandomI::random(randomGenerator, 1u, 10u);

		CodePairs codePairs;
		Indices32 corners2code;

		Vectors2 corners;
		Scalars radii;

		// we create randomly separated codes

		while (codePairs.size() < numberCodes)
		{
			const Vector2 codeCenter = Random::vector2(randomGenerator, -1000, 1000);

			const Scalar bullseyeRadius = Random::scalar(randomGenerator, minRadius, maxRadius);
			const Scalar distanceShort = MessengerCodeDetector::radius2bullseyesDistanceShort(bullseyeRadius);

			const Scalar distanceLong = MessengerCodeDetector::radius2bullseyesDistanceLong(bullseyeRadius);
			ocean_assert(distanceShort > Numeric::eps() && distanceLong > distanceShort);
			if (distanceShort < bullseyeRadius * Scalar(5) || distanceLong <= distanceShort)
			{
				allSucceeded = false;
			}

			bool tooClose = false;

			for (const CodePair& codePair : codePairs)
			{
				if (codeCenter.distance(codePair.first) < distanceLong + MessengerCodeDetector::radius2bullseyesDistanceLong(codePair.second))
				{
					tooClose = true;
					break;
				}
			}

			if (tooClose == false)
			{
				const Index32 codeIndex = Index32(codePairs.size());

				codePairs.push_back(CodePair(codeCenter, bullseyeRadius));

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					corners2code.push_back(codeIndex);
					radii.push_back(bullseyeRadius * Random::scalar(randomGenerator, Scalar(0.95), Scalar(1.05)));
				}

				const Vectors2 codeCorners = createCodeCorners(codeCenter, distanceShort, randomGenerator);
				corners.insert(corners.end(), codeCorners.cbegin(), codeCorners.cend());
			}
		}

		// now we add outliers not connected with any valid corner

		const unsigned int outlierIterations = RandomI::random(randomGenerator, 0u, 50u);

		for (unsigned int n = 0u; n < outlierIterations; ++n)
		{
			const Vector2 outlierCorner = Random::vector2(randomGenerator, -1200, 1200);

			bool tooClose = false;

			for (const Vector2& validCorner : corners)
			{
				if (outlierCorner.distance(validCorner) < Scalar(2) * MessengerCodeDetector::radius2bullseyesDistanceLong(maxRadius))
				{
					tooClose = true;
					break;
				}
			}

			if (tooClose == false)
			{
				corners.push_back(outlierCorner);
				corners2code.push_back(Index32(-1));
				radii.push_back(15);
			}
		}

		ocean_assert(corners.size() == corners2code.size());
		ocean_assert(corners.size() == radii.size());

		// we shuffle the order of all corners
		for (unsigned int n = 0u; n < numberCodes * 4u; ++n)
		{
			const unsigned int index0 = RandomI::random(randomGenerator, (unsigned int)corners.size() - 1u);
			const unsigned int index1 = RandomI::random(randomGenerator, (unsigned int)corners.size() - 1u);

			std::swap(corners[index0], corners[index1]);
			std::swap(corners2code[index0], corners2code[index1]);
			std::swap(radii[index0], radii[index1]);
		}

		const MessengerCodeDetector::IndexQuartets indexQuartets = MessengerCodeDetector::extractCodeCandidates(corners.data(), radii.data(), corners.size());

		if (indexQuartets.size() != size_t(numberCodes))
		{
			allSucceeded = false;
			continue;
		}

		for (const MessengerCodeDetector::IndexQuartet& indexQuartet : indexQuartets)
		{
			if (IndexSet32(indexQuartet.cbegin(), indexQuartet.cend()).size() == 4)
			{
				const Index32 codeIndex = corners2code[indexQuartet[0]];

				// all corners of the code must belong to the same code (must have the same code index)

				if (codeIndex == corners2code[indexQuartet[1]] && codeIndex == corners2code[indexQuartet[2]] && codeIndex == corners2code[indexQuartet[3]])
				{
					const CodePair codePair = codePairs[codeIndex];
					const Scalar distanceShort = MessengerCodeDetector::radius2bullseyesDistanceShort(codePair.second);

					for (unsigned int n = 0u; n < 4u; ++n)
					{
						const unsigned int currentIndex = indexQuartet[n];
						const unsigned int nextIndex = indexQuartet[(n + 1u) % 4u];

						// the distance between neighboring corners must be identical

						if (Numeric::isNotEqual(corners[currentIndex].distance(corners[nextIndex]), distanceShort, Scalar(0.01)))
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMessengerCodeDetector::testBullseyeDetectionArtificial(const unsigned int filterSize, const double testDuration)
{
	ocean_assert(filterSize == 0 || filterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Artificial bullseye detection test, with filter size " << filterSize << ":";

	// BullseyePair stores the center of a bullseye and the radius
	typedef std::pair<Vector2, Scalar> BullseyePair;
	typedef std::vector<BullseyePair> BullseyePairs;

	unsigned long long bullseyesTotal = 0ull;
	unsigned long long bullseyesDetectedCorrect = 0ull;
	unsigned long long bullseyesDetectedWrong = 0ull;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const uint8_t backgroundColor = uint8_t(RandomI::random(randomGenerator, 200u, 255u));

		const unsigned int width = RandomI::random(randomGenerator, 250u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 250u, 1920u);

		const unsigned int frameHorizontalPadding = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

		BullseyePairs bullseyePairs;
		const unsigned int bullseyeIterations = RandomI::random(randomGenerator, 1u, 50u);

		for (unsigned int n = 0u; n < bullseyeIterations; ++n)
		{
			const Scalar radius = Random::scalar(randomGenerator, Scalar(10), Scalar(35));
			const Vector2 location = Random::vector2(randomGenerator, radius * Scalar(4), Scalar(width) - radius * Scalar(3) - Scalar(1), radius * Scalar(4), Scalar(height) - radius * Scalar(3) - Scalar(1));

			bool tooClose = false;

			for (const BullseyePair& bullseyePair : bullseyePairs)
			{
				if (bullseyePair.first.distance(location) <= (bullseyePair.second + radius) * Scalar(1.15))
				{
					tooClose = true;
					break;
				}
			}

			if (tooClose == false)
			{
				bullseyePairs.emplace_back(location, radius);
			}
		}

		ocean_assert(!bullseyePairs.empty());

		Vectors2 noiseLocations;
		const unsigned int noiseIterations = RandomI::random(randomGenerator, 1u, 50u);

		for (unsigned int n = 0u; n < noiseIterations; ++n)
		{
			const Vector2 location = Random::vector2(randomGenerator, Scalar(10), Scalar(width - 11u), Scalar(10), Scalar(height - 11u));

			bool tooClose = false;

			for (const BullseyePair& bullseyePair : bullseyePairs)
			{
				if (bullseyePair.first.distance(location) <= (bullseyePair.second) * Scalar(2.15))
				{
					tooClose = true;
					break;
				}
			}

			if (tooClose == false)
			{
				noiseLocations.push_back(location);
			}
		}

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), frameHorizontalPadding);
		yFrame.setValue(backgroundColor);

		for (const BullseyePair& bullseyePair : bullseyePairs)
		{
			const unsigned char foregroundColor = (unsigned char)(RandomI::random(randomGenerator, 0u, 50u));

			paintBullseye(yFrame.data<uint8_t>(), width, height, bullseyePair.first, bullseyePair.second, foregroundColor, backgroundColor, yFrame.paddingElements());
		}

		for (const Vector2& noiseLocation : noiseLocations)
		{
			const uint8_t foregroundColor = uint8_t(RandomI::random(randomGenerator, 0u, 50u));

			paintNoise(yFrame, noiseLocation, foregroundColor);
		}

		if (filterSize != 0u)
		{
			CV::FrameFilterGaussian::filter(yFrame, filterSize, WorkerPool::get().scopedWorker()());
		}

		bullseyesTotal += bullseyePairs.size();

		const MessengerCodeDetector::Bullseyes detectedBullseyes = MessengerCodeDetector::detectBullseyes(yFrame.constdata<uint8_t>(), width, height, yFrame.paddingElements());

		/// forward check
		for (const BullseyePair& bullseyePair : bullseyePairs)
		{
			bool foundMatch = false;

			for (const MessengerCodeDetector::Bullseye& detectedBullseye : detectedBullseyes)
			{
				if (bullseyePair.first.sqrDistance(detectedBullseye.position()) < Numeric::sqr(5))
				{
					if (Numeric::abs(bullseyePair.second - detectedBullseye.radius()) <= bullseyePair.second * Scalar(0.25))
					{
						foundMatch = true;
						break;
					}
				}
			}

			if (foundMatch)
			{
				bullseyesDetectedCorrect++;
			}
		}

		/// backward check
		for (const MessengerCodeDetector::Bullseye& detectedBullseye : detectedBullseyes)
		{
			bool foundMatch = false;

			for (const BullseyePair& bullseyePair : bullseyePairs)
			{
				if (detectedBullseye.position().sqrDistance(bullseyePair.first) < Numeric::sqr(5))
				{
					if (Numeric::abs(detectedBullseye.radius() - bullseyePair.second) <= bullseyePair.second * Scalar(0.25))
					{
						foundMatch = true;
						break;
					}
				}
			}

			if (foundMatch == false)
			{
				bullseyesDetectedWrong++;
			}
		}

	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(bullseyesTotal != 0ull);

	const double percentCorrect = double(bullseyesDetectedCorrect) / double(bullseyesTotal);
	const double percentWrong = double(bullseyesDetectedWrong) / double(bullseyesTotal);

	Log::info() << "Corrected detected: " << String::toAString(percentCorrect * 100.0, 2u) << "%";
	Log::info() << "False positive: " << String::toAString(percentWrong * 100.0, 2u) << "%";

	const bool succeeded = percentCorrect >= 0.99 && percentWrong <= 0.01;

	if (succeeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

bool TestMessengerCodeDetector::testStressTest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test:";

	// we actually do not validate the result, but we simply try to crash the detector instead

	RandomGenerator randomGenerator;

	size_t dummyValue = 0;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 21u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 21u, 1920u);

		const unsigned int horizontalPadding = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), horizontalPadding);
		CV::CVUtilities::randomizeFrame(yFrame, false, &randomGenerator);

		Worker* useWorker = (worker && RandomI::random(1u) == 1u) ? &worker : nullptr;

		const MessengerCodeDetector::Codes codes = MessengerCodeDetector::detectMessengerCodes(yFrame.constdata<uint8_t>(), width, height, yFrame.paddingElements(), useWorker);

		dummyValue += codes.size();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	// we actually do not validate the result, but we simply try to crash the detector instead
	// using dummyValue to ensure that the compiler is actually calling detectMessengerCodes()

	if (dummyValue % 2 == 0)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: Succeeded.";
	}

	return true;
}

bool TestMessengerCodeDetector::testDetect1Bullseye(Worker& worker)
{
	Log::info() << "Detection of exactly 1 Bullseye:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const HandlePairs handlePairs1Code = testImage1Bullseye();

	for (const HandlePair& handlePair : handlePairs1Code)
	{
		Frame testImageY8 = loadTestImage(handlePair);
		ocean_assert(testImageY8.pixelFormat() == FrameType::FORMAT_Y8);

		if (testImageY8.isValid() == false)
		{
			Log::info() << "Failed to download image: " << handlePair.first;

			allSucceeded = false;
			continue;
		}

		const Vector2 imageCenter(Scalar(testImageY8.width()) * Scalar(0.5), Scalar(testImageY8.height()) * Scalar(0.5));
		const Scalar distance5 = Scalar(std::min(testImageY8.width(), testImageY8.height())) * Scalar(0.05);

		// we rotate the image four times by 90 degree

		for (unsigned int rotateIteration = 0u; rotateIteration < 4u; ++rotateIteration)
		{
			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

				const Bullseyes bullseyes = CV::Detector::MessengerCodeDetector::detectBullseyes(testImageY8.constdata<uint8_t>(), testImageY8.width(), testImageY8.height(), testImageY8.paddingElements(), useWorker);

				if (bullseyes.empty())
				{
					allSucceeded = false;
				}
				else
				{
					for (const Bullseye& bullseye : bullseyes)
					{
						if (bullseye.position().distance(imageCenter) > distance5)
						{
							allSucceeded = false;
						}
					}
				}

				Frame rotatedImage;
				if (!CV::FrameInterpolatorNearestPixel::Comfort::rotate90(testImageY8, rotatedImage, true))
				{
					ocean_assert(false && "Should never happen!");
					allSucceeded = false;
				}

				testImageY8 = std::move(rotatedImage);
			}
		}
	}

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

bool TestMessengerCodeDetector::testDetect0Code(Worker& worker)
{
	Log::info() << "Detection of 0 Messenger code:";

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const HandlePairs handlePairs0Code = testImages0Code();

	for (const HandlePair& handlePair : handlePairs0Code)
	{
		const Frame testImageY8 = loadTestImage(handlePair);
		ocean_assert(testImageY8.pixelFormat() == FrameType::FORMAT_Y8);

		if (testImageY8.isValid() == false)
		{
			Log::info() << "Failed to download image: " << handlePair.first;

			allSucceeded = false;
			continue;
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

			CV::Detector::MessengerCodeDetector::Codes codes = CV::Detector::MessengerCodeDetector::detectMessengerCodes(testImageY8.constdata<uint8_t>(), testImageY8.width(), testImageY8.height(), testImageY8.paddingElements(), useWorker);

			if (codes.size() != 0)
			{
				allSucceeded = false;
			}
		}
	}

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

bool TestMessengerCodeDetector::testDetect1Code(Worker& worker)
{
	Log::info() << "Detection of exactly 1 Messenger code:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const HandlePairs handlePairs1Code = testImages1Code();

	for (const HandlePair& handlePair : handlePairs1Code)
	{
		const Frame testImageY8 = loadTestImage(handlePair);
		ocean_assert(testImageY8.pixelFormat() == FrameType::FORMAT_Y8);

		if (testImageY8.isValid() == false)
		{
			Log::info() << "Failed to download image: " << handlePair.first;

			allSucceeded = false;
			continue;
		}

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;

			CV::Detector::MessengerCodeDetector::Codes codes = CV::Detector::MessengerCodeDetector::detectMessengerCodes(testImageY8.constdata<uint8_t>(), testImageY8.width(), testImageY8.height(), testImageY8.paddingElements(), useWorker);

			if (codes.size() == 1)
			{
				++validIterations;
			}

			++iterations;
		}
	}

	ocean_assert(allSucceeded == false || iterations != 0ull);
	if (iterations != 0u)
	{
		const double percent = double(validIterations) / double(iterations);

		if (percent < 0.95)
		{
			Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";
			allSucceeded = false;
		}
	}

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

Vectors2 TestMessengerCodeDetector::createCodeCorners(const Vector2& codeCenter, const Scalar distance, RandomGenerator& randomGenerator)
{
	ocean_assert(distance > Numeric::eps());

	const Scalar distance_2 = distance * Scalar(0.5);

	// the four corners of the Code (actually a square) in a counter-clock-wise order
	Vectors2 corners =
	{
		Vector2(-distance_2, -distance_2), // top left
		Vector2(-distance_2, distance_2), // bottom left
		Vector2(distance_2, distance_2), // bottom right
		Vector2(distance_2, -distance_2) // top right
	};

	// we shuffle the order
	for (unsigned int n = 0u; n < 10u; ++n)
	{
		std::swap(corners[RandomI::random(randomGenerator, (unsigned int)corners.size() - 1u)], corners[RandomI::random(randomGenerator, (unsigned int)corners.size() - 1u)]);
	}

	const Scalar rotation = Random::scalar(randomGenerator, 0, Numeric::pi2() - Numeric::eps());

	// we shift the corners and rotate the square

	for (Vector2& corner : corners)
	{
		corner = codeCenter + (Quaternion(Vector3(0, 0, 1), rotation) * Vector3(corner, 0)).xy();
	}

	ocean_assert(Numeric::isInsideRange(distance, corners[0].distance(corners[1]), distance * Numeric::sqrt(2), Scalar(0.1)));
	ocean_assert(Numeric::isInsideRange(distance, corners[1].distance(corners[2]), distance * Numeric::sqrt(2), Scalar(0.1)));
	ocean_assert(Numeric::isInsideRange(distance, corners[2].distance(corners[3]), distance * Numeric::sqrt(2), Scalar(0.1)));
	ocean_assert(Numeric::isInsideRange(distance, corners[3].distance(corners[0]), distance * Numeric::sqrt(2), Scalar(0.1)));

	return corners;
}

void TestMessengerCodeDetector::paintBullseye(unsigned char* frame, const unsigned int width, const unsigned int height, const Vector2& location, const Scalar radius, const unsigned char foregroundColor, const unsigned char backgroundColor, const unsigned int framePaddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width >= 21u && height >= 21u);
	ocean_assert(radius >= Scalar(3));

	ocean_assert(location.x() >= Scalar(2) * radius);
	ocean_assert(location.y() >= Scalar(2) * radius);

	ocean_assert(location.x() < Scalar(width) - Scalar(2) * radius);
	ocean_assert(location.y() < Scalar(height) - Scalar(2) * radius);

	//    ring   ring     dot    ring    ring
	// | black | white | black | white | black |
	//                     |       radius      |

	const Scalar elementSize = radius * Scalar(0.4); // = radius / 2.5;

	const unsigned int dotDiameter = ((unsigned int)(elementSize * Scalar(1) + Scalar(0.5))) | 0x00000001u;
	const unsigned int whiteRingDiameter = ((unsigned int)(elementSize * Scalar(3) + Scalar(0.5))) | 0x00000001u;
	const unsigned int blackRingDiameter = ((unsigned int)(elementSize * Scalar(5) + Scalar(0.5))) | 0x00000001u;

	ocean_assert(blackRingDiameter > whiteRingDiameter && whiteRingDiameter > dotDiameter);

	const CV::PixelPosition pixelPosition((unsigned int)Numeric::round32(location.x()), (unsigned int)Numeric::round32(location.y()));

	CV::Canvas::ellipse8BitPerChannel<1u>(frame, width, height, pixelPosition, blackRingDiameter, blackRingDiameter, &foregroundColor, framePaddingElements);
	CV::Canvas::ellipse8BitPerChannel<1u>(frame, width, height, pixelPosition, whiteRingDiameter, whiteRingDiameter, &backgroundColor, framePaddingElements);
	CV::Canvas::ellipse8BitPerChannel<1u>(frame, width, height, pixelPosition, dotDiameter, dotDiameter, &foregroundColor, framePaddingElements);
}

void TestMessengerCodeDetector::paintNoise(Frame& yFrame, const Vector2& location, const uint8_t foregroundColor)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(yFrame.width() >= 21u && yFrame.height() >= 21u);

	ocean_assert(location.x() >= Scalar(10));
	ocean_assert(location.y() >= Scalar(10));

	ocean_assert(location.x() < Scalar(yFrame.width()) - Scalar(10));
	ocean_assert(location.y() < Scalar(yFrame.height()) - Scalar(10));

	const CV::PixelPosition pixelPosition((unsigned int)(Numeric::round32(location.x())), (unsigned int)(Numeric::round32(location.y())));

	for (unsigned int n = 0u; n < 5u; ++n)
	{
		const int xOffset = RandomI::random(-3, 3);
		const int yOffset = RandomI::random(-3, 3);

		CV::PixelPosition offsetPosition((unsigned int)(int(pixelPosition.x()) + xOffset), (unsigned int)(int(pixelPosition.y()) + yOffset));
		ocean_assert(offsetPosition.x() < yFrame.width() && offsetPosition.y() < yFrame.height());

		CV::Canvas::ellipse(yFrame, offsetPosition, 3u, 3u, &foregroundColor);
	}
}

Frame TestMessengerCodeDetector::loadTestImage(const HandlePair& handlePair)
{

#if defined(OCEAN_ENABLED_EVERSTORE_CLIENT)

	Network::EverstoreClient::Buffer buffer;
	if (!Network::EverstoreClient::download(handlePair.second, buffer))
	{
		ocean_assert(false && "Failed to download image from everstore!");
		return Frame();
	}

	const Frame image = Media::OpenImageLibraries::Image::decodeImage(buffer.data(), buffer.size());

#elif defined(__APPLE__)
	const Frame image = Media::ImageIO::Image::readImage(handlePair.first);
#else
	const Frame image; // missing implementation
	OCEAN_SUPPRESS_UNUSED_WARNING(handlePair);
#endif

	if (!image.isValid())
	{
		ocean_assert(false && "This should never happen!");
		return Frame();
	}

	Frame yImage;
	if (!CV::FrameConverter::Comfort::convert(image, FrameType(image, FrameType::FORMAT_Y8), yImage, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		Frame();
	}

	return yImage;
}

std::string TestMessengerCodeDetector::testImagesDirectory()
{
	std::string absolutePath;

#if defined(APPLY_IMAGE_TEST)

	#ifdef OCEAN_ENABLED_EVERSTORE_CLIENT
		#error Either Everstore or local images.
	#endif

	absolutePath = "";
	ocean_assert(!absolutePath.empty() && "Define a valid absolute path");

#endif

	return absolutePath;
}

TestMessengerCodeDetector::HandlePairs TestMessengerCodeDetector::testImage1Bullseye()
{
	const std::string absolutePath = testImagesDirectory();

	HandlePairs handlePairs;

	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00001.png", "GEvQ_QJgQThtJ9AAAEMJfxqlD8lvbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00002.png", "GGYY9wKPDEIxIvgHAIhlL7QyTGsgbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00003.png", "GH6s_ALnkYigPSoBACEG2wbH-gwRbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00004.png", "GLWu8AJR_00FT98BAAXriHdM-QYAbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00005.png", "GPIx9wLmBUv4wUEEAEvtnK9SiVFlbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00006.png", "GPrc9QLIFTnh1E8BABJjelwvL6F6buZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00007.png", "GCE99gJ24na1KbACAEQDaHum5xwIbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00008.png", "GLcQ9wJMsf8fFMsBAJFVDUZRVG0ubuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00009.png", "GH2b9wJ0dAYzDHoAAM2zonaYby82buZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00010.png", "GDp-_AIyHHjALRQCAE_O5_63lRxkbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00011.png", "GIVB9wLlPbg68JAAAKuZ2Umd8MIQbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00012.png", "GK389AL1q0QEFu4GAJ5f_tx3PqRIbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00013.png", "GEx-_AKA0scWOcgHACVGnFnegsRabuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00014.png", "GKz39QKqIdt3RT0HABvNe2kwfcJlbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00015.png", "GB_Q9wJIFP3ZcX4AAFPtMKiWFT9hbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00016.png", "GJdT-AJRAL1d0P4AAOPky5W_WNoabuZcAAAD");

	return handlePairs;
}

TestMessengerCodeDetector::HandlePairs TestMessengerCodeDetector::testImages0Code()
{
	const std::string absolutePath = testImagesDirectory();

	HandlePairs handlePairs;

	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00001.png", "GHKs9AK1NZ0FXooGAOxa4zb5X2d4buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00002.png", "GKIC9AJ3Th0Kd-4BAMntoZdFlUw7buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00003.png", "GIzX8QLrpw71DRUBAOyHxBY1dlw6buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00004.png", "GIqg8wL7pj3BQREIAFd0TR5LF3lJbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00005.png", "GBcG9QKZaEsKSCABANyneYuPMQ5UbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00006.png", "GPwU9QI3JXOsvn4HADwCmSxtj6EXbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00007.png", "GDnz8gLoroGBxysHAIXzXlA-UGZBbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00008.png", "GBPt8gKSHUK_Rp8HABYVZRGbW7ZGbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00009.png", "GJHP8gL14ztfgLEIAPp_1mVFfrAUbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00010.png", "GGUR8wLTTrOGRBQBAJPA7stV0U8abuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00011.png", "GAzH9AIL8Yb-6DAHAKnI5nXUWGFjbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00012.png", "GINt9wJQ35TBwfYAADoq5q8huQp0buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00013.png", "GCJ0_AJzeddQ6tUBACo2-_N05BM0buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00014.png", "GHVZ8QI4DhB_0bgHAN_R4V7mvB40buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00015.png", "GLGP9AKMPA-4pg8BAHZjTbPrG4g7buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00016.png", "GOjd8QJ-mw24SdkGAJseZdL4GntybuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00017.png", "GGZV9QI3_YNH5CAIAJcTiTvzVIN3buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00018.png", "GGrb8wK0pS0KCcYHAMqF0VmbpcUUbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00019.png", "GKmL9ALHClw62UoBAMGqsOqeX_JZbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00020.png", "GFke8gI14lhKD_MHALLcHjR2ChMKbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00021.png", "GCHg9QL0MgZZ6c0CAEu88gKI5tpdbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00022.png", "GNm_9AIzUPjP3zUBAKo0VdNzRLMWbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00023.png", "GJLg8QKA2svBZJ0AAK8w9e4LnRw6buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00024.png", "GGWD8gKTVQWglcYBAGayOJLCTIhNbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00025.png", "GB8G9QIxB1_tKNIBAN5xrQY6bJ9bbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00026.png", "GBfm9wKF8PUyhrsDAPNzi-GiBklybuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00027.png", "GF5e8gKzo_YrpigCAC3R5VQCu_V2buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00028.png", "GIvX8QJLhH8M2FkCAN-87e88SMoYbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00029.png", "GGWs9AI7h1XVfk8BAAJgqGyzaZUXbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00030.png", "GNcR9AL8xoCKIVkDABfcSsrB-4MmbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00031.png", "GHBx_ALaxaenFdYBAP4U-hQKnGcdbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00032.png", "GHWg8wJgE3y-0gABAFBALG6VucUfbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00033.png", "GDhB8QIDO8WW0FgBACNCg3c9MgETbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00034.png", "GFjK8wL7REAVvwUEAPaNmMpmhgkTbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00035.png", "GI2c8wKCig-nJx4BAAZ-fmM-xlIgbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00036.png", "GIxP9gIQHbx3SfcAABW3sTz0L09DbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00037.png", "GGiI8gLkSyCJSJcCADRi-w4PNW8qbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00038.png", "GB4t8gKeg5z34WYBAMqKJ9lxdptibuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00039.png", "GJoN9ALL0tg7OsEAAP_l3Fj7ancfbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00040.png", "GIGy8wL-j7w4V3kAAGmPW82I9mIzbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00041.png", "GDNz9gJOYxAoz3sAAAuNyaDFKoE7buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00042.png", "GOB68wKBF2BgmT4IAAizWeI4T8ERbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00043.png", "GA0w8gIEbNSfk9sBAPVuIOAU4PppbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00044.png", "GPzd8QI107BTXJwBAAGwJAp9s9BHbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00045.png", "GN0V_AIp9aBQol8BAOnaerby3owZbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00046.png", "GHhJ9AI5owWPx-cAANAfKof14uVrbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00047.png", "GNvj8QI0MaJ9O44AAEaPBQAE1nENbuZcAAAD");

	return handlePairs;
}

TestMessengerCodeDetector::HandlePairs TestMessengerCodeDetector::testImages1Code()
{
	const std::string absolutePath = testImagesDirectory();

	HandlePairs handlePairs;

	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_001.png", "GAGD8AKtT_5ys9ECAIZus57wh11PbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_002.png", "GHIq8wLKK_cZM0IBAOrWVBGgAtdhbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_003.png", "GBkH7wKSmEv54d0AADwuSOoQeiI0buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_004.png", "GLRM8AJjKSYmu3oAAK3945OLNp5mbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_005.png", "GMwB8AKrZ4vbFngIAEcztZq1mCAhbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_006.png", "GC128ALH341_CvoAAI_QKyMeEQIBbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_007.png", "GEr48wKxrW3bX8gAACSVRXXGqulMbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_008.png", "GD5q8gIRPerSHgUHAAbMI32NYNYVbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_009.png", "GOtn9ALl_y9dh8MHADbHvyypAphebuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_010.png", "GPCC8AKIrSDV1h0FAE0gaEtKgd8jbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_011.png", "GNn-7wIpMwiXWvwBAHStz4b7AE4tbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_012.png", "GI7w7wKRItPSkTYIAOXLgumCyj4tbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_013.png", "GH7Z8AJJRRxRqJUAAMT0QlszWSI6buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code/test_image_014.png", "GI4Q8QI5Zuc_VQEBALMl3oUM8thMbuZcAAAD");

	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/MNMessengerCodeKitTests_Gaussian_blur_0.8/messengerCodeTestImage1.png", "GG0f_AJ6Lvw_RHMAAHIATdZg-mtPbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/MNMessengerCodeKitTests_Gaussian_blur_0.8/messengerCodeTestImage2.png", "GNLj8QK8xX9tiEUBADgDk3qr_7V0buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/MNMessengerCodeKitTests_Gaussian_blur_0.8/messengerCodeTestImage3.png", "GLFM8AJTUDO6eWoBAKHE92PGB8IbbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/MNMessengerCodeKitTests_Gaussian_blur_0.8/messengerCodeTestImage6.png", "GBEG9QJIKtWSVC8BAJYjzYC-629zbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/MNMessengerCodeKitTests_Gaussian_blur_0.8/messengerCodeTestImage4.png", "GH3-8gKbGrTm8C4HAMDgONwO8aZFbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/MNMessengerCodeKitTests_Gaussian_blur_0.8/messengerCodeTestImage5.png", "GI1n9AISJISTAFUBAIfUnEtSgydvbuZcAAAD");

	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0445.png", "GEk-9ALvgiDspxIBANuNvjlyiA4jbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0446.png", "GBcd9AJHZ4XBzWYBAO0K5IeXr0xnbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0447.png", "GG8Y9wIEuY0OKswCAF1QGCnguSlsbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0448.png", "GKZb_AIi4GM1EMUAAIkVqdNXb5AMbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0449.png", "GLGH8wIjlntIJcoAAPAmTR2oKaovbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0450.png", "GMUm9wKq2XTu4NsCACYzWQ6oYgU4buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0451.png", "GOat8QI3JTYwT_EBAFDekRKsQLBabuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0452.png", "GJOx8gJon9BcNDoHAMBT6Ttw6O0FbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/far_to_close/IMG_0453.png", "GHzK9gIn8RAyMfMBAFPglM8_3m0ybuZcAAAD");

	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00001.png", "GICg8wJ68fw1RggEAInAqcob200WbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00002.png", "GP1Z8wKm5KmxlzEBALPBB78gT09VbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00003.png", "GPUW8wIGSWEewfkGABeE_WEseEF-buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00004.png", "GLFM8ALWO8QdQhkCAM6rl-0btJNRbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00005.png", "GJ_P9AIZuCf3uEYDABzS5-qnR2xpbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00006.png", "GHyx8QL55Kz5yC8CAJlhZxftUyIgbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00007.png", "GMYN8gJQgAUsnwoBAFMLqvjCY4gMbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00008.png", "GJbj9wJipvyKR-sAADqvH5B30sdtbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00009.png", "GKKP9AIcudOavMsAAFLtsqTN-CZfbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00010.png", "GG8W_QL7yGvCQ80AAFnvkhEPpSUKbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00011.png", "GFnN8gLVBrQ9MTgBADYcifvZovFsbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00012.png", "GECK9gKFlz4ZofQAADzaYQDgYThxbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00013.png", "GOZ68wILweqobwUCACE_PyTYQYw2buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00014.png", "GB6d9AJLEQLaVWkDANKYyzstdx4AbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00015.png", "GPX78QK8WHvyGOkBAH_AwJfcux57buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00016.png", "GPLd8QIaYW-BgbkAAMWY0nEqwRRzbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00017.png", "GJuP8gIdqJXiYm0AAIgVQ4C0jphubuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00018.png", "GJ1K8wJY2kcZFjsBAMlKxY_Gy41sbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00019.png", "GGrN8gIvJAytn1YBAFQmQ75YUaF0buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00020.png", "GCSM_AI56Nj7wekBAF3Kx-dyVRxQbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00021.png", "GKxC8gION8FyCFMBAGJhf59cojxjbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00022.png", "GAPS9QL-seIuYzgBAJVaT7SeUXADbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00023.png", "GFym9ALG_-iXBeMBAEYj79bweN5qbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00024.png", "GNox9wKgo-eauEoBAMgSFrV4gAV_buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00025.png", "GHqy8wLxVQUmpaECAG1XOTCqrVI3buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00026.png", "GDnz8gKK7dIoXoAGAJlRt2EWL0hZbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00027.png", "GH1B9wKGWVtovSYBAJ_dcjLuxT5XbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00028.png", "GL4d_AKuhTd0RhYBAEwrdGH4DXg4buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00029.png", "GO1_8wLwXFGGZA8JAMpK6f94ttAWbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00030.png", "GNgR9AIOr_LLikMBABj3S3V7EC9ZbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00031.png", "GGmQ8wI1AvpTLk4HAC1gXXMrSaAObuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00032.png", "GPkU9QLO1qi-v10BAGeoiu-9XvQdbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00033.png", "GGF18gLZkx8CEVIBAL2fs7UFW_JnbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00034.png", "GJRe9AKZGNrFR00BAGUE3994iGFpbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00035.png", "GCYY9ALP-I8Hyq4HAPqiVFdLob9bbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00036.png", "GPVS9AJbfO_0oyYIAKwRZaeep7tqbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00037.png", "GDa18QLutfcFDkEBAAWk3M6dgWd4buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00038.png", "GNYu9QLYBVw01fwGAHjsmRGIWzZGbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00039.png", "GLHh8gKwSwosWtIAANB4BKfpGJZUbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00040.png", "GN6n9gIqGN7q5-kAANxZc7BJn-RsbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00041.png", "GIhr9QIHsipAzDoEACWmMdcIVjEcbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00042.png", "GJM_8wKQdTYipcsJADgmO3MYuJ8mbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00043.png", "GEJq8gIeJ8nig00HANYwyqm_v7shbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00044.png", "GBkT_AK14UxGXw4BAO53LcsRhVx1buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00045.png", "GNNC8gLmjBm2jjUBAJWitXvWvWFvbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00046.png", "GAnJ8gIjj5RvLBoBAP3FnTK90W9YbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00047.png", "GAOe9QK1Mwz_euUHAMC9R0YOhUBGbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00048.png", "GB469QIj8Yr5hTMBAA9pQPQBqe9obuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00049.png", "GN7m8QLFNhiKZzoBAIbPva_hk2t2buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00050.png", "GLBC8gLZyY6IYXIAAHZbv9ZfDpBRbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00051.png", "GGgV_ALK4ajTXYcAAF6Lpfa9HA0rbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00052.png", "GOU78gJBjMrZSPkGAD7yKQdf1Z87buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00053.png", "GPoU9QL79gLAkA0HAPs78eWdTAMbbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00054.png", "GCjs8wLCOBIKgg4BAASE1Zbwsbh5buZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00055.png", "GKXU9wIz211a0YwAAKZHrCfxXfFFbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00056.png", "GMho8gJQjF_8I7gCAGHDW8ITVc4ybuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00057.png", "GINW9gLyEY5Em-EBAOa_J3hv89cBbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00058.png", "GJbO9wJMpyTzIK8BAMWMVLbvK90PbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00059.png", "GOsr9AL87NRsY_0HABx1czV0WbsvbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00060.png", "GJdy9AJS1fpBbPcHAFw8NNMJvYhWbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00061.png", "GKLV8gLqDVFNdtwBANOXB6VzCQImbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00062.png", "GHCs9AIoB6O7qkkHAEL99vv_k9kabuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00063.png", "GC_h8gLM-dFutuQHAKXnDVnyLrpUbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00064.png", "GBrN8QK9TW5RLecGALWQz1j5Xf1EbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00065.png", "GF-b8wKUrFo3O8cAADyzThymMkkDbuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00066.png", "GA1a8wIWi5Xr0wMBABpV0J0KFRhObuZcAAAD");
	handlePairs.emplace_back(absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00067.png", "GB4Y9AIaZ-RUr80AAFYj0GxmNdszbuZcAAAD");

	return handlePairs;
}

}

}

}

}
