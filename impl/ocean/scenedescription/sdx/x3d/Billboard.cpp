/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Billboard.h"

#include "ocean/rendering/Billboard.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Billboard::Billboard(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment),
	axisOfRotation_(Vector3(0, 1, 0))
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createBillboard();
}

Billboard::NodeSpecification Billboard::specifyNode()
{
	NodeSpecification specification("Billboard");

	registerField(specification, "axisOfRotation", axisOfRotation_, ACCESS_GET_SET);

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void Billboard::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGroupingNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::BillboardRef renderingBillboard(renderingObject_);
		if (renderingBillboard)
		{
			const Vector3& axis = axisOfRotation_.value();

			if (axis.x() == 0 && axis.y() == 0 && axis.z() == 0)
			{
				renderingBillboard->setAlignmentType(Rendering::Billboard::TYPE_VIEWER);
			}
			else
			{
				renderingBillboard->setAlignmentType(Rendering::Billboard::TYPE_AXIS);
				renderingBillboard->setAlignmentAxis(axis);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void Billboard::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::BillboardRef renderingBillboard(renderingObject_);
		if (renderingBillboard)
		{
			if (fieldName == "axisOfRotation")
			{
				const Vector3& axis = axisOfRotation_.value();

				if (axis.x() == 0 && axis.y() == 0 && axis.z() == 0)
				{
					renderingBillboard->setAlignmentType(Rendering::Billboard::TYPE_VIEWER);
				}
				else
				{
					renderingBillboard->setAlignmentType(Rendering::Billboard::TYPE_AXIS);
					renderingBillboard->setAlignmentAxis(axis);
				}

				return;
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DGroupingNode::onFieldChanged(fieldName);
}

size_t Billboard::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
