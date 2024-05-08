/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESAbsoluteTransform.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESAbsoluteTransform::GLESAbsoluteTransform() :
	GLESGroup(),
	AbsoluteTransform()
{
	// nothing to do here
}

GLESAbsoluteTransform::~GLESAbsoluteTransform()
{
	//  nothing to do here
}

GLESAbsoluteTransform::TransformationType GLESAbsoluteTransform::transformationType() const
{
	const ScopedLock scopedLock(objectLock);

	return transformationType_;
}

Vector2 GLESAbsoluteTransform::headUpRelativePosition() const
{
	const ScopedLock scopedLock(objectLock);

	return headUpPosition_;
}

HomogenousMatrix4 GLESAbsoluteTransform::transformation() const
{
	const ScopedLock scopedLock(objectLock);

	return absolute_T_children_;
}

bool GLESAbsoluteTransform::setTransformationType(const TransformationType type)
{
	const ScopedLock scopedLock(objectLock);

	switch (type)
	{
		case TT_NONE:
		case TT_VIEW:
		case TT_HEAD_UP:
			transformationType_ = type;
			break;

		default:
			return false;
	}

	return true;
}

bool GLESAbsoluteTransform::setHeadUpRelativePosition(const Vector2& position)
{
	if ((position.x() != -1 && (position.x() < 0 || position.x() > 1)) || (position.y() != -1 && (position.y() < 0 || position.y() > 1)))
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	headUpPosition_ = position;

	return true;
}

void GLESAbsoluteTransform::setTransformation(const HomogenousMatrix4& absolute_T_children)
{
	ocean_assert(absolute_T_children.isValid());

	const ScopedLock scopedLock(objectLock);

	absolute_T_children_ = absolute_T_children;
}

void GLESAbsoluteTransform::addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || groupNodes.empty())
	{
		return;
	}

	HomogenousMatrix4 camera_T_object(true);

	switch (transformationType_)
	{
		case TT_NONE:
			camera_T_object = camera_T_parent;
			break;

		case TT_VIEW:
			// we need the identity transformation
			break;

		case TT_HEAD_UP:
		{
			const SquareMatrix4 iViewProjection(projectionMatrix.inverted());

			Scalar relativePositionX = Scalar(0.5);
			Scalar relativePositionY = Scalar(0.5);

			if (headUpPosition_.x() >= 0 && headUpPosition_.x() <= 1)
			{
				relativePositionX = headUpPosition_.x();
			}
			if (headUpPosition_.y() >= 0 && headUpPosition_.y() <= 1)
			{
				relativePositionY = headUpPosition_.y();
			}

			const Scalar normalizedPositionX = 2 * relativePositionX - 1;
			const Scalar normalizedPositionY = -2 * relativePositionY + 1;

			Vector3 ray(iViewProjection * Vector3(normalizedPositionX, normalizedPositionY, 1));
			ocean_assert(Numeric::isNotEqualEps(ray.z()));

			ray *= Numeric::ratio(1, -ray.z());
			ocean_assert(Numeric::isEqual(ray.z(), -1));

			const Vector3 localPosition(ray * (framebuffer.view()->nearDistance() * Scalar(1.01)));

			camera_T_object = HomogenousMatrix4(localPosition);
		}
	}

	camera_T_object *= absolute_T_children_;

	if (groupLights.empty())
	{
		for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		{
			const SmartObjectRef<GLESNode> node(*i);
			ocean_assert(node);

			node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, lights, traverser);
		}
	}
	else
	{
		Lights newLights(lights);
		newLights.reserve(newLights.size() + groupLights.size());

		for (LightSet::const_iterator i = groupLights.begin(); i != groupLights.end(); ++i)
		{
			if ((*i)->enabled())
			{
				newLights.emplace_back(*i, camera_T_object);
			}
		}

		for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		{
			const SmartObjectRef<GLESNode> node(*i);
			ocean_assert(node);

			node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, newLights, traverser);
		}
	}
}

}

}

}
