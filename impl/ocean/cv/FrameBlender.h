/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_BLENDER_H
#define META_OCEAN_CV_FRAME_BLENDER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a frame blender using an alpha channel to blend frames.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameBlender
{
	public:

		/**
		 * Definition of individual target alpha channel modulation functions.
		 */
		enum AlphaTargetModulation
		{
			/// Undefined target alpha channel modulation.
			ATM_UNDEFINED = 0,
			/// The target alpha channel is constant and is not changed.
			ATM_CONSTANT,
			/// The target alpha channel is blended by alphaResult = alphaTarget + alphaSource * (1 - alphaTarget).
			ATM_BLEND
		};

		/**
		 * Definition of a blend function allowing to apply a more complex blend operation than a simple linear blending for one pixel (and thus for all channels of this pixel).
		 * If e.g., the target frame holds 3 channels, than all three channels have to be handled by one call of the blend function.<br>
		 * The first parameter provides the target pixel
		 * The second parameter provides the target blend factor, with range [0, 255]
		 */
		typedef void (*BlendFunction)(uint8_t*, const uint8_t);

		/**
		 * Helper class allowing to determine the offset that is necessary to access the alpha channel.
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 */
		template <bool tAlphaAtFront>
		class SourceOffset
		{
			public:

				/**
				 * Returns the offset that is applied to access the alpha channel.
				 * @return Offset for the alpha channel
				 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel)
				 */
				template <unsigned int tChannelsWithAlpha>
				static constexpr unsigned int alpha();

				/**
				 * Returns the offset that is applied to access the first data channel.
				 * @return Offset for the first data channel
				 */
				static constexpr unsigned int data();

				/**
				 * Returns the offset that is applied to access the first data channel.
				 * @return Offset for the first data channel
				 */
				template <bool tSourceHasAlpha>
				static constexpr unsigned int data();
		};

		/**
		 * Helper class allowing to determine the number of channels of the target frame.
		 * @tparam tTargetHasAlpha True, if the target frame holds an alpha channel
		 */
		template <bool tTargetHasAlpha>
		class TargetOffset
		{
			public:

				/**
				 * Returns the number of channels of the target frame.
				 * @return Number of target frame channels
				 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel)
				 */
				template <unsigned int tChannelsWithAlpha>
				static constexpr unsigned int channels();

				/**
				 * Returns the offset that is applied to access the first data channel.
				 * @return Offset for the first data channel
				 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
				 */
				template <bool tAlphaAtFront>
				static constexpr unsigned int data();
		};

		/**
		 * Helper class allowing to determine the number of channels of a frame.
		 * @tparam tHasAlpha True, if the frame holds an alpha channel
		 */
		template <bool tHasAlpha>
		class FrameChannels
		{
			public:

				/**
				 * Returns the number of channels of a frame not counting the possible alpha channel.
				 * @return Number of channels without the possible alpha channel
				 * @tparam tChannels The number of channels of a frame including a possible alpha channel
				 */
				template <unsigned int tChannels>
				static constexpr unsigned int dataChannels();

				/**
				 * Returns the number of channels of a frame for that is known whether is has an alpha channel or not.
				 * @return The number of channels of a frame
				 * @tparam tChannelsWithAlpha The number of channels that include an alpha channel, with range [2, infinity)
				 */
				template <unsigned int tChannelsWithAlpha>
				static constexpr unsigned int channels();
		};

	public:

		/**
		 * Blends two frames with same frame type by application of one unique blending factor for all pixels.
		 * The blend function is defined as follows:
		 * <pre>
		 * tTransparentIs0xFF == true:     targetPixel = sourcePixel * (0xFF - alpha)  +   targetPixel *     alpha
		 * tTransparentIs0xFF == false:    targetPixel = sourcePixel *     alpha       +   targetPixel * (0xFF - alpha)
		 * </pre>
		 * @param source The source frame that is blended with the target frame, must be valid
		 * @param target The target frame that receives the blending result from source and target pixels, this frame must have the same frame format as the source frame, must be valid
		 * @param alphaValue Blending factor for all pixels, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool blend(const Frame& source, Frame& target, const uint8_t alphaValue, Worker* worker = nullptr);

		/**
		 * Blends an entire source frame holding an alpha channel with a target frame without an alpha channel.
		 * An alpha value of 0xFF may specifies a fully transparent source pixel or may specify a fully opaque source pixel.<br>
		 * Both frames must have the same frame dimension and pixel origin.<br>
		 * Further, the pixel format of the source frame must match to the pixel format of the target frame.<br>
		 * Valid combinations of pixel formats are e.g. (FORMAT_RGBA32, FORMAT_RGBA32) or (FORMAT_RGBA32, FORMAT_RGB24) or (FORMAT_ABGR32, FORMAT_ABGR32) or (FORMAT_ABGR32, FORMAT_BGR24).<br>
		 * @param sourceWithAlpha Source frame with alpha channel
		 * @param target The target frame with or without alpha channel, if this frame has an alpha channel this channel will be untouched
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value
		 * @tparam tAlphaTargetModulation The target alpha channel modulation function
		 */
		template <bool tTransparentIs0xFF, AlphaTargetModulation tAlphaTargetModulation>
		static bool blend(const Frame& sourceWithAlpha, Frame& target, Worker* worker = nullptr);

		/**
		 * Blends an entire source frame holding an alpha channel with a static background color and creates a new frame without alpha channel.
		 * An alpha value of 0xFF may specifies a fully transparent source pixel or may specify a fully opaque source pixel.<br>
		 * @param sourceWithAlpha Source frame with alpha channel
		 * @param result The resulting frame
		 * @param backgroundColor The static background color for the resulting frame, ensure that the given buffer is large enough for the resulting pixel format (the source pixel format without alpha channel)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value
		 * @tparam tAlphaTargetModulation The target alpha channel modulation function
		 */
		template <bool tTransparentIs0xFF, AlphaTargetModulation tAlphaTargetModulation>
		static bool blend(const Frame& sourceWithAlpha, Frame& result, const uint8_t* backgroundColor, Worker* worker = nullptr);

		/**
		 * Blends a sub-region of a source frame with a target frame without an alpha channel.
		 * An alpha value of 0xFF may specifies a fully transparent source pixel or may specify a fully opaque source pixel.<br>
		 * The pixel format of the source frame must match to the pixel format of the target frame.<br>
		 * Valid combinations of pixel formats are e.g. (FORMAT_RGBA32, FORMAT_RGBA32) or (FORMAT_RGBA32, FORMAT_RGB24) or (FORMAT_ABGR32, FORMAT_ABGR32) or (FORMAT_ABGR32, FORMAT_BGR24).
		 * @param sourceWithAlpha Source frame with alpha channel, must be valid
		 * @param target The target frame with or without alpha channel, if this frame has an alpha channel this channel will be untouched, must be valid
		 * @param sourceLeft Horizontal start position inside the source frame, with range [0, sourceWidth)
		 * @param sourceTop Vertical start position inside the source frame, with range [0, sourceHeight)
		 * @param targetLeft Horizontal start position inside the target frame, with range [0, targetWidth)
		 * @param targetTop Vertical start position inside the target frame, with range [0, targetHeight)
		 * @param width The width of the blending sub-region in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param height The height of the blending sub-region in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 * @tparam tAlphaTargetModulation The target alpha channel modulation function
		 */
		template <bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
		static bool blend(const Frame& sourceWithAlpha, Frame& target, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Blends two 8 bit per channel frames with same frame type by application of one unique blending factor for all pixels.
		 * The blend function is defined as follows:
		 * <pre>
		 * tTransparentIs0xFF == true:     targetPixel = sourcePixel * (0xFF - alpha)  +   targetPixel *     alpha
		 * tTransparentIs0xFF == false:    targetPixel = sourcePixel *     alpha       +   targetPixel * (0xFF - alpha)
		 * </pre>
		 * @param source The source frame that is blended with the target frame, must be valid
		 * @param target The target frame that receives the blending result from source and target pixels, this frame must have the same frame format as the source frame, must be valid
		 * @param alphaValue The blending factor for all pixels, with range [0, 255]
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param height The height of the both frames in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of channels of both frames (without any alpha channel), with range [1, infinity)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent; False, if 0x00 is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static inline void blend8BitPerChannel(const uint8_t* source, uint8_t* target, const uint8_t alphaValue, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Blends an entire source and target frame while the alpha channel is provided in a third frame.
		 * <pre>
		 * tTransparentIs0xFF == true:     targetPixel = sourcePixel * (0xFF - alphaPixel)  +   targetPixel *     alphaPixel
		 * tTransparentIs0xFF == false:    targetPixel = sourcePixel *     alphaPixel       +   targetPixel * (0xFF - alphaPixel)
		 * </pre>
		 * @param source The source frame without alpha channel, which is blended with the target frame, must be valid
		 * @param alpha The alpha frame with defining one alpha blending factor for each pixel, must be valid
		 * @param target The target frame without alpha channel, must be valid
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param height The height of the both frames in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param alphaPaddingElements The optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of channels of both frames (without any alpha channel), with range [1, infinity)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent; False, if 0x00 is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static inline void blend8BitPerChannel(const uint8_t* source, const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Blends an entire source and target frame while the alpha channel is in front of the data channels or behind the data channels.
		 * @param sourceWithAlpha Source frame with alpha channel, must be valid
		 * @param target The target frame which may also hold an alpha channel depending on tTargetHasAlpha, must be valid
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param height The height of both frames in pixel, with range [1, infinity)
		 * @param sourceWithAlphaPaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTargetHasAlpha True, if not only the source frame holds an alpha channel but also the target frame, however values in a target alpha channel will be untouched
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 * @tparam tAlphaTargetModulation The target alpha channel modulation function
		 */
		template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tTargetHasAlpha, bool tTransparentIs0xFF, AlphaTargetModulation tAlphaTargetModulation>
		static inline void blend8BitPerChannel(const uint8_t* sourceWithAlpha, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourceWithAlphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Blends a target frame with a specified constant value for all pixels, while each pixel might have a different blending factor.
		 * @param alpha The alpha frame defining the value blending factor that is applied for each pixel, must be valid
		 * @param target The target frame without alpha channel that will be blended with the constant value parameter, must be valid
		 * @param width The width of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param height The height of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param value Constant blending value that is used for each pixel, must be valid
		 * @param alphaPaddingElements The number of padding elements at the end of each alpha frame row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static inline void blend8BitPerChannel(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* value, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Blends a target frame with a specified blending function, while each pixel might have a different blending factor.
		 * @param alpha The alpha frame defining the target blending factor that is applied for each pixel, must be valid
		 * @param target The target frame without alpha channel that will be blended with the constant value parameter, must be valid
		 * @param width The width of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param height The height of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param blendFunction Blending function allowing to apply a more complex blending function than a simple linear blending, the blend function will receive 0x00 for opaque pixels and 0xFF for transparent pixels as blend factor
		 * @param alphaPaddingElements The number of padding elements at the end of each alpha frame row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static inline void blend8BitPerChannel(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const BlendFunction blendFunction, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Blends a sub-region of a source frame with a target frame while the alpha channel is provided in a third frame with frame size identical to the source frame.
		 * @param source The source frame without alpha channel, must be valid
		 * @param alpha The alpha frame defining the alpha channel connected with the source frame, must be valid
		 * @param target The target frame without alpha channel, must be valid
		 * @param sourceWidth Width of the source (and alpha) frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source (and alpha) frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceLeft Horizontal start position inside the source frame, with range [0, sourceWidth)
		 * @param sourceTop Vertical start position inside the source frame, with range [0, sourceHeight)
		 * @param targetLeft Horizontal start position inside the target frame, with range [0, targetWidth)
		 * @param targetTop Vertical start position inside the target frame, with range [0, targetHeight)
		 * @param width The width of the blending sub-region in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param height The height of the blending sub-region in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements The number of padding elements at the end of each row in the source frame, in elements, with range [0, infinity)
		 * @param alphaPaddingElements The number of padding elements at the end of each row in the alpha frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row in the target frame, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static inline void blend8BitPerChannel(const uint8_t* source, const uint8_t* alpha, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Blends a sub-region of a source frame with a target frame while the alpha channel is in front of the data channels or behind the data channels.
		 * @param sourceWithAlpha Source frame with alpha channel, must be valid
		 * @param target The target frame which may also hold an alpha channel depending on tTargetHasAlpha, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceLeft Horizontal start position inside the source frame, with range [0, sourceWidth)
		 * @param sourceTop Vertical start position inside the source frame, with range [0, sourceHeight)
		 * @param targetLeft Horizontal start position inside the target frame, with range [0, targetWidth)
		 * @param targetTop Vertical start position inside the target frame, with range [0, targetHeight)
		 * @param width The width of the blending sub-region in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param height The height of the blending sub-region in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements The number of padding elements at the end of each row in the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row in the target frame, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTargetHasAlpha True, if not only the source frame holds an alpha channel but also the target frame, however values in a target alpha channel will be untouched
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 * @tparam tAlphaTargetModulation The target alpha channel modulation function
		 */
		template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tTargetHasAlpha, bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
		static inline void blend8BitPerChannel(const uint8_t* sourceWithAlpha, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Returns the alpha value for a fully transparent pixel.
		 * @return Fully transparent alpha value
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static inline uint8_t fullTransparent8Bit();

		/**
		 * Returns the alpha value for a fully opaque pixel.
		 * @return Fully opaque alpha value
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static inline uint8_t fullOpaque8Bit();

		/**
		 * Converts a given alpha value so that 0xFF is interpreted as fully transparent.
		 * @param alpha The alpha value that will be converted
		 * @tparam tTransparentIs0xFF True, if the given alpha is defined so that 0xFF is interpreted as fully transparent
		 * @return Resulting converted alpha value
		 */
		template <bool tTransparentIs0xFF>
		static inline uint8_t alpha8BitToTransparentIs0xFF(const uint8_t alpha);

		/**
		 * Converts a given alpha value so that 0xFF is interpreted as fully opaque.
		 * @param alpha The alpha value that will be converted
		 * @tparam tTransparentIs0xFF True, if the given alpha is defined so that 0xFF is interpreted as fully transparent
		 * @return Resulting converted alpha value
		 */
		template <bool tTransparentIs0xFF>
		static inline uint8_t alpha8BitToOpaqueIs0xFF(const uint8_t alpha);

	protected:

		/**
		 * Blends a subset of two 8 bit per channels frames by application of one unique blending factor for all pixels.
		 * @param source The source frame that is blended with the target frame, must be valid
		 * @param target The target frame that receives the blending result from source and target pixels, this frame must have the same frame format as the source frame, must be valid
		 * @param alphaValue The blending factor for all pixels, with range [0, 255]
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel), with range [1, infinity)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent; False, if 0x00 is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static void blend8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const uint8_t alphaValue, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Blends a subset of a source and target frame (each 8 bit per channel) while the alpha channel is provided as a third frame.
		 * @param source The source frame that is blended with the target frame, must be valid
		 * @param alpha The alpha frame with defining one alpha blending factor for each pixel, must be valid
		 * @param target The target frame that receives the blending result from source and target pixels, this frame must have the same frame format as the source frame, must be valid
		 * @param width The width of all frames, in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param alphaPaddingElements The optional number of padding elements at the end of each alpha row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel), with range [1, infinity)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent; False, if 0x00 is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static void blend8BitPerChannelSubset(const uint8_t* source, const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Blends a subset of a source and target frame while the alpha channel is in front of the data channels or behind the data channels.
		 * @param sourceWithAlpha Source frame with alpha channel, must be valid
		 * @param target The target frame which may also hold an alpha channel depending on tTargetHasAlpha, must be valid
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param height The height of both frames in pixel, with range [1, infinity)
		 * @param sourceWithAlphaPaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel), with range [2, infinity)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTargetHasAlpha True, if not only the source frame holds an alpha channel but also the target frame, however values in a target alpha channel will be untouched
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 * @tparam tAlphaTargetModulation The target alpha channel modulation function
		 */
		template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tTargetHasAlpha, bool tTransparentIs0xFF, AlphaTargetModulation tAlphaTargetModulation>
		static void blend8BitPerChannelSubset(const uint8_t* sourceWithAlpha, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourceWithAlphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Blends a subset of a target frame with a specified constant value for all pixels, while each pixel might have a different blending factor.
		 * @param alpha The alpha frame defining the value blending factor that is applied for each pixel
		 * @param target The target frame without alpha channel that will be blended with the constant value parameter
		 * @param width The width of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param height The height of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param value Constant blending value that is used for each pixel
		 * @param alphaPaddingElements The number of padding elements at the end of each alpha frame row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel), with range [1, infinity)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static void blend8BitPerChannelSubset(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* value, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Blends a subset of a target frame with a specified blending function, while each pixel might have a different blending factor.
		 * @param alpha The alpha frame defining the target blending factor that is applied for each pixel
		 * @param target The target frame without alpha channel that will be blended with the constant value parameter
		 * @param width The width of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param height The height of the target (and alpha) frame in pixel, with range [1, infinity)
		 * @param blendFunction Blending function allowing to apply a more complex blending function than a simple linear blending, the blend function will receive 0x00 for opaque pixels and 0xFF for transparent pixels as blend factor
		 * @param alphaPaddingElements The optional number of padding elements at the end of each alpha row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row of the sub-region to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows of the sub-region to be handled, with range [1, height - firstRow]
		 * @tparam tChannels Number of channels of both data frame (without any alpha channel), with range [1, infinity)
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tTransparentIs0xFF>
		static void blend8BitPerChannelSubset(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const BlendFunction blendFunction, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Returns the blend factor for the source frame depending whether 0xFF is interpreted as transparent or opaque.
		 * @param alpha The alpha value for which the blending factor is returned, with range [0, 255]
		 * @return Resulting blending factor, with range [0, 255]
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 * @see targetBlendFactor().
		 */
		template <bool tTransparentIs0xFF>
		static uint8_t inline sourceBlendFactor(const uint8_t alpha);

		/**
		 * Returns the blend factor for the target frame depending whether 0xFF is interpreted as transparent or opaque.
		 * @param alpha The alpha value for which the blending factor is returned, with range [0, 255]
		 * @return Resulting blending factor, with range [0, 255]
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 * @see sourceBlendFactor().
		 */
		template <bool tTransparentIs0xFF>
		static inline uint8_t targetBlendFactor(const uint8_t alpha);
};

template <bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
bool FrameBlender::blend(const Frame& sourceWithAlpha, Frame& target, Worker* worker)
{
	if (!sourceWithAlpha.isValid() || !target.isValid() || sourceWithAlpha.width() != target.width() || sourceWithAlpha.height() != target.height()
		|| sourceWithAlpha.pixelOrigin() != target.pixelOrigin()
		|| (sourceWithAlpha.pixelFormat() != target.pixelFormat() && FrameType::formatRemoveAlphaChannel(sourceWithAlpha.pixelFormat()) != target.pixelFormat()))
	{
		ocean_assert(false && "Invalid pixel format!");
		return false;
	}

	if (sourceWithAlpha.numberPlanes() == 1u && sourceWithAlpha.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		bool alphaIsLastChannel = false;

		const bool sourceHasAlpha = FrameType::formatHasAlphaChannel(sourceWithAlpha.pixelFormat(), &alphaIsLastChannel);
		ocean_assert_and_suppress_unused(sourceHasAlpha, sourceHasAlpha);

		const bool targetHasAlpha = sourceWithAlpha.pixelFormat() == target.pixelFormat();

		switch (sourceWithAlpha.channels())
		{
			case 2u:
			{
				if (alphaIsLastChannel)
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<2u, false, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<2u, false, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}
				else
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<2u, true, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<2u, true, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}

				return true;
			}

			case 3u:
			{
				if (alphaIsLastChannel)
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<3u, false, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<3u, false, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}
				else
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<3u, true, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<3u, true, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}

				return true;
			}

			case 4u:
			{
				if (alphaIsLastChannel)
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<4u, false, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<4u, false, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(),sourceWithAlpha.paddingElements(), target.paddingElements(),  worker);
					}
				}
				else
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<4u, true, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<4u, true, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}

				return true;
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

template <bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
bool FrameBlender::blend(const Frame& sourceWithAlpha, Frame& result, const uint8_t* backgroundColor, Worker* worker)
{
	if (!result.set(FrameType(sourceWithAlpha, FrameType::formatRemoveAlphaChannel(sourceWithAlpha.pixelFormat())), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	result.setValue<uint8_t>(backgroundColor, result.channels(), 0u);

	return blend<tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha, result, worker);
}

template <bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
bool FrameBlender::blend(const Frame& sourceWithAlpha, Frame& target, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, Worker* worker)
{
	if (!sourceWithAlpha.isValid() || !target.isValid()
		|| sourceWithAlpha.pixelOrigin() != target.pixelOrigin()
		|| (sourceWithAlpha.pixelFormat() != target.pixelFormat() && FrameType::formatRemoveAlphaChannel(sourceWithAlpha.pixelFormat()) != target.pixelFormat())
		|| sourceLeft + width > sourceWithAlpha.width() || sourceTop + height > sourceWithAlpha.height()
		|| targetLeft + width > target.width() || targetTop + height > target.height())
	{
		ocean_assert(false && "Invalid pixel format!");
		return false;
	}

	if (sourceWithAlpha.numberPlanes() == 1u && sourceWithAlpha.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		bool alphaIsLastChannel = false;

		const bool hasAlpha = FrameType::formatHasAlphaChannel(sourceWithAlpha.pixelFormat(), &alphaIsLastChannel);
		ocean_assert_and_suppress_unused(hasAlpha, hasAlpha);

		const bool targetHasAlpha = sourceWithAlpha.pixelFormat() == target.pixelFormat();

		switch (sourceWithAlpha.channels())
		{
			case 2u:
			{
				if (alphaIsLastChannel)
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<2u, false, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<2u, false, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}
				else
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<2u, true, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<2u, true, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}

				return true;
			}

			case 3u:
			{
				if (alphaIsLastChannel)
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<3u, false, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<3u, false, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}
				else
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<3u, true, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<3u, true, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}

				return true;
			}

			case 4u:
			{
				if (alphaIsLastChannel)
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<4u, false, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<4u, false, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}
				else
				{
					if (targetHasAlpha)
					{
						blend8BitPerChannel<4u, true, true, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
					else
					{
						blend8BitPerChannel<4u, true, false, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha.constdata<uint8_t>(), target.data<uint8_t>(), sourceWithAlpha.width(), sourceWithAlpha.height(), target.width(), target.height(), sourceLeft, sourceTop, targetLeft, targetTop, width, height, sourceWithAlpha.paddingElements(), target.paddingElements(), worker);
					}
				}

				return true;
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

template <bool tTransparentIs0xFF>
bool FrameBlender::blend(const Frame& source, Frame& target, const uint8_t alphaValue, Worker* worker)
{
	ocean_assert(source && target);

	if (FrameType::areFrameTypesCompatible(source, target, false /*allowDifferentPixelOrigins*/)
		&& source.numberPlanes() == 1u
		&& source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (source.channels())
		{
			case 1u:
				blend8BitPerChannel<1u, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), alphaValue, source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 2u:
				blend8BitPerChannel<2u, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), alphaValue, source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 3u:
				blend8BitPerChannel<3u, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), alphaValue, source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 4u:
				blend8BitPerChannel<4u, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), alphaValue, source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* source, uint8_t* target, const uint8_t alphaValue, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>, source,  target, alphaValue, width,  sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 6u, 7u, 20u);
	}
	else
	{
		blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>(source, target, alphaValue, width,  sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* source, const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && alpha != nullptr && target != nullptr);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>, source, alpha, target, width, sourcePaddingElements, alphaPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>(source, alpha, target, width, sourcePaddingElements, alphaPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tTargetHasAlpha, bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* sourceWithAlpha, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourceWithAlphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannelsWithAlpha > 1u, "Invalid channel number!");
	ocean_assert(sourceWithAlpha && target);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&blend8BitPerChannelSubset<tChannelsWithAlpha, tAlphaAtFront, tTargetHasAlpha, tTransparentIs0xFF, tAlphaTargetModulation>, sourceWithAlpha, target, width, height, sourceWithAlphaPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 6u, 7u, 20u);
	}
	else
	{
		blend8BitPerChannelSubset<tChannelsWithAlpha, tAlphaAtFront, tTargetHasAlpha, tTransparentIs0xFF, tAlphaTargetModulation>(sourceWithAlpha, target, width, height, sourceWithAlphaPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* value, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(alpha != nullptr && target != nullptr && value != nullptr);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>, alpha, target, width, height, value, alphaPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>(alpha, target, width, height, value, alphaPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const BlendFunction blendFunction, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	ocean_assert(alpha && target && blendFunction);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>, alpha, target, width, height, blendFunction, alphaPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		blend8BitPerChannelSubset<tChannels, tTransparentIs0xFF>(alpha, target, width, height, blendFunction, alphaPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* source, const uint8_t* alpha, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && alpha != nullptr && target != nullptr);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int alphaStrideElements = sourceWidth + alphaPaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const uint8_t* const sourceTopLeft = source + sourceTop * sourceStrideElements + sourceLeft * tChannels;
	const uint8_t* const alphaTopLeft = alpha + sourceTop * alphaStrideElements + sourceLeft;
	uint8_t* const targetTopLeft = target + targetTop * targetStrideElements + targetLeft * tChannels;

	ocean_assert_and_suppress_unused(width <= sourceWidth && height <= sourceHeight, sourceHeight);
	ocean_assert_and_suppress_unused(width <= targetWidth && height <= targetHeight, targetHeight);

	const unsigned int subSourcePaddingElements = (sourceWidth - width) * tChannels + sourcePaddingElements;
	const unsigned int subAlphaPaddingElements = (sourceWidth - width) + alphaPaddingElements;
	const unsigned int subTargetPaddingElements = (targetWidth - width) * tChannels + targetPaddingElements;

	blend8BitPerChannel<tChannels, tTransparentIs0xFF>(sourceTopLeft, alphaTopLeft, targetTopLeft, width, height, subSourcePaddingElements, subAlphaPaddingElements, subTargetPaddingElements, worker);
}

template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tTargetHasAlpha, bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
inline void FrameBlender::blend8BitPerChannel(const uint8_t* sourceWithAlpha, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannelsWithAlpha > 1u, "Invalid channel number!");

	ocean_assert(sourceWithAlpha != nullptr && target != nullptr);

	constexpr unsigned int tTargetChannels = TargetOffset<tTargetHasAlpha>::template channels<tChannelsWithAlpha>();

	const unsigned int sourceStrideElements = sourceWidth * tChannelsWithAlpha + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tTargetChannels + targetPaddingElements;

	const uint8_t* const sourceTopLeft = sourceWithAlpha + sourceTop * sourceStrideElements + sourceLeft * tChannelsWithAlpha;
	uint8_t* const targetTopLeft = target + targetTop * targetStrideElements + targetLeft * tTargetChannels;

	ocean_assert_and_suppress_unused(width <= sourceWidth && height <= sourceHeight, sourceHeight);
	ocean_assert_and_suppress_unused(width <= targetWidth && height <= targetHeight, targetHeight);

	const unsigned int subSourcePaddingElements = (sourceWidth - width) * tChannelsWithAlpha + sourcePaddingElements;
	const unsigned int subTargetPaddingElements = (targetWidth - width) * tTargetChannels + targetPaddingElements;

	blend8BitPerChannel<tChannelsWithAlpha, tAlphaAtFront, tTargetHasAlpha, tTransparentIs0xFF, tAlphaTargetModulation>(sourceTopLeft, targetTopLeft, width, height, subSourcePaddingElements, subTargetPaddingElements, worker);
}

template <bool tTransparentIs0xFF>
inline uint8_t FrameBlender::fullTransparent8Bit()
{
	return 0xFF;
}

template <>
inline uint8_t FrameBlender::fullTransparent8Bit<false>()
{
	return 0x00;
}

template <bool tTransparentIs0xFF>
inline uint8_t FrameBlender::fullOpaque8Bit()
{
	return 0xFF - fullTransparent8Bit<tTransparentIs0xFF>();
}

template <bool tTransparentIs0xFF>
inline uint8_t FrameBlender::alpha8BitToTransparentIs0xFF(const uint8_t alpha)
{
	return alpha;
}

template <>
inline uint8_t FrameBlender::alpha8BitToTransparentIs0xFF<false>(const uint8_t alpha)
{
	return 0xFF - alpha;
}

template <bool tTransparentIs0xFF>
inline uint8_t FrameBlender::alpha8BitToOpaqueIs0xFF(const uint8_t alpha)
{
	return 0xFF - alpha;
}

template <>
inline uint8_t FrameBlender::alpha8BitToOpaqueIs0xFF<false>(const uint8_t alpha)
{
	return alpha;
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
void FrameBlender::blend8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const uint8_t alphaValue, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);

	const uint8_t sourceFactor = sourceBlendFactor<tTransparentIs0xFF>(alphaValue);
	const uint8_t targetFactor = targetBlendFactor<tTransparentIs0xFF>(alphaValue);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int r = 0u; r < numberRows; ++r)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = Ocean::Utilities::divideBy255(target[n] * targetFactor + source[n] * sourceFactor + 127u);
			}

			source += tChannels;
			target += tChannels;
		}

		source += sourcePaddingElements;
		target += targetPaddingElements;
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
void FrameBlender::blend8BitPerChannelSubset(const uint8_t* source, const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && alpha != nullptr && target != nullptr);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int alphaStrideElements = width + alphaPaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	source += firstRow * sourceStrideElements;
	alpha += firstRow * alphaStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int r = 0u; r < numberRows; ++r)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const uint8_t sourceFactor = sourceBlendFactor<tTransparentIs0xFF>(alpha[0]);
			const uint8_t targetFactor = targetBlendFactor<tTransparentIs0xFF>(alpha[0]);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = Ocean::Utilities::divideBy255(target[n] * targetFactor + source[n] * sourceFactor + 127u);
			}

			source += tChannels;
			++alpha;
			target += tChannels;
		}

		source += sourcePaddingElements;
		alpha += alphaPaddingElements;
		target += targetPaddingElements;
	}
}

