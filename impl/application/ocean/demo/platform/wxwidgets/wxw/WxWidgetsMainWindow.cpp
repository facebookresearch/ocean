// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/platform/wxwidgets/wxw/WxWidgetsMainWindow.h"

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(WxWidgetsMainWindow, wxFrame)
	EVT_MENU(WxWidgetsMainWindow::ID_Quit,  WxWidgetsMainWindow::onQuit)
	EVT_MENU(WxWidgetsMainWindow::ID_About, WxWidgetsMainWindow::onAbout)
END_EVENT_TABLE()

WxWidgetsMainWindow::WxWidgetsMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, L"&File");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	CreateStatusBar();
	SetStatusText(L"Welcome to wxWidgets!");
}

void WxWidgetsMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void WxWidgetsMainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is a wxWidgets Hello world sample", L"About Hello World", wxOK | wxICON_INFORMATION, this);
}
