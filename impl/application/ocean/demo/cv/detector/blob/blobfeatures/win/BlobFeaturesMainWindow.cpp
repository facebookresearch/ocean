// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "application/ocean/demo/cv/detector/blob/blobfeatures/win/BlobFeaturesMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

#include "ocean/tracking/Utilities.h"

using namespace Ocean;

BlobFeaturesMainWindow::BlobFeaturesMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& inputMediaName, const std::string& inputResolution) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	inputMediaName_(inputMediaName),
	inputResolution_(inputResolution),
	blobFeatureThreshold_(15),
	ignoreTimestamp_(false),
	calculateOrientationAndDescriptor_(true)
{
	// nothing to do here
}

BlobFeaturesMainWindow::~BlobFeaturesMainWindow()
{
	// nothing to do here
}

void BlobFeaturesMainWindow::onInitialized()
{
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

	if (Media::FiniteMediumRef finiteMedium = inputMedium_)
	{
		finiteMedium->setLoop(true);
	}

	if (inputMedium_)
	{
		inputMedium_->start();
	}
}

void BlobFeaturesMainWindow::onIdle()
{
	if (inputMedium_)
	{
		const FrameRef frame(inputMedium_->frame());

		if (frame && *frame && (frame->timestamp() != frameTimestamp_ || ignoreTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void BlobFeaturesMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		if (keyString == "up")
		{
			if (blobFeatureThreshold_ < 1000)
				blobFeatureThreshold_ += 10;
		}
		else if (keyString == "down")
		{
			if (blobFeatureThreshold_ > 5)
				blobFeatureThreshold_ -= 10;
		}
		else if (keyString == "D")
		{
			calculateOrientationAndDescriptor_ = !calculateOrientationAndDescriptor_;
		}
		else
		{
			ignoreTimestamp_ = !ignoreTimestamp_;
		}
	}
	else
	{
		ignoreTimestamp_ = !ignoreTimestamp_;
	}
}

void BlobFeaturesMainWindow::onFrame(const Frame& frame)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	HighPerformanceTimer timer;

	constexpr unsigned int integralFramePaddingElements = 0u;
	Frame integralFrame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), integralFramePaddingElements);
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements());

	ocean_assert(integralFrame.isContinuous());

	CV::Detector::Blob::BlobFeatures blobFeatures;
	CV::Detector::Blob::BlobFeatureDetector::detectFeatures(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), CV::Detector::Blob::BlobFeatureDetector::SAMPLING_SPARSE, blobFeatureThreshold_, false, blobFeatures, WorkerPool::get().scopedWorker()());

	if (calculateOrientationAndDescriptor_)
	{
		CV::Detector::Blob::BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW, blobFeatures, true, WorkerPool::get().scopedWorker()());
	}

	const double performance = timer.mseconds();

	paintBlobFeatures(rgbFrame, blobFeatures);

	setFrame(rgbFrame);

	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, std::string("Performance: ") + String::toAString(performance, 2u));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 25, std::string("Threshold: ") + String::toAString(blobFeatureThreshold_));
	Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 45, std::string("Features: ") + String::toAString(blobFeatures.size()));

	repaint(false);
}

void BlobFeaturesMainWindow::paintBlobFeatures(Frame& frame, const CV::Detector::Blob::BlobFeatures& features)
{
	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_RGB24 && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	const uint8_t* const colorGreen = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const colorRed = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const colorBlack = CV::Canvas::black(frame.pixelFormat());

	Vectors2 featurePointPositions;
	Scalars featurePointScales;
	Scalars featurePointOrientations;

	featurePointPositions.reserve(features.size());
	featurePointScales.reserve(features.size());
	featurePointOrientations.reserve(features.size());

	for (unsigned int iLaplace = 0u; iLaplace < 2u; ++iLaplace)
	{
		featurePointPositions.clear();
		featurePointScales.clear();
		featurePointOrientations.clear();

		for (size_t n = 0; n < features.size(); ++n)
		{
			if ((iLaplace == 0u) == features[n].laplace())
			{
				featurePointPositions.push_back(features[n].observation());
				featurePointScales.push_back(features[n].scale() * 2); // **TODO** precise documentation regarding a feature's scale necessary
				featurePointOrientations.push_back(Numeric::angleAdjustPositive(-features[n].orientation())); // **TODO** precise documentation regarding a feature's orientation necessary
			}
		}

		Tracking::Utilities::paintFeaturePoints(frame, featurePointPositions.data(), featurePointScales.data(), featurePointOrientations.data(), featurePointPositions.size(), iLaplace == 0u ? colorRed : colorGreen, colorBlack, Vector2(0, 0), WorkerPool::get().scopedWorker()());
	}
}
