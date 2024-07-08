/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESStereoView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESStereoView::GLESStereoView() :
	StereoView()
{
	// world_T_view_ is not used in GLESStereoView and should always be zero
	world_T_view_.toNull();
}

GLESStereoView::~GLESStereoView()
{
	// nothing to do here
}

HomogenousMatrix4 GLESStereoView::transformation() const
{
	return world_T_device_;
}

HomogenousMatrix4 GLESStereoView::leftTransformation() const
{
	ocean_assert(world_T_view_.isNull());

	return world_T_leftView_;
}

HomogenousMatrix4 GLESStereoView::rightTransformation() const
{
	ocean_assert(world_T_view_.isNull());

	return world_T_rightView_;
}

SquareMatrix4 GLESStereoView::leftProjectionMatrix() const
{
	ocean_assert(clip_T_view_.isNull());

	return rightClip_T_leftView_;
}

SquareMatrix4 GLESStereoView::rightProjectionMatrix() const
{
	ocean_assert(clip_T_view_.isNull());

	return rightClip_T_rightView_;
}

void GLESStereoView::setTransformation(const HomogenousMatrix4& world_T_device)
{
	ocean_assert(world_T_device.isValid());

	world_T_device_ = world_T_device;
}

void GLESStereoView::setLeftTransformation(const HomogenousMatrix4& world_T_leftView)
{
	ocean_assert(world_T_leftView.isValid());

	world_T_leftView_ = world_T_leftView;
}

void GLESStereoView::setRightTransformation(const HomogenousMatrix4& world_T_rightView)
{
	ocean_assert(world_T_rightView.isValid());

	world_T_rightView_ = world_T_rightView;
}

void GLESStereoView::setLeftProjectionMatrix(const SquareMatrix4& leftClip_T_leftView)
{
	ocean_assert(!leftClip_T_leftView.isNull());

	rightClip_T_leftView_ = leftClip_T_leftView;
}

void GLESStereoView::setRightProjectionMatrix(const SquareMatrix4& rightClip_T_rightView)
{
	ocean_assert(!rightClip_T_rightView.isNull());

	rightClip_T_rightView_ = rightClip_T_rightView;
}

SquareMatrix4 GLESStereoView::projectionMatrix() const
{
	ocean_assert(false && "GLESStereoView::projectionMatrix() must not be called!");

	return SquareMatrix4(false);
}

}

}

}
