/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IA_IMAGE_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IA_IMAGE_WINDOW_H

#include "application/ocean/demo/misc/imageannotator/wxw/ImageAnnotator.h"
#include "application/ocean/demo/misc/imageannotator/wxw/LineManager.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/LineEvaluator.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Vector2.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

#include <wx/graphics.h>

/**
 * This class implements the surface.
 * @ingroup applicationdemomiscimageannoatoarwxw
 */
class IAImageWindow : public Platform::WxWidgets::BitmapWindow
{
	private:

		/**
		 * Definition of individual line user interaction states.
		 */
		enum LineState
		{
			/// No specific user interaction.
			LS_IDLE,
			/// The user is defining start point of a line.
			LS_DEFINING_START_POINT,
			/// The user is defining end point of a line.
			LS_DEFINING_END_POINT,
			/// The user is adjusting the start point or end point of a line.
			LS_ADJUSTING_POINT,
			/// The user moves an entire line.
			LS_MOVE_LINE
		};

		/**
		 * Definition of individual select user interaction modes.
		 */
		enum SelectState
		{
			/// No specific user interaction.
			SS_IDLE,
			/// The user has pressed the mouse button without moving the cursor yet.
			SS_SELECTION_STARTED,
			/// The user is defining the second corner of the selection.
			SS_DEFINING_SECOND_CORNER,
			/// The user is selecting individual lines while pushing the Ctrl key.
			SS_SELECTING_INDIVIDUAL_LINES
		};

		/**
		 * Definition of individual drawing modes.
		 */
		enum DrawingMode
		{
			/// Lines are drawn without special highlighting.
			DM_NORMAL,
			/// Lines are highlighted.
			DM_HIGHLIGHT,
			/// Lines are highlighted with a second highlight color.
			DM_HIGHLIGHT_SECOND
		};

	public:

		/**
		 * Creates a new surface window object.
		 * @param parent Parent window
		 */
		explicit IAImageWindow(wxWindow* parent);

		/**
		 * Sets the interaction mode.
		 * @param mode Interaction mode to set
		 */
		inline void setInteractionMode(const InteractionMode mode);

		/**
		 * Enables or disables displaying any annotation.
		 * @param enable True, to show annotation; False, to hide annotation
		 */
		void enableAnnotation(const bool enable);

		/**
		 * Sets a line matching evaluation.
		 * @param lineEvaluationMap The map holding evaluation results
		 */
		void setLineEvaluationMap(CV::Detector::LineEvaluator::LineMatchMap&& lineEvaluationMap);

		/**
		 * Returns all lines which are currently used for interaction.
		 * @return Ids of all lines
		 */
		inline LineManager::LineIdSet interactionLineIds() const;

		/**
		 * Rests the interaction data of this window e.g., because a new image has been loaded.
		 */
		void resetInteractionData();

	private:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

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
		 * Left mouse click up event function.
		 * @see BitmapWindow::onMouseRightUp().
		 */
		void onMouseRightUp(wxMouseEvent& event) override;

		/**
		 * Paint event function.
		 * @see BitmapWindow::onPaintOverlay().
		 */
		void onPaintOverlay(wxPaintEvent& event, wxPaintDC& dc) override;

		/**
		 * Key down event function.
		 * @param event The event object
		 */
		void onKeyDown(wxKeyEvent& event);

		/**
		 * Key up event function.
		 * @param event The event object
		 */
		void onKeyUp(wxKeyEvent& event);

		/**
		 * Draws a set of lines in a graphics context.
		 * @param graphicsContext The graphics context in which the line will be drawn
		 * @param lines The lines to be drawn
		 * @param drawingMode The drawing mode to be used
		 */
		void drawLines(wxGraphicsContext& graphicsContext, const FiniteLinesD2& lines, const DrawingMode drawingMode);

		/**
		 * Draws a set of lines in a graphics context.
		 * @param graphicsContext The graphics context in which the line will be drawn
		 * @param lines The lines to be drawn
		 * @param backgroundColor The background color to be used
		 * @param foregroundColor the foreground color to be used
		 */
		void drawLines(wxGraphicsContext& graphicsContext, const FiniteLinesD2& lines, const wxColour& backgroundColor, const wxColour& foregroundColor);

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
		 * Draws a selection bounding box in a grahics context.
		 * @param graphicsContext The graphics context in which the line will be drawn
		 * @param boundingBox The bounding box of the selection to be drawn, must be valid
		 */
		void drawSelection(wxGraphicsContext& graphicsContext, const BoxD2& boundingBox);

