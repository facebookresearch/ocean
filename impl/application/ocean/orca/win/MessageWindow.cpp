/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/orca/win/MessageWindow.h"
#include "application/ocean/orca/win/Application.h"
#include "application/ocean/orca/win/MainWindow.h"

#include "ocean/base/Config.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

#define TAB_ID_ERRORS 2
#define TAB_ID_WARNINGS 3
#define TAB_ID_INFORMATIONS 4

BEGIN_MESSAGE_MAP(MessageWindow, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(MessageWindow::MessageList, CListBox)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

MessageWindow::MessageList::MessageList(const Messenger::MessageType type) :
	messageType_(type)
{
	// nothing to do here
}

MessageWindow::MessageList::~MessageList()
{
	// nothing to do here
}

void MessageWindow::MessageList::checkForNewMessages()
{
	std::string location, message;

	Timestamp newTimestamp;

	while (Messenger::get().popMessage(messageType_, location, message))
	{
		if (newTimestamp.isInvalid())
		{
			newTimestamp.toNow();
		}

		std::string combinedMessage(location.empty() ? message : location + std::string(": ") + message);

		if (combinedMessage == recentMessage_)
		{
			numberRecentMessages_++;

			DeleteString(GetCount() - 1);
			AddString(String::toWString(combinedMessage + std::string(" (") + String::toAString(numberRecentMessages_) + std::string(")")).c_str());
		}
		else
		{
			if (!timestamp_.isInvalid() && newTimestamp > timestamp_ + 2)
			{
				AddString(L"");
			}

			AddString(String::toWString(combinedMessage).c_str());
			numberRecentMessages_ = 1;
			timestamp_ = newTimestamp;
		}

		++numberMessages_;
		recentMessage_ = combinedMessage;

		CDC* dc = GetDC();
		horizontalExtent_ = max(int(dc->GetTextExtent(String::toWString(combinedMessage).c_str(), int(combinedMessage.length())).cx), horizontalExtent_);
		SetHorizontalExtent(horizontalExtent_);
		ReleaseDC(dc);

		SetTopIndex(GetCount() - 1);
	}
}

MessageWindow::MessageWindow() :
	errors_(Messenger::TYPE_ERROR),
	warnings_(Messenger::TYPE_WARNING),
	informations_(Messenger::TYPE_INFORMATION)
{
	// nothing to do here
}

MessageWindow::~MessageWindow()
{
	// nothing to do here
}

void MessageWindow::checkForNewMessages()
{
	if (errors_.GetCount() > 5000)
	{
		errors_.ResetContent();
	}
	if (warnings_.GetCount() > 5000)
	{
		warnings_.ResetContent();
	}
	if (informations_.GetCount() > 5000)
	{
		informations_.ResetContent();
	}

	errors_.checkForNewMessages();
	if (errors_.messages() != numberErrors_)
	{
		numberErrors_ = errors_.messages();
		tabs_.SetTabLabel(0, (std::wstring(L"Errors ( ") + String::toWString(numberErrors_) + std::wstring(L" )")).c_str());
	}

	warnings_.checkForNewMessages();
	if (warnings_.messages() != numberWarnings_)
	{
		numberWarnings_ = warnings_.messages();
		tabs_.SetTabLabel(1, (std::wstring(L"Warnings ( ") + String::toWString(numberWarnings_) + std::wstring(L" )")).c_str());
	}

	informations_.checkForNewMessages();
	if (informations_.messages() != numberInformations_)
	{
		numberInformations_ = informations_.messages();
		tabs_.SetTabLabel(2, (std::wstring(L"Informations ( ") + String::toWString(numberInformations_) + std::wstring(L" )")).c_str());
	}
}

int MessageWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDockablePane::OnCreate(lpCreateStruct);

	font_.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	tabs_.Create(CMFCTabCtrl::STYLE_3D, rectDummy, this, 1);

	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	errors_.Create(dwStyle, rectDummy, &tabs_, TAB_ID_ERRORS);
	warnings_.Create(dwStyle, rectDummy, &tabs_, TAB_ID_WARNINGS);
	informations_.Create(dwStyle, rectDummy, &tabs_, TAB_ID_INFORMATIONS);

	errors_.SetFont(&font_);
	warnings_.SetFont(&font_);
	informations_.SetFont(&font_);

	tabs_.AddTab(&errors_, L"Errors");
	tabs_.AddTab(&warnings_, L"Warnings");
	tabs_.AddTab(&informations_, L"Informations");

	return 0;
}

void MessageWindow::OnShowWindow(BOOL show, UINT status)
{
	CDockablePane::OnShowWindow(show, status);

	if (configurationApplied_ && show == TRUE)
	{
		Application::get().config()["messagewindow"]["visible"] = true;
	}
}

void MessageWindow::OnMove(int left, int top)
{
	CDockablePane::OnMove(left, top);

	if (configurationApplied_ && left > 0 && top > 0)
	{
		Config::Value& messageConfig = Application::get().config()["messagewindow"];

		messageConfig["left"] = left;
		messageConfig["top"] = top;
	}
}

void MessageWindow::OnSize(UINT type, int width, int height)
{
	CDockablePane::OnSize(type, width, height);
	tabs_.SetWindowPos (nullptr, -1, -1, width, height, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	if (configurationApplied_)
	{
		Config::Value& messageConfig = Application::get().config()["messagewindow"];

		messageConfig["width"] = width;
		messageConfig["height"] = height;
	}
}

void MessageWindow::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();

	if (configurationApplied_)
	{
		Application::get().config()["messagewindow"]["visible"] = false;
	}
}

void MessageWindow::OnAfterFloat()
{
	CDockablePane::OnAfterFloat();

	if (configurationApplied_)
	{
		Application::get().config()["messagewindow"]["dockingPosition"] = "none";
	}
}

void MessageWindow::OnAfterDock(CBasePane* bar, LPCRECT rect, AFX_DOCK_METHOD method)
{
	CDockablePane::OnAfterDock(bar, rect, method);

	if (configurationApplied_)
	{
		Application::get().config()["messagewindow"]["dockingPosition"] = "bottom";
	}
}

void MessageWindow::applyConfiguration()
{
	Config::Value& messageConfig = Application::get().config()["messagewindow"];

	int left = messageConfig["left"](invalidWindowValue_);
	int top = messageConfig["top"](invalidWindowValue_);
	int width = messageConfig["width"](invalidWindowValue_);
	int height = messageConfig["height"](invalidWindowValue_);

	unsigned int flag = SWP_NOACTIVATE | SWP_NOZORDER;
	if (left == invalidWindowValue_ && top == invalidWindowValue_)
	{
		flag |= SWP_NOMOVE;
	}
	if (width == invalidWindowValue_ || height == invalidWindowValue_)
	{
		flag |= SWP_NOSIZE;
	}
	SetWindowPos(nullptr, left, top, width, height, flag);

	const bool visible = messageConfig["visible"](true) && !Application::get().config()["application"]["startfullscreen"](false);

	std::string dockingPosition = messageConfig["dockingPosition"]("bottom");
	if (dockingPosition == "none")
	{
		if (left == invalidWindowValue_)
		{
			left = 100;
		}
		if (top == invalidWindowValue_)
		{
			top = 100;
		}
		if (width == invalidWindowValue_)
		{
			width = 100;
		}
		if (height == invalidWindowValue_)
		{
			height = 100;
		}

		FloatPane(CRect(left, top, left + width, top + height), DM_UNKNOWN, visible);
	}
	else
	{
		ShowPane(visible, false, true);
	}

	UpdateWindow();
	AdjustLayout();

	configurationApplied_ = true;
}

}

}

}
