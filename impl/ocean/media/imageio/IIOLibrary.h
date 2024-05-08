/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_LIBRARY_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_LIBRARY_H

#include "ocean/media/imageio/ImageIO.h"

#include "ocean/media/Library.h"
#include "ocean/media/Manager.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements the ImageIO library.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOLibrary : public Library
{
	friend class Media::Manager;
	friend class IIOBufferImage;
	friend class IIOImage;
	friend class IIOImageSequence;

	public:

		/**
		 * Creates a new medium by a given url.
		 * @see Library::newMedium().
		 */
		MediumRef newMedium(const std::string& url, bool useExclusive = false) override;

		/**
		 * Creates a new medium by a given url and an expected type.
		 * @see Library::newMedium().
		 */
		MediumRef newMedium(const std::string& url, const Medium::Type type, bool useExclusive = false) override;

		/**
		 * Creates a new recorder specified by the recorder type.
		 * @see Library::newRecorder().
		 */
		RecorderRef newRecorder(const Recorder::Type type) override;

		/**
		 * Returns the supported medium types.
		 * @see Library::supportedTypes().
		 */
		Medium::Type supportedTypes() const override;

		/**
		 * Returns the image types supported by this library.
		 * @return All image type currently supported, e.g., "jpg", "bmp"
		 */
		static std::set<std::string> supportedImageTypes();

		/**
		 * Registers this library at the global media manager.
		 * With each register call, the reference counter for a specific library will be incremented.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library has not been registered before
		 * @see Manager, unregisterLibrary()
		 */
		static bool registerLibrary();

		/**
		 * Unregisters this library at the global media manager.
		 * With each unregister call, the reference counter for a specific library will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library was actually removed from the system (as the reference counter reached zero); False, if the library is still used by someone else
		 * @see registerLibrary().
		 */
		static bool unregisterLibrary();

	protected:

		/**
		 * Creates a new IIOLibrary object.
		 */
		IIOLibrary();

		/**
		 * Destructs an IIOLibrary object.
		 */
		~IIOLibrary() override;

		/**
		 * Creates this library and returns it an object reference.
		 * @return The new library object
		 */
		static LibraryRef create();

		/**
		 * Creates a new buffer image medium.
		 * @param url Url of the image medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		static MediumRef newBufferImage(const std::string& url, bool useExclusive);

		/**
		 * Creates a new image medium.
		 * @param url Url of the image medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		static MediumRef newImage(const std::string& url, bool useExclusive);

		/**
		 * Creates a new image sequence medium.
		 * @param url Url of the image sequence medium to create
		 * @param useExclusive Determines whether the caller would like to use this medium exclusively
		 * @return Reference of the new medium
		 */
		static MediumRef newImageSequence(const std::string& url, bool useExclusive);
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_LIBRARY_H
