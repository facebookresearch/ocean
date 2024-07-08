/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FRAME_MEDIUM_H
#define META_OCEAN_MEDIA_FRAME_MEDIUM_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameCollection.h"
#include "ocean/media/Medium.h"
#include "ocean/media/MediumRef.h"

#include "ocean/base/Frame.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

#include <functional>

namespace Ocean
{

namespace Media
{

// Forward declaration.
class FrameMedium;

/**
 * Definition of a smart medium reference holding a frame medium object.
 * @see SmartMediumRef, FrameMedium.
 * @ingroup media
 */
typedef SmartMediumRef<FrameMedium> FrameMediumRef;

/**
 * Definition of a vector holding frame medium reference objects.
 * @see FrameMediumRef.
 * @ingroup media
 */
typedef std::vector<FrameMediumRef> FrameMediumRefs;

/**
 * This is the base class for all frame mediums.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT FrameMedium : virtual public Medium
{
	public:

		/**
		 * Definition of a frame frequency defined in Hz.
		 */
		typedef double FrameFrequency;

		/**
		 * Definition of a media frame type composed by the frame dimension, pixel format, pixel origin and a frame frequency.
		 */
		class OCEAN_MEDIA_EXPORT MediaFrameType : public FrameType
		{
			public:

				/**
				 * Creates a new media frame type with invalid parameters.
				 */
				MediaFrameType() = default;

				/**
				 * Creates a new media frame type.
				 * @param frameType Frame type defining most parameters
				 * @param frequency Frame frequency in Hz
				 */
				inline MediaFrameType(const FrameType& frameType, const FrameFrequency frequency);

				/**
				 * Creates a new media frame type from a given media frame type with new width and height parameter.
				 * @param mediaFrameType The media frame type that is used as template for the new one
				 * @param width The width of the frame in pixel, with range [mediaFrameType.croppingLeft() + mediaFrameType.croppingWidth(), infinity)
				 * @param height The height of the frame in pixel, with range [mediaFrameType.croppingTop() + mediaFrameType.croppingHeight(), infinity)
				 */
				inline MediaFrameType(const MediaFrameType& mediaFrameType, const unsigned int width, const unsigned int height);

				/**
				 * Creates a new media frame type from a given media frame type with new pixel format.
				 * @param mediaFrameType The media frame type that is used as template for the new one
				 * @param pixelFormat The new pixel format
				 */
				inline MediaFrameType(const MediaFrameType& mediaFrameType, const PixelFormat pixelFormat);

				/**
				 * Creates a new media frame type from a given media frame type with new pixel format.
				 * @param mediaFrameType The media frame type that is used as template for the new one
				 * @param pixelFormat The new pixel format
				 * @param pixelOrigin The new pixel origin
				 */
				inline MediaFrameType(const MediaFrameType& mediaFrameType, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin);

				/**
				 * Creates a new frame type.
				 * @param width The width of the frame in pixel, with range [0, infinity)
				 * @param height The height of the frame in pixel, with range [0, infinity)
				 * @param pixelFormat Pixel format of the frame
				 * @param pixelOrigin Pixel origin of the frame
				 * @param frequency Frame frequency in Hz
				 */
				inline MediaFrameType(const unsigned int width, const unsigned int height, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin, const FrameFrequency frequency);

				/**
				 * Returns the frame frequency in Hz.
				 * @return Frame frequency in Hz
				 */
				inline FrameFrequency frequency() const;

				/**
				 * Returns the left start position of the cropping area.
				 * @return Left start position, in pixel
				 */
				inline unsigned int croppingLeft() const;

				/**
				 * Returns the top start position of the cropping area.
				 * @return Top start position, in pixel
				 */
				inline unsigned int croppingTop() const;

				/**
				 * Returns the width of the cropping area in pixel,
				 * @return Width of the cropping area, 0 if no cropping is defined
				 */
				inline unsigned int croppingWidth() const;

				/**
				 * Returns the height of the cropping area in pixel,
				 * @return Height of the cropping area, 0 if no cropping is defined
				 */
				inline unsigned int croppingHeight() const;

				/**
				 * Sets the frame frequency in Hz.
				 * @param frequency The frequency to set
				 */
				inline void setFrequency(const FrameFrequency frequency);

				/**
				 * Sets the optional cropping area of this media frame type.
				 * @param left The left cropping start position
				 * @param top The top cropping start position
				 * @param width The width of the cropping area, 0 if no cropping is defined
				 * @param height The height of the cropping area, 0 if no cropping is defined
				 */
				inline void setCroppingArea(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height);

