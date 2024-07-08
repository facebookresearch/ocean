/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTransform.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTransform::GLESTransform() :
	GLESGroup(),
	Transform(),
	parent_T_object_(true)
{
	// nothing to do here
}

GLESTransform::~GLESTransform()
{
	// nothing to do here
}

BoundingBox GLESTransform::boundingBox(const bool involveLocalTransformation) const
{
	const ScopedLock scopedLock(objectLock);

	BoundingBox result;

	for (const NodeRef& groupNode : groupNodes)
	{
		ocean_assert(groupNode);
		const BoundingBox groupBoundingBox = groupNode->boundingBox(true /*involveLocalTransformation*/);

		if (groupBoundingBox.isValid())
		{
			if (involveLocalTransformation)
			{
				result += groupBoundingBox * parent_T_object_;
			}
			else
			{
				result += groupBoundingBox;
			}
		}
	}

	return result;
}

HomogenousMatrix4 GLESTransform::transformation() const
{
	const ScopedLock scopedLock(objectLock);

	return parent_T_object_;
}

GLESTransform::SharedTransformModifier GLESTransform::transformModifier() const
{
	const ScopedLock scopedLock(objectLock);

	return transformModifier_;
}

void GLESTransform::setTransformation(const HomogenousMatrix4& parent_T_transform)
{
	const ScopedLock scopedLock(objectLock);

	parent_T_object_ = parent_T_transform;
}

void GLESTransform::setTransformModifier(SharedTransformModifier transformModifier)
{
	transformModifier_ = std::move(transformModifier);
}

void GLESTransform::addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || groupNodes.empty() || parent_T_object_.rotationMatrix().isNull())
	{
		return;
	}

	const HomogenousMatrix4 camera_T_object = transformModifier_ ? (camera_T_parent * parent_T_object_ * transformModifier_->transformation()) : (camera_T_parent * parent_T_object_);

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
