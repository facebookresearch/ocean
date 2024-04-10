// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_FRAME_PYRAMID_H
#define META_OCEAN_CV_FRAME_PYRAMID_H

#include "ocean/cv/CV.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameShrinker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

// Forward declaration.
class TestFramePyramid;

}

}

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
	friend class Ocean::Test::TestCV::TestFramePyramid;

	public:

		/**
		 * Definition of individual down sampling modes.
		 */
		enum DownsamplingMode
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
			DM_FILTER_14641,

			/**
			 * A custom down sampling mode.
			 */
			DM_CUSTOM
		};

		/**
		 * Definition of a callback function applying the frame down sampling.
		 * The first parameter holds the source frame.<br>
		 * The second parameter holds the target frame receiving the down sampled image content, the frame type must not be changed.<br>
		 * The third parameter might be an optional worker object to distribute the computation.
		 * Returns True, if succeeded
		 */
		typedef Callback<bool, const LegacyFrame&, LegacyFrame&, Worker*> CallbackDownsampling;

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
		 */
		FramePyramid(const FramePyramid& framePyramid);

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
		 * In case the provided frame not a valid 1-plane DT_UNSIGNED_INTEGER_8 frame, the pyramid will be invalid.
		 * @param frame The frame for which the pyramid will be created, with 1 plane and data type DT_UNSIGNED_INTEGER_8, must be valid
		 * @param layers Number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the memory of the first layer into the pyramid; False, to re-use the memory of the first layer only (in this case, ensure that the memory of the first layer exists as long as this pyramid exist)
		 * @param worker Optional worker object to distribute the computation
		 * @see replace8BitPerChannel11().
		 */
		inline FramePyramid(const Frame& frame, const unsigned int layers, const bool copyFirstLayer, Worker* worker = nullptr);

		/**
		 * Creates a frame pyramid object for a given frame and layer number.
		 * The given frame is copied and used as finest layer.
		 * @param frame The frame to create the pyramid for, must be valid.
		 * @param layers Number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param downsamplingMode The down sampling mode to be used to create lower image resolutions, 'DM_CUSTOM' in combination with a valid 'customDownsamplingFunction' to use a custom down sampling mode
		 * @param customDownsamplingFunction Optional explicit custom callback of the down sampling function to be used, define an invalid callback object to use the default down sampling function determined by 'mode'
		 */
		explicit inline FramePyramid(const Frame& frame, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr, const DownsamplingMode downsamplingMode = DM_FILTER_11, const CallbackDownsampling& customDownsamplingFunction = CallbackDownsampling());

		/**
		 * Creates a new frame pyramid by using layers from an existing source frame pyramid.
		 * The range of used layers can be specified by layerIndex and layerCount.<br>
		 * If copyData is False, then only a reference is created to the given source frame data; the new frame pyramid will be a read-only pyramid.<br>
		 * If copyData is True and the source pyramid has less layers then specified, the additional layers are generated; the resulting frame pyramid will be a writable pyramid.<br>
		 * In any case, the source pyramid is not modified.
		 * @param framePyramid Source frame pyramid, This frame pyramid needs to have at least one valid layer
		 * @param copyData If true, then the used frame data of the source pyramid is copied. Otherwise, only a reference to the used frame data of the source pyramid is created
		 * @param firstLayerIndex Index of first layer to use from the source pyramid. The index needs to be in the range [0; source layers], where source layers is the number of valid layers in the specified source pyramid
		 * @param layerCount The number of layers to use from the source pyramid, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param downsamplingMode The down sampling mode to be used to create lower image resolutions, 'DM_CUSTOM' in combination with a valid 'customDownsamplingFunction' to use a custom down sampling mode
		 * @param customDownsamplingFunction Optional explicit custom callback of the down sampling function to be used, define an invalid callback object to use the default down sampling function determined by 'mode'
		 * @see create8BitPerChannel().
		 */
		FramePyramid(const FramePyramid& framePyramid, bool copyData, const unsigned int firstLayerIndex = 0u, const unsigned int layerCount = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr, const DownsamplingMode downsamplingMode = DM_FILTER_11, const CallbackDownsampling& customDownsamplingFunction = CallbackDownsampling());

		/**
		 * Returns the frame of a specified layer.
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline const LegacyFrame& layer(const unsigned int layer) const;

		/**
		 * Returns the frame of a specified layer.
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline LegacyFrame& layer(const unsigned int layer);

		/**
		 * Returns the finest layer frame of this pyramid.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @return Finest pyramid layer frame
		 */
		inline const LegacyFrame& finestLayer() const;

		/**
		 * Returns the coarsest layer frame of this pyramid.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @return Finest pyramid layer frame
		 */
		inline LegacyFrame& finestLayer();

		/**
		 * Returns the coarsest layer frame of this pyramid regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.<br>
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!<br>
		 * @return Finest pyramid layer frame
		 */
		inline const LegacyFrame& coarsestLayer() const;

		/**
		 * Returns the finest layer frame of this pyramid regarding to the number of valid layers.
		 * If no valid layer is stored in this pyramid, the finest layer is used instead.<br>
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!<br>
		 * @return Finest pyramid layer frame
		 */
		inline LegacyFrame& coarsestLayer();

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
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer).
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(const Frame& frame, const DownsamplingMode downsamplingMode, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will re-used the given frame (as finest pyramid layer); thus, ensure that the frame's memory is valid as long as this pyramid exists.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingMode The downsampling mode to use when creating the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(Frame&& frame, const DownsamplingMode downsamplingMode, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will contain a copy of the given frame (as finest pyramid layer).
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingFunction The custom function used to downsample the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(const Frame& frame, const CallbackDownsampling& downsamplingFunction, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr);

		/**
		 * Replaces this frame pyramid based on a new frame.
		 * The function will re-used the existing pyramid's memory if possible.<br>
		 * The resulting pyramid will re-used the given frame (as finest pyramid layer); thus, ensure that the frame's memory is valid as long as this pyramid exists.
		 * @param frame The frame for which the pyramid will be created, must be valid
		 * @param downsamplingFunction The custom function used to downsample the individual pyramid layers, must be valid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if the frame pyramid was replaced
		 * @see replace8BitPerChannel11().
		 */
		bool replace(Frame&& frame, const CallbackDownsampling& downsamplingFunction, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr);

		/**
		 * Deprecated.
		 *
		 * Replaces this frame pyramid by a new frame.
		 * The frame pyramid needs to be the owner of its frame data or needs to be empty; otherwise, nothing is done and False is returned.
		 * Beware: Due to performance issues the new frame should have the same frame type as already defined in this pyramid.<br>
		 * Further, the number of layers to be created should be the same as already defined
		 * Beware: This non-template-based function will result in larger binary code, use the corresponding template-based function if the size of the resulting binary matters.
		 * @param frame The frame to be used to update this pyramid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param downsamplingMode The down sampling mode to be used to create lower image resolutions, 'DM_CUSTOM' in combination with a valid 'customDownsamplingFunction' to use a custom down sampling mode
		 * @param customDownsamplingFunction Optional explicit custom callback of the down sampling function to be used, define an invalid callback object to use the default down sampling function determined by 'mode'
		 * @return True, if the frame pyramid was replaced
		 * @see isOwner(), replace8BitPerChannel().
		 */
		bool replace(const Frame& frame, const unsigned int layers, Worker* worker = nullptr, const DownsamplingMode downsamplingMode = DM_FILTER_11, const CallbackDownsampling& customDownsamplingFunction = CallbackDownsampling());

		/**
		 * Deprecated.
		 *
		 * Replaces this frame pyramid by a new frame.
		 * The frame pyramid needs to be the owner of its frame data or needs to be empty; otherwise, nothing is done and False is returned.
		 * Beware: Due to performance issues the new frame should have the same frame type as already defined in this pyramid.<br>
		 * Further, the number of layers to be created should be the same as already defined
		 * Beware: This non-template-based function will result in larger binary code, use the corresponding template-based function if the size of the resulting binary matters.
		 * @param frame The frame to be used to update this pyramid
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param downsamplingMode The down sampling mode to be used to create lower image resolutions, 'DM_CUSTOM' in combination with a valid 'customDownsamplingFunction' to use a custom down sampling mode
		 * @param customDownsamplingFunction Optional explicit custom callback of the down sampling function to be used, define an invalid callback object to use the default down sampling function determined by 'mode'
		 * @return True, if the frame pyramid was replaced
		 * @see isOwner(), replace8BitPerChannel().
		 */
		bool replace(const LegacyFrame& frame, const unsigned int layers, Worker* worker = nullptr, const DownsamplingMode downsamplingMode = DM_FILTER_11, const CallbackDownsampling& customDownsamplingFunction = CallbackDownsampling());

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
		inline bool replace(const FrameType& frameType, const bool forceOwner, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE);

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
		 * Returns whether this pyramid does not hold any frame layer.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Assigns a frame pyramid to this one.
		 * @param right The right pyramid to be assigned
		 * @return Reference to this object
		 */
		FramePyramid& operator=(const FramePyramid& right);

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
		inline const LegacyFrame& operator[](const unsigned int layer) const;

		/**
		 * Returns the frame of a specified layer.
		 * Beware: The frame will not be the owner of the frame data, if you need a copy of this frame enforce to copy the frame buffer!
		 * @param layer Index of the layer frame to be returned, with range [0, layers())
		 * @return Pyramid layer frame
		 */
		inline LegacyFrame& operator[](const unsigned int layer);

		/**
		 * Returns whether this pyramid holds at least one frame layer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Creates a new frame pyramid by using layers from an existing source frame pyramid.
		 * The range of used layers can be specified by layerIndex and layerCount.<br>
		 * If tCopyData is False, then only a reference is created to the given source frame data; the new frame pyramid will be a read-only pyramid.<br>
		 * If tCopyData is True and the source pyramid has less layers then specified, the additional layers are generated; the resulting frame pyramid will be a writable pyramid.<br>
		 * In any case, the source pyramid is not modified.<br>
		 * Beware: This function can be used instead of the corresponding constructor if the size of the resulting binary matters.<br>
		 * As this function is more restrictive compared to the corresponding constructor (and does not allow to apply a blur filter) the resulting code is significantly smaller.
		 * @param framePyramid Source frame pyramid, This frame pyramid needs to have at least one valid layer
		 * @param firstLayerIndex Index of first layer to use from the source pyramid. The index needs to be in the range [0; source layers], where source layers is the number of valid layers in the specified source pyramid
		 * @param layerCount The number of layers to use from the source pyramid, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tCopyData If true, then the used frame data of the source pyramid is copied. Otherwise, only a reference to the used frame data of the source pyramid is created
		 */
		template <bool tCopyData>
		static FramePyramid create8BitPerChannel(const FramePyramid& framePyramid, const unsigned int firstLayerIndex = 0u, const unsigned int layerCount = AS_MANY_LAYERS_AS_POSSIBLE, Worker* worker = nullptr);

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
		 * @param invalidWidthOrHeight Width or the height of a coarse layer not valid any more, with range (0, infinity)
		 * @return Resulting layers so that the invalid frame size will not be reached, with range [1, infinity), 0 if 'width' or 'height' are smaller or equal than 'invalidWidthOrHeight'
		 */
		static unsigned int idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidthOrHeight);

		/**
		 * Determines the number of layers until an invalid frame size would be reached in the next layer.
		 * @param width The width of the finest layer in pixel, with range [1, infinity)
		 * @param height The height of the finest layer in pixel, with range [1, infinity)
		 * @param invalidWidth Width of a coarse layer not valid any more, with range (0, infinity)
		 * @param invalidHeight Height of a coarse layer not valid any more, with range (0, infinity)
		 * @return Resulting layers so that the invalid frame size will not be reached, with range [1, infinity), 0 if 'width/height' is smaller or equal than 'invalidWidth/invalidHeight'
		 */
		static unsigned int idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidth, const unsigned int invalidHeight);

		/**
		 * Determines the number of layers until an invalid frame size would be reached in the next layer or an overall size radius is reached.
		 * @param width The width of the finest layer in pixel
		 * @param height The height of the finest layer in pixel
		 * @param invalidWidth Width of a coarse layer not valid any more, with range (0, infinity)
		 * @param invalidHeight Height of a coarse layer not valid any more, with range (0, infinity)
		 * @param layerFactor Size factor on each layer (a factor of 2 means that the layer dimension is halved on each layer), with range [2, infinity)
		 * @param maximalRadius Maximal radius that can be reached on the finest layer by starting with coarsestLayerRadius on the coarsest layer in pixel (the maximal expected baseline between two pixels in the finest pyramid layer), with range [1, infinity)
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @return Resulting layers so that the invalid frame size will not be reached, with range [1, infinity), 0 if 'width/height' is smaller or equal than 'invalidWidth/invalidHeight'
		 */
		static unsigned int idealLayers(const unsigned int width, const unsigned int height, const unsigned int invalidWidth, const unsigned int invalidHeight, const unsigned int layerFactor, const unsigned int maximalRadius = (unsigned int)(-1), const unsigned int coarsestLayerRadius = 2u);

	protected:

		/**
		 * Disabled constructor.
		 */
		explicit FramePyramid(Frame&&) = delete;

		/**
		 * Deprecated.
		 *
		 * Disabled constructor.
		 */
		explicit FramePyramid(LegacyFrame&&) = delete;

		/**
		 * Disabled constructor.
		 */
		FramePyramid(const Frame&, const bool) = delete;

		/**
		 * Deprecated.
		 *
		 * Disabled constructor.
		 */
		FramePyramid(const LegacyFrame&, const bool) = delete;

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
		 * Adds a new layer to the end of the frame pyramid.
		 * The image content of the new layer is derived from the coarsest existing layer.
		 * @param worker Optional worker object to distribute the computation
		 * @param downsamplingMode The down sampling mode to be used to create lower image resolutions, 'DM_CUSTOM' in combination with a valid 'customDownsamplingFunction' to use a custom down sampling mode
		 * @param customDownsamplingFunction Optional custom callback of the down sampling function to be used, if mode == DM_CUSTOM
		 */
		bool addLayer(Worker* worker = nullptr, const DownsamplingMode downsamplingMode = DM_FILTER_11, const CallbackDownsampling& customDownsamplingFunction = CallbackDownsampling());

		/**
		 * Adds a new layer to the end of the frame pyramid.
		 * The image content of the new layer is derived from the coarsest existing layer.
		 * @param worker Optional worker object to distribute the computation
		 */
		bool addLayer11(Worker* worker = nullptr);

		/**
		 * Calculates the size of the entire pyramid in bytes covering all images in all pyramid layers.
		 * @param width The width of the finest layer in pixel, with range [0, 65535]
		 * @param height The height of the finest layer in pixel, with range [0, 65535]
		 * @param pixelFormat Pixel format of each layer
		 * @param layers Number of layers, with range [0, infinity)
		 * @param includeFirstLayer True, to determine the memory for all layers; False, to skip the first layer and only determine the memory for all remaining (coarser) layers
		 * @param totalLayers Optional resulting number of pyramid layers that will exist (always counts the very first layer independently of 'includeFirstLayer'), with range [0, layers]
		 * @return Resulting number of bytes, with range [0, infinity)
		 */
		static size_t calculateMemorySize(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const unsigned int layers, const bool includeFirstLayer, unsigned int* totalLayers);

		/**
		 * Deleted function to prevent confusion between Frame and FrameType.
		 * @param frame The potential frame to be used
		 * @param layers The number of layers
		 * @param forceOwner The ownership information
		 * @return True, if succeeded
		 */
		bool replace(const Frame& frame, const bool forceOwner, const unsigned int layers = AS_MANY_LAYERS_AS_POSSIBLE) = delete;

	protected:

		/// Layers of this pyramid.
		LegacyFrames layers_;

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

