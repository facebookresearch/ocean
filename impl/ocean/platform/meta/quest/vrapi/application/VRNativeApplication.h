// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_NATIVE_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_NATIVE_APPLICATION_H

#include "ocean/platform/meta/quest/vrapi/application/NativeApplication.h"

#include "ocean/platform/meta/quest/application/VRImageVisualizer.h"
#include "ocean/platform/meta/quest/application/VRTextVisualizer.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/StereoView.h"

#include "ocean/rendering/glescenegraph/quest/vrapi/GLESWindowFramebuffer.h"

#include <VrApi_Types.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

/**
 * This class implements a basic Quest (VR) application using Ocean's scene graph rendering pipeline (Rendering::GLESceneGraph) using VrApi.
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT VRNativeApplication : public NativeApplication
{
	public:

		/**
		 * Definition of a wrapper class for OVR layers that can be pass to time warps
		 */
		class CustomLayer
		{
			public:

				/**
				 * Creates an invalid custom layer object
				 */
				CustomLayer() = default;

				/**
				 * Creates a custom layer based on `ovrLayerProjection2`
				 * @param layerProjection2 The layer instance will stored in this object, must be valid
				 */
				inline explicit CustomLayer(std::shared_ptr<ovrLayerProjection2>&& layerProjection2);

				/**
				 * Creates a custom layer based on `layerUnion2`
				 * @param layerUnion2 The layer instance will stored in this object, must be valid
				 */
				inline explicit CustomLayer(std::shared_ptr<ovrLayer_Union2>&& layerUnion2);

				/**
				 * Returns if this is a valid custom layer
				 * @return True if the layer type and the corresponding stored pointer are both valid, otherwise false
				 */
				inline bool isValid() const;

				/**
				 * Returns a pointer to the header of the layer that is stored in this instance
				 * @return The header of the stored layer, will be `nullptr` is this instance is invalid
				 */
				inline const ovrLayerHeader2* layerHeader() const;

			protected:

				/// Pointer to an `ovrLayerProjection2`
				std::shared_ptr<ovrLayerProjection2> ovrLayerProjection2_;

				/// Pointer to an `ovrLayerProjection2`
				std::shared_ptr<ovrLayer_Union2> ovrLayer_Union2_;

				/// Pointer to the header object of that layer, that is stored in this instance
				const ovrLayerHeader2* header_ = nullptr;
		};

		/// Definition of a vector of custom layers
		typedef std::vector<CustomLayer> CustomLayers;

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit VRNativeApplication(struct android_app* androidApp);

		/**
		 * Destructs this object.
		 */
		~VRNativeApplication() override;

		/**
		 * Adds a custom layer that will be submitted to time warp
		 * Note that custom layers must be added in each iteration in order to be added to the rendering.
		 * @param customLayer The custom layer that will be added, must be valid
		 * @param postLayer True, the custom layer will be added after the internal main layer, otherwise it will be added before the main layer
		 * @return True if the custom layer was successfully added, otherwise false
		 */
		inline bool addCustomLayer(CustomLayer&& customLayer, const bool postLayer = true);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param nativeApplication Application object which would have been copied
		 */
		VRNativeApplication(const VRNativeApplication& nativeApplication) = delete;

		/**
		 * Main loop of the application.
		 * @see NativeApplication::applicationLoop().
		 */
		void applicationLoop() override;

		/**
		 * Renders a new frame.
		 * @param predictedDisplayTime The predicted display time as provided, with range (0, infinity)
		 */
		virtual void render(const double predictedDisplayTime);

		/**
		 * Returns the TrackedRemoteDevice object allowing access to event of remotes.
		 * @return The desired object
		 */
		inline TrackedRemoteDevice& trackedRemoteDevice();

		/**
		 * Returns whether the VrApi VR mode is currently entered.
		 * @see NativeApplication::isVRModeEntered().
		 */
		bool isVRModeEntered() const override;

		/**
		 * Idle event function called within the main loop whenever all Android related events have been processed.
		 * @see NativeApplication::onIdle().
		 */
		void onIdle() override;

		/**
		 * The event function which is called when all resources should finally be released.
		 */
		virtual void onReleaseResources();

		/**
		 * Event function called after the framebuffer has been initialized.
		 * This event function can be used to configure the framebuffer or to add rendering content.
		 */
		virtual void onFramebufferInitialized();

		/**
		 * Event function called before the framebuffer will be released.
		 * This event function can be used to release depending resources.
		 */
		virtual void onFramebufferReleasing();

		/**
		 * Events function called when the VR mode has been entered.
		 */
		virtual void onVrModeEntered();

		/**
		 * Events function called when the VR mode has been left.
		 */
		virtual void onVrModeLeft();

		/**
		 * Events function called before the scene is rendered.
		 * @param renderTimestamp The timestamp which will be used for rendering, must be valid
		 */
		virtual void onPreRender(const Timestamp& renderTimestamp);

		/**
		 * Event functions for pressed buttons (e.g., from a tracked remote device).
		 * @param buttons The buttons currently pressed either by the left or right remote device/controller
		 * @param buttonsLeft The buttons currently pressed by the left remote device/controller
		 * @param buttonsRight The buttons currently pressed by the right remote device/controller
		 * @param timestamp The timestamp of the event
		 */
		virtual void onButtonPressed(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp);

		/**
		 * Event functions for released buttons (e.g., from a tracked remote device).
		 * @param buttons The buttons currently pressed either by the left or right remote device/controller
		 * @param buttonsLeft The buttons currently pressed by the left remote device/controller
		 * @param buttonsRight The buttons currently pressed by the right remote device/controller
		 * @param timestamp The timestamp of the event
		 */
		virtual void onButtonReleased(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp);

		/**
		 * Registers the system fonts.
		 */
		void registerSystemFonts();

		/**
		 * Disabled copy operator.
		 * @param nativeApplication Application object which would have been copied
		 * @return Reference to this object
		 */
		VRNativeApplication& operator=(const VRNativeApplication& nativeApplication) = delete;

	protected:

		/// The rendering engine to be used in the VR application.
		Rendering::EngineRef engine_;

		/// The rendering framebuffer to be used in the VR application.
		Rendering::WindowFramebufferRef framebuffer_;

		/// The stereo view to be used in the VR Application.
		Rendering::StereoViewRef stereoView_;

		/// The ovrMobile object of this application.
		ovrMobile* ovrMobile_ = nullptr;

		/// The index of the current frame.
		unsigned int frameIndex_ = 0u;

		/// The visualizer for images (mainly a helper class creating textured SceneGraph objects to display image content).
		Quest::Application::VRImageVisualizer vrImageVisualizer_;

		/// The visualizer for text (mainly a helper class creating textured SceneGraph objects to display text).
		Quest::Application::VRTextVisualizer vrTextVisualizer_;

		/// If stencil buffer should be used.
		bool useStencilBuffer_ = false;

	private:

		/// The explicit pointer to the Quest-specialized framebuffer (identical to framebuffer_, but pointing to the derived class).
		Rendering::GLESceneGraph::Quest::VrApi::GLESWindowFramebuffer* questFramebuffer_ = nullptr;

		/// The tracked remote device object providing events of remotes.
		TrackedRemoteDevice trackedRemoteDevice_;

		/// Custom layers that will be prepended to the main layer
		CustomLayers customPreLayers_;

		/// Custom layers that will be appended to the main layer
		CustomLayers customPostLayers_;
};

