/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_ENGINE_H
#define META_OCEAN_RENDERING_GI_ENGINE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIFactory.h"

#include "ocean/rendering/Engine.h"

#include <map>

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * Global Illumination engine class.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIEngine : public Engine
{
	friend class GIFramebuffer;
	friend class GIWindowFramebuffer;
	friend class GIObject;

	public:

		/**
		 * Creates this engine and registeres it at the global engine manager.<br>
		 * Do not register this engine if using it as plugin, because it's done by the plugin itself.<br>
		 * However, if your not using the plugin mechanism you have to initialize this engine once at program initialization.<br>
		 * If the library is not used anymore unregister it using the unregister function.<br>
		 * Beware: This registration must not be done more than once!<br>
		 * @see Manager, unregisterEngine()
		 */
		static void registerEngine();

		/**
		 * Unregisters this engine at the global engine manager.
		 * Do not unregister this engine if using it as plugin, because it's done by the plugin itself when the plugin is unloaded.<br>
		 * Beware: This registration must not be done more than once and must not be done without previous registration!<br>
		 * @return True, if succeeded
		 */
		static bool unregisterEngine();

	private:

		/**
		 * Creates a new rendering engine.
		 * @param preferredGraphicAPI Preferred graphic API to use by the engine
		 */
		GIEngine(const GraphicAPI preferredGraphicAPI);

		/**
		 * Destructs a rendering engine.
		 */
		~GIEngine() override;

		/**
		 * Creates a new Global Illumination rendering engine.
		 * @param graphicAPI Preferred graphic API to use by the engine
		 * @return New rendering engine
		 */
		static Engine* createEngine(const GraphicAPI graphicAPI);

		/**
		 * Returns the factory of this rendering engine.
		 * @see Engine::factory().
		 */
		const Factory& factory() const override;

		/**
		 * Returns the specific type of this engine.
		 * @see Engine::engineId().
		 */
		const std::string& engineName() const override;

		/**
		 * Returns a new framebuffer of this render engine.
		 * @see Engine::internalCreateFramebuffer().
		 */
		Framebuffer* internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config) override;

	private:

		/// Global Illumiatino object factory.
		GIFactory engineFactory;

		/// Name of this engine.
		std::string engineLocalName;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_ENGINE_H
