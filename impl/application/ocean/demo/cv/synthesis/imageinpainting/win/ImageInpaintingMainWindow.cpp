/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpaintingMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/File.h"

#include "ocean/io/image/Image.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/wxwidgets/DnD.h"
#include "ocean/platform/wxwidgets/Utilities.h"

#include "application/ocean/demo/cv/synthesis/imageinpainting/open.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/mark.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/finiteline.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/nozoom.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/remove1.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/remove2.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/remove3.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/remove4.xpm"
#include "application/ocean/demo/cv/synthesis/imageinpainting/remove5.xpm"

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(MainWindow::ID_Open, MainWindow::onOpen)
	EVT_MENU(MainWindow::ID_Save_Image, MainWindow::onSaveImage)
	EVT_MENU(MainWindow::ID_Save_Mask, MainWindow::onSaveMask)
	EVT_MENU(MainWindow::ID_Mark, MainWindow::onMark)
	EVT_MENU(MainWindow::ID_Line, MainWindow::onLine)
	EVT_MENU(MainWindow::ID_Reload, MainWindow::onReload)
	EVT_MENU(MainWindow::ID_Nozoom, MainWindow::onNoZoom)
	EVT_MENU(MainWindow::ID_Remove1, MainWindow::onRemove1)
	EVT_MENU(MainWindow::ID_Remove2, MainWindow::onRemove2)
	EVT_MENU(MainWindow::ID_Remove3, MainWindow::onRemove3)
	EVT_MENU(MainWindow::ID_Remove4, MainWindow::onRemove4)
	EVT_MENU(MainWindow::ID_Remove5, MainWindow::onRemove5)
	EVT_MENU(MainWindow::ID_Quit, MainWindow::onQuit)
	EVT_MENU(MainWindow::ID_About, MainWindow::onAbout)
END_EVENT_TABLE()

using namespace Ocean;

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_Open, L"&Open file\tCtrl-O");
	menuFile->Append(ID_Reload, L"&Reload file\tCtrl-R");
	menuFile->Append(ID_Save_Image, L"Save image");
	menuFile->Append(ID_Save_Mask, L"Save mask");
	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenu *menuInpainting = new wxMenu;

	menuInpainting->Append(ID_Remove1, L"&Remove 1\tCtrl-1");
	menuInpainting->Append(ID_Remove2, L"&Remove 2\tCtrl-2");
	menuInpainting->Append(ID_Remove3, L"&Remove 3\tCtrl-3");
	menuInpainting->Append(ID_Remove4, L"&Remove 4\tCtrl-4");
	menuInpainting->Append(ID_Remove5, L"&Remove 5\tCtrl-5");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");
	menuBar->Append(menuInpainting, L"&Inpainting");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	CreateStatusBar(3);
	SetStatusBarPane(2);

	CreateToolBar(long(wxNO_BORDER) | long(wxHORIZONTAL) | long(wxTB_FLAT), ID_Toolbar);
	GetToolBar()->SetMargins(2, 2);
	GetToolBar()->AddTool(ID_Open, L"", wxBitmap(xpm_open), wxNullBitmap, wxITEM_NORMAL, L"Open file");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Mark, L"", wxBitmap(xpm_mark), wxNullBitmap, wxITEM_CHECK, L"Mark area");
	GetToolBar()->AddTool(ID_Line, L"", wxBitmap(xpm_finiteline), wxNullBitmap, wxITEM_CHECK, L"Line");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Nozoom, L"", wxBitmap(xpm_nozoom), wxNullBitmap, wxITEM_NORMAL, L"No zoom");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Remove1, L"", wxBitmap(xpm_remove1), wxNullBitmap, wxITEM_NORMAL, L"Remove object");
	GetToolBar()->AddTool(ID_Remove2, L"", wxBitmap(xpm_remove2), wxNullBitmap, wxITEM_NORMAL, L"Remove object");
	GetToolBar()->AddTool(ID_Remove3, L"", wxBitmap(xpm_remove3), wxNullBitmap, wxITEM_NORMAL, L"Remove object");
	GetToolBar()->AddTool(ID_Remove4, L"", wxBitmap(xpm_remove4), wxNullBitmap, wxITEM_NORMAL, L"Remove object");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Remove5, L"", wxBitmap(xpm_remove5), wxNullBitmap, wxITEM_NORMAL, L"Remove object");
	GetToolBar()->Realize();

	GetToolBar()->ToggleTool(ID_Mark, true);

	surface_ = new Surface(this);
	surface_->Show();

	Platform::WxWidgets::FileDropTarget* dropTarget = new Platform::WxWidgets::FileDropTarget(Platform::WxWidgets::FileDropTarget::Callback::create(*this, &MainWindow::onFileDragAndDrop));
	SetDropTarget(dropTarget);
}

