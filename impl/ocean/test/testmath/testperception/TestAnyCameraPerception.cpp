// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testmath/testperception/TestAnyCameraPerception.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include "ocean/math/AnyCameraPerception.h"

#include <perception/camera/CameraModelFactory.h>
#include <perception/camera/CameraModelInterface.h>

namespace Ocean
{

namespace Test
{

namespace TestMath
{

namespace TestPerception
{

bool TestAnyCameraPerception::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   AnyCameraPerception test:   ---";
	Log::info() << " ";

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "AnyCameraPerception test succeeded.";
	}
	else
	{
		Log::info() << "AnyCameraPerception test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAnyCameraPerception, Constructor)
{
	EXPECT_TRUE(TestAnyCameraPerception::testConstructor(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestAnyCameraPerception::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "AnyCameraPerception constructor test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const std::vector<double> intrinsics = {500.0, 500.0, 1920.0 * 0.5, 1080.0 * 0.5, 0.0, 0.0, 0.0, 0.0};
	std::unique_ptr<perception::CameraModelInterface<double>> cameraModelInterface = perception::createModel(perception::CameraModelType::RADTAN, perception::ImageSize(1920u, 1080u), intrinsics, false);

	const AnyCameraPerceptionD anyCameraPerception(std::move(cameraModelInterface));

	if (TestAnyCamera::verifyAnyCamera(anyCameraPerception, &randomGenerator) != VR_SUCCEEDED)
	{
		allSucceeded = false;
	}

	for (size_t n = 0; n < intrinsics.size(); ++n)
	{
		std::vector<double> changedIntrinsics(intrinsics);

		changedIntrinsics[n] += RandomD::scalar(0.1, 10.0) * RandomD::sign();

		std::unique_ptr<perception::CameraModelInterface<double>> changedCameraModelInterface = perception::createModel(perception::CameraModelType::RADTAN, perception::ImageSize(1920u, 1080u), changedIntrinsics, false);

		const AnyCameraPerceptionD changedAnyCameraPerception(std::move(changedCameraModelInterface));

		if (anyCameraPerception.isEqual(changedAnyCameraPerception))
		{
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

}

}

}

}
