/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/synthesis/videoinpainting/win/VideoInpaintingMainWindow.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

#include "ocean/cv/detector/LineDetectorHough.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/MaskCreator.h"

#include "ocean/cv/synthesis/CreatorInpaintingContentF1.h"
#include "ocean/cv/synthesis/InitializerHomographyMappingAdaptionF1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodReferenceFrameF1.h"
#include "ocean/cv/synthesis/SynthesisPyramidI1.h"

#include "ocean/geometry/Homography.h"

#include "ocean/math/Lookup2.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/Screen.h"

// #define USE_SYNTHESIS_CONSTRAINTS

VideoInpaintingMainWindow::VideoInpaintingMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFilename, const std::string& frameSize) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFilename_(mediaFilename),
	preferredMediaFrameSize_(frameSize)
{
	// nothing to do here
}

VideoInpaintingMainWindow::~VideoInpaintingMainWindow()
{
	// nothing to do here
}

void VideoInpaintingMainWindow::onInitialized()
{
	if (!mediaFilename_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFilename_, Media::Medium::FRAME_MEDIUM);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_)
	{
		if (preferredMediaFrameSize_ == "640x480")
		{
			frameMedium_->setPreferredFrameDimension(640u, 480u);
		}

		else if (preferredMediaFrameSize_ == "640x400")
		{
			frameMedium_->setPreferredFrameDimension(640u, 400u);
		}

		if (preferredMediaFrameSize_ == "1280x720")
		{
			frameMedium_->setPreferredFrameDimension(1280u, 720u);
		}

		if (preferredMediaFrameSize_ == "1920x1080")
		{
			frameMedium_->setPreferredFrameDimension(1920u, 1080u);
		}

		frameMedium_->start();
	}
}

void VideoInpaintingMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && (*frame && frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Thread::sleep(1u);
}

void VideoInpaintingMainWindow::onMouseDown(const MouseButton button, const int x, const int y)
{
	if (button & BUTTON_LEFT)
	{
		// we reset the application ensuring that any intermediate information has been released
		reset();

		// the user wants to define a contour around the undesired object
		inpaintingMode_ = IM_CONTOUR_BASED;

		// the user now will define a rough contour
		ocean_assert(contourState_ == CS_IDLE);
		contourState_ = CS_DEFINING_ROUGH_CONTOUR;

		// we set the current mouse position as first contour position
		ocean_assert(userDefinedRoughContour_.empty());

		int bitmapX, bitmapY;
		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
		{
			userDefinedRoughContour_.push_back(CV::PixelPosition((unsigned int)(bitmapX), (unsigned int)(bitmapY)));
		}
	}
	else if (button & BUTTON_RIGHT)
	{
		// we reset the application ensuring that any intermediate information has been released
		reset();

		// the user wants to define a mask covering the undesired object
		inpaintingMode_ = IM_HOMOGRAPHY_MASK_BASED;

		// the user now will define the mask
		ocean_assert(maskState_ == MS_IDLE);
		maskState_ = MS_DEFINING_MASK;

		int bitmapX, bitmapY;
		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
		{
			homographyMaskNewBlobPosition_ = CV::PixelPosition((unsigned int)(bitmapX), (unsigned int)(bitmapY));
		}
	}
	else if (button & BUTTON_MIDDLE)
	{
		reset();
	}
}

void VideoInpaintingMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	if (inpaintingMode_ == IM_CONTOUR_BASED)
	{
		ocean_assert(contourState_ == CS_DEFINING_ROUGH_CONTOUR);
		if (userDefinedRoughContour_.size() >= 3)
		{
			contourState_ = CS_DETERMINE_FINE_CONTOUR;
		}
		else
		{
			reset();
		}
	}
	else if (inpaintingMode_ == IM_HOMOGRAPHY_MASK_BASED)
	{
		ocean_assert(maskState_ == MS_DEFINING_MASK);
		maskState_ = MS_MASK_DEFINED_SUCCESSFULLY;
	}
}

void VideoInpaintingMainWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	if (inpaintingMode_ == IM_CONTOUR_BASED)
	{
		if ((buttons & BUTTON_LEFT) && contourState_ == CS_DEFINING_ROUGH_CONTOUR)
		{
			int bitmapX, bitmapY;
			if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
			{
				userDefinedRoughContour_.push_back(CV::PixelPosition((unsigned int)(bitmapX), (unsigned int)(bitmapY)));
			}
		}
	}
	else if (inpaintingMode_ == IM_HOMOGRAPHY_MASK_BASED)
	{
		if ((buttons & BUTTON_RIGHT) && maskState_ == MS_DEFINING_MASK)
		{
			int bitmapX, bitmapY;
			if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
			{
				homographyMaskNewBlobPosition_ = CV::PixelPosition((unsigned int)(bitmapX), (unsigned int)(bitmapY));
			}
		}
	}
}