			protected:

				/// Frame frequency.
				FrameFrequency frequency_ = FrameFrequency(0);

				/// Left cropping start position, in pixel
				unsigned int croppingLeft_ = 0u;

				/// Top cropping start position, in pixel
				unsigned int croppingTop_ = 0u;

				/// Width of the cropping area, in pixel, 0 if no cropping is defined
				unsigned int croppingWidth_ = 0u;

				/// Height of the cropping area, in pixel, 0 if no cropping is defined
				unsigned int croppingHeight_ = 0u;
		};

		/**
		 * Definition of a callback function for frame events.
		 * @param frame The event's frame, will be valid
		 * @param camera The camera profile associated with the frame, invalid if unknown
		 */
		typedef std::function<void(const Frame& frame, const SharedAnyCamera& camera)> FrameCallbackFunction;

		/**
		 * Definition of a subscription object for frame callback events.
		 */
		typedef ScopedSubscriptionHandler::ScopedSubscriptionType FrameCallbackScopedSubscription;

		/**
		 * This class implements a simpler receiver for frames which can be used with the frame callback functionality of FrameMedium.
		 * Below the code showing how to use the FrameReceiver.
		 * @code
		 * Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(...);
		 * ocean_assert(frameMedium);
		 *
		 * FrameReceiver frameReceiver;
		 *
		 * Media::FrameMedium::FrameCallbackScopedSubscription subscription = frameMedium->addFrameCallback(std::bind(&FrameReceiver::onFrame, &frameReceiver, std::placeholders::_1, std::placeholders::_2));
		 *
		 * frameMedium->start();
		 *
		 * // alternative 1: latestFrame() which does not make a copy of the frame's memory
		 * while (!frameMedium->taskFinished())
		 * {
		 *     Frame frame;
		 *     if (frameReceiver.latestFrame(frame))
		 *     {
		 *         // the frame will not own the memory
		 *         ocean_assert(!frame.isOwner());
		 *
		 *         frameReceiver.reset();
		 *     }
		 *
		 *     // the 'frame' most not be accessed anymore
		 * }
		 *
		 * // alternative 2: latestFrameAndReset() which does make a copy of the frame's memory
		 * while (!frameMedium->taskFinished())
		 * {
		 *     Frame frame;
		 *     if (frameReceiver.latestFrameAndReset(frame))
		 *     {
		 *         ocean_assert(frame.isOwner());
		 *     }
		 *
		 *     // we still can access the frame if latestFrameAndReset() returned 'true'
		 * }
		 * @endcode
		 * @see addFrameCallback().
		 */
		class OCEAN_MEDIA_EXPORT FrameReceiver final
		{
			public:

				/**
				 * Default constructor.
				 */
				FrameReceiver() = default;

				/**
				 * Resets and destructor this object.
				 */
				~FrameReceiver();

				/**
				 * Event function for a new movie frame.
				 * This function will not return until this object does not hold a latest frame anymore.
				 * @param frame The new movie frame, will be valid
				 * @param camera The camera profile associated with the frame, invalid if unknown
				 */
				void onFrame(const Frame& frame, const SharedAnyCamera& camera);

				/**
				 * Returns the latest frame and camera profile.
				 * Use this function to avoid making a copy of the original frame as the resulting frame does not own the memory.<br>
				 * The same latest frame will be provided until reset() is called.
				 * @param frame The resulting frame, not owning the memory
				 * @param camera Optional resulting camera
				 * @return True, if a latest frame exists
				 * @see reset(), latestFrameAndReset().
				 */
				bool latestFrame(Frame& frame, SharedAnyCamera* camera = nullptr);

				/**
				 * Returns the latest frame and camera profile and resets the receiver.
				 * This function provides a copy of the original frame so that the frame owns the memory.<br>
				 * The receive will be reset so that the object can receive a new frame.
				 * @param frame The resulting frame, not owning the memory
				 * @param camera Optional resulting camera
				 * @return True, if a latest frame exists
				 */
				bool latestFrameAndReset(Frame& frame, SharedAnyCamera* camera = nullptr);

				/**
				 * Resets the receiver so that a new frame can be received.
				 */
				void reset();

			protected:

				/// The most recent frame.
				Frame frame_;

				/// The camera profile associated with the most recent frame.
				SharedAnyCamera camera_;

				/// The object's lock.
				Lock lock_;
		};

	protected:

