// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_XR_PLAYGROUND_COMMON_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_XR_PLAYGROUND_COMMON_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "ocean/devices/Device.h"
#include "ocean/devices/GPSTracker.h"

#include "ocean/interaction/UserInterface.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/MediaTexture2D.h"

#include "metaonly/ocean/devices/vrs/DeviceRecorder.h"

#include "metaonly/ocean/platform/meta/Login.h"

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	#include "metaonly/ocean/platform/meta/avatars/Manager.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the platform independent part of the XRPlayground app.
 * The class handling rendering, content management and VRS recordings.
 * @ingroup xrplayground
 */
class XRPlaygroundCommon
{
	public:

		/**
		 * Creates a new platform independent XRPlayground object, each app must create one object only.
		 */
		XRPlaygroundCommon();

		/**
		 * Destructs the platform independent XRPlayground object.
		 */
		~XRPlaygroundCommon();

		/**
		 * Initializes the rendering engine and additional resources.
		 * This function should not be called twice.
		 * @param version The version of the app, if known
		 * @return True, if succeeded
		 */
		bool initialize(const std::string& appVersion = std::string());

		/**
		 * Loads or adds new content e.g., a scene file or interaction file.
		 * @param content The new contant to be loaded, must be valid
		 * @param replace True, to replace currently loaded content; False, to add the new content
		 * @return True, if succeeded
		 */
		bool loadContent(const std::string& content, const bool replace);

		/**
		 * Unloads previously loaded content.
		 * @param content The content to be unloaded, must be valid
		 * @return True, if succeeded
		 */
		bool unloadContent(const std::string& content);

		/**
		 * Unloads all previously loaded content.
		 */
		void unloadContent();

		/**
		 * Stars the VRS recording.
		 * @param filename The name of the new VRS file, must be valid
		 * @return True, if succeeded
		 */
		bool startVRSRecording(const std::string& filename);

		/**
		 * Stops the VRS recording.
		 * @return True, if succeeded
		 */
		bool stopVRSRecording();

		/**
		 * Returns whether VRS recording is currently active.
		 * @return True, if so
		 */
		bool isVRSRecordingActive();

		/**
		 * Resizes the framebuffer of the XRPlayground app.
		 * @param width The width of the framebuffer, in pixel, with range [1, infinity), 0 if unknown
		 * @param height The height of the framebuffer, in pixel, with range [1, infinity), 0 if unknown
		 * @param aspectRatio The aspect ratio of the framebuffer, with range (0, infinity), 0 if unknown
		 */
		void resize(const unsigned int width, const unsigned int height, const Scalar aspectRatio);

		/**
		 * Applies all necessary updates before rendering a new frame.
		 * @param userInterface The application's user interface
		 */
		void preRender(const Interaction::UserInterface& userInterface);

		/**
		 * Renders a new frame.
		 */
		void render();

		/**
		 * Sets the transformation between display and device.
		 * @param device_T_display The transformation between display and device, must be valid
		 * @return True, if the transformation could be set
		 */
		bool setDevice_T_display(const HomogenousMatrix4& device_T_display);

		/**
		 * Returns the frame medium which is currently used as video background.
		 * @return The currently used frame medium, invalid if no medium is used
		 */
		Media::FrameMediumRef backgroundFrameMedium() const;

		/**
		 * Sets or changes the frame medium which is used as video background.
		 * @param frameMedium The frame medium to be used, invalid to remove an existing video background
		 * @return True, if succeeded
		 */
		bool setBackgroundFrameMedium(const Media::FrameMediumRef& frameMedium);

		/**
		 * Releases this app and all associated resources.
		 * Must be called out of the main thread.
		 */
		void release();

		/**
		 * Returns the rendering engine which is used.
		 * @return The app's rendering engine
		 */
		inline Rendering::EngineRef engine() const;

		/**
		 * Returns the rendering main framebuffer which is used.
		 * @return The app's rendering main framebuffer
		 */
		inline Rendering::FramebufferRef framebuffer() const;

		/**
		 * Returns whether the rendering engine is initialized.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

		/**
		 * Informs XRPlayground about a started touch event.
		 * @param userInterface The application's user interface
		 * @param screenPosition The 2D screen position of the touch event, in pixel, with range [0, infinity)x[0, infinity)
		 */
		void touchEventStarted(const Interaction::UserInterface& userInterface, const Vector2& screenPosition);

