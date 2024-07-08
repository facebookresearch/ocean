/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_DEVICE_ENUMERATOR_H
#define META_OCEAN_MEDIA_DS_DEVICE_ENUMERATOR_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSObject.h"

#include "ocean/base/Singleton.h"

DISABLE_WARNINGS_BEGIN
	#include <Dshow.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow device enumerator.
 * @ingroup mediads
 */
class DSDeviceEnumerator
{
	public:

		/**
		 * Definition of a vector holding object names.
		 */
		typedef std::vector<std::string> Names;

	protected:

		/**
		 * Struct holding parameter of enumerator objects.
		 */
		struct Object
		{
			/// DirectShow moniker.
			ScopedIMoniker moniker_;

			/// DirectShow friendly name.
			std::string friendlyName_;

			/// DirectShow device name.
			std::string deviceName_;

			/// DirectShow device description.
			std::string deviceDescription_;
		};

		/**
		 * Definition of a vector holding objects.
		 */
		typedef std::vector<Object> Objects;

	public:

		/**
		 * Creates a new device enumerator.
		 */
		DSDeviceEnumerator() = default;

		/**
		 * Creates a new device enumerator.
		 * @param identifier Class identifier
		 */
		explicit DSDeviceEnumerator(const GUID& identifier);

		/**
		 * Destructs a DSDeviceEnumerator.
		 */
		~DSDeviceEnumerator();

		/**
		 * Refreshes the enumerated objects.
		 * @return True, if succeeded
		 */
		bool refresh();

		/**
		 * Returns whether a given object exist.
		 * @param name Friendly name of the object
		 * @return True, if so
		 */
		bool exist(const std::string& name) const;

		/**
		 * Returns the moniker of a specified object.
		 * @param name Friendly name of the object
		 * @return Moniker, if any
		 */
		ScopedIMoniker moniker(const std::string& name) const;

		/**
		 * Returns a list of all enumerator objects.
		 * @return Names of enumerated objects
		 */
		Names names() const;

		/**
		 * Releases all enumerator objects.
		 */
		void release();

		/**
		 * Returns the class identifier of this enumerator.
		 * @return Class identifier
		 */
		const GUID& identifier() const;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		DSDeviceEnumerator(const DSDeviceEnumerator&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		DSDeviceEnumerator& operator=(const DSDeviceEnumerator&) = delete;

	protected:

		/// Class identifier.
		GUID classIdentifier_ = GUID_NULL;

		/// Enumerated objects.
		Objects objects_;
};

/**
 * This class holds several different DirectShow device enumerators.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSEnumerators : public Singleton<DSEnumerators>
{
	friend class Singleton<DSEnumerators>;

	protected:

		/**
		 * Vector holding different device enumerators.
		 */
		typedef std::vector<std::shared_ptr<DSDeviceEnumerator>> Enumerators;

	public:

		/**
		 * Returns a device enumerator for a special class.
		 * @param identifier Class identifier
		 * @return Device enumerator
		 */
		DSDeviceEnumerator& enumerator(const GUID& identifier);

		/**
		 * Releases all internal enumerators.
		 */
		void release();

	protected:

		/**
		 * Destructs all enumerators.
		 */
		~DSEnumerators();

	protected:

		/// Vector holding several enumerators.
		Enumerators enumerators_;

		/// Enumerator lock.
		Lock lock_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_DEVICE_ENUMERATOR_H
