/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_CONTROLLER_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_CONTROLLER_VISUALIZER_H

#include "ocean/platform/meta/quest/openxr/application/Application.h"

#include "ocean/platform/meta/quest/application/VRControllerVisualizer.h"

#include "ocean/platform/meta/quest/openxr/TrackedController.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

/**
 * This class implements helper functions allowing to visualize the controllers of Quest headsets in an Ocean-based VR application (e.g., VRNativeApplication) using OpenXR as input.
 * @see VRNativeApplication.
 * @ingroup platformmetaquestopenxrapplication
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT VRControllerVisualizer final : public Quest::Application::VRControllerVisualizer
{
	public:

		/**
		 * Default constructor.
		 */
		VRControllerVisualizer() = default;

		/**
		 * Creates a new controller visualizer and initializes the object with a given rendering engine and associated framebuffer and load the models from the specified files
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 * @param leftRenderModelFilename The path to file that contains the render model of the left controller, must be valid
		 * @param rightRenderModelFilename The path to file that contains the render model of the left controller, must be valid
		 * @param controllerAim_t_controllerModel The translation offset between controller model and controller aim transformation, must be valid
		 * @see Quest::Application::VRControllerVisualizer::VRControllerVisualizer().
		 */
		inline VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename, const Vector3& controllerAim_t_controllerModel = Vector3(0, 0, 0));

		/**
		 * Creates a new controller visualizer and initializes the object with a given rendering engine and associated framebuffer and load the models from the specified files
		 * @see Quest::Application::VRControllerVisualizer::VRControllerVisualizer().
		 */
		VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const Device::DeviceType deviceType, const std::string& renderModelDirectoryName);

		/**
		 * Visualizes both controllers of an Oculus headset at a specific location in the virtual environment (defined in relation to the world).
		 * A previous visualization can be updated by specifying the same controller again in conjunction with a new transformation.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param trackedController The instance of a tracked controller that is used to query the pose of the controllers, must be valid
		 * @param controllerRayLength Optional explicit parameter used for the length of the controller ray, will be hidden for value 0, -1 to use the length as defined in controllerRayLength()
		 */
		void visualizeControllersInWorld(const TrackedController& trackedController, const Scalar controllerRayLength = -1);

	protected:

		/// The translation offset between controller model and controller aim transformation, must be valid
		Vector3 controllerAim_t_controllerModel_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
};

inline VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename, const Vector3& controllerAim_t_controllerModel) :
	Quest::Application::VRControllerVisualizer(engine, framebuffer, leftRenderModelFilename, rightRenderModelFilename),
	controllerAim_t_controllerModel_(controllerAim_t_controllerModel)
{
	// nothing to do here
}

#ifdef OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_TRANSLATION_OFFSET

/**
 * Returns the translation offset for an external controller.
 * @param deviceType The device type associated with the external controller, must be valid
 * @return The translation offset, Vector3(0, 0, 0) if unknown
 */
Vector3 VRControllerVisualizer_externalTranslationOffset(const uint32_t deviceType);

#endif // OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_TRANSLATION_OFFSET

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_CONTROLLER_VISUALIZER_H
