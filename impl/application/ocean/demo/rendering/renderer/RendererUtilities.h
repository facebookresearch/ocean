/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_RENDERER_UTILITIES_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_RENDERER_UTILITIES_H

#include "application/ocean/demo/rendering/renderer/ApplicationDemoRenderingRenderer.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/View.h"

using namespace Ocean;

/**
 * This class implements utility functions for a renderer.
 * @ingroup applicationdemorenderingrenderer
 */
class RendererUtilities
{
	public:

		/**
		 * Translates the view by moving the mouse.
		 * @param currentX Current horizontal position of the mouse
		 * @param currentY Current vertical position of the mouse
		 * @param previousX Previous horizontal position of the mouse
		 * @param previousY Previous vertical position of the mouse
		 * @param view The view to translate
		 * @param interactionSpeed The interaction speed, with range (0, infinity)
		 */
		static void moveMouseTranslateView(const int currentX, const int currentY, const int previousX, const int previousY, Rendering::View& view, const Scalar interactionSpeed = 1);

		/**
		 * Rotates the view by moving the mouse.
		 * @param currentX Current horizontal position of the mouse
		 * @param currentY Current vertical position of the mouse
		 * @param previousX Previous horizontal position of the mouse
		 * @param previousY Previous vertical position of the mouse
		 * @param view The view to translate
		 * @param interactionSpeed The interaction speed, with range (0, infinity)
		 */
		static void moveMouseRotateView(const int currentX, const int currentY, const int previousX, const int previousY, Rendering::View& view, const Scalar interactionSpeed = 1);

		/**
		 * Creates a new scene to render.
		 * @param engine The rendering engine for which the new scene will be created
		 * @return The scene
		 */
		static Rendering::SceneRef createScene(Rendering::Engine& engine);
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_RENDERER_UTILITIES_H
