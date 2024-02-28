// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GRAVITY_AND_HEADING_NATIVE_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GRAVITY_AND_HEADING_NATIVE_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an basic experience showing how to show Gravity & Heading direction in the view.
 *
 * The same experience can be realized with one single OX3D file as well.<br>
 * Here we just show how to create the same experience in native code.<br>
 * This experience uses the gravity and heading orientation tracker and updates two 3D arrows accordingly.
 * @ingroup xrplayground
 */
class GravityAndHeadingNativeExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~GravityAndHeadingNativeExperience() override;

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
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		GravityAndHeadingNativeExperience() = default;

	protected:

		/// The 3-DOF orientation tracker for the gravity direction.
		Devices::OrientationTracker3DOFRef gravityTracker3DOF_;

		/// The 3-DOF orientation tracker for the heading direction.
		Devices::OrientationTracker3DOFRef headingTracker3DOF_;

		/// The rendering Transform object holding the gravity arrow.
		Rendering::TransformRef renderingGravityTransform_;

		/// The rendering Transform object holding the heading arrow.
		Rendering::TransformRef renderingHeadingTransform_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GRAVITY_AND_HEADING_NATIVE_EXPERIENCE_H
