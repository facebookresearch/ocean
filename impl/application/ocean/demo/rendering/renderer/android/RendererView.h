/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/platform/android/application/GLRendererView.h"

#include "ocean/rendering/Scene.h"

namespace Ocean
{

/**
 * This class implements the main view of the Renderer demo application for Android.
 * The application renders a simple 3D scene with semi-transparent boxes.
 * @ingroup applicationdemorenderingrenderer
 */
class RendererView : public Platform::Android::Application::GLRendererView
{
	friend class Singleton<RendererView>;

	public:

		/**
		 * Initializes the view.
		 * @see GLView::initialize().
		 */
		bool initialize() override;

		/**
		 * Releases the view.
		 * @see GLView::release().
		 */
		bool release() override;

		/**
		 * Creates an instance of this view.
		 * @return The new instance
		 */
		static inline Platform::Android::Application::GLView* createInstance();

	protected:

		/**
		 * Creates a new renderer view.
		 */
		RendererView() = default;

		/**
		 * Destructs the renderer view.
		 */
		~RendererView() override;

	protected:

		/// The rendering scene.
		Rendering::SceneRef scene_;

		/// True, if the instance function is registered.
		static const bool instanceRegistered_;
};

inline Platform::Android::Application::GLView* RendererView::createInstance()
{
	return new RendererView();
}

}
