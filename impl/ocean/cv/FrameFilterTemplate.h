/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_TEMPLATE_H
#define META_OCEAN_CV_FRAME_FILTER_TEMPLATE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a 3x3 filter with arbitrary filter pattern.
 * The filter layout is specified by nine filter factor and one normalization parameter as follows:
 * <pre>
 * | tFactor00   tFactor01   tFactor02 |
 * | tFactor10   tFactor11   tFactor12 |
 * | tFactor20   tFactor21   tFactor22 |
 * </pre>
 * The filter layout is defined for the default pixel direction PixelDirection::PD_NORTH.<br>
 * The pixel direction PixelDirection::PD_WEST applies the filter layout with a counter clockwise rotation of 90 degrees.<br>
 * The pixel direction PixelDirection::PD_EAST applies the filter layout with a clockwise rotation of 90 degrees, and so on.
 * @tparam TFactor Data type of the filter factors
 * @tparam tFactor00 Filter factor (0, 0)
 * @tparam tFactor10 Filter factor (1, 0)
 * @tparam tFactor20 Filter factor (2, 0)
 * @tparam tFactor01 Filter factor (0, 1)
 * @tparam tFactor11 Filter factor (1, 1)
 * @tparam tFactor21 Filter factor (2, 1)
 * @tparam tFactor02 Filter factor (0, 2)
 * @tparam tFactor12 Filter factor (1, 2)
 * @tparam tFactor22 Filter factor (2, 2)
 * @ingroup cv
 */
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
class FrameFilterTemplate
{
	public:

		/**
		 * Definition of the filter factor data type.
		 */
		typedef TFactor Type;

	public:

