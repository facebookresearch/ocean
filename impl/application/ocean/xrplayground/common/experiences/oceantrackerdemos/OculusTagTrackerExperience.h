// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OCULUS_TAG_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OCULUS_TAG_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Text.h"

#include "ocean/tracking/oculustags/OculusTagTracker.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience demonstrates the Oculus Tag tracker.
 * @ingroup xrplayground
 */
class OculusTagTrackerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * Definition of a helper class to visualize Oculus Tags in VR.
		 */
		class TagVisualization
		{
			public:

				/**
				 * Visualizes a Oculus tag in the world coordinate frame
				 * @param world_T_code The 6DOF pose of the code mapping from object space to world space, must be valid.
				 * @param tagSize The size of the tag in the physical world, in meters, range: (0, infinity)
				 * @param tagId The ID of the tag, range: [0, infinity)
				 */
				void visualizeInWorld(const HomogenousMatrix4& world_T_code, const Scalar tagSize, const unsigned int tagId);

				/**
				 * Hides this visualization.
				 */
				void hide();

				/**
				 * Creates a visualization instance.
				 * @param engine The rendering engine that should be used to create this new instance, must be valid.
				 * @param parentScene The scene in which the visualization instance should be registered, must be valid.
				 * @return The instance that has been created.
				 */
				static TagVisualization create(const Rendering::EngineRef& engine, const Rendering::SceneRef& parentScene);

			protected:

				/**
				 * Constructs an empty instance.
				 */
				TagVisualization() = default;

			protected:

				/// The box transformation that will be used as an overlay for the Oculus Tags displayed in Passthrough.
				Rendering::TransformRef boxTransform_;

				/// The box that will be used as an overlay for the Oculus Tags displayed in Passthrough and of which the sizes need to be adjusted to sizes of the Oculus Tags they are representing.
				Rendering::BoxRef box_;

				/// The material configuration of the box.
				Rendering::MaterialRef boxMaterial_;

				/// The location there the tag ID should be visualized.
				Rendering::TransformRef textTagIdTransform_;

				/// The text element that visualizes the tag ID.
				Rendering::TextRef textTagId_;

				/// A coordinate system that will be displayed in the center of the tag.
				Rendering::TransformRef coordinateSystem_;
		};

		/// Definition of a vector of tag visualizations.
		typedef std::vector<TagVisualization> TagVisualizations;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~OculusTagTrackerExperience() override;

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
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/// The FrameMedium objects of all cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;

		/// The helpers for the visualizations of Oculus Tags.
		TagVisualizations tagVisualizations_;

		/// The box transformations that will be used as an overlay for the Oculus Tags displayed in Passthrough.
		Rendering::TransformRefs codeBoxTransforms_;

		/// The boxes that will be used as an overlay for the Oculus Tags displayed in Passthrough and of which the sizes need to be adjusted to sizes of the Oculus Tags they are representing.
		std::vector<Rendering::BoxRef> codeBoxes_;

		/// The lock for the detection results.
		Lock resultLock_;

		/// Indicates if new results are available for displaying.
		bool haveResults_ = false;

		/// The currently tracked Oculus tags.
		Tracking::OculusTags::OculusTags oculusTags_;

		/// The timestamp from when the code were detected
		Timestamp detectionTimestamp_;

		/// Average detection time (in milliseconds) for all processed frames together.
		double averageDetectionTimeMs_ = -1.0;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OCULUS_TAG_TRACKER_EXPERIENCE_H
