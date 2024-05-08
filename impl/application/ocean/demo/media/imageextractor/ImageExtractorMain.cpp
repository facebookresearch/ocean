/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imageextractor/ImageExtractorMain.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"
#include "ocean/base/Thread.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/MovieFrameProvider.h"
#include "ocean/media/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_WINDOWS)
		#include "ocean/media/directshow/DirectShow.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#else
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

using namespace Ocean;
using namespace Ocean::Media;

/*
 * This demo example shows to individual possibilities to extract frames from a video/movie.
 *
 * The first possibility is the application of the MovieFrameProvider (in combination with a Movie object).
 * While the Movie object delivers the actual video frames, the MovieFrameProvider can be used to access random frames.
 * Thus, the MovieFrameProvider could be used to move forward and backward within the video stream,
 * or to jump between arbitrary frames.
 * To use this option just define USE_FRAME_PROVIDER
 *
 * The second possibility is the application of the Movie object directly without using the MovieFrameProvider.
 * In this case, we can configure the Movie object to deliver video frames as fast as possible,
 * and we can define a callback function which is called whenever a new frame arrives.
 */

// #define USE_FRAME_PROVIDER

#ifndef USE_FRAME_PROVIDER

/**
* Simple helper class that stores some public member parameters.
*/
class FrameHandler
{
	public:

		/**
		* Callback function receiving a new frame from the Movie object.
		* @param frame The new frame, with frame type as delivered by the Movie object, will be valid
		*/
		void onFrame(const Frame& frame, const SharedAnyCamera& /*camera*/)
		{
			ocean_assert(!outputDirectoryAndBaseFilename_.empty());

			timestamp_.toNow();

			if (frameIndex_ >= firstFrameToStore_ && (frameIndex_ - firstFrameToStore_) % storeEveryNthFrame_ == 0u && extractedFrame_ <  maxNumberOfFrames_)
			{
				++extractedFrame_;

				const std::string filenameIndex = useTimestamps_ ? String::toAString(double(frame.timestamp()), 6u) : String::toAString(frameIndex_, 6u);

				std::string filename = outputDirectoryAndBaseFilename_ + image_suffix_ + filenameIndex + ".png";

				Frame frameRGB24;
				if (CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, frameRGB24, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
				{
					if (numLevels_ > 0u)
					{
						const CV::FramePyramid pyramid = CV::FramePyramid(frameRGB24, startLevel_ + numLevels_, false /*copyFirstLayer*/);

						for (unsigned int l = 0u; l < numLevels_; ++l)
						{
							const unsigned int level = startLevel_ + l;

							if (level >= pyramid.layers())
							{
								break;
							}

							filename = outputDirectoryAndBaseFilename_ + image_suffix_ + String::toAString(level, 2u) + "_" + filenameIndex + ".png";

							if (!Media::Utilities::saveImage(pyramid[level], filename, false))
							{
								std::cout << "Could not save " << filename << std::endl;
							}
						}
					}
					else if (!Media::Utilities::saveImage(frameRGB24, filename, false))
					{
						std::cout << "Unfortunately, the extract frame could not be saved." << std::endl;
					}
				}
				else
				{
					ocean_assert(false && "Not supported frame type - should never happen!");
				}
			}

			++frameIndex_;
		}

		/// The directory and the base filename of the resulting images.
		std::string outputDirectoryAndBaseFilename_;

		/// The suffix that goes before the frame number
		std::string image_suffix_ = "_extracted_image_";

		/// The index of the next frame.
		unsigned int frameIndex_ = 0u;

		/// We will not write every frame, but every n-th frame.
		unsigned int storeEveryNthFrame_ = 1u;

		/// Optionally start at a certain frame;
		unsigned int firstFrameToStore_ = 0u;

		/// Optionally only save a certain number of frames
		unsigned int maxNumberOfFrames_ = (unsigned int)(-1);

		/// Starting pyramid level
		unsigned int startLevel_ = 0u;

		/// Final (coarsest) pyramid level
		unsigned int numLevels_ = 0u;

		/// The counter of extracted frame.
		unsigned int extractedFrame_ = 0u;

		/// True, to use the image timestamps instead of the an index for the filename.
		bool useTimestamps_ = false;

		/// The timestamp of the most recent frame;
		Timestamp timestamp_;
};

#endif // USE_FRAME_PROVIDER

/**
 * This class is a simple helper class to implement a scope for registered plugins.
 */
class PluginScope
{
	public:

		/**
		 * Registers all plugins.
		 */
		inline PluginScope();