inline VRNativeApplication::CustomLayer::CustomLayer(std::shared_ptr<ovrLayerProjection2>&& layerProjection2) :
	ovrLayerProjection2_(std::move(layerProjection2))
{
	ocean_assert(ovrLayerProjection2_ != nullptr);
	header_ = &ovrLayerProjection2_->Header;
}

inline VRNativeApplication::CustomLayer::CustomLayer(std::shared_ptr<ovrLayer_Union2>&& layerUnion2) :
	ovrLayer_Union2_(std::move(layerUnion2))
{
	ocean_assert(ovrLayer_Union2_ != nullptr);
	header_ = &ovrLayer_Union2_->Header;
}

inline bool VRNativeApplication::CustomLayer::isValid() const
{
	return header_ != nullptr;
}

inline const ovrLayerHeader2* VRNativeApplication::CustomLayer::layerHeader() const
{
	return header_;
}

inline bool VRNativeApplication::addCustomLayer(CustomLayer&& customLayer, const bool postLayer)
{
	if (!customLayer.isValid())
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	if (postLayer)
	{
		customPostLayers_.emplace_back(std::move(customLayer));
	}
	else
	{
		customPreLayers_.emplace_back(std::move(customLayer));
	}

	return true;
}

inline TrackedRemoteDevice& VRNativeApplication::trackedRemoteDevice()
{
	return trackedRemoteDevice_;
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_NATIVE_APPLICATION_H
