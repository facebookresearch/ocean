/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_RESOURCE_MANAGER_H
#define META_OCEAN_PLATFORM_RESOURCE_MANAGER_H

#include "ocean/platform/Platform.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Platform
{

/**
 * This class implements the platform independent base class for all resource managers which can be specialized for specific applications.
 * Application specific resource manager should be implemented as singletons and should be derived from this base class.
 * @ingroup platform
 */
class OCEAN_PLATFORM_EXPORT ResourceManager
{
	public:

		/**
		 * Definition of a vector holding 8 bit values.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of an object reference holding a buffer.
		 */
		typedef ObjectRef<Buffer> BufferRef;

	protected:

		/**
		 * Definition of a map mapping resource ids to frames with individual resolutions.
		 */
		typedef std::unordered_map<unsigned int, FrameRefs> FrameMap;

		/**
		 * Definition of a map mapping resource ids to buffer objects.
		 */
		typedef std::unordered_map<unsigned int, BufferRef> BufferMap;

	public:

		/**
		 * Adds a new frame resolution of/for a specified resource frame.
		 * Beware: The first resolution of a unique resource frame is expected to be the native resolution for screens without any explicit DPI scaling.
		 * @param id The application-wide unique id of the resource frame for which a frame with specific resolution will be registered
		 * @param frame The frame representing one specific resolution of the specified resource frame
		 */
		void addFrameResolution(const unsigned int id, const FrameRef& frame);

		/**
		 * Adds a new data/buffer resource.
		 * @param id The application-wide unique id of the resource
		 * @param buffer The buffer of the resource
		 */
		void addData(const unsigned int id, const BufferRef& buffer);

		/**
		 * Adds a new data/buffer resource (moves the buffer into this manager).
		 * @param id The application-wide unique id of the resource
		 * @param buffer The buffer of the resource to be moved
		 */
		void addData(const unsigned int id, Buffer&& buffer);

		/**
		 * Returns a specific resource frame.
		 * This function allows to specified the dimension of the resulting frame.
		 * @param id The application-wide unique id of the requested resource frame
		 * @param width Optional width of the resulting frame in pixel, with range [1u, infinity), 0 to return the frame with native resolution
		 * @param height Optional height of the resulting frame in pixel, with range [1u, infinity), 0 to return the frame with native resolution
		 * @param downscaleIfNecessary True, to return a frame exactly with the specified frame dimension, otherwise the next larger frame will be returned
		 * @return The specified frame, if any
		 */
		FrameRef frame(const unsigned int id, const unsigned int width = 0u, const unsigned int height = 0u, const bool downscaleIfNecessary = true);

		/**
		 * Returns a specific resource frame for a specified screen scaling factor.
		 * The provided factor is the relation between the current screen scaling and a native scaling.<br>
		 * I.e., The resulting frame for a scaling of 2 has twice of the dimension as the frame for a scaling of 1.
		 * @param id The application-wide unique id of the requested resource frame
		 * @param scaleFactor The screen scaling factor for which the frame with matching resolution is determined, with range (0, infinity)
		 * @return The requested frame, if any
		 */
		FrameRef frame(const unsigned int id, const double scaleFactor);

		/**
		 * Returns a specified resource buffer (any kind of arbitrary resource data which is not a frame/image).
		 * @param id The application-wide unique id of the requested resource data
		 * @return The requested data, if any
		 */
		BufferRef data(const unsigned int id);

		/**
		 * Releases the entire resources of this manager.
		 */
		void release();

	protected:

		/**
		 * The protected default constructor.
		 */
		ResourceManager();

		/**
		 * Destructs the resource manager.
		 */
		virtual ~ResourceManager();

		/**
		 * Returns the interpolated frame of a specified resource frame.
		 * If the specified frame dimension is larger than the dimension of the largest native frame, the largest native frame will be returned.
		 * @param id The id of the resource frame for which the interpolated frame will be returned
		 * @param width The width of the resulting interpolated frame in pixel, with range [1, infinity)
		 * @param height The height of the resulting interpolated frame in pixel, with range [1, infinity)
		 * @return The resulting interpolated frame, or the largest available native frame if the largest native frame is smaller than the requested one
		 */
		FrameRef interpolatedFrame(const unsigned int id, const unsigned int width, const unsigned int height);

		/**
		 * Returns the largest frame from a set of given frames.
		 * @param frames The set of given frames, at least one
		 * @return The largest frame from the specified set
		 */
		static FrameRef largestFrame(const FrameRefs& frames);

	protected:

		/// The frame map of the native resource frames.
		FrameMap nativeFrames_;

		/// The frame map of the interpolated frames.
		FrameMap interpolatedFrames_;

		/// The map of resource buffers.
		BufferMap bufferMap_;

		/// The manager's lock.
		Lock lock_;
};

}

}

#endif // META_OCEAN_PLATFORM_RESOURCE_MANAGER_H
