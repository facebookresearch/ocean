// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/gl/ContextAssociated.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

ContextAssociated::~ContextAssociated()
{
	// nothing to do here
}

bool ContextAssociated::setContext(Context& context)
{
	if (associatedContext_ == &context)
		return true;

	if (associatedContext_ != nullptr)
		return false;

	associatedContext_ = &context;
	return true;
}

}

}

}
