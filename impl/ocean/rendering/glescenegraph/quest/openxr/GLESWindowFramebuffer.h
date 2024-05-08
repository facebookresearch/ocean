/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_QUEST_OPENXR_GLES_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_QUEST_OPENXR_GLES_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR

#include "ocean/platform/meta/quest/openxr/GLESFramebuffer.h"

#include "ocean/rendering/WindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Quest
{

// Forward declaration.
class GLESEngineQuest;

namespace OpenXR
{

/**
 * This class is the base class for all GLESceneGraph window framebuffers for Quest platforms.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESWindowFramebuffer :
	virtual public GLESFramebuffer,
	virtual public WindowFramebuffer
{
	friend class Quest::GLESEngineQuest;

	public:

		/// The number of framebuffers used.
		static constexpr size_t numberEyes_ = 2;

	public:

		/**
		 * Initializes the framebuffer based on the platform specific id.
		 * For Quest platforms, the id is the pointer to the OpenXR::Session object.
		 * @see WindowFramebuffer::initializeById().
		 */
		bool initializeById(const size_t id, const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

		/**
		 * Returns the width of an individual framebuffer.
		 * @param eyeIndex The index of the eye for which the width will be returned, with range [0, numberEyes_ - 1]
		 * @return The width of the framebuffer, in pixel, with range [0, infinity)
		 */
		inline unsigned int width(const size_t eyeIndex) const;

		/**
		 * Returns the height of an individual framebuffer.
		 * @param eyeIndex The index of the eye for which the height will be returned, with range [0, numberEyes_ - 1]
		 * @return The height of the framebuffer, in pixel, with range [0, infinity)
		 */
		inline unsigned int height(const size_t eyeIndex) const;

		/**
		 * Returns OpenXR's swap chain for the individual eyes/framebuffers.
		 * @param eyeIndex The index of the eye for which the swap chain will be returned, with range [0, numberEyes_ - 1]
		 * @return The requested swap chain
		 */
		inline XrSwapchain xrSwapchain(const size_t eyeIndex) const;

		/**
		 * Returns the index of the current texture of OpenXR's swap chain for the individual eyes/framebuffers.
		 * @param eyeIndex The index of the eye for which the index will be returned, with range [0, numberEyes_ - 1]
		 * @return The requested swap chain
		 */
		inline size_t textureSwapChainIndex(const size_t eyeIndex) const;

	protected:

		/**
		 * Creates a new window framebuffer.
		 */
		GLESWindowFramebuffer(const Framebuffer::FramebufferConfig& config);

		/**
		 * Destructs a window framebuffer.
		 */
		~GLESWindowFramebuffer() override;

		/**
		 * Initialize the OpenGL ES dispaly and context.
		 * @return True, if succeeded
		 */
		bool initializeContext() override;

		/**
		 * Releases the framebuffer.
		 */
		void release() override;

	protected:

		/// The actual implementation of the Quest specific framebuffer(s).
		Platform::Meta::Quest::OpenXR::GLESFramebuffers glesFramebuffers_;

		/// The traverser which is used for rendering.
		GLESTraverser traverser_;

		/// The framebuffer's id which is a pointer to the ovrJava object.
		size_t id_ = 0;

		/// The configuration to be used.
		Framebuffer::FramebufferConfig config_;
};

inline unsigned int GLESWindowFramebuffer::width(const size_t eyeIndex) const
{
	ocean_assert(eyeIndex < numberEyes_ && eyeIndex < glesFramebuffers_.size());

	if (eyeIndex < glesFramebuffers_.size())
	{
		return glesFramebuffers_[eyeIndex].width();
	}

	ocean_assert(false && "Invalid eye index!");
	return 0u;
}

inline unsigned int GLESWindowFramebuffer::height(const size_t eyeIndex) const
{
	ocean_assert(eyeIndex < numberEyes_ && eyeIndex < glesFramebuffers_.size());

	if (eyeIndex < glesFramebuffers_.size())
	{
		return glesFramebuffers_[eyeIndex].height();
	}

	ocean_assert(false && "Invalid eye index!");
	return 0u;
}

inline XrSwapchain GLESWindowFramebuffer::xrSwapchain(const size_t eyeIndex) const
{
	ocean_assert(eyeIndex < numberEyes_ && eyeIndex < glesFramebuffers_.size());

	if (eyeIndex < glesFramebuffers_.size())
	{
		return glesFramebuffers_[eyeIndex].xrSwapchain();
	}

	ocean_assert(false && "Invalid eye index!");
	return XR_NULL_HANDLE;
}

inline size_t GLESWindowFramebuffer::textureSwapChainIndex(const size_t eyeIndex) const
{
	ocean_assert(eyeIndex < numberEyes_ && eyeIndex < glesFramebuffers_.size());

	if (eyeIndex < glesFramebuffers_.size())
	{
		return glesFramebuffers_[eyeIndex].textureSwapChainIndex();
	}

	ocean_assert(false && "Invalid eye index!");
	return 0;
}

}

}

}

}

}

#endif // OCEAN_RENDERING_GLES_QUEST_PLATFORM_OPENXR

#endif // META_OCEAN_RENDERING_GLES_QUEST_OPENXR_GLES_WINDOW_FRAMEBUFFER_H
