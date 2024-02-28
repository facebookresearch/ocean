// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testexternal/TestPerceptionUtilities.h"

#include "ocean/base/Timestamp.h"

#include "ocean/cv/PerceptionUtilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestExternal
{

bool TestPerceptionUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   PerceptionUtilities test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFromFisheyeCamera(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "PerceptionUtilities test succeeded.";
	}
	else
	{
		Log::info() << "PerceptionUtilities test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestPerceptionUtilities, FromFisheyeCamera)
{
	EXPECT_TRUE(TestPerceptionUtilities::testFromFisheyeCamera(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestPerceptionUtilities::testFromFisheyeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "From FisheyeCamera:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(100u, 1920u);
		const unsigned int height = RandomI::random(100u, 1080u);

		const double focalLength = RandomD::scalar(0.5, 1.5) * double(width);

		const double principalX = RandomD::scalar(0.1, 0.9) * double(width);
		const double principalY = RandomD::scalar(0.1, 0.9) * double(height);

		std::vector<double> radialDistortions(6);
		for (double& radialDistortion : radialDistortions)
		{
			radialDistortion = RandomD::scalar(-0.001, 0.001);
		}

		std::vector<double> tangentialDistortions(2);
		for (double& tangentialDistortion : tangentialDistortions)
		{
			tangentialDistortion = RandomD::scalar(-0.001, 0.001);
		}

		const FisheyeCameraD fisheyeCamera(width, height, focalLength, focalLength, principalX, principalY, radialDistortions.data(), tangentialDistortions.data());

		const std::unique_ptr<perception::CameraModelInterface<double>> perceptionModel = CV::PerceptionUtilities::fromFisheyeCamera<double, double>(fisheyeCamera);

		FisheyeCameraD testFisheyeCamera;
		if (!perceptionModel || !CV::PerceptionUtilities::toFisheyeCamera<double, double>(*perceptionModel, testFisheyeCamera) || fisheyeCamera != testFisheyeCamera)
		{
			allSucceeded = false;
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

}

}

}

}
