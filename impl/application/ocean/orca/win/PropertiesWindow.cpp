/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/orca/win/PropertiesWindow.h"
#include "application/ocean/orca/win/Application.h"
#include "application/ocean/orca/win/MainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/ConfigMedium.h"
#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/SoundMedium.h"

#include "ocean/rendering/UndistortedBackground.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

BEGIN_MESSAGE_MAP(PropertiesWindow, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_MENUSELECT()
END_MESSAGE_MAP()

PropertiesWindow::PropertyControl::PropertyControl()
{
	// nothing to do here
}

void PropertiesWindow::PropertyControl::setPropertyColumnWidth(const int width)
{
	if (width > 0)
	{
		m_nLeftColumnWidth = width;
	}
}

BOOL PropertiesWindow::PropertyControl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	const unsigned int hwParam = HIWORD(wParam);
	const unsigned int lwParam = LOWORD(wParam);

	if (hwParam == 0u)
	{
		switch (lwParam)
		{
			case PropertiesWindow::backgroundDevicePropertyId_:
			{
				Media::ConfigMediumRef configMedium(View::mainView().backgroundMedium());

				if (configMedium)
				{
					Media::ConfigMedium::ConfigNames configs(configMedium->configs());

					if (configs.size() > 0)
					{
						configMedium->configuration(configs[0], 0ll);
					}
				}

				break;
			}

			case PropertiesWindow::backgroundDeviceFormatId_:
			{
				Media::ConfigMediumRef configMedium(View::mainView().backgroundMedium());

				if (configMedium)
				{
					Media::ConfigMedium::ConfigNames configs(configMedium->configs());

					if (configs.size() > 1)
					{
						configMedium->configuration(configs[1], 0ll);
					}
				}

				break;
			}

			case PropertiesWindow::recorderEncoderMenuId_:
			{
				View::mainView().recorderEncoderConfiguration();
				break;
			}
		}
	}

	return CMFCPropertyGridCtrl::OnCommand(wParam, lParam);
}

PropertiesWindow::PropertyItem::PropertyItem(const EventCallback& callback, const CString& strGroupName, DWORD_PTR dwData, BOOL bIsValueList) :
	CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList),
	eventCallback_(callback)
{
	// nothing to do here
}

PropertiesWindow::PropertyItem::PropertyItem(const EventCallback& callback, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData, LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) :
	CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars),
	eventCallback_(callback)
{
	// nothing to do here
}

PropertiesWindow::PropertyItem::~PropertyItem()
{
	if (menu_)
	{
		DestroyMenu(menu_);
	}
}

void PropertiesWindow::PropertyItem::addPopupMenuEntry(const std::string& entry, const unsigned int id)
{
	ocean_assert(entry.empty() == false);
	menuEntries_.push_back(MenuEntry(entry, id));
}

BOOL PropertiesWindow::PropertyItem::OnEndEdit()
{
	if (GetOptionCount() > 0)
	{
		return CMFCPropertyGridProperty::OnEndEdit();
	}

	if (eventCallback_)
	{
		eventCallback_(this);
	}

	return CMFCPropertyGridProperty::OnEndEdit();
}

void PropertiesWindow::PropertyItem::OnRClickValue(CPoint point, BOOL bSelChanged)
{
	if (menuEntries_.empty() == false)
	{
		if (menu_ == nullptr)
		{
			menu_ = CreatePopupMenu();

			for (size_t n = 0; n < menuEntries_.size(); ++n)
			{
				MENUITEMINFOA info;
				memset(&info, 0, sizeof(MENUITEMINFO));
				info.cbSize = sizeof(MENUITEMINFO);
				info.fMask = MIIM_STRING |  MIIM_ID;
				info.wID = menuEntries_[n].second;
				info.dwTypeData = &menuEntries_[n].first[0];

				InsertMenuItemA(menu_, (unsigned int)(n), TRUE, &info);
			}
		}

		RECT rect;
		::GetWindowRect(m_pWndList->m_hWnd, &rect);

		TrackPopupMenu(menu_, 0, rect.left + point.x, rect.top + point.y, 0, m_pWndList->m_hWnd, 0);
	}

	CMFCPropertyGridProperty::OnRClickValue(point, bSelChanged);
}

void PropertiesWindow::PropertyItem::OnCloseCombo()
{
	if (eventCallback_)
	{
		eventCallback_(this);
	}

	CMFCPropertyGridProperty::OnEndEdit();
}

PropertiesWindow::ColorPropertyItem::ColorPropertyItem(const EventCallback& callback, const CString& strName, const COLORREF& color, CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData),
	eventCallback_(callback)
{
	// nothing to do here
}

BOOL PropertiesWindow::ColorPropertyItem::OnEndEdit()
{
	if (eventCallback_)
	{
		eventCallback_(this);
	}

	return CMFCPropertyGridColorProperty::OnEndEdit();
}

PropertiesWindow::FilePropertyItem::FilePropertyItem(const EventCallback& callback, const CString& strName, const CString& strFolderName, DWORD_PTR dwData, LPCTSTR lpszDescr) :
	CMFCPropertyGridFileProperty(strName, strFolderName, dwData, lpszDescr),
	eventCallback_(callback)
{
	// nothing to do here
}

PropertiesWindow::FilePropertyItem::FilePropertyItem(const EventCallback& callback, const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt, DWORD dwFlags, LPCTSTR lpszFilter, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CMFCPropertyGridFileProperty(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFlags, lpszFilter, lpszDescr, dwData),
	eventCallback_(callback)
{
	// nothing to do here
}

BOOL PropertiesWindow::FilePropertyItem::OnEndEdit()
{
	std::wstring filename(GetValue().bstrVal);

	if (filename != recentFilename_)
	{
		recentFilename_ = filename;

		if (eventCallback_)
		{
			eventCallback_(this);
		}
	}

	return CMFCPropertyGridFileProperty::OnEndEdit();
}

void PropertiesWindow::FilePropertyItem::OnClickButton(CPoint point)
{
	CMFCPropertyGridFileProperty::OnClickButton(point);

	std::wstring filename(GetValue().bstrVal);

	if (filename != recentFilename_)
	{
		recentFilename_ = filename;

		if (eventCallback_)
		{
			eventCallback_(this);
		}
	}
}

void PropertiesWindow::PropertiesToolBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
{
	CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
}

BOOL PropertiesWindow::PropertiesToolBar::AllowShowOnList() const
{
	return FALSE;
}

PropertiesWindow::PropertiesWindow() :
	applicationCallback_(EventCallback(*this, &PropertiesWindow::onApplicationPropertyChanged)),
	viewCallback_(EventCallback(*this, &PropertiesWindow::onViewPropertyChanged)),
	viewDisplayCallback_(EventCallback(*this, &PropertiesWindow::onViewDisplayPropertyChanged)),
	viewBackgroundCallback_(EventCallback(*this, &PropertiesWindow::onViewBackgroundPropertyChanged)),
	viewNavigationCallback_(EventCallback(*this, &PropertiesWindow::onViewNavigationPropertyChanged)),
	viewRendererCallback_(EventCallback(*this, &PropertiesWindow::onViewRendererPropertyChanged)),
	viewRecorderCallback_(EventCallback(*this, &PropertiesWindow::onViewRecorderPropertyChanged)),
	pluginCallback_(EventCallback(*this, &PropertiesWindow::onPluginPropertyChanged))
{
	// nothing to do here
}

PropertiesWindow::~PropertiesWindow()
{
	// nothing to do here
}

void PropertiesWindow::applyConfiguration()
{
	Config::Value& propertiesWindowConfig = Application::get().config()["propertieswindow"];

	int left = propertiesWindowConfig["left"](invalidWindowValue_);
	int top = propertiesWindowConfig["top"](invalidWindowValue_);
	int width = propertiesWindowConfig["width"](invalidWindowValue_);
	int height = propertiesWindowConfig["height"](invalidWindowValue_);

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

	const bool visible = propertiesWindowConfig["visible"](true) && !Application::get().config()["application"]["startfullscreen"](false);

	std::string dockingPosition = propertiesWindowConfig["dockingPosition"]("bottom");
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

	applyProperties();
	ShowPane(visible, false, true);

	propertyList_.UpdateWindow();
	propertyList_.setPropertyColumnWidth(Application::get().config()["propertieswindow"]["propertywidth"](100));
	propertyList_.RedrawWindow();

	configurationApplied_ = true;
}

void PropertiesWindow::storeConfiguration()
{
	ocean_assert(configurationApplied_);

	Application::get().config()["propertieswindow"]["propertywidth"] = propertyList_.GetPropertyColumnWidth();
	Application::get().config()["propertieswindow"]["descriptionheight"] = propertyList_.GetDescriptionHeight();
}

void PropertiesWindow::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	comboBox_.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	comboBox_.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	propertyList_.SetWindowPos(nullptr, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), rectClient.Height() -cyCmb, SWP_NOACTIVATE | SWP_NOZORDER);
}

void PropertiesWindow::SetVSDotNetLook(BOOL bSet)
{
	propertyList_.SetVSDotNetLook(bSet);
	propertyList_.SetGroupNameFullWidth(bSet);
}

int PropertiesWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!comboBox_.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	comboBox_.AddString(_T("Application"));
	comboBox_.AddString(_T("Properties Window"));
	comboBox_.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	comboBox_.SetCurSel(0);

	propertyList_.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2);

	// no column description header line
	propertyList_.EnableHeaderCtrl(FALSE);
	// enable a bottom property description area showing different descriptions for different properties
	propertyList_.EnableDescriptionArea();
	// uses a visual studio look
	propertyList_.SetVSDotNetLook();
	// changed properties will be marked bold
	propertyList_.MarkModifiedProperties();

	addProperties();

	AdjustLayout();
	return 0;
}

void PropertiesWindow::OnShowWindow(BOOL show, UINT status)
{
	CDockablePane::OnShowWindow(show, status);

	if (configurationApplied_ && show == TRUE)
	{
		Application::get().config()["propertieswindow"]["visible"] = true;
	}
}

void PropertiesWindow::OnSize(UINT type, int width, int height)
{
	CDockablePane::OnSize(type, width, height);
	AdjustLayout();

	if (configurationApplied_)
	{
		Application::get().config()["propertieswindow"]["width"] = width;
		Application::get().config()["propertieswindow"]["height"] = height;
	}
}

void PropertiesWindow::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();

	if (configurationApplied_)
	{
		Application::get().config()["propertieswindow"]["visible"] = false;
	}
}

void PropertiesWindow::addProperties()
{
	addApplicationProperties(propertyList_);
	addViewProperties(propertyList_);
	addPluginProperties(propertyList_);
}

