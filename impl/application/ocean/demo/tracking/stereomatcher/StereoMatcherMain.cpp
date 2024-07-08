/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameBlender.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
	#else
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

using namespace Ocean;

/**
 * Just a helper class to ensure that all media plugins are unregistered when this object is disposed.
 */
class ScopedPlugin
{
	public:

		/**
		 * Creates a new object and registers all plugins.
		 */
		inline ScopedPlugin();

		/**
		 * Destructs this object and unregisters all plugins.
		 */
		inline ~ScopedPlugin();
};

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

	const ScopedPlugin scopedPlugin;

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	RandomI::initialize();

	CommandArguments commandArguments;
	commandArguments.registerParameter("left", "l", "The filename of the left stereo image");
	commandArguments.registerParameter("right", "r", "The filename of the right stereo image");
	commandArguments.registerParameter("baseline", "bl", "The maximal baseline between two corresponding feature points (between left and right stereo image), in percentage of the image size, with range [0, 100]", Value(10.0));
	commandArguments.registerParameter("coarsestradius", "cr", "The search radius on the coarsest pyramid layer, in pixel, with range [2, 64]", Value(8));
	commandArguments.registerParameter("binsize", "bs", "The size of each grid bin which are used to filter feature points in the left image, in pixel, with range [5, 200]", Value(40));
	commandArguments.registerParameter("strength", "s", "The minimal strength each feature point must have, with range [0, 255]", Value(25));
	commandArguments.registerParameter("patchsize", "ps", "The size of the image patches when tracking the features, in pixel, either 7, 15, or, 31", Value(15));
	commandArguments.registerParameter("pixelformat", "pf", "The pixel format to be used when tracking, either Y8 or RGB24", Value(std::string("Y8")));
	commandArguments.registerParameter("output", "o", "The filename of the text output file to which all feature coorespondences will be written");
	commandArguments.registerParameter("outputnotmatched", "on", "Optional to output feature correspondences to the output text file which could not be matched, the second coordinate will be set to (-1, -1)");
	commandArguments.registerParameter("imageoutput", "io", "The filename of the image output file showing a blended version of the left and right stereo image with feature corresponences");
	commandArguments.registerParameter("unidirectional", "u", "Optional to use a uni-directional patch matching (instead of a bi-directional matching)");
	commandArguments.registerParameter("zeromean", "z", "Usage of zero-mean SSD, otherwise SSD is used");
	commandArguments.registerParameter("nomulticore", "nm", "Optional to disable multi-core execution, e.g., to guarantee a deterministic behavior");
	commandArguments.registerParameter("initialdownsamplefactor", "idf", "Optional downsample factor which will be applied to both input images before matching, must be a power of two");
	commandArguments.registerParameter("help", "h", "Showing this help");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 1;
	}

	Value leftValue;
	if (!commandArguments.hasValue("left", &leftValue) || !leftValue.isString())
	{
		Log::error() << "No left stereo image defined";
		return 1;
	}

	Value rightValue;
	if (!commandArguments.hasValue("right", &rightValue) || !rightValue.isString())
	{
		Log::error() << "No right stereo image defined";
		return 1;
	}

	Frame leftFrame = Media::Utilities::loadImage(leftValue.stringValue());
	Frame rightFrame = Media::Utilities::loadImage(rightValue.stringValue());

	if (!leftFrame.isValid() || !rightFrame.isValid())
	{
		Log::error() << "Could not load left or right stereo image";
		return 1;
	}

	const bool multicore = !commandArguments.hasValue("nomulticore");

	const Value pixelFormatValue = commandArguments.value("pixelformat");

	const FrameType::PixelFormat pixelFormat = (pixelFormatValue.isString() && String::toLower(pixelFormatValue.stringValue()) == "y8") ? FrameType::FORMAT_Y8 : FrameType::FORMAT_RGB24;

	Frame leftTrackingFrame;
	if (!CV::FrameConverter::Comfort::convert(leftFrame, pixelFormat, leftTrackingFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(multicore)()))
	{
		Log::error() << "Failed to convert left frame";
		return 1;
	}

	Frame rightTrackingFrame;
	if (!CV::FrameConverter::Comfort::convert(rightFrame, pixelFormat, rightTrackingFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().conditionalScopedWorker(multicore)()))
	{
		Log::error() << "Failed to convert left frame";
		return 1;
	}

	Value initialDownsampleFactorValue;
	if (commandArguments.hasValue("initialdownsamplefactor", &initialDownsampleFactorValue) && initialDownsampleFactorValue.isInt())
	{
		int downsampleFactor = initialDownsampleFactorValue.intValue();

		if (downsampleFactor >= 2 && Utilities::isPowerOfTwo((unsigned int)downsampleFactor))
		{
			while (downsampleFactor > 1)
			{
				CV::FrameShrinker::downsampleByTwo11(leftTrackingFrame, WorkerPool::get().scopedWorker()());
				CV::FrameShrinker::downsampleByTwo11(rightTrackingFrame, WorkerPool::get().scopedWorker()());

				downsampleFactor /= 2;
			}
		}
		else if (downsampleFactor != 1u)
		{
			Log::error() << "Invalid downsample factor '" << downsampleFactor << "'";
			return 1;
		}
	}

	const Value baselineValue(commandArguments.value("baseline", true));
	const Value coarsestRadiusValue(commandArguments.value("coarsestradius", true));
	const Value binSizeValue(commandArguments.value("binsize", true));
	const Value strengthValue(commandArguments.value("strength", true));

	const unsigned int maxFrameSize = std::max(std::max(leftTrackingFrame.width(), leftTrackingFrame.height()), std::max(rightTrackingFrame.width(), rightTrackingFrame.height()));

	const unsigned int baseline = (unsigned int)((double(maxFrameSize) * minmax(1.0, baselineValue.float64Value(true), 100.0)) / 100.0);
	const unsigned int coarsestRadius = (unsigned int)(minmax(2, coarsestRadiusValue.intValue(), 64));
	const unsigned int binSize = (unsigned int)(minmax(5, binSizeValue.intValue(), 200));
	const unsigned int stength = (unsigned int)(minmax(0, strengthValue.intValue(), 255));

	const unsigned int horizontalBins = leftTrackingFrame.width() / binSize;
	const unsigned int verticalBins = leftTrackingFrame.height() / binSize;

	Vectors2 leftFeatures = CV::Detector::FeatureDetector::determineHarrisPoints(leftTrackingFrame, CV::SubRegion(), horizontalBins, verticalBins, stength, WorkerPool::get().conditionalScopedWorker(multicore)());

	Log::info() << "Detected features: " << leftFeatures.size();

	Vectors2 rightFeatures;

	const Value patchSizeValue(commandArguments.value("patchsize"));
	const int patchSize = patchSizeValue.isInt() ? patchSizeValue.intValue() : 15;

	const bool unidirectional = commandArguments.hasValue("unidirectional");

	const bool zeromean = commandArguments.hasValue("zeromean");

	Indices32 validIndices;

	switch (patchSize)
	{
		case 7:
		{
			if (unidirectional)
			{
				if (zeromean)
				{
					CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<7u>(leftTrackingFrame, rightTrackingFrame, leftFeatures, leftFeatures, rightFeatures, baseline, coarsestRadius, CV::FramePyramid::DM_FILTER_11, 4u, WorkerPool::get().conditionalScopedWorker(multicore)());
				}
				else
				{
					CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<7u>(leftTrackingFrame, rightTrackingFrame, leftFeatures, leftFeatures, rightFeatures, baseline, coarsestRadius, CV::FramePyramid::DM_FILTER_11, 4u, WorkerPool::get().conditionalScopedWorker(multicore)());
				}

				validIndices = createIndices(leftFeatures.size(), 0u);
			}
			else
			{
				if (zeromean)
				{
					CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<7u>(leftTrackingFrame, rightTrackingFrame, baseline, coarsestRadius, leftFeatures, rightFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, WorkerPool::get().conditionalScopedWorker(multicore)(), &validIndices);
				}
				else
				{
					CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<7u>(leftTrackingFrame, rightTrackingFrame, baseline, coarsestRadius, leftFeatures, rightFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, WorkerPool::get().conditionalScopedWorker(multicore)(), &validIndices);
				}
			}

			break;
		}

		case 31:
		{
			if (unidirectional)
			{
				if (zeromean)
				{
					CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<31u>(leftTrackingFrame, rightTrackingFrame, leftFeatures, leftFeatures, rightFeatures, baseline, coarsestRadius, CV::FramePyramid::DM_FILTER_11, 4u, WorkerPool::get().conditionalScopedWorker(multicore)());
				}
				else
				{
					CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<31u>(leftTrackingFrame, rightTrackingFrame, leftFeatures, leftFeatures, rightFeatures, baseline, coarsestRadius, CV::FramePyramid::DM_FILTER_11, 4u, WorkerPool::get().conditionalScopedWorker(multicore)());
				}

				validIndices = createIndices(leftFeatures.size(), 0u);
			}
			else
			{
				if (zeromean)
				{
					CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<31u>(leftTrackingFrame, rightTrackingFrame, baseline, coarsestRadius, leftFeatures, rightFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, WorkerPool::get().conditionalScopedWorker(multicore)(), &validIndices);
				}
				else
				{
					CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<31u>(leftTrackingFrame, rightTrackingFrame, baseline, coarsestRadius, leftFeatures, rightFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, WorkerPool::get().conditionalScopedWorker(multicore)(), &validIndices);
				}
			}

			break;
		}

		default:
		{
			if (unidirectional)
			{
				if (zeromean)
				{
					CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<15u>(leftTrackingFrame, rightTrackingFrame, leftFeatures, leftFeatures, rightFeatures, baseline, coarsestRadius, CV::FramePyramid::DM_FILTER_11, 4u, WorkerPool::get().conditionalScopedWorker(multicore)());
				}
				else
				{
					CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<15u>(leftTrackingFrame, rightTrackingFrame, leftFeatures, leftFeatures, rightFeatures, baseline, coarsestRadius, CV::FramePyramid::DM_FILTER_11, 4u, WorkerPool::get().conditionalScopedWorker(multicore)());
				}

				validIndices = createIndices(leftFeatures.size(), 0u);
			}
			else
			{
				if (zeromean)
				{
					CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<15u>(leftTrackingFrame, rightTrackingFrame, baseline, coarsestRadius, leftFeatures, rightFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, WorkerPool::get().conditionalScopedWorker(multicore)(), &validIndices);
				}
				else
				{
					CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorder<15u>(leftTrackingFrame, rightTrackingFrame, baseline, coarsestRadius, leftFeatures, rightFeatures, Scalar(0.9 * 0.9), CV::FramePyramid::DM_FILTER_11, WorkerPool::get().conditionalScopedWorker(multicore)(), &validIndices);
				}
			}

			break;
		}
	}

	if (leftFeatures.size() != rightFeatures.size())
	{
		Log::error() << "Internal error";
		return 1;
	}

	Log::info() << "Matched features: " << validIndices.size();

	std::vector<unsigned char> validFeatures = Subset::indices2statements<Index32, 1u>(validIndices, leftFeatures.size());

	const bool outputnotmatched = commandArguments.hasValue("outputnotmatched");

	const Value outputValue(commandArguments.value("output"));

	if (outputValue.isString())
	{
		std::ofstream outputStream(outputValue.stringValue());

		for (size_t n = 0; n < leftFeatures.size(); ++n)
		{
			if (validFeatures[n])
			{
				outputStream << String::toAString(leftFeatures[n].x(), 4u) << " " << String::toAString(leftFeatures[n].y(), 4u) << " " << String::toAString(rightFeatures[n].x(), 4u) << " " << String::toAString(rightFeatures[n].y(), 4u) << std::endl;
			}
			else if (outputnotmatched)
			{
				outputStream << String::toAString(leftFeatures[n].x(), 4u) << " " << String::toAString(leftFeatures[n].y(), 4u) << " -1.0 -1.0" << std::endl;
			}
		}
	}

	const Value imageOutputValue(commandArguments.value("imageoutput"));

	if (imageOutputValue.isString())
	{
		if (CV::FrameBlender::blend<true>(leftTrackingFrame, rightTrackingFrame, 0x80, WorkerPool::get().conditionalScopedWorker(multicore)()))
		{
			CV::FrameConverter::Comfort::change(rightTrackingFrame, FrameType::FORMAT_RGB24, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().conditionalScopedWorker(multicore)());

			for (size_t n = 0; n < leftFeatures.size(); ++n)
			{
				if (validFeatures[n])
				{
					CV::Canvas::line<1u>(rightTrackingFrame, leftFeatures[n], rightFeatures[n], CV::Canvas::green(rightTrackingFrame.pixelFormat()));
				}
				else if (outputnotmatched)
				{
					CV::Canvas::point<5u>(rightTrackingFrame, leftFeatures[n], CV::Canvas::red(rightTrackingFrame.pixelFormat()));
				}
			}

			Media::Utilities::saveImage(rightTrackingFrame, imageOutputValue.stringValue(), false);
		}
	}

	if (outputValue.isNull() && imageOutputValue.isNull())
	{
		Log::error() << "No output defined";
		return 1;
	}

	return 0;
}

inline ScopedPlugin::ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
	OCEAN_APPLY_IF_LINUX(Media::OpenImageLibraries::registerOpenImageLibrariesLibrary());
#endif
}

inline ScopedPlugin::~ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_LINUX(Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary());
#endif
}
