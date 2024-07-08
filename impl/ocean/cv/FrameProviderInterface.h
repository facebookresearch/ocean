/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_PROVIDER_INTERFACE_H
#define META_OCEAN_CV_FRAME_PROVIDER_INTERFACE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"
#include "ocean/base/ObjectRef.h"

namespace Ocean
{

namespace CV
{

// Forward declaration.
class FrameProviderInterface;

/**
 * Definition of an object reference holding a frame provider interface.
 * @see FrameProviderInterface;
 * @ingroup cv
 */
typedef ObjectRef<FrameProviderInterface> FrameProviderInterfaceRef;

/**
 * This class defines an abstract interface allowing to request frames from any kind of frame provider.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameProviderInterface
{
	friend class ObjectRef<FrameProviderInterface>;

	public:

		/**
		 * Definition of a frame request callback function.
		 * A frame request callback is invoked by the interface whenever a requested frame arrives.
		 */
		typedef Callback<void, FrameRef, const unsigned int> FrameCallback;

		/**
		 * Definition of a frame number request callback function.
		 */
		typedef Callback<void, unsigned int> FrameNumberCallback;

		/**
		 * Definition of a frame type request callback function.
		 */
		typedef Callback<void, const FrameType&> FrameTypeCallback;

	protected:

		/**
		 * Definition of a list that stores frame request callbacks.
		 */
		typedef ConcurrentCallbacks<FrameCallback> FrameCallbacks;

		/**
		 * Definition of a list that stores frame number request callbacks.
		 */
		typedef ConcurrentCallbacks<FrameNumberCallback> FrameNumberCallbacks;

		/**
		 * Definition of a list that stores frame type request callbacks.
		 */
		typedef ConcurrentCallbacks<FrameTypeCallback> FrameTypeCallbacks;

	public:

		/**
		 * Returns whether the internal information of this interface has been initialized already and whether request functions can be handled.
		 * No request function of this interface should be invoked before this interface has been initialized successfully.<br>
		 * @return True, if so
		 * @see synchronInitializationRequest(), setPreferredFrameType().
		 */
		virtual bool isInitialized() = 0;

		/**
		 * Waits until this interface has been initialized.
		 * @param timeout The time this functions waits at most for the initialization, in seconds, with range [0, infinity)
		 * @param abort Optional abort statement allowing to abort the frame type request at any time; set the value True to abort the request
		 * @return True, if the interface is initialized
		 * @see isInitialized().
		 */
		virtual bool synchronInitializationRequest(const double timeout = 120.0, bool* abort = nullptr);

		/**
		 * Sets a preferred frame type pixel format and pixel origin for this interface.
		 * If a preferred frame type is set and the native frame type can be converted into the requested frame type, all frames of this interface will have the requested type.<br>
		 * However, there is no guarantee that this interface will be able to provided the requested frame type.<br>
		 * Beware: Ensure that this interface has been initialized before calling this function.<br>
		 * @param pixelFormat The preferred pixel format
		 * @param pixelOrigin The preferred pixel origin
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		virtual bool setPreferredFrameType(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin) = 0;

		/**
		 * Invokes an asynchronous frame request.
		 * The requested frame is identified by the index of the frame.<br>
		 * Even if not frame request callback has been registered, this function can be used to cache the requested frame inside the interface (this might speed up the synchronous frame request).<br>
		 * Beware: There is no guarantee that the requested frame will be delivered eventually, e.g. the requested frame might not exist.<br>
		 * @param index The index of the requested frame, should be in the range [0, synchronFrameNumberRequest())
		 * @param priority True, to request the frame with highest priority
		 * @see synchronFrameRequest().
		 */
		virtual void asynchronFrameRequest(const unsigned int index, const bool priority = false) = 0;

		/**
		 * Invokes a synchronous frame request.
		 * The requested frame is identified by the index of the frame.<br>
		 * This function returns an invalid frame if the requested frame cannot be delivered within a specified time frame or the abort statement has been set.<br>
		 * @param index The index of the requested frame, should be in the range [0, synchronFrameNumberRequest())
		 * @param timeout The time this functions waits at most for a requested frame, in seconds, with range [0, infinity)
		 * @param abort Optional abort statement allowing to abort the frame request at any time; set the value True to abort the request
		 * @return Resulting frame, if the frame can be delivered within the specified time frame
		 */
		virtual FrameRef synchronFrameRequest(const unsigned int index, const double timeout = 10.0, bool* abort = nullptr) = 0;

		/**
		 * Invokes a suggestion to pre-load or to cache some frames that might be requested soon.
		 * However, there is no guarantee that the requested frames will be pre-loaded.<br>
		 * Every provider is responsible to support the frame suggestions as best as possible regarding the overall system performance.<br>
		 * @param index The index of the next frame that is suggested, with range [0, asynchronFrameNumberRequest())
		 * @param range The positive or negative range of frames that are suggested; use a positive range to cache frame with increasing indices, use a negative range to cache frames with decreasing indices
		 */
		virtual void frameCacheRequest(const unsigned int index, const int range);

