/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/moviepreview/wxw/MoviePreviewMainWindow.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/File.h"

#include "ocean/media/Movie.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"
#include "ocean/platform/wxwidgets/DnD.h"
#include "ocean/platform/wxwidgets/Utilities.h"

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(MoviePreviewMainWindow, wxFrame)
	EVT_MENU(MoviePreviewMainWindow::ID_Open_Media, MoviePreviewMainWindow::onOpenMedia)
	EVT_MENU(MoviePreviewMainWindow::ID_Save_Image, MoviePreviewMainWindow::onSaveImage)
	EVT_MENU(MoviePreviewMainWindow::ID_Save_Images, MoviePreviewMainWindow::onSaveImages)
	EVT_MENU(MoviePreviewMainWindow::ID_Quit, MoviePreviewMainWindow::onQuit)
	EVT_MENU(MoviePreviewMainWindow::ID_About, MoviePreviewMainWindow::onAbout)
	EVT_IDLE(MoviePreviewMainWindow::onIdle)
END_EVENT_TABLE()

MoviePreviewMainWindow::MoviePreviewMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_Open_Media, L"&Open media...");
	menuFile->Append(ID_Save_Image, L"Save &image...");
	menuFile->Append(ID_Save_Images, L"Save image &sequence...");
	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, L"&File");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	CreateStatusBar();

	frameProvider_ = Media::MovieFrameProviderRef(new Media::MovieFrameProvider());
	frameProvider_->setPreferredFrameType(FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);

	bitmapWindow_ = new MoviePreviewBitmapWindow(L"bitmap", this);
	bitmapWindow_->setMovieFrameProvider(frameProvider_);

	previewWindow_ = new Platform::WxWidgets::FramePreviewWindow(L"Preview", this);

	previewWindow_->setFrameProvider(frameProvider_);

	previewWindow_->setSelectionCallback(Platform::WxWidgets::FramePreviewWindow::SelectionCallback(*this, &MoviePreviewMainWindow::onSelection));
	previewWindow_->SetBackgroundColour(GetBackgroundColour());
	previewWindow_->SetFocus();

	wxBoxSizer* verticalSizer = new wxBoxSizer(wxVERTICAL);
	verticalSizer->Add(bitmapWindow_, 1, int(wxEXPAND) | int(wxTOP));
	verticalSizer->AddSpacer(2);
	verticalSizer->Add(previewWindow_, 0, wxEXPAND);

	SetSizer(verticalSizer);

	Platform::WxWidgets::FileDropTarget* dropTarget = new Platform::WxWidgets::FileDropTarget(Platform::WxWidgets::FileDropTarget::Callback(*this, &MoviePreviewMainWindow::onFileDragAndDrop));
	SetDropTarget(dropTarget);
}

MoviePreviewMainWindow::~MoviePreviewMainWindow()
{
	ocean_assert(previewWindow_);
	previewWindow_->setSelectionCallback(Platform::WxWidgets::FramePreviewWindow::SelectionCallback());

	frameProvider_.release();
}

void MoviePreviewMainWindow::onOpenMedia(wxCommandEvent& /*event*/)
{
	 wxFileDialog openFileDialog(this, L"Open media files", L"", L"", L"Media files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_OK)
	{
		onFileDragAndDrop(std::vector<std::string>(1, Platform::WxWidgets::Utilities::toAString(openFileDialog.GetPath())));
	}
}

void MoviePreviewMainWindow::onSaveImage(wxCommandEvent& /*event*/)
{
	if (bitmapWindow_)
	{
		Frame frame(bitmapWindow_->frame(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		if (frame)
		{
			CV::FrameConverter::Comfort::change(frame, FrameType::FORMAT_RGB24);

			wxFileDialog saveFileDialog(this, L"Open media files", L"", L"", L"Image files (*.*)|*.*", wxFD_SAVE);

			if (saveFileDialog.ShowModal() == wxID_OK)
			{
				const wxString path = saveFileDialog.GetPath();

				Media::Utilities::saveImage(frame, Platform::WxWidgets::Utilities::toAString(path), false);
			}
		}
	}
}

void MoviePreviewMainWindow::onSaveImages(wxCommandEvent& /*event*/)
{
	if (frameProvider_ && frameProvider_->actualFrameNumber() != 0u)
	{
		wxFileDialog saveFileDialog(this, L"Open media files", L"", L"", L"Image files (*.*)|*.*", wxFD_SAVE);

		if (saveFileDialog.ShowModal() != wxID_OK)
		{
			return;
		}

		const wxString path = saveFileDialog.GetPath();

		const IO::File originalPath(Platform::WxWidgets::Utilities::toAString(path));

		for (unsigned int n = 0u; n < frameProvider_->actualFrameNumber(); ++n)
		{
			const FrameRef frame(frameProvider_->synchronFrameRequest(n));

			if (frame.isNull())
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			Frame rgbFrame;
			if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			Media::Utilities::saveImage(rgbFrame, originalPath.base() + String::toAString(n, 5u) + std::string(".") + originalPath.extension(), false);
		}
	}
}

void MoviePreviewMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void MoviePreviewMainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is a wxWidgets Movie Preview sample application", L"About Movie Preview", wxOK | wxICON_INFORMATION, this);
}

void MoviePreviewMainWindow::onSelection(const unsigned int index)
{
	if (index != (unsigned int)(-1) && frameProvider_)
	{
		ocean_assert(bitmapWindow_);

		bitmapWindow_->setSelectionIndex(index);
		frameProvider_->asynchronFrameRequest(index, true);
	}
}

void MoviePreviewMainWindow::onIdle(wxIdleEvent& /*event*/)
{
	const unsigned int value = frameProvider_ ? frameProvider_->previewProgress() : 0u;

	if (value != recentProgress_)
	{
		std::wstring msg(std::wstring(L"Preview progress: ") + String::toWString(value) + std::wstring(L"%"));

		SetStatusText(msg.c_str());
		recentProgress_ = value;
	}
}

bool MoviePreviewMainWindow::onFileDragAndDrop(const std::vector<std::string> &files)
{
	if (files.empty())
	{
		return false;
	}

	const Media::MovieRef movie = Media::Manager::get().newMedium(files.front(), Media::Medium::MOVIE, true);

	return frameProvider_->setMovie(movie);
}
