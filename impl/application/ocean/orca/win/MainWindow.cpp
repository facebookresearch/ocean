// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/orca/win/MainWindow.h"
#include "application/ocean/orca/win/Application.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/interaction/Manager.h"

#include "ocean/math/Euler.h"

#include "ocean/platform/win/Screen.h"

#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

BEGIN_MESSAGE_MAP(MainWindow, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DROPFILES()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENU_WINDOW_MESSAGE_WINDOW, onToggleMessageWindow)
	ON_COMMAND(ID_MENU_WINDOW_STATUS_BAR, onToggleStatusBar)
	ON_COMMAND(ID_MENU_WINDOW_FULLSCREEN, onToggleFullscreen)
	ON_COMMAND(ID_WINDOW_PROPERTIES, onTogglePropertiesWindow)
END_MESSAGE_MAP()

const int MainWindow::mainWindowInvalidWindowValue = 2147483647;

CShellManager* afxShellManager;

MainWindow::MainWindow() :
	mainWindowIsFullscreen(false),
	mainWindowNonFullScreenStyle(0),
	configurationApplied_(false),
	mainWindowReleased(false),
	contentEventCounter(0)
{
	afxShellManager = new CShellManager();

	ApplicationInterface::get().setContentAddCallbackFunction(ApplicationInterface::ContentCallback(*this, &MainWindow::onApplicationInterfaceContentAdd));
	ApplicationInterface::get().setContentRemoveCallbackFunction(ApplicationInterface::ContentCallback(*this, &MainWindow::onApplicationInterfaceContentRemove));

	ContentManager::get().setContentLoadedCallbackFunction(ContentManager::ContentCallback(*this, &MainWindow::onContentLoaded));
	ContentManager::get().setContentUnloadedCallbackFunction(ContentManager::ContentCallback(*this, &MainWindow::onContentUnloaded));
}

MainWindow::~MainWindow()
{
	ContentManager::get().release();

	ContentManager::get().setContentLoadedCallbackFunction(ContentManager::ContentCallback());
	ContentManager::get().setContentUnloadedCallbackFunction(ContentManager::ContentCallback());

	ApplicationInterface::get().setContentAddCallbackFunction(ApplicationInterface::ContentCallback());
	ApplicationInterface::get().setContentRemoveCallbackFunction(ApplicationInterface::ContentCallback());

	Ocean::Interaction::Manager::get().release();
}

ContentManager::ContentIds MainWindow::loadFiles(const Filenames& filenames, const bool add)
{
	if (filenames.empty())
		return ContentManager::ContentIds();

	if (!add)
		ContentManager::get().removeContent();

	ContentManager::ContentIds contentIds(ContentManager::get().addContent(filenames, mainView().engine(), SceneDescription::TYPE_PERMANENT));

	Filenames allFilenames;

	if (add)
		allFilenames = Application::convertFilenames((*config)["application"]["lastfilenames"](""));

	allFilenames.insert(allFilenames.end(), filenames.begin(), filenames.end());

	if (allFilenames.empty() == false)
	{
		(*config)["application"]["lastfilenames"] = Application::convertFilenames(allFilenames);

		std::string::size_type pos = allFilenames.back().rfind("\\");
		if (pos != std::string::npos)
			(*config)["application"]["lastfilepath"] = allFilenames.back().substr(0, pos);
	}

	return contentIds;
}

void MainWindow::setStatusBarSize(const unsigned int width, const unsigned int height)
{
	std::wstring text(L"Size: ");
	text += String::toWString(width);
	text += L"x";
	text += String::toWString(height);

	mainWindowStatusBar.SetPaneText(0, text.c_str());
}

void MainWindow::setStatusBarPosition(const Ocean::Vector3& position, const Ocean::Quaternion& orientation)
{
	Ocean::Euler euler(orientation);

	std::wstring text(L"Position: ");
	text += String::toWString(position.x());
	text += L", ";
	text += String::toWString(position.y());
	text += L", ";
	text += String::toWString(position.z());
	text += L"; Orientation: ";
	text += String::toWString(Ocean::Numeric::rad2deg(euler.yaw()));
	text += L"\u00B0, ";
	text += String::toWString(Ocean::Numeric::rad2deg(euler.pitch()));
	text += L"\u00B0, ";
	text += String::toWString(Ocean::Numeric::rad2deg(euler.roll()));
	text += L"\u00B0";

	mainWindowStatusBar.SetPaneText(1, text.c_str());
}

