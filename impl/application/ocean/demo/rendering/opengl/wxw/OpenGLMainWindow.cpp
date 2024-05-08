/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/opengl/wxw/OpenGLMainWindow.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Medium.h"

#include "ocean/platform/wxwidgets/System.h"

#ifdef _WINDOWS
	#include <gl/glu.h>
#else
	#include <OpenGL/glu.h>
#endif

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(OpenGLMainWindow, wxFrame)
	EVT_SIZE(OpenGLMainWindow::onResize)
	EVT_IDLE(OpenGLMainWindow::onIdle)
	EVT_MENU(OpenGLMainWindow::ID_Quit,  OpenGLMainWindow::onQuit)
	EVT_MENU(OpenGLMainWindow::ID_About, OpenGLMainWindow::onAbout)
	EVT_PAINT(OpenGLMainWindow::onPaint)
	EVT_ERASE_BACKGROUND(OpenGLMainWindow::onEraseBackground)
END_EVENT_TABLE()

OpenGLMainWindow::OpenGLMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size),
	windowContext_(nullptr)
{
	windowContext_ = new Platform::GL::WxWidgets::WindowContext(this, wxID_ANY, wxPoint(0, 0), size);

	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, L"&File");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#ifdef __APPLE__
	startThread();
#endif
}

void OpenGLMainWindow::initalize()
{
	ocean_assert(windowContext_ != nullptr);

	windowContext_->createOpenGLContext(false, 4u);

	windowFramebuffer_.setContext(*windowContext_);
	windowFramebuffer_.initOpenGL();

	windowTexture_.setContext(*windowContext_);

	const wxSize clientSize = GetClientSize();
	if (clientSize.x >= 0 && clientSize.y >= 0)
	{
		windowContext_->SetSize(clientSize);
		windowFramebuffer_.resize((unsigned int)clientSize.x, (unsigned int)clientSize.y);
	}

	Media::FrameMediumRef medium = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	if (medium.isNull())
	{
		medium = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (medium.isNull())
	{
		const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		medium = Media::Manager::get().newMedium(frameworkPath + "/data/testsuite/media/images/texture512x512.png", Media::Medium::IMAGE);
	}

	if (medium)
	{
		medium->start();
		windowTexture_.setMedium(medium);
	}
}

void OpenGLMainWindow::render()
{
	ocean_assert(windowContext_);

	if (!windowContext_->isValid())
		return;

	windowContext_->makeCurrent();
	ocean_assert(glGetError() == GL_NO_ERROR);

	// the OpenGL fixed function pipeline is not supported in Core profiles starting from 3.0
	ocean_assert(windowContext_->compatibilityProfile() || windowContext_->majorVersion() <= 2u);

	windowFramebuffer_.bind();

	windowTexture_.update();

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
	const Scalar fovX = PinholeCamera::fovX2Y(Numeric::deg2rad(45), aspect);

	gluPerspective(GLdouble(Numeric::rad2deg(fovX)), GLdouble(aspect), 0.1, 1000.0);
	ocean_assert(glGetError() == GL_NO_ERROR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ocean_assert(glGetError() == GL_NO_ERROR);

	windowTexture_.bind();

	glBegin(GL_TRIANGLE_STRIP);

	if (windowTexture_.frameType().pixelOrigin() == FrameType::ORIGIN_LOWER_LEFT)
	{
		glNormal3f(0, 0, 1);
		glTexCoord2f(0, 1);
		glVertex3f(-20, 20, -100);

		glTexCoord2f(0, 0);
		glVertex3f(-20, -20, -100);

		glTexCoord2f(1, 1);
		glVertex3f(20, 20, -100);

		glTexCoord2f(1, 0);
		glVertex3f(20, -20, -100);
	}
	else
	{
		glNormal3f(0, 0, 1);
		glTexCoord2f(0, 0);
		glVertex3f(-20, 20, -100);

		glTexCoord2f(0, 1);
		glVertex3f(-20, -20, -100);

		glTexCoord2f(1, 0);
		glVertex3f(20, 20, -100);

		glTexCoord2f(1, 1);
		glVertex3f(20, -20, -100);
	}

	glEnd();

	ocean_assert(glGetError() == GL_NO_ERROR);

	windowFramebuffer_.unbind();
}

#ifdef __APPLE__

void OpenGLMainWindow::threadRun()
{
	while (!shouldThreadStop())
	{
		Refresh();
		sleep(1);
	}
}

#endif // __APPLE__

void OpenGLMainWindow::onResize(wxSizeEvent& /*event*/)
{
	ocean_assert(windowContext_);

	const wxSize clientSize = GetClientSize();

	if (clientSize.x >= 0 && clientSize.y >= 0)
	{
		windowContext_->SetSize(clientSize);
		windowFramebuffer_.resize((unsigned int)clientSize.x, (unsigned int)clientSize.y);
	}
}

void OpenGLMainWindow::onPaint(wxPaintEvent& /*event*/)
{
	if (!IsShown())
	{
		return;
	}

	wxPaintDC(this);

	render();
}

void OpenGLMainWindow::onEraseBackground(wxEraseEvent& /*event*/)
{
	// nothing to do here
}

void OpenGLMainWindow::onIdle(wxIdleEvent& /*event*/)
{
	Refresh();
}

void OpenGLMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void OpenGLMainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is a wxWidgets OpenGL sample program", L"About Rendering OpenGL", wxOK | wxICON_INFORMATION, this);
}