void PropertiesWindow::applyProperties()
{
	applyApplicationProperties();
	applyViewProperties();
	applyPluginProperties();
}

void PropertiesWindow::addApplicationProperties(CMFCPropertyGridCtrl& group)
{
	ocean_assert(applicationGroup_ == nullptr);
	applicationGroup_ = new PropertyItem(applicationCallback_, L"Application options");

	ocean_assert(applicationSceneFitting_ == nullptr);
	applicationSceneFitting_ = new PropertyItem(applicationCallback_, L"Fitting after loading", _variant_t(false), L"Specifies whether the entire scene will be fittet into the screen after successfully loading.");
	applicationGroup_->AddSubItem(applicationSceneFitting_);

	ocean_assert(applicationLoadLastScene_ == nullptr);
	applicationLoadLastScene_ = new PropertyItem(applicationCallback_, L"Load last scene", _variant_t(false), L"Specifies whether the last scene(s) will be loaded on orca's startup automatically.");
	applicationGroup_->AddSubItem(applicationLoadLastScene_);

	ocean_assert(applicationStartFullscreen_ == nullptr);
	applicationStartFullscreen_ = new PropertyItem(applicationCallback_, L"Start in fullscreen", _variant_t(false), L"Specifies whether orca will start in fullscreen mode independent how is was closed the last time.");
	applicationGroup_->AddSubItem(applicationStartFullscreen_);

	ocean_assert(applicationDoubleClickFullscreen_ == nullptr);
	applicationDoubleClickFullscreen_ = new PropertyItem(applicationCallback_, L"Dbl click fullscreen", _variant_t(false), L"Specifies whether orca will toggle fullscreen mode on double clicking.");
	applicationGroup_->AddSubItem(applicationDoubleClickFullscreen_);

	ocean_assert(applicationCameraCalibrationFile_ == nullptr);
	applicationCameraCalibrationFile_ = new FilePropertyItem(applicationCallback_, L"Camera calibration file", TRUE, String::toWString(Application::get().config()["application"]["cameracalibrationfile"]("")).c_str(), nullptr, 4 | 2, L"Ocean camera calibration (*.occ)|*.occ", L"Specifies a camera calibration file.");
	applicationGroup_->AddSubItem(applicationCameraCalibrationFile_);

	group.AddProperty(applicationGroup_);
}

void PropertiesWindow::applyApplicationProperties()
{
	Config::Value& application = Application::get().config()["application"];

	ocean_assert(applicationSceneFitting_ != nullptr);
	applicationSceneFitting_->SetValue(_variant_t(application["fittingafterloading"](true)));

	ocean_assert(applicationLoadLastScene_ != nullptr);
	applicationLoadLastScene_->SetValue(_variant_t(application["loadlastscene"](false)));

	ocean_assert(applicationStartFullscreen_ != nullptr);
	applicationStartFullscreen_->SetValue(_variant_t(application["startfullscreen"](false)));

	ocean_assert(applicationDoubleClickFullscreen_ != nullptr);
	applicationDoubleClickFullscreen_->SetValue(_variant_t(application["doubleclickfullscreen"](true)));

	ocean_assert(applicationCameraCalibrationFile_ != nullptr);
}

void PropertiesWindow::addViewProperties(CMFCPropertyGridCtrl& group)
{
	ocean_assert(viewGroup_ == nullptr);
	viewGroup_ = new PropertyItem(viewCallback_, L"View options");

	addViewDisplayProperties(*viewGroup_);
	addViewBackgroundProperties(*viewGroup_);
	addViewNavigationProperties(*viewGroup_);
	addViewRendererProperties(*viewGroup_);
	addViewRecorderProperties(*viewGroup_);

	group.AddProperty(viewGroup_);
}

void PropertiesWindow::applyViewProperties()
{
	applyViewRendererProperties();
	applyViewDisplayProperties();
	applyViewBackgroundProperties();
	applyViewNavigationProperties();
	applyViewRecorderProperties();
}

void PropertiesWindow::addViewDisplayProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewDisplayGroup_ == nullptr);
	viewDisplayGroup_ = new PropertyItem(viewDisplayCallback_, L"Display");

	ocean_assert(viewDisplayFieldOfView_ == nullptr);
	viewDisplayFieldOfView_ = new PropertyItem(viewDisplayCallback_, L"Field of view", _variant_t(0.0), L"Specifies the horizontal field of view use for scene rendering. The value is specified in degree.");
	viewDisplayGroup_->AddSubItem(viewDisplayFieldOfView_);

	ocean_assert(viewDisplayNearDistance_ == nullptr);
	viewDisplayNearDistance_ = new PropertyItem(viewDisplayCallback_, L"Near distance", _variant_t(0.0), L"Specifies the distance to the camera's near clipping plane. Objects will be visible if they are inbetween near and far distance.");
	viewDisplayGroup_->AddSubItem(viewDisplayNearDistance_);

	ocean_assert(viewDisplayFarDistance_ == nullptr);
	viewDisplayFarDistance_ = new PropertyItem(viewDisplayCallback_, L"Far distance", _variant_t(0.0), L"Specifies the distance to the display's far clipping plane. Objects will be visible if they are inbetween near and far distance.");
	viewDisplayGroup_->AddSubItem(viewDisplayFarDistance_);

	ocean_assert(viewDisplayFocus_ == nullptr);
	viewDisplayFocus_ = new PropertyItem(viewDisplayCallback_, L"Focus", _variant_t(0.0), L"Specifies the focus distance of the projection plane.");
	viewDisplayGroup_->AddSubItem(viewDisplayFocus_);

	ocean_assert(viewDisplayFramerate_ == nullptr);
	viewDisplayFramerate_ = new PropertyItem(viewDisplayCallback_, L"Framerate", _variant_t(0.0f), L"Specifies the preferred display framerate in Hz. However, depending on the scene and platform the framerate can be lower than specified.");
	viewDisplayGroup_->AddSubItem(viewDisplayFramerate_);

	ocean_assert(viewDisplayType_ == nullptr);
	viewDisplayType_ = new PropertyItem(viewDisplayCallback_, L"Type", _variant_t(L"Mono view"), L"Specifies which view type is used for rendering.");
	viewDisplayType_->AddOption(L"Mono view");
	viewDisplayType_->AddOption(L"Parallel view");
	viewDisplayType_->AddOption(L"Quadbuffered stereo view");
	viewDisplayType_->AddOption(L"Anaglyph stereo view");
	viewDisplayType_->AllowEdit(FALSE);
	viewDisplayGroup_->AddSubItem(viewDisplayType_);

	ocean_assert(viewDisplayEyesReversed_ == nullptr);
	viewDisplayEyesReversed_ = new PropertyItem(viewDisplayCallback_, L"Reverse eyes", _variant_t(false), L"Specifies whether left and right image will be reversed for stereo views.");
	viewDisplayGroup_->AddSubItem(viewDisplayEyesReversed_);

	ocean_assert(viewDisplayHeadlight_ == nullptr);
	viewDisplayHeadlight_ = new PropertyItem(viewDisplayCallback_, L"Use headlight", _variant_t(true), L"Specifies whether the headlight is enabled for this view.");
	viewDisplayGroup_->AddSubItem(viewDisplayHeadlight_);

	ocean_assert(viewDisplayPhantomMode_ == nullptr);
	viewDisplayPhantomMode_ = new PropertyItem(viewDisplayCallback_, L"Phantom mode", _variant_t(L"Default"), L"Specifies which phantom display mode the engine uses for this view.");
	viewDisplayPhantomMode_->AddOption(L"Default");
	viewDisplayPhantomMode_->AddOption(L"Color");
	viewDisplayPhantomMode_->AddOption(L"Debug");
	viewDisplayPhantomMode_->AddOption(L"Video");
	viewDisplayPhantomMode_->AllowEdit(FALSE);
	viewDisplayGroup_->AddSubItem(viewDisplayPhantomMode_);

	group.AddSubItem(viewDisplayGroup_);
}

void PropertiesWindow::applyViewDisplayProperties()
{
	Config::Value& display = Application::get().config()["view"]["display"];

	ocean_assert(viewDisplayFieldOfView_ != nullptr);
	View::mainView().setHorizontalFieldOfView(NumericD::deg2rad(display["fovx"](45.0)));
	viewDisplayFieldOfView_->SetValue(_variant_t(NumericD::rad2deg(View::mainView().horizontalFieldOfView())));

	ocean_assert(viewDisplayNearDistance_ != nullptr);
	View::mainView().setNearDistance(display["near"](0.01));
	viewDisplayNearDistance_->SetValue(_variant_t(View::mainView().nearDistance()));

	ocean_assert(viewDisplayFarDistance_ != nullptr);
	View::mainView().setFarDistance(display["far"](10000.0));
	viewDisplayFarDistance_->SetValue(_variant_t(View::mainView().farDistance()));

	ocean_assert(viewDisplayFocus_ != nullptr);
	View::mainView().setFocusDistance(display["focus"](1.0));
	viewDisplayFocus_->SetValue(_variant_t(View::mainView().focusDistance()));

	ocean_assert(viewDisplayFramerate_ != nullptr);
	View::mainView().setPreferredFramerate(float(display["framerate"](60.0)));
	viewDisplayFramerate_->SetValue(_variant_t(View::mainView().preferredFramerate()));

	ocean_assert(viewDisplayType_ != nullptr);
	View::ViewType type = View::TYPE_MONO_VIEW;
	std::string typeString = display["type"]("Mono view");
	if (typeString == "Mono view")
	{
		type = View::TYPE_MONO_VIEW;
	}
	else if (typeString == "Parallel view")
	{
		type = View::TYPE_PARALLEL_VIEW;
	}
	else if (typeString == "Quadbuffered stereo view")
	{
		type = View::TYPE_STEREO_VIEW_QUADBUFFERED;
	}
	else if (typeString == "Anaglyph stereo view")
	{
		type = View::TYPE_STEREO_VIEW_ANAGLYPH;
	}
	else
	{
		typeString = "Mono view";
	}
	viewDisplayType_->SetValue(_variant_t(String::toWString(typeString).c_str()));
	View::mainView().setType(type);

	ocean_assert(viewDisplayEyesReversed_ != nullptr);
	View::mainView().setEyesReversed(display["eyesreversed"](false));
	viewDisplayEyesReversed_->SetValue(_variant_t(View::mainView().eyesReversed()));

	ocean_assert(viewDisplayHeadlight_ != nullptr);
	View::mainView().setUseHeadlight(display["useheadlight"](true));
	viewDisplayHeadlight_->SetValue(_variant_t(View::mainView().useHeadlight()));

	ocean_assert(viewDisplayPhantomMode_ != nullptr);
	std::string phantomModeString = display["phantommode"]("Default");
	Rendering::Framebuffer::PhantomMode phantomMode = Rendering::PhantomAttribute::PM_DEFAULT;
	if (phantomModeString == "Color")
	{
		phantomMode = Rendering::PhantomAttribute::PM_COLOR;
	}
	else if (phantomModeString == "Debug")
	{
		phantomMode = Rendering::PhantomAttribute::PM_DEBUG;
	}
	else if (phantomModeString == "Video")
	{
		phantomMode = Rendering::PhantomAttribute::PM_VIDEO;
	}
	View::mainView().setPhantomMode(phantomMode);
	viewDisplayPhantomMode_->SetValue(_variant_t(String::toWString(phantomModeString).c_str()));
}

