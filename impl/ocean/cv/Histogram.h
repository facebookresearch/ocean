/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_HISTOGRAM_H
#define META_OCEAN_CV_HISTOGRAM_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/Math.h"

#include <array>

namespace Ocean
{

namespace CV
{

/**
 * This class implements an image histogram.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT Histogram
{
	public:

		/**
		 * This class implements the base class for all histogram objects holding 8 bit per data channel.
		 * @tparam tChannels Defines the number of channels of the histogram
		 */
		template <unsigned int tChannels>
		class HistogramBase8BitPerChannel
		{
			public:

				/**
				 * Returns the bin value of a specific channel and bin.
				 * This function uses a template parameter to define the channel and the bin.<br>
				 * @tparam tChannel Data channel, with range [0, tChannels)
				 * @tparam tIndex Index of the bin
				 * @return Bin value
				 */
				template <unsigned int tChannel, uint8_t tIndex>
				inline unsigned int bin() const;

				/**
				 * Returns the bin value of a specific channel and bin.
				 * This function uses a template parameter to define the channel.<br>
				 * @param index The index of the bin, with range [0, 255]
				 * @tparam tChannel Data channel, with range [0, tChannels - 1]
				 * @return Bin value
				 */
				template <unsigned int tChannel>
				inline unsigned int bin(const uint8_t index) const;

				/**
				 * Returns the bin value of a specific channel and bin.
				 * @param channel Data channel, with range [0, tChannels)
				 * @param index The index of the bin, with range [0, 255]
				 * @return Bin value
				 */
				inline unsigned int bin(const unsigned int channel, const uint8_t index) const;

				/**
				 * Returns the sum of all channels stored for a specific bin.
				 * @param index The index of the bin, with range [0, 255]
				 * @return Sum of all channels
				 */
				inline unsigned int sumBin(const uint8_t index) const;

				/**
				 * Returns the 256 histogram values for a specific channel.
				 * @tparam tChannel Channel for that the histogram is requested, with range [0, tChannels)
				 * @return Channel histogram
				 */
				template <unsigned int tChannel>
				inline const unsigned int* bins() const;

				/**
				 * Returns the 256 histogram values for a specific channel.
				 * @param channel The channel for that the histogram is requested, with range [0, tChannels)
				 * @return Channel histogram
				 */
				inline const unsigned int* bins(const unsigned int channel) const;

				/**
				 * Determines the highest value within the histogram for a specific channel.
				 * @tparam tChannel Channel for that the highest value has to be found, with range [0, tChannels)
				 * @return Highest value inside the specified channel
				 */
				template <unsigned int tChannel>
				unsigned int determineHighestValue() const;

				/**
				 * Determines the highest value within the histogram for a specific channel.
				 * @param channel The channel for that the highest value has to be found, with range [0, tChannels)
				 * @return Highest value inside the specified channel
				 */
				unsigned int determineHighestValue(const unsigned int channel) const;

				/**
				 * Determines the highest value within the entire histogram.
				 * @return Highest histogram value
				 */
				unsigned int determineHighestValue() const;

				/**
				 * Determines the first bin that is not zero for a specific channel.
				 * @param channel Histogram channel in that the search is invoked, with range [0, tChannels)
				 * @return First non-zero bin, otherwise -1
				 */
				unsigned int determineStartBin(const unsigned int channel) const;

				/**
				 * Determines the last bin that is not zero for a specific channel.
				 * @param channel Histogram channel in that the search is invoked, with range [0, tChannels)
				 * @return Last non-zero bin, otherwise -1
				 */
				unsigned int determineEndBin(const unsigned int channel) const;

				/**
				 * Normalizes the entire histogram by application of the highest histogram value.
				 * @param newMaximalValue Normalization value that will be the maximal value inside the entire histogram after normalization
				 */
				inline void normalize(const unsigned int newMaximalValue);

				/**
				 * Normalizes one channel of the histogram by application of the highest value of the channel.
				 * @param channel Histogram channel that will be normalized, with range [0, tChannels)
				 * @param newMaximalValue Normalization value that will be the maximal value inside the histogram channel after normalization
				 */
				inline void normalize(const unsigned int channel, const unsigned int newMaximalValue);

				/**
				 * Clears the entire histogram and sets all bins to zero.
				 */
				void clear();

				/**
				 * Returns whether all bins inside the histogram are zero.
				 * @return True, if so
				 */
				inline bool isNull() const;

				/**
				 * Returns whether two histogram objects are identical according to their histogram bins.
				 * @param histogram Second histogram object
				 * @return True, if so
				 */
				inline bool operator==(const HistogramBase8BitPerChannel<tChannels>& histogram) const;

				/**
				 * Returns whether two histogram objects are not identical according to their histogram bins.
				 * @param histogram Second histogram object
				 * @return True, if so
				 */
				inline bool operator!=(const HistogramBase8BitPerChannel<tChannels>& histogram) const;

				/**
				 * Adds the histogram bins of a second histogram to this histogram.
				 * @param histogram Second histogram that is added to this one
				 * @return Reference to this histogram object
				 */
				HistogramBase8BitPerChannel<tChannels>& operator+=(const HistogramBase8BitPerChannel<tChannels>& histogram);

				/**
				 * Returns all histogram bins of this histogram.
				 * The bins are stored channel by channel (first the 256 bins of the first channel, than the next 256 bins of the next channel, ...)
				 * @return Histogram bins
				 */
				inline const unsigned int* operator()() const;

			protected:

				/**
				 * Creates an empty histogram object and sets all histogram bins to zero.
				 */
				inline HistogramBase8BitPerChannel();

				/**
				 * Explicitly sets a value of a specific histogram bin.
				 * @param channel Histogram channel for that the bin has to be set
				 * @param index The index of the bin to be set
				 * @param value Value to be set as new histogram bin value
				 */
				void setBin(const unsigned int channel, const uint8_t index, const unsigned int value);

			protected:

				/// The histogram bins.
				unsigned int histogramBins[256u * tChannels];
		};

		/**
		 * This class implements a standard histogram object storing 8 bit per channel.
		 * @tparam tChannels Defines the number of channels of the histogram
		 */
		template <unsigned int tChannels>
		class Histogram8BitPerChannel : public HistogramBase8BitPerChannel<tChannels>
		{
			public:

				/**
				 * Creates a new histogram object and sets all histogram bins to zero.
				 */
				inline Histogram8BitPerChannel();

				/**
				 * Increments a specific bin of this histogram (by one).
				 * @param index The index of the bin
				 * @tparam tChannel Histogram channel in that the bin is stored
				 */
				template <unsigned int tChannel>
				inline void incrementBin(const uint8_t index);

				/**
				 * Increments a specific bin of this histogram (by one).
				 * @param channel Histogram channel in that the bin is stored
				 * @param index The index of the bin
				 */
				inline void incrementBin(const unsigned int channel, const uint8_t index);

				/**
				 * Increments all channels of a specific histogram bin (by one).
				 * The given pixel value must provide as much channels as defined for this histogram (by tChannels).<br>
				 * @param pixel Pixel that is used to store all histogram channels
				 */
				inline void increment(const uint8_t* pixel);

				/**
				 * Adds to histogram objects and returns the new histogram.
				 * @param histogram Second histogram object that is used to create the new sum histogram
				 * @return Resulting new sum histogram object
				 */
				Histogram8BitPerChannel<tChannels> operator+(const Histogram8BitPerChannel<tChannels>& histogram) const;
		};

		/**
		 * This class implements an integral histogram object.
		 * This histogram holds the sum of all previous bins in each bin of the histogram.<br>
		 * @tparam tChannels Defines the number of channels of the histogram
		 */
		template <unsigned int tChannels>
		class IntegralHistogram8BitPerChannel : public HistogramBase8BitPerChannel<tChannels>
		{
			public:

				/**
				 * Creates a new integral histogram object and sets all histogram bins to zero.
				 */
				inline IntegralHistogram8BitPerChannel();

				/**
				 * Creates a new integral histogram by application of a standard histogram.
				 * The bins of the standard histogram are used to calculate the integral histogram bins.
				 * @param histogram Standard histogram that will be converted into this integral histogram object
				 */
				inline explicit IntegralHistogram8BitPerChannel(const Histogram8BitPerChannel<tChannels>& histogram);

				/**
				 * Inverts this (normalized) integral histogram so that the inverted histogram can be used as lookup object.
				 * Beware: This histogram has to be normalized to the maximal bin value of 0xFF before!<br>
				 * @return Inverted integral histogram of this object
				 * @see HistogramBase8BitPerChannel::normalize().
				 */
				IntegralHistogram8BitPerChannel invert() const;
		};

