/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/histogram/wxw/HistogramMainWindow.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/wxwidgets/DnD.h"
#include "ocean/platform/wxwidgets/Utilities.h"

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(HistogramMainWindow, wxFrame)
	EVT_MENU(HistogramMainWindow::ID_Open,  HistogramMainWindow::onOpen)
	EVT_MENU(HistogramMainWindow::ID_Quit,  HistogramMainWindow::onQuit)
	EVT_MENU(HistogramMainWindow::ID_About, HistogramMainWindow::onAbout)
	EVT_IDLE(HistogramMainWindow::onIdle)
END_EVENT_TABLE()

using namespace Ocean;

HistogramMainWindow::HistogramMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_Open, L"&Open media...");
	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, L"&File");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	CreateStatusBar();

	bitmapWindow_ = new Platform::WxWidgets::BitmapWindow(L"Bitmap", this);

	wxFrame* frameHistogram = new wxFrame(this, -1, L"Histogram");
	histogramWindow_ = new Platform::WxWidgets::BitmapWindow(L"Histogram", frameHistogram);
	frameHistogram->Show();

	wxFrame* frameIntegral = new wxFrame(this, -1, L"Integral Histogram");
	integralWindow_ = new Platform::WxWidgets::BitmapWindow(L"Integral Histogram", frameIntegral);
	frameIntegral->Show();

	Platform::WxWidgets::FileDropTarget* dropTarget = new Platform::WxWidgets::FileDropTarget(Platform::WxWidgets::FileDropTarget::Callback(*this, &HistogramMainWindow::onFileDragAndDrop));
	SetDropTarget(dropTarget);
}

HistogramMainWindow::~HistogramMainWindow()
{
	// nothing to do here
}

bool HistogramMainWindow::loadFrame(const std::string& filename)
{
	const Frame frame(Media::Utilities::loadImage(filename));

	if (!frame.isValid())
	{
		return false;
	}

	ocean_assert(bitmapWindow_);
	if (bitmapWindow_->setFrame(frame))
	{
		wxBitmap bitmapHistogram, bitmapIntegral;

		if (visualizeHistogram3Channels(frame, 512u, 300u, bitmapHistogram, bitmapIntegral))
		{
			ocean_assert(histogramWindow_);
			histogramWindow_->setBitmap(bitmapHistogram);

			ocean_assert(integralWindow_);
			integralWindow_->setBitmap(bitmapIntegral);
		}

		return true;
	}

	return false;
}

bool HistogramMainWindow::visualizeHistogram3Channels(const Frame& frame, const unsigned int width, const unsigned int height, wxBitmap& bitmapHistogram, wxBitmap& bitmapIntegral)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.numberPlanes() == 1u);
	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 3u));

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	const CV::Histogram::Histogram8BitPerChannel<3u> histogram(CV::Histogram::determineHistogram8BitPerChannel<3u>(rgbFrame.constdata<uint8_t>(), rgbFrame.width(), rgbFrame.height(), rgbFrame.paddingElements(), WorkerPool::get().scopedWorker()()));
	CV::Histogram::IntegralHistogram8BitPerChannel<3u> integralHistogram(histogram);
	integralHistogram.normalize(0xFF);

	CV::Histogram::IntegralHistogram8BitPerChannel<3u> invertedHistogram(integralHistogram.invert());

	return drawHistogram3Channels(histogram, width, height, bitmapHistogram) && drawHistogram3Channels(integralHistogram, width, height, bitmapIntegral);
}

bool HistogramMainWindow::drawHistogram3Channels(const CV::Histogram::HistogramBase8BitPerChannel<3u>& histogram, const unsigned int width, const unsigned int height, wxBitmap& bitmap)
{
	const unsigned int maximalValue = histogram.determineHighestValue();

	if (maximalValue == 0u)
	{
		return false;
	}

	bitmap = wxBitmap(int(width), int(height), -1);

	const wxColour colors[3] =
	{
		wxColour(0x0000FFul),
		wxColour(0x00FF00ul),
		wxColour(0xFF0000ul)
	};

	wxMemoryDC bitmapDC;
	bitmapDC.SelectObject(bitmap);

	bitmapDC.SetBackground(wxBrush(wxColour(0xFFFFFFul)));
	bitmapDC.Clear();

	for (unsigned int c = 0u; c < 3u; ++c)
	{
		bitmapDC.SetPen(wxPen(colors[c], 2));

		for (unsigned int n = 0u; n < 255u; ++n)
		{
			const int x0 = n * width / 256u;
			const int y0 = height - histogram.bin(c, n) * height / maximalValue;

			const int x1 = (n + 1u) * width / 256u;
			const int y1 = height - histogram.bin(c, n + 1u) * height / maximalValue;

			bitmapDC.DrawLine(x0, y0, x1, y1);
		}
	}

	return true;
}

void HistogramMainWindow::onOpen(wxCommandEvent& /*event*/)
{
	 wxFileDialog openFileDialog(this, L"Open media files", L"", L"", L"Media files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_OK)
	{
		loadFrame(Platform::WxWidgets::Utilities::toAString(openFileDialog.GetPath()));
	}
}

void HistogramMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void HistogramMainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is a wxWidgets Histogram sample application", L"About Histogram", wxOK | wxICON_INFORMATION, this);
}

void HistogramMainWindow::onIdle(wxIdleEvent& /*event*/)
{
	// nothing to do here
}

bool HistogramMainWindow::onFileDragAndDrop(const std::vector<std::string> &files)
{
	if (files.empty())
	{
		return false;
	}

	return loadFrame(files.front());
}
