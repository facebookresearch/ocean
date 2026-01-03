/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/regression/ReplayValidation.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/Manager.h"
#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/devices/serialization/SerializerDevicePlayer.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/tracking/slam/TrackerMono.h"
#include "ocean/tracking/slam/Utilities.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestSLAM
{

/**
 * Helper class providing access to gravity and orientation sensor data.
 * This class manages GravityTracker3DOF and OrientationTracker3DOF devices and provides convenience functions to retrieve sensor measurements transformed into the camera coordinate system.
 */
class SensorAccessor
{
	public:

		/**
		 * Creates a new sensor accessor object.
		 */
		SensorAccessor() = default;

		/**
		 * Returns the gravity vector in the camera coordinate system.
		 * On first call, this function will initialize and start the gravity tracker device.
		 * @param device_Q_camera The rotation transforming points in camera coordinates to device coordinates
		 * @param timestamp The timestamp for which the gravity measurement is requested
		 * @return The gravity vector in camera coordinates, a null vector if unavailable
		 */
		Vector3 cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp);

		/**
		 * Returns the camera's orientation in world coordinates.
		 * On first call, this function will initialize and start the orientation tracker device.
		 * @param device_Q_camera The rotation transforming points in camera coordinates to device coordinates
		 * @param timestamp The timestamp for which the orientation measurement is requested
		 * @return The rotation transforming points in camera coordinates to world coordinates, invalid if unavailable
		 */
		Quaternion anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp);

		/**
		 * Releases all device references held by this object.
		 */
		void release();

	protected:

		/// The gravity tracker device providing gravity measurements.
		Devices::GravityTracker3DOFRef gravityTracker_;

		/// The orientation tracker device providing orientation measurements.
		Devices::OrientationTracker3DOFRef orientationTracker_;
};

Vector3 SensorAccessor::cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp)
{
	if (!device_Q_camera.isValid())
	{
		return Vector3(0, 0, 0);
	}

	if (gravityTracker_.isNull())
	{
		gravityTracker_ = Devices::Manager::get().device(Devices::GravityTracker3DOF::deviceTypeGravityTracker3DOF());

		if (gravityTracker_)
		{
			gravityTracker_->start();
		}
	}

	if (gravityTracker_)
	{
		const Devices::GravityTracker3DOF::GravityTracker3DOFSampleRef sample = gravityTracker_->sample(timestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample && sample->gravities().size() >= 1)
		{
			ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);

			const Vector3& deviceGravity = sample->gravities().front();
			const Quaternion camera_Q_device = device_Q_camera.inverted();

			return camera_Q_device * deviceGravity;
		}
	}

	return Vector3(0, 0, 0);
}

Quaternion SensorAccessor::anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp)
{
	if (!device_Q_camera.isValid())
	{
		return Quaternion(false);
	}

	if (orientationTracker_.isNull())
	{
		orientationTracker_ = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

		if (orientationTracker_)
		{
			orientationTracker_->start();
		}
	}

	if (orientationTracker_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker_->sample(timestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample && sample->orientations().size() >= 1)
		{
			ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT);

			const Quaternion anyWorld_Q_device = sample->orientations().front();

			return anyWorld_Q_device * device_Q_camera;
		}
	}

	return Quaternion(false);
}

void SensorAccessor::release()
{
	gravityTracker_.release();
	orientationTracker_.release();
}

bool ReplayValidation::ComparisonResult::passed() const
{
	return !hasMajorRegression_;
}