	private:

		/**
		 * This class implements a simple lookup table.
		 * @tparam tChannels Defines the number of channels of the table
		 */
		template <unsigned int tChannels>
		class LookupTable8BitPerChannel
		{
			public:

				/**
				 * Creates a default lookup table providing the identity lookup result.
				 */
				LookupTable8BitPerChannel();

				/**
				 * Creates a new lookup table by a given normalized integral histogram used as start position and an inverted normalized integral histogram used as end position.
				 * Beware: Both provided integral histograms have to be normalized to the maximal bin value of 0xFF before!<br>
				 * @param normalizedHistogram The normalized integral histogram that is used to create the intermediate lookup value
				 * @param invertedNormalizedHistogram The inverted normalized integral histogram that is used to translate the intermediate lookup value into the final lookup result
				 */
				LookupTable8BitPerChannel(const IntegralHistogram8BitPerChannel<tChannels>& normalizedHistogram, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedHistogram);

				/**
				 * Lookup function providing the lookup value for a specific channel and bin.
				 * @param index The index of the lookup bin
				 * @tparam tChannel Channel in that the lookup bin in located
				 * @return Lookup value
				 */
				template <unsigned int tChannel>
				uint8_t lookup(const uint8_t index) const;

				/**
				 * Lookup function providing the lookup value for a specific channel and bin.
				 * @param index The index of the lookup bin
				 * @param channel The channel in that the lookup bin in located
				 * @return Lookup value
				 */
				uint8_t lookup(const unsigned int channel, const uint8_t index) const;

			protected:

				/// The lookup data.
				uint8_t lookupData[256u * tChannels];
		};

	public:

		/**
		 * Determines the standard histogram for a given frame.
		 * @param frame The frame for that the histogram has to be determined
		 * @param width The width of the given frame in pixel
		 * @param height The height of the given frame in pixel
		 * @param framePaddingElements Optional number of padding elements of the input frame, range: [0, infinity)
		 * @param worker Optional worker object to distributed the computation
		 * @return Resulting standard histogram object
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static Histogram8BitPerChannel<tChannels> determineHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Determines the standard histogram in a sub region of a given frame.
		 * @param frame The frame for that the histogram has to be determined
		 * @param width The width of the given frame in pixel
		 * @param height The height of the given frame in pixel
		 * @param subframeLeft Horizontal start position of the sub region within the frame, in pixel with range [0, width)
		 * @param subframeTop Vertical start position of the sub region within the frame, in pixel with range [0, height)
		 * @param subframeWidth The width of the sub region, in pixel with range [1, width - subframeLeft]
		 * @param subframeHeight The height of the sub region, in pixel with range [1, height - subframeTop]
		 * @param worker Optional worker object to distributed the computation
		 * @param framePaddingElements Optional number of padding elements of the input frame, range: [0, infinity), default: 0
		 * @return Resulting standard histogram object
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static Histogram8BitPerChannel<tChannels> determineHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param frame The frame that will be equalized according the histogram
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool equalization(Frame& frame, const Scalar factor = 1, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param source The source frame for that an equalized target frame will be created
		 * @param target The target frame receiving the equalization result
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool equalization(const Frame& source, Frame& target, const Scalar factor = 1, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.<br>
		 * The frame types of the frame and the given reference frame must be identical.
		 * @param frame The frame for that the color values will be adjusted
		 * @param reference Reference frame providing the color statistics for the new frame
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool adjustColorToReference(Frame& frame, const Frame& reference, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame while using corresponding bins for the adjustments only (not the entire frame information).
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.<br>
		 * The frame types of the frame and the given reference frame must be identical.
		 * @param frame The frame for that the color values will be adjusted
		 * @param reference Reference frame providing the color statistics for the new frame
		 * @param horizontalBins The number of horizontal bins to use, with range [1, min(frame.width(), reference.with())]
		 * @param verticalBins The number of vertical bins to use, with range [1, min(frame.height(), reference.height())]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool adjustColorToReference(Frame& frame, const Frame& reference, const unsigned int horizontalBins, const unsigned int verticalBins, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame while using corresponding bins for the adjustments only (not the entire frame information).
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.<br>
		 * The frame types of the frame and the given reference frame must be identical.
		 * @param frame The frame for that the color values will be adjusted
		 * @param frameWidth The width of the frame in pixel, with range [1, infinity)
		 * @param frameHeight The height of the frame in pixel, with range [1, infinity)
		 * @param reference Reference frame providing the color statistics for the new frame
		 * @param referenceWidth The width of the reference frame in pixel, with range [1, infinity)
		 * @param referenceHeight The height of the reference frame in pixel, with range [1, infinity)
		 * @param horizontalBins The number of horizontal bins to use, with range [1, min(frameWidth, referenceWidth)]
		 * @param verticalBins The number of vertical bins to use, with range [1, min(frameHeight, referenceHeight)]
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool adjustColorToReference(uint8_t* frame, const unsigned int frameWidth, const unsigned int frameHeight, const uint8_t* reference, const unsigned int referenceWidth, const unsigned int referenceHeight, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int framePaddingElements, const unsigned int referencePaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * The frame types of the source frame and the given reference frame must be identical.<br>
		 * @param source The source frame that is used to create the target frame
		 * @param target The target frame with adjusted color information
		 * @param reference Reference frame providing the color statistics for the new frame
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool adjustColorToReference(const Frame& source, Frame& target, const Frame& reference, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param frame The frame that will be equalized according the histogram
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool equalization(uint8_t* frame, const unsigned int width, const unsigned int height, const Scalar factor, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param source The source frame for that an equalized target frame will be created
		 * @param target The target frame receiving the equalization result
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the source and target frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool equalization(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar factor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param frame The frame that will be equalized according the histogram
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param normalizedIntegral Already determined normalized integral histogram of the given frame
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool equalization(uint8_t* frame, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization in a sub region of a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param frame The frame that will be equalized according the histogram
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param subframeLeft Horizontal start position of the sub region within the frame, in pixel with range [0, width)
		 * @param subframeTop Vertical start position of the sub region within the frame, in pixel with range [0, heigh)
		 * @param subframeWidth The width of the sub region, in pixel with range [1, width - subframeLeft]
		 * @param subframeHeight The height of the sub region, in pixel with range [1, height - subframeTop]
		 * @param normalizedIntegral Already determined normalized integral histogram of the given frame
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool equalization(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param source The source frame for that an equalized target frame will be created
		 * @param target The target frame receiving the equalization result
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param normalizedIntegral Already determined normalized integral histogram of the given source frame
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the source and target frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool equalization(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a histogram equalization for a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param source The source frame for that an equalized target frame will be created
		 * @param target The target frame receiving the equalization result
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param subframeLeft Horizontal start position of the sub region within the frame, in pixel with range [0, width)
		 * @param subframeTop Vertical start position of the sub region within the frame, in pixel with range [0, heigh)
		 * @param subframeWidth The width of the sub region, in pixel with range [1, width - subframeLeft]
		 * @param subframeHeight The height of the sub region, in pixel with range [1, height - subframeTop]
		 * @param normalizedIntegral Already determined normalized integral histogram of the given source frame
		 * @param factor Interpolation factor between the original and the fully equalized frame, with range [0, 1]: 0 means the original frame is taken by 100 percent while 1 means that the equalized frame is taken by 100 percent
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the source and target frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool equalization(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * @param frame The frame for that the color values will be adjusted
		 * @param frameWidth The width of the frame in pixel
		 * @param frameHeight The height of the frame in pixel
		 * @param reference Reference frame providing the color statistics for the new frame
		 * @param referenceWidth The width of the reference frame in pixel
		 * @param referenceHeight The height of the reference frame in pixel
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool adjustColorToReference(uint8_t* frame, const unsigned int frameWidth, const unsigned int frameHeight, const uint8_t* reference, const unsigned int referenceWidth, const unsigned int referenceHeight, const unsigned int framePaddingElements, const unsigned int referencePaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * The frame types of the source frame and the given reference frame must be identical.<br>
		 * @param source The source frame that is used to create the target frame
		 * @param target The target frame with adjusted color information
		 * @param sourceWidth The width of the source and target frame in pixel
		 * @param sourceHeight The height of the source and target frame in pixel
		 * @param reference Reference frame providing the color statistics for the new frame
		 * @param referenceWidth The width of the reference frame in pixel
		 * @param referenceHeight The height of the reference frame in pixel
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool adjustColorToReference(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const uint8_t* reference, const unsigned int referenceWidth, const unsigned int referenceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int referencePaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * @param frame The frame for that the color values will be adjusted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param invertedNormalizedReferenceIntegral Already determined inverted and normalized integral histogram of the reference frame
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool adjustColorToReference(uint8_t* frame, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * @param frame The frame for that the color values will be adjusted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param subframeLeft Horizontal start position of the sub region within the frame, in pixel with range [0, width)
		 * @param subframeTop Vertical start position of the sub region within the frame, in pixel with range [0, heigh)
		 * @param subframeWidth The width of the sub region, in pixel with range [1, width - subframeLeft]
		 * @param subframeHeight The height of the sub region, in pixel with range [1, height - subframeTop]
		 * @param invertedNormalizedReferenceIntegral Already determined inverted and normalized integral histogram of the reference frame
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool adjustColorToReference(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * The frame types of the source frame and the given reference frame must be identical.<br>
		 * @param source The source frame that is used to create the target frame
		 * @param target The target frame with adjusted color information
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param invertedNormalizedReferenceIntegral Already determined inverted and normalized integral histogram of the reference frame
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool adjustColorToReference(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * The frame types of the source frame and the given reference frame must be identical.<br>
		 * @param source The source frame that is used to create the target frame
		 * @param target The target frame with adjusted color information
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param subframeLeft Horizontal start position of the sub region within the frame, in pixel with range [0, width)
		 * @param subframeTop Vertical start position of the sub region within the frame, in pixel with range [0, heigh)
		 * @param subframeWidth The width of the sub region, in pixel with range [1, width - subframeLeft]
		 * @param subframeHeight The height of the sub region, in pixel with range [1, height - subframeTop]
		 * @param invertedNormalizedReferenceIntegral Already determined inverted and normalized integral histogram of the reference frame
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool adjustColorToReference(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Determines the standard histogram in a subset of a given frame.
		 * @param frame The frame for that the histogram has to be determined, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param histogram Resulting histogram
		 * @param lock Optional lock object that needs to be defined if this function is executed in parallel
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param framePaddingElements Optional number of padding elements of the input frame, range: [0, infinity)
		 * @param firstRow First row to be handled, with range [0, infinity)
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void determineHistogram8BitPerChannelSubset(const uint8_t* frame, const unsigned int width, const unsigned int height, Histogram8BitPerChannel<tChannels>* histogram, Lock* lock, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a histogram equalization in a subset of a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param frame The frame that will be equalized according the histogram
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param normalizedIntegral Already determined normalized integral histogram of the given frame
		 * @param factor Interpolation factor, with range [0, 256]
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param framePaddingElements Optional number of padding elements of the input frame, range: [0, infinity)
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void equalizationSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>* normalizedIntegral, const unsigned int factor, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a histogram equalization in a subset of a given frame.
		 * The frame is equalized by application of the integral histogram of the original frame.<br>
		 * @param source The source frame for that an equalized target frame will be created
		 * @param target The target frame receiving the equalization result
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param normalizedIntegral Already determined normalized integral histogram of the given frame
		 * @param factor Interpolation factor, with range [0, 256]
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param sourcePaddingElements Optional number of padding elements of the source frame, range: [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements of the target frame, range: [0, infinity)
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of channels of the source and target frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void equalizationOfTargetSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>* normalizedIntegral, const unsigned int factor, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts the color in a subset of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * @param frame The frame for that the color values will be adjusted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param lookupTable Lookup table defining the adjustment of the color
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param framePaddingElements Optional number of padding elements of the input frame, range: [0, infinity)
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void adjustColorToReferenceSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const LookupTable8BitPerChannel<tChannels>* lookupTable, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts the color of a frame according to a given reference frame.
		 * The function applies a forward and backward mapping of the normalized integral histograms of both frames.
		 * The frame types of the source frame and the given reference frame must be identical.<br>
		 * @param source The source frame that is used to create the target frame
		 * @param target The target frame with adjusted color information
		 * @param width The width of the source and target frame in pixel
		 * @param height The height of the source and target frame in pixel
		 * @param lookupTable Lookup table defining the adjustment of the color
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param sourcePaddingElements Optional number of padding elements of the source frame, range: [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements of the target frame, range: [0, infinity)
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void adjustColorToReferenceOfTargetSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const LookupTable8BitPerChannel<tChannels>* lookupTable, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);
};

/**
 * Implementation of Contrast-Limited Adaptive Histogram Equalization (CLAHE).
 * @ingroup cv
 */
class OCEAN_CV_EXPORT ContrastLimitedAdaptiveHistogram
{
	public:

