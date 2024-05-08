/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/rmvtracker/win/RMVTrackerMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

#include "ocean/tracking/Utilities.h"

const Tracking::RMV::RMVFeatureDetector::DetectorType detectorType = Tracking::RMV::RMVFeatureDetector::DT_HARRIS_FEATURE;

const unsigned int numberObjectFeatures = 600u;
const unsigned int numberInitializationObjectFeatures = 120u;

RMVTrackerMainWindow::RMVTrackerMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFilename, const std::string& patternFilename) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	tracker_(detectorType),
	mediaFilename_(mediaFilename),
	patternFilename_(patternFilename)
{
	// nothing to do here
}

RMVTrackerMainWindow::~RMVTrackerMainWindow()
{
	// nothing to do here
}

void RMVTrackerMainWindow::onInitialized()
{
	RandomI::initialize();

	if (!mediaFilename_.empty())
	{
		if (frameMedium_.isNull())
		{
			frameMedium_ = Media::Manager::get().newMedium(mediaFilename_, Media::Medium::FRAME_MEDIUM);
		}
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_)
	{
		frameMedium_->start();

		// we wait until we have the first frame so that we can request the correct camera profile

		const Timestamp startTimestamp(true);
		while (frameMedium_->frame().isNull() && startTimestamp + 5.0 > Timestamp(true))
		{
			Thread::sleep(1u);
		}

		const FrameRef frame = frameMedium_->frame();

		if (frame)
		{
			camera_ = IO::CameraCalibrationManager::get().camera(frameMedium_->url(), frame->width(), frame->height());
		}
	}

	IO::File absoluteFile;

	if (!patternFilename_.empty())
		absoluteFile = IO::File(patternFilename_);

	if (!absoluteFile.exists())
	{
		const IO::Directory environmentDirectory(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		const IO::File relativeFile("/data/tracking/featuremaps/sift640x512.bmp");

		absoluteFile = environmentDirectory + relativeFile;
	}

	if (!absoluteFile.exists())
	{
		absoluteFile = IO::File("points640x512.bmp");
	}

	if (!absoluteFile.exists())
	{
		absoluteFile = IO::File("sift640x512.bmp");
	}

	const Scalar patternWidth = Scalar(0.3); // we take a with of 30cm for any pattern

	if (absoluteFile.exists())
	{
		const Frame pattern = Media::Utilities::loadImage(absoluteFile());

		if (pattern)
		{
			Tracking::RMV::RMVFeatureMap featureMap;
			featureMap.setFeatures(pattern, Vector3(patternWidth, 0, 0), camera_, numberObjectFeatures, detectorType, WorkerPool::get().scopedWorker()());

			featureMap.setInitializationFeatures(pattern, Vector3(patternWidth, 0, 0), camera_, numberInitializationObjectFeatures, detectorType, WorkerPool::get().scopedWorker()());

			tracker_.setFeatureMap(featureMap, randomGenerator_);
		}
	}
}

void RMVTrackerMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && frame->timestamp() != frameTimestamp_)
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Thread::sleep(1u);
}

void RMVTrackerMainWindow::onFrame(const Frame& frame)
{
	if ((performance_.measurements() % 20u) == 0u)
	{
		performance_.reset();
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	performance_.start();

	Tracking::VisualTracker::TransformationSamples transformations;
	if (tracker_.determinePoses(yFrame, camera_, true, transformations, Quaternion(false), WorkerPool::get().scopedWorker()()))
	{
		performance_.stop();

		ocean_assert(!transformations.empty());
		const HomogenousMatrix4& resultingPose = transformations.front().transformation();

		Frame rgbFrame;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, true, WorkerPool::get().scopedWorker()()))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		Box3 boundingBox(tracker_.featureMap().boundingBox());
		if (Numeric::isEqualEps(boundingBox.yDimension()))
		{
			boundingBox += boundingBox.lower() + Vector3(0, boundingBox.diagonal() * Scalar(0.2) , 0);
		}

		const HomogenousMatrix4 resultingPoseIF(PinholeCamera::standard2InvertedFlipped(resultingPose));

		Tracking::Utilities::paintBoundingBoxIF(rgbFrame, resultingPoseIF, AnyCameraPinhole(camera_), boundingBox, CV::Canvas::white(rgbFrame.pixelFormat()), CV::Canvas::black(rgbFrame.pixelFormat()));
		Tracking::Utilities::paintCoordinateSystemIF(rgbFrame, resultingPoseIF, AnyCameraPinhole(camera_), HomogenousMatrix4(true), boundingBox.diagonal() * Scalar(0.1));

		setFrame(rgbFrame);
	}
	else
	{
		performance_.skip();

		setFrame(frame);
	}

	if (performance_.measurements() != 0u)
	{
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(performance_.averageMseconds(), 2u) + std::string("ms"));
	}

	repaint(false);
}