template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tTargetHasAlpha, bool tTransparentIs0xFF, FrameBlender::AlphaTargetModulation tAlphaTargetModulation>
void FrameBlender::blend8BitPerChannelSubset(const uint8_t* sourceWithAlpha, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourceWithAlphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannelsWithAlpha > 1u, "Invalid channel number!");
	ocean_assert(sourceWithAlpha != nullptr && target != nullptr);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceWithAlphaStrideElements = width * tChannelsWithAlpha + sourceWithAlphaPaddingElements;
	const unsigned int targetStrideElements = width * TargetOffset<tTargetHasAlpha>::template channels<tChannelsWithAlpha>() + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* sourceWithAlphaData = sourceWithAlpha + y * sourceWithAlphaStrideElements;
		uint8_t* targetData = target + y * targetStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			const uint8_t sourceFactor = sourceBlendFactor<tTransparentIs0xFF>(sourceWithAlphaData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()]);
			const uint8_t targetFactor = targetBlendFactor<tTransparentIs0xFF>(sourceWithAlphaData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()]);

			for (unsigned int n = 0u; n < tChannelsWithAlpha - 1u; ++n)
			{
				targetData[n + TargetOffset<tTargetHasAlpha>::template data<tAlphaAtFront>()] = Ocean::Utilities::divideBy255(targetData[n + TargetOffset<tTargetHasAlpha>::template data<tAlphaAtFront>()] * targetFactor + sourceWithAlphaData[n + SourceOffset<tAlphaAtFront>::data()] * sourceFactor + 127u);
			}

			if constexpr (tTargetHasAlpha && tAlphaTargetModulation == ATM_BLEND)
			{
				if constexpr (tTransparentIs0xFF)
				{
					targetData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()] =
							Ocean::Utilities::divideBy255(targetData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()] * sourceWithAlphaData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()]);
				}
				else
				{
					// alphaResult = alphaTarget + alphaSource * (1 - alphaTarget), while 0xFF is interpreted as fully opaque
					// alphaResult = (alphaTarget * 255 + alphaSource * (255 - alphaTarget)) / 255, while 0xFF is interpreted as fully opaque

					targetData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()] =
							Ocean::Utilities::divideBy255(255u * targetData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()]
							+ sourceWithAlphaData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()] * (255u - targetData[SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()]));
				}
			}

			sourceWithAlphaData += tChannelsWithAlpha;
			targetData += TargetOffset<tTargetHasAlpha>::template channels<tChannelsWithAlpha>();
		}
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
void FrameBlender::blend8BitPerChannelSubset(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* value, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(alpha != nullptr && target != nullptr && value != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int alphaStrideElements = width + alphaPaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* alphaRow = alpha + y * alphaStrideElements;
		uint8_t* targetRow = target + y * targetStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			const uint8_t valueFactor = sourceBlendFactor<tTransparentIs0xFF>(*alphaRow);
			const uint8_t targetFactor = targetBlendFactor<tTransparentIs0xFF>(*alphaRow);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				targetRow[n] = Ocean::Utilities::divideBy255(targetRow[n] * targetFactor + value[n] * valueFactor + 127u);
			}

			++alphaRow;
			targetRow += tChannels;
		}
	}
}