void PropertiesWindow::addViewBackgroundProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewBackgroundGroup_ == nullptr);
	viewBackgroundGroup_ = new PropertyItem(viewBackgroundCallback_, L"Background");

	ocean_assert(viewBackgroundColor_ == nullptr);
	viewBackgroundColor_ = new ColorPropertyItem(viewBackgroundCallback_, L"Color", 0, nullptr, L"Specifies the background color used for rendering.");
	viewBackgroundColor_->EnableOtherButton(L"Other...");
	viewBackgroundColor_->EnableAutomaticButton(L"Default", 0);
	viewBackgroundGroup_->AddSubItem(viewBackgroundColor_);

	ocean_assert(viewBackgroundType_ == nullptr);
	viewBackgroundType_ = new PropertyItem(viewBackgroundCallback_, L"Type", L"None", L"Specified the background type in addition to the background color used for rendering.");
	viewBackgroundType_->AddOption(L"None");
	viewBackgroundType_->AddOption(L"Device background");
	viewBackgroundType_->AddOption(L"Media background");
	viewBackgroundType_->AllowEdit(FALSE);
	viewBackgroundGroup_->AddSubItem(viewBackgroundType_);

	group.AddSubItem(viewBackgroundGroup_);
}

void PropertiesWindow::applyViewBackgroundProperties()
{
	Config::Value& background = Application::get().config()["view"]["background"];

	ocean_assert(viewBackgroundColor_ != nullptr);
	View::mainView().setBackgroundColor(background["color"](0));
	viewBackgroundColor_->SetColor(View::mainView().backgroundColor());

	ocean_assert(viewBackgroundType_ != nullptr);
	const std::wstring type(String::toWString(background["type"]("None")));
	for (int n = 0; n < viewBackgroundType_->GetOptionCount(); n++)
	{
		if (type == viewBackgroundType_->GetOption(n))
		{
			viewBackgroundType_->SetValue(_variant_t(type.c_str()));

			addViewBackgroundProperties(n, *viewBackgroundGroup_);
			applyViewBackgroundProperties(n);
		}
	}
}

void PropertiesWindow::addViewBackgroundProperties(const int index, CMFCPropertyGridProperty& group)
{
	removeViewBackgroundTypeProperties();

	if (index > 0)
	{
		addViewBackgroundDisplayTypeProperty(group);
	}

	switch (index)
	{
		case 1:
			addViewBackgroundDeviceProperties(group);
			break;

		case 2:
			addViewBackgroundMediaProperties(group);
			break;
	}

	group.Expand(FALSE);
	group.Expand(TRUE);
}

void PropertiesWindow::applyViewBackgroundProperties(const int index)
{
	switch (index)
	{
		case 1:
			applyViewBackgroundDeviceProperties();
			break;

		case 2:
			applyViewBackgroundMediaProperties();
			break;
	}

	if (index > 0)
	{
		applyViewBackgroundDisplayTypeProperty();
	}
}

void PropertiesWindow::removeViewBackgroundTypeProperties()
{
	ocean_assert(viewBackgroundGroup_ != nullptr);

	removeViewBackgroundDisplayTypeProperty();
	removeViewBackgroundDeviceProperties();
	removeViewBackgroundMediaProperties();

	View::mainView().setBackgroundMedium(Media::FrameMediumRef());
}

void PropertiesWindow::removeViewBackgroundDeviceProperties()
{
	if (viewBackgroundDeviceName_)
	{
		CMFCPropertyGridProperty* object = viewBackgroundDeviceName_;
		viewBackgroundGroup_->RemoveSubItem(object);
		viewBackgroundDeviceName_ = nullptr;
	}
}

void PropertiesWindow::removeViewBackgroundMediaProperties()
{
	if (viewBackgroundMediaFile_)
	{
		CMFCPropertyGridProperty* object = viewBackgroundMediaFile_;
		viewBackgroundGroup_->RemoveSubItem(object);
		viewBackgroundMediaFile_ = nullptr;
	}

	if (viewBackgroundMediaLoop_)
	{
		CMFCPropertyGridProperty* object = viewBackgroundMediaLoop_;
		viewBackgroundGroup_->RemoveSubItem(object);
		viewBackgroundMediaLoop_ = nullptr;
	}

	if (viewBackgroundMediaSound_)
	{
		CMFCPropertyGridProperty* object = viewBackgroundMediaSound_;
		viewBackgroundGroup_->RemoveSubItem(object);
		viewBackgroundMediaSound_ = nullptr;
	}
}

void PropertiesWindow::removeViewBackgroundDisplayTypeProperty()
{
	if (viewBackgroundDisplayType_)
	{
		CMFCPropertyGridProperty* object = viewBackgroundDisplayType_;
		viewBackgroundGroup_->RemoveSubItem(object);
		viewBackgroundDisplayType_ = nullptr;
	}
}

void PropertiesWindow::addViewBackgroundDisplayTypeProperty(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewBackgroundDisplayType_ == nullptr);
	viewBackgroundDisplayType_ = new PropertyItem(viewBackgroundCallback_, L"Display type", L"Fastest", L"Specifies the display type of the selected background type.");
	viewBackgroundDisplayType_->AllowEdit(FALSE);

	viewBackgroundDisplayType_->AddOption(L"Fastest");
	viewBackgroundDisplayType_->AddOption(L"Undistorted");

	group.AddSubItem(viewBackgroundDisplayType_);
}

void PropertiesWindow::applyViewBackgroundDisplayTypeProperty()
{
	Config::Value& background = Application::get().config()["view"]["background"];

	ocean_assert(viewBackgroundDisplayType_ != nullptr);
	std::string typeString = background["displaytype"]("Fastest");

	Rendering::UndistortedBackground::DisplayType type = Rendering::UndistortedBackground::DT_FASTEST;

	if (typeString == "Undistorted")
	{
		type = Rendering::UndistortedBackground::DT_UNDISTORTED;
	}
	else
	{
		typeString = "Fastest";
	}

	View::mainView().setBackgroundDisplayType(type);
	viewBackgroundDisplayType_->SetValue(_variant_t(String::toWString(typeString).c_str()));
}

void PropertiesWindow::addViewBackgroundDeviceProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewBackgroundDeviceName_ == nullptr);
	viewBackgroundDeviceName_ = new PropertyItem(viewBackgroundCallback_, L"Device name", L"None", L"Specifies the name of the selected video device used for background rendering.");
	viewBackgroundDeviceName_->AddOption(L"None");

	Media::Library::Definitions definitions(Media::Manager::get().selectableMedia(Media::Medium::LIVE_VIDEO));
	for (unsigned int n = 0; n < definitions.size(); ++n)
	{
		std::wstring url = String::toWString(definitions[n].url());
		viewBackgroundDeviceName_->AddOption(url.c_str());
	}
	viewBackgroundDeviceName_->AllowEdit(FALSE);

	viewBackgroundDeviceName_->addPopupMenuEntry("Format", backgroundDeviceFormatId_);
	viewBackgroundDeviceName_->addPopupMenuEntry("Property", backgroundDevicePropertyId_);

	group.AddSubItem(viewBackgroundDeviceName_);
}

void PropertiesWindow::applyViewBackgroundDeviceProperties()
{
	Config::Value& device = Application::get().config()["view"]["background"]["device"];

	ocean_assert(viewBackgroundDeviceName_ != nullptr);
	const std::wstring name = String::toWString(device["name"](""));

	Media::FrameMediumRef medium;

	if (!name.empty() && name != L"None")
	{
		medium = Media::Manager::get().newMedium(String::toAString(name), Media::Medium::LIVE_VIDEO, true);

		if (medium)
		{
			viewBackgroundDeviceName_->SetValue(_variant_t(medium->url().c_str()));

			const int width = device["preferredwidth"](0);
			const int height = device["preferredheight"](0);

			if (width > 0 && height > 0)
			{
				medium->setPreferredFrameDimension((unsigned int)width, (unsigned int)height);
			}
		}
	}

	View::mainView().setBackgroundMedium(medium);
}

void PropertiesWindow::addViewBackgroundMediaProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewBackgroundMediaFile_ == nullptr);
	viewBackgroundMediaFile_ = new FilePropertyItem(viewBackgroundCallback_, L"File", TRUE, L"N/A", nullptr, 4 | 2, nullptr, L"Specifies the filename of the medium used for background rendering.");
	group.AddSubItem(viewBackgroundMediaFile_);

	ocean_assert(viewBackgroundMediaLoop_ == nullptr);
	viewBackgroundMediaLoop_ = new PropertyItem(viewBackgroundCallback_, L"Loop", _variant_t(false), L"Specifies whether the media file will be played in a loop.");
	group.AddSubItem(viewBackgroundMediaLoop_);

	ocean_assert(viewBackgroundMediaSound_ == nullptr);
	viewBackgroundMediaSound_ = new PropertyItem(viewBackgroundCallback_, L"Sound", _variant_t(false), L"Specifies whether the sound of the media file will be played.");
	group.AddSubItem(viewBackgroundMediaSound_);
}

void PropertiesWindow::applyViewBackgroundMediaProperties()
{
	Config::Value& media = Application::get().config()["view"]["background"]["media"];

	ocean_assert(viewBackgroundMediaFile_ != nullptr);
	Media::FrameMediumRef medium;

	std::string file = media["file"]("");
	if (file.empty() == false)
	{
		medium = Media::Manager::get().newMedium(file, true);
	}

	if (medium.isNull())
	{
		file.clear();
	}

	View::mainView().setBackgroundMedium(medium);
	viewBackgroundMediaFile_->SetValue(_variant_t(String::toWString(file).c_str()));

	ocean_assert(viewBackgroundMediaLoop_ != nullptr);
	const bool loop = media["loop"](true);
	Media::FiniteMediumRef finiteMedium(medium);
	if (finiteMedium)
	{
		finiteMedium->setLoop(loop);
	}
	viewBackgroundMediaLoop_->SetValue(_variant_t(loop));

	ocean_assert(viewBackgroundMediaSound_ != nullptr);
	const bool sound = media["sound"](true);
	Media::SoundMediumRef soundMedium(medium);
	if (soundMedium)
	{
		soundMedium->setSoundMute(!sound);
	}
	viewBackgroundMediaSound_->SetValue(_variant_t(sound));
}