		/**
		 * Applies the filter at one pixel position in a frame with zipped data channels (for one of the channels).
		 * This function returns zero for frame border pixels.
		 * @param frame The frame in that the filter will be applied
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x Horizontal center pixel position of the filter, with range [0, width)
		 * @param y Vertical center pixel position of the filter, width range [0, height)
		 * @return Resulting filter response
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannel The index of the channel for which the filter response is determined, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannel, unsigned int tChannels, PixelDirection tDirection>
		static TResponse filterPixel(const TData* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y);

		/**
		 * Applies the filter at one center pixel position in a frame with zipped data channels (for one of the channels).
		 * This function must not be applied at the frame border.
		 * @param frame Center pixel position in the frame that will be filtered, this pointer must not point to a border pixel of the frame
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @return Resulting filter response
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannel The index of the channel for which the filter response is determined, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannel, unsigned int tChannels, PixelDirection tDirection>
		static TResponse filterPixelCore(const TData* frame, const unsigned int width);

		/**
		 * Applies the filter at one pixel position in a frame with zipped data channels.
		 * This function returns zero for frame border pixels.
		 * @param frame The frame in that the filter will be applied
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x Horizontal center pixel position of the filter, with range [0, width)
		 * @param y Vertical center pixel position of the filter, width range [0, height)
		 * @param response Resulting filter responses, one for each channel
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels, PixelDirection tDirection>
		static void filterPixel(const TData* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, TResponse* response);

		/**
		 * Applies the filter at one center pixel position in a frame with zipped data channels.
		 * This function must not be applied at the frame border.
		 * @param frame Center pixel position in the frame that will be filtered, this pointer must not point to a border pixel of the frame
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param response Resulting filter responses, one for each channel
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels, PixelDirection tDirection>
		static void filterPixelCore(const TData* frame, const unsigned int width, TResponse* response);

		/**
		 * Applies the filter at one pixel position in a frame with zipped data channels.
		 * The resulting filter response is the maximal absolute response of all given channels, thus for each pixel one filter response is provided only.<br>
		 * This function returns zero for frame border pixels.
		 * @param frame The frame in that the filter will be applied
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param x Horizontal center pixel position of the filter, with range [0, width)
		 * @param y Vertical center pixel position of the filter, with range [0, height)
		 * @return Resulting filter response
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response, should be unsigned as e.g., signed integer values can have no corresponding abs signed integer (like -128, -32768)
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels, PixelDirection tDirection>
		static TResponse filterAbsoluteAs1ChannelPixel(const TData* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y);

		/**
		 * Applies the filter at one center pixel position in a frame with zipped data channels.
		 * The resulting filter response is the maximal absolute response of all given channels, thus for each pixel one filter response is provided only.<br>
		 * This function must not be applied at the frame border.
		 * @param frame Center pixel position in the frame that will be filtered, this pointer must not point to a border pixel of the frame
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @return Resulting filter response
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response, should be unsigned as e.g., signed integer values can have no corresponding abs signed integer (like -128, -32768)
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels, PixelDirection tDirection>
		static TResponse filterAbsoluteAs1ChannelPixelCore(const TData* frame, const unsigned int width);

		/**
		 * Applies the filter for a given frame with several zipped data channels and normalizes the filter response by a denominator defined as template parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param channels The number of data channels of the frame, with range [1, 4]
		 * @param direction Filter direction that is applied
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias>
		static void filter(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const unsigned int channels, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies the filter for a given frame with several zipped data channels and normalizes the filter response by a denominator defined as template parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param direction Filter direction that is applied
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels>
		static void filter(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies the filter for a given frame with several zipped data channels and normalizes the filter response by a denominator defined as template parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels, PixelDirection tDirection>
		static inline void filter(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies the filter for a given frame with several zipped data channels and normalizes the filter response by a multiplication factor (not a denominator but a nominator) defined as function parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param factor The normalization factor which will be multiplied with the filter response (it's not a denominator but a nominator)
		 * @param channels The number of data channels of the frame, with range [1, 4]
		 * @param direction Filter direction that is applied
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalizationFactor Data type of the normalization factor (the nominator value)
		 */
		template <typename TData, typename TResponse, typename TNormalizationFactor>
		static void filterWithFactor(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const unsigned int channels, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies the filter for a given frame with several zipped data channels and normalizes the filter response by a multiplication factor (not a denominator but a nominator) defined as function parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param factor The normalization factor which will be multiplied with the filter response (it's not a denominator but a nominator)
		 * @param direction Filter direction that is applied
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalizationFactor Data type of the normalization factor (the nominator value)
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 */
		template <typename TData, typename TResponse, typename TNormalizationFactor, unsigned int tChannels>
		static void filterWithFactor(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies the filter for a given frame with several zipped data channels and normalizes the filter response by a multiplication factor (not a denominator but a nominator) defined as function parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param factor The normalization factor which will be multiplied with the filter response (it's not a denominator but a nominator)
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalizationFactor Data type of the normalization factor (the nominator value)
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalizationFactor, unsigned int tChannels, PixelDirection tDirection>
		static inline void filterWithFactor(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * This functions fills an array with the filter factors of this template class.
		 * @param filterFactors The resulting filter factors, with order tFactor00, tFactor10, tFactor20, tFactor01, and so on
		 */
		static void copyFilterFactors(TFactor filterFactors[9]);

	protected:

		/**
		 * Applies the filter in a subset of a given frame with several zipped data channels and normalizes the filter response by a denominator defined as template parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height)
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalization Data type of the normalization factor (the denominator value)
		 * @tparam tNormalization Denominator value that normalizes the filter response by division, the filter response will be divided by tNormalization, with range (-infinity, infinity) \ {0}
		 * @tparam tNormalizationBias An explicit bias value which will be added to the filter response before the normalization is applied (this bias allows to improve the integer rounding value), 0 to avoid any round optimization, e.g., use tNormalizationBias=2 for tNormlization=4 or  use tNormalizationBias=8 for tNormlization=16
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tNormalizationBias, unsigned int tChannels, PixelDirection tDirection>
		static void filterSubset(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies the filter in a subset of a given frame with several zipped data channels and normalizes the filter response by a multiplication factor (not a denominator but a nominator) defined as function parameter.
		 * @param frame The frame that will be filtered
		 * @param target The target buffer receiving the filter responses, one response for each channel and pixel
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param factor The normalization factor which will be multiplied with the filter response (it's not a denominator but a nominator)
		 * @param framePaddingElements Optional padding at the end of each frame row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height)
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TData Data type of the given frame
		 * @tparam TResponse Data type of the resulting filter response
		 * @tparam TNormalizationFactor Data type of the normalization factor (the nominator value)
		 * @tparam tChannels Number of data channels of the frame, with range [1, infinity)
		 * @tparam tDirection Filter direction that is applied
		 */
		template <typename TData, typename TResponse, typename TNormalizationFactor, unsigned int tChannels, PixelDirection tDirection>
		static void filterWithFactorSubset(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Returns the filter factor of filter element (0, 0).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor00();

		/**
		 * Returns the filter factor of filter element (1, 0).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor10();

		/**
		 * Returns the filter factor of filter element (2, 0).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor20();

		/**
		 * Returns the filter factor of filter element (0, 1).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor01();

		/**
		 * Returns the filter factor of filter element (1, 1).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor11();

		/**
		 * Returns the filter factor of filter element (2, 1).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor21();

		/**
		 * Returns the filter factor of filter element (0, 2).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor02();

		/**
		 * Returns the filter factor of filter element (1, 2).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor12();

		/**
		 * Returns the filter factor of filter element (2, 2).
		 * @return Resulting filter factor depending on the filter direction
		 * @tparam tDirection Filter direction for that the filter factor is requested
		 */
		template <PixelDirection tDirection>
		static inline TFactor factor22();
};

/**
 * This class implements a helper class providing template filter factors.
 * @tparam tDirection Direction of the filter
 * @ingroup cv
 */
template <PixelDirection tDirection>
class FrameFilterFactorProvider
{
	public:

		/**
		 * Returns the filter factor of the filter element (0, 0)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor00();

		/**
		 * Returns the filter factor of the filter element (1, 0)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor10();

		/**
		 * Returns the filter factor of the filter element (2, 0)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor20();

		/**
		 * Returns the filter factor of the filter element (0, 1)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor01();

		/**
		 * Returns the filter factor of the filter element (1, 1)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor11();

		/**
		 * Returns the filter factor of the filter element (2, 1)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor21();

		/**
		 * Returns the filter factor of the filter element (0, 2)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor02();

		/**
		 * Returns the filter factor of the filter element (1, 2)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor12();

		/**
		 * Returns the filter factor of the filter element (2, 2)
		 * @tparam TFactor Data type of the filter factors
		 * @tparam tFactor00 Filter factor (0, 0)
		 * @tparam tFactor10 Filter factor (1, 0)
		 * @tparam tFactor20 Filter factor (2, 0)
		 * @tparam tFactor01 Filter factor (0, 1)
		 * @tparam tFactor11 Filter factor (1, 1)
		 * @tparam tFactor21 Filter factor (2, 1)
		 * @tparam tFactor02 Filter factor (0, 2)
		 * @tparam tFactor12 Filter factor (1, 2)
		 * @tparam tFactor22 Filter factor (2, 2)
		 * @return Resulting filter factor depending on the filter direction
		 */
		template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
		static inline TFactor factor22();
};

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannel, unsigned int tChannels, PixelDirection tDirection>
TResponse FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterPixel(const TData* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
	static_assert(tNormalization != 0, "Invalid normalization parameter!");
	static_assert(tChannel < tChannels, "Invalid channel index!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u);
	ocean_assert(height >= 3u);

	ocean_assert((x - 1u < width - 2u) == (x >= 1u && x + 1u < width));
	ocean_assert((y - 1u < height - 2u) == (y >= 1u && y + 1u < height));

	if (x - 1u < width - 2u && y - 1u < height - 2u)
	{
		return filterPixelCore<TData, TResponse, TNormalization, tNormalization, tBias, tChannel, tChannels, tDirection>(frame + (y * width + x) * tChannels, width);
	}
	else
	{
		return TResponse(0);
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannel, unsigned int tChannels, PixelDirection tDirection>
TResponse FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterPixelCore(const TData* frame, const unsigned int width)
{
	static_assert(tNormalization != 0, "Invalid normalization parameter!");
	static_assert(tChannel < tChannels, "Invalid channel index!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u);

	const unsigned int cWidth = width * tChannels;

	return TResponse((TNormalization(*(frame - cWidth - tChannels + tChannel) * factor00<tDirection>() + *(frame - cWidth + tChannel) * factor01<tDirection>() + *(frame - cWidth + tChannels + tChannel) * factor02<tDirection>()
						+ *(frame - tChannels + tChannel) * factor10<tDirection>() + *(frame + tChannel) * factor11<tDirection>() + *(frame + tChannels + tChannel) * factor12<tDirection>()
						+ *(frame + cWidth - tChannels + tChannel) * factor20<tDirection>() + *(frame + cWidth + tChannel) * factor21<tDirection>() + *(frame + cWidth + tChannels + tChannel) * factor22<tDirection>()) + tBias) / tNormalization);
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, PixelDirection tDirection>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterPixel(const TData* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, TResponse* response)
{
	static_assert(tNormalization != 0, "Invalid normalization parameter!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u);
	ocean_assert(height >= 3u);

	ocean_assert((x - 1u < width - 2u) == (x >= 1u && x + 1u < width));
	ocean_assert((y - 1u < height - 2u) == (y >= 1u && y + 1u < height));

	if (x - 1u < width - 2u && y - 1u < height - 2u)
	{
		return filterPixelCore<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>(frame + (y * width + x) * tChannels, width, response);
	}
	else
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			response[n] = TResponse(0);
		}
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, PixelDirection tDirection>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterPixelCore(const TData* frame, const unsigned int width, TResponse* response)
{
	static_assert(tNormalization != 0, "Invalid normalization parameter!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u);

	const unsigned int cWidth = width * tChannels;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		response[n] = TResponse((TNormalization(*(frame - cWidth - tChannels + n) * factor00<tDirection>() + *(frame - cWidth + n) * factor01<tDirection>() + *(frame - cWidth + tChannels + n) * factor02<tDirection>()
							+ *(frame - tChannels + n) * factor10<tDirection>() + *(frame + n) * factor11<tDirection>() + *(frame + tChannels + n) * factor12<tDirection>()
							+ *(frame + cWidth - tChannels + n) * factor20<tDirection>() + *(frame + cWidth + n) * factor21<tDirection>() + *(frame + cWidth + tChannels + n) * factor22<tDirection>()) + tBias) / tNormalization);
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, PixelDirection tDirection>
TResponse FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterAbsoluteAs1ChannelPixel(const TData* frame, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y)
{
	static_assert(tNormalization != 0, "Invalid normalization parameter!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u);
	ocean_assert(height >= 3u);

	ocean_assert((x - 1u < width - 2u) == (x >= 1u && x + 1u < width));
	ocean_assert((y - 1u < height - 2u) == (y >= 1u && y + 1u < height));

	if (x - 1u < width - 2u && y - 1u < height - 2u)
	{
		return filterAbsoluteAs1ChannelPixelCore<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>(frame + (y * width + x) * tChannels, width);
	}
	else
	{
		return TResponse(0);
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, PixelDirection tDirection>
TResponse FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterAbsoluteAs1ChannelPixelCore(const TData* frame, const unsigned int width)
{
	static_assert(tNormalization != 0, "Invalid normalization parameter!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 3u);

	const unsigned int cWidth = width * tChannels;

	TResponse value = TResponse(NumericT<TNormalization>::secureAbs((TNormalization(*(frame - cWidth - tChannels) * factor00<tDirection>() + *(frame - cWidth) * factor01<tDirection>() + *(frame - cWidth + tChannels) * factor02<tDirection>()
						+ *(frame - tChannels) * factor10<tDirection>() + *(frame) * factor11<tDirection>() + *(frame + tChannels) * factor12<tDirection>()
						+ *(frame + cWidth - tChannels) * factor20<tDirection>() + *(frame + cWidth) * factor21<tDirection>() + *(frame + cWidth + tChannels) * factor22<tDirection>()) + tBias) / tNormalization));

	for (unsigned int n = 1u; n < tChannels; ++n)
	{
		TResponse newValue = TResponse(NumericT<TNormalization>::secureAbs((TNormalization(*(frame - cWidth - tChannels + n) * factor00<tDirection>() + *(frame - cWidth + n) * factor01<tDirection>() + *(frame - cWidth + tChannels + n) * factor02<tDirection>()
								+ *(frame - tChannels + n) * factor10<tDirection>() + *(frame + n) * factor11<tDirection>() + *(frame + tChannels + n) * factor12<tDirection>()
								+ *(frame + cWidth - tChannels + n) * factor20<tDirection>() + *(frame + cWidth + n) * factor21<tDirection>() + *(frame + cWidth + tChannels + n) * factor22<tDirection>()) + tBias) / tNormalization));

		if (newValue > value)
		{
			value = newValue;
		}
	}

	return value;
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filter(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const unsigned int channels, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	switch (channels)
	{
		case 1u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, 1u>(frame, target, width, height, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		case 2u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, 2u>(frame, target, width, height, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		case 3u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, 3u>(frame, target, width, height, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		case 4u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, 4u>(frame, target, width, height, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		default:
			ocean_assert(false && "Invalid channel number!");
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filter(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invaild channel number!");

	switch (direction)
	{
		case PD_NORTH:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_NORTH>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_NORTH_WEST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_NORTH_WEST>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_WEST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_WEST>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_SOUTH_WEST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_SOUTH_WEST>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_SOUTH:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_SOUTH>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_SOUTH_EAST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_SOUTH_EAST>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_EAST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_EAST>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_NORTH_EAST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filter<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, PD_NORTH_EAST>(frame, target, width, height, framePaddingElements, targetPaddingElements, worker);
			break;

		default:
			ocean_assert(false && "Invalid orientation!");
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, PixelDirection tDirection>
inline void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filter(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invaild channel number!");

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterSubset<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>, frame, target, width, height, framePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 6u, 7u, 20u);
	}
	else
	{
		filterSubset<TData, TResponse, TNormalization, tNormalization, tBias, tChannels, tDirection>(frame, target, width, height, framePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalizationFactor>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterWithFactor(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const unsigned int channels, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	switch (channels)
	{
		case 1u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, 1u>(frame, target, width, height, factor, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		case 2u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, 2u>(frame, target, width, height, factor, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		case 3u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, 3u>(frame, target, width, height, factor, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		case 4u:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, 4u>(frame, target, width, height, factor, direction, framePaddingElements, targetPaddingElements, worker);
			break;

		default:
			ocean_assert(false && "Invalid channel number!");
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalizationFactor, unsigned int tChannels>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterWithFactor(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const PixelDirection direction, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invaild channel number!");

	switch (direction)
	{
		case PD_NORTH:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_NORTH>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_NORTH_WEST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_NORTH_WEST>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_WEST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_WEST>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_SOUTH_WEST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_SOUTH_WEST>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_SOUTH:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_SOUTH>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_SOUTH_EAST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_SOUTH_EAST>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_EAST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_EAST>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		case PD_NORTH_EAST:
			CV::FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::template filterWithFactor<TData, TResponse, TNormalizationFactor, tChannels, PD_NORTH_EAST>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, worker);
			break;

		default:
			ocean_assert(false && "Invalid orientation!");
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalizationFactor, unsigned int tChannels, PixelDirection tDirection>
inline void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterWithFactor(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invaild channel number!");

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&filterWithFactorSubset<TData, TResponse, TNormalizationFactor, tChannels, tDirection>, frame, target, width, height, factor, framePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filterWithFactorSubset<TData, TResponse, TNormalizationFactor, tChannels, tDirection>(frame, target, width, height, factor, framePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::copyFilterFactors(TFactor filterFactors[9])
{
	filterFactors[0] = tFactor00;
	filterFactors[1] = tFactor10;
	filterFactors[2] = tFactor20;

	filterFactors[3] = tFactor01;
	filterFactors[4] = tFactor11;
	filterFactors[5] = tFactor21;

	filterFactors[6] = tFactor02;
	filterFactors[7] = tFactor12;
	filterFactors[8] = tFactor22;
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalization, TNormalization tNormalization, TNormalization tBias, unsigned int tChannels, PixelDirection tDirection>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterSubset(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invaild channel number!");

	ocean_assert(frame && target);
	ocean_assert(width >= 3u);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	// check whether the first row has set to zero
	if (firstRow == 0u)
	{
		for (unsigned int n = 0u; n < width * tChannels; ++n)
		{
			target[n] = TResponse(0);
		}
	}

	const unsigned int beginFilterRow = max(0, int(firstRow) - 1) + 1; // inclusive filter position
	const unsigned int endFilterRow = min(firstRow + numberRows + 1, height) - 1; // exclusive filter position

	frame += beginFilterRow * frameStrideElements;
	target += beginFilterRow * targetStrideElements;

	const TData* const frameEnd = frame + (endFilterRow - beginFilterRow) * frameStrideElements;

	while (frame != frameEnd)
	{
		ocean_assert(frame < frameEnd);

		// set first row pixel to zero
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			target[n] = 0;
		}

		frame += tChannels;
		target += tChannels;

		const TData* const frameRowEnd = frame + (width - 2u) * tChannels;

		while (frame != frameRowEnd)
		{
			ocean_assert(frame < frameEnd);
			ocean_assert(frame < frameRowEnd);

			const TData* const topRow = frame - frameStrideElements;
			const TData* const bottomRow = frame + frameStrideElements;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = TResponse((TNormalization(
										*(topRow - tChannels + n) * factor00<tDirection>()
										+ *(topRow + n) * factor01<tDirection>()
										+ *(topRow + tChannels + n) * factor02<tDirection>()
										+ *(frame - tChannels + n) * factor10<tDirection>()
										+ *(frame + n) * factor11<tDirection>()
										+ *(frame + tChannels + n) * factor12<tDirection>()
										+ *(bottomRow - tChannels + n) * factor20<tDirection>()
										+ *(bottomRow + n) * factor21<tDirection>()
										+ *(bottomRow + tChannels + n) * factor22<tDirection>()) + tBias) / tNormalization);
			}

			target += tChannels;
			frame += tChannels;
		}

		// set last row pixel to zero
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			target[n] = 0;
		}

		frame += tChannels;
		target += tChannels;

		frame += framePaddingElements;
		target += targetPaddingElements;
	}

	// check whether the last row has to to zero
	if (firstRow + numberRows == height)
	{
		for (unsigned int n = 0u; n < width * tChannels; ++n)
		{
			target[n] = TResponse(0);
		}
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <typename TData, typename TResponse, typename TNormalizationFactor, unsigned int tChannels, PixelDirection tDirection>
void FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::filterWithFactorSubset(const TData* frame, TResponse* target, const unsigned int width, const unsigned int height, const TNormalizationFactor factor, const unsigned int framePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invaild channel number!");

	ocean_assert(frame && target);
	ocean_assert(width >= 3u);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	// check whether the first row has set to zero
	if (firstRow == 0u)
	{
		for (unsigned int n = 0u; n < width * tChannels; ++n)
		{
			target[n] = TResponse(0);
		}
	}

	const unsigned int beginFilterRow = max(0, int(firstRow) - 1) + 1; // inclusive filter position
	const unsigned int endFilterRow = min(firstRow + numberRows + 1, height) - 1; // exclusive filter position

	frame += beginFilterRow * frameStrideElements;
	target += beginFilterRow * targetStrideElements;

	const TData* const frameEnd = frame + (endFilterRow - beginFilterRow) * frameStrideElements;

	while (frame != frameEnd)
	{
		ocean_assert(frame < frameEnd);

		// set first row pixel to zero
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			target[n] = 0;
		}

		frame += tChannels;
		target += tChannels;

		const TData* const frameRowEnd = frame + (width - 2u) * tChannels;

		while (frame != frameRowEnd)
		{
			ocean_assert(frame < frameEnd);
			ocean_assert(frame < frameRowEnd);

			const TData* const topRow = frame - frameStrideElements;
			const TData* const bottomRow = frame + frameStrideElements;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = TResponse(TNormalizationFactor(
								*(topRow - tChannels + n) * factor00<tDirection>()
								+ *(topRow + n) * factor01<tDirection>()
								+ *(topRow + tChannels + n) * factor02<tDirection>()
								+ *(frame - tChannels + n) * factor10<tDirection>()
								+ *(frame + n) * factor11<tDirection>()
								+ *(frame + tChannels + n) * factor12<tDirection>()
								+ *(bottomRow - tChannels + n) * factor20<tDirection>()
								+ *(bottomRow + n) * factor21<tDirection>()
								+ *(bottomRow + tChannels + n) * factor22<tDirection>()) * factor);
			}

			target += tChannels;
			frame += tChannels;
		}

		// set last row pixel to zero
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			target[n] = 0;
		}

		frame += tChannels;
		target += tChannels;

		frame += framePaddingElements;
		target += targetPaddingElements;
	}

	// check whether the last row has to to zero
	if (firstRow + numberRows == height)
	{
		for (unsigned int n = 0u; n < width * tChannels; ++n)
		{
			target[n] = TResponse(0);
		}
	}
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor00()
{
	return FrameFilterFactorProvider<tDirection>::template factor00<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor10()
{
	return FrameFilterFactorProvider<tDirection>::template factor10<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor20()
{
	return FrameFilterFactorProvider<tDirection>::template factor20<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor01()
{
	return FrameFilterFactorProvider<tDirection>::template factor01<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor11()
{
	return FrameFilterFactorProvider<tDirection>::template factor11<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor21()
{
	return FrameFilterFactorProvider<tDirection>::template factor21<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor02()
{
	return FrameFilterFactorProvider<tDirection>::template factor02<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor12()
{
	return FrameFilterFactorProvider<tDirection>::template factor12<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
template <PixelDirection tDirection>
inline TFactor FrameFilterTemplate<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>::factor22()
{
	return FrameFilterFactorProvider<tDirection>::template factor22<TFactor, tFactor00, tFactor10, tFactor20, tFactor01, tFactor11, tFactor21, tFactor02, tFactor12, tFactor22>();
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor00()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor00()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor00()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor00()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor00()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor00()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor00()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor00()
{
	return tFactor10;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor10()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor10()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor10()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor10()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor10()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor10()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor10()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor10()
{
	return tFactor20;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor20()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor20()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor20()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor20()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor20()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor20()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor20()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor20()
{
	return tFactor21;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor01()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor01()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor01()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor01()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor01()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor01()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor01()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor01()
{
	return tFactor00;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor11()
{
	return tFactor11;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor21()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor21()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor21()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor21()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor21()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor21()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor21()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor21()
{
	return tFactor22;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor02()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor02()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor02()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor02()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor02()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor02()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor02()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor02()
{
	return tFactor01;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor12()
{
	return tFactor12;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor12()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor12()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor12()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor12()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor12()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor12()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor12()
{
	return tFactor02;
}

template <PixelDirection tDirection>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<tDirection>::factor22()
{
	return tFactor22;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_WEST>::factor22()
{
	return tFactor21;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_WEST>::factor22()
{
	return tFactor20;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_WEST>::factor22()
{
	return tFactor10;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH>::factor22()
{
	return tFactor00;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_SOUTH_EAST>::factor22()
{
	return tFactor01;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_EAST>::factor22()
{
	return tFactor02;
}

template <>
template <typename TFactor, TFactor tFactor00, TFactor tFactor10, TFactor tFactor20, TFactor tFactor01, TFactor tFactor11, TFactor tFactor21, TFactor tFactor02, TFactor tFactor12, TFactor tFactor22>
inline TFactor FrameFilterFactorProvider<PD_NORTH_EAST>::factor22()
{
	return tFactor12;
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_TEMPLATE_H
