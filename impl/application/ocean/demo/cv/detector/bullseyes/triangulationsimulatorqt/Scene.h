/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H
#define META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"

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
};

}

}

}

}

}

}

#endif // META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_SCENE_H
