/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_PYRAMID_H
#define META_OCEAN_CV_FRAME_PYRAMID_H

#include "ocean/cv/CV.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/FrameShrinkerAlpha.h"

#include <functional>

namespace Ocean
{

namespace CV
{

/**
 * This class implements a frame pyramid.<br>
 * A frame pyramid holds the same frame with several scale spaces.<br>
 * Each further layer holds the frame with half size (half width and half height).<br>
 * The finest layer has index 0 and the coarsest layer has the highest index.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FramePyramid
{
	public:

		/**
		 * Definition of individual down sampling modes.
		 */
		enum DownsamplingMode : uint32_t
		{
			/**
			 * Down sampling is realized by a 2x2 averaging filter.
			 * This down sampling mode is the fastest mode available, as 2x2 pixel blocks are simply averaged.<br>
			 * The corresponding filter mask has the following layout:
			 * <pre>
			 * | 1 1 |
			 * | 1 1 | * 1/4
			 * </pre>
			 * The upper left filter element is applied to every even pixel location in the source frame.<br>
			 * In case, the width or height of a given frame is odd, the last column/row will apply a 121 filter.
			 */
			DM_FILTER_11,

			/**
			 * Down sampling is realized by a 5x5 Gaussian filter.
			 * This down sampling mode is more expensive but reduces aliasing effects on down sampled images.<br>
			 * The corresponding filter mask has the following layout:
			 * <pre>
			 * | 1  4  6  4 1 |
			 * | 4 16 24 16 4 |
			 * | 6 24 36 24 6 | * 1/256
			 * | 4 16 24 16 4 |
			 * | 1  4  6  4 1 |
			 * </pre>
			 * The center of the filter is applied to every even pixel location in the source frame.<br>
			 * At the border of frames, the filter responses are determined based on mirrored pixel values.
			 */
			DM_FILTER_14641
		};

		/**
		 * Definition of a function allowing to downsample a frame.
		 * @param sourceLayer The source layer to downsample
		 * @param targetLayer The target layer reviving the downsampled image content
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		using DownsamplingFunction = std::function<bool(const Frame& sourceLayer, Frame& targetLayer, Worker* worker)>;

		/**
		 * Definition of a value that can be used to create as many pyramid layers as possible (so that the coarsest pyramid layer has resolution 1x1).
		 */
		static constexpr unsigned int AS_MANY_LAYERS_AS_POSSIBLE = (unsigned int)(-1);

	protected:

		/// The number of bytes for memory alignment.
		static constexpr size_t memoryAlignmentBytes_ = 8;

	public:

		/**
		 * Creates an empty frame pyramid object.
		 */
		inline FramePyramid();

		/**
		 * Copy constructor.
		 * @param framePyramid Frame pyramid to be copied
		 * @param copyData True, to copy the image content of the frame pyramid; False, to only reuse the image content of the frame pyramid
		 */
		FramePyramid(const FramePyramid& framePyramid, const bool copyData);

		/**
		 * Move constructor.
		 * @param framePyramid Frame pyramid to be moved
		 */
		FramePyramid(FramePyramid&& framePyramid) noexcept;

		/**
		 * Creates a frame pyramid object for a given frame type and layer number.
		 * The resulting pyramid may have fewer layers than desired.<br>
		 * The image content of the replaced frame pyramid will be uninitialized.
		 * @param layers The preferred number of layers to be created, with range [1, infinity), AS_MANY_LAYERS_AS_POSSIBLE to create as may layers as possible
		 * @param frameType Type of the frame to create a frame pyramid for, must be valid
		 */
		explicit FramePyramid(const unsigned int layers, const FrameType& frameType);

