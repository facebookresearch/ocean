/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MEDIUM_H
#define META_OCEAN_MEDIA_MEDIUM_H

#include "ocean/media/Media.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Medium;

/**
 * This class implements a medium reference with an internal reference counter.
 * @see ObjectRef, Medium
 * @ingroup media
 */
typedef ObjectRef<Medium> MediumRef;

/**
 * This is the base class for all mediums.
 * A medium may provide visual or/and audio content.<br>
 * The medium's source may be live, a buffer or a file.<br>
 *
 * A new medium object cannot be created directly.<br>
 * The Manager provides powerful functions allowing to create medium object from individual libraries and with individual properties.<br>
 *
 * Mediums can be exclusive or non-exclusive:<br>
 * An non-exclusive medium can be used by several individual clients to save resources like memory or/and computational power.<br>
 * An exclusive medium should be used by one client only.<br>
 * The exclusiveness of a medium can be decided during the creation of a medium.<br>
 * @see Manager
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Medium
{
	friend class ObjectRef<Medium>;

	public:

		/**
		 * Definition of different medium types.
		 */
		enum Type : uint32_t
		{
			/// Simple medium.
			MEDIUM = 0u,
			/// Frame medium.
			FRAME_MEDIUM = 1u << 0u,
			/// Sound medium.
			SOUND_MEDIUM = 1u << 1u,
			/// Finite medium.
			FINITE_MEDIUM = 1u << 2u,
			/// Live medium.
			LIVE_MEDIUM = 1u << 3u,
			/// Config medium.
			CONFIG_MEDIUM = 1u << 4u,
			/// Audio medium.
			AUDIO = (1u << 5u) | FINITE_MEDIUM | SOUND_MEDIUM,
			/// Image medium.
			IMAGE = (1u << 6u) | FRAME_MEDIUM,
			/// Pixel image medium.
			PIXEL_IMAGE = (1u << 7u) | FRAME_MEDIUM,
			/// Pixel image medium.
			BUFFER_IMAGE = (1u << 8u) | FRAME_MEDIUM,
			/// Image sequence medium.
			IMAGE_SEQUENCE = (1u << 9u) | FRAME_MEDIUM | FINITE_MEDIUM,
			/// Live audio medium.
			LIVE_AUDIO = (1u << 10u) | LIVE_MEDIUM | SOUND_MEDIUM,
			/// Live video medium.
			LIVE_VIDEO = (1u << 11u) | LIVE_MEDIUM | FRAME_MEDIUM,
			/// Microphone medium.
			MICROPHONE = (1u << 12u) | LIVE_MEDIUM,
			/// Movie medium.
			MOVIE = (1u << 13u) | FINITE_MEDIUM | FRAME_MEDIUM | SOUND_MEDIUM,
		};

	public:

		/**
		 * Returns the url of the medium.
		 * @return Medium url
		 */
		inline const std::string& url() const;

		/**
		 * Returns whether the medium is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the name of the owner library.
		 * @return Library name
		 */
		inline const std::string& library() const;

		/**
		 * Returns the type of the medium.
		 * @return Medium type
		 */
		inline Type type() const;

		/**
		 * Returns whether a medium has a given type.
		 * @param type Type to check
		 * @return True, if so
		 */
		bool isType(const Type type) const;

		/**
		 * Returns whether the medium is started currently.
		 * @return True, if so
		 */
		virtual bool isStarted() const = 0;

		/**
		 * Returns whether this medium can be use exclusively.
		 * @return True, if so
		 */
		bool isExclusive() const;

		/**
		 * Starts the medium.
		 * @return True, if succeeded
		 */
		virtual bool start() = 0;

		/**
		 * Pauses the medium.
		 * @return True, if succeeded
		 */
		virtual bool pause() = 0;

		/**
		 * Stops the medium.
		 * @return True, if succeeded
		 */
		virtual bool stop() = 0;

		/**
		 * Returns the start timestamp.
		 * @return Timestamp the medium has been started
		 */
		virtual Timestamp startTimestamp() const = 0;

		/**
		 * Returns the pause timestamp.
		 * @return Timestamp the medium has been paused
		 */
		virtual Timestamp pauseTimestamp() const = 0;

		/**
		 * Returns the stop timestamp.
		 * @return Timestamp the medium has been stopped
		 */
		virtual Timestamp stopTimestamp() const = 0;

		/**
		 * Clones this medium and returns a new independent instance of this medium.
		 * Beware: Some medium objects like e.g. live video or live audio objects cannot be cloned.
		 * @return Cloned medium object
		 */
		virtual MediumRef clone() const;

		/**
		 * Converts a medium type to a unique string.
		 * @param type Type of the medium to convert
		 * @return Unique string
		 */
		static std::string convertType(const Type type);

		/**
		 * Converts a medium type string to a medium type.
		 * @param type String of the medium type
		 * @return Medium type
		 */
		static Type convertType(const std::string& type);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param medium Object which would be copied
		 */
		Medium(const Medium& medium) = delete;

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit Medium(const std::string& url);

		/**
		 * Destructs a medium.
		 */
		virtual ~Medium();

		/**
		 * Disabled copy operator.
		 * @param medium Object which would be copied
		 * @return Reference to this object
		 */
		Medium& operator=(const Medium& medium) = delete;

	protected:

		/// Url of the medium.
		std::string url_;

		/// Name of the owner library.
		std::string libraryName_;

		/// Determines whether the medium is valid.
		bool isValid_ = false;

		/// Type of the medium
		Type type_ = MEDIUM;

		/// Medium lock.
		mutable Lock lock_;
};

inline const std::string& Medium::url() const
{
	return url_;
}

inline bool Medium::isValid() const
{
	return isValid_;
}

inline const std::string& Medium::library() const
{
	return libraryName_;
}

inline Medium::Type Medium::type() const
{
	return type_;
}

}

}

#endif // META_OCEAN_MEDIA_MEDIUM_H
