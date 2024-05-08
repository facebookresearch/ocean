/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageSequenceStereoImageProvider.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"

namespace Ocean
{

namespace Media
{

ImageSequenceStereoImageProvider::ImageSequenceStereoImageProvider() :
	framesPerSecond_(30.0)
{
	// nothing to do here
}

ImageSequenceStereoImageProvider::ImageSequenceStereoImageProvider(ImageSequenceStereoImageProvider&& provider)
{
	*this = std::move(provider);
}

ImageSequenceStereoImageProvider::ImageSequenceStereoImageProvider(const std::string& imageSequenceFilenameA, const std::string& imageSequenceFilenameB, const std::string& transformationsFilename, const double framesPerSecond) :
	framesPerSecond_(framesPerSecond)
{
	ocean_assert(framesPerSecond_ > NumericD::eps());

	if (framesPerSecond_ <= NumericD::eps())
	{
		return;
	}

	if (!readTransformationsFile(transformationsFilename, fisheyeCameraA_, fisheyeCameraB_, map_world_T_device_, map_device_T_cameraA_, map_device_T_cameraB_, framesPerSecond_))
	{
		return;
	}

	const IO::File imageSequenceFileA(imageSequenceFilenameA);
	const IO::File imageSequenceFileB(imageSequenceFilenameB);

	if (!imageSequenceFileA.exists())
	{
		Log::error() << "The given first image sequence \"" << imageSequenceFileA() << "\" does not exist";
		return;
	}

	if (!imageSequenceFileB.exists())
	{
		Log::error() << "The given second image sequence \"" << imageSequenceFileB() << "\" does not exist";
		return;
	}

	imageSequenceA_ = Media::Manager::get().newMedium(imageSequenceFileA(), Media::Medium::IMAGE_SEQUENCE);
	imageSequenceB_ = Media::Manager::get().newMedium(imageSequenceFileB(), Media::Medium::IMAGE_SEQUENCE);

	if (imageSequenceA_.isNull() || imageSequenceB_.isNull())
	{
		release();

		Log::error() << "Failed to access image sequences";
		return;
	}

	if (size_t(imageSequenceA_->images()) != map_world_T_device_.size() || size_t(imageSequenceB_->images()) != map_world_T_device_.size())
	{
		release();

		Log::error() << "The given image sequences do not fit to the transformations file";
		return;
	}

	imageSequenceA_->setLoop(false);
	imageSequenceA_->setMode(Media::ImageSequence::SM_EXPLICIT);

	imageSequenceB_->setLoop(false);
	imageSequenceB_->setMode(Media::ImageSequence::SM_EXPLICIT);

	imageSequenceA_->start();
	imageSequenceB_->start();
}

bool ImageSequenceStereoImageProvider::nextStereoImages(FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, Frame& imageA, Frame& imageB, HomogenousMatrixD4* world_T_device, HomogenousMatrixD4* device_T_cameraA, HomogenousMatrixD4* device_T_cameraB, HomogenousMatricesD4* /*world_T_leftHandJoints*/, HomogenousMatricesD4* /*world_T_rightHandJoints*/, bool* /*abort*/)
{
	ocean_assert(isValid());

	if (!isValid())
	{
		return false;
	}

	if (imageSequenceA_->stopTimestamp().isValid())
	{
		// we have reached the end of the image sequence

		ocean_assert(imageSequenceB_->stopTimestamp().isValid());

		return false;
	}

	// we request the most recent frame from our input medium

	const FrameRef currentFrameA = imageSequenceA_->frame();
	const FrameRef currentFrameB = imageSequenceB_->frame();

	if (currentFrameA.isNull() || currentFrameB.isNull())
	{
		return false;
	}

	const unsigned int indexA = imageSequenceA_->index();
	const unsigned int indexB = imageSequenceB_->index();

	ocean_assert(indexA == indexB);
	if (indexA != indexB)
	{
		return false;
	}

	// we have exactly one transformation for each stereo image, the timestamp is the index of the frame

	ocean_assert(framesPerSecond_ > NumericD::eps());
	const double timestamp = double(indexA) / double(framesPerSecond_);

	if (world_T_device)
	{
		double timestampDistance = NumericD::maxValue();

		if (!map_world_T_device_.sample(timestamp, TransformationMap::IS_TIMESTAMP_INTERPOLATE, *world_T_device, &timestampDistance) || timestampDistance >= 0.001)
		{
			ocean_assert(false && "Invalid transformation map!");
		}

		ocean_assert(world_T_device->isValid());
	}

	if (device_T_cameraA)
	{
		double timestampDistance = NumericD::maxValue();

		if (!map_device_T_cameraA_.sample(timestamp, TransformationMap::IS_TIMESTAMP_INTERPOLATE, *device_T_cameraA, &timestampDistance) || timestampDistance >= 0.001)
		{
			ocean_assert(false && "Invalid transformation map!");
		}

		ocean_assert(device_T_cameraA->isValid());
	}

	if (device_T_cameraB)
	{
		double timestampDistance = NumericD::maxValue();

		if (!map_device_T_cameraB_.sample(timestamp, TransformationMap::IS_TIMESTAMP_INTERPOLATE, *device_T_cameraB, &timestampDistance) || timestampDistance >= 0.001)
		{
			ocean_assert(false && "Invalid transformation map!");
		}

		ocean_assert(device_T_cameraB->isValid());
	}

	fisheyeCameraA = fisheyeCameraA_;
	fisheyeCameraB = fisheyeCameraB_;

	imageA = Frame(*currentFrameA, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	imageB = Frame(*currentFrameB, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	imageA.setTimestamp(Timestamp(timestamp));
	imageB.setTimestamp(Timestamp(timestamp));

	imageSequenceA_->forceNextFrame();
	imageSequenceB_->forceNextFrame();

	return true;
}

bool ImageSequenceStereoImageProvider::restart()
{
	if (!isValid())
	{
		return false;
	}

	ocean_assert(imageSequenceA_ && imageSequenceB_);

	if (!imageSequenceA_->setPosition(0.0) || !imageSequenceB_->setPosition(0.0))
	{
		return false;
	}

	return imageSequenceA_->start() && imageSequenceB_->start();
}

bool ImageSequenceStereoImageProvider::hasDeviceTransformations() const
{
	return true;
}

bool ImageSequenceStereoImageProvider::hasCameraTransformations() const
{
	return true;
}

bool ImageSequenceStereoImageProvider::isValid() const
{
	if (imageSequenceA_.isNull())
	{
		return false;
	}

	ocean_assert(imageSequenceB_);

	ocean_assert(fisheyeCameraA_.isValid() && fisheyeCameraB_.isValid());
	ocean_assert(!map_world_T_device_.isEmpty());
	ocean_assert(!map_device_T_cameraA_.isEmpty() && !map_device_T_cameraB_.isEmpty());

	return true;
}

void ImageSequenceStereoImageProvider::release()
{
	 fisheyeCameraA_ = FisheyeCamera();
	 fisheyeCameraB_ = FisheyeCamera();

	 map_world_T_device_.clear();
	 map_device_T_cameraA_.clear();
	 map_device_T_cameraB_.clear();

	 imageSequenceA_.release();
	 imageSequenceB_.release();
}

ImageSequenceStereoImageProvider& ImageSequenceStereoImageProvider::operator=(ImageSequenceStereoImageProvider&& provider)
{
	if (this != &provider)
	{
		imageSequenceA_ = std::move(provider.imageSequenceA_);
		imageSequenceB_ = std::move(provider.imageSequenceB_);

		fisheyeCameraA_ = provider.fisheyeCameraA_;
		fisheyeCameraB_ = provider.fisheyeCameraB_;
		provider.fisheyeCameraA_ = FisheyeCamera();
		provider.fisheyeCameraB_ = FisheyeCamera();

		map_world_T_device_ = std::move(provider.map_world_T_device_);
		map_device_T_cameraA_ = std::move(provider.map_device_T_cameraA_);
		map_device_T_cameraB_ = std::move(provider.map_device_T_cameraB_);

		framesPerSecond_ = provider.framesPerSecond_;
		provider.framesPerSecond_ = 30.0;
	}

	return *this;
}

bool ImageSequenceStereoImageProvider::readTransformationsFile(const std::string& transformationsFilename, FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, TransformationMap& map_world_T_device, TransformationMap& map_device_T_cameraA, TransformationMap& map_device_T_cameraB, const double framesPerSecond)
{
	ocean_assert(framesPerSecond > NumericD::eps());
	if (framesPerSecond <= NumericD::eps())
	{
		return false;
	}

	const IO::File transformationsFile(transformationsFilename);

	if (!transformationsFile.exists())
	{
		Log::error() << "The given transformations file \"" << transformationsFile() << "\" does not exist";
		return false;
	}

	std::ifstream transformationsStream;

	transformationsStream.open(transformationsFile());

	if (transformationsStream.fail())
	{
		Log::error() << "Could not load pre-calculated poses of the device";
		return false;
	}

	unsigned int cameraModelWidthA = 0u;
	unsigned int cameraModelHeightA = 0u;
	Scalar cameraModelParametersA[12] = {0};
	transformationsStream >> cameraModelWidthA;
	transformationsStream >> cameraModelHeightA;

	for (unsigned int n = 0u; n < 12u; ++n)
	{
		transformationsStream >> cameraModelParametersA[n];
	}

	if (transformationsStream.fail() || cameraModelWidthA == 0u || cameraModelHeightA == 0u)
	{
		Log::error() << "Failed to parse camera calibration data";
		return false;
	}

	unsigned int cameraModelWidthB = 0u;
	unsigned int cameraModelHeightB = 0u;
	Scalar cameraModelParametersB[12] = {0};
	transformationsStream >> cameraModelWidthB;
	transformationsStream >> cameraModelHeightB;
	for (unsigned int n = 0u; n < 12u; ++n)
	{
		transformationsStream >> cameraModelParametersB[n];
	}

	if (transformationsStream.fail() || cameraModelWidthB == 0u || cameraModelHeightB == 0u)
	{
		Log::error() << "Failed to parse camera calibration data";
		return false;
	}

	fisheyeCameraA = FisheyeCamera(cameraModelWidthA, cameraModelHeightA, FisheyeCamera::PC_12_PARAMETERS, cameraModelParametersA);
	fisheyeCameraB = FisheyeCamera(cameraModelWidthB, cameraModelHeightB, FisheyeCamera::PC_12_PARAMETERS, cameraModelParametersB);

	if (!fisheyeCameraA.isValid() || !fisheyeCameraB.isValid())
	{
		Log::error() << "Failed to parse camera calibration data";
		return false;
	}

	map_world_T_device = TransformationMap(size_t(-1));
	map_device_T_cameraA = TransformationMap(size_t(-1));
	map_device_T_cameraB = TransformationMap(size_t(-1));

	const double frameDuration = 1.0 / double(framesPerSecond);

	double timestmap = 0.0;

	while (true)
	{
		HomogenousMatrixD4 world_T_device(false);
		HomogenousMatrixD4 device_T_cameraA(false);
		HomogenousMatrixD4 device_T_cameraB(false);

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			transformationsStream >> world_T_device[n];
		}

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			transformationsStream >> device_T_cameraA[n];
		}

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			transformationsStream >> device_T_cameraB[n];
		}

		if (transformationsStream.fail() || transformationsStream.eof())
		{
			break;
		}

		if (!world_T_device.isValid() || !device_T_cameraA.isValid() || !device_T_cameraB.isValid())
		{
			Log::error() << "Failed to parse transformations";
			return false;
		}

		map_world_T_device.insert(world_T_device, timestmap);
		map_device_T_cameraA.insert(device_T_cameraA, timestmap);
		map_device_T_cameraB.insert(device_T_cameraB, timestmap);

		timestmap += frameDuration;
	}

	ocean_assert(map_world_T_device.size() == map_device_T_cameraA.size());
	ocean_assert(map_world_T_device.size() == map_device_T_cameraB.size());

	return !map_world_T_device.isEmpty();
}

}

}