		/**
		 * Definition of a thread-safe subscription handler for frame callback functions.
		 */
		typedef ScopedSubscriptionCallbackHandlerT<FrameCallbackFunction, FrameMedium, true> FrameCallbackHandler;

	protected:

		/**
		 * Definition of a vector holding frame types.
		 */
		typedef std::vector<MediaFrameType> FrameTypes;

		/**
		 * Class allowing the sorting of several frame media types according their popularity.
		 */
		class OCEAN_MEDIA_EXPORT SortableFrameType
		{
			public:

				/**
				 * Creates a new sortable frame media type.
				 * @param frameType Preferable frame type
				 */
				SortableFrameType(const MediaFrameType& frameType);

				/**
				 * Creates a new sortable frame type.
				 * @param width Preferable reference width
				 * @param height Preferable reference height
				 * @param pixelFormat Preferable reference pixel format
				 * @param pixelOrigin Preferable reference pixel origin
				 * @param frequency Preferable reference frequency
				 */
				SortableFrameType(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const FrameFrequency frequency);

				/**
				 * Returns whether the left sortable media type is more preferred than the right one.
				 * @param right The right sortable media type
				 * @return True, if so
				 */
				bool operator<(const SortableFrameType& right) const;

			protected:

				/// Actual frame type.
				MediaFrameType actualFrameType_;

				/// Preferable frame type.
				MediaFrameType preferableFrameType_;
		};

	public:

		/**
		 * Destructs a frame object.
		 */
		~FrameMedium() override;

		/**
		 * Returns whether the object holds a frame.
		 * @return True, if so
		 */
		inline bool hasFrame() const;

		/**
		 * Returns the frequency of the frame in Hz.
		 * @return Frequency of the frame in Hz
		 */
		inline FrameFrequency frameFrequency() const;

		/**
		 * Returns the most recent frame.
		 * @param anyCamera Optional resulting camera profile of the frame, if known
		 * @return Most recent frame
		 */
		virtual FrameRef frame(SharedAnyCamera* anyCamera = nullptr) const;

		/**
		 * Returns the frame with a specific timestamp.
		 * If no frame exists with the given timestamp the most recent frame is returned.
		 * @param timestamp The timestamp of the frame to return
		 * @param anyCamera Optional resulting camera profile of the frame, if known
		 * @return Requested frame
		 * @see hasFrame().
		 */
		virtual FrameRef frame(const Timestamp timestamp, SharedAnyCamera* anyCamera = nullptr) const;

		/**
		 * Waits until the first frame is available.
		 * If the time to wait is exceeded and no frame is available, no frame is returned.<br>
		 * @param timeout Time to wait for the first frame, in seconds, with range [0, infinity)
		 * @param anyCamera Optional resulting camera profile of the frame, if known
		 * @return Frame if available
		 */
		virtual FrameRef frameTimeout(const double timeout, SharedAnyCamera* anyCamera = nullptr) const;

		/**
		 * Returns whether this media object currently holds a frame with specified timestamp.
		 * Beware: There is no guarantee that the frame will be available after this call due to multi-thread issues!
		 * @param timestamp The timestamp to be checked
		 * @return True, if so
		 */
		virtual bool hasFrame(const Timestamp timestamp) const;

		/**
		 * Returns the preferred width of the frame in pixel.
		 * @return Preferred frame width
		 * @see setPreferredFrameDiminsion(), preferredFrameHeight().
		 */
		inline unsigned int preferredFrameWidth() const;

		/**
		 * Returns the preferred height of the frame in pixel.
		 * @return Preferred frame height
		 * @see setPreferredFrameDiminsion(), preferredFrameWidth().
		 */
		inline unsigned int preferredFrameHeight() const;

		/**
		 * Returns the preferred frame pixel format.
		 * @return Preferred pixel format
		 * @see setPreferredFramePixelFormat().
		 */
		inline FrameType::PixelFormat preferredFramePixelFormat() const;

		/**
		 * Returns the preferred frame frequency in Hz.
		 * @return Preferred frame frequency in Hz
		 * @see setPreferredFrameFrequency().
		 */
		inline FrameFrequency preferredFrameFrequency() const;