void MainWindow::setStatusBarProgress(const float progress)
{
	ocean_assert(progress >= 0 && progress <= 1);

	mainWindowStatusBar.SetPaneProgress(3, long(100 * progress));
}

void MainWindow::setStatusBarFramerate(const float fps)
{
	std::wstring text(String::toWString(float(int(fps * 10)) / 10) + std::wstring(L" fps"));
	mainWindowStatusBar.SetPaneText(4, text.c_str());
}

bool MainWindow::onIdle()
{
	if (mainWindowReleased)
		return false;

	mainWindowMessageWindow.checkForNewMessages();
	mainWindowView.onIdle();

	while (true)
	{
		ApplicationEventCaller eventCaller;

		{
			const ScopedLock scopedLock(mainWindowApplicationEventLock);

			if (mainWindowApplicationEvents.empty())
				break;

			eventCaller = mainWindowApplicationEvents.front();
			mainWindowApplicationEvents.pop();
		}

		eventCaller();
	}

	return true;
}

int MainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString windowText;
	GetWindowText(windowText);
	windowText += ", ";
	windowText += Ocean::Build::architectureType().c_str();
	windowText += " ";
	windowText += Ocean::Build::compilerVersion().c_str();
	windowText += " ";
	windowText += Ocean::Build::releaseType().c_str();
	SetWindowText(windowText);

	mainWindowMenuBar.Create(this);
	mainWindowMenuBar.SetPaneStyle(mainWindowMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	// create a view to occupy the client area of the frame
	mainWindowView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr);

	// initializes the status bar
	mainWindowStatusBar.Create(this);

	static UINT indicators[] = {IDS_STATUSBAR_PANE_POSITION, 0, IDS_STATUSBAR_PANE_PROGRESS, IDS_STATUSBAR_PANE_FRAMERATE};
	mainWindowStatusBar.SetIndicators(indicators, 5);

	mainWindowStatusBar.SetPaneWidth(0, 100);
	mainWindowStatusBar.SetPaneStyle(0, SBPS_NORMAL);
	mainWindowStatusBar.SetPaneBackgroundColor(0);

	mainWindowStatusBar.SetPaneWidth(1, 400);
	mainWindowStatusBar.SetPaneStyle(1, SBPS_NORMAL);
	mainWindowStatusBar.SetPaneBackgroundColor(1);

	mainWindowStatusBar.SetPaneStyle(2, SBPS_STRETCH);
	mainWindowStatusBar.SetPaneBackgroundColor(2);

	mainWindowStatusBar.SetPaneWidth(3, 100);
	mainWindowStatusBar.SetPaneStyle(3, SBPS_NORMAL);
	mainWindowStatusBar.EnablePaneProgressBar(3);
	mainWindowStatusBar.SetPaneBackgroundColor(3);

	mainWindowStatusBar.SetPaneWidth(4, 60);
	mainWindowStatusBar.SetPaneStyle(4, SBPS_NORMAL);
	mainWindowStatusBar.SetPaneBackgroundColor(4);

	// docking support
	mainWindowMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&mainWindowMenuBar);

	CDockingManager::SetDockingMode(DT_SMART);

	// Create the properties windows and docks it
	mainWindowPropertiesWindow.Create(L"Properties", this, CRect(0, 0, 200, 500), TRUE, ID_VIEW_PROPERTIESWINDOW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI);
	mainWindowPropertiesWindow.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&mainWindowPropertiesWindow);

	// Create the message output window and docks it
	mainWindowMessageWindow.Create(L"Output window", this, CRect(0, 0, 600, 200), TRUE, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI);
	mainWindowMessageWindow.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&mainWindowMessageWindow);

	// Allowing file dropping
	DragAcceptFiles();

	// Sets the window look
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CDockingManager::SetDockingMode(DT_SMART);

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	return 0;
}

