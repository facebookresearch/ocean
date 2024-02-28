// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_DEPTH_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_DEPTH_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/rendering/FrameTexture2D.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how use a depth tracker.
 * @ingroup xrplayground
 */
class DepthTrackerExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Definition of individual visualization modes.
		 */
		enum VisualizationMode : uint32_t
		{
			/// Using Jet color map.
			VM_JET = 0u,
			/// Using Turbo color map.
			VM_TURBO,
			/// Using Inferno color map.
			VM_INFERNO,
			/// Using Spiral color map.
			VM_SPIRAL,
			/// Using Jet color map on confidence map.
			VM_JET_CONFIDENCE,
			/// The last (exclusive) mode.
			VM_END
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~DepthTrackerExperience() override;

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
		 * Mouse press event function.
		 * @see Experience::onMousePress().
		 */
		void onMousePress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Creates a new DepthTrackerExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		DepthTrackerExperience() = default;

	protected:

		/// The 6-DOF scene tracker providing the depth information.
		Devices::SceneTracker6DOFRef sceneTracker6DOF_;

		/// The frame texture for the depth image.
		Rendering::FrameTexture2DRef renderingFrameTexture_;

		/// The transformatino holding the textured box.
		Rendering::TransformRef renderingBoxTransform_;

		/// The timestamp of the last depth image.
		Timestamp recentDepthImageTimestamp_;

		/// The visualization mode to be used.
		VisualizationMode visualizationMode_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_DEPTH_TRACKER_EXPERIENCE_H
