/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_APPLE_GLES_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_APPLE_GLES_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#include "ocean/rendering/WindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Apple
{

/**
 * This class is the base class for all GLESceneGraph window framebuffers for Apple platforms.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESWindowFramebuffer :
	virtual public GLESFramebuffer,
	virtual public WindowFramebuffer
{
	friend class GLESEngineApple;

	public:

		/**
		 * Initialize the OpenGL ES dispaly and context.
		 * @return True, if succeeded
		 */
		bool initializeContext() override;

		/**
		 * Returns the transformation between display and device.
		 * @see Framebuffer::device_T_display()
		 */
		HomogenousMatrix4 device_T_display() const override;

		/**
		 * Sets the transformation between display and device.
		 * @see Framebuffer::setDevice_T_display().
		 */
		bool setDevice_T_display(const HomogenousMatrix4& device_T_display) override;

	private:

		/**
		 * Creates a new window framebuffer.
		 */
		GLESWindowFramebuffer();

		/**
		 * Destructs a window framebuffer.
		 */
		~GLESWindowFramebuffer() override;

	protected:

		/// The traverser which is used for rendering.
		GLESTraverser traverser_;

		/// The transformation between display and device
		HomogenousMatrix4 device_T_display_;
};

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_APPLE_GLES_WINDOW_FRAMEBUFFER_H
