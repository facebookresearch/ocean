/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/PanoramaFrame.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Subset.h"

#include "ocean/cv/FrameMean.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

PanoramaFrame::PanoramaFrame(const Frame& frame, const Frame& mask, const uint8_t maskValue, const UpdateMode updateMode, Worker* worker) :
	PanoramaFrame(Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT), Frame(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT), maskValue, updateMode, worker)
{
	// nothing to do here
}

PanoramaFrame::PanoramaFrame(Frame&& frame, Frame&& mask, const uint8_t maskValue, const UpdateMode updateMode, Worker* worker) :
	frame_(std::move(frame)),
	mask_(std::move(mask)),
	dimensionWidth_(frame_.width()),
	dimensionHeight_(frame_.height()),
	invertedDimensionWidth_(frame_.width() != 0u ? Scalar(1) / Scalar(frame_.width()) : 0),
	invertedDimensionHeight_(frame_.height() != 0u ? Scalar(1) / Scalar(frame_.height()) : 0),
	frameTopLeft_(0u, 0u),
	maskValue_(maskValue),
	updateMode_(updateMode)
{
	if (updateMode_ == UM_AVERAGE_GLOBAL)
	{
		if (!nominatorFrame_.set(FrameType(frame_, FrameType::genericPixelFormat<uint32_t>(frame_.channels())), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		nominatorFrame_.setValue(0x00u);

		if (!denominatorFrame_.set(FrameType(mask_, FrameType::FORMAT_Y32), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		denominatorFrame_.setValue(0x00u);

		const uint8_t nonMaskValue = 0xFFu - maskValue_;

		FrameMean::addToFrameIndividually(frame_, mask_, nominatorFrame_, denominatorFrame_, nonMaskValue, worker);
	}
}

bool PanoramaFrame::setFrame(const PixelPosition& topLeft, const Frame& frame, const Frame& mask, Worker* worker)
{
	ocean_assert(topLeft.isValid() && frame.isValid());

	ocean_assert(topLeft.x() + frame.width() <= dimensionWidth_);
	ocean_assert(topLeft.y() + frame.height() <= dimensionHeight_);

	if (topLeft.x() + frame.width() > dimensionWidth_ || topLeft.y() + frame.height() > dimensionHeight_)
	{
		return false;
	}

	return reset(topLeft, frame, mask, worker);
}

bool PanoramaFrame::addFrame(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Frame& frame, const Frame& mask, const unsigned int approximationBinSize, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid() && frame.isValid() && frame.numberPlanes() == 1u);

	if (!pinholeCamera.isValid() || !frame.isValid() || frame.numberPlanes() != 1u)
	{
		return false;
	}

	if (!frame_.isValid())
	{
		if (!reset(pinholeCamera, frame, orientation, approximationBinSize, worker))
		{
			return false;
		}
	}
	else
	{
		if (!update(pinholeCamera, frame, mask, orientation, approximationBinSize, worker))
		{
			return false;
		}
	}

	return true;
}

bool PanoramaFrame::cameraFrame2panoramaSubFrame(const PinholeCamera& pinholeCamera, const Frame& frame, const Frame& mask, const SquareMatrix3& orientation, Frame& panoramaSubFrame, Frame& panoramaSubMask, PixelPositionI& subFrameTopLeft, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	ocean_assert(pinholeCamera.isValid() && frame.isValid() && !orientation.isSingular());
	ocean_assert(pinholeCamera.width() == frame.width() && pinholeCamera.height() == frame.height());
	ocean_assert(fineAdjustment == nullptr || ((unsigned int)(fineAdjustment->sizeX()) == pinholeCamera.width() && (unsigned int)(fineAdjustment->sizeY()) == pinholeCamera.height()));

	ocean_assert(!mask.isValid() || FrameType(frame, FrameType::FORMAT_Y8) == mask.frameType());

	if (mask && FrameType(frame, FrameType::FORMAT_Y8) != mask.frameType())
	{
		return false;
	}

	const Box2 boundingBox(panoramaSubFrameBoundingBox(pinholeCamera, orientation));

	const int left = int(Numeric::floor(boundingBox.left()));
	const int top = int(Numeric::floor(boundingBox.top()));

	const int right = int(Numeric::ceil(boundingBox.right()));
	const int bottom = int(Numeric::ceil(boundingBox.bottom()));

	ocean_assert(right >= left && bottom >= top);

	const unsigned int width = (unsigned int)(right - left) + 1u;
	const unsigned int height = (unsigned int)(bottom - top) + 1u;

	ocean_assert(width <= dimensionWidth_ && height <= dimensionHeight_);

	if (!panoramaSubFrame.set(FrameType(width, height, frame.pixelFormat(), frame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/)
			|| !panoramaSubMask.set(FrameType(panoramaSubFrame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	panoramaSubMask.setValue(0xFFu - maskValue_);

	subFrameTopLeft = PixelPositionI(left, top);

	if (mask)
	{
		return cameraFrame2panoramaFrame(pinholeCamera, frame, mask, orientation, dimensionWidth_, dimensionHeight_, subFrameTopLeft, panoramaSubFrame, panoramaSubMask, maskValue_, approximationBinSize, worker, fineAdjustment);
	}
	else
	{
		return cameraFrame2panoramaFrame(pinholeCamera, frame, orientation, dimensionWidth_, dimensionHeight_, subFrameTopLeft, panoramaSubFrame, panoramaSubMask, maskValue_, approximationBinSize, worker, fineAdjustment);
	}
}

void PanoramaFrame::clear()
{
	frame_.release();
	mask_.release();

	nominatorFrame_.release();
	denominatorFrame_.release();

	frameTopLeft_ = PixelPosition((unsigned int)(-1), (unsigned int)(-1));
}

Box2 PanoramaFrame::panoramaSubFrameBoundingBox(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation) const
{
	ocean_assert(dimensionWidth_ >= 1u && dimensionHeight_ >= 1u);

	const Vector2 corners[4] =
	{
		Vector2(0, 0),
		Vector2(0, Scalar(pinholeCamera.height() - 1u)),
		Vector2(Scalar(pinholeCamera.width() - 1u), Scalar(pinholeCamera.height() - 1u)),
		Vector2(Scalar(pinholeCamera.width() - 1u), 0)
	};

	// first, we determine the angle of the principal point so that we can ensure that the resulting bounding box
	// is defined w.r.t. to this point

	const Vector3 principalRay(orientation * pinholeCamera.vector(pinholeCamera.undistort<true>(Vector2(pinholeCamera.principalPointX(), pinholeCamera.principalPointY()))));
	const Vector2 principalAngle(ray2angleStrict(principalRay));

	Box2 result;

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		const Vector2& corner0 = corners[n];
		const Vector2& corner1 = corners[(n + 1u) % 4u];

		for (unsigned int i = 0u; i < 10u; ++i)
		{
			result += cameraPixel2panoramaPixel(pinholeCamera, orientation, (corner0 * Scalar(10 - i) + corner1 * Scalar(i)) * Scalar(0.1), principalAngle);
		}
	}

	const Scalar cameraFovDiagonal = pinholeCamera.fovDiagonal();

	if (principalAngle.y() >= Numeric::pi_2() - cameraFovDiagonal * Scalar(0.5))
	{
		result = Box2(Vector2(0, 0), Vector2(Scalar(dimensionWidth_ - 1u), result.bottom()));
	}

	if (principalAngle.y() < -Numeric::pi_2() + cameraFovDiagonal * Scalar(0.5))
	{
		result = Box2(Vector2(0, result.top()), Vector2(Scalar(dimensionWidth_ - 1u), Scalar(dimensionHeight_ - 1u)));
	}

	ocean_assert(result.width() < Scalar(dimensionWidth_));
	ocean_assert(result.height() < Scalar(dimensionHeight_));

	return result;
}

void PanoramaFrame::cameraFrame2cameraFrameLookupTable(const PinholeCamera& inputCamera, const SquareMatrix3& world_R_input, const PinholeCamera& outputCamera, const SquareMatrix3& world_R_output, LookupTable& input_LT_output)
{
	ocean_assert(inputCamera.isValid() && outputCamera.isValid());
	ocean_assert(!world_R_input.isSingular() && !world_R_output.isSingular());
	ocean_assert(!input_LT_output.isEmpty());

	const SquareMatrix3 world_R_flippedInput = PinholeCamera::flippedTransformationRightSide(world_R_input);
	const SquareMatrix3 world_R_flippedOutput = PinholeCamera::flippedTransformationRightSide(world_R_output);

	const SquareMatrix3 flippedInput_R_flippedOutput = world_R_flippedInput.inverted() * world_R_flippedOutput;

	const SquareMatrix3 input_T_output(inputCamera.intrinsic() * flippedInput_R_flippedOutput * outputCamera.invertedIntrinsic());

	for (unsigned int y = 0u; y <= input_LT_output.binsY(); ++y)
	{
		const Scalar outputPositionY = input_LT_output.binTopLeftCornerPositionY(y);

		for (unsigned int x = 0u; x <= input_LT_output.binsX(); ++x)
		{
			const Scalar outputPositionX = input_LT_output.binTopLeftCornerPositionX(x);

			const Vector2 inputPosition = inputCamera.distort<true>(input_T_output * outputCamera.undistort<true>(Vector2(outputPositionX, outputPositionY)));

			input_LT_output.setBinTopLeftCornerValue(x, y, inputPosition);
		}
	}
}

void PanoramaFrame::panoramaFrame2cameraFrameLookupTable(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPosition& panoramaFrameTopLeft, LookupTable& lookupTable, const LookupTable* fineAdjustment)
{
	ocean_assert(!lookupTable.isEmpty());

	const Vector2 frameTopLeft(Scalar(panoramaFrameTopLeft.x()), Scalar(panoramaFrameTopLeft.y()));

	for (unsigned int y = 0u; y <= lookupTable.binsY(); ++y)
	{
		const Scalar cameraPositionY = lookupTable.binTopLeftCornerPositionY(y);

		for (unsigned int x = 0u; x <= lookupTable.binsX(); ++x)
		{
			const Scalar cameraPositionX = lookupTable.binTopLeftCornerPositionX(x);

			Vector2 cameraPosition = Vector2(cameraPositionX, cameraPositionY);

			if (fineAdjustment)
			{
				cameraPosition += fineAdjustment->bilinearValue(cameraPosition.x(), cameraPosition.y());
			}

			const Vector3 ray(orientation * pinholeCamera.vector(pinholeCamera.undistort<true>(cameraPosition)));

			const Vector2 angle(ray2angleStrict(ray));
			Vector2 panoramaPosition(angle2pixel(angle, panoramaDimensionWidth, panoramaDimensionHeight));

			// now we need to check whether we have to shift the horizontal location as we need a lookup table with continuous values
			// however, this is still not ideal

			if (x == 0u && y >= 1u)
			{
				const Vector2& perviousTop = lookupTable.binTopLeftCornerValue(x, y - 1u);

				if (panoramaPosition.x() > perviousTop.x())
				{
					if (panoramaPosition.x() - perviousTop.x() > Scalar(panoramaDimensionWidth) * Scalar(0.5))
					{
						panoramaPosition.x() -= Scalar(panoramaDimensionWidth);
					}
				}
				else
				{
					if (perviousTop.x() - panoramaPosition.x() > Scalar(panoramaDimensionWidth) * Scalar(0.5))
					{
						panoramaPosition.x() += Scalar(panoramaDimensionWidth);
					}
				}

				ocean_assert(Numeric::abs(panoramaPosition.x() - perviousTop.x()) < Scalar(panoramaDimensionWidth) * Scalar(0.5));
			}
			else if (x >= 1u)
			{
				const Vector2& perviousLeft = lookupTable.binTopLeftCornerValue(x - 1u, y);

				if (panoramaPosition.x() > perviousLeft.x())
				{
					if (panoramaPosition.x() - perviousLeft.x() > Scalar(panoramaDimensionWidth) * Scalar(0.5))
					{
						panoramaPosition.x() -= Scalar(panoramaDimensionWidth);
					}
				}
				else
				{
					if (perviousLeft.x() - panoramaPosition.x() > Scalar(panoramaDimensionWidth) * Scalar(0.5))
					{
						panoramaPosition.x() += Scalar(panoramaDimensionWidth);
					}
				}

				ocean_assert(Numeric::abs(panoramaPosition.x() - perviousLeft.x()) < Scalar(panoramaDimensionWidth) * Scalar(0.5));
			}

			lookupTable.setBinTopLeftCornerValue(x, y, panoramaPosition);
		}
	}
}

void PanoramaFrame::cameraFrame2panoramaFrameLookupTable(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, LookupTable& lookupTable, const LookupTable* fineAdjustment)
{
	ocean_assert(!lookupTable.isEmpty());

	ocean_assert(panoramaDimensionWidth != 0u && panoramaDimensionHeight != 0u);
	const Scalar invPanoramaDimensionWidth = Scalar(1) / Scalar(panoramaDimensionWidth);
	const Scalar invPanoramaDimensionHeight = Scalar(1) / Scalar(panoramaDimensionHeight);

	ocean_assert(!orientation.isSingular());
	const SquareMatrix3 invOrientation(orientation.inverted());

	for (unsigned int y = 0u; y <= lookupTable.binsY(); ++y)
	{
		const Scalar panoramaPositionY = lookupTable.binTopLeftCornerPositionY(y) + Scalar(panoramaFrameTopLeft.y());

		for (unsigned int x = 0u; x <= lookupTable.binsX(); ++x)
		{
			const Scalar panoramaPositionX = lookupTable.binTopLeftCornerPositionX(x) + Scalar(panoramaFrameTopLeft.x());

			const Vector2 panoramaPosition = Vector2(panoramaPositionX, panoramaPositionY);

			const Vector2 angle(pixel2angle(panoramaPosition, invPanoramaDimensionWidth, invPanoramaDimensionHeight));
			const Vector3 ray(angle2ray(angle));

			const Vector3 invRay(invOrientation * ray);
			const Vector3 rayOnPlane(invRay * Scalar(-1) / invRay.z());

			const Vector2 cameraPosition(pinholeCamera.normalizedImagePoint2imagePoint<true>(Vector2(rayOnPlane.x(), -rayOnPlane.y()), true));

			if (fineAdjustment)
			{
				lookupTable.setBinTopLeftCornerValue(x, y, cameraPosition + fineAdjustment->clampedBilinearValue(cameraPosition.x(), cameraPosition.y()));
			}
			else
			{
				lookupTable.setBinTopLeftCornerValue(x, y, cameraPosition);
			}
		}
	}
}

bool PanoramaFrame::update(const PinholeCamera& pinholeCamera, const Frame& frame, const Frame& mask, const SquareMatrix3& orientation, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	ocean_assert(pinholeCamera.isValid() && frame.isValid() && !orientation.isSingular());
	ocean_assert(fineAdjustment == nullptr || ((unsigned int)fineAdjustment->sizeX() == pinholeCamera.width() && (unsigned int)fineAdjustment->sizeY() == pinholeCamera.height()));

	Frame panoramaSubFrame, panoramaSubMask;
	CV::PixelPositionI panoramaSubTopLeft(NumericT<int>::minValue(), NumericT<int>::minValue());

	if (!cameraFrame2panoramaSubFrame(pinholeCamera, frame, mask, orientation, panoramaSubFrame, panoramaSubMask, panoramaSubTopLeft, approximationBinSize, worker, fineAdjustment))
	{
		return false;
	}

	ocean_assert(panoramaSubTopLeft != CV::PixelPositionI(NumericT<int>::minValue(), NumericT<int>::minValue()));
	ocean_assert(panoramaSubFrame.width() <= dimensionWidth_);
	ocean_assert(panoramaSubFrame.height() <= dimensionHeight_);

	// now we need to determine whether the panorama frame lies partially outside the actual entire panorama frame

	if (panoramaSubTopLeft.x() < 0 || panoramaSubTopLeft.x() + int(panoramaSubFrame.width()) > int(dimensionWidth_) || panoramaSubTopLeft.y() < 0 || panoramaSubTopLeft.y() + int(panoramaSubFrame.height()) > int(dimensionHeight_))
	{
		ocean_assert(panoramaSubTopLeft.y() >= 0 && panoramaSubTopLeft.y() + int(panoramaSubFrame.height()) <= int(dimensionHeight_));
		ocean_assert(panoramaSubTopLeft.x() < 0 || panoramaSubTopLeft.x() + int(panoramaSubFrame.width()) > int(dimensionWidth_));

		const int left = panoramaSubTopLeft.x();
		const int top = panoramaSubTopLeft.y();

		const unsigned int height = panoramaSubFrame.height();

		Frame extendedPanoramaSubFrame(FrameType(dimensionWidth_, height, frame.pixelFormat(), frame.pixelOrigin()));
		extendedPanoramaSubFrame.setValue(0x00);

		Frame extendedPanoramaSubMask(FrameType(extendedPanoramaSubFrame, FrameType::FORMAT_Y8));
		extendedPanoramaSubMask.setValue(uint8_t(0xFFu - maskValue_));

		// we need to distribute the frame between the left and the right side of the panorama frame

		// left half
		extendedPanoramaSubFrame.copy(int((unsigned int)(int(dimensionWidth_) + left) % dimensionWidth_), 0, panoramaSubFrame.subFrame(0u, 0u, (unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, height));
		extendedPanoramaSubMask.copy(int((unsigned int)(int(dimensionWidth_) + left) % dimensionWidth_), 0, panoramaSubMask.subFrame(0u, 0u, (unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, height));

		// right half
		extendedPanoramaSubFrame.copy(0, 0, panoramaSubFrame.subFrame((unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, 0u, (unsigned int)(int(panoramaSubFrame.width()) + left) % dimensionWidth_, height));
		extendedPanoramaSubMask.copy(0, 0, panoramaSubMask.subFrame((unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, 0u, (unsigned int)(int(panoramaSubFrame.width()) + left) % dimensionWidth_, height));

		panoramaSubFrame = std::move(extendedPanoramaSubFrame);
		panoramaSubMask = std::move(extendedPanoramaSubMask);

		panoramaSubTopLeft = PixelPositionI(0, top);
	}

	ocean_assert(panoramaSubTopLeft.x() >= 0 && panoramaSubTopLeft.y() >= 0);

	unsigned int newLeft = min((unsigned int)panoramaSubTopLeft.x(), frameTopLeft_.x());
	unsigned int newTop = min((unsigned int)panoramaSubTopLeft.y(), frameTopLeft_.y());

	unsigned int newRightEnd = max((unsigned int)panoramaSubTopLeft.x() + panoramaSubFrame.width(), frameTopLeft_.x() + frame_.width());
	unsigned int newBottomEnd = max((unsigned int)panoramaSubTopLeft.y() + panoramaSubFrame.height(), frameTopLeft_.y() + frame_.height());

	if (newLeft != frameTopLeft_.x() || newTop != frameTopLeft_.y() || newRightEnd != frameTopLeft_.x() + frame_.width() || newBottomEnd != frameTopLeft_.y() + frame_.height())
	{
		newLeft = min((unsigned int)max(0, int(panoramaSubTopLeft.x()) - 50), frameTopLeft_.x());
		newTop = min((unsigned int)max(0, int(panoramaSubTopLeft.y()) - 50), frameTopLeft_.y());

		newRightEnd = max(min(panoramaSubTopLeft.x() + panoramaSubFrame.width() + 50u, dimensionWidth_), frameTopLeft_.x() + frame_.width());
		newBottomEnd = max(min(panoramaSubTopLeft.y() + panoramaSubFrame.height() + 50u, dimensionHeight_), frameTopLeft_.y() + frame_.height());

		resize(PixelPosition(newLeft, newTop), newRightEnd - newLeft, newBottomEnd - newTop);
	}

	if (!merge(panoramaSubFrame, panoramaSubMask, PixelPosition((unsigned int)panoramaSubTopLeft.x(), (unsigned int)panoramaSubTopLeft.y()), worker))
	{
		return false;
	}

	return true;
}

bool PanoramaFrame::reset(const PinholeCamera& pinholeCamera, const Frame& frame, const SquareMatrix3& orientation, const unsigned int approximationBinSize, Worker* worker)
{
	ocean_assert(dimensionWidth_ != 0u && dimensionHeight_ != 0u);

	const Box2 boundingBox(panoramaSubFrameBoundingBox(pinholeCamera, orientation));

	// we give some extra space around the bounding box and ensure that we have pixel accuracy (and not sub-pixel accuracy anymore)

	int left = int(Numeric::floor(boundingBox.left()));
	int top = int(Numeric::floor(boundingBox.top()));

	int right = int(Numeric::ceil(boundingBox.right()));
	int bottom = int(Numeric::ceil(boundingBox.bottom()));

	ocean_assert(right >= left && bottom >= top);

	unsigned int width = (unsigned int)(right - left) + 1u;
	unsigned int height = (unsigned int)(bottom - top) + 1u;

	ocean_assert(width <= dimensionWidth_ && height <= dimensionHeight_);

	// now we add an extra border to ensure that we do not need to change the size of the panorama frame each time a new frame is added

	constexpr unsigned int extraBorder = 50u;

	if (width + extraBorder * 2u <= dimensionWidth_)
	{
		// **TODO** extend left and right to maximal possible instead

		left -= int(extraBorder);
		right += int(extraBorder);

		width = (unsigned int)(right - left) + 1u;
	}

	if (height + extraBorder * 2u <= dimensionHeight_)
	{
		top = max(0, top - int(extraBorder));
		bottom = min(bottom + int(extraBorder), int(dimensionHeight_) - 1);

		height = (unsigned int)(bottom - top) + 1u;
	}

	ocean_assert(width <= dimensionWidth_ && height <= dimensionHeight_);

	if (!frame_.set(FrameType(width, height, frame.pixelFormat(), frame.pixelOrigin()), true, true))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	frame_.setValue(0x00u);

	if (!mask_.set(FrameType(frame_, FrameType::FORMAT_Y8), true, true))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	mask_.setValue(0xFFu - maskValue_);

	if (!cameraFrame2panoramaFrame(pinholeCamera, frame, orientation, dimensionWidth_, dimensionHeight_, CV::PixelPositionI(left, top), frame_, mask_, maskValue_, approximationBinSize, worker))
	{
		frame_.release();
		mask_.release();
		frameTopLeft_ = PixelPosition((unsigned int)(-1), (unsigned int)(-1));
	}

	// now we need to determine whether the panorama frame lies partially outside the actual entire panorama frame

	if (left >= 0 && right < int(dimensionWidth_) && top >= 0 && bottom < int(dimensionHeight_))
	{
		frameTopLeft_ = PixelPosition((unsigned int)left, (unsigned int)top);
	}
	else
	{
		ocean_assert(top >= 0 && bottom < int(dimensionHeight_));
		ocean_assert(left < 0 || right >= int(dimensionWidth_));

		Frame extendedPanoramaFrame(FrameType(dimensionWidth_, height, frame.pixelFormat(), frame.pixelOrigin()));
		extendedPanoramaFrame.setValue(0x00);

		Frame extendedPanoramaMask(FrameType(extendedPanoramaFrame, FrameType::FORMAT_Y8));
		extendedPanoramaMask.setValue(0xFFu - maskValue_);

		// we need to distribute the frame between the left and the right side of the panorama frame

		// left half
		extendedPanoramaFrame.copy(int((unsigned int)(int(dimensionWidth_) + left) % dimensionWidth_), 0, frame_.subFrame(0u, 0u, (unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, height));
		extendedPanoramaMask.copy(int((unsigned int)(int(dimensionWidth_) + left) % dimensionWidth_), 0, mask_.subFrame(0u, 0u, (unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, height));

		// right half
		extendedPanoramaFrame.copy(0, 0, frame_.subFrame((unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, 0u, (unsigned int)(int(width) + left) % dimensionWidth_, height));
		extendedPanoramaMask.copy(0, 0, mask_.subFrame((unsigned int)(int(dimensionWidth_) - left) % dimensionWidth_, 0u, (unsigned int)(int(width) + left) % dimensionWidth_, height));

		frameTopLeft_ = PixelPosition(0u, (unsigned int)(top));

		frame_ = std::move(extendedPanoramaFrame);
		mask_ = std::move(extendedPanoramaMask);
	}

	// for the global average update mode we need a nominator and denominator frames
	if (updateMode_ == UM_AVERAGE_GLOBAL)
	{
		nominatorFrame_.set(FrameType(frame_, FrameType::genericPixelFormat<uint32_t>(frame.channels())), true, true);
		nominatorFrame_.setValue(0x00u);

		denominatorFrame_.set(FrameType(mask_, FrameType::FORMAT_Y32), true, true);
		denominatorFrame_.setValue(0x00u);

		const uint8_t nonMaskValue = 0xFFu - maskValue_;

		FrameMean::addToFrameIndividually(frame_, mask_, nominatorFrame_, denominatorFrame_, nonMaskValue, worker);
	}

	return true;
}

bool PanoramaFrame::reset(const PixelPosition& topLeft, const Frame& frame, const Frame& mask, Worker* worker)
{
	ocean_assert(dimensionWidth_ != 0u && dimensionHeight_ != 0u);
	ocean_assert(frame.isValid() && (!mask.isValid() || FrameType(frame, FrameType::FORMAT_Y8) == mask.frameType()));

	frame_.copy(0, 0, frame);

	if (mask)
	{
		mask_.copy(0, 0, mask);
	}
	else
	{
		mask_.set(FrameType(frame, FrameType::FORMAT_Y8), true, true);
		mask_.setValue(0xFF - maskValue_);
	}

	frameTopLeft_ = topLeft;

	// for the global average update mode we need a nominator and denominator frames
	if (updateMode_ == UM_AVERAGE_GLOBAL)
	{
		if (!nominatorFrame_.set(FrameType(frame_, FrameType::genericPixelFormat<uint32_t>(frame.channels())), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		nominatorFrame_.setValue(0x00u);

		if (!denominatorFrame_.set(FrameType(mask_, FrameType::FORMAT_Y32), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		denominatorFrame_.setValue(0x00u);

		const uint8_t nonMaskValue = 0xFFu - maskValue_;

		FrameMean::addToFrameIndividually(frame_, mask_, nominatorFrame_, denominatorFrame_, nonMaskValue, worker);
	}

	return true;
}

void PanoramaFrame::resize(const PixelPosition& topLeft, const unsigned int width, const unsigned int height)
{
	ocean_assert(width != frame_.width() || height != frame_.height());

	ocean_assert(topLeft.x() < dimensionWidth_ && topLeft.y() < dimensionHeight_);
	ocean_assert(frame_.isValid() && mask_.isValid());

	Frame newFrame(FrameType(frame_, width, height));
	Frame newMask(FrameType(mask_, width, height));
	newFrame.setValue(0x00);
	newMask.setValue(0xFFu - maskValue_);

	const int targetLeft = int(frameTopLeft_.x()) - int(topLeft.x());
	const int targetTop = int(frameTopLeft_.y()) - int(topLeft.y());

	const bool copyResult0 = newFrame.copy(targetLeft, targetTop, frame_);
	const bool copyResult1 = newMask.copy(targetLeft, targetTop, mask_);

	ocean_assert_and_suppress_unused(copyResult0, copyResult0);
	ocean_assert_and_suppress_unused(copyResult1, copyResult1);

	Frame newNominatorFrame, newDenominatorFrame;

	if (nominatorFrame_)
	{
		ocean_assert(denominatorFrame_);

		if (!newNominatorFrame.set(FrameType(nominatorFrame_, width, height), true, true)
				|| !newDenominatorFrame.set(FrameType(denominatorFrame_, width, height), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		newNominatorFrame.setValue(0x00u);
		newDenominatorFrame.setValue(0x00u);

		const bool copyResult2 = newNominatorFrame.copy(targetLeft, targetTop, nominatorFrame_);
		const bool copyResult3 = newDenominatorFrame.copy(targetLeft, targetTop, denominatorFrame_);

		ocean_assert_and_suppress_unused(copyResult2, copyResult2);
		ocean_assert_and_suppress_unused(copyResult3, copyResult3);
	}

	frameTopLeft_ = topLeft;

	frame_ = std::move(newFrame);
	mask_ = std::move(newMask);

	if (newNominatorFrame)
	{
		nominatorFrame_ = std::move(newNominatorFrame);
		denominatorFrame_ = std::move(newDenominatorFrame);
	}
}

bool PanoramaFrame::merge(const Frame& panoramaSubFrame, const Frame& panoramaSubMask, const PixelPosition& subTopLeft, Worker* worker)
{
	ocean_assert(panoramaSubFrame.frameType() == FrameType(panoramaSubMask, panoramaSubFrame.pixelFormat()));
	ocean_assert(panoramaSubFrame.numberPlanes() == 1u);

	ocean_assert(subTopLeft.x() >= frameTopLeft_.x() && subTopLeft.y() >= frameTopLeft_.y());
	ocean_assert(subTopLeft.x() + panoramaSubMask.width() <= frameTopLeft_.x() + frame_.width());
	ocean_assert(subTopLeft.y() + panoramaSubMask.height() <= frameTopLeft_.y() + frame_.height());

	ocean_assert(frame_.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (updateMode_)
	{
		case UM_SET_ALL:
		{
			ocean_assert(frame_.frameType() == FrameType(mask_, frame_.pixelFormat()));

			switch (frame_.channels())
			{
				case 1u:
					mergeSetAll8BitPerChannel<1u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 2u:
					mergeSetAll8BitPerChannel<2u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 3u:
					mergeSetAll8BitPerChannel<3u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 4u:
					mergeSetAll8BitPerChannel<4u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;
			}

			break;
		}

		case UM_SET_NEW:
		{
			ocean_assert(frame_.frameType() == FrameType(mask_, frame_.pixelFormat()));

			switch (frame_.channels())
			{
				case 1u:
					mergeSetNew8BitPerChannel<1u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 2u:
					mergeSetNew8BitPerChannel<2u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 3u:
					mergeSetNew8BitPerChannel<3u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 4u:
					mergeSetNew8BitPerChannel<4u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;
			}

			break;
		}

		case UM_AVERAGE_LOCAL:
		{
			ocean_assert(frame_.frameType() == FrameType(mask_, frame_.pixelFormat()));

			switch (frame_.channels())
			{
				case 1u:
					mergeAverageLocal8BitPerChannel<1u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 2u:
					mergeAverageLocal8BitPerChannel<2u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 3u:
					mergeAverageLocal8BitPerChannel<3u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;

				case 4u:
					mergeAverageLocal8BitPerChannel<4u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
					return true;
			}

			break;
		}

		case UM_AVERAGE_GLOBAL:
		{
			ocean_assert(frame_.frameType() == FrameType(mask_, frame_.pixelFormat()));

			ocean_assert(FrameType(nominatorFrame_, frame_.pixelFormat()) == frame_.frameType());
			ocean_assert(FrameType(denominatorFrame_, mask_.pixelFormat()) == mask_.frameType());

			ocean_assert(nominatorFrame_.isContinuous() && denominatorFrame_.isContinuous());
			if (nominatorFrame_.isContinuous() && denominatorFrame_.isContinuous())
			{
				switch (frame_.channels())
				{
					case 1u:
						mergeAverageGlobal8BitPerChannel<1u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, nominatorFrame_.data<uint32_t>(), denominatorFrame_.data<uint32_t>(), frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
						return true;

					case 2u:
						mergeAverageGlobal8BitPerChannel<2u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, nominatorFrame_.data<uint32_t>(), denominatorFrame_.data<uint32_t>(), frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
						return true;

					case 3u:
						mergeAverageGlobal8BitPerChannel<3u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, nominatorFrame_.data<uint32_t>(), denominatorFrame_.data<uint32_t>(), frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
						return true;

					case 4u:
						mergeAverageGlobal8BitPerChannel<4u>(panoramaSubFrame.constdata<uint8_t>(), panoramaSubMask.constdata<uint8_t>(), panoramaSubFrame.width(), panoramaSubFrame.height(), panoramaSubFrame.paddingElements(), panoramaSubMask.paddingElements(), subTopLeft, nominatorFrame_.data<uint32_t>(), denominatorFrame_.data<uint32_t>(), frame_.data<uint8_t>(), mask_.data<uint8_t>(), frame_.width(), frame_.paddingElements(), mask_.paddingElements(), frameTopLeft_, maskValue_, worker);
						return true;
				}
			}

			break;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

template <unsigned int tChannels>
void PanoramaFrame::mergeSetAll8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows)
{
	ocean_assert(subTopLeftX >= panoramaTopLeftX && subTopLeftY >= panoramaTopLeftY);
	ocean_assert(panoramaWidth >= subFrameWidth);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const unsigned int panoramaSubFrameStrideElements = subFrameWidth * tChannels + panoramaSubFramePaddingElements;
	const unsigned int panoramaSubMaskStrideElements = subFrameWidth + panoramaSubMaskPaddingElements;

	const unsigned int panoramaFrameStrideElements = panoramaWidth * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = panoramaWidth + panoramaMaskPaddingElements;

	panoramaFrame += (subTopLeftY - panoramaTopLeftY) * panoramaFrameStrideElements + (subTopLeftX - panoramaTopLeftX) * tChannels;
	panoramaMask += (subTopLeftY - panoramaTopLeftY) * panoramaMaskStrideElements + (subTopLeftX - panoramaTopLeftX);

	for (unsigned int y = firstSubRow; y < firstSubRow + numberSubRows; ++y)
	{
		const PixelType* panoramaSubFrameRow = (const PixelType*)(panoramaSubFrame + y * panoramaSubFrameStrideElements);
		const uint8_t* panoramaSubMaskRow = panoramaSubMask + y * panoramaSubMaskStrideElements;

		PixelType* panoramaFrameRow = (PixelType*)(panoramaFrame + y * panoramaFrameStrideElements);
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < subFrameWidth; ++x)
		{
			if (*panoramaSubMaskRow == maskValue)
			{
				*panoramaFrameRow = *panoramaSubFrameRow;
				*panoramaMaskRow = maskValue;
			}

			++panoramaFrameRow;
			++panoramaMaskRow;

			++panoramaSubFrameRow;
			++panoramaSubMaskRow;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::mergeSetNew8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows)
{
	ocean_assert(subTopLeftX >= panoramaTopLeftX && subTopLeftY >= panoramaTopLeftY);
	ocean_assert(panoramaWidth >= subFrameWidth);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const unsigned int panoramaSubFrameStrideElements = subFrameWidth * tChannels + panoramaSubFramePaddingElements;
	const unsigned int panoramaSubMaskStrideElements = subFrameWidth + panoramaSubMaskPaddingElements;

	const unsigned int panoramaFrameStrideElements = panoramaWidth * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = panoramaWidth + panoramaMaskPaddingElements;

	panoramaFrame += (subTopLeftY - panoramaTopLeftY) * panoramaFrameStrideElements + (subTopLeftX - panoramaTopLeftX) * tChannels;
	panoramaMask += (subTopLeftY - panoramaTopLeftY) * panoramaMaskStrideElements + (subTopLeftX - panoramaTopLeftX);

	for (unsigned int y = firstSubRow; y < firstSubRow + numberSubRows; ++y)
	{
		const PixelType* panoramaSubFrameRow = (const PixelType*)(panoramaSubFrame + y * panoramaSubFrameStrideElements);
		const uint8_t* panoramaSubMaskRow = panoramaSubMask + y * panoramaSubMaskStrideElements;

		PixelType* panoramaFrameRow = (PixelType*)(panoramaFrame + y * panoramaFrameStrideElements);
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < subFrameWidth; ++x)
		{
			if (*panoramaSubMaskRow == maskValue && *panoramaMaskRow != maskValue)
			{
				*panoramaFrameRow = *panoramaSubFrameRow;
				*panoramaMaskRow = maskValue;
			}

			++panoramaFrameRow;
			++panoramaMaskRow;

			++panoramaSubFrameRow;
			++panoramaSubMaskRow;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::mergeAverageLocal8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows)
{
	ocean_assert(subTopLeftX >= panoramaTopLeftX && subTopLeftY >= panoramaTopLeftY);
	ocean_assert(panoramaWidth >= subFrameWidth);

	const unsigned int panoramaSubFrameStrideElements = subFrameWidth * tChannels + panoramaSubFramePaddingElements;
	const unsigned int panoramaSubMaskStrideElements = subFrameWidth + panoramaSubMaskPaddingElements;

	const unsigned int panoramaFrameStrideElements = panoramaWidth * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = panoramaWidth + panoramaMaskPaddingElements;

	panoramaFrame += (subTopLeftY - panoramaTopLeftY) * panoramaFrameStrideElements + (subTopLeftX - panoramaTopLeftX) * tChannels;
	panoramaMask += (subTopLeftY - panoramaTopLeftY) * panoramaMaskStrideElements + (subTopLeftX - panoramaTopLeftX);

	for (unsigned int y = firstSubRow; y < firstSubRow + numberSubRows; ++y)
	{
		const uint8_t* panoramaSubFrameRow = panoramaSubFrame + y * panoramaSubFrameStrideElements;
		const uint8_t* panoramaSubMaskRow = panoramaSubMask + y * panoramaSubMaskStrideElements;

		uint8_t* panoramaFrameRow = panoramaFrame + y * panoramaFrameStrideElements;
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < subFrameWidth; ++x)
		{
			if (*panoramaSubMaskRow == maskValue)
			{
				if (*panoramaMaskRow == maskValue)
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						panoramaFrameRow[n] = (panoramaFrameRow[n] + panoramaSubFrameRow[n] + 1u) / 2u;
					}
				}
				else
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						panoramaFrameRow[n] = panoramaSubFrameRow[n];
					}

					*panoramaMaskRow = maskValue;
				}
			}

			panoramaFrameRow += tChannels;
			++panoramaMaskRow;

			panoramaSubFrameRow += tChannels;
			++panoramaSubMaskRow;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::mergeAverageGlobal8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint32_t* panoramaNominatorFrame, uint32_t* panoramaDenominatorFrame, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows)
{
	ocean_assert(subTopLeftX >= panoramaTopLeftX && subTopLeftY >= panoramaTopLeftY);
	ocean_assert(panoramaWidth >= subFrameWidth);

	const unsigned int panoramaSubFrameStrideElements = subFrameWidth * tChannels + panoramaSubFramePaddingElements;
	const unsigned int panoramaSubMaskStrideElements = subFrameWidth + panoramaSubMaskPaddingElements;

	const unsigned int panoramaFrameStrideElements = panoramaWidth * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = panoramaWidth + panoramaMaskPaddingElements;

	const unsigned int panoramaNominatorFrameStrideElements = panoramaWidth * tChannels;
	const unsigned int panoramaDenominatorFrameStrideElements = panoramaWidth;

	panoramaFrame += (subTopLeftY - panoramaTopLeftY) * panoramaFrameStrideElements + (subTopLeftX - panoramaTopLeftX) * tChannels;
	panoramaMask += (subTopLeftY - panoramaTopLeftY) * panoramaMaskStrideElements + (subTopLeftX - panoramaTopLeftX);

	panoramaNominatorFrame += (subTopLeftY - panoramaTopLeftY) * panoramaNominatorFrameStrideElements + (subTopLeftX - panoramaTopLeftX) * tChannels;
	panoramaDenominatorFrame += (subTopLeftY - panoramaTopLeftY) * panoramaDenominatorFrameStrideElements + (subTopLeftX - panoramaTopLeftX);

	for (unsigned int y = firstSubRow; y < firstSubRow + numberSubRows; ++y)
	{
		const uint8_t* panoramaSubFrameRow = panoramaSubFrame + y * panoramaSubFrameStrideElements;
		const uint8_t* panoramaSubMaskRow = panoramaSubMask + y * panoramaSubMaskStrideElements;

		uint8_t* panoramaFrameRow = panoramaFrame + y * panoramaFrameStrideElements;
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		uint32_t* panoramaNominatorFrameRow = panoramaNominatorFrame + y * panoramaNominatorFrameStrideElements;
		uint32_t* panoramaDenominatorFrameRow = panoramaDenominatorFrame + y * panoramaDenominatorFrameStrideElements;

		for (unsigned int x = 0u; x < subFrameWidth; ++x)
		{
			if (*panoramaSubMaskRow == maskValue)
			{
				++(*panoramaDenominatorFrameRow);

				const unsigned int denominator_2 = *panoramaDenominatorFrameRow / 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					panoramaNominatorFrameRow[n] += panoramaSubFrameRow[n];

					const unsigned int normalizedValue = (panoramaNominatorFrameRow[n] + denominator_2) / *panoramaDenominatorFrameRow;
					ocean_assert(normalizedValue <= 255u);

					panoramaFrameRow[n] = (uint8_t)(normalizedValue);
				}

				*panoramaMaskRow = maskValue;
			}

			panoramaFrameRow += tChannels;
			++panoramaMaskRow;

			panoramaSubFrameRow += tChannels;
			++panoramaSubMaskRow;

			panoramaNominatorFrameRow += tChannels;
			++panoramaDenominatorFrameRow;
		}
	}
}

bool PanoramaFrame::haveIntersectionByInnerFov(const PinholeCamera& camera0, const SquareMatrix3& orientation0, const PinholeCamera& camera1, const SquareMatrix3& orientation1, Scalar* overlappingPercent)
{
	ocean_assert(camera0.isValid() && camera1.isValid());
	ocean_assert(orientation0.isOrthonormal() && orientation1.isOrthonormal());

	const Scalar innerFov0 = min(camera0.fovX(), camera0.fovY());
	const Scalar innerFov1 = min(camera1.fovX(), camera1.fovY());

	const Vector3 principalRay0(orientation0 * camera0.vector(camera0.undistort<true>(Vector2(camera0.principalPointX(), camera0.principalPointY()))));
	const Vector3 principalRay1(orientation1 * camera1.vector(camera1.undistort<true>(Vector2(camera1.principalPointX(), camera1.principalPointY()))));

	ocean_assert(Numeric::isEqual(principalRay0.length(), 1));
	ocean_assert(Numeric::isEqual(principalRay1.length(), 1));

	const Scalar angularDistance = Numeric::acos(principalRay0 * principalRay1);
	const Scalar maximalDistance = (innerFov0 + innerFov1) * Scalar(0.5);

	if (angularDistance <= maximalDistance)
	{
		if (overlappingPercent)
		{
			*overlappingPercent = Numeric::isEqualEps(maximalDistance) ? 1 : (Scalar(1) - angularDistance / maximalDistance);
		}

		return true;
	}

	if (overlappingPercent)
	{
		*overlappingPercent = 0;
	}

	return false;
}

bool PanoramaFrame::haveIntersectionByRays(const PinholeCamera& camera0, const SquareMatrix3& orientation0, const PinholeCamera& camera1, const SquareMatrix3& orientation1, const Scalar borderFactor, const unsigned int minimalIntersections)
{
	ocean_assert(camera0.isValid() && camera1.isValid());
	ocean_assert(orientation0.isOrthonormal() && orientation1.isOrthonormal());
	ocean_assert(borderFactor >= Scalar(0) && borderFactor < Scalar(0.5));
	ocean_assert(minimalIntersections >= 1u && minimalIntersections < 8u);

	const Scalar borderFactor_1 = Scalar(1) - borderFactor;

	const Vector3 rays0[8] =
	{
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * borderFactor, Scalar(camera0.height()) * borderFactor)), // top left
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * borderFactor, Scalar(camera0.height()) * Scalar(0.5))),
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * borderFactor, Scalar(camera0.height()) * borderFactor_1)), // bottom
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * Scalar(0.5), Scalar(camera0.height()) * borderFactor_1)),
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * borderFactor_1, Scalar(camera0.height()) * borderFactor_1)), // right bottom
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * borderFactor_1, Scalar(camera0.height()) * Scalar(0.5))),
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * borderFactor_1, Scalar(camera0.height()) * borderFactor)), // top right
		orientation0 * camera0.vector(Vector2(Scalar(camera0.width()) * Scalar(0.5), Scalar(camera0.height()) * borderFactor))
	};

	const Vector3 rays1[8] =
	{
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * borderFactor, Scalar(camera1.height()) * borderFactor)),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * borderFactor, Scalar(camera1.height()) * Scalar(0.5))),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * borderFactor, Scalar(camera1.height()) * borderFactor_1)),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * Scalar(0.5), Scalar(camera1.height()) * borderFactor_1)),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * borderFactor_1, Scalar(camera1.height()) * borderFactor_1)),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * borderFactor_1, Scalar(camera1.height()) * Scalar(0.5))),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * borderFactor_1, Scalar(camera1.height()) * borderFactor)),
		orientation1 * camera1.vector(Vector2(Scalar(camera1.width()) * Scalar(0.5), Scalar(camera1.height()) * borderFactor))
	};

	const Triangle3 triangle0A(rays0[0], rays0[2], rays0[4]);
	const Triangle3 triangle0B(rays0[0], rays0[4], rays0[6]);

	Vector3 intersectionPoint;
	Scalar intersectionDistance;

	unsigned int intersections = 0u;

	for (unsigned int n = 0u; n < 8u; ++n)
	{
		if (triangle0A.intersection(Line3(Vector3(0, 0, 0), rays1[n]), intersectionPoint, intersectionDistance) && intersectionDistance > 0)
		{
			intersections++;
		}
		else if (triangle0B.intersection(Line3(Vector3(0, 0, 0), rays1[n]), intersectionPoint, intersectionDistance) && intersectionDistance > 0)
		{
			intersections++;
		}

		if (intersections >= minimalIntersections)
		{
			return true;
		}
	}

	intersections = 0u;

	const Triangle3 triangle1A(rays1[0], rays1[2], rays1[4]);
	const Triangle3 triangle1B(rays1[0], rays1[4], rays1[6]);

	for (unsigned int n = 0u; n < 8u; ++n)
	{
		if (triangle1A.intersection(Line3(Vector3(0, 0, 0), rays0[n]), intersectionPoint, intersectionDistance) && intersectionDistance > 0)
		{
			intersections++;
		}
		else if (triangle1B.intersection(Line3(Vector3(0, 0, 0), rays0[n]), intersectionPoint, intersectionDistance) && intersectionDistance > 0)
		{
			intersections++;
		}

		if (intersections >= minimalIntersections)
		{
			return true;
		}
	}

	return false;
}

