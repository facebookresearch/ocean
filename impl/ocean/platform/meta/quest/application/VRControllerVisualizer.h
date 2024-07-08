/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_CONTROLLER_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_CONTROLLER_VISUALIZER_H

#include "ocean/platform/meta/quest/application/Application.h"
#include "ocean/platform/meta/quest/application/VRVisualizer.h"

#include "ocean/platform/meta/quest/Device.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Application
{

/**
 * This class implements helper functions allowing to visualize the controllers of Quest headsets in an Ocean-based VR application (e.g., VRNativeApplication).
 * @see VRNativeApplication.
 * @ingroup platformmetaquestapplication
 */
class OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT VRControllerVisualizer : public VRVisualizer
{
	public:

		/**
		 * Identifier for the different controllers
		 */
		enum ControllerType : uint32_t
		{
			/// Identifier of the controller for the left hand
			CT_LEFT = 0u,
			/// Identifier of the controller for the right hand
			CT_RIGHT
		};

		/**
		 * This class implements a scoped state object allowing to reset all states of a visualizer.
		 * The state can be stored locally or states can be pushed onto a stack.
		 * @see pushState(), popState().
		 */
		class ScopedState
		{
			public:

				/**
				 * Default constructor.
				 */
				ScopedState() = default;

				/**
				 * Creates a new state object.
				 * @param vrControllerVisualizer The visualizer to which this new object belongs
				 */
				ScopedState(VRControllerVisualizer& vrControllerVisualizer);

				/**
				 * Move constructor.
				 * @param scopedState The state object to be moved
				 */
				ScopedState(ScopedState&& scopedState);

				/**
				 * Destructs this object and releases the state.
				 */
				~ScopedState();

				/**
				 * Explicitly releases this state.
				 * The properties of the owning visualizer will be reset to the situation when the state was created.
				 */
				void release();

				/**
				 * Move operator.
				 * @param scopedState The state object to be moved
				 * @return Reference to this object
				 */
				ScopedState& operator=(ScopedState&& scopedState);

			protected:

				/**
				 * Disabled copy constructor.
				 */
				ScopedState(const ScopedState&) = delete;

				/**
				 * Disabled assign operator.
				 * @return The reference to this object
				 */
				ScopedState& operator=(const ScopedState&) = delete;

			protected:

				/// The visualizer to which this state object belongs.
				VRControllerVisualizer* vrControllerVisualizer_ = nullptr;

				/// True, if the controllers where shown when this object was created.
				bool wasShown_ = false;

				/// The length of the controller ray, when this object was created.
				Scalar controllerRayLength_ = Scalar(0);
		};

		/**
		 * Definition of a vector holding state objects.
		 */
		typedef std::vector<ScopedState> ScopedStates;

	public:

		/**
		 * Default constructor, creates a new invalid visualizer.
		 */
		VRControllerVisualizer() = default;

		/**
		 * Creates a new controller visualizer and initializes the object with a given rendering engine and associated framebuffer and load the models from the specified files
		 * Rendering engine and framebuffer are necessary so that the rendering objects (like Scene, Transform, Texture2D) can be created and attached to the existing rendering objects.
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 * @param leftRenderModelFilename The path to file that contains the render model of the left controller, must be valid
		 * @param rightRenderModelFilename The path to file that contains the render model of the left controller, must be valid
		 */
		inline VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename);

		/**
		 * Creates a new controller visualizer and initializes the object with a given rendering engine and associated framebuffer and load the models from the specified files
		 * Rendering engine and framebuffer are necessary so that the rendering objects (like Scene, Transform, Texture2D) can be created and attached to the existing rendering objects.
		 * This function is looking the files in the directory `renderModelDirectoryName`:
		 * <pre>
		 * | Device Type        | Controller Type | Expected filename            |
		 * |--------------------+-----------------+------------------------------|
		 * | Device::DT_QUEST   | CT_LEFT         | quest_controller_left.obj    |
		 * | Device::DT_QUEST   | CT_RIGHT        | quest_controller_right.obj   |
		 * | Device::DT_QUEST_2 | CT_LEFT         | quest2_controller_left.obj   |
		 * | Device::DT_QUEST_2 | CT_RIGHT        | quest2_controller_right.obj  |
		 * | Device::DT_QUEST_3 | CT_LEFT         | quest3_controller_left.obj   |
		 * | Device::DT_QUEST_3 | CT_RIGHT        | quest3_controller_right.obj  |
		 * </pre>
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 * @param deviceType The device type that this object will be configured for, must not be `Device::DT_UNKNOWN`
		 * @param renderModelDirectoryName The location where render models for the specified device type will be loaded from, must be valid
		 */
		inline VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const Device::DeviceType deviceType, const std::string& renderModelDirectoryName);

		/**
		 * Move constructor
		 * @param vrControllerVisualizer Another instance of the controller visualizer that will be moved to this instance
		 */
		inline VRControllerVisualizer(VRControllerVisualizer&& vrControllerVisualizer);

		/**
		 * Destructor
		 */
		~VRControllerVisualizer() override;

		/**
		 * Shows the controllers.
		 * @see hide().
		 */
		inline void show();

		/**
		 * Hides the controllers.
		 * @see show().
		 */
		inline void hide();

		/**
		 * Returns whether the controllers are shown.
		 * @return True, if succeeded
		 */
		inline bool isShown() const;

		/**
		 * Returns the length of the controller ray.
		 * By default, the controller ray is not visible.
		 * @return The length of the controller ray, in meter, the ray is hidden for value 0, with range [0, infinity)
		 * @see setControllerRayLength().
		 */
		inline Scalar controllerRayLength() const;

		/**
		 * Sets the length of the controller ray.
		 * @param length The length of the controller ray, in meter, the ray will be hidden for value 0, with range [0, infinity)
		 */
		inline void setControllerRayLength(const Scalar length);

		/**
		 * Visualizes a controller of an Oculus headset at a specific location in the virtual environment (defined in relation to the world).
		 * A previous visualization can be updated by specifying the same controller again in conjunction with a new transformation.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param controllerType The identifier of the controller that will updated
		 * @param world_T_controller The transformation at which the controller will be displayed, transforming controller to world, invalid if the controller is currently not tracked
		 * @param controllerRayLength Optional explicit parameter used for the length of the controller ray, will be hidden for value 0, -1 to use the length as defined in controllerRayLength()
		 */
		void visualizeControllerInWorld(const ControllerType controllerType, const HomogenousMatrix4& world_T_controller, const Scalar controllerRayLength = -1);

		/**
		 * Pushes a new configuration state to the stack.
		 * Each push needs to be balanced with a pop.
		 * @see popState().
		 */
		inline void pushState();

		/**
		 * Pops the most recent state from the stack and resets the visualizer's configuration accordingly.
		 */
		inline void popState();

		/**
		 * Releases this visualizer and all associated resources.
		 */
		void release();

		/**
		 * Move-assignment operator
		 * @param vrControllerVisualizer Another instance of the controller visualizer that will be moved to this instance
		 */
		VRControllerVisualizer& operator=(VRControllerVisualizer&& vrControllerVisualizer);

	protected:

		/**
		 * Disabled copy constructor
		 */
		inline VRControllerVisualizer(const VRControllerVisualizer&) = delete;

		/**
		 * Loads the render models from the specified files
		 * @param leftRenderModelFilename The path to file that contains the render model of the left controller, must be valid
		 * @param rightRenderModelFilename The path to file that contains the render model of the left controller, must be valid
		 * @return True if the models have been loaded successfully
		 */
		bool loadModels(const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename);

		/**
		 * Loads the render models from the specified files
		 * @param deviceType The device type that this object will be configured for, must not be `Device::DT_UNKNOWN`
		 * @param renderModelDirectoryName The location where render models for the specified device type will be loaded from, must be valid
		 * @return True if the models have been loaded successfully
		 */
		bool loadModels(const Device::DeviceType deviceType, const std::string& renderModelDirectoryName);

		/**
		 * Disabled copy-assignment operator
		 * @return Reference to this object
		 */
		VRControllerVisualizer& operator=(const VRControllerVisualizer&) = delete;

	protected:

		/// The type of the device.
		Device::DeviceType deviceType_ = Device::DT_UNKNOWN;

		/// The scene object of the renderer;
		Rendering::SceneRef leftControllerScene_;

		/// The scene object of the renderer;
		Rendering::SceneRef rightControllerScene_;

		/// The transformation of the left controller ray
		Rendering::TransformRef leftControllerRayTransform_;

		/// The transformation of the right controller ray
		Rendering::TransformRef rightControllerRayTransform_;

		/// True, if the controllers are visualized; False, if the controllers are hidden.
		std::atomic<bool> isShown_;

		/// The length of the controller ray, in meter, the ray will be hidden for value 0, with range [0, infinity).
		std::atomic<Scalar> controllerRayLength_ = Scalar(0);

		/// A stack of visualization states.
		ScopedStates stateStack_;
};