bool ReplayValidation::testReplayValidation(const std::string& replayDirectory, const Configuration& configuration)
{
	ocean_assert(configuration.isValid());

	if (replayDirectory.empty())
	{
		Log::error() << "Replay validation requires a directory path";
		return false;
	}

	const IO::Directory directory(replayDirectory);

	if (!directory.exists())
	{
		Log::error() << "Replay directory does not exist: " << replayDirectory;
		return false;
	}

	Log::info() << "Running replay validation tests in: " << replayDirectory;
	Log::info() << "Tolerance for valid poses: " << String::toAString(configuration.toleranceValidPoses_ * 100.0, 1u) << "%";
	Log::info() << "Tolerance for tracking correspondences: " << String::toAString(configuration.toleranceTrackingCorrespondences_ * 100.0, 1u) << "%";
	Log::info() << "Tolerance for pose correspondences: " << String::toAString(configuration.tolerancePoseCorrespondences_ * 100.0, 1u) << "%";
	Log::info() << "Tolerance for Bundle Adjustment iterations: " << String::toAString(configuration.toleranceBundleAdjustmentIterations_ * 100.0, 1u) << "%";
	Log::info() << " ";

	const IO::Files osnFiles = directory.findFiles("osn", false);

	if (osnFiles.empty())
	{
		Log::warning() << "No .osn files found in directory";
		return true;
	}

	Log::info() << "Found " << osnFiles.size() << " .osn file(s) to validate";
	Log::info() << " ";

	ComparisonResults comparisonResults;
	comparisonResults.reserve(osnFiles.size());

	IO::Directory newDirectory;

	if (configuration.writeNewToSubfolder_)
	{
		newDirectory = directory + IO::Directory("new_" + DateTime::localStringForFile());
	}

	bool allPassed = true;

	for (const IO::File& osnFile : osnFiles)
	{
		Log::info() << "Processing: " << osnFile.name();

		ComparisonResult comparisonResult;
		comparisonResult.osnFilename_ = osnFile.name();

		Tracking::SLAM::TrackerMono::FramesStatistics currentStatistics;

		if (!replayOSNFile(osnFile(), currentStatistics))
		{
			Log::error() << "  Failed to replay file";

			comparisonResult.hasMajorRegression_ = true;
			comparisonResult.regressionDescription_ = "Failed to replay OSN file";
			comparisonResults.push_back(std::move(comparisonResult));
			allPassed = false;

			continue;
		}

		Log::info() << "  Replayed " << currentStatistics.size() << " frames";

		comparisonResult.currentFrameCount_ = currentStatistics.size();
		comparisonResult.currentValidPoseCount_ = countValidPoses(currentStatistics);
		comparisonResult.currentTotalTrackingCorrespondences_ = totalTrackingCorrespondences(currentStatistics);
		comparisonResult.currentTotalPoseCorrespondences_ = totalPoseCorrespondences(currentStatistics);
		comparisonResult.currentTotalBundleAdjustmentIterations_ = totalBundleAdjustmentIterations(currentStatistics);

		const IO::File baselineFile(osnFile.base() + "_baseline.json");

		if (baselineFile.exists())
		{
			Log::info() << "  Loading baseline: " << baselineFile.name();

			Tracking::SLAM::TrackerMono::FramesStatistics baselineStatistics;

			if (!Tracking::SLAM::Utilities::deserializeFramesStatisticsFromJSON(baselineFile(), baselineStatistics))
			{
				Log::error() << "  Failed to load baseline file";

				comparisonResult.hasMajorRegression_ = true;
				comparisonResult.regressionDescription_ = "Failed to load baseline JSON file";
				comparisonResults.push_back(std::move(comparisonResult));
				allPassed = false;

				continue;
			}

			if (!compareFramesStatistics(currentStatistics, baselineStatistics, configuration, comparisonResult))
			{
				allPassed = false;
			}

			Log::info() << "  Baseline frames: " << comparisonResult.baselineFrameCount_;
			Log::info() << "  Valid poses - baseline: " << comparisonResult.baselineValidPoseCount_ << ", current: " << comparisonResult.currentValidPoseCount_ << " (" << String::toAString(comparisonResult.validPoseCountRelativeDiff_ * 100.0, 1u) << "%)";
			Log::info() << "  Tracking correspondences - baseline: " << comparisonResult.baselineTotalTrackingCorrespondences_ << ", current: " << comparisonResult.currentTotalTrackingCorrespondences_ << " (" << String::toAString(comparisonResult.trackingCorrespondencesRelativeDiff_ * 100.0, 1u) << "%)";
			Log::info() << "  Pose correspondences - baseline: " << comparisonResult.baselineTotalPoseCorrespondences_ << ", current: " << comparisonResult.currentTotalPoseCorrespondences_ << " (" << String::toAString(comparisonResult.poseCorrespondencesRelativeDiff_ * 100.0, 1u) << "%)";

			if (comparisonResult.baselineTotalBundleAdjustmentIterations_ > 0)
			{
				Log::info() << "  Bundle Adjustment iterations - baseline: " << comparisonResult.baselineTotalBundleAdjustmentIterations_ << ", current: " << comparisonResult.currentTotalBundleAdjustmentIterations_ << " (" << String::toAString(comparisonResult.bundleAdjustmentIterationsRelativeDiff_ * 100.0, 1u) << "%)";
			}

			if (comparisonResult.hasMajorRegression_)
			{
				Log::error() << "  REGRESSION: " << comparisonResult.regressionDescription_;
			}
			else if (comparisonResult.hasMajorImprovement_)
			{
				if (comparisonResult.hasMinorIssue_)
				{
					Log::info() << "  PASSED (IMPROVED: " << comparisonResult.improvementDescription_ << ")";
					Log::warning() << "  WARNING: " << comparisonResult.minorIssueDescription_;
				}
				else
				{
					Log::info() << "  PASSED (IMPROVED: " << comparisonResult.improvementDescription_ << ")";
				}
			}
			else if (comparisonResult.hasMinorIssue_)
			{
				Log::info() << "  PASSED";
				Log::warning() << "  WARNING: " << comparisonResult.minorIssueDescription_;
			}
			else
			{
				Log::info() << "  PASSED";
			}

			if (newDirectory.isValid())
			{
				if (!newDirectory.exists())
				{
					newDirectory.create();
				}

				const IO::File newBaselineFile(newDirectory() + osnFile.name().substr(0, osnFile.name().length() - 4) + "_baseline.json");

				if (Tracking::SLAM::Utilities::serializeFramesStatisticsToJSON(currentStatistics, newBaselineFile()))
				{
					Log::info() << "  New statistics written to: " << newBaselineFile();
				}
				else
				{
					Log::error() << "  Failed to write new statistics to subfolder";
				}
			}
		}
		else
		{
			Log::info() << "  No baseline file found, creating: " << baselineFile.name();

			if (!Tracking::SLAM::Utilities::serializeFramesStatisticsToJSON(currentStatistics, baselineFile()))
			{
				Log::error() << "  Failed to create baseline file";
			}
			else
			{
				Log::info() << "  Baseline file created successfully";
				Log::info() << "  Valid poses: " << comparisonResult.currentValidPoseCount_;
				Log::info() << "  Tracking correspondences: " << comparisonResult.currentTotalTrackingCorrespondences_;
				Log::info() << "  Pose correspondences: " << comparisonResult.currentTotalPoseCorrespondences_;
				Log::info() << "  Bundle Adjustment iterations: " << comparisonResult.currentTotalBundleAdjustmentIterations_;
			}
		}

		comparisonResults.push_back(std::move(comparisonResult));

		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << "--- Summary ---";

	logComparisonSummary(comparisonResults);

	return allPassed;
}

bool ReplayValidation::replayOSNFile(const std::string& osnFilePath, Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics)
{
	const IO::File inputFile(osnFilePath);

	if (!inputFile.exists() || inputFile.extension() != "osn")
	{
		Log::error() << "Invalid OSN file: " << osnFilePath;
		return false;
	}

	Devices::Serialization::SerializerDevicePlayer devicePlayer;

	if (!devicePlayer.initialize(inputFile()))
	{
		Log::error() << "Failed to initialize device player for: " << osnFilePath;
		return false;
	}

	if (devicePlayer.frameMediums().empty())
	{
		Log::error() << "Device player has no frame mediums: " << osnFilePath;
		return false;
	}

	if (!devicePlayer.start(0.0f))
	{
		Log::error() << "Failed to start device player";
		return false;
	}

	constexpr double stopMotionInterval = 1.0 / 30.0;
	constexpr double stopMotionTolerance = 0.005; // 5ms

	devicePlayer.setStopMotionTolerance(IO::Serialization::DataTimestamp(stopMotionTolerance));

	Media::FrameMediumRef frameMedium = devicePlayer.frameMediums().front();

	Tracking::SLAM::TrackerMono trackerMono;
	Frame yFrame;

	Timestamp frameTimestamp(false);
	Timestamp iterationTimestamp(false);

	SensorAccessor sensorAccessor;

	while (devicePlayer.isPlaying())
	{
		frameTimestamp = devicePlayer.playNextFrame();

		if (frameTimestamp.isInvalid())
		{
			// we have reached the end of the replay
			break;
		}

		SharedAnyCamera camera;
		FrameRef frame = frameMedium->frame(frameTimestamp, &camera);

		if (!frame || !camera)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		iterationTimestamp.toNow();

		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::formatGrayscalePixelFormat(frame->pixelFormat()), FrameType::ORIGIN_UPPER_LEFT, yFrame))
		{
			Log::error() << "Failed to convert frame to grayscale";
			return false;
		}

		const Quaternion device_Q_camera(frameMedium->device_T_camera().rotation());

		const Vector3 cameraGravity = sensorAccessor.cameraGravity(device_Q_camera, frameTimestamp);
		const Quaternion anyWorld_Q_camera = sensorAccessor.anyWorld_Q_camera(device_Q_camera, frameTimestamp);

		HomogenousMatrix4 world_T_camera(false);
		trackerMono.handleFrame(*camera, std::move(yFrame), world_T_camera, cameraGravity, anyWorld_Q_camera, nullptr);

		ocean_assert(stopMotionInterval > 0.0);
		const double waitTime = stopMotionInterval - double(Timestamp(true) - iterationTimestamp);

		if (waitTime > 0.0)
		{
			Thread::sleep((unsigned int)(double(1000) * waitTime + 0.5));
		}
	}

	framesStatistics = trackerMono.framesStatistics();

	sensorAccessor.release();
	frameMedium.release();
	devicePlayer.release();

	return true;
}