template <unsigned int tChannels, bool tTransparentIs0xFF>
void FrameBlender::blend8BitPerChannelSubset(const uint8_t* alpha, uint8_t* target, const unsigned int width, const unsigned int height, const BlendFunction blendFunction, const unsigned int alphaPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(alpha != nullptr && target != nullptr && blendFunction != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int alphaStrideElements = width + alphaPaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	alpha += firstRow * alphaStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int n = 0u; n < numberRows; ++n)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			blendFunction(target, targetBlendFactor<tTransparentIs0xFF>(*alpha));

			++alpha;
			target += tChannels;
		}

		alpha += alphaPaddingElements;
		target += targetPaddingElements;
	}
}

template <bool tTransparentIs0xFF>
inline uint8_t FrameBlender::sourceBlendFactor(const uint8_t alpha)
{
	return 0xFFu - alpha;
}

template <>
inline uint8_t FrameBlender::sourceBlendFactor<false>(const uint8_t alpha)
{
	return alpha;
}

template <bool tTransparentIs0xFF>
inline uint8_t FrameBlender::targetBlendFactor(const uint8_t alpha)
{
	return alpha;
}

template <>
inline uint8_t FrameBlender::targetBlendFactor<false>(const uint8_t alpha)
{
	return 0xFFu - alpha;
}

