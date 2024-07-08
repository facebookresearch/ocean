/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_ENGINE_H
#define META_OCEAN_RENDERING_ENGINE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Factory.h"
#include "ocean/rendering/WindowFramebuffer.h"

#include "ocean/base/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Engine;

/**
 * Definition of an engine reference object.
 * @ingroup rendering
 */
typedef Ocean::ObjectRef<Engine> EngineRef;

/**
 * This class is the base class for all rendering engines like.<br>
 * The main task of this class is to provide a framebuffer and a factory to create and render geometry nodes.<br>
 * The factory is able to create all supported rendering nodes and objects.<br>
 * The framebuffer holds the scene, the view and the resulting frame.<br>
 * Beware: Do not use the derivated instances of this object.<br>
 *
 * The manager holds and manages all engine instances for the registered rendering plugins like e.g. Nvidia SceniX or the GLESceneGraph.<br>
 * Therefore, receive an engine instance for a specific render engine from the manager only.<br>
 * Afterwards, the engine must be initialized before usage.<br>
 * @see Factory, Framebuffer, Manager
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Engine
{
	friend class DynamicObject;
	friend class Manager;
	friend class Ocean::ObjectRef<Engine>;

	public:

		/**
		 * Definition of different graphic APIs.
		 */
		enum GraphicAPI : uint32_t
		{
			/// Invalid graphic api id.
			API_DEFAULT = 0u,
			/// OpenGL graphic api id.
			API_OPENGL = 1u,
			/// OpenGL ES graphic api id.
			API_OPENGLES = 2u,
			/// DirectX graphic api id.
			API_DIRECTX = 4u,
			/// Raytracer graphic api id.
			API_RAYTRACER = 8u
		};

		/**
		 * Definition of a callback function used to create a registered engine.
		 */
		typedef Callback<Engine*, const GraphicAPI> CreateCallback;

		/**
		 * Definition of a vector holding framebuffer references.
		 */
		typedef std::vector<FramebufferRef> Framebuffers;

	protected:

		/**
		 * Definition of a vector holding framebuffer ids.
		 */
		typedef std::vector<ObjectId> ObjectIds;

	public:

		/**
		 * Returns the factory of this render engine.
		 * @return Factory of render engine
		 */
		virtual const Factory& factory() const = 0;

		/**
		 * Returns all created and valid framebuffer.
		 * @return Framebuffer references
		 */
		Framebuffers framebuffers() const;

		/**
		 * Returns a new framebuffer of this render engine.
		 * This function return the framebuffer created by the internal create framebuffer function.
		 * @param type Type of the framebuffer to be created
		 * @param config The configuration of the framebuffer to create
		 * @see Framebuffer, internalCreateFramebuffer().
		 * @return New framebuffer of this render engine
		 */
		FramebufferRef createFramebuffer(const Framebuffer::FramebufferType type = Framebuffer::FRAMEBUFFER_WINDOW, const Framebuffer::FramebufferConfig& config = {});

		/**
		 * Returns the name of this engine.
		 * @return Engine name
		 */
		virtual const std::string& engineName() const = 0;

		/**
		 * Returns the selected graphic API.
		 * @return Graphic API
		 */
		inline GraphicAPI graphicAPI() const;

		/**
		 * Returns the graphic APIs supported by this engine.
		 * @return Supported graphic APIs.
		 */
		inline GraphicAPI supportedAPIs() const;

		/**
		 * Updates dynamic objects in the engine.
		 * @param timestamp The current timestamp to be used to update the dynamic objects, e.g., to control animations or video textures, must be valid
		 */
		virtual void update(const Timestamp timestamp);

		/**
		 * Returns an object by a given object id belonging to this engine.
		 * If the object does not exist an empty reference is returned.
		 * @param objectId Id of the object to return
		 * @return Object reference of the requested object
		 * @see ObjectRefManager::object().
		 */
		ObjectRef object(const ObjectId objectId) const;

		/**
		 * Returns the first object having a specified name and belonging to this engine.
		 * If the object does not exist an empty reference is returned.
		 * @param name The name of the object to return
		 * @return Object reference of the requested object
		 * @see ObjectRefManager::object().
		 */
		ObjectRef object(const std::string& name) const;

		/**
		 * Returns all objects having a specified name and belonging to this engine.
		 * @param name The name of the objects to return
		 * @return Specified objects
		 * @see ObjectRefManager::objects().
		 */
		ObjectRefs objects(const std::string& name) const;

		/**
		 * Returns the extensions supported by the first created framebuffer.
		 * @return Supported extensions separated by a blank
		 */
		virtual std::string extensions() const;

		/**
		 * Returns whether the first framebuffer created by this engine supports a specific extension.
		 * @param extension Extension to check
		 * @return True, if the extension is supported
		 */
		virtual bool hasExtension(const std::string& extension) const;

		/**
		 * Returns the recent timestamp of the engine.
		 * The engine's timestamp controls e.g., animations or video textures.
		 * @return The engine's recent timestamp
		 */
		inline Timestamp timestamp() const;

		/**
		 * Retruns the lock object of this engine.
		 * Beware: Use this lock with caution.
		 * @return Engine lock object
		 */
		inline Lock& lock() const;

		/**
		 * Returns the render lock of this engine.
		 * The lock should be locked whenever a frame rendering is invoked related to this engine.
		 * @return Rendering lock object
		 */
		inline Lock& renderLock();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param engine Object which would be copied
		 */
		Engine(const Engine& engine) = delete;

		/**
		 * Creates a new render engine.
		 * @param graphicAPI Supported graphic APIs
		 */
		Engine(const GraphicAPI graphicAPI);

		/**
		 * Destructs a render engine.
		 */
		virtual ~Engine();

		/**
		 * Registers a new dynamic object.
		 * @param object Dynamic object to register
		 */
		void registerDynamicObject(DynamicObject* object);

		/**
		 * Unregisters a dynamic object.
		 * @param object Dynamic object to unregister
		 */
		void unregisterDynamicObject(DynamicObject* object);

		/**
		 * Returns a framebuffer reference of a just created framebuffer object.
		 * @param type Type of the framebuffer to be created
		 * @param config The configuration of the framebuffer to create
		 * @return New framebuffer to create a reference from
		 */
		virtual Framebuffer* internalCreateFramebuffer(const Framebuffer::FramebufferType type, const Framebuffer::FramebufferConfig& config) = 0;

		/**
		 * Disabled copy operator.
		 * @param engine Object which would be copied
		 * @return Reference to this object
		 */
		Engine& operator=(const Engine& engine) = delete;

		/**
		 * Registers an engine at the manager.
		 * @param engineName Name of the engine to register
		 * @param callback Engine create callback to create an engine object on demand
		 * @param graphicAPI Graphic API supported by the given engine
		 * @param priority Priority of this engine, if a default engine is requested the engine with higher priority will be retuned
		 */
		static void registerEngine(const std::string& engineName, const CreateCallback& callback, const GraphicAPI graphicAPI, const unsigned int priority);

		/**
		 * Unregisters an engine at the manager.
		 * @param engine Name of the engine to unregister
		 * @return True, if succeeded
		 */
		static bool unregisterEngine(const std::string& engine);

	protected:

		/// Selected graphic API.
		GraphicAPI graphicAPI_;

		/// Supported graphic API.
		const GraphicAPI supportedGraphicAPIs_;

		/// Vector holding ids of all registered dynamic objects for this engine.
		ObjectIds dynamicObjects_;

		/// The recent timestamp of the engine, to control e.g., animations or video textures
		Timestamp timestamp_;

		/// Vector holding all ids of created framebuffers.
		ObjectIds framebufferIds_;

		/// Engine lock.
		mutable Lock lock_;

		/// Render lock.
		Lock renderLock_;

		/// Dynamic object lock.
		Lock objectLock_;
};

inline Engine::GraphicAPI Engine::graphicAPI() const
{
	return graphicAPI_;
}

Engine::GraphicAPI Engine::supportedAPIs() const
{
	return supportedGraphicAPIs_;
}

inline Timestamp Engine::timestamp() const
{
	const ScopedLock scopedLock(lock_);

	return timestamp_;
}

inline Lock& Engine::lock() const
{
	return lock_;
}

inline Lock& Engine::renderLock()
{
	return renderLock_;
}

}

}

#endif // META_OCEAN_RENDERING_ENGINE_H