bool ReplayValidation::compareFramesStatistics(const Tracking::SLAM::TrackerMono::FramesStatistics& current, const Tracking::SLAM::TrackerMono::FramesStatistics& baseline, const Configuration& configuration, ComparisonResult& comparisonResult)
{
	comparisonResult.baselineFrameCount_ = baseline.size();
	comparisonResult.currentFrameCount_ = current.size();

	comparisonResult.baselineValidPoseCount_ = countValidPoses(baseline);
	comparisonResult.currentValidPoseCount_ = countValidPoses(current);

	comparisonResult.baselineTotalTrackingCorrespondences_ = totalTrackingCorrespondences(baseline);
	comparisonResult.currentTotalTrackingCorrespondences_ = totalTrackingCorrespondences(current);

	comparisonResult.baselineTotalPoseCorrespondences_ = totalPoseCorrespondences(baseline);
	comparisonResult.currentTotalPoseCorrespondences_ = totalPoseCorrespondences(current);

	comparisonResult.baselineTotalBundleAdjustmentIterations_ = totalBundleAdjustmentIterations(baseline);
	comparisonResult.currentTotalBundleAdjustmentIterations_ = totalBundleAdjustmentIterations(current);

	// Calculate relative differences
	// A negative value indicates regression (current is worse than baseline)

	if (comparisonResult.baselineValidPoseCount_ > 0)
	{
		comparisonResult.validPoseCountRelativeDiff_ = (double(comparisonResult.currentValidPoseCount_) - double(comparisonResult.baselineValidPoseCount_)) / double(comparisonResult.baselineValidPoseCount_);
	}

	if (comparisonResult.baselineTotalTrackingCorrespondences_ > 0)
	{
		comparisonResult.trackingCorrespondencesRelativeDiff_ = (double(comparisonResult.currentTotalTrackingCorrespondences_) - double(comparisonResult.baselineTotalTrackingCorrespondences_)) / double(comparisonResult.baselineTotalTrackingCorrespondences_);
	}

	if (comparisonResult.baselineTotalPoseCorrespondences_ > 0)
	{
		comparisonResult.poseCorrespondencesRelativeDiff_ = (double(comparisonResult.currentTotalPoseCorrespondences_) - double(comparisonResult.baselineTotalPoseCorrespondences_)) / double(comparisonResult.baselineTotalPoseCorrespondences_);
	}

	if (comparisonResult.baselineTotalBundleAdjustmentIterations_ > 0)
	{
		comparisonResult.bundleAdjustmentIterationsRelativeDiff_ = (double(comparisonResult.currentTotalBundleAdjustmentIterations_) - double(comparisonResult.baselineTotalBundleAdjustmentIterations_)) / double(comparisonResult.baselineTotalBundleAdjustmentIterations_);
	}

	comparisonResult.hasMajorRegression_ = false;
	comparisonResult.regressionDescription_.clear();

	if (comparisonResult.currentFrameCount_ != comparisonResult.baselineFrameCount_)
	{
		comparisonResult.hasMajorRegression_ = true;
		comparisonResult.regressionDescription_ = "Frame count mismatch: baseline=" + String::toAString(comparisonResult.baselineFrameCount_) + ", current=" + String::toAString(comparisonResult.currentFrameCount_);
		return false;
	}

	if (comparisonResult.validPoseCountRelativeDiff_ < -configuration.toleranceValidPoses_)
	{
		comparisonResult.hasMajorRegression_ = true;
		comparisonResult.regressionDescription_ = "Valid pose count regression: " + String::toAString(comparisonResult.baselineValidPoseCount_) + " -> " + String::toAString(comparisonResult.currentValidPoseCount_) + " (" + String::toAString(comparisonResult.validPoseCountRelativeDiff_ * 100.0, 1u) + "%, tolerance: " + String::toAString(-configuration.toleranceValidPoses_ * 100.0, 1u) + "%)";
		return false;
	}

	if (comparisonResult.trackingCorrespondencesRelativeDiff_ < -configuration.toleranceTrackingCorrespondences_)
	{
		comparisonResult.hasMajorRegression_ = true;
		comparisonResult.regressionDescription_ = "Tracking correspondences regression: " + String::toAString(comparisonResult.baselineTotalTrackingCorrespondences_) + " -> " + String::toAString(comparisonResult.currentTotalTrackingCorrespondences_) + " (" + String::toAString(comparisonResult.trackingCorrespondencesRelativeDiff_ * 100.0, 1u) + "%, tolerance: " + String::toAString(-configuration.toleranceTrackingCorrespondences_ * 100.0, 1u) + "%)";
		return false;
	}

	if (comparisonResult.poseCorrespondencesRelativeDiff_ < -configuration.tolerancePoseCorrespondences_)
	{
		comparisonResult.hasMajorRegression_ = true;
		comparisonResult.regressionDescription_ = "Pose correspondences regression: " + String::toAString(comparisonResult.baselineTotalPoseCorrespondences_) + " -> " + String::toAString(comparisonResult.currentTotalPoseCorrespondences_) + " (" + String::toAString(comparisonResult.poseCorrespondencesRelativeDiff_ * 100.0, 1u) + "%, tolerance: " + String::toAString(-configuration.tolerancePoseCorrespondences_ * 100.0, 1u) + "%)";
		return false;
	}

	{
		const double absDiff = comparisonResult.bundleAdjustmentIterationsRelativeDiff_ >= 0.0 ? comparisonResult.bundleAdjustmentIterationsRelativeDiff_ : -comparisonResult.bundleAdjustmentIterationsRelativeDiff_;

		if (absDiff > configuration.toleranceBundleAdjustmentIterations_)
		{
			// Bundle Adjustment iteration changes are considered a minor issue (warning) rather than a major regression
			comparisonResult.hasMinorIssue_ = true;
			comparisonResult.minorIssueDescription_ = "Bundle Adjustment iterations difference: " + String::toAString(comparisonResult.baselineTotalBundleAdjustmentIterations_) + " -> " + String::toAString(comparisonResult.currentTotalBundleAdjustmentIterations_) + " (" + String::toAString(comparisonResult.bundleAdjustmentIterationsRelativeDiff_ * 100.0, 1u) + "%, tolerance: +/-" + String::toAString(configuration.toleranceBundleAdjustmentIterations_ * 100.0, 1u) + "%)";
		}
	}

	comparisonResult.hasMajorImprovement_ = false;
	comparisonResult.improvementDescription_.clear();

	std::vector<std::string> improvements;

	if (comparisonResult.validPoseCountRelativeDiff_ > configuration.toleranceValidPoses_)
	{
		improvements.push_back("Valid poses: +" + String::toAString(comparisonResult.validPoseCountRelativeDiff_ * 100.0, 1u) + "%");
	}

	if (comparisonResult.trackingCorrespondencesRelativeDiff_ > configuration.toleranceTrackingCorrespondences_)
	{
		improvements.push_back("Tracking correspondences: +" + String::toAString(comparisonResult.trackingCorrespondencesRelativeDiff_ * 100.0, 1u) + "%");
	}

	if (comparisonResult.poseCorrespondencesRelativeDiff_ > configuration.tolerancePoseCorrespondences_)
	{
		improvements.push_back("Pose correspondences: +" + String::toAString(comparisonResult.poseCorrespondencesRelativeDiff_ * 100.0, 1u) + "%");
	}

	if (!improvements.empty())
	{
		comparisonResult.hasMajorImprovement_ = true;

		for (size_t i = 0; i < improvements.size(); ++i)
		{
			if (i > 0)
			{
				comparisonResult.improvementDescription_ += ", ";
			}

			comparisonResult.improvementDescription_ += improvements[i];
		}
	}

	return true;
}