template <bool tAlphaAtFront>
template <unsigned int tChannelsWithAlpha>
constexpr inline unsigned int FrameBlender::SourceOffset<tAlphaAtFront>::alpha()
{
	return 0u;
}

template <>
template <unsigned int tChannelsWithAlpha>
constexpr inline unsigned int FrameBlender::SourceOffset<false>::alpha()
{
	static_assert(tChannelsWithAlpha >= 1u, "Invalid channel number!");

	return tChannelsWithAlpha - 1u;
}

template <bool tAlphaAtFront>
constexpr inline unsigned int FrameBlender::SourceOffset<tAlphaAtFront>::data()
{
	return 1u;
}

template <>
constexpr inline unsigned int FrameBlender::SourceOffset<false>::data()
{
	return 0u;
}

template <bool tAlphaAtFront>
template <bool tSourceHasAlpha>
constexpr inline unsigned int FrameBlender::SourceOffset<tAlphaAtFront>::data()
{
	return tSourceHasAlpha ? 1u : 0u;
}

template <>
template <>
constexpr inline unsigned int FrameBlender::SourceOffset<false>::data<true>()
{
	return 0u;
}

template <>
template <>
constexpr inline unsigned int FrameBlender::SourceOffset<false>::data<false>()
{
	return 0u;
}

