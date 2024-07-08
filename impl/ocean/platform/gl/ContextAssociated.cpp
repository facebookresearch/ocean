/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