		/// Number of bins in the tile histograms
		static constexpr unsigned int histogramSize = 256u;

		/// Tile histogram
		typedef std::array<unsigned int, histogramSize> TileHistogram;

		/// Image partitioning and tile boundary lookup
		typedef LookupCenter2<uint8_t> TileLookupCenter2;

	public:

		/**
		 * Histogram equalization a la CLAHE
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param width The width of the source and target frames, range: [horizontalTiles, infinity)
		 * @param height The height of the source and target frames, range: [verticalTiles, infinity)
		 * @param target Destination location for the result, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`) and have the same size as the source
		 * @param clipLimit Global scaling factor to determine the tile clip limit, `tileClipLimit = clipLimit * N`, where `N = (tileSize / histogramSize)` is the number of pixels per bin if all pixels are distributed evenly over the histogram (average), range: [0, infinity), default: 40
		 * @param horizontalTiles Number of tiles the source image will be split horizontally, range: [2, width], default: 8
		 * @param verticalTiles Number of tiles the source image will be split vertically, range: [2, height], default: 8
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param targetPaddingElements Number of padding elements in the target data, range: [0, infinity), default: 0
		 * @param worker Optional worker instance for parallel execution, default: nullptr
		 */
		static void equalization8BitPerChannel(const uint8_t* const source, const unsigned int width, const unsigned height, uint8_t* const target, const Scalar clipLimit = Scalar(40), const unsigned int horizontalTiles = 8u, const unsigned int verticalTiles = 8u, const unsigned int sourcePaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

	protected:

		/**
		 * Computation of a lookup table required to normalize an image histogram (used per tile)
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param width The width of the source and target frames, range: [1, infinity)
		 * @param height The height of the source and target frames, range: [1, infinity)
		 * @param lookupTable Storage location for the computed lookup table, must be initialized, expected size: 256
		 * @param clipLimit Scaling factor to determine the tile clip limit, `tileClipLimit = clipLimit * N`, where `N = (tileSize / histogramSize)` is the number of pixels per bin if all pixels are distributed evenly over the histogram (average), range: [0, infinity)
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 */
		static inline void computeLookupTable(const uint8_t* source, const unsigned int width, const unsigned int height, uint8_t* const lookupTable, const Scalar clipLimit, const unsigned int sourcePaddingElements = 0u);

		/**
		 * Computation of per-tile lookup tables required to normalize an image histogram given a partitioned image
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param lookupCenter2 Defines how the source frame is partitioned, image size and number of bins will be extracted from this object, must be valid
		 * @param tileLookupTables Storage location for the computed lookup tables; will be initialized internally to size N x 256, where N = the total number of bins
		 * @param clipLimit Scaling factor to determine the tile clip limit, `tileClipLimit = clipLimit * N`, where `N = (tileSize / histogramSize)` is the number of pixels per bin if all pixels are distributed evenly over the histogram (average), range: [0, infinity)
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param worker Optional worker instance for parallel execution, default: nullptr
		 */
		static void computeTileLookupTables(const uint8_t* const source, const TileLookupCenter2& lookupCenter2, std::vector<uint8_t>& tileLookupTables, const Scalar clipLimit, const unsigned int sourcePaddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Computation of the bilinear interpolation parameters for the low bins of an image pixel
		 * If `isHorizontal == true`, this function computes the horizontal interpolation parameters, i.e., the left bins and left interpolation factors, otherwise it will compute the top bins and interpolation factors. For the horizontal parameters (`isHorizontal == true`), the right counterparts can be computed as:
		 *
		 * `rightBins[i] = lowBins[i] + 1u`
		 * `rightFactors_fixed7[i] = 128u - lowFactors_fixed7[i]`
		 *
		 * and, similarly, for the vertical parameters (`isHorizontal == false`).
		 *
		 * Note: 128u is the fixed-point, 7-bit precision equivalent of 1.0f
		 *
		 * @param lookupCenter2 Defines how the source frame is partitioned, image size and number of bins will be extracted from this object, must be valid, number of bins (`binsX()`) must >= 2
		 * @param isHorizontal If true, it will compute the horizontal interpolation parameters, otherwise the vertical parameters will be computed
		 * @param lowBins Stores the indices of the closest lower (lower = left if `isHorizontal == true`, otherwise lower = top) bins of a pixel location, must be initialized before calling this function, expected size: image width if `isHorizontal == true`, otherwise image height
		 * @param lowFactors_fixed7 Stores the horizontal (if `isHorizontal == true`) or vertical (if `isHorizontal == false`) interpolation factors for the lower bins (as 8-bit fixed point numbers with 7-bit precision, i.e. range: [0, 1]), must be initialized before calling this function, expected size:  image width if `isHorizontal == true`, otherwise image height
		 */
		static void computeLowBilinearInterpolationFactors7BitPrecision(const TileLookupCenter2& lookupCenter2, const bool isHorizontal, Index32* lowBins, uint8_t* lowFactors_fixed7);

		/**
		 * Histogram normalization by bilinearly interpolating pixels using the CLAHE per-tile lookup tables
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param lookupCenter2 Defines how the source frame is partitioned, image size and number of bins will be extracted from this object, must be valid
		 * @param target Pointer to the data of the target frame, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param tileLookupTables Storage location for the computed lookup tables; will be initialized internally to size N x 256, where N = the total number of bins
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param targetPaddingElements Number of padding elements in the target data, range: [0, infinity), default: 0
		 * @param worker Optional worker instance for parallel execution, default: nullptr
		 */
		static void bilinearInterpolation(const uint8_t* const source, const TileLookupCenter2& lookupCenter2, uint8_t* const target, const std::vector<uint8_t>& tileLookupTables, const unsigned int sourcePaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Helper function for the computation of per-tile lookup tables required to normalize an image histogram given a partitioned image
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param lookupCenter2 Defines how the source frame is partitioned, image size and number of bins will be extracted from this object, must be valid
		 * @param tileLookupTables Storage location for the computed lookup tables; will be initialized internally to size N x 256, where N = the total number of bins
		 * @param clipLimit Scaling factor to determine the tile clip limit, `tileClipLimit = clipLimit * N`, where `N = (tileSize / histogramSize)` is the number of pixels per bin if all pixels are distributed evenly over the histogram (average), range: [0, infinity)
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param firstTile Index of first tile to process, range: [0, N), where N is the total number of tiles
		 * @param tileCount Number of tiles to process, range: [1, N], such `firstTile + tileCount < N` where N is the total number of tiles
		 */
		static void computeTileLookupTablesSubset(const uint8_t* const source, const TileLookupCenter2* lookupCenter2, uint8_t* const tileLookupTables, const Scalar clipLimit, const unsigned int sourcePaddingElements = 0u, const unsigned int firstTile = 0u, const unsigned int tileCount = 0u);

		/**
		 * Integer-based (fixed-point arithmetic) helper function for the histogram normalization by bilinearly interpolating pixels using the CLAHE per-tile lookup tables
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param lookupCenter2 Defines how the source frame is partitioned, image size and number of bins will be extracted from this object, must be valid
		 * @param target Pointer to the data of the target frame, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param tileLookupTables Storage location for the computed lookup tables; will be initialized internally to size N x 256, where N = the total number of bins
		 * @param leftBins Stores the indices of the closest bins left of a horizontal pixel location, must be initialized, expected size: image width
		 * @param leftFactors_fixed7 Stores the horizontal interpolation factors for the left bins (as fixed-point number with 7-bit precision), must be initialized, expected size: image width
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param targetPaddingElements Number of padding elements in the target data, range: [0, infinity), default: 0
		 * @param rowStart First image row to process, range: [0, height)
		 * @param rowCount Number of rows to process, range: [0, height - rowStart)
		 */
		static void bilinearInterpolation7BitPrecisionSubset(const uint8_t* const source, const TileLookupCenter2* lookupCenter2, uint8_t* const target, const uint8_t* const tileLookupTables, const Index32* const leftBins, const uint8_t* const leftFactors_fixed7, const unsigned int sourcePaddingElements = 0u, const unsigned int targetPaddingElements = 0u, const unsigned int rowStart = 0u, const unsigned int rowCount = 0u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Helper function for the histogram normalization by bilinearly interpolating pixels using the CLAHE per-tile lookup tables
		 * @param source Pointer to the data of the source frame that will be processed, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param lookupCenter2 Defines how the source frame is partitioned, image size and number of bins will be extracted from this object, must be valid
		 * @param target Pointer to the data of the target frame, must be valid and 8-bit unsigned, 1-channel (`FrameType::FORMAT_Y8`)
		 * @param tileLookupTables Storage location for the computed lookup tables; will be initialized internally to size N x 256, where N = the total number of bins
		 * @param leftBins Stores the indices of the closest bins left of a horizontal pixel location, must be initialized, expected size: image width
		 * @param leftFactors_fixed7 Stores the horizontal interpolation factors for the left bins (as fixed-point number with 7-bit precision), must be initialized, expected size: image width
		 * @param topBins Stores the indices of the closest vertical bins above a pixel location, must be initialized, expected size: image height
		 * @param topFactors_fixed7 Stores the vertical interpolation factors for the top bins (as fixed-point number with 7-bit precision), must be initialized, expected size: image height
		 * @param sourcePaddingElements Number of padding elements in the source data, range: [0, infinity), default: 0
		 * @param targetPaddingElements Number of padding elements in the target data, range: [0, infinity), default: 0
		 * @param tileStart First tile to process, range: [0, N), where N = number of tiles
		 * @param tileCount Number of tiles to process, range: [0, N - tileStart)
		 */
		static void bilinearInterpolationNEON7BitPrecisionSubset(const uint8_t* const source, const TileLookupCenter2* lookupCenter2, uint8_t* const target, const uint8_t* const tileLookupTables, const Index32* const leftBins, const uint8_t* const leftFactors_fixed7, const Index32* const topBins, const uint8_t* const topFactors_fixed7, const unsigned int sourcePaddingElements = 0u, const unsigned int targetPaddingElements = 0u, const unsigned int tileStart = 0u, const unsigned int tileCount = 0u);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
};

inline void ContrastLimitedAdaptiveHistogram::computeLookupTable(const uint8_t* source, const unsigned int width, const unsigned int height, uint8_t* const lookupTable, const Scalar clipLimit, const unsigned int sourcePaddingElements)
{
	ocean_assert(source != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(lookupTable != nullptr);
	ocean_assert(clipLimit >= Scalar(0));

	const unsigned int sourceArea = width * height;
	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const uint8_t* const sourceEnd = source + height * sourceStrideElements;

	// Histogram computation
	TileHistogram histogram;
	memset(histogram.data(), 0u, histogram.size() * sizeof(TileHistogram::value_type));
	const unsigned int widthEnd = width >= 4u ? width - 4u : 0u;

	for (unsigned int y = 0u; y < height; ++y)
	{
		unsigned int x = 0u;

		while (x < widthEnd)
		{
			ocean_assert_and_suppress_unused(source + x + 3u < sourceEnd, sourceEnd);
			histogram[source[x + 0u]]++;
			histogram[source[x + 1u]]++;
			histogram[source[x + 2u]]++;
			histogram[source[x + 3u]]++;

			x += 4u;
		}

		while (x < width)
		{
			ocean_assert(source + x < sourceEnd);
			histogram[source[x]]++;

			++x;
		}

		source += sourceStrideElements;
	}

	// Clip histogram peaks and redistribute area exceeding the clip limit
	ocean_assert(histogramSize != 0u);
	const unsigned int scaledClipLimit = std::max(1u, (unsigned int)(clipLimit * float(sourceArea) / float(histogramSize)));
	unsigned int clippedArea = 0u;

	for (unsigned int i = 0u; i < histogramSize; ++i)
	{
		if (histogram[i] > scaledClipLimit)
		{
			clippedArea += histogram[i] - scaledClipLimit;
			histogram[i] = scaledClipLimit;
		}
	}

	if (clippedArea != 0u)
	{
		const unsigned int redistribution = clippedArea / histogramSize;
		const unsigned int residual = clippedArea - (redistribution * histogramSize);

		for (unsigned int i = 0u; i < residual; ++i)
		{
			histogram[i] += (redistribution + 1u);
		}

		for (unsigned int i = residual; i < histogramSize; ++i)
		{
			histogram[i] += redistribution;
		}
	}

	// Normalize histogram (CDF + normalization)
	ocean_assert(sourceArea != 0u);
	const float normalizationFactor = float(histogramSize - 1u) / float(sourceArea);
	unsigned int sum = 0u;

	for (unsigned int i = 0u; i < histogramSize; ++i)
	{
		sum += histogram[i];

		ocean_assert((int)(float(sum) * normalizationFactor + 0.5f) >= 0);
		ocean_assert((int)(float(sum) * normalizationFactor + 0.5f) <= (int)(NumericT<uint8_t>::maxValue()));
		lookupTable[i] = (uint8_t)(float(sum) * normalizationFactor + 0.5f);
	}
}

template <unsigned int tChannels>
inline Histogram::HistogramBase8BitPerChannel<tChannels>::HistogramBase8BitPerChannel()
{
	memset(histogramBins, 0x00, sizeof(unsigned int) * 256u * tChannels);
}

template <unsigned int tChannels>
template <unsigned int tChannel>
inline unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::bin(const uint8_t index) const
{
	static_assert(tChannel < tChannels, "Invalid channel index!");

	return histogramBins[tChannel * 256u + index];
}

template <unsigned int tChannels>
inline unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::bin(const unsigned int channel, const uint8_t index) const
{
	ocean_assert(channel < tChannels);

	return histogramBins[channel * 256u + index];
}

template <unsigned int tChannels>
inline unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::sumBin(const uint8_t index) const
{
	unsigned int result = 0u;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		result += histogramBins[n * 256u + index];
	}

	return result;
}

template <unsigned int tChannels>
template <unsigned int tChannel>
inline const unsigned int* Histogram::HistogramBase8BitPerChannel<tChannels>::bins() const
{
	static_assert(tChannel < tChannels, "Invalid channel index!");

	return histogramBins + 256u * tChannel;
}

template <unsigned int tChannels>
inline const unsigned int* Histogram::HistogramBase8BitPerChannel<tChannels>::bins(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);

	return histogramBins + 256u * channel;
}

template <unsigned int tChannels>
template <unsigned int tChannel>
unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::determineHighestValue() const
{
	static_assert(tChannel < tChannels, "Invalid channel index!");

	unsigned int value = 0u;
	const unsigned int* bins = histogramBins + 256u * tChannel;

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		if (bins[n] > value)
		{
			value = bins[n];
		}
	}

	return value;
}

template <unsigned int tChannels>
unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::determineHighestValue(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);

	unsigned int value = 0u;
	const unsigned int* bins = histogramBins + 256u * channel;

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		if (bins[n] > value)
		{
			value = bins[n];
		}
	}