void VideoInpaintingMainWindow::onMouseDoubleClick(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	onToggleFullscreen();
}

void VideoInpaintingMainWindow::onToggleFullscreen()
{
	if (isFullscreen_)
	{
		SetWindowLongPtrA(handle(), GWL_STYLE, nonFullScreenStyle_);
		nonFullScreenStyle_ = 0;

		int left = -1000;
		int top = -1000;
		int width = -1000;
		int height = -1000;

		unsigned int flag = 0;
		if (left == -1000 || top == -1000)
		{
			flag |= SWP_NOMOVE;
		}
		if (width == -1000 || height == -1000)
		{
			flag |= SWP_NOSIZE;
		}

		const int virtualDisplayWidth = Platform::Win::Screen::virtualDisplayWidth();
		const int virtualDisplayHeight = Platform::Win::Screen::virtualDisplayHeight();

		if ((left != -1000 && (left >= virtualDisplayWidth || (width != -1000 && left + width <= 0) || (width == -1000 && left <= 0)))
				|| (top != -1000 && (top >= virtualDisplayHeight || (height != -1000 && top + height <= 0) || (height == -1000 && top <= 0))))
		{
			flag |= SWP_NOMOVE;
			flag |= SWP_NOSIZE;
		}

		::SetWindowPos(handle(), HWND_NOTOPMOST, left, top, width, height, flag);

		isFullscreen_ = false;

	}
	else
	{
		isFullscreen_ = true;

		// store current window style
		ocean_assert(nonFullScreenStyle_ == 0);

		// remove main window border
		nonFullScreenStyle_ = int(SetWindowLongPtr(handle(), GWL_STYLE, WS_VISIBLE));

		unsigned int screenLeft, screenTop, screenWidth, screenHeight;
		if (!Platform::Win::Screen::screen(handle(), screenLeft, screenTop, screenWidth, screenHeight))
		{
			ocean_assert(false && "Invalid screen");
		}

		::SetWindowPos(handle(), HWND_TOPMOST, int(screenLeft), int(screenTop), int(screenWidth), int(screenHeight), SWP_SHOWWINDOW);
	}
}

void VideoInpaintingMainWindow::onFrame(const Frame& frame)
{
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, currentFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (inpaintingMode_ == IM_CONTOUR_BASED)
	{
		ocean_assert(maskState_ == MS_IDLE);

		if (contourState_ == CS_DEFINING_ROUGH_CONTOUR || contourState_ == CS_DETERMINE_FINE_CONTOUR)
		{
			// the user still is defining the contour
			if (!definingContour())
			{
				reset();
			}
		}
		else
		{
			// the fine contour has been determined, so we apply the actual inpainting in every frame
			if (!contourBasedInpainting())
			{
				reset();
			}
		}
	}
	else if (inpaintingMode_ == IM_HOMOGRAPHY_MASK_BASED)
	{
		ocean_assert(contourState_ == CS_IDLE);

		// as we use the previous gray frame for the mask-based video inpainting only, we create the video for this case only

		Frame yCurrent;
		if (!CV::FrameConverter::Comfort::convert(currentFrame_, FrameType::FORMAT_Y8, yCurrent, CV::FrameConverter::CP_ALWAYS_COPY, &worker_))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		if (maskState_ == MS_DEFINING_MASK)
		{
			// the user still is defining the mask
			if (!definingMask())
			{
				reset();
			}
		}
		else
		{
			// the fine contour has been determined, so we apply the actual inpainting in every frame
			if (!maskBasedInpainting())
			{
				reset();
			}
		}

		// in the case no reset has been invoked above
		if (inpaintingMode_ == IM_HOMOGRAPHY_MASK_BASED)
		{
			yPrevousFrame_ = std::move(currentFrame_);
			yPrevousFrame_.makeOwner();
		}
	}

	if (inpaintingMode_ == IM_UNKNOWN)
	{
		ocean_assert(currentFrame_);
		setFrame(currentFrame_);
	}

	repaint();
}

