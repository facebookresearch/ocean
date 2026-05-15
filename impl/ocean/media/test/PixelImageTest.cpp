/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include "ocean/media/PixelImage.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector3.h"

namespace
{

using namespace Ocean;
using namespace Ocean::Media;

/**
 * Testable subclass of PixelImage that exposes the protected constructor.
 */
class TestablePixelImage : public PixelImage
{
	public:
		explicit TestablePixelImage(const std::string& url)
			: Medium(url), FrameMedium(url), PixelImage(url)
		{
		}
};

// ============================================================================
// Lifecycle: start / pause / stop
// ============================================================================

TEST(PixelImageTest, InitiallyNotStarted)
{
	TestablePixelImage image("test://initial");

	EXPECT_FALSE(image.isStarted());
}

TEST(PixelImageTest, StartSetsIsStartedAndTimestamps)
{
	TestablePixelImage image("test://start");

	EXPECT_TRUE(image.start());
	EXPECT_TRUE(image.isStarted());

	EXPECT_TRUE(image.startTimestamp().isValid());
	EXPECT_FALSE(image.pauseTimestamp().isValid());
	EXPECT_FALSE(image.stopTimestamp().isValid());
}

TEST(PixelImageTest, PauseClearsIsStartedAndSetsTimestamps)
{
	TestablePixelImage image("test://pause");

	image.start();
	EXPECT_TRUE(image.pause());
	EXPECT_FALSE(image.isStarted());

	EXPECT_FALSE(image.startTimestamp().isValid());
	EXPECT_TRUE(image.pauseTimestamp().isValid());
	EXPECT_FALSE(image.stopTimestamp().isValid());
}

TEST(PixelImageTest, StopClearsIsStartedAndSetsTimestamps)
{
	TestablePixelImage image("test://stop");

	image.start();
	EXPECT_TRUE(image.stop());
	EXPECT_FALSE(image.isStarted());

	EXPECT_FALSE(image.startTimestamp().isValid());
	EXPECT_FALSE(image.pauseTimestamp().isValid());
	EXPECT_TRUE(image.stopTimestamp().isValid());
}

TEST(PixelImageTest, StartAfterStopResetsTimestamps)
{
	TestablePixelImage image("test://restart");

	image.start();
	image.stop();

	// Restart
	EXPECT_TRUE(image.start());
	EXPECT_TRUE(image.isStarted());

	EXPECT_TRUE(image.startTimestamp().isValid());
	EXPECT_FALSE(image.pauseTimestamp().isValid());
	EXPECT_FALSE(image.stopTimestamp().isValid());
}

// ============================================================================
// Camera profile
// ============================================================================

TEST(PixelImageTest, SetCameraAllowsFrameDeliveryWithoutPerFrameCamera)
{
	TestablePixelImage image("test://camera");

	const unsigned int width = 640u;
	const unsigned int height = 480u;
	const Scalar focalLength = Scalar(500);

	PinholeCamera pinholeCamera(width, height, focalLength, focalLength,
		Scalar(width) / Scalar(2), Scalar(height) / Scalar(2));

	SharedAnyCamera camera = std::make_shared<AnyCameraPinhole>(std::move(pinholeCamera));

	EXPECT_TRUE(image.setCamera(std::move(camera)));

	// Verify the stored camera is actually used when delivering a frame without a per-frame camera
	image.start();

	Frame frame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	frame.setTimestamp(Timestamp(1.0));

	EXPECT_TRUE(image.setPixelImage(std::move(frame)));
}

// ============================================================================
// device_T_camera transformation
// ============================================================================

TEST(PixelImageTest, DeviceTCameraDefaultsToIdentityAndCanBeUpdated)
{
	TestablePixelImage image("test://device_t_camera");

	// Verify default is identity
	const HomogenousMatrixD4 identity(true);
	EXPECT_EQ(image.device_T_camera(), identity);

	// Update to a translation-only transform and verify it persists
	const HomogenousMatrixD4 translationTransform(VectorD3(1.0, 2.0, 3.0));
	image.setDevice_T_camera(translationTransform);

	EXPECT_EQ(image.device_T_camera(), translationTransform);
	EXPECT_NE(image.device_T_camera(), identity);
}

// ============================================================================
// setPixelImage (move overload)
// ============================================================================

TEST(PixelImageTest, SetPixelImageFailsWhenNotStarted)
{
	TestablePixelImage image("test://not_started");

	Frame frame(FrameType(10u, 10u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	frame.setTimestamp(Timestamp(1.0));

	EXPECT_FALSE(image.setPixelImage(std::move(frame)));
}

TEST(PixelImageTest, SetPixelImageSucceedsWhenStartedWithCamera)
{
	TestablePixelImage image("test://started_with_camera");

	image.start();

	Frame frame(FrameType(10u, 10u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	frame.setTimestamp(Timestamp(1.0));

	PinholeCamera pinholeCamera(10u, 10u, Scalar(10), Scalar(10), Scalar(5), Scalar(5));
	SharedAnyCamera camera = std::make_shared<AnyCameraPinhole>(std::move(pinholeCamera));

	EXPECT_TRUE(image.setPixelImage(std::move(frame), std::move(camera)));
}

TEST(PixelImageTest, SetPixelImageSucceedsWhenStartedWithoutCamera)
{
	TestablePixelImage image("test://started_no_camera");

	image.start();

	Frame frame(FrameType(10u, 10u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	frame.setTimestamp(Timestamp(1.0));

	// No camera provided — should use the default camera_ (which is nullptr)
	EXPECT_TRUE(image.setPixelImage(std::move(frame)));
}

// ============================================================================
// setPixelImage (const ref overload)
// ============================================================================

TEST(PixelImageTest, SetPixelImageConstRefSucceedsWhenStarted)
{
	TestablePixelImage image("test://const_ref");

	image.start();

	const Frame frame(FrameType(10u, 10u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT),
		Indices32(), Timestamp(2.0));

	PinholeCamera pinholeCamera(10u, 10u, Scalar(10), Scalar(10), Scalar(5), Scalar(5));
	SharedAnyCamera camera = std::make_shared<AnyCameraPinhole>(std::move(pinholeCamera));

	// The const ref overload copies the frame internally
	EXPECT_TRUE(image.setPixelImage(frame, std::move(camera)));

	// The original frame should still be valid since it was passed by const ref
	EXPECT_TRUE(frame.isValid());
	EXPECT_EQ(frame.width(), 10u);
	EXPECT_EQ(frame.height(), 10u);
}

TEST(PixelImageTest, SetPixelImageConstRefFailsWhenNotStarted)
{
	TestablePixelImage image("test://const_ref_not_started");

	const Frame frame(FrameType(10u, 10u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT),
		Indices32(), Timestamp(3.0));

	EXPECT_FALSE(image.setPixelImage(frame));
}

// ============================================================================
// Timestamp accessors after lifecycle transitions
// ============================================================================

TEST(PixelImageTest, PauseAfterPauseStillSetsTimestamp)
{
	TestablePixelImage image("test://double_pause");

	image.start();
	image.pause();

	const Timestamp firstPause = image.pauseTimestamp();
	EXPECT_TRUE(firstPause.isValid());

	// Pause again
	EXPECT_TRUE(image.pause());
	EXPECT_TRUE(image.pauseTimestamp().isValid());
}

} // anonymous namespace
