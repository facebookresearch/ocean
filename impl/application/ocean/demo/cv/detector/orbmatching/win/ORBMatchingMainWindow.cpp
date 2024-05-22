/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/orbmatching/win/ORBMatchingMainWindow.h"

#include "ocean/base/HighperformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"
#include "ocean/cv/detector/ORBFeatureOrientation.h"

#include "ocean/math/Rotation.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

#include "ocean/tracking/Utilities.h"

ORBMatchingMainWindow::ORBMatchingMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& patternMediaName, const std::string& inputMediaName, const std::string& inputResolution) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	patternMediaName_(patternMediaName),
	inputMediaName_(inputMediaName),
	inputResolution_(inputResolution),
	useHarrisFeatures_(false),
	featureThreshold_(25u)
{
	// nothing to do here
}

ORBMatchingMainWindow::~ORBMatchingMainWindow()
{
	// nothing to do here
}

void ORBMatchingMainWindow::onInitialized()
{
	ocean_assert(!patternFrame_.isValid());

	if (!patternMediaName_.empty())
	{
		patternFrame_ = Media::Utilities::loadImage(patternMediaName_);
	}

	if (!patternFrame_.isValid())
	{
		patternFrame_ = Media::Utilities::loadImage(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH") + std::string("/data/testsuite/media/images/tracking/sift640.bmp"));
	}

	if (!patternFrame_.isValid())
	{
		patternFrame_ = Media::Utilities::loadImage(std::string("sift640.bmp"));
	}

	if (!patternFrame_.isValid())
	{
		Platform::Utilities::showMessageBox("Error", "Could not load pattern!");
		return;
	}

	if (!CV::FrameConverter::Comfort::change(patternFrame_, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, true, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (!inputMediaName_.empty())
	{
		inputMedium_ = Media::Manager::get().newMedium(inputMediaName_);
	}

	if (inputMedium_.isNull())
	{
		inputMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (inputMedium_.isNull())
	{
		inputMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (inputMedium_)
	{
		if (!inputResolution_.empty())
		{
			if (inputResolution_ == "320x240")
			{
				inputMedium_->setPreferredFrameDimension(320u, 240u);
			}
			else if (inputResolution_ == "640x480")
			{
				inputMedium_->setPreferredFrameDimension(640u, 480u);
			}
			else if (inputResolution_ == "1280x720")
			{
				inputMedium_->setPreferredFrameDimension(1280u, 720u);
			}
			else if (inputResolution_ == "1920x1080")
			{
				inputMedium_->setPreferredFrameDimension(1920u, 1080u);
			}
		}

		inputMedium_->start();
	}

	Frame yPatterFrame;
	if (!CV::FrameConverter::Comfort::convert(patternFrame_, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yPatterFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	const unsigned int pyramidLayers = 6u;

	CV::Detector::ORBFeatureDescriptor::detectReferenceFeaturesAndDetermineDescriptors(yPatterFrame, patternFeaturePoints_, pyramidLayers, useHarrisFeatures_, featureThreshold_, WorkerPool::get().scopedWorker()());

	paintFeatures(patternFrame_, patternFeaturePoints_, false);
}

void ORBMatchingMainWindow::onIdle()
{
	if (inputMedium_)
	{
		const FrameRef frame(inputMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void ORBMatchingMainWindow::onMouseDoubleClick(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	if (bitmap().width() == 0u || bitmap().height() == 0u)
	{
		return;
	}

	ocean_assert(width() >= clientWidth());
	ocean_assert(height() >= clientHeight());

	resize(width() - clientWidth() + bitmap().width(), height() - clientHeight() + bitmap().height());
}

void ORBMatchingMainWindow::onFrame(const Frame& frame)
{
	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	const float orbThreshold = 0.25f;

	CV::Detector::ORBFeatures featurePoints;

	if (useHarrisFeatures_)
	{
		CV::Detector::HarrisCorners harrisCorners;
		CV::Detector::HarrisCornerDetector::detectCorners(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), featureThreshold_, true, harrisCorners, true, scopedWorker());

		featurePoints = CV::Detector::ORBFeature::features2ORBFeatures(harrisCorners, yFrame.width(), yFrame.height());
	}
	else
	{
		CV::Detector::FASTFeatures featuresFAST;
		CV::Detector::FASTFeatureDetector::detectFeatures(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), featureThreshold_, false, true, featuresFAST, yFrame.paddingElements(), scopedWorker());

		featurePoints = CV::Detector::ORBFeature::features2ORBFeatures(featuresFAST, yFrame.width(), yFrame.height());
	}

	// now, we describe the detected features points with ORB descriptors

	Frame integralFrame = CV::IntegralImage::Comfort::createLinedImage(yFrame);

	ocean_assert(integralFrame.isContinuous());
	CV::Detector::ORBFeatureOrientation::determineFeatureOrientation(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), featurePoints, scopedWorker());
	CV::Detector::ORBFeatureDescriptor::determineDescriptors(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), featurePoints, true /*useMultiLayers*/, scopedWorker());

	paintFeatures(rgbFrame, featurePoints, false);

	const unsigned int xStartReference = (std::max(patternFrame_.width(), rgbFrame.width()) - patternFrame_.width()) / 2u;
	const unsigned int xStartLive = (std::max(patternFrame_.width(), rgbFrame.width()) - rgbFrame.width()) / 2u;

	ocean_assert(patternFrame_.pixelFormat() == FrameType::FORMAT_RGB24 && rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);
	ocean_assert(patternFrame_.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT && rgbFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	Frame combinedFrame(FrameType(std::max(patternFrame_.width(), rgbFrame.width()), patternFrame_.height() + rgbFrame.height(), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	combinedFrame.setValue(0x00u);

	combinedFrame.copy(xStartReference, 0u, patternFrame_);
	combinedFrame.copy(xStartLive, patternFrame_.height(), rgbFrame);

	// we determine matching ORB features

	IndexPairs32 matches;
	CV::Detector::ORBFeatureDescriptor::determineNonBijectiveCorrespondences(featurePoints, patternFeaturePoints_, matches, orbThreshold, scopedWorker());

	for (const IndexPair32& match : matches)
	{
		const Vector2 featurePoint = featurePoints[match.first].observation() + Vector2(Scalar(xStartLive), Scalar(patternFrame_.height()));
		const Vector2 patternPoint = patternFeaturePoints_[match.second].observation() + Vector2(Scalar(xStartReference), 0);

		CV::Canvas::line<3u>(combinedFrame, patternPoint, featurePoint, CV::Canvas::green(combinedFrame.pixelFormat()));
	}

	setFrame(combinedFrame);

	repaint(false);
}

void ORBMatchingMainWindow::paintFeatures(Frame& frame, const CV::Detector::ORBFeatures& features, const bool shadow)
{
	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_RGB24 && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	Vectors2 featurePointPositions;
	Scalars featurePointScales;
	Scalars featurePointOrientations;

	featurePointPositions.reserve(features.size());
	featurePointScales.reserve(features.size());
	featurePointOrientations.reserve(features.size());

	for (size_t n = 0; n < features.size(); ++n)
	{
		featurePointPositions.push_back(features[n].observation());
		featurePointScales.push_back(10);
		featurePointOrientations.push_back(Numeric::angleAdjustPositive(features[n].orientation()));
	}

	Tracking::Utilities::paintFeaturePoints(frame, featurePointPositions.data(), featurePointScales.data(), featurePointOrientations.data(), featurePointPositions.size(), CV::Canvas::green(), shadow ? CV::Canvas::black() : nullptr, Vector2(0, 0), WorkerPool::get().scopedWorker()());
}
