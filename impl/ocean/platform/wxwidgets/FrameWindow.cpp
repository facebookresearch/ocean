// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/wxwidgets/FrameWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

FrameWindow::FrameWindow(const wxString& title, wxWindow* parent, const wxPoint& pos, const wxSize& size) :
	BitmapWindow(title, parent, pos, size)
{
	// nothing to do here
}

bool FrameWindow::setFrame(const Frame& frame)
{
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, frame_, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		TemporaryScopedLock scopedLock(bitmapLock_);
			bitmap_ = wxBitmap();
		scopedLock.release();

		Refresh(false);

		return false;
	}

	frame_.makeContinuous();

	const wxImage windowImage = wxImage(int(frame_.width()), int(frame_.height()), frame_.data<uint8_t>(), true);

	TemporaryScopedLock scopedLock(bitmapLock_);
		bitmap_ = wxBitmap(windowImage);
	scopedLock.release();

	Refresh(false);

	return true;
}

}

}

}