	return value;
}

template <unsigned int tChannels>
unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::determineHighestValue() const
{
	unsigned int value = 0u;

	for (unsigned int n = 0u; n < 256u * tChannels; ++n)
	{
		if (histogramBins[n] > value)
		{
			value = histogramBins[n];
		}
	}

	return value;
}

template <unsigned int tChannels>
unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::determineStartBin(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);

	const unsigned int* bins = histogramBins + 256u * channel;

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		if (bins[n] != 0u)
		{
			return n;
		}
	}

	return (unsigned int)(-1);
}

template <unsigned int tChannels>
unsigned int Histogram::HistogramBase8BitPerChannel<tChannels>::determineEndBin(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);

	const unsigned int* bins = histogramBins + 256u * channel;

	for (unsigned int n = 255u; n < 256u; --n)
	{
		if (bins[n] != 0u)
		{
			return n;
		}
	}

	return (unsigned int)(-1);
}

template <unsigned int tChannels>
inline void Histogram::HistogramBase8BitPerChannel<tChannels>::normalize(const unsigned int newMaximalValue)
{
	const unsigned int maximalValue = determineHighestValue();

	if (maximalValue == 0u)
	{
		return;
	}

	const unsigned int maximalValue_2 = maximalValue / 2u;

	for (unsigned int n = 0u; n < 256u * tChannels; ++n)
	{
		histogramBins[n] = (histogramBins[n] * newMaximalValue + maximalValue_2) / maximalValue;
	}
}

