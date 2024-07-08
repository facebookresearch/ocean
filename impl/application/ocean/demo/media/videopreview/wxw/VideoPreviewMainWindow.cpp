/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/videopreview/wxw/VideoPreviewMainWindow.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/wxwidgets/DnD.h"

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(MainWindow::ID_Quit,  MainWindow::onQuit)
	EVT_MENU(MainWindow::ID_About, MainWindow::onAbout)
END_EVENT_TABLE()

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, L"&File");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	frameWindow_ = new Platform::WxWidgets::FrameMediumWindow(wxString(L"Preview"), this);
	frameWindow_->Show();

	Media::FrameMediumRef medium = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	if (medium.isNull())
	{
		medium = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (medium)
	{
		frameWindow_->setFrameMedium(medium);
		medium->start();
	}

	CreateStatusBar();
	SetStatusText(L"Welcome to Video Preview!");

	Platform::WxWidgets::FileDropTarget* dropTarget = new Platform::WxWidgets::FileDropTarget(Platform::WxWidgets::FileDropTarget::Callback::create(*this, &MainWindow::onFileDragAndDrop));
	SetDropTarget(dropTarget);
}

void MainWindow::onQuit(wxCommandEvent& /*event*/)
{
	if (frameWindow_)
	{
		frameWindow_->setFrameMedium(Media::FrameMediumRef());
	}

	Close(true);
}

void MainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is a wxWidgets Hello world sample", L"About Hello World", wxOK | wxICON_INFORMATION, this);
}

bool MainWindow::onFileDragAndDrop(const std::vector<std::string> &files)
{
	if (files.empty())
	{
		return false;
	}

	const Media::FrameMediumRef frameMedium(Media::Manager::get().newMedium(files[0], Media::Medium::FRAME_MEDIUM));

	if (frameMedium)
	{
		const Media::FiniteMediumRef finiteMedium(frameMedium);

		if (finiteMedium)
		{
			finiteMedium->setLoop(true);
		}

		if (frameWindow_)
		{
			frameWindow_->setFrameMedium(frameMedium);
		}

		frameMedium->start();
	}

	return true;
}
