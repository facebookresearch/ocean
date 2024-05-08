/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/advanced/poissonblending/wxw/PoissonBlendingMainWindow.h"

#include "ocean/base/String.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/advanced/PoissonBlending.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/wxwidgets/DnD.h"
#include "ocean/platform/wxwidgets/Utilities.h"

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(PoissonBlendingMainWindow, wxFrame)
	EVT_MENU(PoissonBlendingMainWindow::ID_Open, PoissonBlendingMainWindow::onOpen)
	EVT_MENU(PoissonBlendingMainWindow::ID_Quit, PoissonBlendingMainWindow::onQuit)
	EVT_MENU(PoissonBlendingMainWindow::ID_About, PoissonBlendingMainWindow::onAbout)
END_EVENT_TABLE()

PoissonBlendingMainWindow::PoissonBlendingMainWindow(const wxString& title, const wxPoint& position, const wxSize& size) :
	wxFrame(nullptr, -1, title, position, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_Open, L"&Open media...");
	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	outputWindow_ = new Platform::WxWidgets::BitmapWindow(L"Output", this);
}

PoissonBlendingMainWindow::~PoissonBlendingMainWindow()
{
	// nothing to do here
}

bool PoissonBlendingMainWindow::loadFrame(const wxString& filename, Frame& frame)
{
	frame = Media::Utilities::loadImage(Platform::WxWidgets::Utilities::toAString(filename));

	if (!frame)
	{
		return false;
	}

	return CV::FrameConverter::Comfort::change(frame, FrameType::ORIGIN_UPPER_LEFT);
}

void PoissonBlendingMainWindow::onOpen(wxCommandEvent& /*event*/)
{
	wxFileDialog openFileDialog(this, L"Open source frame", L"", L"", L"Media files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() != wxID_OK)
	{
		return;
	}

	Frame sourceFrame;
	if (!loadFrame(openFileDialog.GetPath(), sourceFrame))
	{
		wxMessageBox("Could not load source frame.");
		return;
	}

	if (sourceFrame.numberPlanes() != 1u || sourceFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		wxMessageBox("Pixel format of source frame must be 8bit per color channel.");
		return;
	}

	openFileDialog.SetMessage(L"Open source mask frame");
	if (openFileDialog.ShowModal() != wxID_OK)
	{
		return;
	}

	Frame sourceMask;
	if (!loadFrame(openFileDialog.GetPath(), sourceMask))
	{
		wxMessageBox("Could not load source mask frame.");
		return;
	}

	if (!CV::FrameConverter::Comfort::change(sourceMask, FrameType::FORMAT_Y8, sourceMask.pixelOrigin(), CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		wxMessageBox("Pixel format of source mask frame could not be converted to an 8 bit grayscale frame.");
		return;
	}

	openFileDialog.SetMessage(L"Open target frame");
	if (openFileDialog.ShowModal() != wxID_OK)
	{
		return;
	}

	Frame targetFrame;
	if (!loadFrame(openFileDialog.GetPath(), targetFrame))
	{
		wxMessageBox("Could not load target frame.");
		return;
	}

	if (!CV::FrameConverter::Comfort::change(targetFrame, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		wxMessageBox("The target frame could not be converted so that the frame pixel format matches with the source frame.");
		return;
	}

	wxDialog dialog(nullptr, wxID_ANY, L"Specify top left insert position");
	wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);

	wxTextCtrl* textControlX = new wxTextCtrl(&dialog, wxID_ANY);
	boxSizer->Add(textControlX, 0, wxALL, 5);

	wxTextCtrl* textControlY = new wxTextCtrl(&dialog, wxID_ANY);
	boxSizer->Add(textControlY, 0, wxALL, 5);

	wxButton* buttonOK = new wxButton(&dialog, wxID_OK, L"OK");
	boxSizer->Add(buttonOK, 0, wxALL, 5);

	wxButton* buttonCancel = new wxButton(&dialog, wxID_CANCEL, L"Cancel");
	boxSizer->Add(buttonCancel, 0, wxALL, 5);

	boxSizer->Layout();
	dialog.SetSizer(boxSizer);

	dialog.Fit();
	dialog.SetEscapeId(wxID_ANY);
	buttonOK->SetDefault();

	long insertLeft = -1;
	long insertTop = -1;

	if (dialog.ShowModal() != wxID_OK || !textControlX->GetValue().ToLong(&insertLeft) || !textControlY->GetValue().ToLong(&insertTop))
	{
		return;
	}

	CV::Advanced::PoissonBlending::poissonBlending(sourceFrame, sourceMask, targetFrame, int(insertLeft), int(insertTop), 0xFFu, WorkerPool::get().scopedWorker()());

	outputWindow_->setFrame(targetFrame);
	outputWindow_->Show();
}

void PoissonBlendingMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void PoissonBlendingMainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is a wxWidgets Poisson Blending sample application", L"About Poisson Blending", wxOK | wxICON_INFORMATION, this);
}
