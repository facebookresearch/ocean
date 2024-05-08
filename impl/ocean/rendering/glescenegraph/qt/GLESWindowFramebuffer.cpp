/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/qt/GLESWindowFramebuffer.h"

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace QT
{

GLESWindowFramebuffer::GLESWindowFramebuffer() :
	QOpenGLWidget(),
	GLESFramebuffer(),
	WindowFramebuffer()
{
	QSurfaceFormat qSurfaceFormat;
	qSurfaceFormat.setVersion(3, 3);
	qSurfaceFormat.setSamples(8);
	qSurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);

	setFormat(qSurfaceFormat);

	QSurfaceFormat::setDefaultFormat(qSurfaceFormat);
}

GLESWindowFramebuffer::~GLESWindowFramebuffer()
{
	release();
}

HomogenousMatrix4 GLESWindowFramebuffer::device_T_display() const
{
	return device_T_display_;
}

bool GLESWindowFramebuffer::setDevice_T_display(const HomogenousMatrix4& device_T_display)
{
	if (!device_T_display.isValid())
	{
		ocean_assert(false && "Invalid transformation!");
		return false;
	}

	device_T_display_ = device_T_display;

	return true;
}

void GLESWindowFramebuffer::render()
{
	// telling Qt that we need to render the Widget (and framebuffer)
	// Qt will eventually call paintGL().

	update();
}

bool GLESWindowFramebuffer::initializeById(const size_t id, const FramebufferRef& shareFramebuffer)
{
	ocean_assert(shareFramebuffer.isNull());

	QWidget* parent = (QWidget*)(id);
	setParent(parent);

	return true;
}

bool GLESWindowFramebuffer::initializeContext()
{
	// nothing to do here, QT already initialized the context

	return true;
}

void GLESWindowFramebuffer::paintGL()
{
	const int currentDevicePixelRatio_ = devicePixelRatio();
	ocean_assert(currentDevicePixelRatio_ != 0);

	if (currentDevicePixelRatio_ != previousDevicePixelRatio_)
	{
		resizeGL(width(), height());
	}

	GLESFramebuffer::render();
}

void GLESWindowFramebuffer::resizeGL(int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		return;
	}

	previousDevicePixelRatio_ = devicePixelRatio();
	ocean_assert(previousDevicePixelRatio_ != 0);

	width *= previousDevicePixelRatio_;
	height *= previousDevicePixelRatio_;

	setViewport(0u, 0u, (unsigned int)(width), (unsigned int)(height));

	if (framebufferView)
	{
		const Scalar aspectRatio = Scalar(width) / Scalar(height);
		framebufferView->setAspectRatio(aspectRatio);
	}
}

void GLESWindowFramebuffer::initializeGL()
{
	initialize();
}

}

}

}

}