void MainWindow::OnMove(int left, int top)
{
	CFrameWndEx::OnMove(left, top);
	if (configurationApplied_ && mainWindowIsFullscreen == false && left > 0 && top > 0)
	{
		Ocean::Config::Value& mainConfig = (*config)["mainwindow"];

		mainConfig["left"] = left;
		mainConfig["top"] = top;
	}
}

void MainWindow::OnSize(UINT type, int width, int height)
{
	CFrameWndEx::OnSize(type, width, height);

	if (configurationApplied_ && mainWindowIsFullscreen == false)
	{
		Ocean::Config::Value& mainConfig = (*config)["mainwindow"];

		mainConfig["maximized"] = type == SIZE_MAXIMIZED;
		mainConfig["minimized"] = type == SIZE_MINIMIZED;

		if (type == SIZE_RESTORED)
		{
			mainConfig["width"] = width;
			mainConfig["height"] = height;
		}
	}
}

void MainWindow::OnClose()
{
	storeConfiguration();

	mainWindowReleased = true;

	if ((*config)["readonly"](false) == false)
		config->write();

	CFrameWndEx::OnClose();
}

BOOL MainWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

void MainWindow::OnSetFocus(CWnd* /*lastWindow*/)
{
	mainWindowView.SetFocus();
}

void MainWindow::OnDropFiles(HDROP info)
{
	/*CMFCToolBarMenuButton* a = fileMenu();

	const CObject* c = a->GetCommands().GetAt(a->GetCommands().GetHeadPosition());

	const CMFCToolBarMenuButton* ee = (CMFCToolBarMenuButton*)c;

	CMFCToolBarMenuButton* tt = const_cast<CMFCToolBarMenuButton*>(ee);

	tt->Show(FALSE);

	mainWindowMenuBar.UpdateWindow();

	//tt->Show(FALSE);

	CMFCPopupMenu* m = dynamic_cast<CMFCPopupMenu*>(a);

	int t = 0;

	return;*/
	unsigned int files = DragQueryFile(info, 0xFFFFFFFF, nullptr, 0);
	Filenames filenames;

	char buffer[1025];
	for (unsigned int n = 0; n < files; n++)
	{
		DragQueryFileA(info, UINT(n), buffer, 1024);
		filenames.push_back(buffer);
	}

	DragFinish(info);

	if (filenames.empty() == true)
		return;

	loadFiles(filenames, (GetKeyState(VK_LCONTROL) & 0x800) || (GetKeyState(VK_RCONTROL) & 0x800));
}

BOOL MainWindow::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (mainWindowView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void MainWindow::onToggleFullscreen()
{
	if (mainWindowIsFullscreen)
	{
		SetWindowLongPtrA(m_hWnd, GWL_STYLE, mainWindowNonFullScreenStyle);
		mainWindowNonFullScreenStyle = 0;

		Ocean::Config::Value& mainConfig = (*config)["mainwindow"];

		int left = mainConfig["left"](mainWindowInvalidWindowValue);
		int top = mainConfig["top"](mainWindowInvalidWindowValue);
		int width = mainConfig["width"](mainWindowInvalidWindowValue);
		int height = mainConfig["height"](mainWindowInvalidWindowValue);

		unsigned int flag = 0;
		if (left == mainWindowInvalidWindowValue || top == mainWindowInvalidWindowValue)
			flag |= SWP_NOMOVE;
		if (width == mainWindowInvalidWindowValue || height == mainWindowInvalidWindowValue)
			flag |= SWP_NOSIZE;

		const int virtualDisplayWidth = int(Ocean::Platform::Win::Screen::virtualDisplayWidth());
		const int virtualDisplayHeight = int(Ocean::Platform::Win::Screen::virtualDisplayHeight());

		if ((left != mainWindowInvalidWindowValue && (left >= virtualDisplayWidth || (width != mainWindowInvalidWindowValue && left + width <= 0) || (width == mainWindowInvalidWindowValue && left <= 0)))
				|| (top != mainWindowInvalidWindowValue && (top >= virtualDisplayHeight || (height != mainWindowInvalidWindowValue && top + height <= 0) || (height == mainWindowInvalidWindowValue && top <= 0))))
		{
			flag |= SWP_NOMOVE;
			flag |= SWP_NOSIZE;
		}

		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, left, top, width, height, flag);

		if (mainConfig["maximized"](false))
			ShowWindow(SW_SHOWMAXIMIZED);
		else if (mainConfig["minimized"](false))
			ShowWindow(SW_MINIMIZE);

		// show menu bar
		mainWindowMenuBar.ShowPane(true, true, true);

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

		mainWindowIsFullscreen = false;

		if (configurationApplied_)
			(*config)["mainwindow"]["fullscreen"] = false;
	}
	else
	{
		mainWindowIsFullscreen = true;

		// reset visual style
		CMFCVisualManager::SetDefaultManager(nullptr);

		// store current window style
		ocean_assert(mainWindowNonFullScreenStyle == 0);

		// remove main window border
		mainWindowNonFullScreenStyle = int(SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE));

		// hide menu bar
		mainWindowMenuBar.ShowPane(false, true, true);

		unsigned int screenLeft, screenTop, screenWidth, screenHeight;
		if (Ocean::Platform::Win::Screen::screen(m_hWnd, screenLeft, screenTop, screenWidth, screenHeight) == false)
			ocean_assert(false && "Invalid screen");

		::SetWindowPos(m_hWnd, HWND_TOPMOST, int(screenLeft), int(screenTop), int(screenWidth), int(screenHeight), SWP_SHOWWINDOW);

		if (configurationApplied_)
			(*config)["mainwindow"]["fullscreen"] = true;
	}
}

