/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/homographyimagealigner/HomographyImageAligner.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/tracking/HomographyImageAlignmentSparse.h"
#include "ocean/tracking/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/directshow/DirectShow.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#endif
#endif

HomographyImageAligner::HomographyImageAligner(const std::vector<std::wstring>& commandArguments) :
	trackingNumberFeaturePoints_(150u),
	trackingPatchSize_(15u),
	trackingSubPixelIterations_(4u),
	trackingMaximalOffset_(128u),
	trackingCoarsestLayerRadius_(4u),
	pixelErrorThreshold_(3.0),
	trackingPixelFormat_(FrameType::FORMAT_RGB24),
	trackingZeroMean_(true)
{
#if 0
	// disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	// we check, whether an explicit seed value for the random number generator is provided
	// although it is currently the last command arguments, it must be checked first
	// to ensure that the random number system is initialized as early as possible

	if (commandArguments.size() > 12 && !commandArguments[12].empty())
	{
		const std::string argument(String::toAString(commandArguments[12]));

		int value;
		if (String::isInteger32(argument, &value))
		{
			RandomI::initialize((unsigned int)value);
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid RNG seed value, got: \"" + argument + "\"\nHowever we proceed with a time-based RNG seed value.");
			RandomI::initialize();
		}
	}
	else
	{
		RandomI::initialize();
	}

	// first, we register or load the media plugin(s)
	// if we have a shared runtime we simply load all media plugins available in a specific directory
	// if we have a static runtime we explicitly need to register all plugins we want to use (at compile time)

	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::registerDirectShowLibrary();
		Media::MediaFoundation::registerMediaFoundationLibrary();
		Media::WIC::registerWICLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::registerAVFLibrary();
		Media::ImageIO::registerImageIOLibrary();
	#endif
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());

	// although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif


	// first, we get access to the frame medium that is intended to be used for the tracking

	if (commandArguments.size() > 0 && !commandArguments[0].empty())
	{
		const std::string argument = String::toAString(commandArguments[0]);

        // first we try to get an image sequence

        frameMedium_ = Media::Manager::get().newMedium(argument, Media::Medium::IMAGE_SEQUENCE);

        const Media::ImageSequenceRef imageSequence(frameMedium_);

        if (imageSequence)
        {
            // in the case we have an image sequence as input we want to process the images as fast as possible
            // (not with any specific fps number) so we use the explicit mode

            imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
        }
        else
        {
            // provided command argument seems to be something else but an image sequence
            // so now we try to get any possible medium

            frameMedium_ = Media::Manager::get().newMedium(argument);
        }
	}

	if (frameMedium_.isNull())
	{
		// if the user did not specify a medium, first we try to get a live video with id 1 (often an external web cam - not the builtin camera of a laptop)
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		// if we could not get the device with id 1 we try to get the device with id 0
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (frameMedium_.isNull())
	{
		Platform::Utilities::showMessageBox("Error", "No valid input medium could be found!");

		// the device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)
		return;
	}


	// second, we check whether a desired frame dimension is specified for the input frame medium

	if (commandArguments.size() > 1 && !commandArguments[1].empty())
	{
		const std::string dimension = String::toAString(commandArguments[1]);

		if (dimension == "320x240")
		{
			frameMedium_->setPreferredFrameDimension(320u, 240u);
		}
		else if (dimension == "640x480")
		{
			frameMedium_->setPreferredFrameDimension(640u, 480u);
		}
		else if (dimension == "1280x720")
		{
			frameMedium_->setPreferredFrameDimension(1280u, 720u);
		}
		else if (dimension == "1920x1080")
		{
			frameMedium_->setPreferredFrameDimension(1920u, 1080u);
		}
	}


	// third, we check whether the number of feature points is specified

	if (commandArguments.size() > 2 && !commandArguments[2].empty())
	{
		const std::string argument(String::toAString(commandArguments[2]));

		int value = -1;
		if (String::isInteger32(argument, &value) && value >= 10)
		{
			trackingNumberFeaturePoints_ = (unsigned int)value;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid number of feature points, got: \"" + argument + "\"\nHowever we proceed with the default value (150).");
		}
	}


	// next, we check whether the patch size is specified

	if (commandArguments.size() > 3 && !commandArguments[3].empty())
	{
		const std::string argument(String::toAString(commandArguments[3]));

		int value = -1;
		if (String::isInteger32(argument, &value))
		{
			if (value == 5 || value == 7 || value == 15 || value == 31)
			{
				trackingPatchSize_ = (unsigned int)value;
			}
			else
			{
				Platform::Utilities::showMessageBox("Information", "Invalid patch size, got: \"" + argument + "\"\nHowever we proceed with the default value (15).");
			}
		}
	}


	// next, we check whether the number of sub-pixel iterations is specified

	if (commandArguments.size() > 4 && !commandArguments[4].empty())
	{
		const std::string argument(String::toAString(commandArguments[4]));

		int value = -1;
		if (String::isInteger32(argument, &value) && value >= 0 && value <= 10)
		{
			trackingSubPixelIterations_ = (unsigned int)value;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid sub-pixel iterations, got: \"" + argument + "\"\nHowever we proceed with the default value (4).");
		}
	}


	// next, we check whether the maximal offset between two corresponding feature points between successive video frames is specified

	if (commandArguments.size() > 5 && !commandArguments[5].empty())
	{
		const std::string argument(String::toAString(commandArguments[5]));

		int value = -1;
		if (String::isInteger32(argument, &value) && value >= 2)
		{
			trackingMaximalOffset_ = (unsigned int)value;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid maximal offset value, got: \"" + argument + "\"\nHowever we proceed with the default value (128).");
		}
	}


	// next, the search radius on the coarsest pyramid layer

	if (commandArguments.size() > 6 && !commandArguments[6].empty())
	{
		const std::string argument(String::toAString(commandArguments[6]));

		int value = -1;
		if (String::isInteger32(argument, &value) && value >= 2)
		{
			trackingCoarsestLayerRadius_ = (unsigned int)value;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid coarsest layer search radius, got: \"" + argument + "\"\nHowever we proceed with the default value (4).");
		}
	}


  // next, the RANSAC pixel error threshold is specified

  if (commandArguments.size() > 7 && !commandArguments[7].empty())
  {
    const std::string argument(String::toAString(commandArguments[6]));

    double value = 3.0;
    if (String::isNumber(argument, true, &value) && value > 0)
    {
      pixelErrorThreshold_ = (Scalar)value;
    }
    else
    {
      Platform::Utilities::showMessageBox("Information", "Invalid RANSAC pixel error threshold, got: \"" + argument + "\"\nHowever we proceed with the default value (3.0).");
    }
  }


	// next, we check whether a specific pixel format for tracking is specified

	if (commandArguments.size() > 8 && !commandArguments[8].empty())
	{
		const std::string argument(String::toAString(commandArguments[8]));

		trackingPixelFormat_ = FrameType::translatePixelFormat(argument);

		if (trackingPixelFormat_ == FrameType::FORMAT_UNDEFINED || FrameType::numberPlanes(trackingPixelFormat_) != 1u)
		{
			Platform::Utilities::showMessageBox("Information", "Invalid pixel format, got: \"" + argument + "\"\nHowever we proceed with the default value (RGB24).");
		}
	}


	// next, we check whether a zero mean tracking is requested or not

	if (commandArguments.size() > 9 && !commandArguments[9].empty())
	{
		const std::string argument(String::toAString(commandArguments[9]));

		if (argument == "zeromean")
		{
			trackingZeroMean_ = true;
		}
		else if (argument == "nozeromean")
		{
			trackingZeroMean_ = false;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid tracking property, got: \"" + argument + "\"\nHowever we proceed with the default value (zeromean).");
		}
	}


	// last, we check whether the medium is intended to be looped (as long if the medium is finite)

	bool loopMedium = true;

	if (commandArguments.size() > 10 && !commandArguments[10].empty())
	{
		const std::string argument(String::toAString(commandArguments[10]));

		if (argument == "loop")
		{
			loopMedium = true;
		}
		else if (argument == "noloop")
		{
			loopMedium = false;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid medium property, got: \"" + argument + "\"\nHowever we proceed with the default value (loop).");
		}
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);

	if (finiteMedium)
	{
		finiteMedium->setLoop(loopMedium);
	}


	// we start the medium so that medium will deliver frames and wait for the first frame to be able to receive the matching camera calibration

	frameMedium_->start();
}

