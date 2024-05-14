/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/representativefeaturemap/RepresentativeFeatureMapMain.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Subset.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/MovieFrameProvider.h"
#include "ocean/media/Utilities.h"

#ifdef _WINDOWS
	#include "ocean/platform/win/System.h"
#endif

#include "ocean/tracking/pattern/PatternTrackerCore6DOF.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#endif
#endif

bool createCountingFeatureMap(const IO::File& patternFile, RandomGenerator& randomGenerator, CountingFeatureMap& countingFeatureMap, const bool rotateFrame, const bool shrinkFrame)
{
	ocean_assert(patternFile.exists());

	const Frame pattern = Media::Utilities::loadImage(patternFile());

	if (!pattern.isValid())
	{
		return false;
	}

	Log::info() << " ";
	Log::info() << "Processing pattern: " << patternFile.name();

	countingFeatureMap = CountingFeatureMap(patternFile.name(), pattern, CountingFeatureMap::determineBlobFeatures(pattern, Scalar(6.5), WorkerPool::get().scopedWorker()()));

	unsigned int videoIndex = 0u;

	while (true)
	{
		const IO::File videoFile(patternFile.base() + std::string("_video_") + String::toAString(videoIndex) + std::string(".mp4"));

		if (!videoFile.exists())
		{
			break;
		}

		const Media::MovieRef movie = Media::Manager::get().newMedium(videoFile(), true);

		if (movie.isNull())
		{
			continue;
		}

		Log::info() << "Processing video: " << videoFile.name();

		Media::MovieFrameProvider frameProvider;
		frameProvider.setMovie(movie);

		// we wait until the frame provider has been initialized (until we know the number of frames the provider has)

		unsigned int frameNumber = 0u;
		while (frameNumber == 0u)
		{
			Thread::sleep(1u);
			frameNumber = frameProvider.actualFrameNumber();
		}

		for (unsigned int frameIndex = 0u; frameIndex < frameNumber; ++frameIndex)
		{
			FrameRef frame = frameProvider.synchronFrameRequest(frameIndex);

			if (frame.isNull())
			{
				continue;
			}

			countingFeatureMap.modifyFrameAndDetermineCorrespondences(*frame, randomGenerator, rotateFrame, shrinkFrame, WorkerPool::get().scopedWorker()());

			if (frameIndex % 100u == 0u)
			{
				Log::info() << "Progress: " << frameIndex * 100u / frameNumber << "%";
			}
		}

		videoIndex++;
	}

	return true;
}

CountingFeatureMaps createCountingFeatureMaps(const IO::Directory& directory, const std::string& patternBaseName, const bool rotateFrame, const bool shrinkFrame)
{
	ocean_assert(directory.exists() && !patternBaseName.empty());

	RandomGenerator randomGenerator;

	CountingFeatureMaps countingFeatureMaps;

	for (unsigned int posterIndex = 0u; posterIndex < 26; ++posterIndex)
	{
		const std::string patternName(patternBaseName + std::string("_") + std::string(1, char('A' + posterIndex)) + std::string(".png"));
		const IO::File patternFile(directory + IO::File(patternName));

		if (!patternFile.exists())
		{
			break;
		}

		CountingFeatureMap countingFeatureMap;

		if (createCountingFeatureMap(patternFile, randomGenerator, countingFeatureMap, rotateFrame, shrinkFrame))
		{
			countingFeatureMaps.push_back(std::move(countingFeatureMap));
		}
	}

	return countingFeatureMaps;
}

bool checkFeatures(const IO::File& patternFile, const CV::Detector::Blob::BlobFeatures& posterFeatures, const unsigned int minimalCorrespondences, const bool rotateFrame, const bool shrinkFrame)
{
	RandomGenerator randomGenerator;

	if (!patternFile.exists())
	{
		return false;
	}

	Log::info() << " ";

	unsigned int checkedFrames = 0u;
	unsigned int validFrames = 0u;

	unsigned int videoIndex = 0u;

	while (true)
	{
		const IO::File videoFile(patternFile.base() + std::string("_video_") + String::toAString(videoIndex) + std::string(".mp4"));

		if (!videoFile.exists())
		{
			break;
		}

		const Media::MovieRef movie = Media::Manager::get().newMedium(videoFile(), true);

		if (movie.isNull())
		{
			continue;
		}

		Log::info() << "Checking video: " << videoFile.name();

		Media::MovieFrameProvider frameProvider;
		frameProvider.setMovie(movie);

		// we wait until the frame provider has been initialized (until we know the number of frames the provider has)

		unsigned int frameNumber = 0u;
		while (frameNumber == 0u)
		{
			Thread::sleep(1u);
			frameNumber = frameProvider.actualFrameNumber();
		}

		for (unsigned int frameIndex = 0u; frameIndex < frameNumber; ++frameIndex)
		{
			FrameRef frame = frameProvider.synchronFrameRequest(frameIndex);

			if (frame.isNull())
			{
				continue;
			}

			unsigned int localCheckedFrames = 0u;
			validFrames += CountingFeatureMap::modifyFrameAndCheckCorrespondences(posterFeatures, *frame, randomGenerator, minimalCorrespondences, rotateFrame, shrinkFrame, &localCheckedFrames, WorkerPool::get().scopedWorker()());

			checkedFrames += localCheckedFrames;

			if (frameIndex % 100u == 0u)
			{
				Log::info() << "Progress: " << frameIndex * 100u / frameNumber << "%";
			}
		}

		videoIndex++;
	}

	if (checkedFrames == 0u)
	{
		return false;
	}

	Log::info() << "Overal recognition: " << validFrames * 100u / checkedFrames << "%";

	return true;
}

