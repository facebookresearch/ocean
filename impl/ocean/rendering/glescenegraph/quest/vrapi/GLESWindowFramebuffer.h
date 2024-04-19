// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_RENDERING_GLES_QUEST_VRAPI_GLES_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GLES_QUEST_VRAPI_GLES_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTraverser.h"

#ifdef OCEAN_RENDERING_GLES_QUEST_PLATFORM_VRAPI

#include "ocean/platform/gles/EGLContext.h"

#include "metaonly/ocean/platform/meta/quest/vrapi/GLESFramebuffer.h"

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

namespace VrApi
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

		/**
		 * Initializes the framebuffer based on the platform specific id.
		 * For Quest platforms, the id is the pointer to the ovrJava object.
		 * @see WindowFramebuffer::initializeById().
		 */
		bool initializeById(const size_t id, const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

		/**
		 * Returns the EGL context of this framebuffer.
		 */
		inline const Platform::GLES::EGLContext& eglContext() const;

		/**
		 * Returns the VrAPI's swap chain for the individual eyes/framebuffers.
		 * @param eyeIndex The index of the eye for which the swap chain will be returned, with range [0, VRAPI_FRAME_LAYER_EYE_MAX - 1]
		 * @return The requested swap chain
		 */
		inline ovrTextureSwapChain* colorTextureSwapChain(const size_t eyeIndex) const;

		/**
		 * Returns the index of the current texture of VrAPI's swap chain for the individual eyes/framebuffers.
		 * @param eyeIndex The index of the eye for which the index will be returned, with range [0, VRAPI_FRAME_LAYER_EYE_MAX - 1]
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

		/// The EGL context of this framebuffer.
		Platform::GLES::EGLContext eglContext_;

		/// The actual implementation of the Quest specific framebuffer(s).
		Platform::Meta::Quest::VrApi::GLESFramebuffers glesFramebuffers_;

		/// The framebuffer's id which is a pointer to the ovrJava object.
		size_t id_ = 0;

		/// The configuration to be used.
		Framebuffer::FramebufferConfig config_;
};

inline const Platform::GLES::EGLContext& GLESWindowFramebuffer::eglContext() const
{
	return eglContext_;
}

inline ovrTextureSwapChain* GLESWindowFramebuffer::colorTextureSwapChain(const size_t eyeIndex) const
{
	ocean_assert(eyeIndex < VRAPI_FRAME_LAYER_EYE_MAX);

	if (eyeIndex < glesFramebuffers_.size())
	{
		return glesFramebuffers_[eyeIndex].colorTextureSwapChain();
	}

	ocean_assert(false && "Invalid eye index!");
	return nullptr;
}

inline size_t GLESWindowFramebuffer::textureSwapChainIndex(const size_t eyeIndex) const
{
	ocean_assert(eyeIndex < VRAPI_FRAME_LAYER_EYE_MAX);

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

#endif // OCEAN_RENDERING_GLES_QUEST_PLATFORM_VRAPI

#endif // META_OCEAN_RENDERING_GLES_QUEST_VRAPI_GLES_WINDOW_FRAMEBUFFER_H
