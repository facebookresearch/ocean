/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIPerspectiveView.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIPerspectiveView::GIPerspectiveView() :
	GIView(),
	PerspectiveView(),
	viewFovX(Numeric::deg2rad(60))
{
	calculateProjectionMatrix();
}

GIPerspectiveView::~GIPerspectiveView()
{
	// nothing to do here
}

Scalar GIPerspectiveView::fovX() const
{
	return viewFovX;
}

bool GIPerspectiveView::setFovX(const Scalar fovx)
{
	if (fovx <= 0 || fovx >= Numeric::pi())
		return false;

	if (fovx == viewFovX)
		return true;

	viewFovX = fovx;
	calculateProjectionMatrix();

	return true;
}

void GIPerspectiveView::calculateProjectionMatrix()
{
	viewProjectionMatrix = SquareMatrix4::projectionMatrix(viewFovX, viewAspectRatio, viewNearDistance, viewFarDistance);
}

Line3 GIPerspectiveView::viewingRay(const Scalar x, const Scalar y, const unsigned int width, const unsigned int height) const
{
	ocean_assert(width != 0u && height != 0u);

	if (width == 0u || height == 0u)
		throw OceanException("Invalid parameters.");

	const Scalar width2 = Scalar(width) * Scalar(0.5);
	const Scalar height2 = Scalar(height) * Scalar(0.5);

	const Scalar xRelative = x - width2;
	const Scalar yRelative = y - height2;

	const Scalar distance = width2 / Numeric::tan(viewFovX * Scalar(0.5));
	ocean_assert(distance > 0);

	Vector3 direction(xRelative, -yRelative, -distance);
	ocean_assert(direction.length() > 0);

	direction.normalize();

	return Line3(viewMatrix.translation(), viewMatrix.rotation() * direction);
}

}

}

}
