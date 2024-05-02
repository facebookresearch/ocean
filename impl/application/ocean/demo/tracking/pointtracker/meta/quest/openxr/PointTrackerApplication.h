// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef APPLICATION_DEMO_TRACKING_POINTTRACKER_META_QUEST_OPENXR_POINT_TRACKER_APPLICATION_H
#define APPLICATION_DEMO_TRACKING_POINTTRACKER_META_QUEST_OPENXR_POINT_TRACKER_APPLICATION_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/Thread.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/media/PixelImage.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"

#include "ocean/tracking/point/PointTracker.h"

#include "metaonly/ocean/platform/meta/quest/sensors/FrameProvider.h"

#include <android_native_app_glue.h>

using namespace Ocean;

/**
 * The class implements the actual application logic.
 * @ingroup applicationdemotracking
 */
class PointTrackerApplication :
	public Platform::Meta::Quest::OpenXR::Application::VRNativeApplicationAdvanced,
	protected Thread
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit PointTrackerApplication(struct android_app* androidApp);

	protected:

		/**
		* Event function call when an Android (or Oculus) permission is granted.
		* @see NativeApplication::onAndroidPermissionGranted().
		*/
		void onAndroidPermissionGranted(const std::string& permission) override;

		/**
		 * Initializes the frame provider.
		 * @return True, if succeeded
		 */
		bool initializeFrameProvider();

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

	protected:

		/// The frame provider for the cameras.
		Platform::Meta::Quest::Sensors::FrameProvider frameProvider_;

		/// The actual implementation of the point tracker.
		Tracking::Point::PointTracker pointTracker_;

		/// The 3D object points corresponding to strong feature points.
		Vectors3 objectPoints_;

		/// The frame counter in which the 3D object points are visualized.
		unsigned int frameCounter_ = 0u;

		/// The left display image.
		Frame displayLeft_;

		/// The right display image.
		Frame displayRight_;

		/// The lock to share the tracking results between the worker and the visualization threads.
		Lock resultLock_;

		/// Flag for when results are available to render.
		bool haveResults_ = false;

		/// The rotation angle (in radians) to rotate camera images for display
		float imageRotation_ = 0.0f;
};

#endif // APPLICATION_DEMO_TRACKING_POINTTRACKER_META_QUEST_OPENXR_POINT_TRACKER_APPLICATION_H