		/**
		 * Returns the transformation between the camera and device.
		 * The device can be either a webcam, a mobile phone, or any other device delivering visual information.
		 * The transformation between camera and device represents the offset translation and orientation between the camera sensor and the device (e.g., the display of the device).<br>
		 * The device coordinate system has the origin in the center of the device (e.g., the display) and the Z-axis is pointing out towards the user.<br>
		 * The camera coordinate system has the origin in the center of the camera (center of projection) and the negative Z-axis is pointing towards the scene.
		 * <pre>
		 *          device top
		 * ...........................
		 * .                         .                          camera
		 * .         ^               .                   ---------------------------------
		 * .       Y |               .                  |               ^                 |
		 * .         |               .                  |             Y |                 |
		 * .         |               .                  |               |                 |
		 * .         O --------->    .                  |               |                 |
		 * .        /        X       .                  |               O --------->      |
		 * .       / Z               .                  |              /        X         |
		 * .      v                  .                  |             / Z                 |
		 * .                         .                  |            v                    |
		 * ...........................                   ---------------------------------
		 * .      home button        .
		 * ...........................
		 *      device bottom
		 * </pre>
		 */
		virtual HomogenousMatrixD4 device_T_camera() const;

		/**
		 * Sets the preferred frame dimension.
		 * Beware: There is no guarantee that this dimension can be provided.
		 * @param width Preferred frame width in pixel
		 * @param height Preferred frame height in pixel
		 * @return True, if the medium accepts this preferred values.
		 * @see preferredFrameWidth(), preferredFrameHeight().
		 */
		virtual bool setPreferredFrameDimension(const unsigned int width, const unsigned int height);

		/**
		 * Sets the preferred frame pixel format.
		 * Beware: There is no guarantee that this pixel format can be provided.
		 * @param format Preferred pixel format
		 * @return True, if the medium accepts this preferred value.
		 * @see preferredFramePixelFormat().
		 */
		virtual bool setPreferredFramePixelFormat(const FrameType::PixelFormat format);

		/**
		 * Sets the preferred frame frequency in Hz.
		 * Beware: There is no guarantee that this frequency can be provided.
		 * @param frequency Preferred frequency in Hz
		 * @return True, if the medium accepts this preferred value.
		 * @see preferredFrameFrequency().
		 */
		virtual bool setPreferredFrameFrequency(const FrameFrequency frequency);

		/**
		 * Returns the capacity of the frame history of this frame medium object.
		 * @return The capacity of frames, with range [1, infinity)
		 */
		size_t capacity() const;

		/**
		 * Sets or changes the capacity of the frame history of this frame medium object.
		 * @param capacity The capacity to set, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool setCapacity(const size_t capacity);

		/**
		 * Adds a callback function which is called whenever a new frame arrives.
		 * If at least one callback is registered, this frame medium will not store any frame in the internal frame store.
		 * @param frameCallbackFunction The callback function to be added, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 * @see frame()
		 */
		[[nodiscard]] FrameCallbackScopedSubscription addFrameCallback(FrameCallbackFunction&& frameCallbackFunction);

		/**
		 * Extracts most recent frames from several frame medium objects and ensures that the timestamps of all frames are identical.
		 * @param frameMediums The frame medium objects from which the frames will be extracted, at least one
		 * @param lastTimestamp The timestamp of the last extracted frames to accept frames with newer timestamp only, an invalid timestamp to accept any frame
		 * @param frames The resulting frames with identical timestamp, one for each frame medium object
		 * @param cameras The resulting camera profiles, one for each frame medium object
		 * @param waitTime The wait time in milliseconds this function will wait until it fails if not all frame medium objects can provide the expected frame
		 * @param timedOut Optional resulting True in case the function timed out when waiting for all synced frames; nullptr if not of interest
		 * @param device_T_cameras Optional resulting transformations between camera and device, one for each frame medium object; nullptr if not of interest
		 * @return True, if succeeded
		 */
		static bool syncedFrames(const FrameMediumRefs& frameMediums, const Timestamp lastTimestamp, FrameRefs& frames, SharedAnyCameras& cameras, const unsigned int waitTime = 2u, bool* timedOut = nullptr, HomogenousMatricesD4* device_T_cameras = nullptr);

	protected:

		/**
		 * Creates a new frame medium by a given url.
		 * @param url Url of the frame medium
		 */
		explicit FrameMedium(const std::string& url);

		/**
		 * Delivers a new frame to this FrameMedium.
		 * The new frame will either be stored in the medium's frame collection or forwarded to subscribed frame callback functions.
		 * @param frame The new frame to deliver, must be valid
		 * @param anyCamera The camera profile associated with the frame, invalid if unknown
		 * @return True, if succeeded
		 */
		virtual bool deliverNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera = SharedAnyCamera());

