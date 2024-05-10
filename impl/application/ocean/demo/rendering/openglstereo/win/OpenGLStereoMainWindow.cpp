/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereoMainWindow.h"

#include "ocean/base/String.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Euler.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/Random.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Screen.h"

#include <gl\gl.h>
#include <gl\glu.h>

using namespace Ocean::Media;

OpenGLStereoMainWindow::OpenGLStereoMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	ApplicationWindow(instance, name)
{
	// nothing to do here
}

OpenGLStereoMainWindow::~OpenGLStereoMainWindow()
{
	leftBackground_.release();
	rightBackground_.release();

	releaseOpenGL();
}

bool OpenGLStereoMainWindow::initialize(const HICON icon, const std::string& windowClass)
{
	if (!ApplicationWindow::initialize(icon, windowClass))
	{
		return false;
	}

	if (dc() == nullptr)
	{
		return false;
	}

	PIXELFORMATDESCRIPTOR pixelformatDescriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO, PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	const GLuint pixelformat = ChoosePixelFormat(dc(), &pixelformatDescriptor);

	if (pixelformat == 0)
	{
		ocean_assert(false && "Invalid pixel format.");
		return false;
	}

	if (SetPixelFormat(dc(), pixelformat, &pixelformatDescriptor) == FALSE)
	{
		ocean_assert(false && "Could not select pixel format");
		return false;
	}

	ocean_assert(windowRC_ == nullptr);
	windowRC_ = wglCreateContext(dc());
	if (windowRC_ == nullptr)
	{
		ocean_assert(false && "Invalid OpenGL render context");
		return false;
	}

	if (wglMakeCurrent(dc(), windowRC_) == FALSE)
	{
		ocean_assert(false && "Could not activate the OpenGL render context.");
		return false;
	}

	initializeOpenGL();

	const Media::FrameMediumRef leftFrameMedium = Media::Manager::get().newMedium("LiveVideoId:0", Medium::LIVE_VIDEO, true);
	if (leftFrameMedium)
	{
		leftFrameMedium->start();
		leftBackground_.setMedium(leftFrameMedium);
		leftFrameMedium->stop();
	}

	const Media::FrameMediumRef rightFrameMedium = Media::Manager::get().newMedium("LiveVideoId:1", Medium::LIVE_VIDEO, true);
	if (rightFrameMedium)
	{
		rightFrameMedium->start();
		rightBackground_.setMedium(rightFrameMedium);
		rightFrameMedium->stop();
	}

	if (leftFrameMedium)
	{
		leftFrameMedium->start();
	}
	if (rightFrameMedium)
	{
		rightFrameMedium->start();
	}

	return true;
}

void OpenGLStereoMainWindow::releaseOpenGL()
{
	if (windowRC_ == nullptr)
	{
		return;
	}

	if (wglMakeCurrent(nullptr, nullptr) == false)
	{
		ocean_assert(false && "It's not allowed to release OpenGL at this moment.");
	}

	if (!wglDeleteContext(windowRC_))
	{
		ocean_assert(false && "OpenGL could not be released");
	}

	windowRC_ = nullptr;
}

void OpenGLStereoMainWindow::initializeOpenGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	ocean_assert(glGetError() == GL_NO_ERROR);
}

void OpenGLStereoMainWindow::render()
{
	leftBackground_.update();
	rightBackground_.update();

	// left eye
	glDrawBuffer(GL_BACK_LEFT);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (swapBackground_)
	{
		rightBackground_.draw();
	}
	else
	{
		leftBackground_.draw();
	}


	// right eye
	glDrawBuffer(GL_BACK_RIGHT);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	if (swapBackground_)
	{
		leftBackground_.draw();
	}
	else
	{
		rightBackground_.draw();
	}


	SwapBuffers(dc());
}

void OpenGLStereoMainWindow::onPaint()
{
	render();
}

void OpenGLStereoMainWindow::onIdle()
{
	render();
}

void OpenGLStereoMainWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	if (windowRC_ == nullptr || clientWidth == 0 || clientHeight == 0)
	{
		return;
	}

	glViewport(0, 0, clientWidth, clientHeight);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	const Scalar aspect = float(clientWidth) / float(clientHeight);
	const Scalar fovX = PinholeCamera::fovX2Y(Numeric::deg2rad(Scalar(horizontalFieldOfView_)), aspect);

	gluPerspective(GLdouble(Numeric::rad2deg(fovX)), GLdouble(aspect), 0.1, 1000.0);
	ocean_assert(glGetError() == GL_NO_ERROR);
}

void OpenGLStereoMainWindow::onKeyDown(const int key)
{
	if (key == 'Q')
	{
		onToggleFullscreen();
	}
	else if (key == '1')
	{
		leftBackground_.configurateMedium(0);
	}
	else if (key == '2')
	{
		leftBackground_.configurateMedium(1);
	}
	else if (key == '3')
	{
		rightBackground_.configurateMedium(0);
	}
	else if (key == '4')
	{
		rightBackground_.configurateMedium(1);
	}
	else if (key == '9')
	{
		if (horizontalFieldOfView_ > 5)
		{
			horizontalFieldOfView_ -= 0.5;
			onResize(clientWidth(), clientHeight());
		}
	}
	else if (key == '0')
	{
		if (horizontalFieldOfView_ < 80)
		{
			horizontalFieldOfView_ += 0.5;
			onResize(clientWidth(), clientHeight());
		}
	}
	else
	{
		swapBackground_ = !swapBackground_;
	}
}

void OpenGLStereoMainWindow::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void OpenGLStereoMainWindow::onKeyUp(const int /*key*/)
{
	// nothing to do here
}

void OpenGLStereoMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void OpenGLStereoMainWindow::onToggleFullscreen()
{
	if (isFullscreen_)
	{
		SetWindowLongPtrA(handle(), GWL_STYLE, mainWindowNonFullScreenStyle_);
		mainWindowNonFullScreenStyle_ = 0;

		SetWindowPos(handle(), HWND_NOTOPMOST, mainWindowLastLeft_, mainMindowLastTop_, mainWindowLastWidth_, mainWindowLastHeight_, 0);
		isFullscreen_ = false;
	}
	else
	{
		isFullscreen_ = true;

		// store current window style
		ocean_assert(mainWindowNonFullScreenStyle_ == 0);

		// remove main window border
		mainWindowNonFullScreenStyle_ = int(SetWindowLongPtr(handle(), GWL_STYLE, WS_VISIBLE));

		RECT rect;
		GetWindowRect(handle(), &rect);

		mainWindowLastLeft_ = rect.left;
		mainMindowLastTop_ = rect.top;
		mainWindowLastWidth_ = rect.right - rect.left;
		mainWindowLastHeight_ = rect.bottom - rect.top;

		unsigned int screenLeft, screenTop, screenWidth, screenHeight;
		if (Platform::Win::Screen::screen(handle(), screenLeft, screenTop, screenWidth, screenHeight) == false)
		{
			ocean_assert(false && "Invalid screen");
		}

		SetWindowPos(handle(), HWND_TOPMOST, int(screenLeft), int(screenTop), int(screenWidth), int(screenHeight), SWP_SHOWWINDOW);
	}
}

void OpenGLStereoMainWindow::modifyWindowClass(WNDCLASS& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = nullptr;
}
