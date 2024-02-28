// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRSTEREOCAMERA_MR_STEREO_CAMERA_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRSTEREOCAMERA_MR_STEREO_CAMERA_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to render both lower stereo cameras of a heaset with correct camera distortion to create a stereo effect.
 * @ingroup xrplayground
 */
class MRStereoCameraExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Definition of individual visualiation modes.
		 */
		enum VisualizationMode : uint32_t
		{
			/// Both cameras are shown on the respective eye.
			VM_BOTH_CAMERAS_INDIVIDUAL_EYES,
			/// Only the left camera is shown on both eyes.
			VM_LEFT_CAMERA_BOTH_EYES,
			/// Only the right camera is shown on both eyes.
			VM_RIGHT_CAMERA_BOTH_EYES,
			/// End value.
			VM_END
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MRStereoCameraExperience() override;

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

		/// The current visualization mode.
		VisualizationMode visualizationMode_ = VM_BOTH_CAMERAS_INDIVIDUAL_EYES;

		/// The rendering Transform objects holding the individual cameras.
		Rendering::TransformRefs renderingTransforms_;

		/// The FrameMedium objects of the cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRSTEREOCAMERA_MR_STEREO_CAMERA_EXPERIENCE_H