		/**
		 * Creates a new pyramid frame for frames with 1 plane and data type DT_UNSIGNED_INTEGER_8 applying a 1-1 downsampling.
		 * This constructor is intentionally restrictive to reduce binary impact when used, use other constructors or functions in case more flexibility is needed an binary size does not matter.<br>
		 * The constructor mainly calls replace8BitPerChannel11().
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the given frame has, with range [1, infinity)
		 * @param pixelOrigin The pixel origin of the given frame
		 * @param layers Number of pyramid layers to be created, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @param pixelFormat The explicit pixel format which will be used for each pyramid layer, must be compatible with DT_UNSIGNED_INTEGER_8 and 'channels'; FORMAT_UNDEFINED to use a generic pixel format
		 * @param timestamp Timestamp to be assigned to the frame pyramid (e.g., the timestamp of the frame used to created the timestamp)
		 * @see replace8BitPerChannel11().
		 */
		inline FramePyramid(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker = nullptr, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED, const Timestamp timestamp = Timestamp(false));

		/**
		 * Creates a new pyramid frame for frames with 1 plane and data type DT_UNSIGNED_INTEGER_8 applying a 1-1 downsampling.
		 * This constructor is intentionally restrictive to reduce binary impact when used, use other constructors or functions in case more flexibility is needed an binary size does not matter.<br>
		 * In case the provided frame not a valid 1-plane DT_UNSIGNED_INTEGER_8 frame, the pyramid will be invalid.<br>
		 * The constructor mainly calls replace8BitPerChannel11().
		 * @param frame The frame for which the pyramid will be created, with 1 plane and data type DT_UNSIGNED_INTEGER_8, must be valid
		 * @param layers Number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @see replace8BitPerChannel11().
		 */
		inline FramePyramid(const Frame& frame, const unsigned int layers, const bool copyFirstLayer, Worker* worker = nullptr);

		/**
		 * Creates a frame pyramid based on a frame with 1 plane and data type DT_UNSIGNED_INTEGER_8 applying a custom downsampling.
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer) or will just use the memory, depending on 'copyFirstLayer'.<br>
		 * The constructor mainly calls replace8BitPerChannel().
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the given frame has, with range [1, infinity)
		 * @param pixelOrigin The pixel origin of the given frame
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param layers Number of pyramid layers to be created, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @param pixelFormat The explicit pixel format which will be used for each pyramid layer, must be compatible with DT_UNSIGNED_INTEGER_8 and 'channels'; FORMAT_UNDEFINED to use a generic pixel format
		 * @param timestamp Timestamp to be assigned to the frame pyramid (e.g., the timestamp of the frame used to created the timestamp)
		 * @see replace8BitPerChannel().
		 */
		inline FramePyramid(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const DownsamplingMode downsamplingMode, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED, const Timestamp timestamp = Timestamp(false));

		/**
		 * Creates a frame pyramid based on a frame applying a custom downsampling.
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer) or will just use the memory, depending on 'copyFirstLayer'.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case,
		 * @param worker Optional worker object to distribute the computation
		 */
		inline FramePyramid(const Frame& frame, const DownsamplingMode downsamplingMode, const unsigned int layers, const bool copyFirstLayer, Worker* worker);

		/**
		 * Creates a frame pyramid based on a frame applying a custom downsampling.
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer) or will just use the memory, depending on 'copyFirstLayer'.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingFunction The custom function used to downsample the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case,
		 * @param worker Optional worker object to distribute the computation
		 */
		inline FramePyramid(const Frame& frame, const DownsamplingFunction& downsamplingFunction, const unsigned int layers, const bool copyFirstLayer, Worker* worker);

		/**
		 * Creates a frame pyramid based on a frame applying a custom downsampling.
		 * The resulting pyramid will re-used the given frame (as finest pyramid layer); thus, ensure that the frame's memory is valid as long as this pyramid exists.
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		inline FramePyramid(const DownsamplingMode downsamplingMode, Frame&& frame, const unsigned int layers, Worker* worker);

		/**
		 * Creates a frame pyramid based on a frame applying a custom downsampling.
		 * The resulting pyramid will re-used the given frame (as finest pyramid layer); thus, ensure that the frame's memory is valid as long as this pyramid exists.
		 * @param downsamplingFunction The custom function used to downsample the individual pyramid layers, must be valid
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		inline FramePyramid(const DownsamplingFunction& downsamplingFunction, Frame&& frame, const unsigned int layers, Worker* worker);

		/**
		 * Creates a new frame pyramid based on an existing frame pyramid.
		 * @param framePyramid The existing frame pyramid from which the new pyramid will be created, must be valid
		 * @param firstLayerIndex The index of the first layer to use from the source pyramid, with range [0, framePyramid.layers() - 1]
		 * @param layers The number of layers to use from the source pyramid, with range [1, infinity), AS_MANY_LAYERS_AS_POSSIBLE to use as many layers as exist
		 * @param copyData True, to make a copy of the image content of the existing pyramid; False, to only use the memory
		 */
		FramePyramid(const FramePyramid& framePyramid, const unsigned int firstLayerIndex, const unsigned int layers, bool copyData);

