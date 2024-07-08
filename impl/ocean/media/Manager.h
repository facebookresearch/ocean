/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MANAGER_H
#define META_OCEAN_MEDIA_MANAGER_H

#include "ocean/media/Media.h"
#include "ocean/media/Library.h"
#include "ocean/media/MediumRef.h"
#include "ocean/media/Recorder.h"

#include "ocean/base/Singleton.h"

#include <vector>

namespace Ocean
{

namespace Media
{

/**
 * This class is the manager for all media objects.
 * As media objects cannot be created directly this manager is necessary to create individual media object.<br>
 * Further, this manager encapsulates individual media libraries and allows to create media objects from specific or random libraries during one unique interface.<br>
 * @see Library, Medium.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Manager : public Singleton<Manager>
{
	friend class Singleton<Manager>;
	friend class Library;

	public:

		/**
		 * Definition of a vector holding library names.
		 */
		typedef std::vector<std::string> Names;

	private:

		/**
		 * Definition of a pair combining a library with a reference counter.
		 */
		typedef std::pair<LibraryRef, unsigned int> LibraryCounterPair;

		/**
		 * Definition of a vector holding library pairs.
		 */
		typedef std::vector<LibraryCounterPair> Libraries;

	public:

		/**
		 * Creates a new medium by a given url.
		 * If no medium can be created an empty reference is returned.
		 * @param url Url of the medium
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference to the new medium
		 */
		MediumRef newMedium(const std::string& url, bool useExclusive = false);

		/**
		 * Creates a new medium by a given url and an expected type.
		 * If no medium can be created an empty reference is returned.
		 * @param url Url of the medium
		 * @param type Type of the expected medium
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference to the new medium
		 */
		MediumRef newMedium(const std::string& url, const Medium::Type type, bool useExclusive = false);

		/**
		 * Creates a new medium by a given url, a library name and an expected type.
		 * If no medium can be created an empty reference is returned.
		 * @param url Url of the medium
		 * @param library Name of the library to use for this medium
		 * @param type Type of the expected medium
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference to the new medium
		 */
		MediumRef newMedium(const std::string& url, const std::string& library, const Medium::Type type, bool useExclusive = false);

		/**
		 * Creates a new recorder specified by the recorder type.
		 * @param type Type of the recorder to return
		 * @param library The optional name of the library to be used, empty to use any library
		 * @return Reference to the new recorder
		 */
		RecorderRef newRecorder(const Recorder::Type type, const std::string& library = std::string());

		/**
		 * Returns a list of selectable mediums.
		 * @return Selectable mediums
		 */
		Library::Definitions selectableMedia() const;

		/**
		 * Returns a list of specific selectable mediums.
		 * @param type Type of the selectable mediums
		 * @return Selectable mediums
		 */
		Library::Definitions selectableMedia(const Medium::Type type) const;

		/**
		 * Returns the names currently registered media libraries.
		 * @return Registered media libraries
		 */
		Names libraries() const;

		/**
		 * Releases all registered libraries.
		 * This function should be called once before the application is shutting down.
		 * However this function should be called after all medium reference have been released.
		 */
		void release();

		/**
		 * Creates a new manager object.
		 */
		Manager();

		/**
		 * Destructs the manager, called by the singleton object.
		 * Beware: The release function should be called before the singleton invokes the destructor.
		 * In common cases the singleton is released to late for internal library objects.
		 */
		virtual ~Manager();

		/**
		 * Registers a new library.
		 * With each register call, the reference counter for a specific library will be incremented.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @param name The name of the library to register, must be valid
		 * @return True, if the library has not been registered before
		 * @tparam T The data type of the library to register
		 * @see unregisterLibrary().
		 */
		template <typename T>
		bool registerLibrary(const std::string& name);

		/**
		 * Unregisters a library.
		 * With each unregister call, the reference counter for a specific library will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @param name The name of the library to unregister, must be valid
		 * @return True, if the library was actually removed from the system (as the reference counter reached zero); False, if the library is still used by someone else
		 * @see registerLibrary().
		 */
		bool unregisterLibrary(const std::string& name);

	protected:

		/// Registered libraries.
		Libraries libraries_;

		/// Lock for the libraries.
		mutable Lock lock_;
};

template <typename T>
bool Manager::registerLibrary(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	// first we check whether the library has been registered already

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->name() == name)
		{
			++i->second;
			return false;
		}
	}

	// the library has not been registered before, so we insert the library based on the priority

	LibraryRef newLibrary = T::create();

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->priority() < newLibrary->priority())
		{
			libraries_.insert(i, std::make_pair(std::move(newLibrary), 1u));
			return true;
		}
	}

	// the library goes to the end

	libraries_.emplace_back(std::make_pair(std::move(newLibrary), 1u));
	return true;
}

}

}

#endif
