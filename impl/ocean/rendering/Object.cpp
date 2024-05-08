/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Object.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Node.h"
#include "ocean/rendering/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

Object::Object() :
	objectId(invalidObjectId)
{
	static Lock objectIdCounterLock;

	const ScopedLock scopedLock(objectIdCounterLock);

	static ObjectId objectIdCounter = ObjectId(0);

	objectId = ++objectIdCounter;
}

Object::~Object()
{
	if (objectParents.empty() == false)
	{
		ocean_assert(objectParents.empty());
	}
}

void Object::setName(const std::string& name)
{
	if (objectName == name)
	{
		return;
	}

	ObjectRefManager::get().changeRegisteredObject(objectId, objectName, name);
	objectName = name;
}

Engine& Object::engine() const
{
	ocean_assert(Rendering::Manager::get().engine(engineName()).isNull() == false);
	return *Rendering::Manager::get().engine(engineName());
}

Object::ObjectType Object::type() const
{
	return TYPE_OBJECT;
}

ObjectRefs Object::parentObjects() const
{
	ObjectIdMap objectParentsCopy;

	{
		const ScopedLock scopedLock(objectLock);
		objectParentsCopy = objectParents;
	}

	const ScopedLock scopedLock(objectLock);

	ObjectRefs parentObjects;
	for (ObjectIdMap::const_iterator i = objectParentsCopy.begin(); i != objectParentsCopy.end(); ++i)
	{
		const ObjectRef parent(engine().object(i->first));

		if (parent)
		{
			parentObjects.push_back(parent);
		}
	}

	return parentObjects;
}

ObjectRefSet Object::parentNodes() const
{
	ObjectIdMap objectParentsCopy;

	{
		const ScopedLock scopedLock(objectLock);
		objectParentsCopy = objectParents;
	}

	ObjectRefSet parentNodes;
	for (ObjectIdMap::const_iterator i = objectParentsCopy.begin(); i != objectParentsCopy.end(); ++i)
	{
		const ObjectRef parent(engine().object(i->first));

		if (parent)
		{
			const NodeRef parentNode(parent);

			if (parentNode)
			{
				parentNodes.insert(parent);
			}
			else
			{
				ObjectRefSet parentParentObjects(parent->parentNodes());
				parentNodes.insert(parentParentObjects.begin(), parentParentObjects.end());
			}
		}
	}

	return parentNodes;
}

std::string Object::descriptiveInformation() const
{
	const ScopedLock scopedLock(objectLock);

	if (objectName.empty())
	{
		return std::string("Object with type ") + Object::translateObjectType(type());
	}
	else
	{
		return std::string("Object '") + objectName + std::string("' with type ") + Object::translateObjectType(type());
	}
}