		/**
		 * Returns the frame of a specified layer.
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline const Frame& layer(const unsigned int layer) const;

		/**
		 * Returns the frame of a specified layer.
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline Frame& layer(const unsigned int layer);

		/**
		 * Returns the finest layer frame of this pyramid.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @return Finest pyramid layer frame
		 */
		inline const Frame& finestLayer() const;

		/**
		 * Returns the coarsest layer frame of this pyramid.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @return Finest pyramid layer frame
		 */
		inline Frame& finestLayer();

		/**
		 * Returns the coarsest layer frame of this pyramid regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.<br>
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!<br>
		 * @return Finest pyramid layer frame
		 */
		inline const Frame& coarsestLayer() const;

		/**
		 * Returns the finest layer frame of this pyramid regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.<br>
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!<br>
		 * @return Finest pyramid layer frame
		 */
		inline Frame& coarsestLayer();

		/**
		 * Returns the number of layers this pyramid holds.
		 * @return Pyramid layers, with range [0, infinity)
		 */
		inline unsigned int layers() const;

		/**
		 * Returns the width of a given layer.
		 * @param layer The layer to return the width for, with range [0, layers())
		 * @return Width in pixel
		 */
		inline unsigned int width(const unsigned int layer) const;

		/**
		 * Returns the height of a given layer.
		 * @param layer The layer to return the height for, with range [0, layers())
		 * @return Height in pixel
		 */
		inline unsigned int height(const unsigned int layer) const;

		/**
		 * Returns the width of the finest (first) layer.
		 * @return Width in pixel
		 */
		inline unsigned int finestWidth() const;

		/**
		 * Returns the height of the finest (first) layer.
		 * @return Height in pixel
		 */
		inline unsigned int finestHeight() const;

		/**
		 * Returns the width of the coarsest (last) layer regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.
		 * @return Width in pixel
		 */
		inline unsigned int coarsestWidth() const;

		/**
		 * Returns the height of the coarsest (last) layer regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.
		 * @return Height in pixel
		 */
		inline unsigned int coarsestHeight() const;

		/**
		 * Returns the size factor for the coarsest layer in relation to the finest layer regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.
		 * @return Coarsest layer size factor
		 */
		inline unsigned int coarsestSizeFactor() const;