void ReplayValidation::logComparisonSummary(const ComparisonResults& results)
{
	size_t passedCount = 0;
	size_t failedCount = 0;
	size_t skippedCount = 0;
	size_t improvedCount = 0;
	size_t warningCount = 0;

	for (const ComparisonResult& result : results)
	{
		if (result.baselineFrameCount_ == 0 && !result.hasMajorRegression_)
		{
			++skippedCount; // No baseline available
		}
		else if (result.passed())
		{
			++passedCount;

			if (result.hasMajorImprovement_)
			{
				++improvedCount;
			}

			if (result.hasMinorIssue_)
			{
				++warningCount;
			}
		}
		else
		{
			++failedCount;
		}
	}

	Log::info() << "Total files: " << results.size();
	Log::info() << "  Passed: " << passedCount << (improvedCount > 0 ? " (" + String::toAString(improvedCount) + " improved)" : "") << (warningCount > 0 ? " (" + String::toAString(warningCount) + " with warnings)" : "");
	Log::info() << "  Failed: " << failedCount;
	Log::info() << "  Skipped (no baseline): " << skippedCount;
	Log::info() << " ";

	if (improvedCount > 0)
	{
		Log::info() << "Improved tests:";

		for (const ComparisonResult& result : results)
		{
			if (result.passed() && result.hasMajorImprovement_)
			{
				Log::info() << "  " << result.osnFilename_ << ": " << result.improvementDescription_;
			}
		}

		Log::info() << " ";
	}

	if (warningCount > 0)
	{
		Log::info() << "Tests with warnings:";

		for (const ComparisonResult& result : results)
		{
			if (result.passed() && result.hasMinorIssue_)
			{
				Log::warning() << "  " << result.osnFilename_ << ": " << result.minorIssueDescription_;
			}
		}

		Log::info() << " ";
	}

	if (failedCount > 0)
	{
		Log::info() << "Failed tests:";

		for (const ComparisonResult& result : results)
		{
			if (!result.passed() && result.baselineFrameCount_ > 0)
			{
				Log::error() << "  " << result.osnFilename_ << ": " << result.regressionDescription_;
			}
		}
	}
}