		/**
		 * Draws a text in the graphics context in the upper left (visible) corner.
		 * @param dc The device context in which the text will be drawn
		 * @param texts Several lines of text to draw
		 */
		void drawText(wxPaintDC& dc, const std::vector<std::string>& texts);

		/**
		 * Calculates the virtual window position for a line defined in the coordinate system of the image.
		 * @param imageLine The line defined in the coordinate system of the image, must be valid
		 * @param virtualLine Resulting line defined in the coordinate system of the virtual window
		 * @return True, if the given line lies inside the bitmap
		 */
		bool image2virtualWindow(const FiniteLineD2& imageLine, FiniteLineD2& virtualLine);

		/**
		 * Clamps a finite line so that the end points fit into a given region.
		 * @param line The line to clamp, must be valid
		 * @param width The width of the region in pixel, with range [0, infinity)
		 * @param height The height of the region in pixel, with range [0, infinity)
		 * @return The resulting clamped line, with points located within [0, width)x[0, height), may be invalid
		 */
		static FiniteLineD2 clampLine(const FiniteLineD2& line, const unsigned int width, const unsigned int height);

		/**
		 * Returns the foreground color for a specified drawing mode.
		 * @param drawingMode The mode for which the color will be returned
		 * @return The resulting foreground color
		 */
		static const wxColour& foregroundColor(const DrawingMode drawingMode);

		/**
		 * Returns the background color for a specified drawing mode.
		 * @param drawingMode The mode for which the color will be returned
		 * @return The resulting background color
		 */
		static const wxColour& backgroundColor(const DrawingMode drawingMode);

		/**
		 * Returns individual colors for individual group indices.
		 * Although, any group index can be provided, only six individual colors exist, so that starting with index '6' the resulting colors start from the beginning.
		 * @param groupIndex The index of the group, with range [0, infinity)
		 * @return The color for the group
		 */
		static const wxColour& groupColor(const unsigned int groupIndex);

	private:

		// Interaction mode.
		InteractionMode interactionMode_ = IM_LINE;

		// The current line state.
		LineState lineState_ = LS_IDLE;

		/// The current select state.
		SelectState selectState_ = SS_IDLE;

		/// The start point of a new line, if any.
		VectorD2 lineStartPoint_ = VectorD2(0.0, 0.0);;

		/// The first corner of a selection, if any.
		VectorD2 selectionFirstCorner_;

		/// The ids of the line which are currently used for interaction.
		LineManager::LineIdSet lineIds_;

		/// The id of a line which is currently closest to the cursor position.
		LineManager::LineId closestLineId_ = LineManager::invalidId;

		/// The index of the end point of the line which is currently used for interaction.
		unsigned int linePointIndex_ = (unsigned int)(-1);

		/// The current mouse cursor position.
		VectorD2 interactionCursorCurrent_ = VectorD2(0.0, 0.0);;

		/// The offset between the mouse cursor position and an interaction object when pushing the mouse button.
		VectorD2 interactionCursorOffset_ = VectorD2(0.0, 0.0);;

		/// The maximal distance between an object and the mouse cursor to enable an interaction.
		double maximalInteractionDistance_ = 10.0;

		/// True, if the Alt key is currently pushed.
		bool altKeyDown_ = false;

		/// True, to show any annotation; False, to hide any annotation.
		bool annotationEnabled_ = true;

		/// The minimal length of a valid line, smaller lines cannot be annotated.
		double minimalLineLength_ = 2.0;

		/// The evaluation map for line matches.
		CV::Detector::LineEvaluator::LineMatchMap lineEvaluationMap_;

		/// The map mapping target ids to source ids.
		CV::Detector::LineEvaluator::IdToIdSetMap lineEvaluationReverseMap_;
};

inline void IAImageWindow::setInteractionMode(const InteractionMode mode)
{
	interactionMode_ = mode;
}

inline LineManager::LineIdSet IAImageWindow::interactionLineIds() const
{
	return lineIds_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_IA_IMAGE_WINDOW_H