		/**
		 * Invokes an asynchronous frame number request.
		 * An already registered frame number request callback function is necessary so that the requested frame number can be delivered eventually.<br>
		 * Beware: There is no guarantee that the requested frame number will be delivered eventually.<br>
		 * @see synchronFrameNumberRequest().
		 */
		virtual void asynchronFrameNumberRequest() = 0;

		/**
		 * Invokes a synchronous frame number request.
		 * This function returns an invalid frame number if the requested frame number cannot be determined within a specified time frame or if the abort statement has been set.<br>
		 * @param timeout The time this functions waits at most for the requested frame number, in seconds, with range [0, infinity)
		 * @param abort Optional abort statement allowing to abort the frame number request at any time; set the value True to abort the request
		 * @return Resulting number of frames, 0xFFFFFFFF if the frame number is invalid
		 */
		virtual unsigned int synchronFrameNumberRequest(const double timeout = 10.0, bool* abort = nullptr) = 0;

		/**
		 * Invokes an asynchronous frame type request.
		 * An already registered frame type request callback function is necessary so that the requested frame type can be delivered eventually.<br>
		 * Beware: There is no guarantee that the requested frame type will be delivered eventually.<br>
		 * @see synchronFrameTypeRequest().
		 */
		virtual void asynchronFrameTypeRequest() = 0;

		/**
		 * Invokes a synchronous frame type request.
		 * This function returns an invalid frame type if the requested frame type cannot be determined within a specified time frame or if the abort statement has been set.<br>
		 * @param timeout The time this functions waits at most for the requested frame type, in seconds, with range [0, infinity)
		 * @param abort Optional abort statement allowing to abort the frame type request at any time; set the value True to abort the request
		 * @return Resulting frame type of this interface
		 */
		virtual FrameType synchronFrameTypeRequest(const double timeout = 10.0, bool* abort = nullptr) = 0;

		/**
		 * Registers a new callback function for asynchronous frame requests.
		 * All registered functions will be invoked whenever a requested frame arrives.<br>
		 * Each registered callback must be unregistered when it is not needed anymore or before the system is released.<br>
		 * @param callback Frame request callback function that will be registered
		 * @see unregisterFrameCallback().
		 */
		void registerFrameCallback(const FrameCallback& callback);

		/**
		 * Unregisters an already registered callback function for frame requests.
		 * @param callback The callback function that will be unregistered
		 * @see registerFrameCallback().
		 */
		void unregisterFrameCallback(const FrameCallback& callback);

		/**
		 * Registers a new callback function for asynchronous frame number requests.
		 * All registered functions will be invoked after a frame number request has been invoked.<br>
		 * Each registered callback must be unregistered when it is not needed anymore or before the system is released.<br>
		 * @param callback Frame request callback function that will be registered
		 * @see unregisterFrameCallback().
		 */
		void registerFrameNumberCallback(const FrameNumberCallback& callback);

		/**
		 * Unregisters an already registered callback function for frame number requests.
		 * @param callback The callback function that will be unregistered
		 * @see registerFrameNumberCallback().
		 */
		void unregisterFrameNumberCallback(const FrameNumberCallback& callback);

		/**
		 * Registers a new callback function for asynchronous frame type requests.
		 * All registered functions will be invoked after a frame type request has been invoked.<br>
		 * Each registered callback must be unregistered when it is not needed anymore or before the system is released.<br>
		 * @param callback Frame type request callback function that will be registered
		 * @see unregisterFrameTypeCallback().
		 */
		void registerFrameTypeCallback(const FrameTypeCallback& callback);

		/**
		 * Unregisters an already registered callback function for frame type requests.
		 * @param callback The callback function that will be unregistered
		 * @see registerFrameTypeCallback().
		 */
		void unregisterFrameTypeCallback(const FrameTypeCallback& callback);

		/**
		 * Releases all associated resources.
		 * Beware: The registered callback functions are not released.
		 */
		virtual void release();

	protected:

		/**
		 * Protected default constructor.
		 */
		FrameProviderInterface() = default;

		/**
		 * Disabled copy constructor.
		 * @param frameProviderInterface Object which would be copied
		 */
		FrameProviderInterface(const FrameProviderInterface& frameProviderInterface) = delete;

		/**
		 * Releases the interface.
		 * At the moment this interface is released all registered resources (callback functions) must be unregistered.
		 */
		virtual ~FrameProviderInterface();

		/**
		 * Disabled copy constructor.
		 * @param frameProviderInterface Object which would be copied
		 * @return Reference to this object
		 */
		FrameProviderInterface& operator=(const FrameProviderInterface& frameProviderInterface) = delete;

	protected:

		/// A list of frame request callbacks.
		FrameCallbacks frameCallbacks_;

		/// A list of frame number request callbacks.
		FrameNumberCallbacks frameNumberCallbacks_;

		/// A list of frame type request callbacks.
		FrameTypeCallbacks frameTypeCallbacks_;
};

}

}

#endif // META_OCEAN_CV_FRAME_PROVIDER_INTERFACE_H
