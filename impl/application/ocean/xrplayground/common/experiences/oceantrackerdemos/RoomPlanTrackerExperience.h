// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ROOM_PLAN_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ROOM_PLAN_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to use Apple's Room Plan in an experience.
 * @ingroup xrplayground
 */
class RoomPlanTrackerExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~RoomPlanTrackerExperience() override;

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
		 * @see Library::onMousePress().
		 */
		void onMousePress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Mouse move event function.
		 * @see Library::onMouseMove().
		 */
		void onMouseMove(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Mouse release event function.
		 * @see Library::onMouseRelease().
		 */
		void onMouseRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Creates a new UserProfileExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		RoomPlanTrackerExperience() = default;

		/**
		 * Event function for new tracking samples from the scene tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from scene tracker
		 */
		void onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * Returns the color for a given planar room object.
		 * @param planarType The type of the planar room object
		 * @param alpha The alpha value to be used, with range [0, 1]
		 * @return The resulting color
		 */
		static RGBAColor planarColor(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType, const float alpha = 1.0f);

		/**
		 * Returns the thickness for a given planar room object.
		 * @param planarType The type of the planar room object
		 * @return The resulting thickness
		 */
		static Scalar planarThickness(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType);

		/**
		 * Returns the color for a given volumetric room object.
		 * @param volumetricType The type of the volumetric room object
		 * @param alpha The alpha value to be used, with range [0, 1]
		 * @return The resulting color
		 */
		static RGBAColor volumetricColor(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const float alpha = 0.85f);

		/**
		 * Returns an adjusted dimension for volumetric objects.
		 * Adjustments will be minor to improve visualization.
		 * @param volumetricType The type of the volumetric room object
		 * @param dimension The dimension of the object to adjust
		 * @return The adjusted volumetric object
		 */
		static Vector3 adjustedVolumetricDimension(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const Vector3& dimension);

	protected:

		/// The rendering Transform node holding the room objects in world.
		Rendering::TransformRef renderingWorldTransform_;

		/// The rendering Transform node holding the room objects in view.
		Rendering::TransformRef renderingViewTransform_;

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The 6-DOF Scene tracker.
		Devices::SceneTracker6DOFRef sceneTracker6DOF_;

		/// The subscription object for sample events from the scene tracker.
		Devices::Measurement::SampleEventSubscription sceneTrackerSampleEventSubscription_;

		/// The map holding the latest room objects.
		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap roomObjectMap_;

		/// The previous screen position when the user touched the screen.
		Vector2 previousScreenPosition_ = Vector2(Numeric::minValue(), Numeric::minValue());

		/// The rotation angle around the y-axis for the room preview.
		std::atomic<float> previewViewRotationAngle_ = 0.0f;

		/// The lock for room objects.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ROOM_PLAN_TRACKER_EXPERIENCE_H
