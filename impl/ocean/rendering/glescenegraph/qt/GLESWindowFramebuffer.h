/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_QT_GLES_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_QT_GLES_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#include "ocean/rendering/WindowFramebuffer.h"

#include <QtWidgets/QOpenGLWidget>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace QT
{

/**
 * This class is the base class for all GLESceneGraph window framebuffers for Qt-enabled platforms.
 * @ingroup renderinggles
 */
class GLESWindowFramebuffer :
	virtual public QOpenGLWidget,
	virtual public GLESFramebuffer,
	virtual public WindowFramebuffer
{
	friend class GLESEngineQT;

	public:

		/**
		 * Returns the transformation between display and device.
		 * @see Framebuffer::device_T_display().
		 */
		HomogenousMatrix4 device_T_display() const override;

		/**
		 * Sets the transformation between display and device.
		 * @see Framebuffer::setDevice_T_display().
		 */
		bool setDevice_T_display(const HomogenousMatrix4& device_T_display) override;

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

		/**
		 * Sets the platform dependent framebuffer base id or handle and initializes the framebuffer.
		 * @see WindowFramebuffer::initializeById().
		 */
		bool initializeById(const size_t id, const FramebufferRef& shareFramebuffer) override;

	private:

		/**
		 * Creates a new window framebuffer.
		 */
		GLESWindowFramebuffer();

		/**
		 * Destructs a window framebuffer.
		 */
		~GLESWindowFramebuffer() override;

		/**
		 * Initialize the OpenGL ES dispaly and context.
		 * @return True, if succeeded
		 */
		bool initializeContext() override;

		/**
		 * This virtual function is called whenever the widget needs to be painted.
		 * @see QOpenGLWidget::paintGL().
		 */
		void paintGL() override;

		/**
		 * This virtual function is called whenever the widget has been resized.
		 * @see QOpenGLWidget::resizeGL().
		 */
		void resizeGL(int width, int height) override;

		/**
		 * This virtual function is called once before the first call to paintGL() or resizeGL().
		 * @see QOpenGLWidget::initializeGL().
		 */
		void initializeGL() override;

	protected:

		/// The transformation between display and device.
		HomogenousMatrix4 device_T_display_ = HomogenousMatrix4(true);

		/// The previous devicePixelRatio, e.g., is > 1 on Apple's retina desplays.
		int previousDevicePixelRatio_ = 0;
};

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_QT_GLES_WINDOW_FRAMEBUFFER_H