bool VideoInpaintingMainWindow::definingContour()
{
	ocean_assert(inpaintingMode_ == IM_CONTOUR_BASED);
	ocean_assert(contourState_ == CS_DEFINING_ROUGH_CONTOUR || contourState_ == CS_DETERMINE_FINE_CONTOUR);

	ocean_assert(currentFrame_ && currentFrame_.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	setFrame(currentFrame_);

	switch (contourState_)
	{
		case CS_DEFINING_ROUGH_CONTOUR:
		{
			// the user is currently defining the rough contour, so we simply draw the contour

			if (!userDefinedRoughContour_.empty())
			{
				HPEN pen = CreatePen(0, 5, 0xFF);
				HPEN oldPen = (HPEN)SelectObject(bitmap().dc(), pen);

				MoveToEx(bitmap().dc(), int(userDefinedRoughContour_.front().x()), int(userDefinedRoughContour_.front().y()), nullptr);
				for (size_t n = 1; n < userDefinedRoughContour_.size(); ++n)
				{
					const int x = int(userDefinedRoughContour_[n].x());
					const int y = int(userDefinedRoughContour_[n].y());

					LineTo(bitmap().dc(), x, y);
				}

				(HPEN)SelectObject(bitmap().dc(), oldPen);
				DeleteObject(pen);
			}

			return true;
		}

		case CS_DETERMINE_FINE_CONTOUR:
		{
			if (!contourTracker_.detectObject(currentFrame_, CV::Segmentation::PixelContour(true, false, userDefinedRoughContour_), randomGenerator_, 10u, &worker_) || contourTracker_.denseContourSubPixel().size() < 3) // **TODO** explicit gray frame if available?
				return false;

			// the undesired object enclosed in the rough user-defined contour could be determined successfully, so we proceed with the actual inpainting
			contourState_ = CS_CONTOUR_DEFINED_SUCCESSFULLY;

			return true;
		}

		case CS_IDLE:
		case CS_CONTOUR_DEFINED_SUCCESSFULLY:
			break;
	}

	ocean_assert(false && "Invalid state!");
	return false;
}

bool VideoInpaintingMainWindow::contourBasedInpainting()
{
	ocean_assert(inpaintingMode_ == IM_CONTOUR_BASED);
	ocean_assert(contourState_ == CS_CONTOUR_DEFINED_SUCCESSFULLY);

	ocean_assert(currentFrame_ && currentFrame_.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	// first we track the contour from the previous frame to the current frame
	if (!contourTracker_.trackObject(currentFrame_, randomGenerator_, 10u, &worker_) || contourTracker_.denseContourSubPixel().size() < 3) // **TODO** explicit gray frame if available?
	{
		return false;
	}

	// we check whether the new contour has left the frame so that we stop here, thus we simply do not re-track an object which has left the camera frame
	for (const CV::PixelPosition& pixel : contourTracker_.denseContour().pixels())
	{
		if (pixel.x() >= currentFrame_.width() || pixel.y() >= currentFrame_.height())
		{
			return false;
		}
	}

	// now as we have the dominant homography and the accurate/fine contour for the current frame we invoke the core inpainting approach with is identical for either the contour-based or mask-based mode

	return coreInpainting(contourTracker_.homography(), contourTracker_.denseContour());
}

bool VideoInpaintingMainWindow::definingMask()
{
	ocean_assert(inpaintingMode_ == IM_HOMOGRAPHY_MASK_BASED);
	ocean_assert(maskState_ == MS_DEFINING_MASK);

	// first, we determine the most dominant homography between the previous and the current frame, mainly based on the mask's border locations in the previous and current frame
	// in the case, the homography determination fails we simply take the previous points as current points (we also could stop here, as the tracking situation seems to be very complex)

	SquareMatrix3 currentHomographyPrevious;
	if (homographyTracker_.trackPoints(currentFrame_, yPrevousFrame_, randomGenerator_, homographyMaskPreviousPoints_, currentHomographyPrevious, &worker_)) // we call also for the first frame so that the tracker can store a frame pyramid internally
	{
		homographyMaskPreviousPoints_ = Tracking::HomographyTracker::transformPoints(homographyMaskPreviousPoints_, currentHomographyPrevious);
	}

	currentMask_.set(FrameType(currentFrame_, FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/);
	currentMask_.setValue(0xFFu);

	// then, we paint the mask from the previous frame into the current mask (so that the mask does not get lost as time goes by)

	CV::Segmentation::PixelContour maskPixelContour;

	if (!homographyMaskPreviousPoints_.empty())
	{
		maskPixelContour = CV::Segmentation::PixelContour(CV::PixelPosition::vectors2pixelPositions(homographyMaskPreviousPoints_, currentFrame_.width(), currentFrame_.height()));
		maskPixelContour.makeDistinct();
		maskPixelContour.makeDense();

		CV::Segmentation::MaskCreator::denseContour2inclusiveMask(currentMask_.data<uint8_t>(), currentMask_.width(), currentMask_.height(), currentMask_.paddingElements(), maskPixelContour, 0x00);
	}

	if (homographyMaskNewBlobPosition_)
	{
		const unsigned char value = 0x00;
		CV::Canvas::ellipse8BitPerChannel<1u>(currentMask_.data<uint8_t>(), currentMask_.width(), currentMask_.height(), homographyMaskNewBlobPosition_, 51u, 51u, &value, currentMask_.paddingElements());
		homographyMaskNewBlobPosition_ = CV::PixelPosition();

		CV::PixelPositions borderPixels4;
		CV::Segmentation::MaskAnalyzer::findBorderPixels4(currentMask_.constdata<uint8_t>(), currentMask_.width(), currentMask_.height(), currentMask_.paddingElements(), borderPixels4);

		CV::PixelPositions contourPixelPositions;
		CV::Segmentation::MaskAnalyzer::pixels2contour(borderPixels4, currentMask_.width(), currentMask_.height(), contourPixelPositions);

		homographyMaskPreviousPoints_ = CV::PixelPosition::pixelPositions2vectors(contourPixelPositions);

		maskPixelContour = CV::Segmentation::PixelContour(contourPixelPositions);

		ocean_assert(maskPixelContour.isDistinct());
		ocean_assert(maskPixelContour.isDense());
	}

	if (!maskPixelContour.isEmpty())
	{
		highlightMask(currentFrame_, currentMask_, maskPixelContour.boundingBox(), maskPixelContour.pixels());
	}

	setFrame(currentFrame_);

	return true;
}

bool VideoInpaintingMainWindow::maskBasedInpainting()
{
	ocean_assert(inpaintingMode_ == IM_HOMOGRAPHY_MASK_BASED);
	ocean_assert(maskState_ == MS_MASK_DEFINED_SUCCESSFULLY);

	ocean_assert(currentFrame_ && currentFrame_.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	// first we track the mask from the previous frame to the current frame (via the mask's strong boundary locations defining a dominant homography)

	SquareMatrix3 currentHomographyPrevious(true);
	if (homographyTracker_.trackPoints(currentFrame_, yPrevousFrame_, randomGenerator_, homographyMaskPreviousPoints_, currentHomographyPrevious, &worker_))
	{
		homographyMaskPreviousPoints_ = Tracking::HomographyTracker::transformPoints(homographyMaskPreviousPoints_, currentHomographyPrevious);
	}

	// we check whether the new mask has left the frame so that we stop here, thus we simply do not re-track an object which has left the camera frame
	for (const Vector2& point : homographyMaskPreviousPoints_)
	{
		if ((unsigned int)(Numeric::round32(point.x())) >= currentFrame_.width() || (unsigned int)(Numeric::round32(point.y())) >= currentFrame_.height())
		{
			return false;
		}
	}

	if (homographyMaskPreviousPoints_.size() < 3)
	{
		return false;
	}

	CV::Segmentation::PixelContour contour(CV::PixelPosition::vectors2pixelPositions(homographyMaskPreviousPoints_, currentFrame_.width(), currentFrame_.height()));
	contour.makeDistinct();
	contour.makeDense();

	if (contour.size() < 3)
	{
		return false;
	}

	// now as we have the dominant homography and the accurate/fine contour for the current frame we invoke the core inpainting approach with is identical for either the contour-based or mask-based mode

	return coreInpainting(currentHomographyPrevious, contour);
}

bool VideoInpaintingMainWindow::coreInpainting(const SquareMatrix3& currentHomographyPrevious, const CV::Segmentation::PixelContour& pixelContour)
{
	ocean_assert(pixelContour.size() >= 3);

	SquareMatrix3 previousHomographyCurrent;
	if (!currentHomographyPrevious.invert(previousHomographyCurrent))
	{
		return false;
	}

	// we compute the most dominant homography between the current and the first frame
	firstHomographyRecent_ = Geometry::Homography::normalizedHomography(firstHomographyRecent_ * previousHomographyCurrent);

	// create the mask of the undesired object according to the tracked object contour
	currentMask_.set(FrameType(currentFrame_, FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/);
	currentMask_.setValue(0xFFu);

	CV::Segmentation::MaskCreator::denseContour2inclusiveMask(currentMask_.data<uint8_t>(), currentMask_.width(), currentMask_.height(), currentMask_.paddingElements(), pixelContour, 0x00);

	// the very first frame needs a different treatment as the successive frames
	if (!firstInpaintingFrame_)
	{
		// the current frame is the very first inpainting frame, so we do not care about any previous frame (wrt. video coherence) but we simply try to create a plausible and high quality inpainting result

		CV::Synthesis::SynthesisPyramidI1 initialSynthesisPyramid;
		initialSynthesisPyramid.arrange(currentFrame_, currentMask_, &worker_, false, false);

		CV::Synthesis::Constraints constraints;

#ifdef USE_SYNTHESIS_CONSTRAINTS

		CV::Detector::LineDetector::InfiniteLines infiniteLines;
		CV::Detector::LineDetector::detectLines(applicationCurrentFrame, CV::Detector::LineDetector::FT_SOBEL, CV::Detector::LineDetector::FR_HORIZONTAL_VERTICAL, infiniteLines, nullptr, true, 8, 61u, 2u, 5, true, &applicationWorker);

		if (!infiniteLines.empty())
		{
			std::sort(infiniteLines.begin(), infiniteLines.end());

			const Line2& line = infiniteLines.front();

			const Vector2 point0 = line.point() + Vector2(Scalar(applicationCurrentFrame.width()) * Scalar(0.5), Scalar(applicationCurrentFrame.height()) * Scalar(0.5));
			const Vector2 point1 = point0 + line.direction();


			constraints.addConstraint(new CV::Synthesis::LineConstraint(point0, point1, 200, 40));
		}

#endif // USE_SYNTHESIS_CONSTRAINTS

		if (!constraints.isEmpty())
		{
			initialSynthesisPyramid.applyInpainting(constraints, randomGenerator_, 5u, 26u, (unsigned int)(-1), 4u, 2u, &worker_);
		}
		else
		{
			initialSynthesisPyramid.applyInpainting(CV::Synthesis::SynthesisPyramidI1::IT_PATCH_SUB_REGION_2, randomGenerator_, 5u, 26u, (unsigned int)(-1), 2u, 1u, 1u, &worker_);
		}

		// as we will not use the 'applicationCurrentFrame' anymore during this inpainting iteration we store it as first inpainting frame
		firstInpaintingFrame_ = std::move(currentFrame_);

		// we apply the determined mapping and create the final inpainting result
		initialSynthesisPyramid.createInpaintingResult(firstInpaintingFrame_, &worker_);

		// we also store a small resolution of the first inpainting frame as we will use this resolution during the creation of the reference frame
		CV::FrameShrinker::downsampleByTwo11(firstInpaintingFrame_, firstInpaintingFrameQuarter_, &worker_);
		CV::FrameShrinker::downsampleByTwo11(firstInpaintingFrameQuarter_, &worker_);

		// we store the synthesis result for the first frame (the mapping of the finest synthesis layer) as initial rough guess for the successive frame
		previousMapping_ = std::move(initialSynthesisPyramid.finestLayer().mapping());

		setFrame(firstInpaintingFrame_);
	}
	else
	{
		// the current frame is not the very first inpainting frame, e.g., the second, third or any successive frame
		// we do not need (we even must not) to synthesis an entirely new image content, we need to synthesis image content matching with the previous frame wrt. video coherence

		// we store the bounding box of the undesired object to improve the computational performance in several of the following functions
		const CV::PixelBoundingBox maskBoundingBox(pixelContour.boundingBox());

		// determine the inner distance between the mask pixels and the border of the mask and stores this information as the pixel values within the mask, 0 means the border itself, 1 means one pixel until the border is reached, and so on...
		CV::Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(currentMask_.data<uint8_t>(), currentMask_.width(), currentMask_.height(), currentMask_.paddingElements(), 6u, false, maskBoundingBox, &worker_);

		// now, we create a reference frame which will be used for the image synthesis guiding the algorithm to create an visual result similar to the reference frame
		createReferenceFrame(currentFrame_, currentMask_, firstInpaintingFrame_, firstInpaintingFrameQuarter_, firstHomographyRecent_, pixelContour.pixels(), maskBoundingBox, referenceFrame_, &worker_);


		// we initialize the sub-pixel accurate synthesis pyramid
		Frame copyCurrentFrame(currentFrame_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
		CV::Synthesis::LayerF1 newSynthesisPixelLayer(copyCurrentFrame, currentMask_, maskBoundingBox); // **TODO** remove frame copy

		// we adopt the synthesis mapping from the previous frame
		CV::Synthesis::InitializerHomographyMappingAdaptionF1(newSynthesisPixelLayer, previousMapping_, randomGenerator_, previousHomographyCurrent).invoke(&worker_);

		// we optimize the synthesis for the current frame while respecting a reference frame
		CV::Synthesis::Optimizer4NeighborhoodReferenceFrameF1<5u, 25u, true>(newSynthesisPixelLayer, randomGenerator_, referenceFrame_).invoke(5u, 1u, (unsigned int)(-1), &worker_, true);


		synthesisResult_.copy(currentFrame_); // **TODO** perhaps we do not need to create a copy, neither here, nor above

		CV::Synthesis::CreatorInpaintingContentF1(newSynthesisPixelLayer, synthesisResult_).invoke(&worker_);

		// finally, we improve the resulting video quality by blending the synthesized content with the current live content (at the border of the undesired object, actually all pixels with distance <= 5 pixels)
		// beware: although the blending is in general a nice idea to improve the image quality (to prevent hard transitions) the blending may introduce other issues like adding a shadow from the current/original frame - in this case the extra boundary around the undesired object should be increased

		for (unsigned int y = maskBoundingBox.top(); y < maskBoundingBox.bottomEnd(); ++y)
		{
			const uint8_t* currentData = currentFrame_.constpixel<uint8_t>(maskBoundingBox.left(), y);
			const uint8_t* maskData = currentMask_.constpixel<uint8_t>(maskBoundingBox.left(), y);
			uint8_t* resultData = synthesisResult_.pixel<uint8_t>(maskBoundingBox.left(), y);

			for (unsigned int x = maskBoundingBox.left(); x < maskBoundingBox.rightEnd(); ++x)
			{
				if (*maskData > 0u && *maskData <= 5u)
				{
					//                <- mask-area | non-mask-area ->
					//  FF FF FF 05 04 03 02 01 00 |

					// factor: low (0) at the direct border of the mask and high (5 * 1024) inside the mask
					const unsigned int factor = *maskData * 1024u / 6u;
					ocean_assert(factor <= 1024u);

					// factor_: is high at the direct border of the mask and low inside the mask
					const unsigned int factor_ = 1024u - factor;

					for (unsigned int n = 0u; n < 3u; ++n)
					{
						resultData[n] = uint8_t((currentData[n] * factor_ + resultData[n] * factor) >> 10u);
					}
				}

				currentData += 3;
				resultData += 3;
				maskData++;
			}
		}

		previousMapping_ = std::move(newSynthesisPixelLayer.mapping());

		setFrame(synthesisResult_);
	}

	return true;
}

void VideoInpaintingMainWindow::reset()
{
	inpaintingMode_ = IM_UNKNOWN;
	contourState_ = CS_IDLE;
	maskState_ = MS_IDLE;

	userDefinedRoughContour_.clear();

	firstHomographyRecent_.toIdentity();

	contourTracker_.clear();
	homographyTracker_.clear();

	homographyMaskPreviousPoints_.clear();
	homographyMaskNewBlobPosition_ = CV::PixelPosition();

	firstInpaintingFrame_.release();
	firstInpaintingFrameQuarter_.release();

	yPrevousFrame_.release();
	currentMask_.release();

	synthesisResult_.release();
}

void VideoInpaintingMainWindow::createReferenceFrame(const Frame& currentFrame, const Frame& currentMask, const Frame& inpaintingReferenceFrame, const Frame& inpaintingReferenceFrameQuarter, const SquareMatrix3& referenceHomographyCurrent, const CV::PixelPositions& contourPoints, const CV::PixelBoundingBox& trackingMaskBoundingBox, Frame& referenceFrame, Worker* worker)
{
	ocean_assert(contourPoints.size() >= 3);
	ocean_assert(trackingMaskBoundingBox);

	ocean_assert(currentFrame && currentMask);
	ocean_assert(FrameType::formatIsGeneric(currentFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 3u) && "The current implemented is restricted to thee-channel frames");

	// we create a reference frame based on the current frame so that is looks like the very first inpainting frame
	referenceFrame = Frame(currentFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	// first we fill the undesired area in the current frame with visual information from the very first inpainting frame
	CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::homographyFilterMask(inpaintingReferenceFrame, currentMask, referenceFrame, referenceHomographyCurrent, trackingMaskBoundingBox, worker);

	// now we need to adjust the appearance of the undesired area so that it matches with the appearance of the current frame, wrt. e.g., ambient lighting changes (camera exposure, shadows, etc...)
	// however, due to performance reasons we apply a very basic adjustment only

	Frame currentFrameQuarter;
	CV::FrameShrinker::downsampleByTwo11(currentFrame, currentFrameQuarter, worker); // **TODO** masked shrinking as we do not want visual information from non-mask pixels
	CV::FrameShrinker::downsampleByTwo11(currentFrameQuarter, worker);

	ocean_assert(inpaintingReferenceFrameQuarter.frameType() == currentFrameQuarter.frameType());

	// now we determine pairs of color values between corresponding image locations in the current and very first frame

	ColorAdjustmentObject<3u>::ColorAdjustmentObjects colorAdjustmentObjects;
	colorAdjustmentObjects.reserve(contourPoints.size() / 4);

	CV::PixelPosition lastPixelPositionQuarter;
	for (CV::PixelPositions::const_iterator i = contourPoints.begin(); i != contourPoints.end(); ++i)
	{
		const CV::PixelPosition currentPixelPositionQuarter((i->x() + 2u) / 4u, (i->y() + 2u) / 4u);

		// we want to ensure that each (downsampled) contour point is not used more than once (as long as subsequent)
		if (currentPixelPositionQuarter != lastPixelPositionQuarter)
		{
			lastPixelPositionQuarter = currentPixelPositionQuarter;

			// we determine the location of the current position in the reference frame
			const Vector2 referencePositionQuarter((referenceHomographyCurrent * i->vector()) * Scalar(0.25));

			// we do not take reference points lying outside the reference frame

			if (referencePositionQuarter.x() >= Scalar(0) && referencePositionQuarter.y() >= Scalar(0) && referencePositionQuarter.x() <= Scalar(currentFrameQuarter.width() - 1u) && referencePositionQuarter.y() <= Scalar(currentFrameQuarter.height() - 1u))
			{
				const CV::PixelPosition referencePixelPositionQuarter(CV::PixelPosition::vector2pixelPosition(referencePositionQuarter));
				ocean_assert(referencePixelPositionQuarter.x() < inpaintingReferenceFrameQuarter.width() && referencePixelPositionQuarter.y() < inpaintingReferenceFrameQuarter.height());

				const uint8_t* const referencePixel = inpaintingReferenceFrameQuarter.constpixel<uint8_t>(referencePixelPositionQuarter.x(), referencePixelPositionQuarter.y());
				const uint8_t* const currentPixel = currentFrameQuarter.constpixel<uint8_t>(currentPixelPositionQuarter.x(), currentPixelPositionQuarter.y());

				colorAdjustmentObjects.emplace_back(currentPixelPositionQuarter.vector(), referencePixel, currentPixel);
			}
		}
	}

	// now we merge neighboring adjustment objects as long as we have too much objects
	while (colorAdjustmentObjects.size() / 2 > 30)
	{
		ColorAdjustmentObject<3u>::ColorAdjustmentObjects newColorAdjustmentObjects;
		newColorAdjustmentObjects.reserve(newColorAdjustmentObjects.size() / 2);

		for (size_t n = 1; n < colorAdjustmentObjects.size(); n += 2)
		{
			newColorAdjustmentObjects.emplace_back(colorAdjustmentObjects[n - 1u], colorAdjustmentObjects[n]);
		}

		colorAdjustmentObjects = std::move(newColorAdjustmentObjects);
	}

	// now we create a sparse grid/mesh with 10x10 bins and we determine an adjustment values for each node of the grid (based on the above determined adjustment location-value objects)

	ocean_assert(trackingMaskBoundingBox.width() >= 10u);
	ocean_assert(trackingMaskBoundingBox.height() >= 10u);

	LookupCorner2<Vector3> colorAdjustmentLookup(trackingMaskBoundingBox.width(), trackingMaskBoundingBox.height(), 10, 10);

	const Scalar diagonalQuarter = Numeric::sqrt(Numeric::sqr(Scalar(trackingMaskBoundingBox.width())) + Numeric::sqr(Scalar(trackingMaskBoundingBox.height()))) * Scalar(0.25);

	for (size_t yBin = 0; yBin <= colorAdjustmentLookup.binsY(); ++yBin)
	{
		const Scalar positionY = colorAdjustmentLookup.binTopLeftCornerPositionY(yBin) + Scalar(trackingMaskBoundingBox.top());

		for (size_t xBin = 0; xBin <= colorAdjustmentLookup.binsX(); ++xBin)
		{
			const Scalar positionX = colorAdjustmentLookup.binTopLeftCornerPositionX(xBin) + Scalar(trackingMaskBoundingBox.left());

			ocean_assert(positionX >= Scalar(trackingMaskBoundingBox.left()) && positionX <= Scalar(trackingMaskBoundingBox.rightEnd()));
			ocean_assert(positionY >= Scalar(trackingMaskBoundingBox.top()) && positionY <= Scalar(trackingMaskBoundingBox.bottomEnd()));

			const Vector2 positionQuarter(positionX * Scalar(0.25), positionY * Scalar(0.25));

			Vector3 deltas(0, 0, 0);
			Scalar weights = 0;

			for (ColorAdjustmentObject<3u>::ColorAdjustmentObjects::const_iterator i = colorAdjustmentObjects.begin(); i != colorAdjustmentObjects.end(); ++i)
			{
				//const Scalar w = Numeric::exp(-Numeric::sqrt(Numeric::sqrt((unsigned int)(p.sqrDistance(pos))))); // <- more ideal but slow
				//const Scalar w = Numeric::exp(-Numeric::sqrt(p.sqrDistance(pos)));

				const Scalar sqrDistance = positionQuarter.sqrDistance(*i);
				const Scalar weight = sqrDistance > Numeric::eps() ? diagonalQuarter / Numeric::sqrt(sqrDistance) : diagonalQuarter;

				weights += weight;
				deltas[0] += i->delta(0) * weight;
				deltas[1] += i->delta(1) * weight;
				deltas[2] += i->delta(2) * weight;
			}

			colorAdjustmentLookup.setBinTopLeftCornerValue(xBin, yBin, Numeric::isNotEqualEps(weights) ? (deltas / weights) : Vector3(0, 0, 0));
		}
	}

	// now we adjust each pixel value of the reference frame (inside the mask only) by using interpolated adjustment values from the lookup table, **TODO** multicore?
	// further we blend the visual content of the current frame and the reference frame at the border of the mask (actually all pixels with distance <= 5 pixels)

	for (unsigned int y = trackingMaskBoundingBox.top(); y < trackingMaskBoundingBox.bottomEnd(); ++y)
	{
		uint8_t* referenceData = referenceFrame.pixel<uint8_t>(trackingMaskBoundingBox.left(), y);

		const uint8_t* currentData = currentFrame.constpixel<uint8_t>(trackingMaskBoundingBox.left(), y);
		const uint8_t* maskData = currentMask.constpixel<uint8_t>(trackingMaskBoundingBox.left(), y);

		for (unsigned int x = trackingMaskBoundingBox.left(); x < trackingMaskBoundingBox.rightEnd(); ++x)
		{
			if (*maskData != 0xFF)
			{
				const Vector3 interpolatedAdjustment = colorAdjustmentLookup.bilinearValue(Scalar(x - trackingMaskBoundingBox.left()), Scalar(y - trackingMaskBoundingBox.top()));

				if (*maskData > 0u && *maskData <= 5u)
				{
					// we have a special handling for all pixels near to the maks's border (we actually want to blend the content with the current frame)
					// beware: although the blending is in general a nice idea to improve the image quality (to prevent hard transitions) the blending may introduce other issues like adding a shadow from the current/original frame - in this case the extra boundary around the undesired object should be increased

					//                <- mask-area | non-mask-area ->
					//  FF FF FF 05 04 03 02 01 00 |

					// factor: low (0) at the direct border of the mask and high (5 * 1024) inside the mask
					const unsigned int factor = *maskData * 1024u / 6u;
					ocean_assert(factor <= 1024u);

					// factor_: is high at the direct border of the mask and low inside the mask
					const unsigned int factor_ = 1024u - factor;

					for (unsigned int n = 0u; n < 3u; ++n)
					{
						const int delta = Numeric::round32(interpolatedAdjustment[n]);
						referenceData[n] = uint8_t((currentData[n] * factor_ + (unsigned char)minmax(0, int(referenceData[n]) + delta, 255) * factor) >> 10u);
					}
				}
				else
				{
					// we are in the middle of the mask (far away from the border) so we simply adjust the content

					for (unsigned int n = 0u; n < 3u; ++n)
					{
						const int delta = Numeric::round32(interpolatedAdjustment[n]);
						referenceData[n] = (unsigned char)minmax(0, int(referenceData[n]) + delta, 255);
					}
				}
			}

			maskData++;
			referenceData += 3;
			currentData += 3;
		}
	}
}

void VideoInpaintingMainWindow::highlightMask(Frame& frame, const Frame& mask, const CV::PixelBoundingBox& boundingBox, const CV::PixelPositions& contourPixels)
{
	ocean_assert(frame.isValid() && mask.isValid());
	ocean_assert(FrameType(frame, mask.pixelFormat()) == mask.frameType());

	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 3u));
	ocean_assert(FrameType::formatIsGeneric(mask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	// **TODO** this highlighting is designed for RGB, we need something more flexible e.g., also for YUV, and multicore?

	unsigned int left = 0u;
	unsigned int top = 0u;

	unsigned int rightEnd = frame.width();
	unsigned int bottomEnd = frame.height();

	if (boundingBox)
	{
		left = min(boundingBox.left(), frame.width() - 1u);
		top = min(boundingBox.top(), frame.height() - 1u);

		rightEnd = min(boundingBox.rightEnd(), frame.width());
		bottomEnd = min(boundingBox.bottomEnd(), frame.height());
	}

	for (unsigned int y = top; y < bottomEnd; ++y)
	{
		uint8_t* framePixel = frame.pixel<uint8_t>(left, y);
		const uint8_t* maskPixel = mask.constpixel<uint8_t>(left, y);

		for (unsigned int x = left; x < rightEnd; ++x)
		{
			if (*maskPixel++ == 0x00u)
			{
				framePixel[0u] = 0xFF;
				framePixel[1u] /= 2u;
				framePixel[2u] /= 2u;
			}

			framePixel += 3;
		}
	}

	if (!contourPixels.empty())
	{
		for (const CV::PixelPosition& contourPixel : contourPixels)
		{
			if (contourPixel.x() < frame.width() && contourPixel.y() < frame.height())
			{
				uint8_t* const framePixel = frame.pixel<uint8_t>(contourPixel.x(), contourPixel.y());

				framePixel[0u] = 0x00u;
				framePixel[1u] = 0x00u;
				framePixel[2u] = 0xFFu;
			}
		}
	}
}