void PropertiesWindow::addViewNavigationProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewNavigationGroup_ == nullptr);
	viewNavigationGroup_ = new PropertyItem(viewNavigationCallback_, L"Navigation");

	ocean_assert(viewNavigationCursorMode_ == nullptr);
	viewNavigationCursorMode_ = new PropertyItem(viewNavigationCallback_, L"Cursor fullscreen mode", _variant_t("Hide while inactive"), L"Specifies the behavior of the cursor in fullscreen mode.");
	viewNavigationCursorMode_->AddOption(L"Show always");
	viewNavigationCursorMode_->AddOption(L"Hide while inactive");
	viewNavigationCursorMode_->AddOption(L"Hide always");
	viewNavigationCursorMode_->AllowEdit(FALSE);
	viewNavigationGroup_->AddSubItem(viewNavigationCursorMode_);

	assert (viewNavigationStorePosition_ == nullptr);
	viewNavigationStorePosition_ = new PropertyItem(viewNavigationCallback_, L"Store position", _variant_t(false), L"Specifies whether the last viewing position is stored and recovered at the next restart.");
	viewNavigationGroup_->AddSubItem(viewNavigationStorePosition_);

	group.AddSubItem(viewNavigationGroup_);
}

void PropertiesWindow::applyViewNavigationProperties()
{
	Config::Value& navigation = Application::get().config()["view"]["navigation"];

	ocean_assert(viewNavigationCursorMode_ != nullptr);
	std::string cursorModeString = navigation["cursorfullscreenmode"]("Hide while inactive");
	View::CursorMode cursorMode = View::CM_HIDE_INACTIVITY;
	if (cursorModeString == "Show always")
	{
		cursorMode = View::CM_VISIBLE;
	}
	else if (cursorModeString == "Hide always")
	{
		cursorMode = View::CM_HIDE_ALWAYS;
	}
	else
	{
		cursorModeString = "Hide while inactive";
	}
	View::mainView().setCursorMode(cursorMode);
	viewNavigationCursorMode_->SetValue(_variant_t(String::toWString(cursorModeString).c_str()));

	ocean_assert(viewNavigationStorePosition_ != nullptr);
	viewNavigationStorePosition_->SetValue(_variant_t(navigation["storeposition"](false)));
}

void PropertiesWindow::addViewRendererProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewRendererGroup_ == nullptr);
	viewRendererGroup_ = new PropertyItem(viewRendererCallback_, L"Renderer");

	ocean_assert(viewRendererName_ == nullptr);
	viewRendererName_ = new PropertyItem(viewRendererCallback_, L"Renderer", _variant_t(L"Default"), L"Specifies which engine will be used for rendering.");
	viewRendererGroup_->AddSubItem(viewRendererName_);
	updateViewRenderer();

	ocean_assert(viewRendererAPI_ == nullptr);
	viewRendererAPI_ = new PropertyItem(viewRendererCallback_, L"API", _variant_t(L"Default"), L"Specifies which type of graphic API is used by the renderer for frame creation.");
	viewRendererAPI_->AddOption(L"Default");
	viewRendererAPI_->AllowEdit(FALSE);
	viewRendererGroup_->AddSubItem(viewRendererAPI_);

	ocean_assert(viewRendererFaceMode_ == nullptr);
	viewRendererFaceMode_ = new PropertyItem(viewRendererCallback_, L"Face mode", _variant_t(L"Default"), L"Specifies which face mode the engine uses for rendering.");
	viewRendererFaceMode_->AddOption(L"Default");
	viewRendererFaceMode_->AddOption(L"Face");
	viewRendererFaceMode_->AddOption(L"Line");
	viewRendererFaceMode_->AddOption(L"Point");
	viewRendererFaceMode_->AllowEdit(FALSE);
	viewRendererGroup_->AddSubItem(viewRendererFaceMode_);

	ocean_assert(viewRendererCullingMode_ == nullptr);
	viewRendererCullingMode_ = new PropertyItem(viewRendererCallback_, L"Culling mode", _variant_t(L"Default"), L"Specifies which culling mode the engine uses for rendering.");
	viewRendererCullingMode_->AddOption(L"Default");
	viewRendererCullingMode_->AddOption(L"Back");
	viewRendererCullingMode_->AddOption(L"Both");
	viewRendererCullingMode_->AddOption(L"Front");
	viewRendererCullingMode_->AddOption(L"None");
	viewRendererCullingMode_->AllowEdit(FALSE);
	viewRendererGroup_->AddSubItem(viewRendererCullingMode_);

	ocean_assert(viewRendererTechnique_ == nullptr);
	viewRendererTechnique_ = new PropertyItem(viewRendererCallback_, L"Technique", _variant_t(L"Full"), L"Specifies which shading technique the engine uses for rendering.");
	viewRendererTechnique_->AddOption(L"Full");
	viewRendererTechnique_->AddOption(L"Textured");
	viewRendererTechnique_->AddOption(L"Shaded");
	viewRendererTechnique_->AddOption(L"Unlit");
	viewRendererTechnique_->AllowEdit(FALSE);
	viewRendererGroup_->AddSubItem(viewRendererTechnique_);

	ocean_assert(viewRendererSupportQuadbufferedStereo_ == nullptr);
	viewRendererSupportQuadbufferedStereo_ = new PropertyItem(viewRendererCallback_, L"Support quadbuffered stereo", _variant_t(false), L"Specifies whether quadbuffered stereo rendering is supported for this view.");
	viewRendererGroup_->AddSubItem(viewRendererSupportQuadbufferedStereo_);

	ocean_assert(viewRendererSupportAntialiasing_ == nullptr);
	viewRendererSupportAntialiasing_ = new PropertyItem(viewRendererCallback_, L"Support antialiasing", _variant_t(L"None"), L"Specifies whether antialiasing is supported for this view.");
	viewRendererSupportAntialiasing_->AddOption(L"None");
	viewRendererSupportAntialiasing_->AllowEdit(FALSE);
	viewRendererGroup_->AddSubItem(viewRendererSupportAntialiasing_);

	ocean_assert(viewRendererEnableAntialiasing_ == nullptr);
	viewRendererEnableAntialiasing_ = new PropertyItem(viewRendererCallback_, L"Enable antialiasing", _variant_t(false), L"Specifies whether antialiasing is enabled for this view.");
	viewRendererGroup_->AddSubItem(viewRendererEnableAntialiasing_);

	group.AddSubItem(viewRendererGroup_);
}

void PropertiesWindow::updateViewRenderer()
{
	ocean_assert(viewRendererName_ != nullptr);
	viewRendererName_->RemoveAllOptions();

	viewRendererName_->AddOption(L"None");
	viewRendererName_->AddOption(L"Default");
	Rendering::Manager::EngineNames engines(Rendering::Manager::get().engines());
	for (unsigned int n = 0; n < engines.size(); ++n)
	{
		viewRendererName_->AddOption(String::toWString(engines[n]).c_str());
	}
	viewRendererName_->AllowEdit(FALSE);
}

