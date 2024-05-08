/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_MANAGER_H
#define META_OCEAN_RENDERING_MANAGER_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Engine.h"

#include "ocean/base/Singleton.h"

#include <map>
#include <vector>

namespace Ocean
{

namespace Rendering
{

/**
 * This class manages all scene graphs.<br>
 * Use this manager to receive a specific render engine.<br>
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Manager : public Singleton<Manager>
{
	friend class Engine;
	friend class Singleton<Manager>;

	public:

		/**
		 * Definition of a vector holding engine names.
		 */
		typedef std::vector<std::string> EngineNames;

	private:

		/**
		 * Class holding engine informations.
		 */
		class EngineInformation
		{
			public:

				/**
				 * Creates an information object.
				 */
				EngineInformation() = default;

				/**
				 * Creates a new engine information object.
				 * @param name The name of the engine object.
				 * @param callback Callback function creating the engine object
				 * @param graphicAPI Graphic API supported by the engine
				 */
				EngineInformation(const std::string& name, const Engine::CreateCallback& callback, const Engine::GraphicAPI graphicAPI);

				/**
				 * Returns the engine of this information object.
				 * @param preferredGraphicAPI Graphic API preferred for rendering
				 * @return Exsiting engine object or new created engine object
				 */
				EngineRef engine(const Engine::GraphicAPI preferredGraphicAPI);

				/**
				 * Returns the name of this information object.
				 * @return Engine name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the graphic API supported by the engine of this information object.
				 * @return Supported graphic API
				 */
				inline Engine::GraphicAPI graphicAPI() const;

				/**
				 * Releases the stored engine object.
				 */
				void release();

				/**
				 * Returns whether no internal engine object is stored.
				 * @return True, if so
				 */
				bool isNull() const;

			protected:

				/// Engine name.
				std::string name_;

				/// Engine creation callback function.
				Engine::CreateCallback createCallback_;

				/// Supported graphic API.
				Engine::GraphicAPI graphicAPI_ = Engine::API_DEFAULT;

				/// Engine object.
				EngineRef engine_;
		};

		/**
		 * Definition of a map mapping rendering engine priorities to engine information objects.
		 */
		typedef std::multimap<unsigned int, EngineInformation> EngineMap;

	public:

		/**
		 * Returns a render engine of a specific scene graph.
		 * If no engine name is specified one of the registered engines is returned.
		 * @param engine Name of the engine to return
		 * @param graphicAPI Preferred graphic API used for rendering, however there is no guarantee for this API
		 * @return Specified render engine
		 */
		EngineRef engine(const std::string& engine = std::string(), const Engine::GraphicAPI graphicAPI = Engine::API_DEFAULT);

		/**
		 * Returns all names of registered engines.
		 * @return Engine names
		 */
		EngineNames engines();

		/**
		 * Returns the graphic API supported by a specified engine.
		 * @param engine Name of the engine to return the graphic API for
		 * @return Supported graphic API, default api if the engine is unknown
		 */
		Engine::GraphicAPI supportedGraphicAPI(const std::string& engine);

		/**
		 * Releases all registered rendering engines.
		 */
		void release();

	private:

		/**
		 * Destructs the manager.
		 */
		virtual ~Manager();

		/**
		 * Registers a new render engine.
		 * @param engineName Name of the render engine
		 * @param callback Callback function which will create the engine object requested
		 * @param graphicAPI Graphic API supported by the engine
		 * @param priority Priority of this engine, if a default engine is requested the engine with higher priority will be retuned
		 */
		void registerEngine(const std::string& engineName, const Engine::CreateCallback& callback, const Engine::GraphicAPI graphicAPI, const unsigned int priority);

		/**
		 * Unregisters a render engine.
		 * @param engine Name of the render engine to unregister
		 * @return True, if succeeded
		 */
		bool unregisterEngine(const std::string& engine);

		/**
		 * Callback function to inform this manager that an engine is not in use anymore.
		 * @param engine Engine which is out of use
		 */
		void onRemoveEngine(const Engine* engine);

	private:

		/// Registered engines.
		EngineMap engineMap_;

		/// Manager lock.
		Lock lock_;
};

inline const std::string& Manager::EngineInformation::name() const
{
	return name_;
}

inline Engine::GraphicAPI Manager::EngineInformation::graphicAPI() const
{
	return graphicAPI_;
}

}

}

#endif // META_OCEAN_RENDERING_MANAGER_H