void MainWindow::onTogglePropertiesWindow()
{
	const bool nowVisible = !mainWindowPropertiesWindow.IsVisible();

	mainWindowPropertiesWindow.ShowPane(nowVisible, false, true);

	if (configurationApplied_)
		(*config)["propertieswindow"]["visible"] = nowVisible;
}

void MainWindow::onToggleMessageWindow()
{
	const bool nowVisible = !mainWindowMessageWindow.IsVisible();

	mainWindowMessageWindow.ShowPane(nowVisible, false, true);

	if (configurationApplied_)
		(*config)["messagewindow"]["visible"] = nowVisible;
}

void MainWindow::onToggleStatusBar()
{
	const bool nowVisible = !mainWindowStatusBar.IsVisible();

	mainWindowStatusBar.ShowPane(nowVisible, false, true);

	if (configurationApplied_)
		(*config)["statusbar"]["visible"] = nowVisible;
}

void MainWindow::onContentLoaded(const ContentManager::ContentId contentId, const bool state)
{
	const ScopedLock scopedLock(mainWindowApplicationEventLock);
	mainWindowApplicationEvents.push(ApplicationEventCaller::create(*this, &MainWindow::applicationInterfaceContentAdded, contentId, state));
}

void MainWindow::onContentUnloaded(const ContentManager::ContentId contentId, const bool state)
{
	const ScopedLock scopedLock(mainWindowApplicationEventLock);
	mainWindowApplicationEvents.push(ApplicationEventCaller::create(*this, &MainWindow::applicationInterfaceContentRemoved, contentId, state));
}

CMFCToolBarMenuButton* MainWindow::fileMenu()
{
	CMFCToolBarButton* button = mainWindowMenuBar.GetMenuItem(0);

	ocean_assert(button != nullptr);
	ocean_assert(button->m_strText == L"&File");
	ocean_assert(dynamic_cast<CMFCToolBarMenuButton*>(button) != nullptr);

	return (CMFCToolBarMenuButton*)button;
}

CMFCToolBarMenuButton* MainWindow::windowMenu()
{
	CMFCToolBarButton* button = mainWindowMenuBar.GetMenuItem(3);

	ocean_assert(button != nullptr);
	ocean_assert(button->m_strText == L"&Window");
	ocean_assert(dynamic_cast<CMFCToolBarMenuButton*>(button) != nullptr);

	return (CMFCToolBarMenuButton*)button;
}

void MainWindow::setMenuMessageWindowChecked(const bool /*state*/)
{
	const CObList& commands = windowMenu()->GetCommands();
	ocean_assert(commands.GetCount() >= 3);

	POSITION pos = commands.FindIndex(0);
	ocean_assert(pos != nullptr);

	CMFCToolBarMenuButton* button = (CMFCToolBarMenuButton*)(commands.GetAt(pos));
	ocean_assert(button != nullptr);

	button->SetStyle(TBBS_CHECKBOX | TBBS_CHECKED);
	button->SetRadio();
	button->SetVisible(FALSE);
	button->Show(false);

	this->mainWindowMenuBar.UpdateButton(3);
	windowMenu()->SetStyle(TBBS_CHECKBOX);
}

