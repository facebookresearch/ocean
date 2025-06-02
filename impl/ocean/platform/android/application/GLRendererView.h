/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_RENDERER_VIEW_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_RENDERER_VIEW_H

#include "ocean/platform/android/application/Application.h"
#include "ocean/platform/android/application/GLView.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

/**
 * This class implements an OpenGLES-based view using the OpenGELESceneGraph renderer for Android platform applications.
 * The main view is implemented as singleton object.
 * @ingroup platformandroidapplication
 */
class OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT GLRendererView : public GLView
{
	public:

		/**
		 * Initializes the view.
		 * @return True, if succeeded
		 */
		bool initialize() override;

		/**
		 * Releases the view.
		 * @return True, if succeeded
		 */
		bool release() override;

		/**
		 * Sets the horizontal field of view for this view.
		 * @param fovx Field of view to set in radian
		 * @return True, if succeeded
		 */
		virtual bool setFovX(const Scalar fovx);

		/**
		 * Sets the background color of this view.
		 * @param color Background color to set
		 * @return True, if succeeded
		 */
		virtual bool setBackgroundColor(const RGBAColor& color);

		/**
		 * View resize event function.
		 * @param width New view width, with range [1, infinity)
		 * @param height New view height, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool resize(const int width, const int height) override;

		/**
		 * Renders the next frame.
		 * @return True, if succeeded
		 */
		bool render() override;

		/**
		 * Sets whether the user can change the view's position and rotation by touch events.
		 * @param enabled True, to allow the interaction; False, to disable the interaction
		 */
		virtual void setViewInteractionEnabled(const bool enabled);

		/**
		 * Touch down event function.
		 * @param x Horizontal touch position
		 * @param y Vertical touch position
		 */
		void onTouchDown(const float x, const float y) override;

		/**
		 * Touch move event function.
		 * @param x Horizontal touch position
		 * @param y Vertical touch position
		 */
		void onTouchMove(const float x, const float y) override;

		/**
		 * Touch move event function.
		 * @param x Horizontal touch position
		 * @param y Vertical touch position
		 */
		void onTouchUp(const float x, const float y) override;

		/**
		 * Creates an instance of this object.
		 * @return The new instance
		 */
		static inline GLView* createInstance();

	protected:

		/**
		 * Creates a new view object.
		 */
		GLRendererView() = default;

		/**
		 * Destructs a view object.
		 */
		~GLRendererView() override;

	protected:

		/// Rendering engine object.
		Rendering::EngineRef engine_;

		/// Rendering framebuffer object.
		Rendering::FramebufferRef framebuffer_;

		/// Initial viewport width.
		unsigned int initialViewportWidth_ = (unsigned int)(-1);

		/// Initial viewport height.
		unsigned int initialViewportHeight_ = (unsigned int)(-1);

		/// Previous horizontal touch position.
		float previousTouchX_ = -1.0f;

		/// Previous vertical touch position.
		float previousTouchY_ = -1.0f;

		/// Rendering start timestamp.
		Timestamp renderingStartTimestamp_;

		/// Rendering iterations.
		unsigned int renderingIterations_ = 0u;

		/// State determining that the view's field of view has to be adjusted to the background's field of view.
		bool adjustFovXToBackground_ = false;

		/// True, to allow the user to change the view's position and rotation by touch events; False, to disable the user interaction.
		bool viewInteractionEnabled_ = true;

		/// View lock object.
		mutable Lock lock_;
};

inline Ocean::Platform::Android::Application::GLView* GLRendererView::createInstance()
{
	return new GLRendererView();
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_RENDERER_VIEW_H
