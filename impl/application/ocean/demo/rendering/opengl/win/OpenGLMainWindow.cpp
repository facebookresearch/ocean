/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/opengl/win/OpenGLMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

#include <gl/glu.h>

using namespace Ocean::Media;
using namespace Ocean::Platform::Win;

OpenGLMainWindow::OpenGLMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	ApplicationWindow(instance, name),
	windowContext_(instance, L"GL Window", true),
	horizontalFieldOfView_(45),
	renderFrameCounter_(0)
{
	renderFrameCounterTimestamp_ = highPerformanceTimer_.seconds();
}

OpenGLMainWindow::~OpenGLMainWindow()
{
	texture_.release();
}

void OpenGLMainWindow::onInitialized()
{
	ApplicationWindow::onInitialized();

	windowContext_.setParent(handle());
	windowContext_.initialize();
	windowContext_.move(0, 0);
	windowContext_.resize(clientWidth(), clientHeight());
	windowContext_.show();
	windowContext_.createOpenGLContext(false, 4u);

	windowFramebuffer_.setContext(windowContext_);
	windowFramebuffer_.initOpenGL();

	texture_.setContext(windowContext_);

	Media::FrameMediumRef medium = Media::Manager::get().newMedium("LiveVideoId:1", Medium::LIVE_VIDEO, true);
	if (medium.isNull())
	{
		medium = Media::Manager::get().newMedium("LiveVideoId:0", Medium::LIVE_VIDEO, true);
	}

	if (medium)
	{
		medium->start();
		texture_.setMedium(medium);
	}
}

void OpenGLMainWindow::render()
{
	windowContext_.makeCurrent();
	ocean_assert(glGetError() == GL_NO_ERROR);

	// the OpenGL fixed function pipeline is not supported in Core profiles starting from 3.0
	ocean_assert(windowContext_.compatibilityProfile() || windowContext_.majorVersion() <= 2u);

	windowFramebuffer_.bind();

	texture_.update();

	glClearColor(0, 0, 0, 0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glMatrixMode(GL_PROJECTION);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	ocean_assert(windowFramebuffer_.height() != 0u);
	const Scalar aspect = float(windowFramebuffer_.width()) / float(windowFramebuffer_.height());
	const Scalar fovX = PinholeCamera::fovX2Y(Numeric::deg2rad(Scalar(horizontalFieldOfView_)), aspect);

	gluPerspective(GLdouble(Numeric::rad2deg(fovX)), GLdouble(aspect), 0.1, 1000.0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	texture_.bind();

	glBegin(GL_TRIANGLE_STRIP);

		glNormal3f(0, 0, 1);
		glTexCoord2f(0, 1);
		glVertex3f(-20, 20, -100);

		glTexCoord2f(0, 0);
		glVertex3f(-20, -20, -100);

		glTexCoord2f(1, 1);
		glVertex3f(20, 20, -100);

		glTexCoord2f(1, 0);
		glVertex3f(20, -20, -100);

	glEnd();

	ocean_assert(glGetError() == GL_NO_ERROR);

	windowFramebuffer_.unbind();

	const double stopTimestamp = highPerformanceTimer_.seconds();
	++renderFrameCounter_;

	const double elapsedTime = stopTimestamp - renderFrameCounterTimestamp_;

	if (elapsedTime > 1)
	{
		float reachedFps = float(renderFrameCounter_ / elapsedTime);

		SetWindowTextA(handle(), (std::string("OpenGL (") + Build::buildString() + std::string(") ") + String::toAString(reachedFps) + std::string("fps")).c_str());

		renderFrameCounterTimestamp_ = stopTimestamp;
		renderFrameCounter_ = 0;
	}
}

void OpenGLMainWindow::onPaint()
{
	render();
}

void OpenGLMainWindow::onIdle()
{
	render();
}

void OpenGLMainWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	windowContext_.resize(clientWidth, clientHeight);
	windowFramebuffer_.resize(clientWidth, clientHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	const Scalar aspect = float(clientWidth) / float(clientHeight);
	const Scalar fovX = PinholeCamera::fovX2Y(Numeric::deg2rad(Scalar(horizontalFieldOfView_)), aspect);

	gluPerspective(GLdouble(Numeric::rad2deg(fovX)), GLdouble(aspect), 0.1, 1000.0);
	ocean_assert(glGetError() == GL_NO_ERROR);
}

void OpenGLMainWindow::modifyWindowClass(WNDCLASS& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = nullptr;
}
