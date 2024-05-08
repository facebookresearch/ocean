/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_PROPERTIESWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_PROPERTIESWINDOW_H

#include "application/ocean/orca/win/OrcaWin.h"

#include "ocean/base/Callback.h"

#include <vector>

namespace Ocean
{

namespace Orca
{

namespace Win
{

/**
 * This class implements a property window for orca.
 * The window holds all configuration possibilities of orca.<br>
 * @ingroup orcawin
 */
class PropertiesWindow : public CDockablePane
{
	public:

		/// Definition of a background device format id.
		static constexpr unsigned int backgroundDeviceFormatId_ = 1;

		/// Definition of a background device property id.
		static constexpr unsigned int backgroundDevicePropertyId_ = 2;

		/// Definition of a recorder menu id.
		static constexpr unsigned int recorderEncoderMenuId_ = 3;

	protected:

		/**
		 * This class enhances the property grid control class.
		 */
		class PropertyControl : public CMFCPropertyGridCtrl
		{
			public:

				/**
				 * Creates a new property control object.
				 */
				PropertyControl();

				/**
				 * Sets the property column width in pixel.
				 * @param width The width in pixel to set
				 */
				void setPropertyColumnWidth(const int width);

			private:

				/**
				 * The framework calls this member function when the user selects an item from a menu,
				 * when a child control sends a notification message, or when an accelerator keystroke is translated.
				 */
				BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
		};

		/**
		 * Definition of an event callback function for property grid properties.
		 */
		typedef Callback<void, CMFCPropertyGridProperty*> EventCallback;

		/**
		 * This class implements a property item with and enhanced event handling.
		 * @param orcawin
		 */
		class PropertyItem : public CMFCPropertyGridProperty
		{
			protected:

				/**
				 * Definition of a pair holding a menu entry string and a corresponding id.
				 */
				typedef std::pair<std::string, unsigned int> MenuEntry;

				/**
				 * Definition of a vector holding popup menu entries.
				 */
				typedef std::vector<MenuEntry> MenuEntries;

			public:

				/**
				 * Creates a new item as group.
				 * @param callback Event callback function
				 * @param strGroupName Property group name
				 * @param dwData Property data
				 * @param bIsValueList Determines whether this property is a value list or not
				 */
				PropertyItem(const EventCallback& callback, const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);

				/**
				 * Creates a new item.
				 * @param callback Event callback function
				 * @param strName Property name
				 * @param varValue Property default value
				 * @param lpszDescr Property description
				 * @param dwData Property data
				 * @param lpszEditMask Property edit mask
				 * @param lpszEditTemplate Property edit template
				 * @param lpszValidChars List of valid characters for this property
				 */
				PropertyItem(const EventCallback& callback, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0, LPCTSTR lpszEditMask = nullptr, LPCTSTR lpszEditTemplate = nullptr, LPCTSTR lpszValidChars = nullptr);

				/**
				 * Destructs a property item.
				 */
				~PropertyItem() override;

				/**
				 * Adds a popup menu entry.
				 * @param entry Menu entry name
				 * @param id Menu entry id
				 */
				void addPopupMenuEntry(const std::string& entry, const unsigned int id);

			private:

				/**
				 * Called by the GUI framework when the user is finished modifying a property value.
				 * @return True, if succeeded
				 */
				BOOL OnEndEdit() override;

				/**
				 * Called by the framework when the user clicks the right mouse button in the property value area.
				 */
				void OnRClickValue(CPoint point, BOOL bSelChanged) override;

				/**
				 * Called by the GUI framework when a combo box that is contained in a property is closed.
				 */
				void OnCloseCombo() override;

			private:

				/// Popup menu.
				HMENU menu_ = nullptr;

				/// Popup menu entries.
				MenuEntries menuEntries_;

				/// Event callback function.
				EventCallback eventCallback_;
		};

		/**
		 * This class implements a color property item with and enhanced event handling.
		 * @param orcawin
		 */
		class ColorPropertyItem : public CMFCPropertyGridColorProperty
		{
			public:

				/**
				 * Creates a new item.
				 * @param callback Event callback function
				 * @param strName Name of the color property
				 * @param color Default color
				 * @param pPalette Color palette
				 * @param lpszDescr Property description
				 * @param dwData Property data
				 */
				ColorPropertyItem(const EventCallback& callback, const CString& strName, const COLORREF& color, CPalette* pPalette = nullptr, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0);