template <unsigned int tChannels>
inline void Histogram::HistogramBase8BitPerChannel<tChannels>::normalize(const unsigned int channel, const unsigned int newMaximalValue)
{
	ocean_assert(channel < tChannels);

	const unsigned int maximalValue = determineHighestValue(channel);

	if (maximalValue == 0u)
	{
		return;
	}

	const unsigned int maximalValue_2 = maximalValue / 2u;

	unsigned int* bins = histogramBins + 256u * channel;

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		bins[n] = (bins[n] * newMaximalValue + maximalValue_2) / maximalValue;
	}
}

template <unsigned int tChannels>
void Histogram::HistogramBase8BitPerChannel<tChannels>::clear()
{
	memset(histogramBins, 0x00, sizeof(unsigned int) * 256u * tChannels);
}

template <unsigned int tChannels>
inline bool Histogram::HistogramBase8BitPerChannel<tChannels>::operator==(const HistogramBase8BitPerChannel<tChannels>& histogram) const
{
	for (unsigned int n = 0u; n < 256u * tChannels; ++n)
	{
		if (histogramBins[n] != histogram.histogramBins[n])
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline bool Histogram::HistogramBase8BitPerChannel<tChannels>::operator!=(const HistogramBase8BitPerChannel<tChannels>& histogram) const
{
	return !(*this == histogram);
}

template <unsigned int tChannels>
Histogram::HistogramBase8BitPerChannel<tChannels>& Histogram::HistogramBase8BitPerChannel<tChannels>::operator+=(const HistogramBase8BitPerChannel<tChannels>& histogram)
{
	for (unsigned int n = 0u; n < 256u * tChannels; ++n)
	{
		HistogramBase8BitPerChannel<tChannels>::histogramBins[n] += histogram.histogramBins[n];
	}

	return *this;
}

template <unsigned int tChannels>
inline const unsigned int* Histogram::HistogramBase8BitPerChannel<tChannels>::operator()() const
{
	return histogramBins;
}

template <unsigned int tChannels>
void Histogram::HistogramBase8BitPerChannel<tChannels>::setBin(const unsigned int channel, const uint8_t index, const unsigned int value)
{
	ocean_assert(channel < tChannels);

	histogramBins[channel * 256u + index] = value;
}

template <unsigned int tChannels>
inline bool Histogram::HistogramBase8BitPerChannel<tChannels>::isNull() const
{
	for (unsigned int n = 0u; n < 256u * tChannels; ++n)
	{
		if (histogramBins[n] != 0u)
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline Histogram::Histogram8BitPerChannel<tChannels>::Histogram8BitPerChannel() :
	HistogramBase8BitPerChannel<tChannels>()
{
	// nothing to do here
}

template <unsigned int tChannels>
template <unsigned int tChannel>
inline void Histogram::Histogram8BitPerChannel<tChannels>::incrementBin(const uint8_t index)
{
	static_assert(tChannel < tChannels, "Invalid channel index!");

	HistogramBase8BitPerChannel<tChannels>::histogramBins[tChannel * 256u + index]++;
}

template <unsigned int tChannels>
inline void Histogram::Histogram8BitPerChannel<tChannels>::incrementBin(const unsigned int channel, const uint8_t index)
{
	ocean_assert(channel < tChannels);

	HistogramBase8BitPerChannel<tChannels>::histogramBins[channel * 256u + index]++;
}

template <unsigned int tChannels>
inline void Histogram::Histogram8BitPerChannel<tChannels>::increment(const uint8_t* pixel)
{
	ocean_assert(pixel != nullptr);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		HistogramBase8BitPerChannel<tChannels>::histogramBins[n * 256u + pixel[n]]++;
	}
}

template <unsigned int tChannels>
Histogram::Histogram8BitPerChannel<tChannels> Histogram::Histogram8BitPerChannel<tChannels>::operator+(const Histogram8BitPerChannel<tChannels>& histogram) const
{
	Histogram8BitPerChannel<tChannels> result;

	for (unsigned int n = 0u; n < 256u * tChannels; ++n)
	{
		result.histogramBins[n] = HistogramBase8BitPerChannel<tChannels>::histogramBins[n] + histogram.histogramBins[n];
	}

	return result;
}

template <unsigned int tChannels>
inline Histogram::IntegralHistogram8BitPerChannel<tChannels>::IntegralHistogram8BitPerChannel() :
	HistogramBase8BitPerChannel<tChannels>()
{
	// nothing to do here
}

template <unsigned int tChannels>
inline Histogram::IntegralHistogram8BitPerChannel<tChannels>::IntegralHistogram8BitPerChannel(const Histogram8BitPerChannel<tChannels>& histogram) :
	HistogramBase8BitPerChannel<tChannels>()
{
	for (unsigned int c = 0u;  c < tChannels; ++c)
	{
		const unsigned int* const bins = histogram.bins(c);
		unsigned int* const thisBins = HistogramBase8BitPerChannel<tChannels>::histogramBins + 256u * c;

		// copy the first bin
		thisBins[0] = bins[0];

		// copy and increment the remaining bins
		for (unsigned int b = 1u; b < 256u; ++b)
		{
			thisBins[b] = thisBins[b - 1u] + bins[b];
		}
	}

#ifdef OCEAN_DEBUG

	// check whether the increment values are identical
	for (unsigned int c = 1u; c < tChannels; ++c)
	{
		ocean_assert(HistogramBase8BitPerChannel<tChannels>::bin(0, 255) == HistogramBase8BitPerChannel<tChannels>::bin(c, 255));
	}

#endif // OCEAN_DEBUG
}

template <unsigned int tChannels>
Histogram::IntegralHistogram8BitPerChannel<tChannels> Histogram::IntegralHistogram8BitPerChannel<tChannels>::invert() const
{
	ocean_assert(HistogramBase8BitPerChannel<tChannels>::determineHighestValue() <= 0xFFu);

	IntegralHistogram8BitPerChannel<tChannels> result;

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		for (unsigned int n = 0u; n < 256u; ++n)
		{
			const unsigned int value = HistogramBase8BitPerChannel<tChannels>::bin(c, (uint8_t)(n));

			for (int r = int(value); r >= 0; --r)
			{
				if (result.bin(c, (uint8_t)(r)) == 0)
				{
					result.setBin(c, (uint8_t)(r), n);
				}
				else
				{
					break;
				}
			}
		}
	}

	return result;
}

template <unsigned int tChannels>
Histogram::LookupTable8BitPerChannel<tChannels>::LookupTable8BitPerChannel()
{
	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		for (unsigned int n = 0u; n < 256u; ++n)
		{
			lookupData[c * 256u + n] = n;
		}
	}
}

template <unsigned int tChannels>
Histogram::LookupTable8BitPerChannel<tChannels>::LookupTable8BitPerChannel(const IntegralHistogram8BitPerChannel<tChannels>& normalizedHistogram, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedHistogram)
{
	ocean_assert(normalizedHistogram.determineHighestValue() <= 255u);
	ocean_assert(invertedNormalizedHistogram.determineHighestValue() <= 255u);

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		for (unsigned int n = 0u; n < 256u; ++n)
		{
			lookupData[c * 256u + n] = (uint8_t)(invertedNormalizedHistogram.bin(c, (uint8_t)(normalizedHistogram.bin(c, (uint8_t)(n)))));
		}
	}
}

