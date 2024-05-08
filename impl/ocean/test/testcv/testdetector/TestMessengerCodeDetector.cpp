/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

#include "ocean/io/image/Image.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using namespace CV::Detector;

TestMessengerCodeDetector::FileDataCollection::FileDataCollection(std::vector<std::string>&& filenames) :
	filenames_(std::move(filenames))
{
	ocean_assert(!filenames_.empty());
}

SharedTestData TestMessengerCodeDetector::FileDataCollection::data(const size_t index)
{
	ocean_assert(index < filenames_.size());

	if (index >= filenames_.size())
	{
		return nullptr;
	}

	Frame image = IO::Image::readImage(filenames_[index]);

	if (image.isValid())
	{
		if (CV::FrameConverter::Comfort::change(image, FrameType::FORMAT_Y8))
		{
			return std::make_shared<TestData>(std::move(image), Value());
		}
	}

	ocean_assert(false && "Failed to load the image!");
	return nullptr;
}

size_t TestMessengerCodeDetector::FileDataCollection::size()
{
	return filenames_.size();
}

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

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

	const TestDataManager::ScopedSubscriptions scopedSubscriptions = TestMessengerCodeDetector_registerTestDataCollections();

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

#endif // OCEAN_USE_TEST_DATA_COLLECTION

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

} // namespace TestDetector

/**
 * This class implements a simple instance for the GTest ensuring test data collections are registered.
 */
class TestMessengerCodeDetector : public ::testing::Test
{
	protected:

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_USE_TEST_DATA_COLLECTION
			scopedSubscriptions_ = TestCV::TestDetector::TestMessengerCodeDetector_registerTestDataCollections();
#endif // OCEAN_USE_TEST_DATA_COLLECTION
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
			scopedSubscriptions_.clear();
		}

	protected:

		/// The subscriptions to all registered data collections.
		TestDataManager::ScopedSubscriptions scopedSubscriptions_;
};

TEST_F(TestMessengerCodeDetector, ExtractCodeCandidates)
{
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testExtractCodeCandidates(GTEST_TEST_DURATION));
}

TEST_F(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize0)
{
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testBullseyeDetectionArtificial(0u, GTEST_TEST_DURATION));
}

TEST_F(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize3)
{
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testBullseyeDetectionArtificial(3u, GTEST_TEST_DURATION));
}

TEST_F(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize5)
{
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testBullseyeDetectionArtificial(5u, GTEST_TEST_DURATION));
}

TEST_F(TestMessengerCodeDetector, BullseyeDetectionArtificialFilterSize7)
{
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testBullseyeDetectionArtificial(7u, GTEST_TEST_DURATION));
}

TEST_F(TestMessengerCodeDetector, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testStressTest(GTEST_TEST_DURATION, worker));
}

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

TEST_F(TestMessengerCodeDetector, Detect1Bullseye)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testDetect1Bullseye(worker));
}

TEST_F(TestMessengerCodeDetector, Detect0CodeTest)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testDetect0Code(worker));
}

TEST_F(TestMessengerCodeDetector, Detect1CodeTest)
{
	Worker worker;
	EXPECT_TRUE(TestDetector::TestMessengerCodeDetector::testDetect1Code(worker));
}

#endif // OCEAN_USE_TEST_DATA_COLLECTION

