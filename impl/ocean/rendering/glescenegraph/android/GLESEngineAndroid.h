/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ANDROID_GLES_ENGINE_ANDROID_H
#define META_OCEAN_RENDERING_GLES_ANDROID_GLES_ENGINE_ANDROID_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESEngine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

namespace Android
{

/**
 * This class implements the engine for Android platforms.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESEngineAndroid : public GLESEngine
{
	public:

		/**
		 * Creates this engine and registeres it at the global engine manager.<br>
		 * Do not register this engine if using it as plugin, because it's done by the plugin itself.<br>
		 * However, if your not using the plugin mechanism you have to initialize this engine once at program initialization.<br>
		 * If the library is not used anymore unregister it using the unregister function.<br>
		 * Beware: This registration must not be done more than once!
		 * @see Manager, GLESEngine::unregisterEngine()
		 */
		static void registerEngine();

	protected:

		/**
		 * Creates a new rendering engine.
		 * @param graphicAPI Preferred graphic API to use by the engine
		 */
		GLESEngineAndroid(const GraphicAPI graphicAPI);

		/**
		 * Destructs a rendering engine.
		 */
		~GLESEngineAndroid() override = default;

		/**
		 * Returns a framebuffer reference of a just created framebuffer object.
		 * @see Engine::internalCreateFramebuffer().
		 */
		Framebuffer* internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config) override;

		/**
		 * Creates a new Nvidia SceniX rendering engine.
		 * @param graphicAPI Preferred graphic API to use by the engine
		 * @return New rendering engine
		 */
		static Engine* createEngine(const GraphicAPI graphicAPI);
};

}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ANDROID_GLES_ENGINE_ANDROID_H