		/**
		 * Returns the frame type of the finest layer.
		 * Beware: Ensure that the pyramid holds at least one pyramid layer before calling this function.
		 * @return Frame type
		 */
		inline const FrameType& frameType() const;

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer) or will just use the memory, depending on 'copyFirstLayer'.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(const Frame& frame, const DownsamplingMode downsamplingMode, const unsigned int layers, const bool copyFirstLayer, Worker* worker);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer) or will just use the memory, depending on 'copyFirstLayer'.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingFunction The custom function used to downsample the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(const Frame& frame, const DownsamplingFunction& downsamplingFunction, const unsigned int layers, const bool copyFirstLayer, Worker* worker);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will re-used the given frame (as finest pyramid layer); thus, ensure that the frame's memory is valid as long as this pyramid exists.
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(const DownsamplingMode downsamplingMode, Frame&& frame, const unsigned int layers, Worker* worker);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will re-used the given frame (as finest pyramid layer); thus, ensure that the frame's memory is valid as long as this pyramid exists.
		 * @param downsamplingFunction The custom function used to downsample the individual pyramid layers, must be valid
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(const DownsamplingFunction& downsamplingFunction, Frame&& frame, const unsigned int layers, Worker* worker);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer).
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the given frame has, with range [1, infinity)
		 * @param pixelOrigin The pixel origin of the given frame
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param layers Number of pyramid layers to be created, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @param pixelFormat The explicit pixel format which will be used for each pyramid layer, must be compatible with DT_UNSIGNED_INTEGER_8 and 'channels'; FORMAT_UNDEFINED to use a generic pixel format
		 * @param timestamp Timestamp to be assigned to the frame pyramid (e.g., the timestamp of the frame used to created the timestamp)
		 * @return True, if the frame pyramid was replaced
		 * @see replace().
		 */
		bool replace8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const DownsamplingMode downsamplingMode, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED, const Timestamp timestamp = Timestamp(false));

		/**
		 * Replaces this frame pyramid by a new frame with 1 plane and data type DT_UNSIGNED_INTEGER_8 applying a 1-1 downsampling.
		 * This function is intentionally restrictive to reduce binary impact when used, use other function or the constructor in case more flexibility is needed an binary size does not matter.<br>
		 * The function will re-used the existing pyramid's memory of possible.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the given frame has, with range [1, infinity)
		 * @param pixelOrigin The pixel origin of the given frame
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @param pixelFormat The explicit pixel format which will be used for each pyramid layer, must be compatible with DT_UNSIGNED_INTEGER_8 and 'channels'; FORMAT_UNDEFINED to use a generic pixel format
		 * @param timestamp Timestamp to be assigned to the frame pyramid (e.g., the timestamp of the frame used to created the timestamp)
		 * @return True, if the frame pyramid was replaced
		 * @see isOwner(), replace().
		 */
		bool replace8BitPerChannel11(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED, const Timestamp timestamp = Timestamp(false));

		/**
		 * Replaces this frame pyramid by a new frame with 1 plane and data type DT_UNSIGNED_INTEGER_8 applying a 1-1 downsampling.
		 * This function is intentionally restrictive to reduce binary impact when used, use other function or the constructor in case more flexibility is needed an binary size does not matter.<br>
		 * The function will re-used the existing pyramid's memory of possible.<br>
		 * This function does not provide the optimal image quality for images with alpha channel, use replace() instead.
		 * @param frame The frame for which the pyramid will be created, with 1 plane and data type DT_UNSIGNED_INTEGER_8, should not contain an alpha channel, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see isOwner(), replace().
		 */
		inline bool replace8BitPerChannel11(const Frame& frame, const unsigned int layers, const bool copyFirstLayer, Worker* worker);

		/**
		 * Replaces this frame pyramid with a new pyramid defined by the frame type of the finest layer.
		 * The image content of the replaced frame pyramid will be uninitialized.
		 * @param frameType The type of the finest pyramid layer, must be valid
		 * @param layers The number of layers to be created during the resizing, the resulting layers will be as many as possible but not exceed this value, with range [1, infinity)
		 * @param forceOwner True, to force the pyramid to be the owner of the memory afterwards; False, to allow that the pyramid is not owning the memory (because the memory is managed outside of this pyramid)
		 * @return True, if succeeded
		 */
		inline bool replace(const FrameType& frameType, const bool forceOwner, const unsigned int layers);

		/**
		 * Reduces the number of pyramid layers.
		 * @param layers The number of pyramid layers, with range [0, layers()]
		 */
		void reduceLayers(const size_t layers);

		/**
		 * Releases the internal frame layers.
		 */
		inline void clear();

		/**
		 * Returns whether the frame pyramid is the owner of the entire image data or owner of a specific pyramid layer.
		 * @param layerIndex The index of the layer to be checked, 'AS_MANY_LAYERS_AS_POSSIBLE" to check all layers
		 * @return True, if so
		 */
		bool isOwner(const unsigned int layerIndex = AS_MANY_LAYERS_AS_POSSIBLE) const;

		/**
		 * Returns whether this pyramid holds at least one frame layer.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the pyramid's memory block.
		 * This functions is intended for testing purposes only, don't use this function.
		 * @return The pyramid's memory block, if any
		 */
		inline const Memory& memory() const;

		/**
		 * Move operator
		 * @param right The right frame to assign
		 * @return Reference to this frame
		 */
		FramePyramid& operator=(FramePyramid&& right) noexcept;

		/**
		 * Returns the frame of a specified layer.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline const Frame& operator[](const unsigned int layer) const;

		/**
		 * Returns the frame of a specified layer.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline Frame& operator[](const unsigned int layer);

		/**
		 * Returns whether this pyramid holds at least one frame layer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Returns the size factor of a specified layer in relation to the finest layer.
		 * The finest (first) layer has factor 1, the second layer has factor 2, the third layer has factor 4, ...<br>
		 * @param layer The layer to return the size factor for, with range [1, 31]
		 * @return The resulting size factor, with range [1, infinity)
		 */
		static constexpr unsigned int sizeFactor(const unsigned int layer);

		/**
		 * Determines the number of layers until an invalid frame size would be reached in the next layer.
		 * @param width The width of the finest layer in pixel, with range [1, infinity)
		 * @param height The height of the finest layer in pixel, with range [1, infinity)
		 * @param invalidCoarsestWidthOrHeight The width or the height of a coarse layer not valid any more, with range (0, infinity)
		 * @param coarsestLayerWidth Optional resulting width of the coarsest valid pyramid layer, with range [invalidCoarsestWidth + 1u, width], nullptr if not of interest
		 * @param coarsestLayerHeight Optional resulting height of the coarsest valid pyramid layer, with range [invalidCoarsestHeight + 1u, height], nullptr if not of interest
		 * @return Resulting layers so that the invalid frame size will not be reached, with range [1, infinity), 0 if 'width' or 'height' are smaller or equal than 'invalidCoarsestWidthOrHeight'
		 */
		static unsigned int idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidCoarsestWidthOrHeight, unsigned int* coarsestLayerWidth = nullptr, unsigned int* coarsestLayerHeight = nullptr);

		/**
		 * Determines the number of layers until an invalid frame size would be reached in the next layer.
		 * @param width The width of the finest layer in pixel, with range [1, infinity)
		 * @param height The height of the finest layer in pixel, with range [1, infinity)
		 * @param invalidCoarsestWidth The width of a coarse layer not valid any more, with range (0, infinity)
		 * @param invalidCoarsestHeight The height of a coarse layer not valid any more, with range (0, infinity)
		 * @param coarsestLayerWidth Optional resulting width of the coarsest valid pyramid layer, with range [invalidCoarsestWidth + 1u, width], nullptr if not of interest
		 * @param coarsestLayerHeight Optional resulting height of the coarsest valid pyramid layer, with range [invalidCoarsestHeight + 1u, height], nullptr if not of interest
		 * @return Resulting layers so that the invalid frame size will not be reached, with range [1, infinity), 0 if 'width/height' is smaller or equal than 'invalidCoarsestWidth/invalidCoarsestHeight'
		 */
		static unsigned int idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidCoarsestWidth, const unsigned int invalidCoarsestHeight, unsigned int* coarsestLayerWidth = nullptr, unsigned int* coarsestLayerHeight = nullptr);

		/**
		 * Determines the number of layers until an invalid frame size would be reached in the next layer or an overall size radius is reached.
		 * @param width The width of the finest layer in pixel
		 * @param height The height of the finest layer in pixel
		 * @param invalidCoarsestWidth Width of a coarse layer not valid any more, with range (0, infinity)
		 * @param invalidCoarsestHeight Height of a coarse layer not valid any more, with range (0, infinity)
		 * @param layerFactor Size factor on each layer (a factor of 2 means that the layer dimension is halved on each layer), with range [2, infinity)
		 * @param maximalRadius Maximal radius that can be reached on the finest layer by starting with coarsestLayerRadius on the coarsest layer in pixel (the maximal expected baseline between two pixels in the finest pyramid layer), with range [1, infinity)
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param coarsestLayerWidth Optional resulting width of the coarsest valid pyramid layer, with range [invalidCoarsestWidth + 1u, width], nullptr if not of interest
		 * @param coarsestLayerHeight Optional resulting height of the coarsest valid pyramid layer, with range [invalidCoarsestHeight + 1u, height], nullptr if not of interest
		 * @return Resulting layers so that the invalid frame size will not be reached, with range [1, infinity), 0 if 'width/height' is smaller or equal than 'invalidCoarsestWidth/invalidCoarsestHeight'
		 */
		static unsigned int idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidCoarsestWidth, const unsigned int invalidCoarsestHeight, const unsigned int layerFactor, const unsigned int maximalRadius = (unsigned int)(-1), const unsigned int coarsestLayerRadius = 2u, unsigned int* coarsestLayerWidth = nullptr, unsigned int* coarsestLayerHeight = nullptr);

	protected:

		/**
		 * Disabled constructor.
		 */
		explicit FramePyramid(Frame&&) = delete;

		/**
		 * Disabled constructor.
		 */
		FramePyramid(const Frame&, const bool) = delete;

		/**
		 * Disabled constructor to prevent confusion between all constructors.
		 */
		FramePyramid(const Frame& frame, const DownsamplingMode downsamplingMode, const unsigned int layers,  Worker* worker) = delete;

		/**
		 * Disabled constructor to prevent confusion between all constructors.
		 */
		FramePyramid(const Frame& frame, const DownsamplingFunction& downsamplingFunction, const unsigned int layers, Worker* worker) = delete;

		/**
		 * Replaces this frame pyramid with a new pyramid defined by the frame type of the finest layer.
		 * The image content of the replaced frame pyramid will be uninitialized.
		 * @param frameType The type of the finest pyramid layer, must be valid
		 * @param reserveFirstLayerMemory True, to reserve memory for the first pyramid layer (and to initialize the first layer frame); False, to reserve memory for the remaining pyramid layers only (and to skip initializing the first layer frame)
		 * @param layers The number of layers to be created during the resizing, the resulting layers will be as many as possible but not exceed this value, with range [1, infinity)
		 * @param forceOwner True, to force the pyramid to be the owner of the memory afterwards; False, to allow that the pyramid is not owning the memory (because the memory is managed outside of this pyramid)
		 * @return True, if succeeded
		 */
		bool replace(const FrameType& frameType, const bool reserveFirstLayerMemory, const bool forceOwner, const unsigned int layers);

		/**
		 * Calculates the size of the entire pyramid in bytes covering all images in all pyramid layers.
		 * @param width The width of the finest layer in pixel, with range [0, 65535]
		 * @param height The height of the finest layer in pixel, with range [0, 65535]
		 * @param pixelFormat The pixel format of each layer, must be a generic 1-plane pixel format, must be valid
		 * @param layers Number of layers, with range [0, infinity)
		 * @param includeFirstLayer True, to determine the memory for all layers; False, to skip the first layer and only determine the memory for all remaining (coarser) layers
		 * @param totalLayers Optional resulting number of pyramid layers that will exist (always counts the very first layer independently of 'includeFirstLayer'), with range [0, layers]
		 * @return Resulting number of bytes, with range [0, infinity)
		 */
		static size_t calculateMemorySize(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const unsigned int layers, const bool includeFirstLayer, unsigned int* totalLayers = nullptr);

		/**
		 * Deleted function to prevent confusion between Frame and FrameType.
		 * @param frame The potential frame to be used
		 * @param layers The number of layers
		 * @param forceOwner The ownership information
		 * @return True, if succeeded
		 */
		bool replace(const Frame& frame, const bool forceOwner, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE) = delete;

		/**
		 * Disabled assign operator
		 * Use a constructor or the move operator instead.
		 * @return Reference to this object
		 */
		FramePyramid& operator=(const FramePyramid&) = delete;

		/**
		 * Downsamples a frame with 1-1 filter.
		 * @param finerLayer The finer pyramid layer, must be valid
		 * @param coarserLayer The coarser pyramid layer, must be valid
		 * @param worker The optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool downsampleByTwo11(const Frame& finerLayer, Frame& coarserLayer, Worker* worker);

		/**
		 * Downsamples a frame with 1-1 filter which contains an alpha channel.
		 * @param finerLayer The finer pyramid layer, must be valid
		 * @param coarserLayer The coarser pyramid layer, must be valid
		 * @param worker The optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool downsampleAlphaByTwo11(const Frame& finerLayer, Frame& coarserLayer, Worker* worker);

		/**
		 * Downsamples a frame with 1-4-6-4-1 filter.
		 * @param finerLayer The finer pyramid layer, must be valid
		 * @param coarserLayer The coarser pyramid layer, must be valid
		 * @param worker The optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool downsampleByTwo14641(const Frame& finerLayer, Frame& coarserLayer, Worker* worker);

		/**
		 * Returns the downsampling function for a specified downsampling mode.
		 * @param downsamplingMode The downsampling mode for which the function will be returned
		 * @param pixelFormat The pixel format for which the downsampling function will be returned, must be valid
		 * @return The downsampling function, nullptr if unknown
		 */
		static CV::FramePyramid::DownsamplingFunction downsamplingFunction(const CV::FramePyramid::DownsamplingMode downsamplingMode, const FrameType::PixelFormat pixelFormat);

		/**
		 * Deleted function to prevent confusion between several different replace functions.
		 */
		bool replace(const Frame&, const DownsamplingMode, const unsigned int layers, Worker* worker) = delete;

		/**
		 * Deleted function to prevent confusion between several different replace functions.
		 */
		bool replace(const Frame&, const DownsamplingFunction&, const unsigned int layers, Worker* worker) = delete;

	protected:

		/// The individual layers of this pyramid, zero if not valid.
		Frames layers_;

		/// Optional memory which may be used by at least one pyramid layer.
		Memory memory_;
};