		/**
		 * Unregisters all plugins.
		 */
		inline ~PluginScope();
};

PluginScope::PluginScope()
{
#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		ImageIO::registerImageIOLibrary();
		AVFoundation::registerAVFLibrary();
	#elif defined(_WINDOWS)
		DirectShow::registerDirectShowLibrary();
		MediaFoundation::registerMediaFoundationLibrary();
		WIC::registerWICLibrary();
	#else
		OpenImageLibraries::registerOpenImageLibrariesLibrary();
	#endif
#endif
}

PluginScope::~PluginScope()
{
#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		AVFoundation::unregisterAVFLibrary();
		ImageIO::unregisterImageIOLibrary();
	#elif defined(_WINDOWS)
		WIC::unregisterWICLibrary();
		MediaFoundation::unregisterMediaFoundationLibrary();
		DirectShow::unregisterDirectShowLibrary();
	#else
		OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	#endif
#endif
}

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	// we forward all information/warning/error messages to the standard output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional: The first command argument is interpreted as input parameter.");
	commandArguments.registerParameter("input", "i", "The input movie/video from which the images will be extracted.");
	commandArguments.registerParameter("output", "o", "In explicit filename of the resulting images, otherwise the images will be created with default file names");
	commandArguments.registerParameter("startFrame", "sf", "The index of the first image to be extracted, otherwise the very first frame", Value(0));
	commandArguments.registerParameter("maximalFrames", "mf", "The maximal number of frames to extract, otherwise as many frames as available", Value(-1));
	commandArguments.registerParameter("everyNthFrame", "en", "The offset between extracted frame, 1 to extract each frame, 2 to extract every second frame etc.", Value(1));
	commandArguments.registerParameter("startLayer", "sl", "The first pyramid layer to be extracted", Value(0));
	commandArguments.registerParameter("numberLayers", "nl", "The number of pyramid layers to be extracted", Value(0));
	commandArguments.registerParameter("useTimestamps", "ut", "When specified, the filenames of the resulting images will contain the timestamps of the frame instead of an index");
	commandArguments.registerParameter("help", "h", "Showing this help output.");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failure when parsing the command arguments.";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	// first we have to register the media plugins

	const PluginScope pluginScope;

	// let's check whether the provided file is valid and exists

	Value inputValue;
	if (!commandArguments.hasValue("input", &inputValue, false, 0u) || !inputValue.isString())
	{
		Log::error() << "No input specified.";
		return 1;
	}

	const IO::File inputMediaFile(inputValue.stringValue());

	if (!inputMediaFile.exists())
	{
		std::cout << "The provided file \"" << inputMediaFile() << "\" does not exist." << std::endl;
		return 1;
	}

	// let's acquire the movie from the media manager

	MovieRef movie = Manager::get().newMedium(inputMediaFile(), Medium::MOVIE, true);

	// we need to check whether the filename could be

	if (movie.isNull())
	{
		std::cout << "The type of the movie is not supported or the movie is damaged." << std::endl;
		return 1;
	}

	const bool useTimestamps = commandArguments.hasValue("useTimestamps");

#ifdef USE_FRAME_PROVIDER

	// the Movie object come with standard playback capabilities
	// however, the Movie object is not able to extract a specific frame defined by an index
	// therefore, we use the MovieFrameProvider that is able to allow a random frame access

	MovieFrameProvider movieFrameProvider;

	// we can specify a preferred frame type - so that we do not need to apply an explicit conversion anymore
	// however, there is no guarantee that the resulting frame has this type
	movieFrameProvider.setPreferredFrameType(FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);

	// now we can connect the frame provider with out movie
	movieFrameProvider.setMovie(movie);


	// we will need to wait until the frame provider has analyzed the entire frame sequence of the provided movie
	// this can take up to several minutes (especially in debug builds)

	std::cout << "The provided movie will be analyzed..." << std::endl;
	std::cout << "The estimated number of frames is " << movieFrameProvider.estimatedFrameNumber() << std::endl;

	const Timestamp startTimestamp(true);

	while (movieFrameProvider.actualFrameNumber() == 0u && Timestamp(true) < startTimestamp + 10.0)
	{
		Thread::sleep(1u);
	}

	std::cout << "Finished analyzing" << std::endl;
	std::cout << "The provided movie exposes " << movieFrameProvider.actualFrameNumber() << std::endl;

	if (movieFrameProvider.actualFrameNumber() == 0u)
	{
		std::cout << "We cannot extract any frame." << std::endl;
		return 1;
	}