		/**
		 * Informs XRPlayground about a moved touch event.
		 * @param userInterface The application's user interface
		 * @param screenPosition The 2D screen position of the touch event, in pixel, with range [0, infinity)x[0, infinity)
		 */
		void touchEventMoved(const Interaction::UserInterface& userInterface, const Vector2& screenPosition);

		/**
		 * Informs XRPlayground about a stopped touch event.
		 * @param userInterface The application's user interface
		 * @param screenPosition The 2D screen position of the touch event, in pixel, with range [0, infinity)x[0, infinity)
		 */
		void touchEventStopped(const Interaction::UserInterface& userInterface, const Vector2& screenPosition);

		/**
		 * Informs XRPlayground about a pressed key event.
		 * @param userInterface The application's user interface
		 * @param key The key which has been pressed
		 */
		void keyEventPressed(const Interaction::UserInterface& userInterface, const std::string& key);

		/**
		 * Informs XRPlayground about a released key event.
		 * @param userInterface The application's user interface
		 * @param key The key which has been released
		 */
		void keyEventReleased(const Interaction::UserInterface& userInterface, const std::string& key);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param xrPlaygroundCommon Object which would be copied
		 */
		XRPlaygroundCommon(const XRPlaygroundCommon& xrPlaygroundCommon) = delete;

		/**
		 * Disabled assign operator.
		 * @param xrPlaygroundCommon Object which would be assigned
		 * @return Reference to this object
		 */
		XRPlaygroundCommon& operator=(const XRPlaygroundCommon& xrPlaygroundCommon) = delete;

		/**
		 * Initializes the verts system and avatar system.
		 */
		void initializeNetworking();

		/**
		 * Determines the picked object for a given screen location.
		 * @param screenPosition The screen position for which the picked object will be determined, in pixel, with range [0, infinity)x[0, infinity)
		 * @param ray The resulting 3D ray starting at the current view's center of projecting and intersecting the given screen position
		 * @param pickedObjectId The resulting id of the picked object, if any
		 * @param pickedObjectPosition The resulting 3D location at which the 3D ray intersected with the picked object, invalid if no object was picked
		 * @return True, if succeeded
		 */
		bool determinePickedObject(const Vector2& screenPosition, Line3& ray, Rendering::ObjectId& pickedObjectId, Vector3& pickedObjectPosition) const;

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

		/**
		 * Translates a login type to a user type.
		 * @param loginType The login type to translate
		 * @return The translated user type
		 */
		static Platform::Meta::Avatars::Manager::UserType translateLoginType(const Platform::Meta::Login::LoginType loginType);

#endif // OCEAN_PLATFORM_BUILD_MOBILE

	protected:

		/// The rendering engine to be used.
		Rendering::EngineRef engine_;

		/// The rendering framebuffer to be used to visualize the main content of the XRPlayground app.
		Rendering::FramebufferRef framebuffer_;

		/// The rendering scene holding additional information like e.g., the version of the app.
		Rendering::SceneRef sceneText_;

		/// We keep the GPS Tracker to ensure that the tracker is constantly available.
		Devices::GPSTrackerRef gpsTracker_;

		/// The VRS recorder which can be used to make live VRS recordings.
		Devices::VRS::DeviceRecorder deviceRecorder_;

		/// Additional devices which are needed in a VRS recording.
		std::vector<Devices::DeviceRef> vrsDevices_;

		/// Additional medium devices which need to exist as long as VRS recording is active.
		Media::FrameMediumRefs vrsFrameMediums_;

		/// The application's lock.
		mutable Lock lock_;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/// The texture object necessary for ARCore.
		Rendering::MediaTexture2DRef arCoreTexture_;

#endif
};

inline Rendering::EngineRef XRPlaygroundCommon::engine() const
{
	const ScopedLock scopedLock(lock_);

	return engine_;
}

inline Rendering::FramebufferRef XRPlaygroundCommon::framebuffer() const
{
	const ScopedLock scopedLock(lock_);

	return framebuffer_;
}

inline bool XRPlaygroundCommon::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return engine_ && framebuffer_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_XR_PLAYGROUND_COMMON_H