inline FramePyramid::FramePyramid()
{
	// nothing to do here
}

inline FramePyramid::FramePyramid(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker, const FrameType::PixelFormat pixelFormat, const Timestamp timestamp)
{
	ocean_assert(frame != nullptr && width >= 1u && height >= 1u && layers >= 1u);
	ocean_assert(channels >= 1u);

	const bool result = replace8BitPerChannel11(frame, width, height, channels, pixelOrigin, layers, framePaddingElements, copyFirstLayer, worker, pixelFormat, timestamp);
	ocean_assert_and_suppress_unused(result, result);
}

inline FramePyramid::FramePyramid(const Frame& frame, const unsigned int layers, const bool copyFirstLayer, Worker* worker)
{
	const bool result = replace8BitPerChannel11(frame, layers, copyFirstLayer, worker);
	ocean_assert_and_suppress_unused(result, result);
}

inline FramePyramid::FramePyramid(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const FrameType::PixelOrigin pixelOrigin, const DownsamplingMode downsamplingMode, const unsigned int layers, const unsigned int framePaddingElements, const bool copyFirstLayer, Worker* worker, const FrameType::PixelFormat pixelFormat, const Timestamp timestamp)
{
	ocean_assert(frame != nullptr && width >= 1u && height >= 1u && layers >= 1u);
	ocean_assert(channels >= 1u);

	const bool result = replace8BitPerChannel(frame, width, height, channels, pixelOrigin, downsamplingMode, layers, framePaddingElements, copyFirstLayer, worker, pixelFormat, timestamp);
	ocean_assert_and_suppress_unused(result, result);
}

