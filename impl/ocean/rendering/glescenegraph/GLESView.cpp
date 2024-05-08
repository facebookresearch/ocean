/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESView.h"
#include "ocean/rendering/glescenegraph/GLESGroup.h"
#include "ocean/rendering/glescenegraph/GLESNode.h"
#include "ocean/rendering/glescenegraph/GLESScene.h"

#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESView::GLESView() :
	GLESObject(),
	View()
{
	setUseHeadlight(true);

	headlight_ = engine().factory().createPointLight();
	headlight_->set(RGBAColor(0.3f, 0.3f, 0.3f), RGBAColor(0.7f, 0.7f, 0.7f), RGBAColor(0.5f, 0.5f, 0.5f), 1);
}

GLESView::~GLESView()
{
	// nothing to do here
}

Scalar GLESView::aspectRatio() const
{
	return aspectRatio_;
}

Scalar GLESView::nearDistance() const
{
	return nearDistance_;
}

Scalar GLESView::farDistance() const
{
	return farDistance_;
}

SquareMatrix4 GLESView::projectionMatrix() const
{
	return clip_T_view_;
}

HomogenousMatrix4 GLESView::transformation() const
{
	return world_T_view_;
}

RGBAColor GLESView::backgroundColor() const
{
	return backgroundColor_;
}

bool GLESView::useHeadlight() const
{
	return useHeadlight_;
}

const LightSourceRef& GLESView::headlight() const
{
	return headlight_;
}

bool GLESView::setAspectRatio(const Scalar aspectRatio)
{
	if (aspectRatio <= Numeric::eps())
	{
		return false;
	}

	if (aspectRatio == aspectRatio_)
	{
		return true;
	}

	const ScopedLock scopedLock(objectLock);

	aspectRatio_ = aspectRatio;
	calculateProjectionMatrix();

	return true;
}

bool GLESView::setNearDistance(const Scalar distance)
{
	if (nearDistance_ >= farDistance_ || nearDistance_ < Numeric::eps())
	{
		return false;
	}

	if (nearDistance_ == distance)
	{
		return true;
	}

	const ScopedLock scopedLock(objectLock);

	nearDistance_ = distance;
	calculateProjectionMatrix();

	return true;
}

bool GLESView::setFarDistance(const Scalar distance)
{
	if (farDistance_ <= nearDistance_)
	{
		return false;
	}

	if (farDistance_ == distance)
	{
		return true;
	}

	const ScopedLock scopedLock(objectLock);

	farDistance_ = distance;
	calculateProjectionMatrix();

	return View::setFarDistance(distance);
}

bool GLESView::setNearFarDistance(const Scalar nearDistance, const Scalar farDistance)
{
	if (nearDistance <= Numeric::eps() || nearDistance > farDistance)
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	nearDistance_ = nearDistance;
	farDistance_  = farDistance;
	calculateProjectionMatrix();

	return View::setNearFarDistance(nearDistance, farDistance);
}

void GLESView::setTransformation(const HomogenousMatrix4& transformation)
{
	ocean_assert(transformation.isValid());
	ocean_assert(transformation.rotationMatrix().isOrthonormal(Numeric::weakEps()));

	const ScopedLock scopedLock(objectLock);

	world_T_view_ = transformation;
}

bool GLESView::setBackgroundColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	backgroundColor_ = color;
	return false;
}

void GLESView::setUseHeadlight(const bool state)
{
	const ScopedLock scopedLock(objectLock);

	useHeadlight_ = state;
}

bool GLESView::setPhantomMode(const PhantomAttribute::PhantomMode mode)
{
	const ScopedLock scopedLock(objectLock);

	switch (mode)
	{
		case PhantomAttribute::PM_COLOR:
		case PhantomAttribute::PM_DEBUG:
		case PhantomAttribute::PM_VIDEO:
		case PhantomAttribute::PM_DEFAULT:
			phantomMode_ = mode;
			return true;

		default:
			break;
	}

	return false;
}

PhantomAttribute::PhantomMode GLESView::phantomMode() const
{
	return phantomMode_;
}

bool GLESView::calculateProjectionMatrix()
{
	return false;
}

}

}

}