	protected:

		/// Frame collection storing several frames with different timestamps.
		FrameCollection frameCollection_;

		/// The handler for frame callback functions.
		FrameCallbackHandler frameCallbackHandler_;

		/// Frame frequency.
		FrameFrequency recentFrameFrequency_ = FrameFrequency(0);

		/// Preferred frame type of the medium.
		MediaFrameType preferredFrameType_;
};

inline FrameMedium::MediaFrameType::MediaFrameType(const FrameType& frameType, const FrameFrequency frequency) :
	FrameType(frameType),
	frequency_(frequency)
{
	// nothing to do here
}

inline FrameMedium::MediaFrameType::MediaFrameType(const MediaFrameType& mediaFrameType, const unsigned int width, const unsigned int height) :
	FrameType(mediaFrameType, width, height),
	frequency_(mediaFrameType.frequency_),
	croppingLeft_(mediaFrameType.croppingLeft_),
	croppingTop_(mediaFrameType.croppingTop_),
	croppingWidth_(mediaFrameType.croppingWidth_),
	croppingHeight_(mediaFrameType.croppingHeight_)
{
	ocean_assert(croppingLeft_ + croppingWidth_ <= width);
	ocean_assert(croppingTop_ + croppingHeight_ <= height);
}

inline FrameMedium::MediaFrameType::MediaFrameType(const MediaFrameType& mediaFrameType, const PixelFormat pixelFormat) :
	FrameType(mediaFrameType, pixelFormat),
	frequency_(mediaFrameType.frequency_),
	croppingLeft_(mediaFrameType.croppingLeft_),
	croppingTop_(mediaFrameType.croppingTop_),
	croppingWidth_(mediaFrameType.croppingWidth_),
	croppingHeight_(mediaFrameType.croppingHeight_)
{
	// nothing to do here
}

inline FrameMedium::MediaFrameType::MediaFrameType(const MediaFrameType& mediaFrameType, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin) :
	FrameType(mediaFrameType, pixelFormat, pixelOrigin),
	frequency_(mediaFrameType.frequency_),
	croppingLeft_(mediaFrameType.croppingLeft_),
	croppingTop_(mediaFrameType.croppingTop_),
	croppingWidth_(mediaFrameType.croppingWidth_),
	croppingHeight_(mediaFrameType.croppingHeight_)
{
	// nothing to do here
}

inline FrameMedium::MediaFrameType::MediaFrameType(const unsigned int width, const unsigned int height, const PixelFormat pixelFormat, const PixelOrigin pixelOrigin, const FrameFrequency frequency) :
	FrameType(width, height, pixelFormat, pixelOrigin),
	frequency_(frequency)
{
	// nothing to do here
}

inline FrameMedium::FrameFrequency FrameMedium::MediaFrameType::frequency() const
{
	return frequency_;
}

inline unsigned int FrameMedium::MediaFrameType::croppingLeft() const
{
	return croppingLeft_;
}

inline unsigned int FrameMedium::MediaFrameType::croppingTop() const
{
	return croppingTop_;
}

inline unsigned int FrameMedium::MediaFrameType::croppingWidth() const
{
	return croppingWidth_;
}

inline unsigned int FrameMedium::MediaFrameType::croppingHeight() const
{
	return croppingHeight_;
}

inline void FrameMedium::MediaFrameType::setFrequency(const FrameFrequency frequency)
{
	frequency_ = frequency;
}

inline void FrameMedium::MediaFrameType::setCroppingArea(const unsigned int left, const unsigned int top, const unsigned int width, const unsigned int height)
{
	croppingLeft_ = left;
	croppingTop_ = top;
	croppingWidth_ = width;
	croppingHeight_ = height;
}

inline bool FrameMedium::hasFrame() const
{
	return bool(frameCollection_.recent());
}

inline FrameMedium::FrameFrequency FrameMedium::frameFrequency() const
{
	return recentFrameFrequency_;
}

inline unsigned int FrameMedium::preferredFrameWidth() const
{
	return preferredFrameType_.width();
}

inline unsigned int FrameMedium::preferredFrameHeight() const
{
	return preferredFrameType_.height();
}

inline FrameType::PixelFormat FrameMedium::preferredFramePixelFormat() const
{
	return preferredFrameType_.pixelFormat();
}

inline FrameMedium::FrameFrequency FrameMedium::preferredFrameFrequency() const
{
	return preferredFrameType_.frequency();
}

}

}

#endif