Scalar PanoramaFrame::approximateIntersectionArea(const PinholeCamera& camera0, const SquareMatrix3& orientation0, const PinholeCamera& camera1, const SquareMatrix3& orientation1, Scalar* intersectionRatio)
{
	ocean_assert(camera0.isValid() && camera1.isValid());
	ocean_assert(orientation0.isOrthonormal() && orientation1.isOrthonormal());

	if (intersectionRatio)
	{
		*intersectionRatio = Scalar(0);
	}

	if (!CV::Advanced::PanoramaFrame::haveIntersectionByRays(camera0, orientation0, camera1, orientation1, Scalar(0), 1u))
	{
		return 0;
	}

	// Rotate the outline of the source frame into the coordinate frame of the target frame
	const SquareMatrix3 transformation(camera0.intrinsic() * orientation0.inverted() * orientation1 * camera1.invertedIntrinsic());

	// Project the outline of the source frame into the target frame and then determine the intersection between them.
	const unsigned int numberOfSegments = 10u;
	const Vectors2 targetFrameContour = generateFrameContour(camera0.width(), camera0.height(), std::max(camera0.width(), camera0.height()) / numberOfSegments);
	const Vectors2 sourceFrameContour = generateFrameContour(camera1.width(), camera1.height(), std::max(camera1.width(), camera1.height()) / numberOfSegments);
	ocean_assert(Geometry::Utilities::isPolygonConvex(targetFrameContour.data(), targetFrameContour.size(), false));
	ocean_assert(Geometry::Utilities::isPolygonConvex(sourceFrameContour.data(), sourceFrameContour.size(), false));

	Vectors2 transformedSourceFrameContour;
	transformedSourceFrameContour.reserve(sourceFrameContour.size());

	for (size_t i = 0; i < sourceFrameContour.size(); ++i)
	{
		transformedSourceFrameContour.emplace_back(camera0.distort<true>(transformation * camera1.undistort<true>(sourceFrameContour[i])));
	}

	if (!Geometry::Utilities::isPolygonConvex(transformedSourceFrameContour.data(), transformedSourceFrameContour.size(), false))
	{
		return 0;
	}

	Vectors2 intersection;
	if (!Geometry::Utilities::intersectConvexPolygons(targetFrameContour, transformedSourceFrameContour, intersection))
	{
		return 0;
	}

	const Scalar intersectionArea = Geometry::Utilities::computePolygonArea(intersection);

	if (intersectionRatio)
	{
		const Scalar targetFrameArea = Scalar(camera0.width() * camera0.height());
		ocean_assert(Numeric::isNotEqualEps(targetFrameArea));

		*intersectionRatio = intersectionArea / targetFrameArea;
		ocean_assert(Numeric::isInsideRange(Scalar(0), *intersectionRatio, Scalar(1)));
	}

	return intersectionArea;
}

