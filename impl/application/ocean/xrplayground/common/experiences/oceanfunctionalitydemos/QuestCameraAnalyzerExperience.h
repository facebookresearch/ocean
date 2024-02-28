// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QUEST_CAMERA_ANALYZER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QUEST_CAMERA_ANALYZER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/math/Variance.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Text.h"

#include <queue>

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to access the cameras on Quest platforms and how to apply some image analyzis.
 * @ingroup xrplayground
 */
class QuestCameraAnalyzerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * This clas holds statistics for one frame.
		 */
		class FrameStatistic
		{
			protected:

				/**
				 * Definition of a queue holding double values.
				 */
				using Queue = std::queue<double>;

			public:

				/**
				 * Default constructor creating an object with default data.
				 */
				FrameStatistic() = default;

				/**
				 * Move constructor.
				 * @param frameStatistic The object to be moved
				 */
				FrameStatistic(FrameStatistic&& frameStatistic);

				/**
				 * Adds a new frame to be analyzed.
				 * @param cameraName The name of the cameraName providing the frame
				 * @param yFrame The frame to be added, must have pixel format FORMAT_Y8
				 * @return True, if succeeded
				 */
				bool addFrame(const std::string& cameraName, const Frame& yFrame);

				/**
				 * Returns the most recent statistic results.
				 * @param cameraName The resulting name of the camera providing the frame
				 * @param meanIntensity The resulting mean pixel intensity
				 * @param saturatedPixels The resulting amount of saturated pixels, in percent, with range [0, 1]
				 * @param averageMeanIntensity The resulting average mean intensity for the last 60 frames
				 * @param deviationMeanIntensity The resulting deviation of the mean intensity for the last 60 frames
				 * @return True, if succeeded; False, if no updated information was available
				 */
				bool recentResults(std::string& cameraName, double& meanIntensity, double& saturatedPixels, double& averageMeanIntensity, double& deviationMeanIntensity) const;

				/**
				 * Resets the statistics.
				 */
				void reset();

				/**
				 * Move operator.
				 * @param frameStatistic The object to be moved
				 * @return Reference to this object
				 */
				FrameStatistic& operator=(FrameStatistic&& frameStatistic);

			protected:

				/// True, if this object holds updated information.
				mutable bool updated_ = false;

				/// The name of the camera providing the data.
				std::string cameraName_;

				/// The recent mean intensity.
				double meanIntensity_ = -1.0;

				/// The recent amount of saturated pixels, in percent, with range [0, 1].
				double saturatedPixels_ = -1.0;

				/// The variance object for the mean intensity.
				VarianceD meanIntensityVariance_;

				/// The queue holding previous mean intensities.
				Queue meanIntensityQueue_;

				/// The object's lock.
				mutable Lock lock_;
		};

		/**
		 * Definition of a vector holding per-frame statistics.
		 */
		using FrameStatistics = std::vector<FrameStatistic>;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~QuestCameraAnalyzerExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Thread run function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Analyzers a camera frame.
		 * @param frame The camera frame to analyzer, must be valid
		 * @return True, if succeeded
		 */
		static bool analyzeFrame(const Frame& frame);

	protected:

		/// True, if the visualization is shown in world; False, if the visualization is shown in view.
		bool showInWorld_ = true;

		/// The rendering Transform objects holding the individual cameras.
		Rendering::TransformRefs renderingTransformCameras_;

		/// The rendering Transform objects holding the individual texts.
		Rendering::TransformRefs renderingTransformTexts_;

		/// The rendering Text objects holding the text for the individual statistics.
		std::vector<Rendering::TextRef> renderingTexts_;

		/// The FrameMedium objects of all cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;

		/// The per-frame statistics.
		FrameStatistics frameStatistics_;

		/// True, in case the user wants to toggle to the next cameras.
		std::atomic_bool toggleCameras_ = false;

		/// True, if new cameras are available.
		bool newCamerasAvailable_ = false;

		/// The lock for the frame mediums.
		Lock frameMediumsLock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QUEST_CAMERA_ANALYZER_EXPERIENCE_H
