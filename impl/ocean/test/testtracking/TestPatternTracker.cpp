/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestPatternTracker.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/media/android/Android.h"
	#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/PowerMonitor.h"
	#include "ocean/platform/android/Processor.h"

	#include "ocean/platform/linux/Utilities.h"

	#include "ocean/tracking/pattern/PatternTracker6DOF.h"
#endif

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestPatternTracker::test(const double /*testDuration*/, Worker& /*worker*/)
{
	bool allSucceeded = true;

	Log::info() << "---   PatternTracker test:   ---";
	Log::info() << " ";

	allSucceeded = testPowerConsumption() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "PatternTracker test succeeded.";
	}
	else
	{
		Log::info() << "PatternTracker test FAILED";
	}

	return allSucceeded;
}

bool TestPatternTracker::testPowerConsumption()
{
	Log::info() << "Power consumption test:";
	Log::info() << " ";

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	if (!Platform::Linux::Utilities::checkSecurityEnhancedLinuxStateIsPermissive())
	{
		Log::info() << "The enhanced linux state is not permissive, can be changed with 'setenforce 0'";
		Log::info() << "Thus, skipping the test and rating it as succeeded.";

		return true;
	}

	bool allSucceeded = true;

	const std::string inputVideo = ""; // **TODO** set both paths before running the test
	const std::string patternFile = "";

	if (inputVideo.empty() || patternFile.empty())
	{
		Log::info() << "Skipping test as now input video/pattern is defined";

		return true;
	}

	uint32_t cpuMask;
	if (Platform::Android::Processor::currentThreadAffinity(cpuMask))
	{
		std::string binaryMask(16, '0');

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			if (cpuMask & (1u << n))
			{
				binaryMask[16u - n - 1u] = '1';
			}
		}

		Log::info() << "Current thread cpu core affinity mask: 0b" << binaryMask;
		Log::info() << " ";
	}

	bool isCharging = true;
	if (Platform::Android::Battery::isCharging(isCharging))
	{
		Log::info() << "Battery is " << (isCharging ? "charging" : "not charging");
	}
	else
	{
		Log::info() << "Unknown whether battery is charging";
	}

	Log::info() << " ";

	HighPerformanceStatistic performance;

#ifdef OCEAN_RUNTIME_STATIC
	Media::Android::registerAndroidLibrary();
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
#endif

	const double updateFrequency = Platform::Android::PowerMonitor::determineIdealUpdateFrequency();

	if (updateFrequency > 0.0)
	{
		Log::info() << "Using update frequency: " << updateFrequency << "Hz";
		Log::info() << " ";

		constexpr unsigned int numberIterations = 10u;

		for (unsigned int iteration = 0u; iteration < numberIterations; ++iteration)
		{
			Platform::Android::PowerMonitor trackerPowerMonitor(updateFrequency);
			Platform::Android::PowerMonitor idlePowerMonitor(updateFrequency);

			size_t trackerValidPoses = 0;
			size_t trackerNumberFrames = 0;

			size_t idleValidPoses = 0;
			size_t idleNumberFrames = 0;

			for (const bool trackerIteration : {false, true})
			{
				Platform::Android::PowerMonitor& powerMonitor = trackerIteration ? trackerPowerMonitor : idlePowerMonitor;

				Tracking::Pattern::PatternTracker6DOF patternTracker;

				if (patternTracker.addPattern(patternFile, Vector2(1, 0)) == (unsigned int)(-1))
				{
					Log::error() << "Failed to add pattern '" << patternFile << "'";

					allSucceeded = false;
					break;
				}

				const Media::MovieRef movie = Media::Manager::get().newMedium(inputVideo, Media::Medium::MOVIE);

				if (!movie)
				{
					Log::error() << "Failed to load movie '" << inputVideo << "'";

					allSucceeded = false;
					break;
				}

				Thread::sleep(2u * 1000u); // sleep 2 seconds before starting a measurement

				const Platform::Android::PowerMonitor::ScopedSubscription scopedMeasurement(powerMonitor.startMeasurement());

				if (!movie->start())
				{
					Log::error() << "Failed to start movie";

					allSucceeded = false;
					break;
				}

				size_t& validPoses = trackerIteration ? trackerValidPoses : idleValidPoses;
				size_t& numberFrames = trackerIteration ? trackerNumberFrames : idleNumberFrames;

				Timestamp frameTimestamp;

				PinholeCamera pinholeCamera;

				while (!movie->taskFinished())
				{
					const FrameRef frame = movie->frame();

					if (!frame || frame->timestamp() == frameTimestamp)
					{
						Thread::sleep(1u);
						continue;
					}

					++numberFrames;

					frameTimestamp = frame->timestamp();

					if (pinholeCamera.width() != frame->width() || pinholeCamera.height() != frame->height())
					{
						pinholeCamera = PinholeCamera(frame->width(), frame->height(), Numeric::deg2rad(60));
					}

					if (trackerIteration)
					{
						const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

						Tracking::VisualTracker::TransformationSamples transformations;
						if (patternTracker.determinePoses(*frame, pinholeCamera, false, transformations, Quaternion(false)))
						{
							if (!transformations.empty())
							{
								++validPoses;
							}
						}
					}

					Thread::sleep(1u);
				}
			}

			Log::info() << "Iteration " << iteration << ":";

			double idleAveragePower = 0.0;
			double idleStandardDeviation = -1.0;
			size_t idleNumberMeasurements = 0;
			double idleMedian = 0.0;

			if (idlePowerMonitor.averagePower(idleAveragePower, &idleStandardDeviation, &idleNumberMeasurements) && idlePowerMonitor.medianPower(idleMedian))
			{
				Log::info() << "Idle: Average " << String::toAString(idleAveragePower * 1000.0, 1u) << " mW with " << String::toAString(idleStandardDeviation * 1000.0, 1u) << " SD, median " << String::toAString(idleMedian * 1000.0, 1u) << " mW, with " << idleNumberMeasurements << " measurements, valid poses: " << idleValidPoses << "/" << idleNumberFrames;

				double trackerAveragePower = 0.0;
				double trackerStandardDeviation = -1.0;
				size_t trackerNumberMeasurements;
				double trackerMedian = 0.0;

				if (trackerPowerMonitor.averagePower(trackerAveragePower, &trackerStandardDeviation, &trackerNumberMeasurements) && trackerPowerMonitor.medianPower(trackerMedian))
				{
					Log::info() << "With tracker: Average " << String::toAString(trackerAveragePower * 1000.0, 1u) << " mW with " << String::toAString(trackerStandardDeviation * 1000.0, 1u) << " SD, median " << String::toAString(trackerMedian * 1000.0, 1u) << " mW, with " << trackerNumberMeasurements << " measurements, valid poses: " << trackerValidPoses << "/" << trackerNumberFrames;

					Log::info() << "Tracker consumed approx. " << (trackerAveragePower - idleAveragePower) * 1000.0 << " mW (average), " << (trackerMedian - idleMedian) * 1000.0 << " mW (median)";
					Log::info() << "Tracker needed " << performance.averageMseconds() << " ms per frame";
				}
			}

			Log::info() << " ";
		}
	}
	else
	{
		Log::error() << "Failed to initialize the power monitor";

		allSucceeded = false;
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	Media::Android::unregisterAndroidLibrary();
#endif

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;

#else // OCEAN_PLATFORM_BUILD_ANDROID

	Log::info() << "Skipping test as it is only supported on Android platforms";
	return true;

#endif // OCEAN_PLATFORM_BUILD_ANDROID

}

}

}

}