			private:

				/**
				 * Called by the GUI framework when the user is finished modifying a property value.
				 * @return True, if succeeded
				 */
				BOOL OnEndEdit() override;

			private:

				/// Event callback function.
				EventCallback eventCallback_;
		};

		/**
		 * This class implements a color property item with and enhanced event handling.
		 * @param orcawin
		 */
		class FilePropertyItem : public CMFCPropertyGridFileProperty
		{
			public:

				/**
				 * Creates a new directory item.
				 * @param callback Event callback function
				 * @param strName Property name
				 * @param strFolderName Property folder name
				 * @param dwData Property data
				 * @param lpszDescr Property description
				 */
				FilePropertyItem(const EventCallback& callback, const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = nullptr);

				/**
				 * Creates a new file item.
				 * @param callback Event callback function
				 * @param strName Property name
				 * @param bOpenFileDialog Determines whether a file or directory dialog property is created
				 * @param strFileName Property filename
				 * @param lpszDefExt Default file extension
				 * @param dwFlags Dialog flags
				 * @param lpszFilter Dialog file filter
				 * @param lpszDescr Property description
				 * @param dwData Property data
				 */
				FilePropertyItem(const EventCallback& callback, const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = nullptr, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = nullptr, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0);

			private:

				/**
				 * Called by the GUI framework when the user is finished modifying a property value.
				 * @return True, if succeeded
				 */
				BOOL OnEndEdit() override;

				/**
				 * Called by the GUI framework when the user clicks a button that is contained in a property.
				 * @param point A point, in client coordinates
				 */
				void OnClickButton(CPoint point) override;

			private:

				/// Recent filename.
				std::wstring recentFilename_;

				/// Event callback function.
				EventCallback eventCallback_;
		};

		/**
		 * This class implements a property tool bar.
		 */
		class PropertiesToolBar : public CMFCToolBar
		{
			public:

				/**
				 * For internal use only.
				 */
				void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler) override;

				/**
				 * Determines whether the toolbar is displayed in the list on the 'Toolbars' pane of the Customize dialog box.
				 * @return True, if so
				 */
				BOOL AllowShowOnList() const override;
		};

	public:

		/**
		 * Creates a new property window.
		 */
		PropertiesWindow();

		/**
		 * Destructs a property window.
		 */
		~PropertiesWindow() override;

		/**
		 * Loads and applies configuration parameters like e.g. position and dimension.
		 */
		void applyConfiguration();

		/**
		 * Stores configuration parameters.
		 */
		void storeConfiguration();

		/**
		 * Event function if the framebuffer has been changed.
		 */
		void onFramebufferChanged();

	private:

		/**
		 * The GUI framework calls this function when an application requests a window creation.
		 * @param createStruct Create structure
		 * @return Creation result
		 */
		afx_msg int OnCreate(LPCREATESTRUCT createStruct);

		/**
		 * The GUI framework calls this function when the CWnd object is about to be hidden or shown.
		 * @param show Specifies whether a window is being shown or not.
		 * @param status Specifies the status of the window being shown.
		 */
		afx_msg void OnShowWindow(BOOL show, UINT status);

		/**
		 * The GUI framework calls this function after the window's size has changed.
		 * @param type Of the size change
		 * @param width New client width
		 * @param height New client height
		 */
		afx_msg void OnSize(UINT type, int width, int height);

		/**
		 * Called by the GUI framework when the user presses the close button on the caption for the pane.
		 */
		virtual void OnPressCloseButton();

		/**
		 * Adjusts the layout of the window.
		 */
		void AdjustLayout();

		/**
		 * Sets the VS .Net look.
		 */
		void SetVSDotNetLook(BOOL);

		/**
		 * Event function for setting the focus.
		 */
		afx_msg void OnSetFocus(CWnd*);

		/**
		 * Event function for changing the settings.
		 */
		afx_msg void OnSettingChange(UINT, LPCTSTR);

		/**
		 * Adds the different properties.
		 */
		void addProperties();

		/**
		 * Applies the different properties from configuration.
		 */
		void applyProperties();

		/**
		 * Adds the application properties to a given property group.
		 * @param group Property group receiving the application properties
		 */
		void addApplicationProperties(CMFCPropertyGridCtrl& group);

		/**
		 * Applies the application properties from the configuration.
		 */
		void applyApplicationProperties();

