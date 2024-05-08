/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESPerspectiveView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESPerspectiveView::GLESPerspectiveView() :
	GLESView(),
	PerspectiveView(),
	fovX_(Numeric::deg2rad(60))
{
	calculateProjectionMatrix();
}

GLESPerspectiveView::~GLESPerspectiveView()
{
	// nothing to do here
}

Scalar GLESPerspectiveView::fovX() const
{
	return fovX_;
}

bool GLESPerspectiveView::setFovX(const Scalar fovx)
{
	if (fovx <= Numeric::eps() || fovx >= Numeric::pi())
	{
		return false;
	}

	if (fovx == fovX_)
	{
		return true;
	}

	fovX_ = fovx;
	calculateProjectionMatrix();

	return true;
}

bool GLESPerspectiveView::calculateProjectionMatrix()
{
	clip_T_view_ = SquareMatrix4::projectionMatrix(fovX_, aspectRatio_, nearDistance_, farDistance_);

	return true;
}

void GLESPerspectiveView::setProjectionMatrix(const SquareMatrix4& clip_T_view)
{
	ocean_assert(!clip_T_view.isSingular());

	clip_T_view_ = clip_T_view;
	fovX_ = Scalar(-1);
}

Line3 GLESPerspectiveView::viewingRay(const Scalar x, const Scalar y, const unsigned int width, const unsigned int height) const
{
	ocean_assert(width != 0u && height != 0u);

	if (width == 0u || height == 0u)
	{
		throw OceanException("Invalid parameters.");
	}

	const Scalar width_2 = Scalar(width) * Scalar(0.5);
	const Scalar height_2 = Scalar(height) * Scalar(0.5);

	const Scalar xRelative = x - width_2;
	const Scalar yRelative = y - height_2;

	const Scalar distance = width_2 / Numeric::tan(fovX_ * Scalar(0.5));
	ocean_assert(distance > 0);

	Vector3 direction(xRelative, -yRelative, -distance);
	ocean_assert(direction.length() > 0);

	direction.normalize();

	return Line3(world_T_view_.translation(), world_T_view_.rotation() * direction);
}

}

}

}
