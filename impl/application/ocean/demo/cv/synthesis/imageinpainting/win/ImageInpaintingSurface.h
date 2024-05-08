/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_SURFACE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_SURFACE_H

#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpainting.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/math/Vector2.h"

#include "ocean/platform/wxwidgets/BitmapWindow.h"

/**
 * This class implements the surface.
 * @ingroup applicationdemocvsynthesisimageinpaintingwxw
 */
class Surface : public Platform::WxWidgets::BitmapWindow
{
	private:

		/**
		 * Definition of individual line modes.
		 */
		enum LineMode
		{
			/// No specific line mode.
			LM_NONE,
			/// First point of the line.
			LM_FIRST
		};

		/**
		 * Definition of a pair holding two 2D vectors.
		 */
		typedef std::pair<Vector2, Vector2> Line;

		/**
		 * Definition of a vector holding vector pairs.
		 */
		typedef std::vector<Line> Lines;

	public:

		/**
		 * Creates a new surface window object.
		 * @param parent The parent window
		 */
		explicit Surface(wxWindow* parent);

		/**
		 * Returns the frame of this bitmap window.
		 * @return Bitmap frame
		 */
		inline const Frame& frame() const;

		/**
		 * Returns the mask of this bitmap window.
		 * @return Mask frame
		 */
		inline const Frame& mask() const;

		/**
		 * Sets or changes the frame of this bitmap window.
		 * @param frame The frame to be set
		 * @return True, if succeeded
		 */
		bool setFrame(const Frame& frame);

		/**
		 * Sets or changes the mask of this bitmap window.
		 * @param mask The mask to be set
		 * @return True, if succeeded
		 */
		bool setMask(const Frame& mask);

		/**
		 * Executes the inpainting.
		 * @param quality Resulting inpainting quality, with range [0, 3]
		 * @return True, if succeeded
		 */
		bool executeInpainting(const unsigned int quality);

		/**
		 * Executes the inpainting.
		 */
		bool executeInpaintingTexture();

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
		virtual void onMouseLeftDown(wxMouseEvent& event);

		/**
		 * Right mouse click down event function.
		 * @see BitmapWindow::onMouseRightDown().
		 */
		virtual void onMouseRightDown(wxMouseEvent& event);

		/**
		 * Right mouse double click down event function.
		 * @see BitmapWindow::onMouseRightDblClick().
		 */
		virtual void onMouseRightDblClick(wxMouseEvent& event);

		/**
		 * Mouse move event function.
		 * @see BitmapWindow::onMouseMove().
		 */
		virtual void onMouseMove(wxMouseEvent& event);

		/**
		 * Paint event function.
		 * @see BitmapWindow::onPaintOverlay().
		 */
		virtual void onPaintOverlay(wxPaintEvent& event, wxPaintDC& dc);

		/**
		 * Updates the internal bitmap.
		 */
		void updateBitmap();

		/**
		 * Updates the internal bitmap.
		 * @param firstRow First row to be handled
		 * @param numberRows Number rows to be handled
		 */
		void updateBitmapSubset(const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Updates the internal mask.
		 * @param xWindow Horizontal window position
		 * @param yWindow Vertical window position
		 * @param factor Size factor
		 * @param value Mask value
		 * @return True, if succeeded
		 */
		bool updateMask(const int xWindow, const int yWindow, const unsigned int factor, const unsigned char value);

	private:

		/// Inpainting frame.
		Frame frame_;

		/// Inpainting mask.
		Frame mask_;

		/// The previous inpainting mask.
		Frame previousMask_;

		/// WxWidgets image.
		wxImage image_;

		/// Random number generator.
		RandomGenerator randomGenerator_;

		/// Start point of a user-defined line.
		Vector2 lineStartPoint_ = Vector2(0, 0);

		/// User-defined lines.
		Lines lines_;

		// Interaction mode.
		InteractionMode interactionMode_ = IM_MARK;

		// Line mode.
		LineMode lineMode_ = LM_NONE;
};

inline const Frame& Surface::frame() const
{
	return frame_;
}

inline const Frame& Surface::mask() const
{
	if (previousMask_)
	{
		return previousMask_;
	}

	return mask_;
}

inline void Surface::setInteractionMode(const InteractionMode mode)
{
	interactionMode_ = mode;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_IMAGEINPAINTING_WXW_IMAGEINPAINTING_SURFACE_H
