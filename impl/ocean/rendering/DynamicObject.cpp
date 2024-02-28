// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/DynamicObject.h"
#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

DynamicObject::DynamicObject() :
	Object()
{
	// nothing to do here
}

DynamicObject::~DynamicObject()
{
	// nothing to do here
}

void DynamicObject::registerDynamicUpdateObject()
{
	engine().registerDynamicObject(this);
}

void DynamicObject::unregisterDynamicUpdateObject()
{
	engine().unregisterDynamicObject(this);
}

}

}