void PropertiesWindow::applyViewRendererProperties()
{
	Config::Value& renderer = Application::get().config()["view"]["renderer"];

	ocean_assert(viewRendererSupportQuadbufferedStereo_ != nullptr);
	const bool supportQuadbufferedStereo = renderer["supportquadbufferedstereo"](false);
	viewRendererSupportQuadbufferedStereo_->SetValue(_variant_t(supportQuadbufferedStereo));
	View::mainView().setSupportQuadbufferedStereo(supportQuadbufferedStereo);

	ocean_assert(viewRendererSupportAntialiasing_ != nullptr);
	std::string supportAntialiasingString = renderer["supportantialiasing"]("None");
	unsigned int antialiasingBuffers = 0;
	if (!supportAntialiasingString.empty())
	{
		antialiasingBuffers = atoi(supportAntialiasingString.c_str());
		supportAntialiasingString = String::toAString(antialiasingBuffers) + std::string("x");
	}
	View::mainView().setSupportAntialiasing(antialiasingBuffers);

	ocean_assert(viewRendererAPI_ != nullptr);
	viewRendererAPI_->RemoveAllOptions();
	const Rendering::Engine::GraphicAPI supportedGraphicAPI = Rendering::Manager::get().supportedGraphicAPI(renderer["name"](""));
	viewRendererAPI_->AddOption(L"Default");
	if ((supportedGraphicAPI & Rendering::Engine::API_DIRECTX) == Rendering::Engine::API_DIRECTX)
		viewRendererAPI_->AddOption(L"DirectX");
	if ((supportedGraphicAPI & Rendering::Engine::API_OPENGL) == Rendering::Engine::API_OPENGL)
		viewRendererAPI_->AddOption(L"OpenGL");
	if ((supportedGraphicAPI & Rendering::Engine::API_OPENGLES) == Rendering::Engine::API_OPENGLES)
		viewRendererAPI_->AddOption(L"OpenGL ES");
	if ((supportedGraphicAPI & Rendering::Engine::API_RAYTRACER) == Rendering::Engine::API_RAYTRACER)
		viewRendererAPI_->AddOption(L"Raytracer");

	std::string graphicAPIString = renderer["graphicapi"]("Default");
	Rendering::Engine::GraphicAPI graphicAPI = Rendering::Engine::API_DEFAULT;
	if (graphicAPIString == "OpenGL")
		graphicAPI = Rendering::Engine::API_OPENGL;
	else if (graphicAPIString == "DirectX")
		graphicAPI = Rendering::Engine::API_DIRECTX;
	else if (graphicAPIString == "OpenGL ES")
		graphicAPI = Rendering::Engine::API_OPENGLES;
	else if (graphicAPIString == "Raytracer")
		graphicAPI = Rendering::Engine::API_RAYTRACER;
	else
		graphicAPIString = "Default";

	ocean_assert(viewRendererName_ != nullptr);
	std::wstring preferredRenderer(String::toWString(renderer["name"]("Default")));
	for (int n = 0; n < viewRendererName_->GetOptionCount(); ++n)
	{
		if (preferredRenderer == viewRendererName_->GetOption(n))
		{
			viewRendererName_->SetValue(_variant_t(preferredRenderer.c_str()));
			viewRendererAPI_->SetValue(_variant_t(String::toWString(graphicAPIString).c_str()));

			View::mainView().setRenderer(String::toAString(preferredRenderer), graphicAPI);
		}
	}

	// set available anti-aliasing modes
	for (unsigned int n = 2; n <= 128; n *= 2)
		if (View::mainView().supportsAntialiasing(n))
			viewRendererSupportAntialiasing_->AddOption((String::toWString(n) + std::wstring(L"x")).c_str());
	viewRendererSupportAntialiasing_->SetValue(_variant_t(String::toWString(supportAntialiasingString).c_str()));

	ocean_assert(viewRendererFaceMode_ != nullptr);
	std::string faceModeString = renderer["facemode"]("Default");
	Rendering::Framebuffer::FaceMode faceMode = Rendering::PrimitiveAttribute::MODE_DEFAULT;
	if (faceModeString == "Face")
		faceMode = Rendering::PrimitiveAttribute::MODE_FACE;
	else if (faceModeString == "Line")
		faceMode = Rendering::PrimitiveAttribute::MODE_LINE;
	else if (faceModeString == "Point")
		faceMode = Rendering::PrimitiveAttribute::MODE_POINT;
	View::mainView().setRendererFaceMode(faceMode);
	viewRendererFaceMode_->SetValue(_variant_t(String::toWString(faceModeString).c_str()));

	ocean_assert(viewRendererCullingMode_ != nullptr);
	std::string cullingModeString = renderer["cullingmode"]("Default");
	Rendering::Framebuffer::CullingMode cullingMode = Rendering::PrimitiveAttribute::CULLING_DEFAULT;
	if (cullingModeString == "Back")
		cullingMode = Rendering::PrimitiveAttribute::CULLING_BACK;
	else if (cullingModeString == "Both")
		cullingMode = Rendering::PrimitiveAttribute::CULLING_BOTH;
	else if (cullingModeString == "Front")
		cullingMode = Rendering::PrimitiveAttribute::CULLING_FRONT;
	else if (cullingModeString == "None")
		cullingMode = Rendering::PrimitiveAttribute::CULLING_NONE;
	View::mainView().setRendererCullingMode(cullingMode);
	viewRendererCullingMode_->SetValue(_variant_t(String::toWString(cullingModeString).c_str()));

	ocean_assert(viewRendererTechnique_ != nullptr);
	std::string techniqueString = renderer["technique"]("Full");
	Rendering::Framebuffer::RenderTechnique technique = Rendering::Framebuffer::TECHNIQUE_FULL;
	if (techniqueString == "Textured")
		technique = Rendering::Framebuffer::TECHNIQUE_TEXTURED;
	else if (techniqueString == "Shaded")
		technique = Rendering::Framebuffer::TECHNIQUE_SHADED;
	else if (techniqueString == "Unlit")
		technique = Rendering::Framebuffer::TECHNIQUE_UNLIT;
	View::mainView().setRendererTechnique(technique);
	viewRendererTechnique_->SetValue(_variant_t(String::toWString(techniqueString).c_str()));

	ocean_assert(viewRendererEnableAntialiasing_ != nullptr);
	const bool enableAntialiasing = renderer["enableantialiasing"](false);
	viewRendererEnableAntialiasing_->SetValue(_variant_t(enableAntialiasing));
	View::mainView().setAntialiasing(enableAntialiasing);
}

void PropertiesWindow::addViewRecorderProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewRecorderGroup_ == nullptr);
	viewRecorderGroup_ = new PropertyItem(viewRecorderCallback_, L"Recorder");

	ocean_assert(viewRecorderType_ == nullptr);
	viewRecorderType_ = new PropertyItem(viewRecorderCallback_, L"Type", _variant_t(L"None"), L"Specifies which type of recorder will be used.");
	viewRecorderType_->AddOption(L"None");
	viewRecorderType_->AddOption(L"Image Recorder");
	viewRecorderType_->AddOption(L"Image Sequence Recorder");
	viewRecorderType_->AddOption(L"Movie Recorder");
	viewRecorderType_->AddOption(L"Memory Recorder");
	viewRecorderType_->AllowEdit(FALSE);
	viewRecorderGroup_->AddSubItem(viewRecorderType_);

	group.AddSubItem(viewRecorderGroup_);
}

void PropertiesWindow::addViewRecorderMovieProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewMovieRecorderEncoder_ == nullptr);
	viewMovieRecorderEncoder_ = new PropertyItem(viewRecorderCallback_, L"Encoder", _variant_t(L"None"), L"Specifies the movie frame encoder used for compression.");
	viewMovieRecorderEncoder_->AllowEdit(FALSE);
	viewMovieRecorderEncoder_->addPopupMenuEntry("Configuration", recorderEncoderMenuId_);
	group.AddSubItem(viewMovieRecorderEncoder_);

	ocean_assert(viewMovieRecorderFilename_ == nullptr);
	viewMovieRecorderFilename_ = new FilePropertyItem(viewRecorderCallback_, L"Filename", FALSE, L"", nullptr, 4 | 2, nullptr, L"Specifies the recorder output filename.");
	group.AddSubItem(viewMovieRecorderFilename_);

	ocean_assert(viewMovieRecorderFilenameSuffix_ == nullptr);
	viewMovieRecorderFilenameSuffix_ = new PropertyItem(viewRecorderCallback_, L"Filename extension", _variant_t(true), L"Specifies whether the recorder filename will be extended with current date and time.");
	group.AddSubItem(viewMovieRecorderFilenameSuffix_);

	ocean_assert(viewMovieRecorderFps_ == nullptr);
	viewMovieRecorderFps_ = new PropertyItem(viewRecorderCallback_, L"Preferred fps", _variant_t(25.0), L"Specifies the preferred frame rate (in Hz) the recorder provides.");
	group.AddSubItem(viewMovieRecorderFps_);

	group.Expand(FALSE);
	group.Expand(TRUE);
}

void PropertiesWindow::addViewRecorderMemoryProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewMemoryRecorderFps_ == nullptr);
	viewMemoryRecorderFps_ = new PropertyItem(viewRecorderCallback_, L"Preferred fps", _variant_t(25.0f), L"Specifies the preferred frame rate (in Hz) the recorder provides.");
	group.AddSubItem(viewMemoryRecorderFps_);

	group.Expand(FALSE);
	group.Expand(TRUE);
}

void PropertiesWindow::addViewRecorderImageProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewImageRecorderFilename_ == nullptr);
	viewImageRecorderFilename_ = new FilePropertyItem(viewRecorderCallback_, L"Filename", FALSE, L"", nullptr, 4 | 2, nullptr, L"Specifies the recorder output filename.");
	group.AddSubItem(viewImageRecorderFilename_);

	ocean_assert(viewImageRecorderFilenameSuffix_ == nullptr);
	viewImageRecorderFilenameSuffix_ = new PropertyItem(viewRecorderCallback_, L"Filename extension", _variant_t(true), L"Specifies whether the recorder filename will be extended with current date and time.");
	group.AddSubItem(viewImageRecorderFilenameSuffix_);

	group.Expand(FALSE);
	group.Expand(TRUE);
}

void PropertiesWindow::addViewRecorderImageSequenceProperties(CMFCPropertyGridProperty& group)
{
	ocean_assert(viewImageSequenceRecorderFilename_ == nullptr);
	viewImageSequenceRecorderFilename_ = new FilePropertyItem(viewRecorderCallback_, L"Filename", FALSE, L"", nullptr, 4 | 2, nullptr, L"Specifies the recorder output filename.");
	group.AddSubItem(viewImageSequenceRecorderFilename_);

	ocean_assert(viewImageSequenceRecorderFilenameSuffix_ == nullptr);
	viewImageSequenceRecorderFilenameSuffix_ = new PropertyItem(viewRecorderCallback_, L"Filename extension", _variant_t(true), L"Specifies whether the recorder filename will be extended with current date and time.");
	group.AddSubItem(viewImageSequenceRecorderFilenameSuffix_);

	ocean_assert(viewImageSequenceRecorderFps_ == nullptr);
	viewImageSequenceRecorderFps_ = new PropertyItem(viewRecorderCallback_, L"Preferred fps", _variant_t(25.0f), L"Specifies the preferred frame rate (in Hz) the recorder provides.");
	group.AddSubItem(viewImageSequenceRecorderFps_);

	group.Expand(FALSE);
	group.Expand(TRUE);
}

void PropertiesWindow::removeViewRecorderMovieProperties()
{
	ocean_assert(viewRecorderGroup_ != nullptr);

	if (viewMovieRecorderEncoder_)
	{
		CMFCPropertyGridProperty* object = viewMovieRecorderEncoder_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewMovieRecorderEncoder_ = nullptr;
	}

	if (viewMovieRecorderFilename_)
	{
		CMFCPropertyGridProperty* object = viewMovieRecorderFilename_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewMovieRecorderFilename_ = nullptr;
	}

	if (viewMovieRecorderFilenameSuffix_)
	{
		CMFCPropertyGridProperty* object = viewMovieRecorderFilenameSuffix_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewMovieRecorderFilenameSuffix_ = nullptr;
	}

	if (viewMovieRecorderFps_)
	{
		CMFCPropertyGridProperty* object = viewMovieRecorderFps_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewMovieRecorderFps_ = nullptr;
	}

	viewRecorderGroup_->Expand(FALSE);
	viewRecorderGroup_->Expand(TRUE);
}

void PropertiesWindow::removeViewRecorderMemoryProperties()
{
	ocean_assert(viewRecorderGroup_ != nullptr);

	if (viewMemoryRecorderFps_)
	{
		CMFCPropertyGridProperty* object = viewMemoryRecorderFps_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewMemoryRecorderFps_ = nullptr;
	}

	viewRecorderGroup_->Expand(FALSE);
	viewRecorderGroup_->Expand(TRUE);
}

void PropertiesWindow::removeViewRecorderImageProperties()
{
	ocean_assert(viewRecorderGroup_ != nullptr);

	if (viewImageRecorderFilename_)
	{
		CMFCPropertyGridProperty* object = viewImageRecorderFilename_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewImageRecorderFilename_ = nullptr;
	}

	if (viewImageRecorderFilenameSuffix_)
	{
		CMFCPropertyGridProperty* object = viewImageRecorderFilenameSuffix_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewImageRecorderFilenameSuffix_ = nullptr;
	}

	viewRecorderGroup_->Expand(FALSE);
	viewRecorderGroup_->Expand(TRUE);
}

