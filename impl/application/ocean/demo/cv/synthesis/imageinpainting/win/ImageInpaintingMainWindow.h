/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_MAINWINDOW_H

#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpainting.h"
#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpaintingSurface.h"

/**
 * This class implements the main window.
 * @ingroup applicationdemocvsynthesisimageinpaintingwxw
 */
class MainWindow : public wxFrame
{
	public:

		/**
		 * Definition of individual event ids.
		 */
		enum
		{
			/// Open event id.
			ID_Open = 1,
			/// Reload event id.
			ID_Reload,
			/// Save image event id.
			ID_Save_Image,
			/// Save mask event id.
			ID_Save_Mask,
			/// Quit event id.
			ID_Quit,
			/// About event id.
			ID_About,
			/// Mark event id.
			ID_Mark,
			/// Line event id.
			ID_Line,
			/// No zoom event id.
			ID_Nozoom,
			/// Remove1 event id.
			ID_Remove1,
			/// Remove2 event id.
			ID_Remove2,
			/// Remove3 event id.
			ID_Remove3,
			/// Remove4 event id.
			ID_Remove4,
			/// Remove5 event id.
			ID_Remove5,
			/// Toolbar event id.
			ID_Toolbar
		};

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the main window to be created
		 * @param pos The position of the main window
		 * @param size The size of the main window
		 */
		MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

		/**
		 * Loads a given image file.
		 * @param filename Name of the image file
		 * @return True, if succeeded
		 */
		bool loadFile(const std::string& filename);

		/**
		 * Loads a given image mask.
		 * @param filename Name of the mask file
		 * @return True, if succeeded
		 */
		bool loadMask(const std::string& filename);

		/**
		 * Saves the image.
		 * @param event The event command
		 */
		void onSaveImage(wxCommandEvent& event);

		/**
		 * Saves the mask.
		 * @param event The event command
		 */
		void onSaveMask(wxCommandEvent& event);

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

		/**
		 * Open event function.
		 * @param event The event command
		 */
		void onOpen(wxCommandEvent& event);

		/**
		 * Mark event function.
		 * @param event The event command
		 */
		void onMark(wxCommandEvent& event);

		/**
		 * Line event function.
		 * @param event The event command
		 */
		void onLine(wxCommandEvent& event);

		/**
		 * Reload event function.
		 * @param event The event command
		 */
		void onReload(wxCommandEvent& event);

		/**
		 * On no zoom event function.
		 * @param event The event command
		 */
		void onNoZoom(wxCommandEvent& event);

		/**
		 * On remove event function.
		 * @param event The event command
		 */
		void onRemove1(wxCommandEvent& event);

		/**
		 * On remove event function.
		 * @param event The event command
		 */
		void onRemove2(wxCommandEvent& event);

		/**
		 * On remove event function.
		 * @param event The event command
		 */
		void onRemove3(wxCommandEvent& event);

		/**
		 * On remove event function.
		 * @param event The event command
		 */
		void onRemove4(wxCommandEvent& event);

		/**
		 * On remove event function.
		 * @param event The event command
		 */
		void onRemove5(wxCommandEvent& event);

		/**
		 * Quit event function.
		 * @param event The event command
		 */
		void onQuit(wxCommandEvent& event);

		/**
		 * About event function.
		 * @param event The event command
		 */
		void onAbout(wxCommandEvent& event);

		/**
		 * On drag and drop file event function.
		 * @param files Event files
		 */
		bool onFileDragAndDrop(const std::vector<std::string> &files);

	private:

		/// Interaction mode.
		InteractionMode interactionMode_ = IM_MARK;

		/// Current image file.
		std::string imageFile_;

		/// Surface object.
		Surface* surface_ = nullptr;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_MAINWINDOW_H
