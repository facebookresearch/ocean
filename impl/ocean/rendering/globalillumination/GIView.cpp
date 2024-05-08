/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIView.h"
#include "ocean/rendering/globalillumination/GIGroup.h"
#include "ocean/rendering/globalillumination/GINode.h"
#include "ocean/rendering/globalillumination/GIScene.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIView::GIView() :
	GIObject(),
	View(),
	viewProjectionMatrix(false),
	viewMatrix(true),
	invertedViewMatrix(true),
	viewAspectRatio(1),
	viewNearDistance(Scalar(0.01)),
	viewFarDistance(Scalar(1000)),
	viewBackgroundColor(0, 0, 0),
	viewUseHeadlight(true)
{
	setUseHeadlight(true);

	viewHeadlight = engine().factory().createPointLight();
	viewHeadlight->set(RGBAColor(0.3f, 0.3f, 0.3f), RGBAColor(0.7f, 0.7f, 0.7f), RGBAColor(0.5f, 0.5f, 0.5f), 1);
}

GIView::~GIView()
{
	// nothing to do here
}

Scalar GIView::aspectRatio() const
{
	return viewAspectRatio;
}

Scalar GIView::nearDistance() const
{
	return viewNearDistance;
}

Scalar GIView::farDistance() const
{
	return viewFarDistance;
}

SquareMatrix4 GIView::projectionMatrix() const
{
	return viewProjectionMatrix;
}

HomogenousMatrix4 GIView::transformation() const
{
	return viewMatrix;
}

RGBAColor GIView::backgroundColor() const
{
	return viewBackgroundColor;
}

bool GIView::useHeadlight() const
{
	return viewUseHeadlight;
}

const LightSourceRef& GIView::headlight() const
{
	return viewHeadlight;
}

bool GIView::setAspectRatio(const Scalar aspectRatio)
{
	if (aspectRatio <= 0)
		return false;

	if (aspectRatio == viewAspectRatio)
		return true;

	viewAspectRatio = aspectRatio;
	calculateProjectionMatrix();

	return true;
}

bool GIView::setNearDistance(const Scalar distance)
{
	if (viewNearDistance >= viewFarDistance)
		return false;

	if (viewNearDistance == distance)
		return true;

	viewNearDistance = distance;
	calculateProjectionMatrix();

	return true;
}

bool GIView::setFarDistance(const Scalar distance)
{
	if (viewFarDistance <= viewNearDistance)
		return false;

	if (viewFarDistance == distance)
		return true;

	viewFarDistance = distance;
	calculateProjectionMatrix();

	return View::setFarDistance(distance);
}

bool GIView::setNearFarDistance(const Scalar nearDistance, const Scalar farDistance)
{
	if (nearDistance <= 0 || nearDistance > farDistance)
		return false;

	viewNearDistance = nearDistance;
	viewFarDistance = farDistance;
	calculateProjectionMatrix();

	return View::setNearFarDistance(nearDistance, farDistance);
}

void GIView::setTransformation(const HomogenousMatrix4& transformation)
{
	ocean_assert(transformation.isValid());
	viewMatrix = transformation;
	invertedViewMatrix = viewMatrix.inverted();
}

bool GIView::setBackgroundColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	viewBackgroundColor = color;
	return false;
}

void GIView::setUseHeadlight(const bool state)
{
	viewUseHeadlight = state;
}

}

}

}
