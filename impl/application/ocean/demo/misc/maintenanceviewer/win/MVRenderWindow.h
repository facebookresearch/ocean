/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_RENDER_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_RENDER_WINDOW_H

#include "application/ocean/demo/misc/maintenanceviewer/win/MVApplication.h"

#include "ocean/base/Frame.h"

#include "ocean/platform/win/Window.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/WindowFramebuffer.h"

/**
 * This class implements a renderer window of the application.
 * @ingroup applicationdemomiscmaintenanceviewerwin
 */
class MVRenderWindow : virtual public Platform::Win::Window
{
	public:

		/**
		 * Creates a new main window.
		 * @param applicationInstance Application instance
		 * @param name The name of the main window
		 * @param parent Optional parent window, if any
		 */
		MVRenderWindow(HINSTANCE applicationInstance, const std::wstring& name, HWND parent = nullptr);

		/**
		 * Destructs the main window.
		 */
		~MVRenderWindow() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Adds a new mesh composed of 3D object points and corresponding 2D image points defining the topology.
		 * Further, a frame covering the image points can be provided to which will be used as mesh texture.
		 * @param objectPoints The 3D object points of the mesh
		 * @param imagePoints The 2D image points of the mesh, one image points for each object point
		 * @param frame Optional frame used as texture
		 * @return True, if succeeded
		 */
		bool addMesh(const Vectors3& objectPoints, const Vectors2& imagePoints, const Frame& frame = Frame());

		/**
		 * Adds a new camera to the environment
		 * @param camera The camera pose
		 * @return True, if succeeded
		 */
		bool addCamera(const HomogenousMatrix4& camera);

		/**
		 * Adds 3D object points as small boxes.
		 * @param objectPoints The object points to add
		 * @param adjustExpansion True, to adjust the scene expansion factor so that it matches with the given object points
		 * @return True, if succeeded
		 */
		bool addPoints(const Vectors3& objectPoints, const bool adjustExpansion = true);

		/**
		 * Sets the camera pose (the extrinsic camera matrix).
		 * @param pose The pose to set
		 */
		void setPose(const HomogenousMatrix4& pose);

		/**
		 * Updates the transformation of the coordinate system.
		 * @param transformation The transformation of the plane to set
		 */
		void updateCoordinateSystem(const HomogenousMatrix4& transformation);

		/**
		 * Updates the transformation of the plane.
		 * @param transformation The transformation of the plane to set
		 */
		void updatePlane(const HomogenousMatrix4& transformation);

		/**
		 * Updates highlight points.
		 * @param objectPoints The object points which will be high lighted
		 */
		void updateHighlightObjectPoints(const Vectors3& objectPoints);

		/**
		 * Removes all rendering object.
		 */
		void clear();

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function for window resize event.
		 * @see Window::onResize().
		 */
		void onResize(const unsigned int clientWidth, const unsigned int clientHeight) override;

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
		 * Function for keyboard button up events.
		 * @see Window::onKeyUp().
		 */
		void onKeyUp(const int key) override;

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
		 * Allows the modification of the window class before the registration.
		 * @see Window::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASS& windowClass) override;

		/**
		 * Renders the scene.
		 */
		void render();

		/**
		 * Determines a reference size of a 3D point cloud by measuring the median distance between all object points and the median point of all object points.
		 * @param objectPoints The 3D object points for which the reference size will be determined
		 * @param size The number of given object points
		 * @return The reference size
		 */
		Scalar determineMedianDimension(const Vector3* objectPoints, const size_t size) const;

	protected:

		/// Rendering engine.
		Rendering::EngineRef engine_;

		/// Rendering framebuffer.
		Rendering::WindowFramebufferRef framebuffer_;

		/// Old horizontal mouse cursor position.
		unsigned int oldCursorX_;

		/// Old vertical mouse cursor position.
		unsigned int oldCursorY_;

		/// The mesh visibility statement.
		bool meshVisibility_;

		/// The point visibility statement.
		bool pointVisibility_;

		/// The scene expansion value.
		Scalar sceneExpansion_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_RENDER_WINDOW_H
