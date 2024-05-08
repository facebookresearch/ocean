/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/blob/blobmatching/win/BlobMatchingMainWindow.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Rotation.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

BlobMatchingMainWindow::BlobMatchingMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& patternMediaName, const std::string& inputMediaName, const std::string& inputResolution) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	patternMediaName_(patternMediaName),
	inputMediaName_(inputMediaName),
	inputResolution_(inputResolution)
{
	// nothing to do here
}

BlobMatchingMainWindow::~BlobMatchingMainWindow()
{
	// nothing to do here
}

void BlobMatchingMainWindow::onInitialized()
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

	featureMap_ = Tracking::Blob::FeatureMap(patternFrame_, Vector2(1, 0), Scalar(6.5), true, 0, WorkerPool::get().scopedWorker()());

	paintBlobFeatures(patternFrame_, featureMap_.features(), false);
}

void BlobMatchingMainWindow::onIdle()
{
	if (inputMedium_)
	{
		const FrameRef frame(inputMedium_->frame());

		if (frame && frame->isValid() && (frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void BlobMatchingMainWindow::onMouseDoubleClick(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	if (bitmap().width() == 0u || bitmap().height() == 0u)
	{
		return;
	}

	ocean_assert(width() >= clientWidth());
	ocean_assert(height() >= clientHeight());

	resize(width() - clientWidth() + bitmap().width(), height() - clientHeight() + bitmap().height());
}

void BlobMatchingMainWindow::onFrame(const Frame& frame)
{
	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	Frame integralFrame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));

	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements());

	const Scalar threshold = Scalar(6.5);

	ocean_assert(integralFrame.isContinuous());

	CV::Detector::Blob::BlobFeatures blobFeatures;
	CV::Detector::Blob::BlobFeatureDetector::detectFeatures(integralFrame.constdata<uint32_t>(), yFrame.width(), yFrame.height(), CV::Detector::Blob::BlobFeatureDetector::SAMPLING_SPARSE, threshold, false, blobFeatures, WorkerPool::get().scopedWorker()());

	std::sort(blobFeatures.begin(), blobFeatures.end());

#if 1
	CV::Detector::Blob::BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralFrame.constdata<uint32_t>(), frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW, blobFeatures, (unsigned int)(-1), true, WorkerPool::get().scopedWorker()());
#else
	CV::Detector::Blob::BlobFeatureDescriptor::calculateNotOrientedDescriptors(integralFrame.constdata<uint32_t>(), frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, blobFeatures, WorkerPool::get().scopedWorker()());
#endif

	paintBlobFeatures(rgbFrame, blobFeatures, false);

	const unsigned int xStartReference = (max(patternFrame_.width(), rgbFrame.width()) - patternFrame_.width()) / 2u;
	const unsigned int xStartLive = (max(patternFrame_.width(), rgbFrame.width()) - rgbFrame.width()) / 2u;

	ocean_assert(patternFrame_.pixelFormat() == FrameType::FORMAT_RGB24 && rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);
	ocean_assert(patternFrame_.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT && rgbFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	Frame combinedFrame(FrameType(max(patternFrame_.width(), rgbFrame.width()), patternFrame_.height() + rgbFrame.height(), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	combinedFrame.setValue(0x00u);

	combinedFrame.copy(xStartReference, 0u, patternFrame_);
	combinedFrame.copy(xStartLive, patternFrame_.height(), rgbFrame);

	const Tracking::Blob::Correspondences::CorrespondencePairs correspondences(Tracking::Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(blobFeatures, featureMap_.features(), blobFeatures.size(), Scalar(0.1), Scalar(0.7), WorkerPool::get().scopedWorker()()));

	const unsigned char colorGreen[] = {0x00, 0xFF, 0x00};

	Vectors2 patternPoints;
	Vectors2 livePoints;

	patternPoints.reserve(correspondences.size());
	livePoints.reserve(correspondences.size());

	for (Tracking::Blob::Correspondences::CorrespondencePairs::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
	{
		patternPoints.push_back(featureMap_.features()[i->second].observation());
		livePoints.push_back(blobFeatures[i->first].observation());

		const Vector2 patternPoint = featureMap_.features()[i->second].observation() + Vector2(Scalar(xStartReference), 0);
		const Vector2 livePoint = blobFeatures[i->first].observation() + Vector2(Scalar(xStartLive), Scalar(patternFrame_.height()));

		CV::Canvas::line8BitPerChannel<3u, 3u>(combinedFrame.data<uint8_t>(), combinedFrame.width(), combinedFrame.height(), patternPoint.x(), patternPoint.y(), livePoint.x(), livePoint.y(), colorGreen);
	}

	setFrame(combinedFrame);

	if (patternPoints.size() >= 10)
	{
		ocean_assert(patternPoints.size() == livePoints.size());

		RandomGenerator randomGenerator;
		SquareMatrix3 homography(false);
		Indices32 validIndices;

		if (Geometry::RANSAC::homographyMatrix(patternPoints.data(), livePoints.data(), patternPoints.size(), randomGenerator, homography, 4u, true, 200u, Scalar(3 * 3), &validIndices, WorkerPool::get().scopedWorker()()) && validIndices.size() >= 10)
		{
			Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, "Valid correspondences: " + String::toAString(validIndices.size()) + " (" + String::toAString(validIndices.size() * 100 / correspondences.size()) + "%)");
		}
		else
		{
			Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, "Pattern not visible");
		}
	}
	else
	{
		Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, "Not enough correspondences");
	}

	repaint(false);
}

void BlobMatchingMainWindow::paintBlobFeatures(Frame& frame, const CV::Detector::Blob::BlobFeatures& features, const bool shadow)
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

		Tracking::Utilities::paintFeaturePoints(frame, featurePointPositions.data(), featurePointScales.data(), featurePointOrientations.data(), featurePointPositions.size(), iLaplace == 0u ? colorRed : colorGreen, shadow ? colorBlack : nullptr, Vector2(0, 0), WorkerPool::get().scopedWorker()());
	}
}
