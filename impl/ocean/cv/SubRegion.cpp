/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/SubRegion.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/base/WorkerPool.h"

namespace Ocean
{

namespace CV
{

SubRegion::SubRegion(const Triangles2& triangles) :
	triangles_(triangles),
	boundingBox_(triangles)
{
	// nothing to do here
}

SubRegion::SubRegion(const Triangle2* triangles, const size_t size) :
	triangles_(size)
{
	for (size_t n = 0; n < size; ++n)
	{
		triangles_[n] = triangles[n];
	}

	boundingBox_ = Box2(triangles_);
}

SubRegion::SubRegion(const Frame& mask, const PixelBoundingBox& boundingBox, const uint8_t maskValue) :
	mask_(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT),
	maskValue_(maskValue)
{
	ocean_assert(FrameType::formatIsGeneric(mask_.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	if (boundingBox.isValid())
	{
		ocean_assert(boundingBox.left() >= 0u && boundingBox.top() >= 0u);
		ocean_assert(boundingBox.right() < mask_.width() && boundingBox.bottom() < mask_.height());

		boundingBox_ = Box2(Scalar(boundingBox.left()), Scalar(boundingBox.top()), Scalar(boundingBox.right()), Scalar(boundingBox.bottom()));
	}
	else
	{
		const uint8_t nonMaskValue = 0xFFu - maskValue;

		const PixelBoundingBox pixelBoundingBox = MaskAnalyzer::detectBoundingBox(mask_.constdata<uint8_t>(), mask_.width(), mask_.height(), nonMaskValue, mask_.paddingElements());

		boundingBox_ = Box2(Scalar(pixelBoundingBox.left()), Scalar(pixelBoundingBox.top()), Scalar(pixelBoundingBox.right()), Scalar(pixelBoundingBox.bottom()));
	}
}

SubRegion::SubRegion(Frame&& mask, const PixelBoundingBox& boundingBox, const uint8_t maskValue) :
	mask_(std::move(mask)),
	maskValue_(maskValue)
{
	ocean_assert(FrameType::formatIsGeneric(mask_.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	if (boundingBox.isValid())
	{
		ocean_assert(boundingBox.left() >= 0u && boundingBox.top() >= 0u);
		ocean_assert(boundingBox.right() < mask_.width() && boundingBox.bottom() < mask_.height());

		boundingBox_ = Box2(Scalar(boundingBox.left()), Scalar(boundingBox.top()), Scalar(boundingBox.right()), Scalar(boundingBox.bottom()));
	}
	else
	{
		const uint8_t nonMaskValue = 0xFFu - maskValue;

		const PixelBoundingBox pixelBoundingBox = MaskAnalyzer::detectBoundingBox(mask_.constdata<uint8_t>(), mask_.width(), mask_.height(), nonMaskValue, mask_.paddingElements());

		boundingBox_ = Box2(Scalar(pixelBoundingBox.left()), Scalar(pixelBoundingBox.top()), Scalar(pixelBoundingBox.right()), Scalar(pixelBoundingBox.bottom()));
	}
}

SubRegion::SubRegion(const Scalar left, const Scalar top, const Scalar width, const Scalar height) :
	maskValue_(0x00)
{
	ocean_assert(width > Numeric::eps() && height > Numeric::eps());

	if (width > Numeric::eps() && height > Numeric::eps())
	{
		const Scalar right = left + width;
		const Scalar bottom = top + height;

		const Vector2 regionCorners[4] =
		{
			Vector2(left, top),
			Vector2(left, bottom),
			Vector2(right, bottom),
			Vector2(right, top)
		};

		triangles_.resize(2);
		triangles_[0] = Triangle2(regionCorners[0], regionCorners[1], regionCorners[2]);
		triangles_[1] = Triangle2(regionCorners[0], regionCorners[2], regionCorners[3]);

		boundingBox_ = Box2(triangles_);
	}
}

SubRegion::SubRegion(const Box2& boundingBox) :
	maskValue_(0x00)
{
	if (boundingBox.isValid())
	{
		const Vector2 regionCorners[4] =
		{
			Vector2(boundingBox.left(), boundingBox.top()),
			Vector2(boundingBox.left(), boundingBox.bottom()),
			Vector2(boundingBox.right(), boundingBox.bottom()),
			Vector2(boundingBox.right(), boundingBox.top())
		};

		triangles_.resize(2);
		triangles_[0] = Triangle2(regionCorners[0], regionCorners[1], regionCorners[2]);
		triangles_[1] = Triangle2(regionCorners[0], regionCorners[2], regionCorners[3]);

		boundingBox_ = boundingBox;
	}
}

SubRegion::SubRegion(const PixelBoundingBox& boundingBox) :
	maskValue_(0x00)
{
	if (boundingBox.isValid())
	{
		const Vector2 regionCorners[4] =
		{
			Vector2(Scalar(boundingBox.left()), Scalar(boundingBox.top())),
			Vector2(Scalar(boundingBox.left()), Scalar(boundingBox.bottom())),
			Vector2(Scalar(boundingBox.right()), Scalar(boundingBox.bottom())),
			Vector2(Scalar(boundingBox.right()), Scalar(boundingBox.top()))
		};

		triangles_.resize(2);
		triangles_[0] = Triangle2(regionCorners[0], regionCorners[1], regionCorners[2]);
		triangles_[1] = Triangle2(regionCorners[0], regionCorners[2], regionCorners[3]);

		boundingBox_ = Box2(triangles_);
	}
}

bool SubRegion::isInside(const Vector2& point) const
{
	if (!triangles_.empty())
	{
		// first check whether the point is inside the bounding box
		ocean_assert(boundingBox_);
		if (!boundingBox_.isInside(point))
		{
			return false;
		}

		for (Triangles2::const_iterator i = triangles_.begin(); i != triangles_.end(); ++i)
		{
			if (i->isInside(point))
			{
				return true;
			}
		}

		return false;
	}
	else
	{
		if (boundingBox_ && !boundingBox_.isInside(point))
		{
			return false;
		}

		const unsigned int x = Numeric::round32(point.x());
		const unsigned int y = Numeric::round32(point.y());

		if (x >= mask_.width() || y >= mask_.height())
		{
			return false;
		}

		const unsigned int maskStrideElements = mask_.strideElements();

		return mask_.constdata<uint8_t>()[y * maskStrideElements + x] == maskValue_;
	}
}

Vectors2 SubRegion::filterPoints(const Vectors2& points) const
{
	Vectors2 result;
	result.reserve(points.size());

	for (Vectors2::const_iterator i = points.begin(); i != points.end(); ++i)
	{
		if (isInside(*i))
		{
			result.push_back(*i);
		}
	}

	return result;
}

Scalar SubRegion::size() const
{
	if (isEmpty())
	{
		return 0;
	}

	if (mask_.isValid())
	{
		ocean_assert(mask_.isValid());
		ocean_assert(mask_.pixels() == mask_.size());

		unsigned int maskPixels = 0u;

		unsigned int subRegionLeft = 0u;
		unsigned int subRegionTop = 0u;
		unsigned int subRegionWidth = mask_.width();
		unsigned int subRegionHeight = mask_.height();

		if (boundingBox_.isValid())
		{
			boundingBox_.box2integer(mask_.width(), mask_.height(), subRegionLeft, subRegionTop, subRegionWidth, subRegionHeight);
		}

		for (unsigned int y = subRegionTop; y < subRegionTop + subRegionHeight; ++y)
		{
			const uint8_t* const maskRow = mask_.constrow<uint8_t>(y);

			for (unsigned int x = subRegionLeft; x < subRegionLeft + subRegionWidth; ++x)
			{
				if (maskRow[x] == maskValue_)
				{
					++maskPixels;
				}
			}
		}

		return Scalar(maskPixels);
	}
	else if (!triangles_.empty())
	{
		Scalar result = 0;

		for (Triangles2::const_iterator i = triangles_.begin(); i != triangles_.end(); ++i)
		{
			result += i->area();
		}

		return result;
	}
	else if (boundingBox_.isValid())
	{
		return boundingBox_.xDimension() * boundingBox_.yDimension();
	}

	return Scalar(0);
}

SubRegion SubRegion::operator*(const Scalar factor) const
{
	ocean_assert(factor >= 0);

	if (!triangles_.empty())
	{
		Triangles2 scaledTriangles;
		scaledTriangles.reserve(triangles_.size());

		for (Triangles2::const_iterator i = triangles_.begin(); i != triangles_.end(); ++i)
		{
			scaledTriangles.push_back(Triangle2(i->point0() * factor, i->point1() * factor, i->point2() * factor));
		}

		return SubRegion(scaledTriangles);
	}
	else if (mask_)
	{
		const unsigned int scaledWidth = max(0, Numeric::round32(Scalar(mask_.width()) * factor));
		const unsigned int scaledHeight = max(0, Numeric::round32(Scalar(mask_.height()) * factor));

		Frame scaledMask(FrameType(mask_, scaledWidth, scaledHeight));
		ocean_assert(scaledMask.pixelFormat() == FrameType::FORMAT_Y8);

		CV::FrameInterpolatorNearestPixel::resize<uint8_t, 1u>(mask_.constdata<uint8_t>(), scaledMask.data<uint8_t>(), mask_.width(), mask_.height(), scaledMask.width(), scaledMask.height(), mask_.paddingElements(), scaledMask.paddingElements(), WorkerPool::get().conditionalScopedWorker(scaledMask.pixels() >= 400u * 400u)());

		const uint8_t nonMask = 0xFFu - maskValue_;

		const CV::PixelBoundingBox boundingBox(MaskAnalyzer::detectBoundingBox(scaledMask.constdata<uint8_t>(), scaledMask.width(), scaledMask.height(), nonMask, scaledMask.paddingElements()));
		return SubRegion(std::move(scaledMask), boundingBox, maskValue_);
	}

	return SubRegion();
}

SubRegion SubRegion::operator*(const SquareMatrix3& homography) const
{
	if (!triangles_.empty())
	{
		Triangles2 transformedTriangles;
		transformedTriangles.reserve(triangles_.size());

		for (Triangles2::const_iterator i = triangles_.begin(); i != triangles_.end(); ++i)
		{
			transformedTriangles.push_back(Triangle2(homography * i->point0(), homography * i->point1(), homography * i->point2()));
		}

		return SubRegion(transformedTriangles);
	}
	else if (mask_)
	{
		Frame transformedMask(mask_.frameType());
		ocean_assert(transformedMask.pixelFormat() == FrameType::FORMAT_Y8);

		const uint8_t nonMask = 0xFFu - maskValue_;
		CV::FrameInterpolatorNearestPixel::homography<uint8_t, 1u>(mask_.constdata<uint8_t>(), mask_.width(), mask_.height(), homography.inverted(), &nonMask, transformedMask.data<uint8_t>(), CV::PixelPositionI(0, 0), transformedMask.width(), transformedMask.height(), mask_.paddingElements(), transformedMask.paddingElements(), WorkerPool::get().conditionalScopedWorker(transformedMask.pixels() >= 400u * 400u)());

		const PixelBoundingBox pixelBoundingBox = MaskAnalyzer::detectBoundingBox(transformedMask.constdata<uint8_t>(), transformedMask.width(), transformedMask.height(), nonMask, transformedMask.paddingElements());

		return SubRegion(std::move(transformedMask), pixelBoundingBox);
	}

	return SubRegion();
}

}

}
