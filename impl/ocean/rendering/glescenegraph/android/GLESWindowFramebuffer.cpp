/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/android/GLESWindowFramebuffer.h"

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Android
{

GLESWindowFramebuffer::GLESWindowFramebuffer() :
	GLESFramebuffer(),
	WindowFramebuffer()
{
	initialize();
}

GLESWindowFramebuffer::~GLESWindowFramebuffer()
{
	release();
}

bool GLESWindowFramebuffer::initializeContext()
{
	// nothing to do here
	return true;
}

}

}

}

}