		/**
		 * Adds the view properties to a given property group.
		 * @param group Property group receiving the view properties
		 */
		void addViewProperties(CMFCPropertyGridCtrl& group);

		/**
		 * Applies view properties from the configuration.
		 */
		void applyViewProperties();

		/**
		 * Adds the display properties of the view property to a given property group.
		 * @param group Property group receiving the display properties
		 */
		void addViewDisplayProperties(CMFCPropertyGridProperty& group);

		/**
		 * Applies display properties of the view property from the configuration.
		 */
		void applyViewDisplayProperties();

		/**
		 * Adds the background properties of the view property to a given property group.
		 * @param group Property group receiving the background properties
		 */
		void addViewBackgroundProperties(CMFCPropertyGridProperty& group);

		/**
		 * Applies background properties of the view property from the configuration.
		 */
		void applyViewBackgroundProperties();

		/**
		 * Adds a property of the background properties to a given property group.
		 * @param index The index of the background type
		 * @param group Property group receiving the device properties
		 */
		void addViewBackgroundProperties(const int index, CMFCPropertyGridProperty& group);

		/**
		 * Applies a property of the background properties from the configuration.
		 * @param index The index of the background type
		 */
		void applyViewBackgroundProperties(const int index);

		/**
		 * Removes all view background type properties.
		 */
		void removeViewBackgroundTypeProperties();

		/**
		 * Removes all view background device properties.
		 */
		void removeViewBackgroundDeviceProperties();

		/**
		 * Removes all view background media properties.
		 */
		void removeViewBackgroundMediaProperties();

		/**
		 * Removes the view background display type property.
		 */
		void removeViewBackgroundDisplayTypeProperty();

		/**
		 * Adds the display type properties of the background properties to a given property group.
		 * @param group Property group receiving the device properties
		 */
		void addViewBackgroundDisplayTypeProperty(CMFCPropertyGridProperty& group);

		/**
		 * Applies the display type property of the background properties from the configuration.
		 */
		void applyViewBackgroundDisplayTypeProperty();

		/**
		 * Adds the device properties of the background properties to a given property group.
		 * @param group Property group receiving the device properties
		 */
		void addViewBackgroundDeviceProperties(CMFCPropertyGridProperty& group);

		/**
		 * Applies the device properties of the background properties from the configuration.
		 */
		void applyViewBackgroundDeviceProperties();

		/**
		 * Adds the media properties of the background properties to a given property group.
		 * @param group Property group receiving the media properties
		 */
		void addViewBackgroundMediaProperties(CMFCPropertyGridProperty& group);

		/**
		 * Applies the media properties of the background properties from the configuration.
		 */
		void applyViewBackgroundMediaProperties();

		/**
		 * Adds the navigation properties of the view property to a given property group.
		 * @param group Property group receiving the navigation properties
		 */
		void addViewNavigationProperties(CMFCPropertyGridProperty& group);

		/**
		 * Applies navigation properties of the view property from the configuration.
		 */
		void applyViewNavigationProperties();

		/**
		 * Adds the renderer properties to a given property group.
		 * @param group Property group receiving the view properties
		 */
		void addViewRendererProperties(CMFCPropertyGridProperty& group);

		/**
		 * Updates the renderer name property.
		 */
		void updateViewRenderer();

		/**
		 * Applies the renderer properties from the configuration.
		 */
		void applyViewRendererProperties();

		/**
		 * Adds the recorder properties to a given property group.
		 * @param group Property group receiving the recorder properties
		 */
		void addViewRecorderProperties(CMFCPropertyGridProperty& group);

		/**
		 * Adds the movie recorder properties to a given property group.
		 * @param group Property group receiving the movie recorder properties
		 */
		void addViewRecorderMovieProperties(CMFCPropertyGridProperty& group);

		/**
		 * Adds the memory recorder properties to a given property group.
		 * @param group Property group receiving the memory recorder properties
		 */
		void addViewRecorderMemoryProperties(CMFCPropertyGridProperty& group);

		/**
		 * Adds the image recorder properties to a given property group.
		 * @param group Property group receiving the image recorder properties
		 */
		void addViewRecorderImageProperties(CMFCPropertyGridProperty& group);

		/**
		 * Adds the image sequence recorder properties to a given property group.
		 * @param group Property group receiving the image sequence recorder properties
		 */
		void addViewRecorderImageSequenceProperties(CMFCPropertyGridProperty& group);