inline VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename) :
	VRVisualizer(engine, framebuffer),
	isShown_(true),
	controllerRayLength_(0)
{
	ocean_assert(!leftRenderModelFilename.empty() && !rightRenderModelFilename.empty());
	if (!loadModels(leftRenderModelFilename, rightRenderModelFilename))
	{
		Log::error() << "Failed to load the render models of the controllers";
	}
}

inline VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const Device::DeviceType deviceType, const std::string& renderModelDirectoryName) :
	VRVisualizer(engine, framebuffer),
	isShown_(true),
	controllerRayLength_(0)
{
	ocean_assert(deviceType != Device::DT_UNKNOWN);
	ocean_assert(!renderModelDirectoryName.empty());

	if (loadModels(deviceType, renderModelDirectoryName))
	{
		deviceType_ = deviceType;
	}
	else
	{
		Log::error() << "Failed to load the render models of the controllers";
	}
}

inline VRControllerVisualizer::VRControllerVisualizer(VRControllerVisualizer&& vrControllerVisualizer) :
	VRControllerVisualizer()
{
	*this = std::move(vrControllerVisualizer);
}

inline void VRControllerVisualizer::show()
{
	isShown_ = true;
}

inline void VRControllerVisualizer::hide()
{
	isShown_ = false;
}

inline bool VRControllerVisualizer::isShown() const
{
	return isShown_;
}

inline Scalar VRControllerVisualizer::controllerRayLength() const
{
	return controllerRayLength_;
}

inline void VRControllerVisualizer::setControllerRayLength(const Scalar length)
{
	controllerRayLength_ = length;
}

inline void VRControllerVisualizer::pushState()
{
	const ScopedLock scopedLock(lock_);

	stateStack_.emplace_back(*this);
}

inline void VRControllerVisualizer::popState()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!stateStack_.empty());

	stateStack_.pop_back();
}

#ifdef OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_CONTROLLER_MODEL_FILE

/**
 * Returns the model file for an external controller.
 * @param deviceType The device type associated with the external controller, must be valid
 * @param leftModelFile The resulting model file for the left controller
 * @param rightModelFile The resulting model file for the right controller
 * @return True, if succeeded
 */
bool VRControllerVisualizer_externalControllerModelFiles(const uint32_t deviceType, std::string& leftModelFile, std::string& rightModelFile);

#endif // OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_CONTROLLER_MODEL_FILE

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_CONTROLLER_VISUALIZER_H
