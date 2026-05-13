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

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/VertexSet.h"

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
	protected:

		/// The number of controllers.
		static constexpr size_t numberControllers_ = 2;

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
		 * @param rightRenderModelFilename The path to file that contains the render model of the right controller, must be valid
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

		/**
		 * Computes the transformation of the controller render model relative to base space.
		 * Applies the same offsets that visualizeControllersInWorld uses to place the rendered controller model,
		 * so callers can align other content (e.g. virtual markers or coordinate systems) with the rendered controller.
		 * @param trackedController The instance of a tracked controller used to query the controller pose, must be valid
		 * @param controllerType The type of the controller (CT_LEFT or CT_RIGHT)
		 * @param baseSpace_T_controllerModel The resulting transformation of the controller model in base space (only valid if true is returned)
		 * @return True if the pose could be obtained and the transformation was computed; false otherwise
		 */
		bool baseSpaceFromControllerModel(const TrackedController& trackedController, const TrackedController::ControllerType controllerType, HomogenousMatrix4& baseSpace_T_controllerModel) const;

		/**
		 * Sets whether the controller coordinate systems are shown.
		 * @param show True to show the coordinate systems; False to hide them
		 */
		inline void showCoordinateSystems(const bool show);

		/**
		 * Returns whether the controller coordinate systems are shown.
		 * @return True, if the coordinate systems are shown
		 */
		inline bool coordinateSystemsShown() const;

	protected:

		/**
		 * Visualizes the coordinate system of a single controller.
		 * @param controllerIndex The index of the controller (0 = left, 1 = right)
		 * @param world_T_controller The transformation of the controller in world coordinates, must be valid
		 */
		void visualizeControllerCoordinateSystem(const size_t controllerIndex, const HomogenousMatrix4& world_T_controller);

	protected:

		/// The translation offset between controller model and controller aim transformation, must be valid
		Vector3 controllerAim_t_controllerModel_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

		/// The rendering Transform nodes for rendering the controller coordinate systems (one per controller).
		Rendering::TransformRef transformCoordinateSystems_[numberControllers_];

		/// The rendering VertexSet objects for rendering the controller coordinate systems (one per controller).
		Rendering::VertexSetRef vertexSetCoordinateSystems_[numberControllers_];

		/// The scene for the coordinate system visualizations.
		Rendering::SceneRef sceneCoordinateSystems_;

		/// True, if the controller coordinate systems are shown.
		bool showCoordinateSystems_ = false;
};

inline VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename, const Vector3& controllerAim_t_controllerModel) :
	Quest::Application::VRControllerVisualizer(engine, framebuffer, leftRenderModelFilename, rightRenderModelFilename),
	controllerAim_t_controllerModel_(controllerAim_t_controllerModel)
{
	// nothing to do here
}

inline void VRControllerVisualizer::showCoordinateSystems(const bool show)
{
	showCoordinateSystems_ = show;
}

inline bool VRControllerVisualizer::coordinateSystemsShown() const
{
	return showCoordinateSystems_;
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
