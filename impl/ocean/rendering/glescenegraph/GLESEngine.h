/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ENGINE_H
#define META_OCEAN_RENDERING_GLES_ENGINE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESFactory.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class is the base class for all GLESceneGraph engines.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESEngine : public Engine
{
	public:

		/**
		 * Unregisters this engine at the global engine manager.
		 * Do not unregister this engine if using it as plugin, because it's done by the plugin itself when the plugin is unloaded.<br>
		 * Beware: This registration must not be done more than once and must not be done without previous registration!<br>
		 * @return True, if succeeded
		 */
		static bool unregisterEngine();

	protected:

		/**
		 * Creates a new rendering engine.
		 * @param graphicAPI Preferred graphic API to use by the engine
		 */
		GLESEngine(const GraphicAPI graphicAPI);

		/**
		 * Destructs a rendering engine.
		 */
		~GLESEngine() override;

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
		 * Returns the engine's graphic API.
		 * @return The graphic API
		 */
		static constexpr GraphicAPI glesGraphicAPI();

	private:

		/// GLESceneGraph object factory.
		GLESFactory factory_;

		/// Name of this engine.
		std::string localName_;
};

constexpr GLESEngine::GraphicAPI GLESEngine::glesGraphicAPI()
{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE

	// on mobile platforms, we use OpenGLES
	return API_OPENGLES;

#else

	// on desktop platforms, we use OpenGL
	return API_OPENGL;

#endif
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ENGINE_H