inline FramePyramid::FramePyramid(const Frame& frame, const DownsamplingMode downsamplingMode, const unsigned int layers, const bool copyFirstLayer, Worker* worker)
{
	const bool result = replace(frame, downsamplingMode, layers, copyFirstLayer, worker);
	ocean_assert_and_suppress_unused(result, result);
}

inline FramePyramid::FramePyramid(const Frame& frame, const DownsamplingFunction& downsamplingFunction, const unsigned int layers, const bool copyFirstLayer, Worker* worker)
{
	const bool result = replace(frame, downsamplingFunction, layers, copyFirstLayer, worker);
	ocean_assert_and_suppress_unused(result, result);
}

inline FramePyramid::FramePyramid(const DownsamplingMode downsamplingMode, Frame&& frame, const unsigned int layers, Worker* worker)
{
	const bool result = replace(downsamplingMode, std::move(frame), layers, worker);
	ocean_assert_and_suppress_unused(result, result);
}

inline FramePyramid::FramePyramid(const DownsamplingFunction& downsamplingFunction, Frame&& frame, const unsigned int layers, Worker* worker)
{
	const bool result = replace(downsamplingFunction, std::move(frame), layers, worker);
	ocean_assert_and_suppress_unused(result, result);
}

inline const Frame& FramePyramid::layer(const unsigned int layer) const
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline Frame& FramePyramid::layer(const unsigned int layer)
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline const Frame& FramePyramid::finestLayer() const
{
	ocean_assert(isValid());
	return layers_.front();
}

