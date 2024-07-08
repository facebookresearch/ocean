/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/renderer/wxw/RendererWxWidgetsMainWindow.h"

#include "application/ocean/demo/rendering/renderer/RendererUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/platform/Keyboard.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Objects.h"

#ifdef __APPLE__
	#include <wx/osx/private.h>
#endif

using namespace Ocean;

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(RendererWxWidgetsMainWindow, wxFrame)
	EVT_SIZE(RendererWxWidgetsMainWindow::onResize)
	EVT_PAINT(RendererWxWidgetsMainWindow::onPaint)
	EVT_LEFT_DOWN(RendererWxWidgetsMainWindow::onMouseDown)
	EVT_RIGHT_DOWN(RendererWxWidgetsMainWindow::onMouseDown)
	EVT_MOTION(RendererWxWidgetsMainWindow::onMouseMove)
	EVT_LEFT_UP(RendererWxWidgetsMainWindow::onMouseUp)
	EVT_RIGHT_UP(RendererWxWidgetsMainWindow::onMouseUp)
	EVT_MOUSEWHEEL(RendererWxWidgetsMainWindow::onMouseWheel)
	EVT_MENU(RendererWxWidgetsMainWindow::ID_Quit,  RendererWxWidgetsMainWindow::onQuit)
END_EVENT_TABLE()

RendererWxWidgetsMainWindow::RendererWxWidgetsMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size)
{
	SetBackgroundColour(wxColour(0x808080));
}

void RendererWxWidgetsMainWindow::initalize()
{
	try
	{
		// take any rendering engine applying an OpenGL API
		engine_ = Rendering::Manager::get().engine("", Rendering::Engine::API_OPENGL);

		if (engine_)
		{
			framebuffer_ = engine_->createFramebuffer();
			ocean_assert(framebuffer_);

#if defined(_WINDOWS)
			framebuffer_->initializeById(size_t(GetHWND()));
#elif defined(__APPLE__)
			ocean_assert(GetPeer() && GetPeer()->GetWXWidget());
			framebuffer->initializeById(size_t(GetPeer()->GetWXWidget()));
#endif

			Rendering::ViewRef view = engine_->factory().createPerspectiveView();

			const wxSize clientSize = GetClientSize();

			const unsigned int clientWidth = (unsigned int)max(0, clientSize.x);
			const unsigned int clientHeight = (unsigned int)max(0, clientSize.y);

			framebuffer_->setView(view);
			framebuffer_->setViewport(0, 0, clientWidth, clientHeight);

			if (clientHeight != 0u)
			{
				view->setAspectRatio(float(clientWidth) / float(clientHeight));
			}

			framebuffer_->view()->setBackgroundColor(RGBAColor(0, 0, 0));

			framebuffer_->addScene(RendererUtilities::createScene(*engine_));

			// set a user defined view position
			framebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));

			// or set the view position according to the scene
			framebuffer_->view()->fitCamera();
		}
	}
	catch(...)
	{
		ocean_assert(false && "Un-handled exception!");
	}
}

void RendererWxWidgetsMainWindow::render()
{
	const ScopedLock scopedLock(lock_);

	if (framebuffer_)
	{
		ocean_assert(engine_);
		engine_->update(Timestamp(true));

		framebuffer_->makeCurrent();
		framebuffer_->render();
	}
}

void RendererWxWidgetsMainWindow::onResize(wxSizeEvent& /*event*/)
{
	const ScopedLock scopedLock(lock_);

	const wxSize clientSize = GetClientSize();

	const unsigned int clientWidth = (unsigned int)max(0, clientSize.x);
	const unsigned int clientHeight = (unsigned int)max(0, clientSize.y);

	if (framebuffer_)
	{
		framebuffer_->setViewport(0, 0, clientWidth, clientHeight);

		if (clientHeight != 0u && framebuffer_->view())
		{
			framebuffer_->view()->setAspectRatio(float(clientWidth) / float(clientHeight));
		}
	}
}

void RendererWxWidgetsMainWindow::onPaint(wxPaintEvent& /*event*/)
{
	if (!IsShown())
	{
		return;
	}

	wxPaintDC(this);

	render();
}

void RendererWxWidgetsMainWindow::onMouseDown(wxMouseEvent& event)
{
	oldCursorX_ = event.GetPosition().x;
	oldCursorY_ = event.GetPosition().y;
}

void RendererWxWidgetsMainWindow::onMouseMove(wxMouseEvent& event)
{
	if (!event.LeftIsDown() && !event.RightIsDown())
	{
		return;
	}

	if (framebuffer_.isNull() || framebuffer_->view().isNull())
	{
		return;
	}

	ocean_assert(oldCursorX_ != NumericT<int>::minValue());
	ocean_assert(oldCursorY_ != NumericT<int>::minValue());

	Scalar factor = Scalar(0.5);
	if (Platform::Keyboard::oneKeyDown(Platform::Keyboard::KEY_CONTROL))
	{
		factor *= Scalar(10);
	}
	else if (Platform::Keyboard::oneKeyDown(Platform::Keyboard::KEY_MENU))
	{
		factor *= Scalar(0.1);
	}

	const int x = event.GetPosition().x;
	const int y = event.GetPosition().y;

	if (event.LeftIsDown())
	{
		RendererUtilities::moveMouseTranslateView(x, y, oldCursorX_, oldCursorY_, *framebuffer_->view(), factor);

		oldCursorX_ = x;
		oldCursorY_ = y;

		Refresh(false);
	}
	else if (event.RightIsDown())
	{
		RendererUtilities::moveMouseRotateView(x, y, oldCursorX_, oldCursorY_, *framebuffer_->view(), factor);

		oldCursorX_ = x;
		oldCursorY_ = y;

		Refresh(false);
	}
}

void RendererWxWidgetsMainWindow::onMouseUp(wxMouseEvent& /*event*/)
{
	oldCursorX_ = NumericT<int>::minValue();
	oldCursorY_ = NumericT<int>::minValue();
}

void RendererWxWidgetsMainWindow::onMouseWheel(wxMouseEvent& event)
{
	if (framebuffer_)
	{
		HomogenousMatrix4 transformation = framebuffer_->view()->transformation();

		const Vector3 direction(transformation.rotation() * Vector3(0, 0, -1));
		const Vector3 position(transformation.translation());

		transformation.setTranslation(position + direction * (Scalar(event.GetWheelRotation()) * Scalar(0.01)));

		framebuffer_->view()->setTransformation(transformation);

		Refresh(false);
	}
}

void RendererWxWidgetsMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}
