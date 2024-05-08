/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SUB_REGION_H
#define META_OCEAN_CV_SUB_REGION_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/math/Box2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Triangle2.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implement a sub-region either defined by 2D triangles or defined by a binary mask.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT SubRegion
{
	public:

		/**
		 * Creates an empty sub-region.
		 */
		inline SubRegion();

		/**
		 * Copy constructor.
		 * @param subRegion Sub-region object to be copied
		 */
		inline SubRegion(const SubRegion& subRegion);

		/**
		 * Move constructor.
		 * @param subRegion Sub-region object to be moved
		 */
		inline SubRegion(SubRegion&& subRegion) noexcept;

		/**
		 * Creates a sub-region object by a given set of 2D triangles.
		 * @param triangles 2D triangles defining the sub-region
		 */
		explicit SubRegion(const Triangles2& triangles);

		/**
		 * Creates a sub-region object by a given set of 2D triangles.
		 * @param triangles 2D triangles defining the sub-region
		 * @param size The number of given triangles
		 */
		SubRegion(const Triangle2* triangles, const size_t size);

		/**
		 * Creates a sub-region object by a given binary mask.
		 * @param mask Binary mask defining the sub-region
		 * @param boundingBox Optional bounding box of the sub-region
		 * @param maskValue The mask value defining the valid pixels
		 */
		explicit SubRegion(const Frame& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox(), const uint8_t maskValue = 0x00);

		/**
		 * Creates a sub-region object by a given binary mask.
		 * @param mask Binary mask defining the sub-region
		 * @param boundingBox Optional bounding box of the sub-region
		 * @param maskValue The mask value defining the valid pixels
		 */
		explicit SubRegion(Frame&& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox(), const uint8_t maskValue = 0x00);

		/**
		 * Creates a rectangular sub-region which will be handled (internally) as two triangles with one common edge.
		 * @param left Horizontal start position of the sub-region
		 * @param top Vertical start position of the sub-region
		 * @param width The width of the sub-region, with range (0, infinity)
		 * @param height The height of the sub-region, with range (0, infinity)
		 */
		SubRegion(const Scalar left, const Scalar top, const Scalar width, const Scalar height);

		/**
		 * Creates a rectangular sub-region which will be handled (internally) as two triangles with one common edge.
		 * If the given bounding box is invalid the new sub-region will be a default sub-region.<br>
		 * @param boundingBox Bounding box that will defined the sub-region
		 */
		explicit SubRegion(const Box2& boundingBox);

		/**
		 * Creates a rectangular sub-region which will be handled (internally) as two triangles with one common edge.
		 * If the given bounding box is invalid the new sub-region will be a default sub-region.<br>
		 * @param boundingBox Bounding box that will defined the sub-region
		 */
		explicit SubRegion(const PixelBoundingBox& boundingBox);

		/**
		 * Returns the bounding box of this sub-region.
		 * @return Sub-region bounding box
		 */
		inline const Box2& boundingBox() const;

		/**
		 * Returns whether a given point lies inside this sub-region.
		 * @param point Point to be checked
		 * @return True, if so
		 */
		bool isInside(const Vector2& point) const;

		/**
		 * Filters a set of given image points so that all resulting points ly inside this sub-region.
		 * @param points The set of image points which will be filtered
		 * @return Image points lying inside this sub-region
		 */
		Vectors2 filterPoints(const Vectors2& points) const;

		/**
		 * Returns whether this sub-region is empty.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns the binary sub-region mask, if any
		 * @return Sub-region mask, otherwise an invalid frame is returned
		 */
		inline const Frame& mask() const;

		/**
		 * Returns the 2D triangles of this sub-region, if any
		 * @return 2D sub-region triangles
		 */
		inline const Triangles2& triangles() const;

		/**
		 * Returns the number of pixels that are covered by this region.
		 * @return The number of pixels, with range [0, infinity)
		 */
		Scalar size() const;

		/**
		 * Assign operator.
		 * @param subRegion Sub-region object to be assigned
		 * @return Reference to this object
		 */
		inline SubRegion& operator=(const SubRegion& subRegion);

		/**
		 * Move operator.
		 * @param subRegion Sub-region object to be moved
		 * @return Reference to this object
		 */
		inline SubRegion& operator=(SubRegion&& subRegion) noexcept;

		/**
		 * Increases or decreases a sub-region by application of a scalar multiplication factor.
		 * @param factor The multiplication factor defining the new sub-region, with range [0, infinity)
		 * @return Resulting sub-region with new location and size due to the provided factor
		 */
		SubRegion operator*(const Scalar factor) const;

		/**
		 * Transforms a sub-region by application of a homography that transforms points in the initial frame to points in the new frame.
		 * @param homography The homography transforming points defined in the coordinate system of the initial frame into points defined in the coordinate system of the new frame.
		 * @return Resulting transformed subregion
		 */
		SubRegion operator*(const SquareMatrix3& homography) const;

		/**
		 * Returns whether this sub-region is not empty.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// A set of 2D triangles defining the sub-region.
		Triangles2 triangles_;

		/// A binary frame defining the sub-region per pixel.
		Frame mask_;

		/// The pixel value defining the sub-region.
		uint8_t maskValue_ = 0x00u;

		/// Bounding box of this sub-region.
		Box2 boundingBox_;
};

inline SubRegion::SubRegion() :
	maskValue_(0x00)
{
	// nothing to do here
}

inline SubRegion::SubRegion(const SubRegion& subRegion) :
	triangles_(subRegion.triangles_),
	mask_(subRegion.mask_),
	maskValue_(subRegion.maskValue_),
	boundingBox_(subRegion.boundingBox_)
{
	// nothing to do here
}

inline SubRegion::SubRegion(SubRegion&& subRegion) noexcept :
	SubRegion()
{
	*this = std::move(subRegion);
}

inline const Box2& SubRegion::boundingBox() const
{
	return boundingBox_;
}

inline bool SubRegion::isEmpty() const
{
	return triangles_.empty() && !mask_.isValid();
}

inline const Frame& SubRegion::mask() const
{
	return mask_;
}

inline const Triangles2& SubRegion::triangles() const
{
	return triangles_;
}

inline SubRegion& SubRegion::operator=(const SubRegion& subRegion)
{
	triangles_ = subRegion.triangles_;
	mask_ = subRegion.mask_;
	maskValue_ = subRegion.maskValue_;
	boundingBox_ = subRegion.boundingBox_;

	return *this;
}

inline SubRegion& SubRegion::operator=(SubRegion&& subRegion) noexcept
{
	if (this != &subRegion)
	{
		triangles_ = std::move(subRegion.triangles_);
		mask_ = std::move(subRegion.mask_);
		maskValue_ = subRegion.maskValue_;
		boundingBox_ = subRegion.boundingBox_;

		subRegion.boundingBox_ = Box2();
	}

	return *this;
}

inline SubRegion::operator bool() const
{
	return !isEmpty();
}

}

}

#endif // META_OCEAN_CV_ADVANCED_SUB_REGION_H
