/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ANDROID_GLES_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_ANDROID_GLES_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"

#include "ocean/rendering/WindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Android
{

/**
 * This class is the base class for all GLESceneGraph window framebuffers for Android (not Oculus) platforms.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESWindowFramebuffer :
	virtual public GLESFramebuffer,
	virtual public WindowFramebuffer
{
	friend class GLESEngineAndroid;

	public:

		/**
		 * Initialize the OpenGL ES dispaly and context.
		 * @return True, if succeeded
		 */
		bool initializeContext() override;

	protected:

		/**
		 * Creates a new AGLESceneGraph window framebuffer.
		 */
		GLESWindowFramebuffer();

		/**
		 * Destructs a AGLESceneGraph window framebuffer.
		 */
		~GLESWindowFramebuffer() override;
};

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ANDROID_GLES_WINDOW_FRAMEBUFFER_H
