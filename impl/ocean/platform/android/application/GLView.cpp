/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/GLView.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

GLView& GLView::Instance::instance()
{
	if (!view_)
	{
		const ScopedLock scopedLock(lock_);

		if (!view_)
		{
			// we create the default GLView object is no specific instance function is defined at this moment

			if (derivedInstanceFunction_ != nullptr)
			{
				view_ = derivedInstanceFunction_();
			}
			else if (baseInstanceFunction_ != nullptr)
			{
				view_ = baseInstanceFunction_();
			}
			else
			{
				Log::error() << "GLView does not have a valid instance function!";
			}
		}
	}

	ocean_assert(view_ != nullptr);
	return *view_;
}

GLView::Instance::~Instance()
{
	delete view_;
}

bool GLView::initialize()
{
	// should be implemented in derived classes

	return true;
}

bool GLView::release()
{
	// should be implemented in derived classes

	return true;
}

bool GLView::resize(const int width, const int height)
{
	// should be implemented in derived classes

	return true;
}

bool GLView::render()
{
	// should be implemented in derived classes

	return true;
}

bool GLView::screen2frame(const Scalar xScreen, const Scalar yScreen, Scalar& xFrame, Scalar& yFrame)
{
    // should be implemented in derived classes

	return false;
}

void GLView::onTouchDown(const float x, const float y)
{
	 // should be implemented in derived classes
}

void GLView::onTouchMove(const float x, const float y)
{
	 // should be implemented in derived classes
}

void GLView::onTouchUp(const float x, const float y)
{
	 // should be implemented in derived classes
}

void GLView::onResume()
{
	// can be implemented in derived classes
}

void GLView::onPause()
{
	// can be implemented in derived classes
}

void GLView::onStop()
{
	// can be implemented in derived classes
}

void GLView::onDestroy()
{
	// can be implemented in derived classes
}

void GLView::onPermissionGranted(const std::string& /*permission*/)
{
	// can be implemented in derived classes
}

GLView& GLView::get()
{
	return Instance::get().instance();
}

}

}

}

}