Vector2 PanoramaFrame::cameraPixel2cameraPixel(const PinholeCamera& inputCamera, const SquareMatrix3& inputOrientation, const Vector2& inputPosition, const PinholeCamera& outputCamera, const SquareMatrix3& outputOrientation)
{
	ocean_assert(inputCamera.isValid() && outputCamera.isValid());
	ocean_assert(!inputOrientation.isSingular() && !outputOrientation.isSingular());

	return outputCamera.projectToImage<true>(HomogenousMatrix4(outputOrientation), inputOrientation * inputCamera.vectorToPlane(inputCamera.undistort<true>(inputPosition), 1), true);
}

bool PanoramaFrame::cameraFrame2cameraFrame(const PinholeCamera& inputCamera, const SquareMatrix3& inputOrientation, const Frame& inputFrame, const Frame& inputMask, const PinholeCamera& outputCamera, const SquareMatrix3& outputOrientation, Frame& outputFrame, Frame& outputMask, const uint8_t maskValue, const unsigned int approximationBinsSize, Worker* worker)
{
	ocean_assert(inputCamera.isValid() && inputFrame.isValid() && inputCamera.width() == inputFrame.width() && inputCamera.height() == inputFrame.height());
	ocean_assert(!inputMask.isValid() || (inputFrame.width() == inputMask.width() && inputFrame.height() == inputMask.height() && inputFrame.pixelOrigin() == inputMask.pixelOrigin()));
	ocean_assert(!inputOrientation.isSingular() && !outputOrientation.isSingular());
	ocean_assert(outputCamera.isValid());

	if (!outputFrame.set(FrameType(outputCamera.width(), outputCamera.height(), inputFrame.pixelFormat(), inputFrame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/)
			|| !outputMask.set(FrameType(outputCamera.width(), outputCamera.height(), FrameType::FORMAT_Y8, inputFrame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	ocean_assert(inputFrame.numberPlanes() == 1u && inputFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (inputFrame.channels())
	{
		case 1u:
			cameraFrame2cameraFrame8BitPerChannel<1u>(inputCamera, inputOrientation, inputFrame.constdata<uint8_t>(), inputMask.isValid() ? inputMask.constdata<uint8_t>() : nullptr, inputFrame.paddingElements(), inputMask.paddingElements(), outputCamera, outputOrientation, outputFrame.data<uint8_t>(), outputMask.data<uint8_t>(), outputFrame.paddingElements(), outputMask.paddingElements(), maskValue, approximationBinsSize, worker);
			return true;

		case 2u:
			cameraFrame2cameraFrame8BitPerChannel<2u>(inputCamera, inputOrientation, inputFrame.constdata<uint8_t>(), inputMask.isValid() ? inputMask.constdata<uint8_t>() : nullptr, inputFrame.paddingElements(), inputMask.paddingElements(), outputCamera, outputOrientation, outputFrame.data<uint8_t>(), outputMask.data<uint8_t>(), outputFrame.paddingElements(), outputMask.paddingElements(), maskValue, approximationBinsSize, worker);
			return true;

		case 3u:
			cameraFrame2cameraFrame8BitPerChannel<3u>(inputCamera, inputOrientation, inputFrame.constdata<uint8_t>(), inputMask.isValid() ? inputMask.constdata<uint8_t>() : nullptr, inputFrame.paddingElements(), inputMask.paddingElements(), outputCamera, outputOrientation, outputFrame.data<uint8_t>(), outputMask.data<uint8_t>(), outputFrame.paddingElements(), outputMask.paddingElements(), maskValue, approximationBinsSize, worker);
			return true;

		case 4u:
			cameraFrame2cameraFrame8BitPerChannel<4u>(inputCamera, inputOrientation, inputFrame.constdata<uint8_t>(), inputMask.isValid() ? inputMask.constdata<uint8_t>() : nullptr, inputFrame.paddingElements(), inputMask.paddingElements(), outputCamera, outputOrientation, outputFrame.data<uint8_t>(), outputMask.data<uint8_t>(), outputFrame.paddingElements(), outputMask.paddingElements(), maskValue, approximationBinsSize, worker);
			return true;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool PanoramaFrame::panoramaFrame2cameraFrame(const PinholeCamera& pinholeCamera, const Frame& panoramaFrame, const Frame& panoramaMask, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPosition& panoramaFrameTopLeft, const SquareMatrix3& orientation, Frame& cameraFrame, Frame& cameraMask, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	ocean_assert(pinholeCamera.isValid() && panoramaFrame.isValid() && panoramaMask.isValid() && !orientation.isSingular());

	ocean_assert(FrameType::formatIsGeneric(panoramaMask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(panoramaMask.pixelOrigin() == panoramaFrame.pixelOrigin());

	ocean_assert(cameraFrame.width() == 0u || cameraFrame.width() == pinholeCamera.width());
	ocean_assert(cameraFrame.height() == 0u || cameraFrame.height() == pinholeCamera.height());

	if (!cameraFrame.set(FrameType(pinholeCamera.width(), pinholeCamera.height(), panoramaFrame.pixelFormat(), panoramaFrame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/)
			|| !cameraMask.set(FrameType(pinholeCamera.width(), pinholeCamera.height(), FrameType::FORMAT_Y8, panoramaFrame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	ocean_assert(panoramaFrame.numberPlanes() == 1u && panoramaFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (panoramaFrame.channels())
	{
		case 1u:
			panoramaFrame2cameraFrame8BitPerChannel<1u>(pinholeCamera, panoramaFrame.constdata<uint8_t>(), panoramaMask.constdata<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, orientation, cameraFrame.data<uint8_t>(), cameraMask.data<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 2u:
			panoramaFrame2cameraFrame8BitPerChannel<2u>(pinholeCamera, panoramaFrame.constdata<uint8_t>(), panoramaMask.constdata<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, orientation, cameraFrame.data<uint8_t>(), cameraMask.data<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 3u:
			panoramaFrame2cameraFrame8BitPerChannel<3u>(pinholeCamera, panoramaFrame.constdata<uint8_t>(), panoramaMask.constdata<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, orientation, cameraFrame.data<uint8_t>(), cameraMask.data<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 4u:
			panoramaFrame2cameraFrame8BitPerChannel<4u>(pinholeCamera, panoramaFrame.constdata<uint8_t>(), panoramaMask.constdata<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, orientation, cameraFrame.data<uint8_t>(), cameraMask.data<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool PanoramaFrame::cameraFrame2panoramaFrame(const PinholeCamera& pinholeCamera, const Frame& cameraFrame, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, Frame& panoramaFrame, Frame& panoramaMask, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular() && cameraFrame.isValid() && panoramaFrame.isValid() && panoramaMask.isValid());

	ocean_assert(pinholeCamera.width() == cameraFrame.width());
	ocean_assert(pinholeCamera.height() == cameraFrame.height());

	if (pinholeCamera.width() != cameraFrame.width() || pinholeCamera.height() != cameraFrame.height())
	{
		return false;
	}

	ocean_assert(cameraFrame.pixelFormat() == panoramaFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == panoramaFrame.pixelOrigin());

	if (!panoramaFrame.set(FrameType(panoramaFrame, cameraFrame.pixelFormat(), cameraFrame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	if (!panoramaMask.set(FrameType(panoramaFrame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (cameraFrame.channels())
	{
		case 1u:
			cameraFrame2panoramaFrame8BitPerChannel<1u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 2u:
			cameraFrame2panoramaFrame8BitPerChannel<2u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 3u:
			cameraFrame2panoramaFrame8BitPerChannel<3u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 4u:
			cameraFrame2panoramaFrame8BitPerChannel<4u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool PanoramaFrame::cameraFrame2panoramaFrame(const PinholeCamera& pinholeCamera, const Frame& cameraFrame, const Frame& cameraMask, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, Frame& panoramaFrame, Frame& panoramaMask, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular() && cameraFrame.isValid() && panoramaFrame.isValid() && panoramaMask.isValid());

	ocean_assert(pinholeCamera.width() == cameraFrame.width());
	ocean_assert(pinholeCamera.height() == cameraFrame.height());

	if (pinholeCamera.width() != cameraFrame.width() || pinholeCamera.height() != cameraFrame.height())
	{
		return false;
	}

	ocean_assert(cameraFrame.pixelFormat() == panoramaFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == panoramaFrame.pixelOrigin());

	ocean_assert(FrameType::formatIsGeneric(cameraMask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(cameraMask.pixelOrigin() == panoramaFrame.pixelOrigin());

	if (!panoramaFrame.set(FrameType(panoramaFrame, cameraFrame.pixelFormat(), cameraFrame.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	if (!panoramaMask.set(FrameType(panoramaFrame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	switch (cameraFrame.channels())
	{
		case 1u:
			cameraFrame2panoramaFrameMask8BitPerChannel<1u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraMask.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 2u:
			cameraFrame2panoramaFrameMask8BitPerChannel<2u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraMask.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 3u:
			cameraFrame2panoramaFrameMask8BitPerChannel<3u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraMask.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;

		case 4u:
			cameraFrame2panoramaFrameMask8BitPerChannel<4u>(pinholeCamera, cameraFrame.constdata<uint8_t>(), cameraMask.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraMask.paddingElements(), orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, panoramaFrame.data<uint8_t>(), panoramaMask.data<uint8_t>(), panoramaFrame.width(), panoramaFrame.height(), panoramaFrame.paddingElements(), panoramaMask.paddingElements(), maskValue, approximationBinSize, worker, fineAdjustment);
			return true;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

Vectors2 PanoramaFrame::generateFrameContour(const unsigned int width, const unsigned int height, const unsigned int count)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(count > 0u);

	const Scalar xStep = static_cast<Scalar>(width) / static_cast<Scalar>(count);
	const Scalar yStep = static_cast<Scalar>(height) / static_cast<Scalar>(count);

	const unsigned int verticesOnHorizontalEdge = count + 1u;
	const unsigned int verticesOnVerticalEdge = count - 1u;
	const unsigned int totalNumberOfVertices = 2u * (verticesOnHorizontalEdge + verticesOnVerticalEdge);

	Vectors2 contour(totalNumberOfVertices);

	// Top and bottom edges
	unsigned int topIndex = 0u;
	unsigned int bottomIndex = verticesOnHorizontalEdge + verticesOnVerticalEdge;

	for (unsigned int i = 0u; i <= count; ++i)
	{
		ocean_assert(topIndex < contour.size() && bottomIndex < contour.size());

		const Scalar x = Scalar(i) * xStep;
		contour[topIndex++] = Vector2(x, Scalar(0));
		contour[bottomIndex++] = Vector2(Scalar(width) - x, Scalar(height));
	}

	// Left and right edges
	unsigned int leftIndex = (2u * verticesOnHorizontalEdge) + verticesOnVerticalEdge;
	unsigned int rightIndex = verticesOnHorizontalEdge;

	for (unsigned int i = 1u; i < count; ++i)
	{
		ocean_assert(rightIndex < contour.size() && leftIndex < contour.size());

		const Scalar y = Scalar(i) * yStep;
		contour[rightIndex++] = Vector2(Scalar(width), y);
		contour[leftIndex++] = Vector2(Scalar(0), Scalar(height) - y);
	}

	return contour;
}

}

}

}