		/**
		 * Removes the movie recorder properties.
		 */
		void removeViewRecorderMovieProperties();

		/**
		 * Removes the memory recorder properties.
		 */
		void removeViewRecorderMemoryProperties();

		/**
		 * Removes the image recorder properties.
		 */
		void removeViewRecorderImageProperties();

		/**
		 * Removes the image sequence recorder properties.
		 */
		void removeViewRecorderImageSequenceProperties();

		/**
		 * Applies the recorder properties from the configuration.
		 */
		void applyViewRecorderProperties();

		/**
		 * Applies the movie recorder properties from the configuration.
		 */
		void applyViewRecorderMovieProperties();

		/**
		 * Applies the memory recorder properties from the configuration.
		 */
		void applyViewRecorderMemoryProperties();

		/**
		 * Applies the image recorder properties from the configuration.
		 */
		void applyViewRecorderImageProperties();

		/**
		 * Applies the image sequence recorder properties from the configuration.
		 */
		void applyViewRecorderImageSequenceProperties();

		/**
		 * Adds the plugin properties to a given property group.
		 * @param group Property group receiving the plugin properties
		 */
		void addPluginProperties(CMFCPropertyGridCtrl& group);

		/**
		 * Applies the plugin properties from the configuration.
		 */
		void applyPluginProperties();

		/**
		 * Called if an application property item has changed.
		 * @param item Changed property item
		 */
		void onApplicationPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a view property item has changed.
		 * @param item Changed property item
		 */
		void onViewPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a view display property item has changed.
		 * @param item Changed property item
		 */
		void onViewDisplayPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a view background property item has changed.
		 * @param item Changed property item
		 */
		void onViewBackgroundPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a view navigation property item has changed.
		 * @param item Changed property item
		 */
		void onViewNavigationPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a view renderer property item has changed.
		 * @param item Changed property item
		 */
		void onViewRendererPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a view recorder property item has changed.
		 * @param item Changed property item
		 */
		void onViewRecorderPropertyChanged(CMFCPropertyGridProperty* item);

		/**
		 * Called if a plugin property item has changed.
		 * @param item Changed property item
		 */
		void onPluginPropertyChanged(CMFCPropertyGridProperty* item);

		virtual void OnUpdateCmdUI(CFrameWnd* target, BOOL disableIfNoHandler);

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();

	private:

		/// Property grid navigation object.
		PropertyControl propertyList_;

		/// The combo box.
		CComboBox comboBox_;


		/// Application property group.
		PropertyItem* applicationGroup_ = nullptr;

		/// Application property fitting to screen.
		PropertyItem* applicationSceneFitting_ = nullptr;

		/// Application property load last scene.
		PropertyItem* applicationLoadLastScene_ = nullptr;

		/// Application property start with fullscreen.
		PropertyItem* applicationStartFullscreen_ = nullptr;

		/// Application property double click fullscreen toggle.
		PropertyItem* applicationDoubleClickFullscreen_ = nullptr;

		/// Application property camera calibration file.
		FilePropertyItem* applicationCameraCalibrationFile_ = nullptr;


		// View property group.
		PropertyItem* viewGroup_ = nullptr;

		/// View display property group.
		PropertyItem* viewDisplayGroup_ = nullptr;

		/// View display property field of view.
		PropertyItem* viewDisplayFieldOfView_ = nullptr;

		/// View display property near distance.
		PropertyItem* viewDisplayNearDistance_ = nullptr;

		/// View display property far distance.
		PropertyItem* viewDisplayFarDistance_ = nullptr;

		/// View display property focus.
		PropertyItem* viewDisplayFocus_ = nullptr;

		/// View display property framerate.
		PropertyItem* viewDisplayFramerate_ = nullptr;

		/// View display property type.
		PropertyItem* viewDisplayType_ = nullptr;

		/// View display property eyes reversed.
		PropertyItem* viewDisplayEyesReversed_ = nullptr;

		/// View display property headlight.
		PropertyItem* viewDisplayHeadlight_ = nullptr;

		/// View display property phantom mode.
		PropertyItem* viewDisplayPhantomMode_ = nullptr;


		/// View background property group.
		PropertyItem* viewBackgroundGroup_ = nullptr;

