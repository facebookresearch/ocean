/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNONATOR_IMAGE_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNONATOR_IMAGE_WINDOW_H

#include "application/ocean/demo/misc/imageannotator/wxw/ImageAnnotator.h"
#include "application/ocean/demo/misc/imageannotator/wxw/LineManager.h"

#include "ocean/base/Frame.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Vector2.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

#include <wx/graphics.h>

/**
 * This class implements the surface.
 * @ingroup applicationdemomiscimageannoatoarwxw
 */
class ImageAnnotatorImageWindow : public Platform::WxWidgets::BitmapWindow
{
	private:

		/**
		 * Definition of individual line modes.
		 */
		enum LineMode
		{
			/// No specific user interaction.
			LM_IDLE,
			/// The user is defining start point of a line.
			LM_DEFINING_START_POINT,
			/// The user is defining end point of a line.
			LM_DEFINING_END_POINT,
			/// The user is adjusting the start point or end point of a line.
			LM_ADJUSTING_POINT,
			/// The user moves an entire line.
			LM_MOVE_LINE
		};

		/**
		 * Definition of individual drawing modes.
		 */
		enum DrawingMode
		{
			DM_NORMAL,
			DM_HIGHLIGHT
		};

	public:

		/**
		 * Creates a new surface window object.
		 * @param parent Parent window
		 */
		explicit ImageAnnotatorImageWindow(wxWindow* parent);

		/**
		 * Sets the interaction mode.
		 * @param mode Interaction mode to set
		 */
		inline void setInteractionMode(const InteractionMode mode);

	private:

		/**
		 * Left mouse click down event function.
		 * @see BitmapWindow::onMouseLeftDown().
		 */
		void onMouseLeftDown(wxMouseEvent& event) override;

		/**
		 * Right mouse click down event function.
		 * @see BitmapWindow::onMouseRightDown().
		 */
		void onMouseRightDown(wxMouseEvent& event) override;

		/**
		 * Right mouse double click down event function.
		 * @see BitmapWindow::onMouseRightDblClick().
		 */
		void onMouseRightDblClick(wxMouseEvent& event) override;

		/**
		 * Mouse move event function.
		 * @see BitmapWindow::onMouseMove().
		 */
		void onMouseMove(wxMouseEvent& event) override;

		/**
		 * Left mouse click up event function.
		 * @see BitmapWindow::onMouseLeftUp().
		 */
		void onMouseLeftUp(wxMouseEvent& event) override;

		/**
		 * Paint event function.
		 * @see BitmapWindow::onPaintOverlay().
		 */
		void onPaintOverlay(wxPaintEvent& event, wxPaintDC& dc) override;

		/**
		 * Draws a set of lines in a graphics context.
		 * @param graphicsContext The graphics context in which the line will be drawn
		 * @param lines The lines to be drawn
		 * @param drawingMode The drawing mode to be used
		 */
		void drawLines(wxGraphicsContext& graphicsContext, const FiniteLinesD2& lines, const DrawingMode drawingMode);

		/**
		 * Draws a line in a graphics context.
		 * @param graphicsContext The graphics context in which the line will be drawn
		 * @param line The line to be drawn, must be valid
		 * @param drawingMode The drawing mode to be used
		 */
		void drawLine(wxGraphicsContext& graphicsContext, const FiniteLineD2& line, const DrawingMode drawingMode);

		/**
		 * Draws an anchor in a graphics context (a rectangle around a specified position).
		 * @param graphicsContext The graphics context in which the anchor will be drawn
		 * @param position The center position of the anchor
		 * @param drawingMode The drawing mode to be used when drawing the anchor
		 */
		void drawAnchor(wxGraphicsContext& graphicsContext, const VectorD2& position, const DrawingMode drawingMode);

		/**
		 * Calculates the virtual window position for a line defined in the coordinate system of the image.
		 * @param imageLine The line defined in the coordinate system of the image, must be valid
		 * @param virtualLine Resulting line defined in the coordinate system of the virtual window
		 * @return True, if the given line lies inside the bitmap
		 */
		bool image2virtualWindow(const FiniteLineD2& imageLine, FiniteLineD2& virtualLine);

	private:

		wxFrame* zoomWindow_;

		// Interaction mode.
		InteractionMode interactionMode_;

		// The current line mode.
		LineMode lineMode_;

		/// The start point of a new line, if any.
		VectorD2 lineStartPoint_;

		/// The id of the line which is currently used for interaction.
		LineManager::LineId lineId_;

		/// The index of the end point of the line which is currently used for interaction.
		unsigned int linePointIndex_;

		/// The current mouse cursor position.
		VectorD2 interactionCursorCurrent_;

		/// The offset between the mouse cursor position and an interaction object when pushing the mouse button.
		VectorD2 interactionCursorOffset_;

		/// The maximal distance between an object and the mouse cursor to enable an interaction.
		double maximalInteractionDistance_;
};

inline void ImageAnnotatorImageWindow::setInteractionMode(const InteractionMode mode)
{
	interactionMode_ = mode;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IMAGE_ANNONATOR_IMAGE_WINDOW_H
