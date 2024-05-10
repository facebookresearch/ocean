/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_OBJECT_REF_H
#define META_OCEAN_RENDERING_OBJECT_REF_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/SmartObjectRef.h"

#include <map>
#include <vector>

namespace Ocean
{

namespace Rendering
{

/**
 * This class implements a smart rendering object reference.
 * @tparam T Type of the derived object that has to be encapsulated by the smart reference object
 * @ingroup rendering
 */
template <typename T>
class SmartObjectRef : public Ocean::SmartObjectRef<T, Object>
{
	public:

		/**
		 * Creates an empty smart object reference.
		 */
		SmartObjectRef();

		/**
		 * Creates a new smart object reference by a given object reference.
		 * @param objectRef Object reference to copy
		 */
		SmartObjectRef(const Rendering::ObjectRef& objectRef);

		/**
		 * Copies a smart object reference.
		 * @param reference Reference to copy
		 */
		template <typename T2> SmartObjectRef(const Rendering::SmartObjectRef<T2>& reference);
};

/**
 * This class implements a rendering object reference manager.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT ObjectRefManager : public Singleton<ObjectRefManager>
{
	friend class Singleton<ObjectRefManager>;
	friend class Ocean::ObjectRef<Object>;
	friend class Engine;
	friend class Factory;
	friend class Object;

	protected:

		/**
		 * Definition of a multimap mapping object names to object ids.
		 */
		typedef std::unordered_multimap<std::string, ObjectId> NameMap;

		/**
		 * Definition of a map mapping object ids to object references.
		 */
		typedef std::unordered_map<ObjectId, ObjectRef> ObjectMap;

	public:

		/**
		 * Tests whether this manager holds still some objects of a specific rendering engine.
		 * @param engine Name of the engine to test
		 * @param writeObjectToLog True, to write all engine objects to the log
		 * @return True, if so
		 */
		bool hasEngineObject(const std::string& engine, const bool writeObjectToLog) const;

	protected:

		/**
		 * Destructs the manager.
		 */
		~ObjectRefManager();

		/**
		 * Returns an object by a given object id.
		 * If the object does not exist an empty reference is returned.
		 * @param objectId Id of the object to return
		 * @return Object reference of the requested object
		 * @see Engine::object().
		 */
		ObjectRef object(const ObjectId objectId) const;

		/**
		 * Returns the first object specified by a given name.
		 * If the object does not exist an empty reference is returned.
		 * @param name The name of the object
		 * @return Object reference of the requested object
		 * @see Engine::object().
		 */
		ObjectRef object(const std::string& name) const;

		/**
		 * Returns all objects specified by a given name.
		 * @param name The name of the objects to return
		 * @return Specified objects
		 * @see Engine::objects().
		 */
		ObjectRefs objects(const std::string& name) const;

		/**
		 * Registers a new object.
		 * @param object The object to manage, must be valid
		 * @return Object reference
		 */
		ObjectRef registerObject(Object* object);

		/**
		 * Changes the name of a registered object.
		 * @param objectId Id of the object to change
		 * @param oldName Old object name
		 * @param newName New object name
		 */
		void changeRegisteredObject(ObjectId objectId, const std::string& oldName, const std::string& newName);

		/**
		 * Unregisters an object.
		 */
		void unregisterObject(const Object* object);

	protected:

		/// Map holding all object references.
		ObjectMap objectMap_;

		/// Map mapping object names to object pointers.
		NameMap nameMap_;

		/// Lock for the object map
		mutable Lock lock_;
};

template <typename T>
SmartObjectRef<T>::SmartObjectRef() :
	Ocean::SmartObjectRef<T, Object>()
{
	// nothing to do here
}

template <typename T>
SmartObjectRef<T>::SmartObjectRef(const Rendering::ObjectRef& objectRef) :
	Ocean::SmartObjectRef<T, Object>(objectRef)
{
	// nothing to do here
}

template <typename T>
template <typename T2>
SmartObjectRef<T>::SmartObjectRef(const Rendering::SmartObjectRef<T2>& reference) :
	Ocean::SmartObjectRef<T, Object>(reference)
{
	// nothing to do here
}

}

}

#endif // META_OCEAN_RENDERING_OBJECT_REF_H