struct ResourceRegistrations
{
	ResourceRegistrations()
	{
	#ifdef OCEAN_RUNTIME_STATIC
		OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
		OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
		OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
		OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
	#else
		const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
		PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
	#endif
	}

	~ResourceRegistrations()
	{
	#ifdef OCEAN_RUNTIME_STATIC
		OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
		OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
		OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
		OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	#else
		PluginManager::get().release();
	#endif
	}
};

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	ResourceRegistrations resourceRegistrations;

	if (argc < 2)
	{
		Log::info() << "Invalid parameter number.";
		Log::info() << "First, provide the directory in wich the pattern are located.";
		Log::info() << "Optional second, provide the base name of each pattern, (\"pattern\" by default).";
		Log::info() << "Optional third, provide the base name of the resulting feature maps, (\"feature_map\" by default).";

		return 1;
	}

	RandomI::initialize();

	int resultValue = 1;

	const IO::Directory directory = IO::Directory(String::toAString(argv[1]));

	if (!directory.exists())
	{
		Log::info() << "The provided directory \"" << directory() << "\" does not exist!";
		return 1;
	}

	const std::string patternBaseName = argc >= 3 ? String::toAString(argv[2]) : std::string("pattern");
	const std::string featureMapBaseName = argc >= 4 ? String::toAString(argv[3]) : std::string("feature_map");

	// first we determine the 'counting' feature maps
	// the feature maps do not only providing the features but also counting how often a feature is used during matching/recognition of the corresponding pattern

	const bool rotateFrame = true;
	const bool shrinkFrame = true;

	CountingFeatureMaps countingFeatureMaps = createCountingFeatureMaps(directory, patternBaseName, rotateFrame, shrinkFrame);

	Log::info() << " ";
	Log::info() << "We created " << countingFeatureMaps.size() << " individual feature maps, now we will process them...";
	Log::info() << " ";

	// now we identify a small subset of all features that should be enough to represent the entire pattern for recognition tasks

	std::vector<Indices32> representativeFeatureIndexGroups;
	CountingFeatureMap::determineRepresentativeFeatures(countingFeatureMaps, representativeFeatureIndexGroups, 50u, false, WorkerPool::get().scopedWorker()());

	for (size_t n = 0; n < countingFeatureMaps.size(); ++n)
	{
		CV::Detector::Blob::BlobFeatures representativeFeatures = Subset::subset(countingFeatureMaps[n].features(), representativeFeatureIndexGroups[n]);

		for (size_t i = 0; i < representativeFeatures.size(); ++i)
		{
			const Vector2& observation = representativeFeatures[i].observation();
			const Vector3 position(observation.x(), 0, observation.y());

			representativeFeatures[i].setPosition(position);
		}

		const IO::File featureMapFile = directory + IO::File(IO::File(countingFeatureMaps[n].patternName()).base() + std::string("_") + featureMapBaseName + std::string(".opfm"));

		if (Tracking::Pattern::PatternTrackerCore6DOF::writeFeatureMap(featureMapFile(), countingFeatureMaps[n].patternFrame(), representativeFeatures))
		{
			Log::info() << "Created the representative feature map: " << featureMapFile.name();
		}
	}

	Log::info() << " ";
	Log::info() << "Now we check the quality of the representative feature maps...";
	Log::info() << " ";

	for (size_t n = 0; n < representativeFeatureIndexGroups.size(); ++n)
	{
		checkFeatures(directory + IO::File(countingFeatureMaps[n].patternName()), Subset::subset(countingFeatureMaps[n].features(), representativeFeatureIndexGroups[n]), 15u, rotateFrame, shrinkFrame);
	}

	return resultValue;
}