void MainWindow::setMenuStatusBarChecked(const bool /*state*/)
{
	// nothing to do here
}

void MainWindow::applyConfiguration()
{
	Ocean::Config::Value& mainConfig = (*config)["mainwindow"];

	int left = mainConfig["left"](mainWindowInvalidWindowValue);
	int top = mainConfig["top"](mainWindowInvalidWindowValue);
	int width = mainConfig["width"](mainWindowInvalidWindowValue);
	int height = mainConfig["height"](mainWindowInvalidWindowValue);

	unsigned int flag = SWP_NOZORDER;
	if (left == mainWindowInvalidWindowValue || top == mainWindowInvalidWindowValue)
		flag |= SWP_NOMOVE;
	if (width == mainWindowInvalidWindowValue || height == mainWindowInvalidWindowValue)
		flag |= SWP_NOSIZE;

	const int virtualDisplayWidth = int(Ocean::Platform::Win::Screen::virtualDisplayWidth());
	const int virtualDisplayHeight = int(Ocean::Platform::Win::Screen::virtualDisplayHeight());

	if ((left != mainWindowInvalidWindowValue && (left >= virtualDisplayWidth || (width != mainWindowInvalidWindowValue && left + width <= 0) || (width == mainWindowInvalidWindowValue && left <= 0)))
			|| (top != mainWindowInvalidWindowValue && (top >= virtualDisplayHeight || (height != mainWindowInvalidWindowValue && top + height <= 0) || (height == mainWindowInvalidWindowValue && top <= 0))))
	{
		flag |= SWP_NOMOVE;
		flag |= SWP_NOSIZE;
	}

	SetWindowPos(nullptr, left, top, width, height, flag);

	if (mainConfig["maximized"](false))
		ShowWindow(SW_SHOWMAXIMIZED);
	else if (mainConfig["minimized"](false))
		ShowWindow(SW_MINIMIZE);

	mainWindowStatusBar.ShowPane((*config)["statusbar"]["visible"](true) && !(*config)["application"]["startfullscreen"](false), false, true);

	mainWindowView.applyConfiguration();
	mainWindowMessageWindow.applyConfiguration();
	mainWindowPropertiesWindow.applyConfiguration();

	if (((*config)["application"]["startfullscreen"](false) || mainConfig["fullscreen"](false)) && mainWindowIsFullscreen == false)
		onToggleFullscreen();

	AdjustDockingLayout();
	UpdateWindow();

	if ((*config)["application"]["loadlastscene"](false))
	{
		const Filenames filenames = Application::convertFilenames((*config)["application"]["lastfilenames"](""));

		if (filenames.empty() == false)
			loadFiles(filenames, true);
	}

	configurationApplied_ = true;
}

void MainWindow::storeConfiguration()
{
	ocean_assert(configurationApplied_);
	mainWindowView.storeConfiguration();
	mainWindowPropertiesWindow.storeConfiguration();
}

MainWindow& MainWindow::mainWindow()
{
	ocean_assert(AfxGetApp() != nullptr);
	ocean_assert(dynamic_cast<MainWindow*>(AfxGetApp()->m_pMainWnd) != nullptr);

	return *(MainWindow*)(AfxGetApp()->m_pMainWnd);
}

ApplicationInterface::EventIds MainWindow::onApplicationInterfaceContentAdd(const ApplicationInterface::StringVector& content)
{
	const ScopedLock scopedLock(mainWindowApplicationEventLock);

	ApplicationInterface::EventIds eventIds;
	eventIds.reserve(content.size());

	for (ApplicationInterface::StringVector::const_iterator i = content.begin(); i != content.end(); ++i)
	{
		mainWindowContentEventIds.push_back(std::make_pair(ContentManager::invalidContentId_, ++contentEventCounter));
		eventIds.push_back(contentEventCounter);
	}

	mainWindowApplicationEvents.push(ApplicationEventCaller::create(*this, &MainWindow::applicationInterfaceContentAdd, content, eventIds));

	return eventIds;
}

