/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H
#define META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Demo
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

namespace TriangulationSimulatorQt
{

/**
 * This class implements the 3D scene rendering and simulation logic for the triangulation simulator.
 */
class Scene
{
	public:

		/**
		 * Creates a new scene object.
		 */
		Scene();

		/**
		 * Destructs the scene object.
		 */
		~Scene();

		/**
		 * Initializes the scene with a framebuffer.
		 * @param engine The rendering engine to use
		 * @param framebuffer The framebuffer to render into
		 * @return True if initialization succeeded
		 */
		bool initialize(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer);

		/**
		 * Handles a mouse press event.
		 * @param button The mouse button that was pressed
		 * @param position The position of the mouse in screen coordinates
		 */
		void handleMousePress(int button, const Vector2& position);

		/**
		 * Handles a mouse move event.
		 * @param position The current position of the mouse in screen coordinates
		 * @param buttons The mouse buttons that are currently pressed (bitmask)
		 */
		void handleMouseMove(const Vector2& position, int buttons);

		/**
		 * Handles a mouse wheel event.
		 * @param delta The scroll delta
		 */
		void handleMouseWheel(int delta);

		/**
		 * Resets the camera to the default view.
		 */
		void resetCamera();

	protected:

		/**
		 * Updates the camera transform based on orbit parameters.
		 */
		void updateCameraTransform();

	protected:

		/// The rendering engine
		Rendering::EngineRef engine_;

		/// The framebuffer
		Rendering::FramebufferRef framebuffer_;

		/// The scene
		Rendering::SceneRef scene_;

		/// The perspective view
		Rendering::PerspectiveViewRef perspectiveView_;

		/// Transform for the coordinate axes
		Rendering::TransformRef axesTransform_;

		/// Last mouse position
		Vector2 lastMousePosition_;

		/// Orbit camera rotation
		Quaternion orbitRotation_;

		/// Orbit camera distance
		Scalar orbitDistance_ = Scalar(2.0);

		/// Orbit camera center
		Vector3 orbitCenter_;
};

}

}

}

}

}

}

#endif // META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H
