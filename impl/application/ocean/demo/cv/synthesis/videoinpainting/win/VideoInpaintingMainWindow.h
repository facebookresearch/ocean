/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_VIDEO_INPAINTING_WIN_VIDEO_INPAINTING_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_VIDEO_INPAINTING_WIN_VIDEO_INPAINTING_MAINWINDOW_H

#include "application/ocean/demo/cv/synthesis/videoinpainting/win/VideoInpainting.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"
#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/cv/segmentation/ContourTracker.h"

#include "ocean/cv/synthesis/MappingF1.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/tracking/HomographyTracker.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsynthesisvideoinpaintingwin
 */
class VideoInpaintingMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	protected:

		/**
		 * Definition of individual inpainting modes.
		 */
		enum InpaintingMode
		{
			/// Unknown inpainting mode, e.g., before the user has decided which mode to use
			IM_UNKNOWN,
			/// Contour-based inpainting mode, the undesired object is enclosed by a contour which can change from frame to frame (up to some extend).
			IM_CONTOUR_BASED,
			/// Homography-mask-based inpainting mode, a mask will defined the undesired area while tracking is mainly based on a homography.
			IM_HOMOGRAPHY_MASK_BASED
		};

		/**
		 * Definition of individual contour states.
		 */
		enum ContourState
		{
			/// Idle state.
			CS_IDLE,
			/// The user is currently defining the rough contour.
			CS_DEFINING_ROUGH_CONTOUR,
			/// The user has completed the rough contour, now the fine contour based on the visual information needs to be determined.
			CS_DETERMINE_FINE_CONTOUR,
			/// The fine contour has been determined and now the enclosed area will be inpainted in each new frame.
			CS_CONTOUR_DEFINED_SUCCESSFULLY
		};

		/**
		 * Definition of individual mask states.
		 */
		enum MaskState
		{
			/// The Idle state.
			MS_IDLE,
			/// The user is currently defining the mask.
			MS_DEFINING_MASK,
			/// The user has finished to define the mask and now the covered area will be inpainted in each new frame.
			MS_MASK_DEFINED_SUCCESSFULLY
		};

		/**
		 * Definition of a simple color adjustment object able to store color differences for individual image positions.
		 * This object can be used to adjust the color values of an image e.g., due to ambient lighting changes.
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		class ColorAdjustmentObject : public Vector2
		{
			public:

				/**
				 * Definition of a vector holding color adjustment objects.
				 */
				typedef std::vector<ColorAdjustmentObject<tChannels>> ColorAdjustmentObjects;

			public:

				/**
				 * Creates a new adjustment object based on a location and two corresponding color values.
				 * The resulting adjustment is the channel-wise difference between the second and the first color values, i.e. valuesB[i] - valuesA[i]
				 * @param position The position of the adjustment object in pixel
				 * @param valuesA The first color values, one for each channel
				 * @param valuesB The second color values, one for each channel
				 */
				inline ColorAdjustmentObject(const Vector2& position, const unsigned char* valuesA, const unsigned char* valuesB);

				/**
				 * Creates a new adjustment object based on two already existing adjustment objects by merging both adjustments together.
				 * The location of the resulting adjustment object will be in the middle between both given adjustment objects.
				 * @param adjustment0 The first adjustment object
				 * @param adjustment1 The second adjustment object
				 */
				inline ColorAdjustmentObject(const ColorAdjustmentObject<tChannels>& adjustment0, const ColorAdjustmentObject<tChannels>& adjustment1);

			public:

				/**
				 * Returns a specific adjustment value.
				 * @param channel The index of the value to be returned, with range [0, tChannels)
				 */
				inline Scalar delta(const unsigned int channel) const;

			protected:

				/// The individual adjustment values, one for each frame channel, with range [-255, 255]
				Scalar deltas_[tChannels];
		};

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param mediaFilename Optional name of the media file to be used
		 * @param frameSize Optional preferred frame size, e.g. "640x480"
		 */
		VideoInpaintingMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFilename, const std::string& frameSize);

		/**
		 * Destructs the main window.
		 */
		~VideoInpaintingMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for mouse button down events.
		 * @see Window::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for mouse move event.
		 * @see Window::onMouseMove().
		 */
		void onMouseMove(const MouseButton buttons, const int x, const int y) override;

		/**
		 * Function for mouse double click events.
		 * @see Window::onMouseDoubleClick().
		 */
		void onMouseDoubleClick(const MouseButton button, const int x, const int y) override;

		/**
		 * Toggles the fullscreen state of the application.
		 */
		void onToggleFullscreen();

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Invokes the contour defining for the contour-based inpainting mode.
		 * Call this function as long as the user is defining the rough contour or has just finished to define the rough contour.
		 * @return True, if succeeded
		 */
		bool definingContour();

		/**
		 * Invokes the contour-based inpainting.
		 * Do not call this function before the fine contour around the undesired object has been determined successfully.
		 * @return True, if succeeded
		 */
		bool contourBasedInpainting();

		/**
		 * Invokes the mask defining for the mask-based inpainting mode.
		 * Call this function as long as the user is defining the mask or has just finished to define the mask.
		 * @return True, if succeeded
		 */
		bool definingMask();

		/**
		 * Invokes the mask(-homography)-based inpainting.
		 * Do not call this function before the mask covering the undesired object has been determined successfully.
		 * @return True, if succeeded
		 */
		bool maskBasedInpainting();

		/**
		 * Invokes the core inpainting for either the contour-based or mask-based mode as the core inpainting approach is identical for both modes.
		 * @param currentHomographyPrevious The dominant homography transforming points defined in the previous frame to points defined in the current frame
		 * @param pixelContour The dense pixel contour enclosing the undesired object in the current frame
		 * @return True, if succeeded
		 */
		bool coreInpainting(const SquareMatrix3& currentHomographyPrevious, const CV::Segmentation::PixelContour& pixelContour);

		/**
		 * Resets the entire video inpainting process.
		 * Releases all resources and intermediate information.<br>
		 * The application will act as just started.
		 */
		void reset();

		/**
		 * Creates a reference frame for the current frame based on a previously inpainting frame (e.g., the very first one or a specific keyframe).
		 * The previously inpainted frame will be transformed so that it matches with the current frame by application of a homography.
		 * @param currentFrame The current frame for which the reference frame will be created, must be valid
		 * @param currentMask The current mask identifying the undesired object, with same frame dimension and pixel origin as the current frame, must be valid
		 * @param inpaintingReferenceFrame The previously inpainted frame which will provide the visual content for the resulting reference frame, with same frame dimension and pixel origin as the current frame, must be valid
		 * @param inpaintingReferenceFrameQuarter The previously inpainted frame with a quarter of the original resolution, must be valid
		 * @param referenceHomographyCurrent The homography transforming points defined in the current frame to points defined in the previously inpainted frame
		 * @param contourPoints The dense mask border contour points matching with the provided mask
		 * @param trackingMaskBoundingBox The bounding box of the mask (and the bounding box of the provided contour points)
		 * @param referenceFrame The resulting reference frame composed of the previously inpainted frame (for mask pixels) and the current frame (for non-mask pixels)
		 * @param worker Optional worker object to distribute the computation
		 */
		static void createReferenceFrame(const Frame& currentFrame, const Frame& currentMask, const Frame& inpaintingReferenceFrame, const Frame& inpaintingReferenceFrameQuarter, const SquareMatrix3& referenceHomographyCurrent, const CV::PixelPositions& contourPoints, const CV::PixelBoundingBox& trackingMaskBoundingBox, Frame& referenceFrame, Worker* worker = nullptr);

		/**
		 * Highlights an area within a frame defined by a mask, and optional highlights the contour of the mask.
		 * @param frame The frame in which the (corresponding mask) area will be highlighted, must be valid
		 * @param mask The 8 bit mask defining which area will be highlighted (in the frame), with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param boundingBox Optional bounding box to improved the computation
		 * @param contourPixels Optional pixel location of the mask's contour which will be highlighted in addition, if defined with range (-infinity, infinity)x(-infinity, infinity)
		 */
		static void highlightMask(Frame& frame, const Frame& mask, const CV::PixelBoundingBox& boundingBox = CV::PixelBoundingBox(), const CV::PixelPositions& contourPixels = CV::PixelPositions());

	protected:

		/// The current inpainting mode of the application.
		InpaintingMode inpaintingMode_ = IM_UNKNOWN;

		/// The current state of the contour, if the application is in the contour-based inpainting mode.
		ContourState contourState_ = CS_IDLE;

		/// The current state of the mask, if the application is in the mask-based inpainting mode.
		MaskState maskState_ = MS_IDLE;

		/// The individual pixel positions of the user-defined rough contour.
		CV::PixelPositions userDefinedRoughContour_;

		/// The current frame with upper left corner as pixel origin.
		Frame currentFrame_;

		/// The previous frame as 8 bit grayscale frame with upper left corner as pixel origin, used for the mask-based inpainting only.
		Frame yPrevousFrame_;

		/// The first frame that has been inpainted which will be used as reference for all successive frames.
		Frame firstInpaintingFrame_;

		/// The first frame that has been inpainted with a quarter of the original resolution.
		Frame firstInpaintingFrameQuarter_;

		/// The frame holding the (intermediate) synthesis result, defined as member object to avoid to re-create a new frame during each synthesis iteration.
		Frame synthesisResult_;

		/// The frame holding the reference content for the image synthesis, defined as member object to avoid to re-create a new frame during each synthesis iteration.
		Frame referenceFrame_;

		/// The 8 bit mask for the current frame covering the undesired object, width upper left corner as pixel origin.
		Frame currentMask_;

		/// The most dominant homography transforming points defined in the most recent frame to points defined in the first inpainting frame.
		SquareMatrix3 firstHomographyRecent_ = SquareMatrix3(true);

		/// Synthesis mapping of the previous frame.
		CV::Synthesis::MappingF1 previousMapping_;

		/// The frame medium object providing the visual information to be inpainted.
		Media::FrameMediumRef frameMedium_;

		/// The random number generator object needed in some functions.
		RandomGenerator randomGenerator_;

		/// The contour tracker allowing to determine and to track a fine contour from a given rough user-defined contour.
		CV::Segmentation::ContourTracker contourTracker_;

		/// The homography tracker allowing to determine a dominant homography from a set of image points.
		Tracking::HomographyTracker homographyTracker_;

		/// The image points around the mask in the previous frame, used for the mask-based inpainting only.
		Vectors2 homographyMaskPreviousPoints_;

		/// The new used-defined position of a mask blob, used for the mask-based inpainting only, an invalid position if no further point is desired.
		CV::PixelPosition homographyMaskNewBlobPosition_;

		/// Most recent timestamp of the input frame data.
		Timestamp frameTimestamp_;

		/// Optional media file to be used as input data.
		std::string mediaFilename_;

		/// Optional preferred frame size of the input data.
		std::string preferredMediaFrameSize_;

		/// True, if the application is in fullscreen mode.
		bool isFullscreen_ = false;

		/// Non fullscreen window style.
		int nonFullScreenStyle_ = 0;

		/// Worker object to distribute the computation.
		Worker worker_;
};

template <unsigned int tChannels>
inline VideoInpaintingMainWindow::ColorAdjustmentObject<tChannels>::ColorAdjustmentObject(const Vector2& position, const unsigned char* valuesA, const unsigned char* valuesB) :
	Vector2(position)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		deltas_[n] = Scalar(int(valuesB[n]) - int(valuesA[n]));
	}
}

template <unsigned int tChannels>
inline VideoInpaintingMainWindow::ColorAdjustmentObject<tChannels>::ColorAdjustmentObject(const ColorAdjustmentObject& adjustment0, const ColorAdjustmentObject& adjustment1) :
	Vector2((adjustment0 + adjustment1) * Scalar(0.5))
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		deltas_[n] = (adjustment0.deltas_[n] + adjustment1.deltas_[n]) * Scalar(0.5);
	}
}

template <unsigned int tChannels>
inline Scalar VideoInpaintingMainWindow::ColorAdjustmentObject<tChannels>::delta(const unsigned int channel) const
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(channel < tChannels);
	return deltas_[channel];
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SYNTHESIS_VIDEO_INPAINTING_WIN_VIDEO_INPAINTING_MAINWINDOW_H
