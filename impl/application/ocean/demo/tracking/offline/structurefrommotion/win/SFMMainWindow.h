/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OFFLINE_STRUCTURE_FROM_MOTION_WIN_SFM_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OFFLINE_STRUCTURE_FROM_MOTION_WIN_SFM_MAIN_WINDOW_H

#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMApplication.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/MovieFrameProviderInterface.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/tracking/Database.h"

/**
 * This struct implements some parameters to the SFM code
 * @ingroup applicationdemotrackingofflinestructurefrommotionwin
 */
struct SFMParameters
{
  // Note: at some point should make these command-line settable
  unsigned int binSize = 40; //!< number of pixels per filtering bin (0 to avoid)
  unsigned int strength = 5; //!< minimal strength parameter for tracking range[0, 256]
  unsigned int coarsestLayerRadius = 20; //!< search radius on the coarsest pyramid layer in pixels, with range[2, infinity)
  unsigned int pyramidLayers = 4; //!< number of pyramid layers, with range[1, infinity)
};

/**
 * This class implements the main window of the application.
 * @ingroup applicationdemotrackingofflinestructurefrommotionwin
 */
class SFMMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow,
	public SFMParameters
{
	protected:

		/**
		 * Definition of individual application states.
		 */
		enum ApplicationState
		{
			/// Idle state.
			AS_IDLE,
			/// Paints the feature paths.
			AS_PAINT_FEATURE_PATHS,
			/// Determines the initial object points.
			AS_DETERMINE_INITIAL_OBJECT_POINTS,
			/// Paints the initial object points.
			AS_PAINT_INITIAL_OBJECT_POINTS,
			/// Extends the initial object points.
			AS_EXTEND_INITIAL_OBJECT_POINTS,
			/// Paints the extended initial object points.
			AS_PAINT_EXTENDED_INITIAL_OBJECT_POINTS,
			/// Extends the final initial object points.
			AS_EXTEND_FINAL_INITIAL_OBJECT_POINTS,
			/// Paints the final object points.
			AS_PAINT_FINAL_OBJECT_POINTS,
			/// The user can define a plane.
			AS_DEFINE_PLANE,
			/// The application failed.
			AS_BREAK
		};

		/**
		 * Definition of a vector holding image points.
		 */
		typedef std::vector<Vectors2> ImagePointGroups;

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 * @param commandArguments The command arguments with optional configurations
		 */
		SFMMainWindow(HINSTANCE instance, const std::wstring& name, const CommandArguments& commandArguments);

		/**
		 * Destructs the main window.
		 */
		virtual ~SFMMainWindow();

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for mouse move event.
		 * @see Window::onMouseMove().
		 */
		void onMouseMove(const MouseButton buttons, const int x, const int y) override;

		/**
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Functions for mouse wheel events.
		 * @see Window::onMouseWheel().
		 */
		void onMouseWheel(const MouseButton buttons, const int wheel, const int x, const int y) override;

		/**
		 * Paints a frame and adds visual information.
		 * The frame index will be incremented afterwards, if the frame index exceeds the pose borders the frame index is reset to the lower pose border.<br>
		 * @param text Optional text
		 * @param showCorrespondences True, to visualize the point correspondences
		 * @param showImagePoints True, to visualize the image points if the correspondences are visualized
		 * @return False, if no further frame is available
		 */
		bool paintFrame(const std::string& text, const bool showCorrespondences = true, const bool showImagePoints = true);

		/**
		 * Determines the motion of the camera within the defined frame borders.
		 * @param lowerFrame The index of the lower frame
		 * @param upperFrame The index of the upper frame
		 * @return True, if succeeded
		 */
		bool determineCameraMotion(const unsigned int lowerFrame, const unsigned int upperFrame);

		/**
		 * Sends maintenance information regarding the current environment stored in the database.
		 */
		void maintenanceSendEnvironment();

		/**
		 * Determines a point on a given 3D plane which is located at the interection between plane and the ray of a given image point.
		 * @param plane The 3D plane, must be valid
		 * @param imagePoint The 2D image point
		 * @param objectPoint The resulting 3D object point
		 * @return True, if succeeded
		 */
		bool determinePointOnPlane(const Plane3& plane, const Vector2& imagePoint, Vector3& objectPoint);

		/**
		 * Writes the tracking database to a file.
		 * @param camera The camera profile defining the projection, matching with the database
		 * @param database The database that will be written
		 * @param filename The name of the file which will received the database information
		 * @return True, if succeeded
		 */
		static bool writeDatabase(const PinholeCamera& camera, const Tracking::Database& database, const std::string& filename);

		/**
		 * Reads a tracking database from a file.
		 * @param filename The name of the file from which the database will be read
		 * @param camera The resulting camera profile, matching with the database
		 * @param database The resulting database that has been read
		 * @return True, if succeeded
		 */
		static bool readDatabase(const std::string& filename, PinholeCamera& camera, Tracking::Database& database);

	protected:

		/// The command arguments.
		const CommandArguments& commandArguments_;

		/// The current state of the application.
		ApplicationState applicationState_;

		/// The database holding object points, image points and camera poses.
		Tracking::Database database_;

		/// The camera profile.
		PinholeCamera camera_;

		/// The frame provider interface.
		CV::FrameProviderInterfaceRef frameProviderInterface_;

		/// Random generator object.
		RandomGenerator randomGenerator_;

		/// The current frame index.
		unsigned int frameIndex_;

		/// The number of frames.
		unsigned int frameNumber_;

		/// The lower valid pose.
		unsigned int lowerPoseBorder_;

		/// The upper valid pose.
		unsigned int upperPoseBorder_;

		/// The frame paint slow-down factor.
		unsigned int frameSlowDownFactor_;

		/// Media file to be used.
		std::string mediaFile_;

		/// The mask frame which is used to define an area of interest.
		Frame maskFrame_;

		/// And optional transformation between a plane and world, will be rendered if defined.
		HomogenousMatrix4 world_T_plane_;

		/// The expansion of the plane, in world domain.
		Scalar planeSize_;

		/// The 3D location on the plane which is used to move the plane.
		Vector3 planeMoveObjectPoint_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OFFLINE_STRUCTURE_FROM_MOTION_WIN_SFM_MAIN_WINDOW_H