void PropertiesWindow::removeViewRecorderImageSequenceProperties()
{
	ocean_assert(viewRecorderGroup_ != nullptr);

	if (viewImageSequenceRecorderFilename_)
	{
		CMFCPropertyGridProperty* object = viewImageSequenceRecorderFilename_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewImageSequenceRecorderFilename_ = nullptr;
	}

	if (viewImageSequenceRecorderFilenameSuffix_)
	{
		CMFCPropertyGridProperty* object = viewImageSequenceRecorderFilenameSuffix_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewImageSequenceRecorderFilenameSuffix_ = nullptr;
	}

	if (viewImageSequenceRecorderFps_)
	{
		CMFCPropertyGridProperty* object = viewImageSequenceRecorderFps_;
		viewRecorderGroup_->RemoveSubItem(object);
		viewImageSequenceRecorderFps_ = nullptr;
	}

	viewRecorderGroup_->Expand(FALSE);
	viewRecorderGroup_->Expand(TRUE);
}

void PropertiesWindow::applyViewRecorderProperties()
{
	Config::Value& recorder = Application::get().config()["view"]["recorder"];

	ocean_assert(viewRecorderType_ != nullptr);
	std::string recorderTypeString = recorder["type"]("None");
	if (recorderTypeString == "None")
	{
		View::mainView().setRecorderType(View::TYPE_NONE);
		removeViewRecorderImageProperties();
		removeViewRecorderImageSequenceProperties();
		removeViewRecorderMovieProperties();
		removeViewRecorderMemoryProperties();
	}
	else if (recorderTypeString == "Image Recorder")
	{
		if (View::mainView().setRecorderType(View::TYPE_IMAGE))
		{
			addViewRecorderImageProperties(*viewRecorderGroup_);
			applyViewRecorderImageProperties();
		}
	}
	else if (recorderTypeString == "Image Sequence Recorder")
	{
		if (View::mainView().setRecorderType(View::TYPE_IMAGE_SEQUENCE))
		{
			addViewRecorderImageSequenceProperties(*viewRecorderGroup_);
			applyViewRecorderImageSequenceProperties();
		}
	}
	else if (recorderTypeString == "Movie Recorder")
	{
		if (View::mainView().setRecorderType(View::TYPE_MOVIE))
		{
			addViewRecorderMovieProperties(*viewRecorderGroup_);
			applyViewRecorderMovieProperties();
		}
	}
	else if (recorderTypeString == "Memory Recorder")
	{
		if (View::mainView().setRecorderType(View::TYPE_MEMORY))
		{
			addViewRecorderMemoryProperties(*viewRecorderGroup_);
			applyViewRecorderMemoryProperties();
		}
	}
	else
		recorderTypeString = "None";
	viewRecorderType_->SetValue(_variant_t(String::toWString(recorderTypeString).c_str()));
}

void PropertiesWindow::applyViewRecorderMovieProperties()
{
	Config::Value& movieRecorder = Application::get().config()["view"]["recorder"]["movierecorder"];

	ocean_assert(viewMovieRecorderFilename_ != nullptr);
	const std::string filename = movieRecorder["filename"]("");
	View::mainView().setRecorderFilename(filename);
	viewMovieRecorderFilename_->SetValue(_variant_t(String::toWString(filename).c_str()));

	ocean_assert(viewMovieRecorderFilenameSuffix_ != nullptr);
	const bool suffix = movieRecorder["extendedfilename"](true);
	View::mainView().setRecorderExtendedFilename(suffix);
	viewMovieRecorderFilenameSuffix_->SetValue(_variant_t(suffix));

	ocean_assert(viewMovieRecorderFps_ != nullptr);
	View::mainView().setRecorderFrameRate(movieRecorder["framerate"](25.0));
	viewMovieRecorderFps_->SetValue(_variant_t(View::mainView().recorderFramerate()));

	const View::EncoderNames encoders = View::mainView().recorderEncoderNames();
	ocean_assert(viewMovieRecorderEncoder_ != nullptr);
	viewMovieRecorderEncoder_->RemoveAllOptions();
	viewMovieRecorderEncoder_->AddOption(L"None");

	for (size_t n = 0; n < encoders.size(); n++)
		viewMovieRecorderEncoder_->AddOption(String::toWString(encoders[n]).c_str());
	std::string recorderEncoder = movieRecorder["encoder"]["name"]("None");

	View::mainView().setRecorderEncoder(recorderEncoder);
	viewMovieRecorderEncoder_->SetValue(_variant_t(String::toWString(View::mainView().recorderEncoder()).c_str()));
}

void PropertiesWindow::applyViewRecorderMemoryProperties()
{
	Config::Value& memoryRecorder = Application::get().config()["view"]["recorder"]["memoryrecorder"];

	ocean_assert(viewMemoryRecorderFps_ != nullptr);
	View::mainView().setRecorderFrameRate(float(memoryRecorder["framerate"](25.0)));
	viewMemoryRecorderFps_->SetValue(_variant_t(View::mainView().recorderFramerate()));
}

void PropertiesWindow::applyViewRecorderImageProperties()
{
	Config::Value& imageRecorder = Application::get().config()["view"]["recorder"]["imagerecorder"];

	ocean_assert(viewImageRecorderFilename_ != nullptr);
	const std::string filename = imageRecorder["filename"]("");
	View::mainView().setRecorderFilename(filename);
	viewImageRecorderFilename_->SetValue(_variant_t(String::toWString(filename).c_str()));

	ocean_assert(viewImageRecorderFilenameSuffix_ != nullptr);
	const bool suffix = imageRecorder["extendedfilename"](true);
	View::mainView().setRecorderExtendedFilename(suffix);
	viewImageRecorderFilenameSuffix_->SetValue(_variant_t(suffix));
}

void PropertiesWindow::applyViewRecorderImageSequenceProperties()
{
	Config::Value& imageSequenceRecorder = Application::get().config()["view"]["recorder"]["imagesequencerecorder"];

	ocean_assert(viewImageSequenceRecorderFilename_ != nullptr);
	const std::string filename = imageSequenceRecorder["filename"]("");
	View::mainView().setRecorderFilename(filename);
	viewImageSequenceRecorderFilename_->SetValue(_variant_t(String::toWString(filename).c_str()));

	ocean_assert(viewImageSequenceRecorderFilenameSuffix_ != nullptr);
	const bool suffix = imageSequenceRecorder["extendedfilename"](true);
	View::mainView().setRecorderExtendedFilename(suffix);
	viewImageSequenceRecorderFilenameSuffix_->SetValue(_variant_t(suffix));

	ocean_assert(viewImageSequenceRecorderFps_ != nullptr);
	View::mainView().setRecorderFrameRate(float(imageSequenceRecorder["framerate"](25.0)));
	viewImageSequenceRecorderFps_->SetValue(_variant_t(View::mainView().recorderFramerate()));
}

void PropertiesWindow::addPluginProperties(CMFCPropertyGridCtrl& group)
{
	ocean_assert(pluginGroup_ == nullptr);
	pluginGroup_ = new PropertyItem(pluginCallback_, L"Plugin options");

	std::wstring plugindirectory = L"\\plugins";

	const bool useDefaultPluginDirectory = Application::get().usesDefaultPluginDirectory();

	if (!useDefaultPluginDirectory)
		plugindirectory = String::toWString(Application::get().config()["plugins"]["version"][Build::buildString()]["plugindirectory"](""));

	ocean_assert(pluginDirectory_ == nullptr);
	pluginDirectory_ = new FilePropertyItem(pluginCallback_, L"Plugin directory", plugindirectory.c_str(), DWORD(0), L"Specifies the directory plugins are loaded from.");
	pluginDirectory_->Enable(!useDefaultPluginDirectory);
	pluginGroup_->AddSubItem(pluginDirectory_);

	group.AddProperty(pluginGroup_);
}

void PropertiesWindow::applyPluginProperties()
{
	ocean_assert(pluginGroup_ != nullptr);
	ocean_assert(pluginDirectory_ != nullptr);
}

void PropertiesWindow::onApplicationPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	// application properties
	if (item == applicationSceneFitting_)
		Application::get().config()["application"]["fittingafterloading"] = item->GetValue().boolVal == -1;
	else if (item == applicationLoadLastScene_)
		Application::get().config()["application"]["loadlastscene"] = item->GetValue().boolVal == -1;
	else if (item == applicationStartFullscreen_)
		Application::get().config()["application"]["startfullscreen"] = item->GetValue().boolVal == -1;
	else if (item == applicationDoubleClickFullscreen_)
		Application::get().config()["application"]["doubleclickfullscreen"] = item->GetValue().boolVal == -1;
	else if (item == applicationCameraCalibrationFile_)
	{
		IO::File file(String::toAString(applicationCameraCalibrationFile_->GetValue().bstrVal));

		if (file.isValid())
		{
			if (IO::CameraCalibrationManager::get().registerCalibrationFile(file()))
			{
				Log::info() << "Successfully loaded the camera calibration file \"" << file() << "\".";
			}
			else
			{
				Log::warning() << "Failed to load the camera calibration file \"" << file() << "\".";
			}

			Application::get().config()["application"]["cameracalibrationfile"] = file();
		}
		else
		{
			Log::error() << "Invalid camera calibration file \"" << file() << "\".";
		}
	}
}

void PropertiesWindow::onViewPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	ocean_assert(false);
}

