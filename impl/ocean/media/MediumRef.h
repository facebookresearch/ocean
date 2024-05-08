/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MEDIUM_REF_H
#define META_OCEAN_MEDIA_MEDIUM_REF_H

#include "ocean/media/Media.h"
#include "ocean/media/Medium.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/SmartObjectRef.h"

#include <map>

namespace Ocean
{

namespace Media
{

/**
 * This class implements a smart medium reference.
 * @tparam T Type of the derived object that has to be encapsulated by the smart reference object
 * @ingroup media
 */
template <typename T>
class SmartMediumRef : public SmartObjectRef<T, Medium>
{
	private:

		/**
		 * Redefinition of the release callback function defined in ObjectRef.
		 */
		typedef typename SmartObjectRef<T, Medium>::ReleaseCallback ReleaseCallback;

	public:

		/**
		 * Creates an empty smart medium reference.
		 */
		SmartMediumRef();

		/**
		 * Creates a new smart medium reference by a given medium reference.
		 * @param mediumRef Medium reference to copy
		 */
		SmartMediumRef(const MediumRef& mediumRef);

		/**
		 * Creates a new SmartMediumRef by a given object.
		 * This given object will be released by the smart medium reference itself.
		 * @param object Internal object
		 */
		explicit SmartMediumRef(Medium* object);

		/**
		 * Copies a smart medium reference.
		 * @param reference Reference to copy
		 */
		template <typename T2> SmartMediumRef(const SmartMediumRef<T2>& reference);

		/**
		 * Assigns a smart medium reference.
		 * @param mediumRef reference Reference to assign
		 * @return Reference to this object
		 */
		SmartMediumRef& operator=(const MediumRef& mediumRef);
};

/**
 * This class implements a medium reference manager.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT MediumRefManager : public Singleton<MediumRefManager>
{
	friend class Singleton<MediumRefManager>;
	friend class ObjectRef<Medium>;
	friend class Medium;
	friend class Manager;

	protected:

		/**
		 * Map mapping urls to medium references.
		 */
		typedef std::multimap<std::string, MediumRef> MediumMap;

	public:

		/**
		 * Registers a new medium.
		 * @param medium Medium object to manage
		 * @return Medium reference
		 */
		MediumRef registerMedium(Medium* medium);

		/**
		 * Returns a medium by a given url.
		 * If the medium does not exist an empty reference is returned.
		 * @param url Url of the new medium
		 * @param library Name of the owner library
		 * @param type Type of the expected medium
		 * @return Medium reference of the requested url
		 */
		MediumRef medium(const std::string& url, const std::string& library, const Medium::Type type);

	protected:

		/**
		 * Destructs the manager.
		 */
		~MediumRefManager();

		/**
		 * Returns a medium by a given url.
		 * If the medium does not exist an empty reference is returned.
		 * @param url Url of the medium that has to be returned
		 * @return Medium reference of the requested url
		 */
		MediumRef medium(const std::string& url);

		/**
		 * Returns a medium by a given url.
		 * If the medium does not exist an empty reference is returned.
		 * @param url Url of the requested medium
		 * @param type Type of the expected medium
		 * @return Medium reference of the requested url
		 */
		MediumRef medium(const std::string& url, const Medium::Type type);

		/**
		 * Returns whether a medium is registered at this manager.
		 * @param medium Medium to check
		 * @return True, if so
		 */
		bool isRegistered(const Medium* medium);

		/**
		 * Unregisters a medium.
		 * @param medium Medium to unregister
		 */
		void unregisterMedium(const Medium* medium);

	protected:

		/// Map holding all medium references.
		MediumMap mediumMap;

		/// Lock for the medium map
		Lock lock;
};

template <typename T>
SmartMediumRef<T>::SmartMediumRef() :
	SmartObjectRef<T, Medium>()
{
	// nothing to do here
}

template <typename T>
SmartMediumRef<T>::SmartMediumRef(const MediumRef& mediumRef) :
	SmartObjectRef<T, Medium>(mediumRef)
{
	// nothing to do here
}

template <typename T>
SmartMediumRef<T>::SmartMediumRef(Medium* object) :
	SmartObjectRef<T, Medium>(object, ReleaseCallback(MediumRefManager::get(), &MediumRefManager::unregisterMedium))
{
	// nothing to do here
}

template <typename T>
template <typename T2>
SmartMediumRef<T>::SmartMediumRef(const SmartMediumRef<T2>& reference) :
	SmartObjectRef<T, Medium>(reference)
{
	// nothing to do here
}

template <typename T>
SmartMediumRef<T>& SmartMediumRef<T>::operator=(const MediumRef& mediumRef)
{
	SmartObjectRef<T, Medium>::operator =(mediumRef);
	return *this;
}

}

}

#endif // META_OCEAN_MEDIA_MEDIUM_REF_H
