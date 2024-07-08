/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DGroupingNode.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DGroupingNode::X3DGroupingNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment)
{
	// nothing to do here
}

X3DGroupingNode::~X3DGroupingNode()
{
	if (initialized_)
	{
		for (MultiNode::Values::const_iterator i = children_.values().begin(); i != children_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}
	}
}

void X3DGroupingNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "addChildren", addChildren_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "removeChildren", removeChildren_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "children", children_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));

	X3DChildNode::registerFields(specification);
	X3DBoundedObject::registerFields(specification);
}

void X3DGroupingNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DBoundedObject::onInitialize(scene, timestamp);
	X3DChildNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::GroupRef renderingGroup(renderingObject_);
		if (renderingGroup)
		{
			for (MultiNode::Values::const_iterator i = children_.values().begin(); i != children_.values().end(); ++i)
			{
				const SDXNodeRef childNode(*i);

				registerThisNodeAsParent(childNode);
				childNode->initialize(scene, timestamp);

				renderingGroup->addChild(childNode->renderingObject());
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

bool X3DGroupingNode::onFieldChanging(const std::string& fieldName, const Field& field)
{
	Rendering::GroupRef renderingGroup(renderingObject_);
	ocean_assert(renderingGroup);

	if (renderingGroup && field.isType(Field::TYPE_NODE, 1u))
	{
		const MultiNode& multiNode = dynamic_cast<const MultiNode&>(field);
		const SDXNodeSet ancestors(ancestorNodes());

		if (fieldName == "children")
		{
			// remove all previous child nodes
			for (MultiNode::Values::const_iterator i = children_.values().begin(); i != children_.values().end(); ++i)
			{
				const SDXNodeRef childNode(*i);
				ocean_assert(childNode);

				renderingGroup->removeChild(childNode->renderingObject());
				unregisterThisNodeAsParent(childNode);
			}

			children_.values().clear();

			// add the new nodes
			for (MultiNode::Values::const_iterator i = multiNode.values().begin(); i != multiNode.values().end(); ++i)
			{
				// check whether the given node is no ancestor node
				if (ancestors.find(*i) == ancestors.end())
				{
					children_.values().push_back(*i);
				}
				else
				{
					Log::warning() << "One of the new child nodes is an ancestor node.";
				}
			}

			children_.setTimestamp(field.timestamp());

			for (MultiNode::Values::const_iterator i = multiNode.values().begin(); i != multiNode.values().end(); ++i)
			{
				const SDXNodeRef childNode(*i);
				ocean_assert(childNode);

				renderingGroup->addChild(childNode->renderingObject());
				registerThisNodeAsParent(childNode);
			}

			return true;
		}

		if (fieldName == "addChildren")
		{
			for (MultiNode::Values::const_iterator i = multiNode.values().begin(); i != multiNode.values().end(); ++i)
			{
				bool existAlready = false;
				for (MultiNode::Values::const_iterator iC = children_.values().begin(); iC != children_.values().end(); ++iC)
				{
					if (*i == *iC)
					{
						existAlready = true;
						break;
					}
				}

				if (existAlready)
				{
					continue;
				}

				// check whether the given node is no ancestor node
				if (ancestors.find(*i) == ancestors.end())
				{
					children_.values().push_back(*i);
					registerThisNodeAsParent(*i);

					const SDXNodeRef childNode(*i);
					renderingGroup->addChild(childNode->renderingObject());
				}
				else
				{
					Log::warning() << "One of the new child nodes is an ancestor node.";
				}
			}
		}
		else if (fieldName == "removeChildren")
		{
			for (MultiNode::Values::const_iterator i = multiNode.values().begin(); i != multiNode.values().end(); ++i)
			{
				for (MultiNode::Values::iterator iC = children_.values().begin(); iC != children_.values().end(); /* noop */)
				{
					if (*i == *iC)
					{
						unregisterThisNodeAsParent(*i);

						const SDXNodeRef childNode(*i);
						renderingGroup->removeChild(childNode->renderingObject());

						iC = children_.values().erase(iC);
						continue;
					}

					++iC;
				}
			}
		}

	}

	return X3DChildNode::onFieldChanging(fieldName, field);
}

}

}

}

}