template <unsigned int tChannels>
template <unsigned int tChannel>
uint8_t Histogram::LookupTable8BitPerChannel<tChannels>::lookup(const uint8_t index) const
{
	static_assert(tChannel < tChannels, "Invalid channel!");

	return lookupData[tChannel * 256u + index];
}

template <unsigned int tChannels>
uint8_t Histogram::LookupTable8BitPerChannel<tChannels>::lookup(const unsigned int channel, const uint8_t index) const
{
	ocean_assert(channel < tChannels);

	return lookupData[channel * 256u + index];
}

template <unsigned int tChannels>
Histogram::Histogram8BitPerChannel<tChannels> Histogram::determineHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(frame && width >= 1u && height >= 1u);

	Histogram8BitPerChannel<tChannels> result;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(determineHistogram8BitPerChannelSubset<tChannels>, frame, width, height, &result, &lock, 0u, width, framePaddingElements, 0u, 0u), 0u, height, 8u, 9u, 20u);
	}
	else
	{
		if (framePaddingElements == 0u)
		{
			const uint8_t* const frameEnd = frame + height * width * tChannels;

			while (frame != frameEnd)
			{
				result.increment(frame);
				frame += tChannels;
			}
		}
		else
		{
			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					result.increment(frame);
					frame += tChannels;
				}

				frame += framePaddingElements;
			}
		}
	}

	return result;
}

template <unsigned int tChannels>
Histogram::Histogram8BitPerChannel<tChannels> Histogram::determineHistogram8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(frame && width >= 1u && height >= 1u);

	ocean_assert(subframeLeft + subframeWidth <= width);
	ocean_assert(subframeTop + subframeHeight <= height);

	Histogram8BitPerChannel<tChannels> result;

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(determineHistogram8BitPerChannelSubset<tChannels>, frame, width, height, &result, &lock, subframeLeft, subframeWidth, framePaddingElements, 0u, 0u), subframeTop, subframeHeight, 8u, 9u, 20u);
	}
	else
	{
		determineHistogram8BitPerChannelSubset<tChannels>(frame, width, height, &result, nullptr, subframeLeft, subframeWidth, framePaddingElements, subframeTop, subframeHeight);
	}

	return result;
}

