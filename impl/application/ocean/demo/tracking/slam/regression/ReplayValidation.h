/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_REGRESSION_REPLAY_VALIDATION_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_REGRESSION_REPLAY_VALIDATION_H

#include "application/ocean/demo/tracking/slam/ApplicationDemoTrackingSLAM.h"

#include "ocean/tracking/slam/TrackerMono.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestSLAM
{

/**
 * This class implements replay validation for the SLAM tracker.
 * It replays .osn files and compares the resulting FramesStatistics with previous baseline runs to ensure that no major regression has occurred in tracking performance.
 * @ingroup applicationdemotrackingslamregression
 */
class ReplayValidation
{
	public:

		/**
		 * Configuration class for replay validation parameters.
		 */
		class Configuration
		{
			public:

				/**
				 * Creates a new configuration with default values.
				 */
				inline Configuration();

				/**
				 * Returns whether this configuration is valid.
				 * @return True if all tolerance values are within valid range [0, 1]
				 */
				inline bool isValid() const;

			public:

				/// The allowed relative tolerance for valid pose count regression (e.g., 0.05 = 5%), with range [0, 1].
				double toleranceValidPoses_ = 0.05;

				/// The allowed relative tolerance for tracking correspondence count regression (e.g., 0.1 = 10%), with range [0, 1].
				double toleranceTrackingCorrespondences_ = 0.1;

				/// The allowed relative tolerance for pose correspondence count regression (e.g., 0.1 = 10%), with range [0, 1].
				double tolerancePoseCorrespondences_ = 0.1;

				/// The allowed relative tolerance for Bundle Adjustment iteration difference (e.g., 0.1 = 10%), with range [0, 1].
				double toleranceBundleAdjustmentIterations_ = 0.1;

				/// If true, writes new statistics to a "new" subfolder when a baseline already exists.
				bool writeNewToSubfolder_ = false;
		};

		/**
		 * Definition of comparison results for a single replay validation.
		 */
		class ComparisonResult
		{
			public:

				/**
				 * Creates a new comparison result with initial values.
				 */
				ComparisonResult() = default;

				/**
				 * Returns whether the comparison passed (no major regressions detected).
				 * @return True if the comparison passed
				 */
				bool passed() const;

			public:

				/// The name of the .osn file being validated.
				std::string osnFilename_;

				/// The number of frames in the baseline run.
				size_t baselineFrameCount_ = 0;

				/// The number of frames in the current run.
				size_t currentFrameCount_ = 0;

				/// The number of valid poses in the baseline run.
				size_t baselineValidPoseCount_ = 0;

				/// The number of valid poses in the current run.
				size_t currentValidPoseCount_ = 0;

				/// The total frame-to-frame tracking correspondences in the baseline run.
				size_t baselineTotalTrackingCorrespondences_ = 0;

				/// The total frame-to-frame tracking correspondences in the current run.
				size_t currentTotalTrackingCorrespondences_ = 0;

				/// The total pose estimation correspondences in the baseline run.
				size_t baselineTotalPoseCorrespondences_ = 0;

				/// The total pose estimation correspondences in the current run.
				size_t currentTotalPoseCorrespondences_ = 0;

				/// The relative difference in valid pose count (current vs baseline), negative means regression.
				double validPoseCountRelativeDiff_ = 0.0;

				/// The relative difference in tracking correspondences (current vs baseline), negative means regression.
				double trackingCorrespondencesRelativeDiff_ = 0.0;

				/// The relative difference in pose correspondences (current vs baseline), negative means regression.
				double poseCorrespondencesRelativeDiff_ = 0.0;

				/// The total Bundle Adjustment iterations (sum of mapVersion changes) in the baseline run.
				size_t baselineTotalBundleAdjustmentIterations_ = 0;

				/// The total Bundle Adjustment iterations (sum of mapVersion changes) in the current run.
				size_t currentTotalBundleAdjustmentIterations_ = 0;

				/// The relative difference in Bundle Adjustment iterations (current vs baseline), negative means regression.
				double bundleAdjustmentIterationsRelativeDiff_ = 0.0;

				/// True if a major regression was detected.
				bool hasMajorRegression_ = false;

				/// Description of any detected regression.
				std::string regressionDescription_;

				/// True if a major improvement was detected.
				bool hasMajorImprovement_ = false;

				/// Description of any detected improvement.
				std::string improvementDescription_;

				/// True if a minor issue (warning) was detected.
				bool hasMinorIssue_ = false;

				/// Description of any detected minor issue (warning).
				std::string minorIssueDescription_;
		};

		using ComparisonResults = std::vector<ComparisonResult>;

	public:

		/**
		 * Runs replay validation on all .osn files in a directory.
		 * For each .osn file, the function:
		 * 1. Replays the file through TrackerMono
		 * 2. Compares the resulting FramesStatistics with the corresponding baseline JSON file (if it exists)
		 * 3. Creates a baseline JSON file if one doesn't exist
		 * 4. Reports any major regressions
		 * @param replayDirectory The directory containing .osn files and their corresponding baseline JSON files
		 * @param configuration The validation configuration containing tolerance values and options
		 * @return True if all validations passed (no major regressions detected)
		 */
		static bool testReplayValidation(const std::string& replayDirectory, const Configuration& configuration = Configuration());

		/**
		 * Replays a single .osn file and returns the resulting FramesStatistics.
		 * @param osnFilePath The path to the .osn file
		 * @param framesStatistics The resulting frames statistics from the replay
		 * @return True if the replay succeeded
		 */
		static bool replayOSNFile(const std::string& osnFilePath, Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics);

		/**
		 * Compares current FramesStatistics with baseline FramesStatistics.
		 * @param current The current run's frames statistics
		 * @param baseline The baseline frames statistics
		 * @param configuration The validation configuration containing tolerance values
		 * @param comparisonResult The resulting comparison result
		 * @return True if the comparison indicates no major regression
		 */
		static bool compareFramesStatistics(const Tracking::SLAM::TrackerMono::FramesStatistics& current, const Tracking::SLAM::TrackerMono::FramesStatistics& baseline, const Configuration& configuration, ComparisonResult& comparisonResult);

		/**
		 * Logs a summary of the comparison results.
		 * @param results The comparison results to summarize
		 */
		static void logComparisonSummary(const ComparisonResults& results);

	protected:

		/**
		 * Counts the number of valid camera poses in the frames statistics.
		 * @param framesStatistics The frames statistics to analyze
		 * @return The number of frames with valid camera poses
		 */
		static size_t countValidPoses(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics);

		/**
		 * Calculates the total frame-to-frame tracking correspondences across all frames.
		 * @param framesStatistics The frames statistics to analyze
		 * @return The total number of frame-to-frame tracking correspondences
		 */
		static size_t totalTrackingCorrespondences(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics);

		/**
		 * Calculates the total pose estimation correspondences across all frames.
		 * @param framesStatistics The frames statistics to analyze
		 * @return The total number of pose estimation correspondences
		 */
		static size_t totalPoseCorrespondences(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics);

		/**
		 * Calculates the total number of Bundle Adjustment iterations across all frames.
		 * This is computed as the maximum mapVersion value across all frames.
		 * @param framesStatistics The frames statistics to analyze
		 * @return The total number of Bundle Adjustment iterations
		 */
		static size_t totalBundleAdjustmentIterations(const Tracking::SLAM::TrackerMono::FramesStatistics& framesStatistics);
};

inline ReplayValidation::Configuration::Configuration()
{
	// nothing to do here
}

inline bool ReplayValidation::Configuration::isValid() const
{
	return toleranceValidPoses_ >= 0.0 && toleranceValidPoses_ <= 1.0
		&& toleranceTrackingCorrespondences_ >= 0.0 && toleranceTrackingCorrespondences_ <= 1.0
		&& tolerancePoseCorrespondences_ >= 0.0 && tolerancePoseCorrespondences_ <= 1.0
		&& toleranceBundleAdjustmentIterations_ >= 0.0 && toleranceBundleAdjustmentIterations_ <= 1.0;
}

}

}

}

}

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_REGRESSION_REPLAY_VALIDATION_H