std::string Object::translateObjectType(const ObjectType objectType)
{
	switch (objectType)
	{
		case TYPE_UNKNOWN:
			return "UNKNOWN";

		case TYPE_ABSOLUTE_TRANSFORM:
			return "ABSOLUTE_TRANSFORM";

		case TYPE_ATTRIBUTE:
			return "ATTRIBUTE";

		case TYPE_ATTRIBUTE_SET:
			return "ATTRIBUTE_SET";

		case TYPE_BACKGROUND:
			return "BACKGROUND";

		case TYPE_BILLBOARD:
			return "BILLBOARD";

		case TYPE_BITMAP_FRAMEBUFFER:
			return "BITMAP_FRAMEBUFFER";

		case TYPE_BLEND_ATTRIBUTE:
			return "BLEND_ATTRIBUTE";

		case TYPE_BOX:
			return "BOX";

		case TYPE_CONE:
			return "CONE";

		case TYPE_CYLINDER:
			return "CYLINDER";

		case TYPE_DEPTH_ATTRIBUTE:
			return "DEPTH_ATTRIBUTE";

		case TYPE_DIRECTIONAL_LIGHT:
			return "DIRECTIONAL_LIGHT";

		case TYPE_FRAMEBUFFER:
			return "FRAMEBUFFER";

		case TYPE_FRAME_TEXTURE_2D:
			return "FRAME_TEXTURE_2D";

		case TYPE_GEOMETRY:
			return "GEOMETRY";

		case TYPE_GROUP:
			return "GROUP";

		case TYPE_INDEPENDENT_PRIMITIVE:
			return "INDEPENDENT_PRIMITIVE";

		case TYPE_LIGHT_SOURCE:
			return "LIGHT_SOURCE";

		case TYPE_LINES:
			return "LINES";

		case TYPE_LINE_STRIPS:
			return "LINE_STRIPS";

		case TYPE_LOD:
			return "LOD";

		case TYPE_MATERIAL:
			return "MATERIAL";

		case TYPE_NODE:
			return "NODE";

		case TYPE_OBJECT:
			return "OBJECT";

		case TYPE_PARALLEL_VIEW:
			return "PARALLEL_VIEW";

		case TYPE_PERSPECTIVE_VIEW:
			return "PERSPECTIVE_VIEW";

		case TYPE_PHANTOM_ATTRIBUTE:
			return "PHANTOM_ATTRIBUTE";

		case TYPE_POINT_LIGHT:
			return "POINT_LIGHT";

		case TYPE_POINTS:
			return "POINTS";

		case TYPE_PRIMITIVE:
			return "PRIMITIVE";

		case TYPE_PRIMITIVE_ATTRIBUTE:
			return "PRIMITIVE_ATTRIBUTE";

		case TYPE_QUAD_STRIPS:
			return "QUAD_STRIPS";

		case TYPE_QUADS:
			return "QUADS";

		case TYPE_RENDERABLE:
			return "RENDERABLE";

		case TYPE_SCENE:
			return "SCENE";

		case TYPE_SHADER_PROGRAM:
			return "SHADER_PROGRAM";

		case TYPE_SHAPE:
			return "SHAPE";

		case TYPE_SKY_BACKGROUND:
			return "SKY_BACKGROUND";

		case TYPE_SPHERE:
			return "SPHERE";

		case TYPE_SPOT_LIGHT:
			return "SPOT_LIGHT";

		case TYPE_STEREO_ATTRIBUTE:
			return "STEREO_ATTRIBUTE";

		case TYPE_STEREO_VIEW:
			return "STEREO_VIEW";

		case TYPE_STRIP_PRIMITIVE:
			return "STRIP_PRIMITIVE";

		case TYPE_SWITCH:
			return "SWITCH";

		case TYPE_TEXT:
			return "TEXT";

		case TYPE_TEXTURE:
			return "TEXTURE";

		case TYPE_TEXTURE_2D:
			return "TEXTURE_2D";

		case TYPE_MEDIA_TEXTURE_2D:
			return "MEDIA_TEXTURE_2D";

		case TYPE_TEXTURE_FRAMEBUFFER:
			return "TEXTURE_FRAMEBUFFER";

		case TYPE_TEXTURES:
			return "TEXTURES";

		case TYPE_TRANSFORM:
			return "TRANSFORM";

		case TYPE_TRIANGLE_FANS:
			return "TRIANGLE_FANS";

		case TYPE_TRIANGLES:
			return "TRIANGLES";

		case TYPE_TRIANGLE_STRIPS:
			return "TRIANGLE_STRIPS";

		case TYPE_UNDISTORTED_BACKGROUND:
			return "UNDISTORTED_BACKGROUND";

		case TYPE_VERTEX_SET:
			return "VERTEX_SET";

		case TYPE_VIEW:
			return "VIEW";

		case TYPE_WINDOW_FRAMEBUFFER:
			return "WINDOW_FRAMEBUFFER";
	}

	ocean_assert(false && "Invalid type!");
	return "INVALID";
}

void Object::registerThisObjectAsParent(const ObjectRef& child)
{
	if (child)
	{
		child->registerParent(id());
	}
}

void Object::unregisterThisObjectAsParent(const ObjectRef& child)
{
	if (child)
	{
		child->unregisterParent(id());
	}
}

}

}