template <unsigned int tChannels>
bool Histogram::adjustColorToReference(uint8_t* frame, const unsigned int frameWidth, const unsigned int frameHeight, const uint8_t* reference, const unsigned int referenceWidth, const unsigned int referenceHeight, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int framePaddingElements, const unsigned int referencePaddingElements, Worker* worker)
{
	ocean_assert(frame != nullptr && reference != nullptr);

	ocean_assert(horizontalBins > 0u && verticalBins > 0u);
	ocean_assert(horizontalBins <= min(frameWidth, referenceWidth));
	ocean_assert(verticalBins <= min(frameHeight, referenceHeight));

	if (frame == nullptr || reference == nullptr || verticalBins == 0u || horizontalBins == 0u || horizontalBins > frameWidth || horizontalBins > referenceWidth || verticalBins > frameHeight || verticalBins > referenceHeight)
	{
		return false;
	}

	std::vector<LookupTable8BitPerChannel<tChannels>> lookups;
	lookups.reserve(horizontalBins * verticalBins);

	// create reference histograms
	for (unsigned int y = 0; y < verticalBins; ++y)
	{
		for (unsigned int x = 0; x < horizontalBins; ++x)
		{
			const unsigned int referenceBinLeft = x * referenceWidth / horizontalBins;
			const unsigned int referenceBinTop = y * referenceHeight / verticalBins;

			const unsigned int referenceBinRight = min((x + 1) * referenceWidth / horizontalBins, referenceWidth);
			const unsigned int referenceBinBottom = min((y + 1) * referenceHeight / verticalBins, referenceHeight);

			const unsigned int referenceBinWidth = referenceBinRight - referenceBinLeft;
			const unsigned int referenceBinHeight = referenceBinBottom - referenceBinTop;

			IntegralHistogram8BitPerChannel<tChannels> integralReferenceHistogram(determineHistogram8BitPerChannel<tChannels>(reference, referenceWidth, referenceHeight, referenceBinLeft, referenceBinTop, referenceBinWidth, referenceBinHeight, referencePaddingElements, worker));
			integralReferenceHistogram.normalize(0xFF);

			const unsigned int frameBinLeft = x * frameWidth / horizontalBins;
			const unsigned int frameBinTop = y * frameHeight / verticalBins;

			const unsigned int frameBinRight = min((x + 1) * frameWidth / horizontalBins, frameWidth);
			const unsigned int frameBinBottom = min((y + 1) * frameHeight / verticalBins, frameHeight);

			const unsigned int frameBinWidth = frameBinRight - frameBinLeft;
			const unsigned int frameBinHeight = frameBinBottom - frameBinTop;

			IntegralHistogram8BitPerChannel<tChannels> frameHistogram(determineHistogram8BitPerChannel<tChannels>(frame, frameWidth, frameHeight, frameBinLeft, frameBinTop, frameBinWidth, frameBinHeight, framePaddingElements, worker));
			frameHistogram.normalize(0xFF);

			lookups.push_back(LookupTable8BitPerChannel<tChannels>(frameHistogram, integralReferenceHistogram.invert()));
		}
	}

	const unsigned int frameStrideElements = frameWidth * tChannels + framePaddingElements;

	for (unsigned int y = 0u; y < frameHeight; ++y)
	{
		for (unsigned int x = 0u; x < frameWidth; ++x)
		{
			const unsigned int xBin = (x * horizontalBins) / frameWidth;
			const unsigned int yBin = (y * verticalBins) / frameHeight;

			const unsigned int xBinCenter = (xBin * frameWidth / horizontalBins + min((xBin + 1u) * frameWidth / horizontalBins, frameWidth)) / 2u;
			const unsigned int yBinCenter = (yBin * frameHeight / verticalBins + min((yBin + 1u) * frameHeight / verticalBins, frameHeight)) / 2u;

			ocean_assert(xBinCenter < frameWidth);
			ocean_assert(yBinCenter < frameHeight);

			const unsigned int xLowBin = (x >= xBinCenter) ? xBin : max(0, int(xBin) - 1);
			const unsigned int xHighBin = (x < xBinCenter) ? xBin : min(xLowBin + 1u, horizontalBins - 1u);

			const unsigned int yLowBin = (y >= yBinCenter) ? yBin : max(0, int(yBin) - 1);
			const unsigned int yHighBin = (y < yBinCenter) ? yBin : min(yLowBin + 1u, verticalBins - 1u);

			ocean_assert(((xLowBin == 0u || xLowBin == horizontalBins - 1u) && xHighBin == xLowBin) || xLowBin + 1u == xHighBin);
			ocean_assert(((yLowBin == 0u || yLowBin == verticalBins - 1u) && yHighBin == yLowBin) || yLowBin + 1u == yHighBin);

			const unsigned int leftCenter = (xLowBin * frameWidth / horizontalBins + min((xLowBin + 1u) * frameWidth / horizontalBins, frameWidth)) / 2u;
			const unsigned int rightCenter = (xHighBin * frameWidth / horizontalBins + min((xHighBin + 1u) * frameWidth / horizontalBins, frameWidth)) / 2u;

			const unsigned int topCenter = (yLowBin * frameHeight / verticalBins + min((yLowBin + 1u) * frameHeight / verticalBins, frameHeight)) / 2u;
			const unsigned int bottomCenter = (yHighBin * frameHeight / verticalBins + min((yHighBin + 1u) * frameHeight / verticalBins, frameHeight)) / 2u;


			ocean_assert(leftCenter <= rightCenter);
			ocean_assert(topCenter <= bottomCenter);

			const unsigned int centerWidth = rightCenter - leftCenter;
			const unsigned int centerHeight = bottomCenter - topCenter;

			const unsigned int xFactor = centerWidth != 0u ? (abs(int(x) - int(leftCenter)) * 256u + centerWidth / 2u) / centerWidth : 256u;
			const unsigned int yFactor = centerHeight != 0u ? (abs(int(y) - int(topCenter)) * 256u + centerHeight / 2u) / centerHeight : 256u;

			ocean_assert(xFactor <= 256u);
			ocean_assert(yFactor <= 256u);

			uint8_t* const framePixel = frame + y * frameStrideElements + x * tChannels;

			const LookupTable8BitPerChannel<tChannels>& topLeft = lookups[yLowBin * horizontalBins + xLowBin];
			const LookupTable8BitPerChannel<tChannels>& topRight = lookups[yLowBin * horizontalBins + xHighBin];
			const LookupTable8BitPerChannel<tChannels>& bottomLeft = lookups[yHighBin * horizontalBins + xLowBin];
			const LookupTable8BitPerChannel<tChannels>& bottomRight = lookups[yHighBin * horizontalBins + xHighBin];

			const unsigned int factorTopLeft = (256u - xFactor) * (256u - yFactor);
			const unsigned int factorTopRight = xFactor * (256u - yFactor);
			const unsigned int factorBottomLeft = (256u - xFactor) * yFactor;
			const unsigned int factorBottomRight = xFactor * yFactor;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const uint8_t value = framePixel[n];

				framePixel[n] = (uint8_t)((topLeft.lookup(n, value) * factorTopLeft + topRight.lookup(n, value) * factorTopRight
									+ bottomLeft.lookup(n, value) * factorBottomLeft + bottomRight.lookup(n, value) * factorBottomRight + 32768u) >> 16u); // / 65536;
			}
		}
	}

	return true;
}

template <unsigned int tChannels>
inline bool Histogram::equalization(uint8_t* frame, const unsigned int width, const unsigned int height, const Scalar factor, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(frame && width > 0u && height > 0u);
	ocean_assert(factor >= 0 && factor <= 1);

	if (factor < 0 || factor > 1)
	{
		return false;
	}

	IntegralHistogram8BitPerChannel<tChannels> integralHistogram(determineHistogram8BitPerChannel<tChannels>(frame, width, height, framePaddingElements, worker));
	integralHistogram.normalize(0xFF);

	return equalization<tChannels>(frame, width, height, integralHistogram, factor, framePaddingElements, worker);
}

template <unsigned int tChannels>
inline bool Histogram::equalization(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar factor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source && target && width > 0u && height > 0u);
	ocean_assert(factor >= 0 && factor <= 1);

	if (factor < 0 || factor > 1)
	{
		return false;
	}

	IntegralHistogram8BitPerChannel<tChannels> integralHistogram(determineHistogram8BitPerChannel<tChannels>(source, width, height, sourcePaddingElements, worker));
	integralHistogram.normalize(0xFF);

	return equalization<tChannels>(source, target, width, height, integralHistogram, factor, sourcePaddingElements, targetPaddingElements, worker);
}

template <unsigned int tChannels>
inline bool Histogram::equalization(uint8_t* frame, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int framePaddingElements, Worker* worker)
{
	return equalization<tChannels>(frame, width, height, 0u, 0u, width, height, normalizedIntegral, factor, framePaddingElements, worker);
}

template <unsigned int tChannels>
bool Histogram::equalization(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(frame && width > 0u && height > 0u);
	ocean_assert(factor >= 0 && factor <= 1);

	ocean_assert(subframeLeft + subframeWidth <= width);
	ocean_assert(subframeTop + subframeHeight <= height);

	if (factor < 0 || factor > 1 || subframeLeft + subframeWidth > width || subframeTop + subframeHeight > height)
	{
		return false;
	}

	const unsigned int iFactor = (unsigned int)(factor * Scalar(256));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(equalizationSubset<tChannels>, frame, width, height, (const IntegralHistogram8BitPerChannel<tChannels>*)&normalizedIntegral, iFactor, subframeLeft, subframeWidth, framePaddingElements, 0u, 0u), 0u, height, 8u, 9u, 20u);
	}
	else
	{
		equalizationSubset<tChannels>(frame, width, height, &normalizedIntegral, iFactor, subframeLeft, subframeWidth, framePaddingElements, subframeTop, subframeHeight);
	}

	return true;
}

template <unsigned int tChannels>
inline bool Histogram::equalization(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	return equalization<tChannels>(source, target, width, height, 0u, 0u, width, height, normalizedIntegral, factor, sourcePaddingElements, targetPaddingElements, worker);
}

template <unsigned int tChannels>
bool Histogram::equalization(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& normalizedIntegral, const Scalar factor, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source && target && width > 0u && height > 0u);
	ocean_assert(factor >= 0 && factor <= 1);

	ocean_assert(subframeLeft + subframeWidth <= width);
	ocean_assert(subframeTop + subframeHeight <= height);

	if (factor < 0 || factor > 1 || subframeLeft + subframeWidth > width || subframeTop + subframeHeight > height)
	{
		return false;
	}

	const unsigned int iFactor = (unsigned int)(factor * Scalar(256));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(equalizationOfTargetSubset<tChannels>, source, target, width, height, &normalizedIntegral, iFactor, subframeLeft, subframeWidth, sourcePaddingElements, targetPaddingElements, 0u, 0u), subframeTop, subframeHeight, 10u, 11u, 20u);
	}
	else
	{
		equalizationOfTargetSubset<tChannels>(source, target, width, height, &normalizedIntegral, iFactor, subframeLeft, subframeWidth, sourcePaddingElements, targetPaddingElements, subframeTop, subframeHeight);
	}

	return true;
}

template <unsigned int tChannels>
bool Histogram::adjustColorToReference(uint8_t* frame, const unsigned int frameWidth, const unsigned int frameHeight, const uint8_t* reference, const unsigned int referenceWidth, const unsigned int referenceHeight, const unsigned int framePaddingElements, const unsigned int referencePaddingElements, Worker* worker)
{
	ocean_assert(frame && reference);

	IntegralHistogram8BitPerChannel<tChannels> integralReferenceHistogram(determineHistogram8BitPerChannel<tChannels>(reference, referenceWidth, referenceHeight, referencePaddingElements, worker));
	integralReferenceHistogram.normalize(0xFF);
	const IntegralHistogram8BitPerChannel<tChannels> invertedReferenceHistogram(integralReferenceHistogram.invert());

	return adjustColorToReference<tChannels>(frame, frameWidth, frameHeight, invertedReferenceHistogram, framePaddingElements, worker);
}