template <bool tTargetHasAlpha>
template <unsigned int tChannelsWithAlpha>
constexpr inline unsigned int FrameBlender::TargetOffset<tTargetHasAlpha>::channels()
{
	static_assert(tChannelsWithAlpha >= 2u, "Invalid channel input!");

	return tChannelsWithAlpha;
}

template <>
template <unsigned int tChannelsWithAlpha>
constexpr inline unsigned int FrameBlender::TargetOffset<false>::channels()
{
	static_assert(tChannelsWithAlpha >= 2u, "Invalid channel number!");

	return tChannelsWithAlpha - 1u;
}

template <bool tTargetHasAlpha>
template <bool tAlphaAtFront>
constexpr inline unsigned int FrameBlender::TargetOffset<tTargetHasAlpha>::data()
{
	return 0u;
}

template <>
template <>
constexpr inline unsigned int FrameBlender::TargetOffset<true>::data<true>()
{
	return 1u;
}

template <bool tHasAlpha>
template <unsigned int tChannels>
constexpr inline unsigned int FrameBlender::FrameChannels<tHasAlpha>::dataChannels()
{
	static_assert(tChannels >= 2u, "Invalid channel input!");

	// we have a frame with alpha channel
	return tChannels - 1u;
}

template <>
template <unsigned int tChannels>
constexpr inline unsigned int FrameBlender::FrameChannels<false>::dataChannels()
{
	static_assert(tChannels >= 1u, "Invalid channel input!");

	// we have a frame without alpha channel
	return tChannels;
}

template <bool tHasAlpha>
template <unsigned int tChannelsWithAlpha>
constexpr inline unsigned int FrameBlender::FrameChannels<tHasAlpha>::channels()
{
	static_assert(tChannelsWithAlpha >= 2u, "Invalid channel input!");

	// we have a frame with alpha channel
	return tChannelsWithAlpha;
}

template <>
template <unsigned int tChannelsWithAlpha>
constexpr inline unsigned int FrameBlender::FrameChannels<false>::channels()
{
	static_assert(tChannelsWithAlpha >= 2u, "Invalid channel input!");

	// we have a frame without alpha channel
	return tChannelsWithAlpha - 1u;
}

}

}

#endif // META_OCEAN_CV_FRAME_BLENDER_H