inline Frame& FramePyramid::finestLayer()
{
	ocean_assert(isValid());
	return layers_.front();
}

inline const Frame& FramePyramid::coarsestLayer() const
{
	ocean_assert(isValid());

	return layers_.back();
}

inline Frame& FramePyramid::coarsestLayer()
{
	ocean_assert(isValid());

	return layers_.back();
}

inline unsigned int FramePyramid::layers() const
{
	return (unsigned int)layers_.size();
}

inline unsigned int FramePyramid::width(const unsigned int layer) const
{
	ocean_assert(layer < layers_.size());
	return layers_[layer].width();
}

inline unsigned int FramePyramid::height(const unsigned int layer) const
{
	ocean_assert(layer < layers_.size());
	return layers_[layer].height();
}

inline unsigned int FramePyramid::finestWidth() const
{
	ocean_assert(isValid());
	return layers_.front().width();
}

inline unsigned int FramePyramid::finestHeight() const
{
	ocean_assert(isValid());
	return layers_.front().height();
}

inline unsigned int FramePyramid::coarsestWidth() const
{
	ocean_assert(isValid());

	return layers_.back().width();
}

inline unsigned int FramePyramid::coarsestHeight() const
{
	ocean_assert(isValid());

	return layers_.back().height();
}

unsigned int FramePyramid::coarsestSizeFactor() const
{
	ocean_assert(isValid());

	return 1u << (unsigned int)((layers_.size() - 1));
}