HomographyImageAligner::~HomographyImageAligner()
{
	// we do not release the aligner, this should be done by the user before the application ends
}

void HomographyImageAligner::release()
{
	frameMedium_.release();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::unregisterDirectShowLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::WIC::unregisterWICLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::unregisterAVFLibrary();
		Media::ImageIO::unregisterImageIOLibrary();
	#endif
#else
	PluginManager::get().release();
#endif
}

bool HomographyImageAligner::alignNewFrame(Frame& frame, double& time, SquareMatrix3* currentHomographyPrevious, Vectors2* previousPoints, Vectors2* currentPoints, Indices32* validPointIndices, bool* lastFrameReached)
{
	if (lastFrameReached)
	{
		*lastFrameReached = false;
	}

	if (frameMedium_.isNull())
	{
		return false;
	}

	if (lastFrameReached && frameMedium_->stopTimestamp().isValid())
	{
		*lastFrameReached = true;
	}

	// we request the most recent frame from our input medium

	const FrameRef currentFrame = frameMedium_->frame();

	if (currentFrame.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (currentFrame->timestamp() == frameTimestamp_)
	{
		return false;
	}

	frameTimestamp_ = currentFrame->timestamp();

	const Media::ImageSequenceRef imageSequence(frameMedium_);

	// as we will need worker objects in several function calls we simply request one for the remaining function

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	Frame currentFrameRGB;
	if (!CV::FrameConverter::Comfort::convert(*currentFrame, FrameType::FORMAT_RGB24, currentFrameRGB, true, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance_);

	Frame trackingFrame;
	if (!CV::FrameConverter::Comfort::convert(*currentFrame, trackingPixelFormat_, trackingFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(!frame.isValid());

	ocean_assert(trackingCoarsestLayerRadius_ >= 2u);

	// we determine the optimal number of pyramid layers (for the given configuration)

	unsigned int pyramidLayers = previousFramePyramid_.layers();

	if (pyramidLayers == 0u)
	{
		pyramidLayers = CV::FramePyramid::idealLayers(trackingFrame.width(), trackingFrame.height(), trackingPatchSize_, trackingPatchSize_, 2u, trackingMaximalOffset_, trackingCoarsestLayerRadius_);

		if (pyramidLayers == 0u)
		{
			ocean_assert(false && "Image too small!");
			return false;
		}
	}

	// in each iteration we do not create a new pyramid but we use the existing pyramid to increase performance

	if (!currentFramePyramid_.replace8BitPerChannel11(trackingFrame, pyramidLayers, true /*copyFirstLayer*/, scopedWorker()))
	{
		return false;
	}

	if (previousFramePyramid_)
	{
		ocean_assert(trackingNumberFeaturePoints_ >= 10u);
		ocean_assert(trackingSubPixelIterations_ <= 10u);

		// now we apply the actual point tracking and homography calculation

		SquareMatrix3 internalCurrentHomographyPrevious;
		const Scalar squarePixelErrorThreshold = pixelErrorThreshold_ * pixelErrorThreshold_;
		if (Tracking::HomographyImageAlignmentSparse::determineHomographyWithPoints(previousFramePyramid_.finestLayer(), previousFramePyramid_, currentFramePyramid_, CV::SubRegion(), trackingNumberFeaturePoints_, trackingPatchSize_, trackingCoarsestLayerRadius_, true, trackingSubPixelIterations_, internalCurrentHomographyPrevious, scopedWorker(), previousPoints, currentPoints, validPointIndices, squarePixelErrorThreshold, trackingZeroMean_))
		{
			scopedPerformance.release();

			if (currentHomographyPrevious)
			{
				*currentHomographyPrevious = internalCurrentHomographyPrevious;
			}

			// now we create a visual result for the caller

			SquareMatrix3 previousHomographyCurrent;
			if (internalCurrentHomographyPrevious.invert(previousHomographyCurrent))
			{
				Frame copyFrame;
				Tracking::Utilities::alignFramesHomography(currentFrameRGB, rgbPreviousFrame_, previousHomographyCurrent, copyFrame, true, scopedWorker());

				frame = std::move(copyFrame);

				// we assign the timestamp of the current frame to the resulting aligned frame
				frame.setTimestamp(currentFrame->timestamp());
			}
		}
	}

	scopedPerformance.release();

	// we swap both pyramids for the next iteration

	std::swap(currentFramePyramid_, previousFramePyramid_);
	rgbPreviousFrame_ = std::move(currentFrameRGB);

	time = performance_.average();

	if (imageSequence)
	{
		// in the case we use an image sequence as input we simply now activate the next frame
		// (as we use the explicit mode of the sequence)

		imageSequence->forceNextFrame();
	}

	return true;
}

HomographyImageAligner& HomographyImageAligner::operator=(HomographyImageAligner&& homographyImageAligner)
{
	if (this != &homographyImageAligner)
	{
		// the aligner is designed to exist only once
		ocean_assert(frameMedium_.isNull());

		frameMedium_ = std::move(homographyImageAligner.frameMedium_);

		trackingNumberFeaturePoints_ = homographyImageAligner.trackingNumberFeaturePoints_;
		trackingPatchSize_ = homographyImageAligner.trackingPatchSize_;
		trackingSubPixelIterations_ = homographyImageAligner.trackingSubPixelIterations_;
		trackingMaximalOffset_ = homographyImageAligner.trackingMaximalOffset_;
		trackingCoarsestLayerRadius_ = homographyImageAligner.trackingCoarsestLayerRadius_;
		trackingPixelFormat_ = homographyImageAligner.trackingPixelFormat_;
		trackingZeroMean_ = homographyImageAligner.trackingZeroMean_;

		currentFramePyramid_ = std::move(homographyImageAligner.currentFramePyramid_);
		previousFramePyramid_ = std::move(homographyImageAligner.previousFramePyramid_);
		rgbPreviousFrame_ = std::move(homographyImageAligner.rgbPreviousFrame_);

		frameTimestamp_ = homographyImageAligner.frameTimestamp_;
	}

	return *this;
}
