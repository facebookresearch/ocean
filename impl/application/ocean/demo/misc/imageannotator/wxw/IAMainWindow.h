/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IA_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IA_MAIN_WINDOW_H

#include "application/ocean/demo/misc/imageannotator/wxw/ImageAnnotator.h"
#include "application/ocean/demo/misc/imageannotator/wxw/IAImageWindow.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Vector3.h"

#include <vector>

/**
 * This class implements the main window.
 * @ingroup applicationdemomiscimageannoatoarwxw
 */
class IAMainWindow : public wxFrame
{
	public:

		/**
		 * Definition of individual event ids.
		 */
		enum
		{
			/// Load image event id.
			ID_Load_Image = 1,
			/// Load lines event id.
			ID_Load_Lines,
			/// Event id for loading an image or lines.
			ID_Load_Image_Or_Lines,
			/// Event id for "Save Lines" events.
			ID_Save_Lines,
			/// Event id for "Save Groups" events.
			ID_Save_Groups,
			/// Quit event id.
			ID_Quit,
			/// About event id.
			ID_About,
			/// Line icon event id.
			ID_Icon_Line,
			/// Select icon event id.
			ID_Icon_Select,
			/// No zoom event id.
			ID_Nozoom,
			/// Event id for toggling showing lines.
			ID_Toggle_Show_Lines,
			/// Event id for toggling to or from the gradient image.
			ID_Toggle_Show_Gradients,
			/// Event id for toggling to or from the enhanced gradient image.
			ID_Toggle_Show_Gradients_Plus,
			/// Event id for "Detect LSD lines" menu entry.
			ID_Detect_Lines_LSD,
			/// Event id for "Detect ULF lines" menu entry.
			ID_Detect_Lines_ULF,
			/// Event id for the "Remove all lines" menu entry.
			ID_Remove_All_Lines,
			/// Event id for the "Evaluate lines" menu entry.
			ID_Evaluate_Lines,
			/// Event id for clustering lines.
			ID_Cluster_Lines,
			/// Toolbar event id.
			ID_Toolbar
		};

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the main window to be created
		 * @param pos Position of the main window
		 * @param size Size of the main window
		 */
		IAMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

		/**
		 * Loads a given image file.
		 * @param filename Name of the image file
		 * @return True, if succeeded
		 */
		bool loadImage(const std::string& filename);

		/**
		 * Loads lines from a given image file.
		 * @param filename Name of the file containing the lines
		 * @return True, if succeeded
		 */
		bool loadLines(const std::string& filename);

		/**
		 * Loads groups from a given image file.
		 * @param filename Name of the file containing the group information
		 * @return True, if succeeded
		 */
		bool loadGroups(const std::string& filename);

		/**
		 * Saves lines from to given line file.
		 * @param filename Name of the file which will receive the lines
		 * @return True, if succeeded
		 */
		bool saveLines(const std::string& filename);

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

		/**
		 * Load image event function.
		 * @param event Event command
		 */
		void onLoadImage(wxCommandEvent& event);

		/**
		 * Load lines event function.
		 * @param event Event command
		 */
		void onLoadLines(wxCommandEvent& event);

		/**
		 * Event function for loading an image or lines.
		 * @param event Event command
		 */
		void onLoadImageOrLines(wxCommandEvent& event);

		/**
		 * Saves lines, will open a save dialog followed by writing the file.
		 * @return True, if succeeded
		 */
		bool saveLines();

		/**
		 * Saves groups, will open a save dialog followed by writing the file.
		 * @return True, if succeeded
		 */
		bool saveGroups();

		/**
		 * Event function for saving the lines.
		 * @param event Event command
		 */
		void onSaveLines(wxCommandEvent& event);

		/**
		 * Event function for saving the groups.
		 * @param event Event command
		 */
		void onSaveGroups(wxCommandEvent& event);

		/**
		 * Event function for the 'line' icon.
		 * @param event Event command
		 */
		void onIconLine(wxCommandEvent& event);

		/**
		 * Event function for the 'select' icon.
		 * @param event Event command
		 */
		void onIconSelect(wxCommandEvent& event);

		/**
		 * On no zoom event function.
		 * @param event Event command
		 */
		void onNoZoom(wxCommandEvent& event);

		/**
		 * Event function for toggling showing lines.
		 * @param event Event command
		 */
		void onToggleShowLines(wxCommandEvent& event);

		/**
		 * Event function for toggling showing the image or gradients.
		 * @param event Event command
		 */
		void onToggleShowGradients(wxCommandEvent& event);

		/**
		 * Event function for toggling showing the image or gradients (plus).
		 * @param event Event command
		 */
		void onToggleShowGradientsPlus(wxCommandEvent& event);

		/**
		 * Event function for detecting LSD lines.
		 * @param event Event command
		 */
		void onDetectLinesLSD(wxCommandEvent& event);

		/**
		 * Event function for detecting ULF lines.
		 * @param event Event command
		 */
		void onDetectLinesULF(wxCommandEvent& event);

		/**
		 * Event function for removing all lines.
		 * @param event Event command
		 */
		void onRemoveAllLines(wxCommandEvent& event);

		/**
		 * Event function for evaluating lines.
		 * @param event Event command
		 */
		void onEvaluateLines(wxCommandEvent& event);

		/**
		 * Event function for clustering lines.
		 * @param event Event command
		 */
		void onClusterLines(wxCommandEvent& event);

		/**
		 * Quit event function.
		 * @param event Event command
		 */
		void onQuit(wxCommandEvent& event);

		/**
		 * Close event function.
		 * @param event Event command
		 */
		void onClose(wxCloseEvent& event);

		/**
		 * About event function.
		 * @param event Event command
		 */
		void onAbout(wxCommandEvent& event);

		/**
		 * On drag and drop file event function.
		 * @param files Event files
		 */
		bool onFileDragAndDrop(const std::vector<std::string> &files);

		/**
		 * Creates the gradient image for a given image.
		 * The gradient frame will have pixel format FORMAT_Y8 and will hold the maximal gradient (of horizontal, vertical, and diagonal gradient value).
		 * @param frame The image for which the gradient image will be created, must be valid
		 * @param gradientFrame The resulting radient image of the given frame
		 * @param gradientFramePlus The resulting enhanced radient image of the given frame (also including some additional filtering/image processing)
		 * @return True, if succeeded
		 */
		static bool createGradientImage(const Frame& frame, Frame& gradientFrame, Frame& gradientFramePlus);

		/**
		 * Determines the vanishing point for a given set of lines.
		 * @param lines The lines for which the common vanishing point will be determined, at least two
		 * @param parallelLinesTresholdAngle The maximal angle between two lines so that they count as parallel, in radian, with range (0, PI/2]
		 * @return The common vanishing point, may be finite or infinite
		 */
		static VectorD3 determineVanishingPoint(const FiniteLinesD2& lines, const double parallelLinesTresholdAngle = NumericD::deg2rad(3));

	private:

		/// Interaction mode.
		InteractionMode interactionMode_  = IM_LINE;

		/// Current image as loaded from the file.
		Frame image_;

		/// Gradient image of the current image.
		Frame gradientImage_;

		/// Gradient image of the current image plus some additional filtering/image processing.
		Frame gradientImagePlus_;

		/// Current image file.
		std::string imageFile_;

		/// Image window object.
		IAImageWindow* imageWindow_ = nullptr;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IA_MAIN_WINDOW_H