inline const FrameType& FramePyramid::frameType() const
{
	ocean_assert(isValid());
	return layers_.front().frameType();
}

inline bool FramePyramid::replace8BitPerChannel11(const Frame& frame, const unsigned int layers, const bool copyFirstLayer, Worker* worker)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.pixelOrigin(), layers, frame.paddingElements(), copyFirstLayer, worker, frame.pixelFormat(), frame.timestamp());
	}

	return false;
}

inline bool FramePyramid::replace(const FrameType& frameType, const bool forceOwner, const unsigned int layers)
{
	return replace(frameType, true /*reserveFirstLayerMemory*/, forceOwner, layers);
}

constexpr unsigned int FramePyramid::sizeFactor(const unsigned int layer)
{
	ocean_assert(layer <= 31u);
	if (layer > 31u)
	{
		return 0u;
	}

	return 1u << layer;
}

inline void FramePyramid::clear()
{
	layers_.clear();
	memory_.free();
}

inline const Frame& FramePyramid::operator[](const unsigned int layer) const
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline Frame& FramePyramid::operator[](const unsigned int layer)
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline bool FramePyramid::isValid() const
{
	return !layers_.empty();
}

inline const Memory& FramePyramid::memory() const
{
	return memory_;
}

inline FramePyramid::operator bool() const
{
	return isValid();
}

}

}

#endif // META_OCEAN_CV_FRAME_PYRAMID_H