template <unsigned int tChannels>
bool Histogram::adjustColorToReference(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const uint8_t* reference, const unsigned int referenceWidth, const unsigned int referenceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int referencePaddingElements, Worker* worker)
{
	ocean_assert(source && target && reference);

	IntegralHistogram8BitPerChannel<tChannels> integralReferenceHistogram(determineHistogram8BitPerChannel<tChannels>(reference, referenceWidth, referenceHeight, referencePaddingElements, worker));
	integralReferenceHistogram.normalize(0xFF);
	const IntegralHistogram8BitPerChannel<tChannels> invertedReferenceHistogram(integralReferenceHistogram.invert());

	return adjustColorToReference<tChannels>(source, target, sourceWidth, sourceHeight, invertedReferenceHistogram, sourcePaddingElements, targetPaddingElements, worker);
}

template <unsigned int tChannels>
inline bool Histogram::adjustColorToReference(uint8_t* frame, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int framePaddingElements, Worker* worker)
{
	return adjustColorToReference<tChannels>(frame, width, height, 0u, 0u, width, height, invertedNormalizedReferenceIntegral, framePaddingElements, worker);
}

template <unsigned int tChannels>
bool Histogram::adjustColorToReference(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(frame);

	if (!frame || subframeLeft + subframeWidth > width || subframeTop + subframeHeight > height)
	{
		return false;
	}

	IntegralHistogram8BitPerChannel<tChannels> frameHistogram(determineHistogram8BitPerChannel<tChannels>(frame, width, height, framePaddingElements, worker));
	frameHistogram.normalize(0xFF);

	const LookupTable8BitPerChannel<tChannels> lookupTable(frameHistogram, invertedNormalizedReferenceIntegral);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(adjustColorToReferenceSubset<tChannels>, frame, width, height, &lookupTable, subframeLeft, subframeWidth, framePaddingElements, 0u, 0u), subframeTop, subframeHeight, 8u, 9u, 20u);
	}
	else
	{
		adjustColorToReferenceSubset<tChannels>(frame, width, height, &lookupTable, subframeLeft, subframeWidth, framePaddingElements, subframeTop, subframeHeight);
	}

	return true;
}

template <unsigned int tChannels>
inline bool Histogram::adjustColorToReference(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	return adjustColorToReference<tChannels>(source, target, width, height, 0u, 0u, width, height, invertedNormalizedReferenceIntegral, sourcePaddingElements, targetPaddingElements, worker);
}

template <unsigned int tChannels>
bool Histogram::adjustColorToReference(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const IntegralHistogram8BitPerChannel<tChannels>& invertedNormalizedReferenceIntegral, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source && target);

	if (!source || !target || subframeLeft + subframeWidth > width || subframeTop + subframeHeight > height)
	{
		return false;
	}

	IntegralHistogram8BitPerChannel<tChannels> frameHistogram(determineHistogram8BitPerChannel<tChannels>(source, width, height, sourcePaddingElements, worker));
	frameHistogram.normalize(0xFF);

	const LookupTable8BitPerChannel<tChannels> lookupTable(frameHistogram, invertedNormalizedReferenceIntegral);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(adjustColorToReferenceOfTargetSubset<tChannels>, source, target, width, height, &lookupTable, subframeLeft, subframeWidth, sourcePaddingElements, targetPaddingElements, 0u, 0u), subframeTop, subframeHeight, 9u, 10u, 20u);
	}
	else
	{
		adjustColorToReferenceOfTargetSubset<tChannels>(source, target, width, height, &lookupTable, subframeLeft, subframeWidth, sourcePaddingElements, targetPaddingElements, subframeTop, subframeHeight);
	}

	return true;
}

template <unsigned int tChannels>
void Histogram::determineHistogram8BitPerChannelSubset(const uint8_t* frame, const unsigned int width, const unsigned int height, Histogram8BitPerChannel<tChannels>* histogram, Lock* lock, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	Histogram8BitPerChannel<tChannels> localHistogram;

	if (numberColumns == width && framePaddingElements == 0u)
	{
		ocean_assert(firstColumn == 0u);

		frame += firstRow * width * tChannels;
		const uint8_t* const frameEnd = frame + numberRows * width * tChannels;

		while (frame != frameEnd)
		{
			localHistogram.increment(frame);
			frame += tChannels;
		}
	}
	else if (framePaddingElements == 0u)
	{
#ifdef OCEAN_DEBUG
		const uint8_t* const debugFrame = frame;
#endif

		frame += (firstRow * width + firstColumn) * tChannels;
		const uint8_t* const frameEnd = frame + numberRows * width * tChannels;

		const unsigned int rowOffset = (width - numberColumns) * tChannels;

		while (frame != frameEnd)
		{
			ocean_assert(frame < frameEnd);
			ocean_assert((frame - debugFrame) % (width * tChannels) == firstColumn * tChannels);

			const uint8_t* const frameRowEnd = frame + numberColumns * tChannels;

			while (frame != frameRowEnd)
			{
				ocean_assert(frame < frameEnd);
				ocean_assert(frame < frameRowEnd);

				localHistogram.increment(frame);
				frame += tChannels;
			}

			frame += rowOffset;
		}
	}
	else
	{
		const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
		frame += firstRow * frameStrideElements + firstColumn * tChannels;

		const unsigned int rowOffset = (width - numberColumns) * tChannels + framePaddingElements;

		for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
		{
			for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
			{
				localHistogram.increment(frame);
				frame += tChannels;
			}

			frame += rowOffset;
		}
	}

	const OptionalScopedLock scopedLock(lock);
	*histogram += localHistogram;
}

template <unsigned int tChannels>
void Histogram::equalizationSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>* normalizedIntegral, const unsigned int factor, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr && normalizedIntegral != nullptr);
	ocean_assert(width > 0u && height > 0u);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	ocean_assert(normalizedIntegral->determineHighestValue() <= 0xFFu);

	ocean_assert(factor <= 256u);
	const unsigned int factor_ = 256u - factor;

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		uint8_t* frameRow = frame + y * frameStrideElements + firstColumn * tChannels;

		for (unsigned int x = 0u; x < numberColumns; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				frameRow[n] = (uint8_t)((normalizedIntegral->bin(n, (uint8_t)(frameRow[n])) * factor + frameRow[n] * factor_) >> 8u);
			}

			frameRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void Histogram::equalizationOfTargetSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const IntegralHistogram8BitPerChannel<tChannels>* normalizedIntegral, const unsigned int factor, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr && normalizedIntegral != nullptr);
	ocean_assert(width > 0u && height > 0u);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	ocean_assert(normalizedIntegral->determineHighestValue() <= 0xFF);

	ocean_assert(factor <= 256u);
	const unsigned int factor_ = 256u - factor;

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements + firstColumn * tChannels;
		uint8_t* targtRow = target + y * targetStrideElements + firstColumn * tChannels;

		for (unsigned int x = 0u; x < numberColumns; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				targtRow[n] = (uint8_t)((normalizedIntegral->bin(n, sourceRow[n]) * factor + sourceRow[n] * factor_) >> 8u);
			}

			sourceRow += tChannels;
			targtRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void Histogram::adjustColorToReferenceSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const LookupTable8BitPerChannel<tChannels>* lookupTable, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(frame != nullptr && lookupTable != nullptr);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		uint8_t* frameRow = frame + y * frameStrideElements + firstColumn * tChannels;

		for (unsigned int x = 0u; x < numberColumns; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				frameRow[n] = lookupTable->lookup(n, frameRow[n]);
			}

			frameRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void Histogram::adjustColorToReferenceOfTargetSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const LookupTable8BitPerChannel<tChannels>* lookupTable, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source && target && lookupTable);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements + firstColumn * tChannels;
		uint8_t* targetRow = target + y * targetStrideElements + firstColumn * tChannels;

		for (unsigned int x = 0u; x < numberColumns; ++x)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				targetRow[n] = lookupTable->lookup(n, sourceRow[n]);
			}

			sourceRow += tChannels;
			targetRow += tChannels;
		}
	}
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_HISTOGRAM_H
