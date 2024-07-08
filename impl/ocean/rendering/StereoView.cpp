/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/StereoView.h"

namespace Ocean
{

namespace Rendering
{

StereoView::StereoView() :
	PerspectiveView()
{
	// nothing to do here
}

StereoView::~StereoView()
{
	// nothing to do here
}

Scalar StereoView::focusDistance() const
{
	throw NotSupportedException("This view does not support a focus distance.");
}

bool StereoView::eyesReversed() const
{
	throw NotSupportedException("This view does not support reversed eyes.");
}

StereoView::StereoType StereoView::stereoType() const
{
	throw NotSupportedException("This view does not support reversed eyes.");
}

HomogenousMatrix4 StereoView::transformation() const
{
	throw NotSupportedException("This view does not support transformation().");
}

HomogenousMatrix4 StereoView::leftTransformation() const
{
	throw NotSupportedException("This view does not support leftTransformation().");
}

HomogenousMatrix4 StereoView::rightTransformation() const
{
	throw NotSupportedException("This view does not support rightTransformation().");
}

SquareMatrix4 StereoView::leftProjectionMatrix() const
{
	throw NotSupportedException("This view does not support leftProjectionMatrix().");
}

SquareMatrix4 StereoView::rightProjectionMatrix() const
{
	throw NotSupportedException("This view does not support rightProjectionMatrix().");
}

void StereoView::setFocusDistance(const Scalar /*focus*/)
{
	throw NotSupportedException("This view does not support a focus distance.");
}

void StereoView::setEyesReversed(const bool /*reverse*/)
{
	throw NotSupportedException("This view does not support reversed eyes.");
}

bool StereoView::setStereoType(const StereoType /*type*/)
{
	throw NotSupportedException("This view does not support reversed eyes.");
}

void StereoView::setTransformation(const HomogenousMatrix4& /*world_T_device*/)
{
	throw NotSupportedException("This view does not support an explicit device transformation.");
}

void StereoView::setLeftTransformation(const HomogenousMatrix4& /*world_T_leftView*/)
{
	throw NotSupportedException("This view does not support an explicit left transformation.");
}

void StereoView::setRightTransformation(const HomogenousMatrix4& /*world_T_rightView*/)
{
	throw NotSupportedException("This view does not support an explicit right transformation.");
}

void StereoView::setLeftProjectionMatrix(const SquareMatrix4& /*matrix*/)
{
	throw NotSupportedException("This view does not support an explicit left projection matrix.");
}

void StereoView::setRightProjectionMatrix(const SquareMatrix4& /*matrix*/)
{
	throw NotSupportedException("This view does not support an explicit right projection matrix.");
}

StereoView::ObjectType StereoView::type() const
{
	return TYPE_STEREO_VIEW;
}

SquareMatrix4 StereoView::projectionMatrix() const
{
	ocean_assert(false && "StereoView::projectionMatrix() must not be called!");

	return SquareMatrix4(false);
}

}

}
