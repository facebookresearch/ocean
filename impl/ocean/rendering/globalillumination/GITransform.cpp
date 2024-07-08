/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GITransform.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GITransform::GITransform() :
	GIGroup(),
	Transform(),
	parent_T_object_(true)
{
	// nothing to do here
}

GITransform::~GITransform()
{
	// nothing to do here
}

HomogenousMatrix4 GITransform::transformation() const
{
	return parent_T_object_;
}

GITransform::SharedTransformModifier GITransform::transformModifier() const
{
	return transformModifier_;
}

void GITransform::setTransformation(const HomogenousMatrix4& parent_T_transform)
{
	parent_T_object_ = parent_T_transform;
}

void GITransform::setTransformModifier(SharedTransformModifier transformModifier)
{
	transformModifier_ = std::move(transformModifier);
}


BoundingBox GITransform::boundingBox(const bool involveLocalTransformation) const
{
	if (groupNodes.empty())
	{
		return BoundingBox();
	}

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

BoundingSphere GITransform::boundingSphere(const bool involveLocalTransformation) const
{
	return BoundingSphere(boundingBox(involveLocalTransformation));
}

void GITransform::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const
{
	ocean_assert(modelTransform.isValid());

	if (nodeVisible && !parent_T_object_.rotationMatrix().isNull())
	{
		const HomogenousMatrix4 newModelTransform = transformModifier_ ? (modelTransform * parent_T_object_ * transformModifier_->transformation()) : (modelTransform * parent_T_object_);

		if (groupLights.empty())
		{
			for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
			{
				SmartObjectRef<GINode> node(*i);
				ocean_assert(node);

				node->buildTracing(group, newModelTransform, lightSources);
			}
		}
		else
		{
			LightSources newLightSources(lightSources);

			for (LightSet::const_iterator i = groupLights.begin(); i != groupLights.end(); ++i)
			{
				newLightSources.push_back(LightPair(*i, newModelTransform));
			}

			for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
			{
				SmartObjectRef<GINode> node(*i);
				ocean_assert(node);

				node->buildTracing(group, newModelTransform, newLightSources);
			}
		}
	}
}

}

}

}