bool MainWindow::loadFile(const std::string& filename)
{
	const Frame frame = IO::Image::readImage(filename);

	if (!frame.isValid())
	{
		wxMessageBox(std::wstring(L"Failed to open the image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}

	ocean_assert(surface_);
	if (!surface_->setFrame(frame))
	{
		wxMessageBox(std::wstring(L"Failed to convert the image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}

	imageFile_ = filename;
	const IO::File file(filename);

	SetStatusText(String::toWString(file.name()) + std::wstring(L", ") + String::toWString(frame.width()) + std::wstring(L"x") + String::toWString(frame.height()));

	return true;
}

bool MainWindow::loadMask(const std::string& filename)
{
	const Frame frame = IO::Image::readImage(filename);

	if (!frame.isValid())
	{
		wxMessageBox(std::wstring(L"Failed to open the image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}

	Frame maskFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, maskFrame, true, nullptr))
	{
		return false;
	}

	for (unsigned int y = 0u; y < maskFrame.height(); ++y)
	{
		const uint8_t* maskRow = maskFrame.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < maskFrame.width(); ++x)
		{
			if (maskRow[x] != 0x00u && maskRow[x] != 0xFFu)
			{
				return false;
			}
		}
	}

	ocean_assert(surface_ != nullptr);
	if (!surface_->setMask(maskFrame))
	{
		wxMessageBox(std::wstring(L"Failed to convert the image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}

	return true;
}

void MainWindow::onSaveImage(wxCommandEvent& /*event*/)
{
	const std::wstring allWildcard(L"All supported files|*.bmp;*.gif;*.jpg;*.jpeg;*.png");
	const std::wstring bmpWildcard(L"Bitmap files (*.bmp)|*.bmp");
	const std::wstring gifWildcard(L"GIF files (*.gif)|*.gif");
	const std::wstring jpgWildcard(L"JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg");
	const std::wstring pngwildCard(L"PNG files (*.png)|*.png");

	const std::wstring wildcard = allWildcard + std::wstring(L"|") + bmpWildcard + std::wstring(L"|") + gifWildcard + std::wstring(L"|") + jpgWildcard + std::wstring(L"|") + pngwildCard;

	wxFileDialog dialog(this, L"Save image file...", L"", L"", wildcard.c_str(), wxFD_SAVE);

	if (wxID_OK != dialog.ShowModal())
	{
		return;
	}

	Media::Utilities::saveImage(surface_->frame(), Platform::WxWidgets::Utilities::toAString(dialog.GetPath()), false);
}

void MainWindow::onSaveMask(wxCommandEvent& /*event*/)
{
	const std::wstring allWildcard(L"All supported files|*.bmp;*.gif;*.jpg;*.jpeg;*.png");
	const std::wstring bmpWildcard(L"Bitmap files (*.bmp)|*.bmp");
	const std::wstring gifWildcard(L"GIF files (*.gif)|*.gif");
	const std::wstring jpgWildcard(L"JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg");
	const std::wstring pngwildCard(L"PNG files (*.png)|*.png");

	const std::wstring wildcard = allWildcard + std::wstring(L"|") + bmpWildcard + std::wstring(L"|") + gifWildcard + std::wstring(L"|") + jpgWildcard + std::wstring(L"|") + pngwildCard;

	wxFileDialog dialog(this, L"Save mask file...", L"", L"", wildcard.c_str(), wxFD_SAVE);

	if (wxID_OK != dialog.ShowModal())
	{
		return;
	}

	Media::Utilities::saveImage(surface_->mask(), Platform::WxWidgets::Utilities::toAString(dialog.GetPath()), false);
}

void MainWindow::onOpen(wxCommandEvent& /*event*/)
{
	const std::wstring allWildcard(L"All supported files|*.bmp;*.gif;*.jpg;*.jpeg;*.png");
	const std::wstring bmpWildcard(L"Bitmap files (*.bmp)|*.bmp");
	const std::wstring gifWildcard(L"GIF files (*.gif)|*.gif");
	const std::wstring jpgWildcard(L"JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg");
	const std::wstring pngwildCard(L"PNG files (*.png)|*.png");

	const std::wstring wildcard = allWildcard + std::wstring(L"|") + bmpWildcard + std::wstring(L"|") + gifWildcard + std::wstring(L"|") + jpgWildcard + std::wstring(L"|") + pngwildCard;

	wxFileDialog dialog(this, L"Select image file...", L"", L"", wildcard.c_str(), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

	if (wxID_OK != dialog.ShowModal())
	{
		return;
	}

	loadFile(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
}

void MainWindow::onMark(wxCommandEvent& /*event*/)
{
	if (interactionMode_ == IM_MARK)
	{
		GetToolBar()->ToggleTool(ID_Mark, false);
		interactionMode_ = IM_NONE;
	}
	else
	{
		GetToolBar()->ToggleTool(ID_Line, false);
		GetToolBar()->ToggleTool(ID_Mark, true);
		interactionMode_ = IM_MARK;
	}

	surface_->setInteractionMode(interactionMode_);
}

void MainWindow::onLine(wxCommandEvent& /*event*/)
{
	if (interactionMode_ == IM_LINE)
	{
		GetToolBar()->ToggleTool(ID_Line, false);
		interactionMode_ = IM_NONE;
	}
	else
	{
		GetToolBar()->ToggleTool(ID_Mark, false);
		GetToolBar()->ToggleTool(ID_Line, true);
		interactionMode_ = IM_LINE;
	}

	surface_->setInteractionMode(interactionMode_);
}

void MainWindow::onReload(wxCommandEvent& /*event*/)
{
	if (imageFile_.empty())
	{
		return;
	}

	loadFile(imageFile_);
}

void MainWindow::onNoZoom(wxCommandEvent& /*event*/)
{
	ocean_assert(surface_);
	surface_->setZoom(1);
}

void MainWindow::onRemove1(wxCommandEvent& /*event*/)
{
	const HighPerformanceTimer timer;

	ocean_assert(surface_);
	surface_->executeInpainting(0);

	const double time = timer.mseconds();
	SetStatusText(std::wstring(L"Performance: ") + String::toWString(time) + std::wstring(L"ms"));
}

void MainWindow::onRemove2(wxCommandEvent& /*event*/)
{
	const HighPerformanceTimer timer;

	ocean_assert(surface_);
	surface_->executeInpainting(1);

	const double time = timer.mseconds();
	SetStatusText(std::wstring(L"Performance: ") + String::toWString(time) + std::wstring(L"ms"));
}

void MainWindow::onRemove3(wxCommandEvent& /*event*/)
{
	const HighPerformanceTimer timer;

	ocean_assert(surface_);
	surface_->executeInpainting(2);

	const double time = timer.mseconds();
	SetStatusText(std::wstring(L"Performance: ") + String::toWString(time) + std::wstring(L"ms"));
}

void MainWindow::onRemove4(wxCommandEvent& /*event*/)
{
	const HighPerformanceTimer timer;

	ocean_assert(surface_);
	surface_->executeInpainting(3);

	const double time = timer.mseconds();
	SetStatusText(std::wstring(L"Performance: ") + String::toWString(time) + std::wstring(L"ms"));
}

void MainWindow::onRemove5(wxCommandEvent& /*event*/)
{
	const HighPerformanceTimer timer;

	ocean_assert(surface_);
	surface_->executeInpaintingTexture();

	const double time = timer.mseconds();
	SetStatusText(std::wstring(L"Performance: ") + String::toWString(time) + std::wstring(L"ms"));
}

void MainWindow::onQuit(wxCommandEvent& /*event*/)
{
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

	if (wxGetKeyState(WXK_CONTROL))
	{
		return loadMask(files.front());
	}

	return loadFile(files.front());
}