		/// View background property color.
		ColorPropertyItem* viewBackgroundColor_ = nullptr;

		/// View background property type.
		PropertyItem* viewBackgroundType_ = nullptr;

		/// View background property display type.
		PropertyItem* viewBackgroundDisplayType_ = nullptr;

		/// View background device name
		PropertyItem* viewBackgroundDeviceName_ = nullptr;

		/// View background media property file.
		FilePropertyItem* viewBackgroundMediaFile_ = nullptr;

		/// View background media property loop.
		PropertyItem* viewBackgroundMediaLoop_ = nullptr;

		/// View background media property sound.
		PropertyItem* viewBackgroundMediaSound_ = nullptr;


		/// View navigation property group.
		PropertyItem* viewNavigationGroup_ = nullptr;

		/// View navigation property cursor fullscreen mode.
		PropertyItem* viewNavigationCursorMode_ = nullptr;

		/// View navigation property store last viewing position.
		PropertyItem* viewNavigationStorePosition_ = nullptr;


		/// View renderer property group.
		PropertyItem* viewRendererGroup_ = nullptr;

		/// View renderer property renderer name.
		PropertyItem* viewRendererName_ = nullptr;

		/// View renderer property renderer API.
		PropertyItem* viewRendererAPI_ = nullptr;

		/// View renderer property face mode.
		PropertyItem* viewRendererFaceMode_ = nullptr;

		/// View renderer property culling mode.
		PropertyItem* viewRendererCullingMode_ = nullptr;

		/// View renderer property technique.
		PropertyItem* viewRendererTechnique_ = nullptr;

		/// View renderer support quad-buffered stereo.
		PropertyItem* viewRendererSupportQuadbufferedStereo_ = nullptr;

		/// View renderer support anti-aliasing.
		PropertyItem* viewRendererSupportAntialiasing_ = nullptr;

		/// View renderer enable anti-aliasing.
		PropertyItem* viewRendererEnableAntialiasing_ = nullptr;


		/// View recorder property group.
		PropertyItem* viewRecorderGroup_ = nullptr;

		/// View recorder property type.
		PropertyItem* viewRecorderType_ = nullptr;

		/// View movie recorder property type.
		PropertyItem* viewMovieRecorderEncoder_ = nullptr;

		/// View movie recorder property fps.
		PropertyItem* viewMovieRecorderFps_ = nullptr;

		/// View memory recorder property fps.
		PropertyItem* viewMemoryRecorderFps_ = nullptr;

		/// View image sequence recorder property fps.
		PropertyItem* viewImageSequenceRecorderFps_ = nullptr;

		/// View image recorder property filename.
		FilePropertyItem* viewImageRecorderFilename_ = nullptr;

		/// View image sequence recorder property filename.
		FilePropertyItem* viewImageSequenceRecorderFilename_ = nullptr;

		/// View movie recorder property filename.
		FilePropertyItem* viewMovieRecorderFilename_ = nullptr;

		/// View image recorder property filename suffix.
		PropertyItem* viewImageRecorderFilenameSuffix_ = nullptr;

		/// View image recorder property filename suffix.
		PropertyItem* viewImageSequenceRecorderFilenameSuffix_ = nullptr;

		/// View movie recorder property filename suffix.
		PropertyItem* viewMovieRecorderFilenameSuffix_ = nullptr;


		/// Plugin property group.
		PropertyItem* pluginGroup_ = nullptr;

		/// Plugin directory.
		FilePropertyItem* pluginDirectory_ = nullptr;


		/// Event callback function for all application property items.
		EventCallback applicationCallback_;

		/// Event callback function for all view property items.
		EventCallback viewCallback_;

		/// Event callback function for all view display property items.
		EventCallback viewDisplayCallback_;

		/// Event callback function for all view background property items.
		EventCallback viewBackgroundCallback_;

		/// Event callback function for all view background property items.
		EventCallback viewNavigationCallback_;

		/// Event callback function for all view background property items.
		EventCallback viewRendererCallback_;

		/// Event callback function for all view background property items.
		EventCallback viewRecorderCallback_;

		/// Event callback function for all plugin property items.
		EventCallback pluginCallback_;


		/// Flag determines whether the configuration has been applied.
		bool configurationApplied_ = false;

		/// Definition of an invalid window value.
		static constexpr int invalidWindowValue_ = 2147483647;
};

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_PROPERTIESWINDOW_H