void PropertiesWindow::onViewDisplayPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	// view display properties
	if (item == viewDisplayFieldOfView_)
	{
		const double fovx = item->GetValue().dblVal;

		if (fovx > 0 && fovx < 180)
		{
			View::mainView().setHorizontalFieldOfView(NumericD::deg2rad(fovx));
			Application::get().config()["view"]["display"]["fovx"] = NumericD::rad2deg(View::mainView().horizontalFieldOfView());
		}
	}
	else if (item == viewDisplayNearDistance_)
	{
		const double nearDistance = item->GetValue().dblVal;

		if (nearDistance > 0)
		{
			View::mainView().setNearDistance(nearDistance);
			Application::get().config()["view"]["display"]["near"] = View::mainView().nearDistance();
		}
	}
	else if (item == viewDisplayFarDistance_)
	{
		const double farDistance = item->GetValue().dblVal;

		if (farDistance > 0)
		{
			View::mainView().setFarDistance(farDistance);
			Application::get().config()["view"]["display"]["far"] = View::mainView().farDistance();
		}
	}
	else if (item == viewDisplayFocus_)
	{
		View::mainView().setFocusDistance(item->GetValue().dblVal);
		Application::get().config()["view"]["display"]["focus"] = View::mainView().focusDistance();
	}
	else if (item == viewDisplayFramerate_)
	{
		const float framerate = item->GetValue().fltVal;

		if (framerate >= 0 || framerate <= 10000)
		{
			View::mainView().setPreferredFramerate(framerate);
			Application::get().config()["view"]["display"]["framerate"] = View::mainView().preferredFramerate();
		}
	}
	else if (item == viewDisplayType_)
	{
		View::ViewType type = View::TYPE_MONO_VIEW;
		std::string typeString = String::toAString(item->GetValue().bstrVal);

		if (typeString == "Mono view")
			type = View::TYPE_MONO_VIEW;
		else if (typeString == "Parallel view")
			type = View::TYPE_PARALLEL_VIEW;
		else if (typeString == "Quadbuffered stereo view")
			type = View::TYPE_STEREO_VIEW_QUADBUFFERED;
		else if (typeString == "Anaglyph stereo view")
			type = View::TYPE_STEREO_VIEW_ANAGLYPH;
		else
			typeString = "Mono view";

		View::mainView().setType(type);

		Application::get().config()["view"]["display"]["type"] = typeString;
	}
	else if (item == viewDisplayEyesReversed_)
	{
		View::mainView().setEyesReversed(item->GetValue().boolVal == -1);
		Application::get().config()["view"]["display"]["eyesreversed"] = item->GetValue().boolVal == -1;
	}
	else if (item == viewDisplayHeadlight_)
	{
		View::mainView().setUseHeadlight(item->GetValue().boolVal == -1);
		Application::get().config()["view"]["display"]["useheadlight"] = View::mainView().useHeadlight();
	}
	else if (item == viewDisplayPhantomMode_)
	{
		std::string phantomModeString = String::toAString(viewDisplayPhantomMode_->GetValue().bstrVal);
		Rendering::Framebuffer::PhantomMode phantomMode = Rendering::PhantomAttribute::PM_DEFAULT;
		if (phantomModeString == "Color")
			phantomMode = Rendering::PhantomAttribute::PM_COLOR;
		else if (phantomModeString == "Debug")
			phantomMode = Rendering::PhantomAttribute::PM_DEBUG;
		else if (phantomModeString == "Video")
			phantomMode = Rendering::PhantomAttribute::PM_VIDEO;
		View::mainView().setPhantomMode(phantomMode);

		Application::get().config()["view"]["display"]["phantommode"] = phantomModeString;
	}
}

void PropertiesWindow::onViewBackgroundPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	BeginWaitCursor();

	// view background properties
	if (item == viewBackgroundColor_)
	{
		ocean_assert(dynamic_cast<ColorPropertyItem*>(item) != nullptr);
		ColorPropertyItem* colorItem = (ColorPropertyItem*)item;

		View::mainView().setBackgroundColor(colorItem->GetColor());
		Application::get().config()["view"]["background"]["color"] = int(View::mainView().backgroundColor());
	}
	else if (item == viewBackgroundType_)
	{
		int index = 0;

		for (int n = 1; n < item->GetOptionCount(); ++n)
		{
			if (std::wstring(item->GetOption(n)) == std::wstring(item->GetValue().bstrVal))
			{
				index = n;
				break;
			}
		}

		Application::get().config()["view"]["background"]["type"] = String::toAString(item->GetValue().bstrVal);

		ocean_assert(item->GetParent() != nullptr);
		addViewBackgroundProperties(index, *item->GetParent());
		applyViewBackgroundProperties(index);
	}
	else if (item == viewBackgroundDisplayType_)
	{
		std::string typeString = String::toAString(viewBackgroundDisplayType_->GetValue().bstrVal);

		Rendering::UndistortedBackground::DisplayType type = Rendering::UndistortedBackground::DT_FASTEST;
		if (typeString == "Undistorted")
		{
			type = Rendering::UndistortedBackground::DT_UNDISTORTED;
		}

		View::mainView().setBackgroundDisplayType(type);

		Application::get().config()["view"]["background"]["displaytype"] = String::toAString(item->GetValue().bstrVal);
	}

	// view background device properties
	else if (item == viewBackgroundDeviceName_)
	{
		std::string name = String::toAString(viewBackgroundDeviceName_->GetValue().bstrVal);

		Media::FrameMediumRef medium;

		if (name != "None")
		{
			medium = (Media::Manager::get().newMedium(name, Media::Medium::LIVE_VIDEO, true));
		}

		Application::get().config()["view"]["background"]["device"]["name"] = name;

		View::mainView().setBackgroundMedium(medium);
	}

	// view background file properties
	else if (item == viewBackgroundMediaFile_)
	{
		ocean_assert(viewBackgroundMediaFile_ != nullptr);
		std::string file = String::toAString(viewBackgroundMediaFile_->GetValue().bstrVal);

		Media::FrameMediumRef medium;

		if (file.empty() == false)
		{
			medium = (Media::Manager::get().newMedium(file, true));
		}

		Application::get().config()["view"]["background"]["media"]["file"] = file;

		Media::FiniteMediumRef finiteMedium(medium);
		if (finiteMedium)
		{
			ocean_assert(viewBackgroundMediaLoop_ != nullptr);
			finiteMedium->setLoop(viewBackgroundMediaLoop_->GetValue().boolVal == -1);
		}

		Media::SoundMediumRef soundMedium(medium);
		if (soundMedium)
		{
			ocean_assert(viewBackgroundMediaSound_ != nullptr);
			soundMedium->setSoundMute(viewBackgroundMediaSound_->GetValue().boolVal != -1);
		}

		View::mainView().setBackgroundMedium(medium);
	}
	else if (item == viewBackgroundMediaLoop_)
	{
		ocean_assert(viewBackgroundMediaLoop_ != nullptr);

		Media::FiniteMediumRef finiteMedium(View::mainView().backgroundMedium());

		const bool loop = viewBackgroundMediaLoop_->GetValue().boolVal == -1;

		if (finiteMedium)
		{
			finiteMedium->setLoop(loop);
			finiteMedium->start();
		}

		Application::get().config()["view"]["background"]["media"]["loop"] = loop;
	}
	else if (item == viewBackgroundMediaSound_)
	{
		ocean_assert(viewBackgroundMediaSound_ != nullptr);

		Media::SoundMediumRef soundMedium(View::mainView().backgroundMedium());

		const bool sound = viewBackgroundMediaSound_->GetValue().boolVal == -1;

		if (soundMedium)
		{
			soundMedium->setSoundMute(!sound);
		}

		Application::get().config()["view"]["background"]["media"]["sound"] = sound;
	}

	EndWaitCursor();
}

void PropertiesWindow::onViewNavigationPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	if (item == viewNavigationCursorMode_)
	{
		std::string cursorModeString = String::toAString(viewNavigationCursorMode_->GetValue().bstrVal);
		View::CursorMode cursorMode = View::CM_HIDE_INACTIVITY;
		if (cursorModeString == "Show always")
		{
			cursorMode = View::CM_VISIBLE;
		}
		else if (cursorModeString == "Hide always")
		{
			cursorMode = View::CM_HIDE_ALWAYS;
		}

		View::mainView().setCursorMode(cursorMode);
		Application::get().config()["view"]["navigation"]["cursorfullscreenmode"] = cursorModeString;
	}

	if (item == viewNavigationStorePosition_)
	{
		const bool storePosition = viewNavigationStorePosition_->GetValue().boolVal == -1;
		View::mainView().setStorePosition(storePosition);
		Application::get().config()["view"]["navigation"]["storeposition"] = storePosition;
	}
}