inline FramePyramid::FramePyramid(const Frame& frame, const unsigned int layers, Worker* worker, const DownsamplingMode downsamplingMode, const CallbackDownsampling& customDownsamplingFunction)
{
	replace(frame, layers, worker, downsamplingMode, customDownsamplingFunction);
}

inline const LegacyFrame& FramePyramid::layer(const unsigned int layer) const
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline LegacyFrame& FramePyramid::layer(const unsigned int layer)
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline const LegacyFrame& FramePyramid::finestLayer() const
{
	ocean_assert(isValid());
	return layers_.front();
}

inline LegacyFrame& FramePyramid::finestLayer()
{
	ocean_assert(isValid());
	return layers_.front();
}

inline const LegacyFrame& FramePyramid::coarsestLayer() const
{
	ocean_assert(isValid());

	return layers_.back();
}

inline LegacyFrame& FramePyramid::coarsestLayer()
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

template <bool tCopyData>
FramePyramid FramePyramid::create8BitPerChannel(const FramePyramid& framePyramid, const unsigned int layerIndex, const unsigned int layerCount, Worker* worker)
{
	ocean_assert(framePyramid.layers() >= 0u);
	ocean_assert(layerIndex < framePyramid.layers());
	ocean_assert(layerCount >= 0u);

	const LegacyFrame& firstLayer = framePyramid.layers_[layerIndex];
	const unsigned int maxUsableSourceLayers = framePyramid.layers() - layerIndex;

	FramePyramid newPyramid;

	if constexpr (tCopyData)
	{
		unsigned int resultingLayers;
		const size_t pyramidFrameSize = calculateMemorySize(firstLayer.width(), firstLayer.height(), firstLayer.pixelFormat(), layerCount, true /*includeFirstLayer*/, &resultingLayers);

		if (pyramidFrameSize == 0)
		{
			return FramePyramid();
		}

		unsigned int selectedSourceLayers = min(resultingLayers, maxUsableSourceLayers);

		const LegacyFrame& lastLayer = framePyramid.layers_[layerIndex + selectedSourceLayers - 1u];
		const size_t sizeSelectedSourceLayers = size_t(lastLayer.constdata() - firstLayer.constdata()) + lastLayer.size();

		ocean_assert(framePyramid.memory_.isInside(firstLayer.constdata(), firstLayer.size()));
		ocean_assert(framePyramid.memory_.isInside(lastLayer.constdata(), lastLayer.size()));
		ocean_assert((lastLayer.constdata() == firstLayer.constdata() && lastLayer.size() == firstLayer.size()) || lastLayer.constdata() >= firstLayer.constdata() + firstLayer.size());

		newPyramid.memory_ = Memory(pyramidFrameSize, memoryAlignmentBytes_);
		memcpy(newPyramid.memory_.data(), firstLayer.constdata(), sizeSelectedSourceLayers);

		// Create layers from source:
		newPyramid.layers_.reserve(resultingLayers);
		newPyramid.layers_.push_back(LegacyFrame(firstLayer.frameType(), firstLayer.timestamp(), newPyramid.memory_.data<uint8_t>(), false));

		for (unsigned int index = 1u; index < selectedSourceLayers; index++)
		{
			const LegacyFrame& sourceLayer = framePyramid.layers_[index + layerIndex];
			LegacyFrame& previousLayer = newPyramid.layers_[index - 1u];

			newPyramid.layers_.push_back(LegacyFrame(sourceLayer.frameType(), sourceLayer.timestamp(), previousLayer.data() + previousLayer.size(), false));
		}

		// Create additional missing layers, if frame data is not a reference:
		if (resultingLayers > newPyramid.layers_.size())
		{
			// Pyramid frame needs to hold requested number of layers:
			ocean_assert(newPyramid.memory_.size() >= calculateMemorySize(newPyramid.finestWidth(), newPyramid.finestHeight(), newPyramid.finestLayer().pixelFormat(), resultingLayers, true /*includeFirstLayer*/, nullptr));

			for (unsigned int n = (unsigned int)(newPyramid.layers_.size()); n < resultingLayers && newPyramid.layers_[n - 1u].width() > 1u && newPyramid.layers_[n - 1u].height() > 1u; ++n)
			{
				ocean_assert(n == (unsigned int)newPyramid.layers_.size());
				if (!newPyramid.addLayer11(worker))
				{
					break;
				}
			}
		}
	}
	else
	{
		const unsigned int selectedSourceLayers = min(layerCount, maxUsableSourceLayers);
		const LegacyFrame& lastLayer = framePyramid.layers_[layerIndex + selectedSourceLayers - 1u];

		ocean_assert(framePyramid.memory_.isInside(firstLayer.constdata(), firstLayer.size()));
		ocean_assert(framePyramid.memory_.isInside(lastLayer.constdata(), lastLayer.size()));
		ocean_assert((lastLayer.constdata() == firstLayer.constdata() && lastLayer.size() == firstLayer.size()) || lastLayer.constdata() >= firstLayer.constdata() + firstLayer.size());

		ocean_assert(lastLayer.constdata() - firstLayer.constdata() >= 0);
		newPyramid.memory_ = Memory(firstLayer.constdata(), lastLayer.constdata() - firstLayer.constdata() + lastLayer.size());

		newPyramid.layers_.reserve(selectedSourceLayers);
		newPyramid.layers_.push_back(LegacyFrame(firstLayer.frameType(), firstLayer.timestamp(), firstLayer.constdata(), false));

		for (unsigned int index = 1u; index < selectedSourceLayers; index++)
		{
			const LegacyFrame& sourceLayer = framePyramid.layers_[index + layerIndex];
			const LegacyFrame& previousLayer = newPyramid.layers_[index - 1u];

			newPyramid.layers_.push_back(LegacyFrame(sourceLayer.frameType(), sourceLayer.timestamp(), previousLayer.constdata() + previousLayer.size(), false));
		}
	}

	return newPyramid;
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

inline const LegacyFrame& FramePyramid::operator[](const unsigned int layer) const
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline LegacyFrame& FramePyramid::operator[](const unsigned int layer)
{
	ocean_assert(layer < layers_.size());
	return layers_[layer];
}

inline bool FramePyramid::isValid() const
{
	return !layers_.empty();
}

inline bool FramePyramid::isNull() const
{
	return layers_.empty();
}

inline FramePyramid::operator bool() const
{
	return isValid();
}

}

}

#endif // META_OCEAN_CV_FRAME_PYRAMID_H
