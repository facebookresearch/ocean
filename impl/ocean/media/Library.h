/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_LIBRARY_H
#define META_OCEAN_MEDIA_LIBRARY_H

#include "ocean/media/Media.h"
#include "ocean/media/MediumRef.h"
#include "ocean/media/Recorder.h"

#include <set>
#include <string>

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Library;

/**
 * Definition of a object reference holding a library object.
 * @see ObjectRef, Library.
 * @ingroup media
 */
typedef ObjectRef<Library> LibraryRef;

/**
 * This class is the base class for all media libraries.
 * Any derived library providing individual types of media objects must implement a library class which must be registered at the Manager object of the base media library.<br>
 * A derived library may implement only one type of media object, all types of media objects or a subset of the media objects.<br>
 * @see Manager.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Library
{
	friend class Manager;
	friend class ObjectRef<Library>;

	public:

		/**
		 * Struct combining the url, type and library of a medium.
		 */
		struct Definition
		{
			/// Url of the medium.
			std::string url;

			/// Type of the medium.
			Medium::Type type;

			/// Name of the library.
			std::string library;
		};

		/**
		 * Definition of a vector holding medium definition objects.
		 */
		typedef std::vector<Definition> Definitions;

	protected:

		/**
		 * Definition of a set holding file extensions.
		 */
		typedef std::set<std::string> ExtensionSet;

	public:

		/**
		 * Returns the name of the library.
		 * @return Library name
		 */
		inline const std::string& name() const;

		/**
		 * Creates a new medium by a given url.
		 * @param url Url to create the medium from
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference to the medium
		 */
		virtual MediumRef newMedium(const std::string& url, bool useExclusive = false) = 0;

		/**
		 * Creates a new medium by a given url and an expected type.
		 * If no medium can be created an empty reference is returned.
		 * @param url Url of the medium
		 * @param type Type of the expected medium
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference to the new medium
		 */
		virtual MediumRef newMedium(const std::string& url, const Medium::Type type, bool useExclusive = false) = 0;

		/**
		 * Creates a new recorder specified by the recorder type.
		 * @param type Type of the recorder to return
		 * @return Reference to the new recorder
		 */
		virtual RecorderRef newRecorder(const Recorder::Type type) = 0;

		/**
		 * Returns a list of selectable mediums.
		 * @return Selectable mediums
		 */
		virtual Definitions selectableMedia() const;

		/**
		 * Returns a list of specific selectable mediums.
		 * @param type Type of the selectable mediums
		 * @return Selectable mediums
		 */
		virtual Definitions selectableMedia(const Medium::Type type) const;

		/**
		 * Returns whether the library supports a given medium type.
		 * @param type Medium type to check
		 * @return True, if so
		 */
		bool supports(const Medium::Type type) const;

		/**
		 * Returns the supported medium types.
		 * @return Supported medium types
		 */
		virtual Medium::Type supportedTypes() const;

		/**
		 * Returns whether a given file extension is not supported by this library.
		 * @param extension File extension to check with lower case
		 * @return True, if so
		 */
		bool notSupported(const std::string& extension) const;

		/**
		 * Converts a medium definition to one string.
		 * @param definition Medium definition to convert
		 * @return String of the definition
		 */
		static std::string convertDefinition(const Definition& definition);

		/**
		 * Converts a string to a medium definition.
		 * @param string String of the medium definition
		 * @return Medium definition
		 */
		static Definition convertDefinition(const std::string& string);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param library Object which would be copied
		 */
		Library(const Library& library) = delete;

		/**
		 * Creates a new Library object.
		 * @param name The name of the library
		 * @param priority Priority of this library, libraries with higher priority will be privileged compared to libraries with lower priority
		 */
		Library(const std::string& name, const unsigned int priority);

		/**
		 * Destructs a library.
		 */
		virtual ~Library();

		/**
		 * Returns the priority of this library.
		 * @return Library priority
		 */
		inline unsigned int priority() const;

		/**
		 * Registers file extensions not supported by this library.
		 * @param extension Not supported file extension
		 */
		void registerNotSupportedExtension(const std::string& extension);

		/**
		 * Disabled copy operator.
		 * @param library Object which would be copied
		 * @return Reference to this object
		 */
		Library& operator=(const Library& library) = delete;

	protected:

		/// The name of the library.
		std::string libraryName;

		/// Priority of this library, the higher the value, the higher the priority.
		unsigned int libraryPriority;

		/// Library lock.
		mutable Lock lock;

	private:

		/// Set holding file extensions which are not supported by this library.
		ExtensionSet notSupportedExtensionSet;
};

inline const std::string& Library::name() const
{
	return libraryName;
}

inline unsigned int Library::priority() const
{
	return libraryPriority;
}

}

}

#endif // META_OCEAN_MEDIA_LIBRARY_H