	// now we will extract three frames at three 'random' locations
	for (unsigned int n = 0u; n < 3u; ++n)
	{
		// we want frames at position 25%, 50% and 75%
		const unsigned int frameIndex = movieFrameProvider.actualFrameNumber() * (n + 1u) / 4u;

		// we apply a synchronous frame request - with 10 seconds timeout
		const FrameRef frame = movieFrameProvider.synchronFrameRequest(frameIndex, 10.0);

		if (frame)
		{
			std::cout << "We extract the frame with index " << frameIndex << std::endl;
			std::cout << "The frame has the following resolution " << frame->width() << "x" << frame->height() << "," << std::endl;
			std::cout << "and has the following pixel format: " << FrameType::translatePixelFormat(frame->pixelFormat()) << std::endl;

			const std::string filenameIndex = useTimestamps ? String::toAString(frame->timestamp(), 6u) : String::toAString(frameIndex, 6u);

			const IO::Directory movieDirector(inputMediaFile);
			const IO::File imageFile(movieDirector + IO::File(inputMediaFile.baseName() + "_extracted_image_" + filenameIndex + ".png"));

			std::cout << "We will save the extracted file as: " << imageFile() << std::endl;

			if (!Media::Utilities::saveImage(*frame, imageFile(), false))
			{
				std::cout << "Unfortunately, the extract frame could not be saved." << std::endl;
			}
		}
		else
		{
			std::cout << "We failed to extract the frame with index " << frameIndex << std::endl;
		}
	}

	// finally, we release our resources

	movieFrameProvider.setMovie(MovieRef());
	movie.release();

#else // USE_FRAME_PROVIDER

	// we want to receive the frames of the Movie as fast as possible
	// and we want to ensure that we do not drop any frame
	movie->setSpeed(Media::Movie::AS_FAST_AS_POSSIBLE);

	// we pass the frames only once
	movie->setLoop(false);

	const Value outputValue(commandArguments.value("output"));

	FrameHandler frameHandler;

	if (outputValue.isString())
	{
		frameHandler.outputDirectoryAndBaseFilename_ = outputValue.stringValue();
		frameHandler.image_suffix_.clear();
	}
	else
	{
		// let's define the output director (and base filename) for our images
		const IO::Directory movieDirector(inputMediaFile);
		const IO::File imageFileBase(movieDirector + IO::File(inputMediaFile.baseName()));

		frameHandler.outputDirectoryAndBaseFilename_ = imageFileBase();
	}

	Value startFrameValue;
	if (commandArguments.hasValue("startFrame", &startFrameValue, true) && startFrameValue.isInt() && startFrameValue.intValue() >= 0)
	{
		frameHandler.firstFrameToStore_ = (unsigned int)(startFrameValue.intValue());
	}

	Value maximalFramesValue;
	if (commandArguments.hasValue("maximalFrames", &maximalFramesValue, true) && maximalFramesValue.isInt() && maximalFramesValue.intValue() > 0)
	{
		frameHandler.maxNumberOfFrames_ = (unsigned int)(maximalFramesValue.intValue());
	}

	Value everyNthFrameValue;
	if (commandArguments.hasValue("everyNthFrame", &everyNthFrameValue, true) && everyNthFrameValue.isInt() && everyNthFrameValue.intValue() > 0)
	{
		frameHandler.storeEveryNthFrame_ = (unsigned int)(everyNthFrameValue.intValue());
	}

	Value startLayerValue;
	if (commandArguments.hasValue("startLayer", &startLayerValue, true) && startLayerValue.isInt() && startLayerValue.intValue() >= 0)
	{
		frameHandler.startLevel_ = (unsigned int)(startLayerValue.intValue());
	}

	Value numberLayersValue;
	if (commandArguments.hasValue("numberLayers", &numberLayersValue, true) && numberLayersValue.isInt() && numberLayersValue.intValue() > 0)
	{
		frameHandler.numLevels_ = (unsigned int)(numberLayersValue.intValue());
	}

	frameHandler.useTimestamps_ = useTimestamps;

	// we register a callback function which will receive every single frame of the Movie object
	Media::Movie::FrameCallbackScopedSubscription scopedFrameCallbackSubscription = movie->addFrameCallback(std::bind(&FrameHandler::onFrame, &frameHandler, std::placeholders::_1, std::placeholders::_2));

	// we start the Movie
	movie->start();

	// we have to wait until all frames have been processed
	// we add an additional 5-seconds check in case the underlying media infrastructure is not able
	// to recognize whether the last frame has been reached...
	while (movie->stopTimestamp().isInvalid() && (frameHandler.timestamp_.isInvalid() || frameHandler.timestamp_ + 5.0 > Timestamp(true)))
	{
		// let's pause this thread, to reduce the cpu load
		Thread::sleep(1u);
	}

	movie->stop();

	// we release our resources
	scopedFrameCallbackSubscription.release();
	movie.release();

#endif // USE_FRAME_PROVIDER

	return 0;
}