size_t ReplayValidation::countValidPoses(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics)
{
	size_t count = 0;

	for (const Tracking::SLAM::TrackerMono::FrameStatistics& frameStats : framesStatistics)
	{
		if (frameStats.world_T_camera_.isValid())
		{
			++count;
		}
	}

	return count;
}

size_t ReplayValidation::totalTrackingCorrespondences(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics)
{
	size_t total = 0;

	for (const Tracking::SLAM::TrackerMono::FrameStatistics& frameStats : framesStatistics)
	{
		total += frameStats.frameToFrameTrackingActual_;
	}

	return total;
}

size_t ReplayValidation::totalPoseCorrespondences(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics)
{
	size_t total = 0;

	for (const Tracking::SLAM::TrackerMono::FrameStatistics& frameStats : framesStatistics)
	{
		total += frameStats.poseEstimationCorrespondences_;
	}

	return total;
}

size_t ReplayValidation::totalBundleAdjustmentIterations(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics)
{
	size_t maxVersion = 0;

	for (const Tracking::SLAM::TrackerMono::FrameStatistics& frameStats : framesStatistics)
	{
		if (frameStats.mapVersion_ > maxVersion)
		{
			maxVersion = frameStats.mapVersion_;
		}
	}

	return maxVersion;
}

}

}

}

}
