/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/renderer/win/RendererWinMainWindow.h"

#include "application/ocean/demo/rendering/renderer/RendererUtilities.h"

#include "ocean/base/Thread.h"

#include "ocean/math/Random.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Objects.h"

RendererWinMainWindow::RendererWinMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	ApplicationWindow(instance, name)
{
	// nothing to do here
}

RendererWinMainWindow::~RendererWinMainWindow()
{
	// nothing to do here
}

bool RendererWinMainWindow::initialize(const HICON icon, const std::string& windowClass)
{
	if (!ApplicationWindow::initialize(icon, windowClass))
	{
		return false;
	}

	try
	{
		// take any rendering engine applying an OpenGL API
		engine_ = Rendering::Manager::get().engine("", Rendering::Engine::API_OPENGL);

		if (engine_)
		{
			framebuffer_ = engine_->createFramebuffer();
			ocean_assert(framebuffer_);

			framebuffer_->initializeById(size_t(handle()));

			Rendering::ViewRef view = engine_->factory().createPerspectiveView();

			framebuffer_->setView(view);
			framebuffer_->setViewport(0, 0, clientWidth(), clientHeight());

			if (clientHeight() != 0)
			{
				view->setAspectRatio(float(clientWidth()) / float(clientHeight()));
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
		// the user should be informed about the error
	}

	return true;
}

void RendererWinMainWindow::render()
{
	if (framebuffer_)
	{
		ocean_assert(engine_);
		engine_->update(Timestamp(true));

		framebuffer_->makeCurrent();
		framebuffer_->render();
	}
}

void RendererWinMainWindow::onPaint()
{
	render();
}

void RendererWinMainWindow::onIdle()
{
	render();
	Thread::sleep(1);
}

void RendererWinMainWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	if (framebuffer_)
	{
		framebuffer_->setViewport(0, 0, clientWidth, clientHeight);

		if (clientHeight != 0 && framebuffer_->view())
		{
			framebuffer_->view()->setAspectRatio(float(clientWidth) / float(clientHeight));
		}
	}
}

void RendererWinMainWindow::onKeyDown(const int key)
{
	if (framebuffer_)
	{
		if (key == 'A')
		{
			framebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(10, 0, 40)));
		}
		else if (key == 'B')
		{
			framebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(10, 10, 40)));
		}
	}
}

void RendererWinMainWindow::onMouseDown(const MouseButton /*button*/, const int x, const int y)
{
	oldCursorX_ = x;
	oldCursorY_ = y;
}

void RendererWinMainWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	if ((buttons & BUTTON_LEFT) == 0 && (buttons & BUTTON_RIGHT) == 0)
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
	if (GetKeyState(VK_LCONTROL) & 0x800 || GetKeyState(VK_RCONTROL) & 0xF800)
	{
		factor *= Scalar(10);
	}
	else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
	{
		factor *= Scalar(0.1);
	}

	if (buttons & BUTTON_LEFT)
	{
		RendererUtilities::moveMouseTranslateView(x, y, oldCursorX_, oldCursorY_, *framebuffer_->view(), factor);

		oldCursorX_ = x;
		oldCursorY_ = y;

		repaint(true);
	}
	else if (buttons & BUTTON_RIGHT)
	{
		RendererUtilities::moveMouseRotateView(x, y, oldCursorX_, oldCursorY_, *framebuffer_->view(), factor);

		oldCursorX_ = x;
		oldCursorY_ = y;

		repaint(true);
	}
}

void RendererWinMainWindow::onKeyUp(const int key)
{
	if (framebuffer_)
	{
		if (key == 'L')
		{
			framebuffer_->view()->setUseHeadlight(!framebuffer_->view()->useHeadlight());
		}
		else if (key == 'F')
		{
			Rendering::Framebuffer::FaceMode mode = framebuffer_->faceMode();
			mode = Rendering::Framebuffer::FaceMode(mode + 1);

			if (mode > Rendering::PrimitiveAttribute::MODE_POINT)
			{
				mode = Rendering::PrimitiveAttribute::MODE_DEFAULT;
			}

			framebuffer_->setFaceMode(mode);
		}
	}
}

void RendererWinMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	oldCursorX_ = NumericT<int>::minValue();
	oldCursorY_ = NumericT<int>::minValue();
}

void RendererWinMainWindow::onMouseWheel(const MouseButton /*buttons*/, const int wheel, const int /*x*/, const int /*y*/)
{
	if (framebuffer_)
	{
		HomogenousMatrix4 transformation = framebuffer_->view()->transformation();

		const Vector3 direction(transformation.rotation() * Vector3(0, 0, -1));
		const Vector3 position(transformation.translation());

		transformation.setTranslation(position + direction * (Scalar(wheel) * Scalar(0.01)));

		framebuffer_->view()->setTransformation(transformation);

		repaint(true);
	}
}

void RendererWinMainWindow::modifyWindowClass(WNDCLASS& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = nullptr;
}
