/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESParallelView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESParallelView::GLESParallelView() :
	GLESView(),
	ParallelView(),
	width_(Scalar(2.0))
{
	calculateProjectionMatrix();
}

GLESParallelView::~GLESParallelView()
{
	// nothing to do here
}

Scalar GLESParallelView::width() const
{
	return width_;
}

bool GLESParallelView::setWidth(const Scalar width)
{
	if (width <= Numeric::eps())
	{
		return false;
	}

	if (width == width_)
	{
		return true;
	}

	width_ = width;
	calculateProjectionMatrix();

	return true;
}

bool GLESParallelView::calculateProjectionMatrix()
{
	/**
	 * Creates the following orthographic projection matrix:
	 *
	 *  --------------------------------------------------
	 * | 2/(r-l)       0            0      -(r+l)/(r-l)   |
	 * |    0       2/(t-b)         0      -(t+b)/(t-b)   |
	 * |    0          0        -2/(f-n)   -(f+n)/(f-n)   |
	 * |    0          0            0            1        |
	 *  --------------------------------------------------
	 *
	 * For a symmetric orthographic view centered at origin:
	 * left = -halfWidth, right = halfWidth
	 * bottom = -halfHeight, top = halfHeight
	 *
	 * This simplifies the matrix since (r+l) = 0 and (t+b) = 0
	 */

	ocean_assert(width_ > Numeric::eps());
	ocean_assert(aspectRatio_ > Numeric::eps());
	ocean_assert(nearDistance_ >= Numeric::eps() && farDistance_ > nearDistance_);

	const Scalar halfWidth = width_ * Scalar(0.5);
	const Scalar halfHeight = halfWidth / aspectRatio_;

	const Scalar right = halfWidth;
	const Scalar left = -halfWidth;
	const Scalar top = halfHeight;
	const Scalar bottom = -halfHeight;

	const Scalar rightLeft = Scalar(1.0) / (right - left);
	const Scalar topBottom = Scalar(1.0) / (top - bottom);
	const Scalar farNear = Scalar(1.0) / (farDistance_ - nearDistance_);

	// Initialize matrix with zeros
	clip_T_view_ = SquareMatrix4(false);

	// Set the diagonal scaling elements
	clip_T_view_(0, 0) = Scalar(2.0) * rightLeft;
	clip_T_view_(1, 1) = Scalar(2.0) * topBottom;
	clip_T_view_(2, 2) = Scalar(-2.0) * farNear;

	// Set the translation elements (in the last column)
	clip_T_view_(0, 3) = -(right + left) * rightLeft;
	clip_T_view_(1, 3) = -(top + bottom) * topBottom;
	clip_T_view_(2, 3) = -(farDistance_ + nearDistance_) * farNear;

	// Set the homogeneous coordinate (1 instead of -1 for perspective)
	clip_T_view_(3, 3) = Scalar(1.0);

	return true;
}

Line3 GLESParallelView::viewingRay(const Scalar x, const Scalar y, const unsigned int width, const unsigned int height) const
{
	ocean_assert(width != 0u && height != 0u);

	if (width == 0u || height == 0u)
	{
		throw OceanException("Invalid parameters.");
	}

	// For parallel projection, all rays have the same direction: camera's forward axis (-Z in camera space)
	const Vector3 direction = world_T_view_.rotation() * Vector3(0, 0, -1);

	// Map pixel coordinates to world position on the near plane
	// x ranges from 0 to width, y ranges from 0 to height (floating point)

	// Convert pixel to normalized coordinates [-0.5, 0.5]
	const Scalar normalizedX = (x / Scalar(width)) - Scalar(0.5);
	const Scalar normalizedY = (y / Scalar(height)) - Scalar(0.5);

	// Convert to world coordinates (note: Y is inverted, screen Y goes down, world Y goes up)
	const Scalar localX = normalizedX * width_;
	const Scalar localY = -normalizedY * (width_ / aspectRatio_);

	// Position on the near plane in camera local coordinates
	const Vector3 localPosition(localX, localY, -nearDistance_);

	// Transform to world coordinates
	const Vector3 worldPosition = world_T_view_ * localPosition;

	return Line3(worldPosition, direction);
}

}

}

}