ApplicationInterface::EventIds MainWindow::onApplicationInterfaceContentRemove(const ApplicationInterface::StringVector& content)
{
	const ScopedLock scopedLock(mainWindowApplicationEventLock);

	ApplicationInterface::EventIds eventIds;
	eventIds.reserve(content.size());

	for (ApplicationInterface::StringVector::const_iterator i = content.begin(); i != content.end(); ++i)
	{
		mainWindowContentEventIds.push_back(std::make_pair(ContentManager::invalidContentId_, ++contentEventCounter));
		eventIds.push_back(contentEventCounter);
	}

	mainWindowApplicationEvents.push(ApplicationEventCaller::create(*this, &MainWindow::applicationInterfaceContentRemove, content, eventIds));

	return eventIds;
}

void MainWindow::applicationInterfaceContentAdd(const ApplicationInterface::StringVector content, const ApplicationInterface::EventIds eventIds)
{
	const ContentManager::ContentIds contentIds(loadFiles(content, true));
	ocean_assert(contentIds.size() == content.size());

	const ScopedLock scopedLock(mainWindowApplicationEventLock);

	for (size_t n = 0; n < eventIds.size(); ++n)
	{
		bool found = false;

		for (ContentEventIds::iterator i = mainWindowContentEventIds.begin(); i != mainWindowContentEventIds.end(); ++i)
			if (i->second == eventIds[n])
			{
				i->first = contentIds[n];
				found = true;
				break;
			}

		ocean_assert(found);
	}
}

void MainWindow::applicationInterfaceContentRemove(const ApplicationInterface::StringVector content, const ApplicationInterface::EventIds eventIds)
{
	if (content.empty())
	{
		ocean_assert(eventIds.empty());
		ContentManager::get().removeContent();
	}
	else
	{
		const ContentManager::ContentIds contentIds(ContentManager::get().removeContent(content));
		ocean_assert(contentIds.size() == content.size());

		const ScopedLock scopedLock(mainWindowApplicationEventLock);

		for (size_t n = 0; n < eventIds.size(); ++n)
		{
			bool found = false;

			for (ContentEventIds::iterator i = mainWindowContentEventIds.begin(); i != mainWindowContentEventIds.end(); ++i)
				if (i->second == eventIds[n])
				{
					i->first = contentIds[n];
					found = true;
					break;
				}

			ocean_assert(found);
		}
	}
}

void MainWindow::applicationInterfaceContentAdded(const ContentManager::ContentId contentId, const bool state)
{
	if (state)
		Log::info() << "Content loaded successfully.";
	else
		Log::error() << "Failed to load content.";

	if ((*config)["application"]["fittingafterloading"](true))
		mainView().fitSceneToScreen();

	ApplicationInterface::EventId eventId = ApplicationInterface::invalidEventId;

	{
		const ScopedLock scopedLock(mainWindowApplicationEventLock);
		for (ContentEventIds::iterator i = mainWindowContentEventIds.begin(); i != mainWindowContentEventIds.end(); ++i)
			if (i->first == contentId)
			{
				eventId = i->second;
				mainWindowContentEventIds.erase(i);
				break;
			}
	}

	if (eventId != ApplicationInterface::invalidEventId)
		ApplicationInterface::get().contentAdded(eventId, state);
}

void MainWindow::applicationInterfaceContentRemoved(const ContentManager::ContentId contentId, const bool state)
{
	if (state)
		Log::info() << "Content unloaded successfully.";
	else
		Log::error() << "Failed to unload content.";

	ApplicationInterface::EventId eventId = ApplicationInterface::invalidEventId;

	{
		const ScopedLock scopedLock(mainWindowApplicationEventLock);
		for (ContentEventIds::iterator i = mainWindowContentEventIds.begin(); i != mainWindowContentEventIds.end(); ++i)
			if (i->first == contentId)
			{
				eventId = i->second;
				mainWindowContentEventIds.erase(i);
				break;
			}
	}

	if (eventId != ApplicationInterface::invalidEventId)
		ApplicationInterface::get().contentRemoved(eventId, state);
}

}

}

}
