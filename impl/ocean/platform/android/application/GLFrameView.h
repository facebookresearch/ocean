/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_FRAME_VIEW_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_FRAME_VIEW_H

#include "ocean/platform/android/application/Application.h"
#include "ocean/platform/android/application/GLView.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/UndistortedBackground.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

/**
 * This class implements the main view of an OpenGLES frame for Android platform applications.
 * The main view is implemented as singleton object.
 * @ingroup platformandroidapplication
 */
class OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT GLFrameView : public GLView
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
		 * Sets the background medium of this view by the medium's url and several further parameters.
		 * @param url URL of the media object to be used as background medium
		 * @param type Hint defining the media object in more detail, possible values are "LIVE_VIDEO", "IMAGE", "MOVIE", "IMAGE_SEQUENCE" or "FRAME_STREAM", if no hint is given the first possible media object will be created
		 * @param preferredWidth Preferred width of the medium in pixel, use 0 to use the default width
		 * @param preferredHeight Preferred height of the medium in pixel, use 0 to use the default height
		 * @param adjustFov True, to adjust the view's field of view to the field of the background automatically
		 * @return True, if succeeded
		 */
		virtual bool setBackgroundMedium(const std::string& url, const std::string& type, const int preferredWidth, const int preferredHeight, const bool adjustFov);

		/**
		 * Sets the background medium of this view.
		 * @param frameMedium Background media object to set
		 * @param adjustFov True, to adjust the view's field of view to the field of view of the medium automatically
		 * @return True, if succeeded
		 */
		virtual bool setBackgroundMedium(const Media::FrameMediumRef& frameMedium, const bool adjustFov = false);

		/**
		 * Returns the background medium.
		 * @return The view's background medium, if any
		 */
		virtual Media::FrameMediumRef backgroundMedium() const;

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
		 * Converts the given screen positions into frame positions.
		 * @param xScreen Horizontal screen position, with range [0, infinity)
		 * @param yScreen Vertical screen position, with range [0, infinity)
		 * @param xFrame Resulting horizontal frame position, with range [0, infinity)
		 * @param yFrame Resulting vertical frame position, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool screen2frame(const Scalar xScreen, const Scalar yScreen, Scalar& xFrame, Scalar& yFrame) override;

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
		GLFrameView() = default;

		/**
		 * Destructs a view object.
		 */
		~GLFrameView() override;

	protected:

		/// Rendering engine object.
		Rendering::EngineRef engine_;

		/// Rendering framebuffer object.
		Rendering::FramebufferRef framebuffer_;

		/// Rendering undistorted background object.
		Rendering::UndistortedBackgroundRef background_;

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
		bool adjustFovXToBackground_;

		/// The frame medium of the background which is stored as long as the view hasn't been initialized.
		Media::FrameMediumRef intermediateBackgroundFrameMedium_;

		/// True, to adjust the field of view of the view automatically so that the background medium is entirely covered.
		bool intermediateBackgroundAdjustFov_ = false;

		/// View lock object.
		mutable Lock lock_;
};

inline Ocean::Platform::Android::Application::GLView* GLFrameView::createInstance()
{
	return new GLFrameView();
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_FRAME_VIEW_H