void PropertiesWindow::onViewRendererPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	if (item == viewRendererName_)
	{
		//::MessageBoxW(nullptr, L"You are about to change rendering engine attributes.\nHowever, to apply the changes you have to restart the application.", L"Information", MB_ICONINFORMATION);

		std::string name = String::toAString(viewRendererName_->GetValue().bstrVal);

		ocean_assert(viewRendererAPI_ != 0);
		std::wstring apiSelection(viewRendererAPI_->GetValue().bstrVal);
		viewRendererAPI_->RemoveAllOptions();
		const Rendering::Engine::GraphicAPI supportedGraphicAPI = Rendering::Manager::get().supportedGraphicAPI(name);
		viewRendererAPI_->AddOption(L"Default");
		if ((supportedGraphicAPI & Rendering::Engine::API_DIRECTX) == Rendering::Engine::API_DIRECTX)
			viewRendererAPI_->AddOption(L"DirectX");
		if ((supportedGraphicAPI & Rendering::Engine::API_OPENGL) == Rendering::Engine::API_OPENGL)
			viewRendererAPI_->AddOption(L"OpenGL");
		if ((supportedGraphicAPI & Rendering::Engine::API_OPENGLES) == Rendering::Engine::API_OPENGLES)
			viewRendererAPI_->AddOption(L"OpenGL ES");
		if ((supportedGraphicAPI & Rendering::Engine::API_RAYTRACER) == Rendering::Engine::API_RAYTRACER)
			viewRendererAPI_->AddOption(L"Raytracer");

		bool found = false;
		for (int n = 0; n < viewRendererAPI_->GetOptionCount(); n++)
		{
			if (apiSelection == viewRendererAPI_->GetOption(n))
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			viewRendererAPI_->SetValue(_variant_t(apiSelection.c_str()));
		}
		else
		{
			viewRendererAPI_->SetValue(_variant_t(L"Default"));
		}

		View::mainView().setRenderer(name, supportedGraphicAPI);

		Application::get().config()["view"]["renderer"]["name"] = name;
	}
	else if (item == viewRendererAPI_)
	{
		//::MessageBoxW(nullptr, L"You are about to change rendering engine attributes.\nHowever, to apply the changes you have to restart the application.", L"Information", MB_ICONINFORMATION);

		ocean_assert(viewRendererAPI_ != nullptr);
		const std::string graphicAPIString = String::toAString(viewRendererAPI_->GetValue().bstrVal);
		Rendering::Engine::GraphicAPI graphicAPI = Rendering::Engine::API_DEFAULT;
		if (graphicAPIString == "OpenGL")
			graphicAPI = Rendering::Engine::API_OPENGL;
		else if (graphicAPIString == "DirectX")
			graphicAPI = Rendering::Engine::API_DIRECTX;
		else if (graphicAPIString == "OpenGL ES")
			graphicAPI = Rendering::Engine::API_OPENGLES;
		else if (graphicAPIString == "Raytracer")
			graphicAPI = Rendering::Engine::API_RAYTRACER;

		Application::get().config()["view"]["renderer"]["graphicapi"] = graphicAPIString;
	}
	else if (item == viewRendererFaceMode_)
	{
		std::string faceModeString = String::toAString(viewRendererFaceMode_->GetValue().bstrVal);
		Rendering::Framebuffer::FaceMode faceMode = Rendering::PrimitiveAttribute::MODE_DEFAULT;
		if (faceModeString == "Face")
			faceMode = Rendering::PrimitiveAttribute::MODE_FACE;
		else if (faceModeString == "Line")
			faceMode = Rendering::PrimitiveAttribute::MODE_LINE;
		else if (faceModeString == "Point")
			faceMode = Rendering::PrimitiveAttribute::MODE_POINT;
		View::mainView().setRendererFaceMode(faceMode);

		Application::get().config()["view"]["renderer"]["facemode"] = faceModeString;
	}
	else if (item == viewRendererCullingMode_)
	{
		std::string cullingModeString = String::toAString(viewRendererCullingMode_->GetValue().bstrVal);
		Rendering::Framebuffer::CullingMode cullingMode = Rendering::PrimitiveAttribute::CULLING_DEFAULT;
		if (cullingModeString == "Both")
			cullingMode = Rendering::PrimitiveAttribute::CULLING_BOTH;
		else if (cullingModeString == "Back")
			cullingMode = Rendering::PrimitiveAttribute::CULLING_BACK;
		else if (cullingModeString == "Front")
			cullingMode = Rendering::PrimitiveAttribute::CULLING_FRONT;
		else if (cullingModeString == "None")
			cullingMode = Rendering::PrimitiveAttribute::CULLING_NONE;
		View::mainView().setRendererCullingMode(cullingMode);

		Application::get().config()["view"]["renderer"]["cullingmode"] = cullingModeString;
	}
	else if (item == viewRendererTechnique_)
	{
		std::string techniqueString = String::toAString(viewRendererTechnique_->GetValue().bstrVal);
		Rendering::Framebuffer::RenderTechnique technique = Rendering::Framebuffer::TECHNIQUE_FULL;
		if (techniqueString == "Textured")
			technique = Rendering::Framebuffer::TECHNIQUE_TEXTURED;
		else if (techniqueString == "Shaded")
			technique = Rendering::Framebuffer::TECHNIQUE_SHADED;
		else if (techniqueString == "Unlit")
			technique = Rendering::Framebuffer::TECHNIQUE_UNLIT;
		View::mainView().setRendererTechnique(technique);

		Application::get().config()["view"]["renderer"]["technique"] = techniqueString;
	}
	else if (item == viewRendererSupportQuadbufferedStereo_)
	{
		/*if (View::mainView().renderer().empty() || ::MessageBoxW(nullptr, L"Your are about to change the rendering framebuffer.\nThe view has to be (re-)initialized\n\nDo you want to continue?",
				L"Warning", MB_ICONWARNING | MB_YESNO) == IDYES)*/
		{
			const bool supportQuadbufferedStereo = viewRendererSupportQuadbufferedStereo_->GetValue().boolVal == -1;

			//View::mainView().setSupportQuadbufferedStereo(supportQuadbufferedStereo);
			Application::get().config()["view"]["renderer"]["supportquadbufferedstereo"] = supportQuadbufferedStereo;
		}
		//else
		//	viewRendererSupportQuadbufferedStereo->SetValue(_variant_t(View::mainView().supportsQuadbufferedStereo()));
	}
	else if (item == viewRendererSupportAntialiasing_)
	{
		/*if (View::mainView().renderer().empty() || ::MessageBoxW(nullptr, L"Your are about to change the rendering framebuffer.\nThe view has to be (re-)initialized\n\nDo you want to continue?",
				L"Warning", MB_ICONWARNING | MB_YESNO) == IDYES)*/
		{
			std::string antialiasingString = String::toAString(viewRendererSupportAntialiasing_->GetValue().bstrVal);
			unsigned int buffers = 0;

			if (!antialiasingString.empty())
				buffers = atoi(antialiasingString.c_str());

			View::mainView().setSupportAntialiasing(buffers);
			Application::get().config()["view"]["renderer"]["supportantialiasing"] = antialiasingString;
		}
		//else
		//	viewRendererSupportStereo->SetValue(_variant_t(View::mainView().supportsStereo()));
	}
	else if (item == viewRendererEnableAntialiasing_)
	{
		View::mainView().setAntialiasing(item->GetValue().boolVal == -1);

		const bool enabled = View::mainView().antialiasing();
		item->SetValue(_variant_t(enabled));
		Application::get().config()["view"]["renderer"]["enableantialiasing"] = enabled;
	}
}

void PropertiesWindow::onViewRecorderPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	if (item == viewRecorderType_)
	{
		removeViewRecorderImageProperties();
		removeViewRecorderImageSequenceProperties();
		removeViewRecorderMovieProperties();
		removeViewRecorderMemoryProperties();

		std::wstring value(item->GetValue().bstrVal);

		if (value == L"None")
		{
			View::mainView().setRecorderType(View::TYPE_NONE);
			Application::get().config()["view"]["recorder"]["type"] = "None";
		}
		else if (value == L"Image Recorder")
		{
			View::mainView().setRecorderType(View::TYPE_IMAGE);
			Application::get().config()["view"]["recorder"]["type"] = "Image Recorder";

			ocean_assert(viewRecorderGroup_ != nullptr);
			addViewRecorderImageProperties(*viewRecorderGroup_);
			applyViewRecorderImageProperties();
		}
		else if (value == L"Image Sequence Recorder")
		{
			View::mainView().setRecorderType(View::TYPE_IMAGE_SEQUENCE);
			Application::get().config()["view"]["recorder"]["type"] = "Image Sequence Recorder";

			ocean_assert(viewRecorderGroup_ != nullptr);
			addViewRecorderImageSequenceProperties(*viewRecorderGroup_);
			applyViewRecorderImageSequenceProperties();
		}
		else if (value == L"Movie Recorder")
		{
			View::mainView().setRecorderType(View::TYPE_MOVIE);
			Application::get().config()["view"]["recorder"]["type"] = "Movie Recorder";

			ocean_assert(viewRecorderGroup_ != nullptr);
			addViewRecorderMovieProperties(*viewRecorderGroup_);
			applyViewRecorderMovieProperties();
		}
		else if (value == L"Memory Recorder")
		{
			View::mainView().setRecorderType(View::TYPE_MEMORY);
			Application::get().config()["view"]["recorder"]["type"] = "Memory Recorder";

			ocean_assert(viewRecorderGroup_ != nullptr);
			addViewRecorderMemoryProperties(*viewRecorderGroup_);
			applyViewRecorderMemoryProperties();
		}
	}

	if (item == viewMovieRecorderEncoder_)
	{
		const std::string value(String::toAString(item->GetValue().bstrVal));
		View::mainView().setRecorderEncoder(value);

		Application::get().config()["view"]["recorder"]["movierecorder"]["encoder"]["name"] = value;
	}

	if (item == viewMovieRecorderFps_)
	{
		const double value = item->GetValue().dblVal;
		View::mainView().setRecorderFrameRate(value);

		Application::get().config()["view"]["recorder"]["movierecorder"]["framerate"] = value;
	}

	if (item == viewMemoryRecorderFps_)
	{
		const float value = item->GetValue().fltVal;
		View::mainView().setRecorderFrameRate(value);

		Application::get().config()["view"]["recorder"]["memoryrecorder"]["framerate"] = value;
	}

	if (item == viewImageSequenceRecorderFps_)
	{
		const float value = item->GetValue().fltVal;
		View::mainView().setRecorderFrameRate(value);

		Application::get().config()["view"]["recorder"]["imagesequencerecorder"]["framerate"] = value;
	}

	if (item == viewMovieRecorderFilename_)
	{
		const std::string value(String::toAString(item->GetValue().bstrVal));
		View::mainView().setRecorderFilename(value);

		Application::get().config()["view"]["recorder"]["movierecorder"]["filename"] = value;
	}

	if (item == viewMovieRecorderFilenameSuffix_)
	{
		const bool value = item->GetValue().boolVal == -1;
		View::mainView().setRecorderExtendedFilename(value);

		Application::get().config()["view"]["recorder"]["movierecorder"]["extendedfilename"] = value;
	}

	if (item == viewImageRecorderFilename_)
	{
		const std::string value(String::toAString(item->GetValue().bstrVal));
		View::mainView().setRecorderFilename(value);

		Application::get().config()["view"]["recorder"]["imagerecorder"]["filename"] = value;
	}

	if (item == viewImageSequenceRecorderFilename_)
	{
		const std::string value(String::toAString(item->GetValue().bstrVal));
		View::mainView().setRecorderFilename(value);

		Application::get().config()["view"]["recorder"]["imagesequencerecorder"]["filename"] = value;
	}

	if (item == viewImageRecorderFilenameSuffix_)
	{
		const bool value = item->GetValue().boolVal == -1;
		View::mainView().setRecorderExtendedFilename(value);

		Application::get().config()["view"]["recorder"]["imagerecorder"]["extendedfilename"] = value;
	}

	if (item == viewImageSequenceRecorderFilenameSuffix_)
	{
		const bool value = item->GetValue().boolVal == -1;
		View::mainView().setRecorderExtendedFilename(value);

		Application::get().config()["view"]["recorder"]["imagesequencerecorder"]["extendedfilename"] = value;
	}
}

void PropertiesWindow::onPluginPropertyChanged(CMFCPropertyGridProperty* item)
{
	if (item == nullptr)
	{
		return;
	}

	if (item == pluginDirectory_)
	{
		IO::Directory directory(String::toAString(pluginDirectory_->GetValue().bstrVal));

		if (directory.isValid())
		{
			if (PluginManager::get().unloadAllPlugins())
			{
				PluginManager::get().collectPlugins(directory());
				PluginManager::get().loadAllPlugins();

				updateViewRenderer();
			}
			else
			{
				Log::error() << "Could not unload all plugins, some resource seem still to be in use! Restart the viewer to handle the problem!";
			}

			Application::get().config()["plugins"]["version"][Build::buildString()]["plugindirectory"] = directory();
		}
		else
		{
			Log::error() << "Invalid plugin directory selected \"" << directory() << "\".";
		}
	}
}

void PropertiesWindow::OnUpdateCmdUI(CFrameWnd* target, BOOL disableIfNoHandler)
{
	CDockablePane::OnUpdateCmdUI(target, disableIfNoHandler);
}

void PropertiesWindow::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	propertyList_.SetFocus();
}

void PropertiesWindow::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
}

void PropertiesWindow::onFramebufferChanged()
{
	ocean_assert(viewRendererSupportQuadbufferedStereo_);
	viewRendererSupportQuadbufferedStereo_->Enable(View::mainView().supportsQuadbufferedStereo());
}

}

}

}