namespace TestDetector
{

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

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

bool TestMessengerCodeDetector::testDetect1Bullseye(Worker& worker)
{
	Log::info() << "Detection of exactly 1 Bullseye:";

	bool allSucceeded = true;

	const SharedTestDataCollection dataCollection = TestDataManager::get().testDataCollection("messengercodedetector_1bullseye");

	if (dataCollection && dataCollection->size() > 0)
	{
		for (size_t dataIndex = 0; dataIndex < dataCollection->size(); ++dataIndex)
		{
			const SharedTestData data = dataCollection->data(dataIndex);

			if (data && data->dataType() == TestData::DT_IMAGE)
			{
				Frame yTestImage = Frame(data->image(), Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				ocean_assert(yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8));
				if (yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8))
				{
					const Vector2 imageCenter(Scalar(yTestImage.width()) * Scalar(0.5), Scalar(yTestImage.height()) * Scalar(0.5));
					const Scalar distance5 = Scalar(std::min(yTestImage.width(), yTestImage.height())) * Scalar(0.05);

					// we rotate the image four times by 90 degree

					for (unsigned int rotateIteration = 0u; rotateIteration < 4u; ++rotateIteration)
					{
						for (const bool useWorker : {false, true})
						{
							const Bullseyes bullseyes = CV::Detector::MessengerCodeDetector::detectBullseyes(yTestImage.constdata<uint8_t>(), yTestImage.width(), yTestImage.height(), yTestImage.paddingElements(), useWorker ? &worker : nullptr);

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
							if (!CV::FrameInterpolatorNearestPixel::Comfort::rotate90(yTestImage, rotatedImage, true))
							{
								ocean_assert(false && "Should never happen!");
								allSucceeded = false;
							}

							yTestImage = std::move(rotatedImage);
						}
					}

					continue;
				}
			}

			Log::error() << "Invalid test image with index " << dataIndex;

			allSucceeded = false;
		}
	}
	else
	{
		Log::error() << "Failed to access test data";

		allSucceeded = false;
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

	const SharedTestDataCollection dataCollection = TestDataManager::get().testDataCollection("messengercodedetector_0code");

	if (dataCollection && dataCollection->size() > 0)
	{
		for (size_t dataIndex = 0; dataIndex < dataCollection->size(); ++dataIndex)
		{
			const SharedTestData data = dataCollection->data(dataIndex);

			if (data && data->dataType() == TestData::DT_IMAGE)
			{
				const Frame yTestImage = data->image();

				ocean_assert(yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8));
				if (yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8))
				{
					for (const bool useWorker : {false, true})
					{
						CV::Detector::MessengerCodeDetector::Codes codes = CV::Detector::MessengerCodeDetector::detectMessengerCodes(yTestImage.constdata<uint8_t>(), yTestImage.width(), yTestImage.height(), yTestImage.paddingElements(), useWorker ? &worker : nullptr);

						if (codes.size() != 0)
						{
							allSucceeded = false;
						}
					}

					continue;
				}
			}

			Log::error() << "Invalid test image with index " << dataIndex;

			allSucceeded = false;
		}
	}
	else
	{
		Log::error() << "Failed to access test data";

		allSucceeded = false;
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

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	bool allSucceeded = true;

	const SharedTestDataCollection dataCollection = TestDataManager::get().testDataCollection("messengercodedetector_1code");

	if (dataCollection && dataCollection->size() > 0)
	{
		for (size_t dataIndex = 0; dataIndex < dataCollection->size(); ++dataIndex)
		{
			const SharedTestData data = dataCollection->data(dataIndex);

			if (data && data->dataType() == TestData::DT_IMAGE)
			{
				const Frame yTestImage = data->image();

				ocean_assert(yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8));
				if (yTestImage.isValid() && yTestImage.isPixelFormatCompatible(FrameType::FORMAT_Y8))
				{
					for (const bool useWorker : {false, true})
					{
						CV::Detector::MessengerCodeDetector::Codes codes = CV::Detector::MessengerCodeDetector::detectMessengerCodes(yTestImage.constdata<uint8_t>(), yTestImage.width(), yTestImage.height(), yTestImage.paddingElements(), useWorker ? &worker : nullptr);

						if (codes.size() == 1)
						{
							++validIterations;
						}

						++iterations;
					}

					continue;
				}
			}

			Log::error() << "Invalid test image with index " << dataIndex;

			allSucceeded = false;
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

#endif // OCEAN_USE_TEST_DATA_COLLECTION

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

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

TestDataManager::ScopedSubscriptions TestMessengerCodeDetector_registerTestDataCollections()
{
	std::string absolutePath = "";
	ocean_assert(!absolutePath.empty());

	TestDataManager::ScopedSubscriptions scopedSubscriptions;

	{
		std::vector<std::string> filenames1Bullseye =
		{
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00001.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00002.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00003.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00004.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00005.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00006.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00007.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00008.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00009.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00010.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00011.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00012.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00013.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00014.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00015.png",
			absolutePath + "bullseyes_test_images/1_bullseye/test_image_1bullseye_00016.png"
		};

		scopedSubscriptions.emplace_back(TestDataManager::get().registerTestDataCollection("messengercodedetector_1bullseye", std::make_unique<TestMessengerCodeDetector::FileDataCollection>(std::move(filenames1Bullseye))));
	}

	{
		std::vector<std::string> filenames0Code =
		{
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00001.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00002.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00003.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00004.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00005.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00006.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00007.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00008.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00009.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00010.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00011.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00012.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00013.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00014.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00015.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00016.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00017.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00018.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00019.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00020.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00021.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00022.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00023.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00024.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00025.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00026.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00027.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00028.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00029.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00030.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00031.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00032.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00033.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00034.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00035.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00036.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00037.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00038.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00039.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00040.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00041.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00042.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00043.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00044.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00045.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00046.png",
			absolutePath + "messenger_codes_test_images/0_code/test_image_0code_00047.png"
		};

		scopedSubscriptions.emplace_back(TestDataManager::get().registerTestDataCollection("messengercodedetector_0code", std::make_unique<TestMessengerCodeDetector::FileDataCollection>(std::move(filenames0Code))));
	}

	{
		std::vector<std::string> filenames1Code =
		{
			absolutePath + "messenger_codes_test_images/1_code/test_image_001.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_002.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_003.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_004.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_005.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_006.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_007.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_008.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_009.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_010.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_011.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_012.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_013.png",
			absolutePath + "messenger_codes_test_images/1_code/test_image_014.png",

			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0445.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0446.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0447.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0448.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0449.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0450.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0451.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0452.png",
			absolutePath + "messenger_codes_test_images/far_to_close/IMG_0453.png",

			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00001.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00002.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00003.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00004.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00005.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00006.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00007.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00008.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00009.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00010.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00011.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00012.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00013.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00014.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00015.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00016.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00017.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00018.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00019.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00020.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00021.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00022.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00023.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00024.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00025.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00026.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00027.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00028.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00029.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00030.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00031.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00032.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00033.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00034.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00035.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00036.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00037.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00038.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00039.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00040.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00041.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00042.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00043.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00044.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00045.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00046.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00047.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00048.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00049.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00050.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00051.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00052.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00053.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00054.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00055.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00056.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00057.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00058.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00059.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00060.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00061.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00062.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00063.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00064.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00065.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00066.png",
			absolutePath + "messenger_codes_test_images/1_code_additional/test_image_1code_00067.png"
		};

		scopedSubscriptions.emplace_back(TestDataManager::get().registerTestDataCollection("messengercodedetector_1code", std::make_unique<TestMessengerCodeDetector::FileDataCollection>(std::move(filenames1Code))));
	}

	return scopedSubscriptions;
}

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

}

}

}

}
