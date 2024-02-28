// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		 * @see Quest::Application::VRControllerVisualizer::VRControllerVisualizer().
		 */
		inline VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename);

		/**
		 * Creates a new controller visualizer and initializes the object with a given rendering engine and associated framebuffer and load the models from the specified files
		 * @see Quest::Application::VRControllerVisualizer::VRControllerVisualizer().
		 */
		inline VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const Device::DeviceType deviceType, const std::string& renderModelDirectoryName);

		/**
		 * Visualizes both contollers of an Oculus headset at a specific location in the virtual environment (defined in relation to the world).
		 * A previous visualization can be updated by specifying the same controller again in conjuction with a new transformation.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param trackedController The instance of a tracked controller that is used to query the pose of the controllers, must be valid
		 * @param controllerRayLength Optional explicit parameter used for the length of the controller ray, will be hidden for value 0, -1 to use the length as defined in controllerRayLength()
		 */
		void visualizeControllersInWorld(const TrackedController& trackedController, const Scalar controllerRayLength = -1);
};

inline VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename) :
	Quest::Application::VRControllerVisualizer(engine, framebuffer, leftRenderModelFilename, rightRenderModelFilename)
{
	// nothing to do here
}

inline VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const Device::DeviceType deviceType, const std::string& renderModelDirectoryName) :
	Quest::Application::VRControllerVisualizer(engine, framebuffer, deviceType, renderModelDirectoryName)
{
	// nothing to do here
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_CONTROLLER_VISUALIZER_H
