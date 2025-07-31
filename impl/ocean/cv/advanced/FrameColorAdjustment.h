/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_FRAME_COLOR_ADJUSTMENT_H
#define META_OCEAN_CV_ADVANCED_FRAME_COLOR_ADJUSTMENT_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Lookup2.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements color adjustment functions between frames.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT FrameColorAdjustment
{
	public:

		/**
		 * This class implements an bin adjustment object supporting an arbitrary number of data channels.
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		class Object
		{
			public:

				/**
				 * Creates a new object with values set to zero.
				 */
				inline Object();

				/**
				 * Creates a new object and copies the given values.
				 * @param values The values to be copied
				 */
				inline explicit Object(Scalar* values);

				/**
				 * Creates a new object and sets all elements to the given value.
				 * @param value The value to be copied
				 */
				inline explicit Object(const Scalar value);

				/**
				 * Returns whether the absolute value of each element of this object is below or equal to a specified threshold.
				 * @param threshold The threshold to be checked, with range (0, infinity)
				 * @return True, if so
				 */
				inline bool belowThreshold(const Scalar threshold) const;

				/**
				 * Add operator adding two objects element by element.
				 * @param object The second object to be added
				 * @return The add result
				 */
				inline Object operator+(const Object& object) const;

				/**
				 * Subtract operator subtracting two objects element by element.
				 * @param object The second object to be subtracted
				 * @return The subtraction result
				 */
				inline Object operator-(const Object& object) const;

				/**
				 * Multiplication operator multiplying all elements of this object by a scalar.
				 * @param value The scalar value for multiplication
				 * @return The multiplication result
				 */
				inline Object operator*(const Scalar value) const;

				/**
				 * Division operator dividing all elements of this object by a scalar.
				 * @param value The scalar value for division, must not be zero
				 * @return The division result
				 */
				inline Object operator/(const Scalar value) const;

				/**
				 * Index operator returning one element of this object.
				 * @param index The index of the element to be returned, with range [0, tChannels - 1]
				 * @return The requested element
				 */
				inline Scalar operator[](const unsigned int index) const;

				/**
				 * Index operator returning one element of this object.
				 * @param index The index of the element to be returned, with range [0, tChannels - 1]
				 * @return The requested element
				 */
				inline Scalar& operator[](const unsigned int index);

			protected:

				/// The object's elements.
				Scalar objectValues_[tChannels];
		};

		/**
		 * Definition of a center lookup table holding objects.
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		using LookupTable = LookupCenter2<Object<tChannels>>;

		/**
		 * Definition of an advanced center lookup table holding objects.
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		using AdvancedLookupTable = AdvancedLookupCenter2<Object<tChannels>>;

	public:

		/**
		 * Adjusts the color values of one frame with respect to a given reference frame both showing almost the same visual content.
		 * The reference frame and the frame which will be adjusted must have the same frame dimension, pixel origin and pixel format.<br>
		 * Both frames are separated into bins, the pixel values within each bin is averaged and normalized by the number of valid pixels in the individual bins.<br>
		 * The adjustment is applied due to a bilinear interpolation between neighboring bins.
		 * @param reference The reference frame holding the reference color values, must be valid
		 * @param referenceMask Optional mask defining valid pixels in the reference frame, invalid to interpret all pixels as valid
		 * @param frame The target frame which will be adjusted, must be valid
		 * @param frameMask Optional mask defining valid pixels in the target frame, invalid to interpret all pixels as valid
		 * @param horizontalBins The number of horizontal bins, with range [1, frame.height() / 2]
		 * @param verticalBins the number of vertical bins, with range [1, frame.height() / 2]
		 * @param threshold The color threshold value which must not be exceeded by the difference between corresponding bins between reference and target frame (individually for each data channel)
		 * @param maskValue The mask value defining valid pixels
		 * @param worker Optional worker object to distribute the computation
		 * @see adjustFrameBilinear8BitPerChannel().
		 */
		static bool adjustFrameBilinear(const Frame& reference, const Frame& referenceMask, Frame& frame, const Frame& frameMask, const unsigned int horizontalBins, const unsigned int verticalBins, const Scalar threshold = Scalar(20), const uint8_t maskValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Adjusts the color values of one frame with respect to a given reference frame both showing almost the same visual content.
		 * The reference frame and the frame which will be adjusted must have the same frame dimension, pixel origin and pixel format.<br>
		 * Both frames are separated into bins, the pixel values within each bin is averaged and normalized by the number of valid pixels in the individual bins.<br>
		 * The adjustment is applied due to a bilinear interpolation between neighboring bins.
		 * @param reference The reference frame holding the reference color values, must be valid
		 * @param referenceMask Optional mask defining valid pixels in the reference frame, nullptr to interpret all pixels as valid
		 * @param frame The target frame which will be adjusted, must be valid
		 * @param frameMask Optional mask defining valid pixels in the target frame, nullptr to interpret all pixels as valid
		 * @param width The width of reference and target frame (and optional both masks) in pixel, with range [1, infinity)
		 * @param height The height of reference and target frame (and optional both masks) in pixel, with range [1, infinity)
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param referenceMaskPaddingElements The number of padding elements at the end of each reference mask row, in elements, with range [0, infinity), ignored if referenceMask == nullptr
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param frameMaskPaddingElements The number of padding elements at the end of each frame mask row, in elements, with range [0, infinity), ignored if frameMask == nullptr
		 * @param horizontalBins The number of horizontal bins, with range [1, height / 2]
		 * @param verticalBins the number of vertical bins, with range [1, height / 2]
		 * @param threshold The color threshold value which must not be exceeded by the difference between corresponding bins between reference and target frame (individually for each data channel)
		 * @param maskValue The mask value defining valid pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @see adjustFrameBilinear().
		 */
		template <unsigned int tChannels>
		static inline void adjustFrameBilinear8BitPerChannel(const uint8_t* reference, const uint8_t* referenceMask, uint8_t* frame, const uint8_t* frameMask, const unsigned int width, const unsigned int height, const unsigned int referencePaddingElements, const unsigned int referenceMaskPaddingElements, const unsigned int framePaddingElements, const unsigned int frameMaskPaddingElements, const unsigned int horizontalBins, const unsigned int verticalBins, const Scalar threshold = Scalar(20), const uint8_t maskValue = 0xFF, Worker* worker = nullptr);

		/**
		 * Determines the adjustment lookup table between two frames.
		 * @param reference The reference frame holding the reference color values, must be valid
		 * @param frame The target frame which is expected to be adjusted, must be valid
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param threshold The maximal threshold which will not be exceeded by an adjustment bin, an adjustment bin with (at least) one channel exceeding this threshold will be set as invalid bin
		 * @param lookupTable The resulting lookup table, with valid and invalid bins (depending on the specified threshold and the visual differences between both frames)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineAdjustmentLookupTable8BitPerChannel(const uint8_t* reference, const uint8_t* frame, const unsigned int referencePaddingElements, const unsigned int framePaddingElements, const Scalar threshold, LookupTable<tChannels>& lookupTable, Worker* worker = nullptr);

		/**
		 * Determines the adjustment lookup table between two frames while for each frame an optional mask frame can specify valid and invalid pixels.
		 * @param reference The reference frame holding the reference color values
		 * @param referenceMask An optional 8 bit mask specifying valid and invalid pixels in the reference frame, if valid 'tUseReferenceMask' must be True
		 * @param frame The target frame which is expected to be adjusted
		 * @param frameMask An optional 8 bit mask specifying valid and invalid pixels in the target frame, if valid 'tUseFrameMask' must be True
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param referenceMaskPaddingElements The number of padding elements at the end of each reference mask row, in elements, with range [0, infinity), ignored if tUseReferenceMask == false
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param frameMaskPaddingElements The number of padding elements at the end of each frame mask row, in elements, with range [0, infinity), ignored if tUseFrameMask == false
		 * @param threshold The maximal threshold which will not be exceeded by an adjustment bin, an adjustment bin with (at least) one channel exceeding this threshold will be set as invalid bin
		 * @param lookupTable The resulting lookup table, with valid and invalid bins (depending on the specified threshold and the visual differences between both frames)
		 * @param maskValue The 8 bit mask value specifying valid pixels in the mask frames
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tUseReferenceMask True, if a mask for the reference frame is specified
		 * @tparam tUseFrameMask True, if a mask for the target frame is given
		 */
		template <unsigned int tChannels, bool tUseReferenceMask, bool tUseFrameMask>
		static inline void determineAdjustmentLookupTable8BitPerChannel(const uint8_t* reference, const uint8_t* referenceMask, const uint8_t* frame, const uint8_t* frameMask, const unsigned int referencePaddingElements, const unsigned int referenceMaskPaddingElements, const unsigned int framePaddingElements, const unsigned int frameMaskPaddingElements, const Scalar threshold, LookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by a given color adjustment lookup table.
		 * @param frame The frame which will be adjusted, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table (providing offset values) which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void adjustFrameBilinear8BitPerChannel(uint8_t* frame, const unsigned int framePaddingElements, const LookupTable<tChannels>& lookupTable, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by a given color adjustment lookup table while a mask specifies valid and invalid pixels of the frame.
		 * @param frame The frame which will be adjusted, must be valid
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity), ignored if tUseFrameMask == false
		 * @param lookupTable The lookup table (providing offset values) which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param maskValue The mask value defining a valid pixel, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void adjustFrameMaskBilinear8BitPerChannel(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const LookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by a given color adjustment lookup table holding valid and invalid lookup bins while further a mask specifies valid and invalid pixels of the frame.
		 * @param frame The frame which will be adjusted, must be valid
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity), ignored if tUseFrameMask == false
		 * @param lookupTable The lookup table (providing offset values) which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param maskValue The mask value defining a valid pixel, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void adjustFrameMaskBilinearAdvanced8BitPerChannel(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const AdvancedLookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by two given color adjustment lookup table each holding valid and invalid lookup bins while a mask separates the frame into two parts and thus allows to use one of both lookup tables for each individual pixel of the frame.
		 * @param frame The frame which will be adjusted, must be valid
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity), ignored if tUseFrameMask == false
		 * @param lookupTableA The first lookup table (providing offset values) which is applied to adjust the given frame for 'A' mask pixels, the size of the table must be identical to the dimension of the given frame
		 * @param lookupTableB The second lookup table (providing offset values) which is applied to adjust the given frame for 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void adjustFrameMaskBilinearAdvanced8BitPerChannel(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const AdvancedLookupTable<tChannels>& lookupTableA, const AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by a given color adjustment lookup table holding valid and invalid lookup bins while further a mask specifies valid and invalid pixels of the frame.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param adjustedFrame The resulting adjusted frame
		 * @param lookupTable The lookup table (providing offset values) which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param maskValue The mask value defining a valid pixel, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tCopyPixelForInvalidLookup True, to copy image pixels if no valid corresponding lookup value can be determined; False, to do nothing in this case
		 */
		template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
		static inline void adjustFrameMaskBilinearAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by two given color adjustment lookup table each holding valid and invalid lookup bins while a mask separates the frame into two parts and thus allows to use one of both lookup tables for each individual pixel of the frame.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param adjustedFrame The resulting adjusted frame
		 * @param lookupTableA The first lookup table (providing offset values) which is applied to adjust the given frame for 'A' mask pixels, the size of the table must be identical to the dimension of the given frame
		 * @param lookupTableB The second lookup table (providing offset values) which is applied to adjust the given frame for 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tCopyPixelForInvalidLookup True, to copy image pixels if no valid corresponding lookup value can be determined; False, to do nothing in this case
		 */
		template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
		static inline void adjustFrameMaskBilinearAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>& lookupTableA, const AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjusts a frame by two given color adjustment lookup table each holding valid and invalid lookup bins while a mask separates the frame into two parts and thus allows to use one of both lookup tables for each individual pixel of the frame.
		 * @param frame The frame which will be adjusted
		 * @param filterFrame The filter frame to adjust the frame
		 * @param mask The 8 bit mask of the frame specifying valid and invalid pixels
		 * @param filterMask The 8 bit mask specifying valid and invalid filter pixels
		 * @param adjustedFrame The resulting adjusted frame
		 * @param lookupTableA The first lookup table (providing offset values) which is applied to adjust the given frame for 'A' mask pixels, the size of the table must be identical to the dimension of the given frame
		 * @param lookupTableB The second lookup table (providing offset values) which is applied to adjust the given frame for 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tCopyPixelForInvalidLookup True, to copy pixel data for invalid lookup bins
		 */
		template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
		static inline void adjustFrameMaskBilinearFilterAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* filterFrame, const uint8_t* mask, const uint8_t* filterMask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>& lookupTableA, const AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Adjust a color transfer algorithm that uses the mean and standard deviation of the image channels.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param target The target frame which will be adjusted
		 * @param width The width of reference and target frame (and optional both masks) in pixel, with range [1, infinity)
		 * @param height The height of reference and target frame (and optional both masks) in pixel, with range [1, infinity)
		 * @param frameMean Mean values (each channel) of the current frame
		 * @param frameStd Standard deviations (each channel) of the current frame
		 * @param refMean Mean values (each channel) of the reference frame
		 * @param refStd Standard deviations (each channel) of the reference frame
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void colorMatchingTransfer(const uint8_t* frame, const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const std::vector<Scalar>& frameMean, const std::vector<Scalar>& frameStd, const std::vector<Scalar>& refMean, const std::vector<Scalar>& refStd, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Compute color statistics for the source and target images
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param target The target frame which will be adjusted
		 * @param width The width of reference and target frame (and optional both masks) in pixel, with range [1, infinity)
		 * @param height The height of reference and target frame (and optional both masks) in pixel, with range [1, infinity)
		 * @param frameMean Mean values (each channel) of the current frame
		 * @param frameStd Standard deviations (each channel) of the current frame
		 * @param refMean Mean values (each channel) of the reference frame
		 * @param refStd Standard deviations (each channel) of the reference frame
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template<unsigned int tChannels>
		static inline void colorMatchingTransfer8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar* frameMean, const Scalar* frameStd, const Scalar* refMean, const Scalar* refStd, const unsigned int firstRow, const unsigned int numberRows, const uint8_t maskValue);

		/**
		 * Determines the mean color values of a given frame which may have valid and invalid pixels (specified by a given mask).
		 * The mean values are determined for small equally distributed bins and stored in a lookup table.
		 * The resulting lookup table defines the size of the provided frame (and mask).
		 * @param frame The frame for which the mean color values will be determined, with zipped pixel format and 8 bit per channel and pixel, with frame size lookupTable.sizeX() x lookupTable.sizeY()
		 * @param mask The mask frame with 8 bit per pixel, each mask pixel has a corresponding frame pixel
		 * @param lookupTable The resulting lookup table which will received the mean color intensities of the provided frame
		 * @param maskValue The mask value defining a valid pixel, large frame areas with invalid pixels may result in lookup bins set to be invalid, with range [0, 255]
		 * @param minimalSamples The minimal number of samples so that a bin counts as valid and that the mean value will be set, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineMeanFrameMaskAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, AdvancedLookupTable<tChannels>& lookupTable, const uint8_t maskValue, const unsigned int minimalSamples, Worker* worker = nullptr);

		/**
		 * Determines the mean color values of a given frame separated into two parts with own lookup table for the mean values.
		 * The mean values are determined for small equally distributed bins and stored in lookup tables.<br>
		 * The resulting lookup tables must have the same size and define the size of the provided frame (and mask).
		 * @param frame The frame for which the mean color values will be determined, with zipped pixel format and 8 bit per channel and pixel, with frame size lookupTable.sizeX() x lookupTable.sizeY()
		 * @param separationMask The mask frame with 8 bit per pixel separating the frame into 'A' and 'B' pixels, each mask pixel has a corresponding frame pixel
		 * @param lookupTableA The resulting lookup table which will receive the mean color intensities of all frame pixels with 'A' mask pixels
		 * @param lookupTableB The resulting lookup table which will receive the mean color intensities of all frame pixels with 'B' mask pixels, with identical size as lookupTableA
		 * @param separationMaskValue The separation mask value defining 'A' pixel, all other mask values define 'B' pixels, with range [0, 255]
		 * @param minimalSamples The minimal number of samples so that a bin counts as valid and that the mean value will be set, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineMeanFrameMaskAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* separationMask, AdvancedLookupTable<tChannels>& lookupTableA, AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t separationMaskValue, const unsigned int minimalSamples, Worker* worker = nullptr);

		/**
		 * Determines the mean color values of a given frame separated into two parts with own lookup table for the mean values.
		 * Further, a mask is used defining valid and invalid pixels.<br>
		 * The mean values are determined for small equally distributed bins and stored in lookup tables.<br>
		 * The resulting lookup tables must have the same size and define the size of the provided frame (and mask).
		 * @param frame The frame for which the mean color values will be determined, with zipped pixel format and 8 bit per channel and pixel, with frame size lookupTable.sizeX() x lookupTable.sizeY()
		 * @param mask The mask frame with 8 bit per pixel defining valid and invalid pixels in general, each mask pixel has a corresponding frame pixel
		 * @param separationMask The separation mask frame with 8 bit per pixel separating the frame into 'A' and 'B' pixels, each mask pixel has a corresponding frame pixel
		 * @param lookupTableA The resulting lookup table which will receive the mean color intensities of all frame pixels with 'A' mask pixels
		 * @param lookupTableB The resulting lookup table which will receive the mean color intensities of all frame pixels with 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining valid pixels, mask pixels with different value count as invalid and will not be investigated for mean calculation, with range [0, 255]
		 * @param separationMaskValue The separation mask value defining 'A' pixel, all other mask values define 'B' pixels, with range [0, 255]
		 * @param minimalSamples The minimal number of samples so that a bin counts as valid and that the mean value will be set, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineMeanFrameMaskAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* separationMask, AdvancedLookupTable<tChannels>& lookupTableA, AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, const uint8_t separationMaskValue, const unsigned int minimalSamples, Worker* worker = nullptr);

	protected:

		/**
		 * Determines a subset of an adjustment lookup table between two frames.
		 * @param reference The reference frame holding the reference color values, must be valid
		 * @param frame The target frame which is expected to be adjusted, must be valid
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param threshold The maximal threshold which will not be exceeded by an adjustment bin, an adjustment bin with (at least) one channel exceeding this threshold will be set as invalid bin
		 * @param lookupTable The resulting lookup table, with valid and invalid bins (depending on the specified threshold and the visual differences between both frames)
		 * @param firstBin The first bin to be handled
		 * @param numberBins The number of bins to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void determineAdjustmentLookupTable8BitPerChannelSubset(const uint8_t* reference, const uint8_t* frame, const unsigned int referencePaddingElements, const unsigned int framePaddingElements, const Scalar threshold, LookupTable<tChannels>* lookupTable, unsigned int firstBin, const unsigned int numberBins);

		/**
		 * Determines a subset of an adjustment lookup table between two frames while for each frame an optional mask frame can specify valid and invalid pixels.
		 * @param reference The reference frame holding the reference color values, must be valid
		 * @param referenceMask An optional 8 bit mask specifying valid and invalid pixels in the reference frame, if valid 'tUseReferenceMask' must be True
		 * @param frame The target frame which is expected to be adjusted, must be valid
		 * @param frameMask An optional 8 bit mask specifying valid and invalid pixels in the target frame, if valid 'tUseFrameMask' must be True
		 * @param referencePaddingElements The number of padding elements at the end of each reference row, in elements, with range [0, infinity)
		 * @param referenceMaskPaddingElements The number of padding elements at the end of each reference mask row, in elements, with range [0, infinity), ignored if tUseReferenceMask == false
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param frameMaskPaddingElements The number of padding elements at the end of each frame mask row, in elements, with range [0, infinity), ignored if tUseFrameMask == false
		 * @param threshold The maximal threshold which will not be exceeded by an adjustment bin, an adjustment bin with (at least) one channel exceeding this threshold will be set as invalid bin
		 * @param lookupTable The resulting lookup table, with valid and invalid bins (depending on the specified threshold and the visual differences between both frames)
		 * @param maskValue The 8 bit mask value specifying valid pixels in the mask frames
		 * @param firstBin The first bin to be handled
		 * @param numberBins The number of bins to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tUseReferenceMask True, if a mask for the reference frame is specified
		 * @tparam tUseFrameMask True, if a mask for the target frame is given
		 */
		template <unsigned int tChannels, bool tUseReferenceMask, bool tUseFrameMask>
		static void determineAdjustmentLookupTable8BitPerChannelSubset(const uint8_t* reference, const uint8_t* referenceMask, const uint8_t* frame, const uint8_t* frameMask, const unsigned int referencePaddingElements, const unsigned int referenceMaskPaddingElements, const unsigned int framePaddingElements, const unsigned int frameMaskPaddingElements, const Scalar threshold, LookupTable<tChannels>* lookupTable, const uint8_t maskValue, unsigned int firstBin, const unsigned int numberBins);

		/**
		 * Adjusts a subset of a frame by a given color adjustment lookup table.
		 * @param frame The frame which will be adjusted, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void adjustFrameBilinear8BitPerChannelSubset(uint8_t* frame, const unsigned int framePaddingElements, const LookupTable<tChannels>* lookupTable, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts a subset of a frame by a given color adjustment lookup table while a mask specifies valid and invalid pixels of the frame.
		 * @param frame The frame which will be adjusted, must be valid
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels, must be valid
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param maskValue The mask value defining a valid pixel
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void adjustFrameMaskBilinear8BitPerChannelSubset(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const LookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts a subset of a frame by a given color adjustment lookup table while a mask specifies valid and invalid pixels of the frame.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param lookupTable The lookup table which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param maskValue The mask value defining a valid pixel
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(uint8_t* frame, const uint8_t* mask, const AdvancedLookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts a subset of a frame by two given color adjustment lookup table each holding valid and invalid lookup bins while a mask separates the frame into two parts and thus allows to use one of both lookup tables for each individual pixel of the frame.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param lookupTableA The first lookup table (providing offset values) which is applied to adjust the given frame for 'A' mask pixels, the size of the table must be identical to the dimension of the given frame
		 * @param lookupTableB The second lookup table (providing offset values) which is applied to adjust the given frame for 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(uint8_t* frame, const uint8_t* mask, const AdvancedLookupTable<tChannels>* lookupTableA, const AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts a subset of a frame by a given color adjustment lookup table while a mask specifies valid and invalid pixels of the frame.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param adjustedFrame The resulting adjusted frame
		 * @param lookupTable The lookup table which is applied to adjust the given frame, the size of the table must be identical to the dimension of the given frame
		 * @param maskValue The mask value defining a valid pixel
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tCopyPixelForInvalidLookup True, to copy image pixels if no valid corresponding lookup value can be determined; False, to do nothing in this case
		 */
		template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
		static void adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adjusts a subset of a frame by two given color adjustment lookup table each holding valid and invalid lookup bins while a mask separates the frame into two parts and thus allows to use one of both lookup tables for each individual pixel of the frame.
		 * @param frame The frame which will be adjusted
		 * @param mask The 8 bit mask specifying valid and invalid frame pixels
		 * @param adjustedFrame The resulting adjusted frame
		 * @param lookupTableA The first lookup table (providing offset values) which is applied to adjust the given frame for 'A' mask pixels, the size of the table must be identical to the dimension of the given frame
		 * @param lookupTableB The second lookup table (providing offset values) which is applied to adjust the given frame for 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 * @tparam tCopyPixelForInvalidLookup True, to copy image pixels if no valid corresponding lookup value can be determined; False, to do nothing in this case
		 */
		template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
		static void adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>* lookupTableA, const AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Determines the mean color values for a subset of a given frame which may have valid and invalid pixels (specified by a given mask).
		 * The mean values are determined for small equally distributed bins and stored in a lookup table.<br>
		 * The resulting lookup table defines the size of the provided frame (and mask).
		 * @param frame The frame for which the mean color values will be determined, with zipped pixel format and 8 bit per channel and pixel, with frame size lookupTable.sizeX() x lookupTable.sizeY()
		 * @param mask The mask frame with 8 bit per pixel, each mask pixel has a corresponding frame pixel
		 * @param lookupTable The resulting lookup table which will received the mean color intensities of the provided frame
		 * @param maskValue The mask value defining a valid pixel, large frame areas with invalid pixels may result in lookup bins set to be invalid
		 * @param minimalSamples The minimal number of samples so that a bin counts as valid and that the mean value will be set, with range [1, infinity)
		 * @param firstBin The first bin to be handled, with range [0, lookupTable->binsY())
		 * @param numberBins The number of bins to be handled, with range [1, lookupTable->binsY() - firstBin]
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineMeanFrameMaskAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, AdvancedLookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int minimalSamples, const unsigned int firstBin, const unsigned int numberBins);

		/**
		 * Determines the mean color values for a subset of a given frame separated into two parts with own lookup table for the mean values.
		 * The mean values are determined for small equally distributed bins and stored in lookup tables.<br>
		 * The resulting lookup tables must have the same size and define the size of the provided frame (and mask).
		 * @param frame The frame for which the mean color values will be determined, with zipped pixel format and 8 bit per channel and pixel, with frame size lookupTable.sizeX() x lookupTable.sizeY()
		 * @param mask The mask frame with 8 bit per pixel, each mask pixel has a corresponding frame pixel
		 * @param lookupTableA The resulting lookup table which will receive the mean color intensities of all frame pixels with 'A' mask pixels
		 * @param lookupTableB The resulting lookup table which will receive the mean color intensities of all frame pixels with 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining 'A' pixel, all other mask values define 'B' pixels
		 * @param minimalSamples The minimal number of samples so that a bin counts as valid and that the mean value will be set, with range [1, infinity)
		 * @param firstBin The first bin to be handled, with range [0, lookupTable->binsY())
		 * @param numberBins The number of bins to be handled, with range [1, lookupTable->binsY() - firstBin]
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineMeanFrameMaskAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, AdvancedLookupTable<tChannels>* lookupTableA, AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const unsigned int minimalSamples, const unsigned int firstBin, const unsigned int numberBins);

		/**
		 * Determines the mean color values for a subset of a given frame separated into two parts with own lookup table for the mean values.
		 * Further, a mask is used defining valid and invalid pixels.<br>
		 * The mean values are determined for small equally distributed bins and stored in lookup tables.<br>
		 * The resulting lookup tables must have the same size and define the size of the provided frame (and mask).
		 * @param frame The frame for which the mean color values will be determined, with zipped pixel format and 8 bit per channel and pixel, with frame size lookupTable.sizeX() x lookupTable.sizeY()
		 * @param mask The mask frame with 8 bit per pixel defining valid and invalid pixels in general, each mask pixel has a corresponding frame pixel
		 * @param separationMask The separation mask frame with 8 bit per pixel separating the frame into 'A' and 'B' pixels, each mask pixel has a corresponding frame pixel
		 * @param lookupTableA The resulting lookup table which will receive the mean color intensities of all frame pixels with 'A' mask pixels
		 * @param lookupTableB The resulting lookup table which will receive the mean color intensities of all frame pixels with 'B' mask pixels, with identical size as lookupTableA
		 * @param maskValue The mask value defining valid pixels, mask pixels with different value count as invalid and will not be investigated for mean calculation, with range [0, 255]
		 * @param separationMaskValue The separation mask value defining 'A' pixel, all other mask values define 'B' pixels, with range [0, 255]
		 * @param minimalSamples The minimal number of samples so that a bin counts as valid and that the mean value will be set, with range [1, infinity)
		 * @param firstBin The first bin to be handled, with range [0, lookupTable->binsY())
		 * @param numberBins The number of bins to be handled, with range [1, lookupTable->binsY() - firstBin]
		 * @tparam tChannels The number of data channels of the given frames, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void determineMeanFrameMaskAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, const uint8_t* separationMask, AdvancedLookupTable<tChannels>* lookupTableA, AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const uint8_t separationMaskValue, const unsigned int minimalSamples, const unsigned int firstBin, const unsigned int numberBins);
};

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels>::Object()
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		objectValues_[n] = Scalar(0);
	}
}

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels>::Object(Scalar* values)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(values != nullptr);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		objectValues_[n] = values[n];
	}
}

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels>::Object(const Scalar value)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		objectValues_[n] = value;
	}
}

template <unsigned int tChannels>
inline bool FrameColorAdjustment::Object<tChannels>::belowThreshold(const Scalar threshold) const
{
	ocean_assert(threshold >= 0);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		if (Numeric::abs(objectValues_[n]) > threshold)
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels> FrameColorAdjustment::Object<tChannels>::operator+(const Object& object) const
{
	Object<tChannels> copy(*this);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		copy.objectValues_[n] += object.objectValues_[n];
	}

	return copy;
}

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels> FrameColorAdjustment::Object<tChannels>::operator-(const Object& object) const
{
	Object<tChannels> copy(*this);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		copy.objectValues_[n] -= object.objectValues_[n];
	}

	return copy;
}

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels> FrameColorAdjustment::Object<tChannels>::operator*(const Scalar value) const
{
	Object<tChannels> copy(*this);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		copy.objectValues_[n] *= value;
	}

	return copy;
}

template <unsigned int tChannels>
inline FrameColorAdjustment::Object<tChannels> FrameColorAdjustment::Object<tChannels>::operator/(const Scalar value) const
{
	Object<tChannels> copy(*this);

	ocean_assert(Numeric::isNotEqualEps(value));
	const Scalar invValue = Scalar(1) / value;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		copy.objectValues_[n] *= invValue;
	}

	return copy;
}

template <unsigned int tChannels>
inline Scalar FrameColorAdjustment::Object<tChannels>::operator[](const unsigned int index) const
{
	ocean_assert(index < tChannels);
	return objectValues_[index];
}

template <unsigned int tChannels>
inline Scalar& FrameColorAdjustment::Object<tChannels>::operator[](const unsigned int index)
{
	ocean_assert(index < tChannels);
	return objectValues_[index];
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::adjustFrameBilinear8BitPerChannel(const uint8_t* reference, const uint8_t* referenceMask, uint8_t* frame, const uint8_t* frameMask, const unsigned int width, const unsigned int height, const unsigned int referencePaddingElements, const unsigned int referenceMaskPaddingElements, const unsigned int framePaddingElements, const unsigned int frameMaskPaddingElements, const unsigned int horizontalBins, const unsigned int verticalBins, const Scalar threshold, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(reference != nullptr && frame != nullptr);

	LookupTable<tChannels> lookupTable(width, height, horizontalBins, verticalBins);

	if (referenceMask != nullptr && frameMask != nullptr)
	{
		determineAdjustmentLookupTable8BitPerChannel<tChannels, true, true>(reference, referenceMask, frame, frameMask, referencePaddingElements, referenceMaskPaddingElements, framePaddingElements, frameMaskPaddingElements, threshold, lookupTable, maskValue, worker);
	}
	else if (referenceMask != nullptr)
	{
		determineAdjustmentLookupTable8BitPerChannel<tChannels, true, false>(reference, referenceMask, frame, nullptr, referencePaddingElements, referenceMaskPaddingElements, framePaddingElements, 0u /*frameMaskPaddingElements*/, threshold, lookupTable, maskValue, worker);
	}
	else if (frameMask)
	{
		determineAdjustmentLookupTable8BitPerChannel<tChannels, false, true>(reference, nullptr, frame, frameMask, referencePaddingElements, 0u /*referenceMaskPaddingElements*/, framePaddingElements, frameMaskPaddingElements, threshold, lookupTable, maskValue, worker);
	}
	else
	{
		determineAdjustmentLookupTable8BitPerChannel<tChannels>(reference, frame, referencePaddingElements, framePaddingElements, threshold, lookupTable, worker);
	}

	if (frameMask)
	{
		adjustFrameMaskBilinear8BitPerChannel<tChannels>(frame, frameMask, framePaddingElements, frameMaskPaddingElements, lookupTable, maskValue, worker);
	}
	else
	{
		adjustFrameBilinear8BitPerChannel<tChannels>(frame, framePaddingElements, lookupTable, worker);
	}
}

template <unsigned int tChannels>
void FrameColorAdjustment::determineAdjustmentLookupTable8BitPerChannel(const uint8_t* reference, const uint8_t* frame, const unsigned int referencePaddingElements, const unsigned int framePaddingElements, const Scalar threshold, LookupTable<tChannels>& lookupTable, Worker* worker)
{
	ocean_assert(reference != nullptr && frame != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::determineAdjustmentLookupTable8BitPerChannelSubset<tChannels>, reference, frame, referencePaddingElements, framePaddingElements, threshold, &lookupTable, 0u, 0u), 0u, (unsigned int)(lookupTable.binsX() * lookupTable.binsY()));
	}
	else
	{
		determineAdjustmentLookupTable8BitPerChannelSubset<tChannels>(reference, frame, referencePaddingElements, framePaddingElements, threshold, &lookupTable, 0u, (unsigned int)(lookupTable.binsX() * lookupTable.binsY()));
	}
}

template <unsigned int tChannels, bool tUseReferenceMask, bool tUseFrameMask>
inline void FrameColorAdjustment::determineAdjustmentLookupTable8BitPerChannel(const uint8_t* reference, const uint8_t* referenceMask, const uint8_t* frame, const uint8_t* frameMask, const unsigned int referencePaddingElements, const unsigned int referenceMaskPaddingElements, const unsigned int framePaddingElements, const unsigned int frameMaskPaddingElements, const Scalar threshold, LookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(reference != nullptr && frame != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::determineAdjustmentLookupTable8BitPerChannelSubset<tChannels, tUseReferenceMask, tUseFrameMask>, reference, referenceMask, frame, frameMask, referencePaddingElements,  referenceMaskPaddingElements, framePaddingElements, frameMaskPaddingElements, threshold, &lookupTable, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTable.binsX() * lookupTable.binsY()));
	}
	else
	{
		determineAdjustmentLookupTable8BitPerChannelSubset<tChannels, tUseReferenceMask, tUseFrameMask>(reference, referenceMask, frame, frameMask, referencePaddingElements, referenceMaskPaddingElements, framePaddingElements, frameMaskPaddingElements, threshold, &lookupTable, maskValue, 0u, (unsigned int)(lookupTable.binsX() * lookupTable.binsY()));
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::adjustFrameBilinear8BitPerChannel(uint8_t* frame, const unsigned int framePaddingElements, const LookupTable<tChannels>& lookupTable, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameBilinear8BitPerChannelSubset<tChannels>, frame, framePaddingElements, &lookupTable, 0u, 0u), 0u, (unsigned int)(lookupTable.sizeY()));
	}
	else
	{
		adjustFrameBilinear8BitPerChannelSubset<tChannels>(frame, framePaddingElements, &lookupTable, 0u, (unsigned int)(lookupTable.sizeY()));
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::adjustFrameMaskBilinear8BitPerChannel(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const LookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameMaskBilinear8BitPerChannelSubset<tChannels>, frame, mask, framePaddingElements, maskPaddingElements, &lookupTable, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTable.sizeY()));
	}
	else
	{
		adjustFrameMaskBilinear8BitPerChannelSubset<tChannels>(frame, mask, framePaddingElements, maskPaddingElements, &lookupTable, maskValue, 0u, (unsigned int)(lookupTable.sizeY()));
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannel(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const AdvancedLookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameMaskBilinear8BitPerChannelSubset<tChannels>, frame, mask, framePaddingElements, maskPaddingElements, &lookupTable, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTable.sizeY()));
	}
	else
	{
		adjustFrameMaskBilinear8BitPerChannelSubset<tChannels>(frame, mask, framePaddingElements, maskPaddingElements, &lookupTable, maskValue, 0u, (unsigned int)(lookupTable.sizeY()));
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannel(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const AdvancedLookupTable<tChannels>& lookupTableA, const AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(!lookupTableA.isEmpty());
	ocean_assert(lookupTableA.sizeX() == lookupTableB.sizeX() && lookupTableA.sizeY() == lookupTableB.sizeY());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameMaskBilinear8BitPerChannelSubset<tChannels>, frame, mask, framePaddingElements, maskPaddingElements, &lookupTableA, &lookupTableB, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTableA.sizeY()));
	}
	else
	{
		adjustFrameMaskBilinear8BitPerChannelSubset<tChannels>(frame, mask, framePaddingElements, maskPaddingElements, &lookupTableA, &lookupTableB, maskValue, 0u, (unsigned int)(lookupTableA.sizeY()));
	}
}

template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
inline void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>& lookupTable, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && adjustedFrame != nullptr);
	ocean_assert(!lookupTable.isEmpty());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset<tChannels, tCopyPixelForInvalidLookup>, frame, mask, adjustedFrame, &lookupTable, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTable.sizeY()));
	}
	else
	{
		adjustFrameMaskBilinearAdvanced8BitPerChannelSubset<tChannels, tCopyPixelForInvalidLookup>(frame, mask, adjustedFrame, &lookupTable, maskValue, 0u, (unsigned int)(lookupTable.sizeY()));
	}
}

template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
inline void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>& lookupTableA, const AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && adjustedFrame != nullptr);
	ocean_assert(!lookupTableA.isEmpty());
	ocean_assert(lookupTableA.sizeX() == lookupTableB.sizeX() && lookupTableA.sizeY() == lookupTableB.sizeY());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset<tChannels, tCopyPixelForInvalidLookup>, frame, mask, adjustedFrame, &lookupTableA, &lookupTableB, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTableA.sizeY()));
	}
	else
	{
		adjustFrameMaskBilinearAdvanced8BitPerChannelSubset<tChannels, tCopyPixelForInvalidLookup>(frame, mask, adjustedFrame, &lookupTableA, &lookupTableB, maskValue, 0u, (unsigned int)(lookupTableA.sizeY()));
	}
}

template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
inline void FrameColorAdjustment::adjustFrameMaskBilinearFilterAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* filterFrame, const uint8_t* mask, const uint8_t* filterMask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>& lookupTableA, const AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && adjustedFrame != nullptr);
	ocean_assert(!lookupTableA.isEmpty());
	ocean_assert(lookupTableA.sizeX() == lookupTableB.sizeX() && lookupTableA.sizeY() == lookupTableB.sizeY());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset<tChannels, tCopyPixelForInvalidLookup>, frame, filterFrame, mask, filterMask, adjustedFrame, &lookupTableA, &lookupTableB, maskValue, 0u, 0u), 0u, (unsigned int)(lookupTableA.sizeY()));
	}
	else
	{
		adjustFrameMaskBilinearAdvanced8BitPerChannelSubset<tChannels, tCopyPixelForInvalidLookup>(frame, filterFrame, mask, filterMask, adjustedFrame, &lookupTableA, &lookupTableB, maskValue, 0u, (unsigned int)(lookupTableA.sizeY()));
	}
}

template<unsigned int tChannels>
inline void FrameColorAdjustment::colorMatchingTransfer(const uint8_t* frame, const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const Scalars& frameMean, const Scalars& frameStd, const Scalars& refMean, const Scalars& refStd, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");
	ocean_assert(frame != nullptr && mask != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::colorMatchingTransfer8BitPerChannelSubset<tChannels>, frame, mask, target, width, height, frameMean.data(), frameStd.data(), refMean.data(), refStd.data(), 0u, 0u, maskValue), 0u, height, 9u, 10u, 20u);
	}
	else
	{
		colorMatchingTransfer8BitPerChannelSubset<tChannels>(frame, mask, target, width, height, frameMean.data(), frameStd.data(), refMean.data(), refStd.data(), 0u, height, maskValue);
	}
}

template<unsigned int tChannels>
inline void FrameColorAdjustment::colorMatchingTransfer8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const Scalar* frameMean, const Scalar* frameStd, const Scalar* refMean, const Scalar* refStd, const unsigned int firstRow, const unsigned int numberRows, const uint8_t maskValue)
{
	ocean_assert(frame != nullptr && mask != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const uint8_t* pixFrame = frame;
	const uint8_t* pixMask = mask;

	pixFrame += firstRow * width * tChannels;
	pixMask += firstRow * width;
	target += firstRow * width * tChannels;

	const uint8_t* const fEnd = pixFrame + (width * numberRows * tChannels);

	while (pixFrame != fEnd)
	{
		ocean_assert(pixFrame < fEnd);
		if(*pixMask == maskValue)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				Scalar colorTransValue = pixFrame[n];
				colorTransValue -= refMean[n];
				colorTransValue *= (refStd[n] / frameStd[n]);
				colorTransValue += frameMean[n];
				target[n] = (uint8_t)(minmax<int>(0, Numeric::round32(colorTransValue), 255));
			}
		}
		else
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = pixFrame[n];
			}
		}

		pixMask++;
		pixFrame += tChannels;
		target += tChannels;
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, AdvancedLookupTable<tChannels>& lookupTable, const uint8_t maskValue, const unsigned int minimalSamples, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(!lookupTable.isEmpty());
	ocean_assert(minimalSamples >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannelSubset<tChannels>, frame, mask, &lookupTable, maskValue, minimalSamples, 0u, 0u), 0u, (unsigned int)(lookupTable.binsY()));
	}
	else
	{
		determineMeanFrameMaskAdvanced8BitPerChannelSubset<tChannels>(frame, mask, &lookupTable, maskValue, minimalSamples, 0u, (unsigned int)(lookupTable.binsY()));
	}
}

template <unsigned int tChannels>
void FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* separationMask, AdvancedLookupTable<tChannels>& lookupTableA, AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t separationMaskValue, const unsigned int minimalSamples, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && separationMask != nullptr);
	ocean_assert(!lookupTableA.isEmpty());
	ocean_assert(lookupTableA.sizeX() == lookupTableB.sizeX() && lookupTableA.sizeY() == lookupTableB.sizeY());
	ocean_assert(minimalSamples >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannelSubset<tChannels>, frame, separationMask, &lookupTableA, &lookupTableB, separationMaskValue, minimalSamples, 0u, 0u), 0u, (unsigned int)(lookupTableA.binsY()));
	}
	else
	{
		determineMeanFrameMaskAdvanced8BitPerChannelSubset<tChannels>(frame, separationMask, &lookupTableA, &lookupTableB, separationMaskValue, minimalSamples, 0u, (unsigned int)(lookupTableA.binsY()));
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const uint8_t* separationMask, AdvancedLookupTable<tChannels>& lookupTableA, AdvancedLookupTable<tChannels>& lookupTableB, const uint8_t maskValue, const uint8_t separationMaskValue, const unsigned int minimalSamples, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && separationMask != nullptr);
	ocean_assert(!lookupTableA.isEmpty());
	ocean_assert(lookupTableA.sizeX() == lookupTableB.sizeX() && lookupTableA.sizeY() == lookupTableB.sizeY());
	ocean_assert(minimalSamples >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannelSubset<tChannels>, frame, mask, separationMask, &lookupTableA, &lookupTableB, maskValue, separationMaskValue, minimalSamples, 0u, 0u), 0u, (unsigned int)(lookupTableA.binsY()));
	}
	else
	{
		determineMeanFrameMaskAdvanced8BitPerChannelSubset<tChannels>(frame, mask, separationMask, &lookupTableA, &lookupTableB, maskValue, separationMaskValue, minimalSamples, 0u, (unsigned int)(lookupTableA.binsY()));
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::determineAdjustmentLookupTable8BitPerChannelSubset(const uint8_t* reference, const uint8_t* frame, const unsigned int referencePaddingElements, const unsigned int framePaddingElements, const Scalar threshold, LookupTable<tChannels>* lookupTable, unsigned int firstBin, const unsigned int numberBins)
{
	ocean_assert(reference != nullptr && frame != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());

	const unsigned int widthElements = (unsigned int)(lookupTable->sizeX()) * tChannels;

	const unsigned int referenceStrideElements = widthElements + referencePaddingElements;
	const unsigned int frameStrideElements = widthElements + framePaddingElements;

	for (unsigned int n = firstBin; n < firstBin + numberBins; ++n)
	{
		const unsigned int binY = n / (unsigned int)(lookupTable->binsX());
		const unsigned int binX = n - binY * (unsigned int)(lookupTable->binsX());
		ocean_assert(binX == n % lookupTable->binsX());

		const unsigned int left = (unsigned int)(lookupTable->binTopLeftX(binX));
		const unsigned int top = (unsigned int)(lookupTable->binTopLeftY(binY));

		const unsigned int right = (unsigned int)(lookupTable->binBottomRightX(binX));
		const unsigned int bottom = (unsigned int)(lookupTable->binBottomRightY(binY));

		Object<tChannels> referenceObject, frameObject;

		const uint8_t* framePixel = frame + top * frameStrideElements + left * tChannels;
		const uint8_t* referencePixel = reference + top * referenceStrideElements + left * tChannels;

		const unsigned int binWidthElements = (right - left + 1u) * tChannels;
		ocean_assert(binWidthElements <= frameStrideElements && binWidthElements <= referenceStrideElements);

		const unsigned int frameRowOffset = frameStrideElements - binWidthElements;
		const unsigned int referenceRowOffset = referenceStrideElements - binWidthElements;

		for (unsigned int y = top; y <= bottom; ++y)
		{
			for (unsigned int x = left; x <= right; ++x)
			{
				for (unsigned int i = 0u; i < tChannels; ++i)
				{
					frameObject[i] += framePixel[i];
				}

				for (unsigned int i = 0u; i < tChannels; ++i)
				{
					referenceObject[i] += referencePixel[i];
				}

				framePixel += tChannels;
				referencePixel += tChannels;
			}

			framePixel += frameRowOffset;
			referencePixel += referenceRowOffset;
		}

		ocean_assert((right - left + 1u) * (bottom - top + 1u) != 0u);
		const Scalar normalization = Scalar(1) / Scalar((right - left + 1u) * (bottom - top + 1u));

		Object<tChannels> object;

		for (unsigned int i = 0u; i < tChannels; ++i)
		{
			object[i] = (referenceObject[i] - frameObject[i]) * normalization;
		}

		if (object.belowThreshold(threshold))
		{
			lookupTable->setBinCenterValue(binX, binY, object);
		}
		else
		{
			lookupTable->setBinCenterValue(binX, binY, Object<tChannels>());
		}
	}
}

template <unsigned int tChannels, bool tUseReferenceMask, bool tUseFrameMask>
inline void FrameColorAdjustment::determineAdjustmentLookupTable8BitPerChannelSubset(const uint8_t* reference, const uint8_t* referenceMask, const uint8_t* frame, const uint8_t* frameMask, const unsigned int referencePaddingElements, const unsigned int referenceMaskPaddingElements, const unsigned int framePaddingElements, const unsigned int frameMaskPaddingElements, const Scalar threshold, LookupTable<tChannels>* lookupTable, const uint8_t maskValue, unsigned int firstBin, const unsigned int numberBins)
{
	ocean_assert(reference != nullptr && frame != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());

	const unsigned int widthElements = (unsigned int)(lookupTable->sizeX()) * tChannels;

	const unsigned int referenceStrideElements = widthElements + referencePaddingElements;
	const unsigned int referenceMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + referenceMaskPaddingElements;

	const unsigned int frameStrideElements = widthElements + framePaddingElements;
	const unsigned int frameMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + frameMaskPaddingElements;

	for (unsigned int n = firstBin; n < firstBin + numberBins; ++n)
	{
		const unsigned int binY = n / (unsigned int)lookupTable->binsX();
		const unsigned int binX = n - binY * (unsigned int)lookupTable->binsX();
		ocean_assert(binX == n % lookupTable->binsX());

		const unsigned int left = (unsigned int)lookupTable->binTopLeftX(binX);
		const unsigned int top = (unsigned int)lookupTable->binTopLeftY(binY);

		const unsigned int right = (unsigned int)lookupTable->binBottomRightX(binX);
		const unsigned int bottom = (unsigned int)lookupTable->binBottomRightY(binY);

		Object<tChannels> referenceObject, frameObject;

		unsigned int referenceCounter = 0u;
		unsigned int frameCounter = 0u;

		const uint8_t* framePixel = frame + top * frameStrideElements + left * tChannels;
		const uint8_t* frameMaskPixel = tUseFrameMask ? frameMask + top * frameMaskStrideElements + left : nullptr;

		const uint8_t* referencePixel = reference + top * referenceStrideElements + left * tChannels;
		const uint8_t* referenceMaskPixel = tUseReferenceMask ? referenceMask + top * referenceMaskStrideElements + left : nullptr;

		const unsigned int binWidthElements = (right - left + 1u) * tChannels;
		ocean_assert(binWidthElements <= frameStrideElements && binWidthElements <= referenceStrideElements);

		const unsigned int frameRowOffset = frameStrideElements - binWidthElements;
		const unsigned int frameMaskRowOffset = tUseFrameMask ? frameMaskStrideElements - (right - left + 1u) : 0u;

		const unsigned int referenceRowOffset = referenceStrideElements - binWidthElements;
		const unsigned int referenceMaskRowOffset = tUseReferenceMask ? referenceMaskStrideElements - (right - left + 1u) : 0u;

		for (unsigned int y = top; y <= bottom; ++y)
		{
			for (unsigned int x = left; x <= right; ++x)
			{
				if (!tUseFrameMask || *frameMaskPixel == maskValue)
				{
					for (unsigned int i = 0u; i < tChannels; ++i)
					{
						frameObject[i] += framePixel[i];
					}

					++frameCounter;
				}

				if (!tUseReferenceMask || *referenceMaskPixel == maskValue)
				{
					for (unsigned int i = 0u; i < tChannels; ++i)
					{
						referenceObject[i] += referencePixel[i];
					}

					++referenceCounter;
				}

				framePixel += tChannels;
				referencePixel += tChannels;

				if constexpr (tUseFrameMask)
				{
					++frameMaskPixel;
				}

				if constexpr (tUseReferenceMask)
				{
					++referenceMaskPixel;
				}
			}

			framePixel += frameRowOffset;
			referencePixel += referenceRowOffset;

			if constexpr (tUseFrameMask)
			{
				frameMaskPixel += frameMaskRowOffset;
			}

			if constexpr (tUseReferenceMask)
			{
				referenceMaskPixel += referenceMaskRowOffset;
			}
		}

		if (frameCounter != 0u && referenceCounter != 0u)
		{
			const Scalar frameNormalization = Scalar(1) / Scalar(frameCounter);
			const Scalar referenceNormalization = Scalar(1) / Scalar(referenceCounter);

			Object<tChannels> object;

			for (unsigned int i = 0u; i < tChannels; ++i)
			{
				object[i] = referenceObject[i] * referenceNormalization - frameObject[i] * frameNormalization;
			}

			if (object.belowThreshold(threshold))
			{
				lookupTable->setBinCenterValue(binX, binY, object);
			}
			else
			{
				lookupTable->setBinCenterValue(binX, binY, Object<tChannels>());
			}
		}
	}
}

template <unsigned int tChannels>
void FrameColorAdjustment::adjustFrameBilinear8BitPerChannelSubset(uint8_t* frame, const unsigned int framePaddingElements, const LookupTable<tChannels>* lookupTable, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(firstRow + numberRows <= lookupTable->sizeY());

	const unsigned int frameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + framePaddingElements;

	frame += firstRow * frameStrideElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)(lookupTable->sizeX()); ++x)
		{
			const Object<tChannels> object(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				frame[n] = (uint8_t)(minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255));
			}

			frame += tChannels;
		}

		frame += framePaddingElements;
	}
}

template <unsigned int tChannels>
void FrameColorAdjustment::adjustFrameMaskBilinear8BitPerChannelSubset(uint8_t* frame, const uint8_t* mask, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const LookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(firstRow + numberRows <= lookupTable->sizeY());

	const unsigned int frameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = (unsigned int)(lookupTable->sizeX()) + maskPaddingElements;

	frame += firstRow * frameStrideElements;
	mask += firstRow * maskStrideElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)lookupTable->sizeX(); ++x)
		{
			if (*mask == maskValue)
			{
				const Object<tChannels> object(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					frame[n] = (uint8_t)(minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255));
				}
			}

			frame += tChannels;
			++mask;
		}

		frame += framePaddingElements;
		mask += maskPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(uint8_t* frame, const uint8_t* mask, const AdvancedLookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(firstRow + numberRows <= lookupTable->sizeY());

	frame += firstRow * lookupTable->sizeX() * tChannels;
	mask += firstRow * lookupTable->sizeX();

	Object<tChannels> object;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)lookupTable->sizeX(); ++x)
		{
			if (*mask == maskValue && lookupTable->bilinearValue(Scalar(x), Scalar(y), object))
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
					frame[n] = minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255);
			}

			frame += tChannels;
			mask++;
		}
	}
}

template <unsigned int tChannels>
void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(uint8_t* frame, const uint8_t* mask, const AdvancedLookupTable<tChannels>* lookupTableA, const AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(lookupTableA != nullptr && lookupTableB != nullptr && !lookupTableA->isEmpty());
	ocean_assert(lookupTableA->sizeX() == lookupTableB->sizeX() && lookupTableA->sizeY() == lookupTableB->sizeY());
	ocean_assert(firstRow + numberRows <= lookupTableA->sizeY());

	frame += firstRow * lookupTableA->sizeX() * tChannels;
	mask += firstRow * lookupTableA->sizeX();

	Object<tChannels> object;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)lookupTableA->sizeX(); ++x)
		{
			if (*mask == maskValue)
			{
				if (lookupTableA->bilinearValue(Scalar(x), Scalar(y), object))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
						frame[n] = minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255);
				}
			}
			else
			{
				ocean_assert(*mask != maskValue);

				if (lookupTableB->bilinearValue(Scalar(x), Scalar(y), object))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
						frame[n] = minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255);
				}
			}

			frame += tChannels;
			mask++;
		}
	}
}

template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && adjustedFrame != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(firstRow + numberRows <= lookupTable->sizeY());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	frame += firstRow * lookupTable->sizeX() * tChannels;
	adjustedFrame += firstRow * lookupTable->sizeX() * tChannels;
	mask += firstRow * lookupTable->sizeX();

	Object<tChannels> object;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)lookupTable->sizeX(); ++x)
		{
			if (*mask == maskValue)
			{
				if (lookupTable->bilinearValue(Scalar(x), Scalar(y), object))
				{
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						adjustedFrame[n] = minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255);
					}
				}
				else
				{
					if constexpr (tCopyPixelForInvalidLookup)
					{
						*((PixelType*)adjustedFrame) = *((PixelType*)frame);
					}
				}
			}

			frame += tChannels;
			adjustedFrame += tChannels;
			mask++;
		}
	}
}

template <unsigned int tChannels, bool tCopyPixelForInvalidLookup>
void FrameColorAdjustment::adjustFrameMaskBilinearAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, uint8_t* adjustedFrame, const AdvancedLookupTable<tChannels>* lookupTableA, const AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr && adjustedFrame != nullptr);
	ocean_assert(lookupTableA != nullptr && lookupTableB != nullptr && !lookupTableA->isEmpty());
	ocean_assert(lookupTableA->sizeX() == lookupTableB->sizeX() && lookupTableA->sizeY() == lookupTableB->sizeY());
	ocean_assert(firstRow + numberRows <= lookupTableA->sizeY());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	frame += firstRow * lookupTableA->sizeX() * tChannels;
	adjustedFrame += firstRow * lookupTableA->sizeX() * tChannels;
	mask += firstRow * lookupTableA->sizeX();

	Object<tChannels> object;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)(lookupTableA->sizeX()); ++x)
		{
			if (*mask == maskValue)
			{
				if (lookupTableA->bilinearValue(Scalar(x), Scalar(y), object))
				{
					//object = lookupTableA->binCenterValue(lookupTableA->binX(x), lookupTableA->binY(y));
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						adjustedFrame[n] = minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255);
					}
				}
				else
				{
					if constexpr (tCopyPixelForInvalidLookup)
					{
						*((PixelType*)adjustedFrame) = *((PixelType*)frame);
					}
				}
			}
			else
			{
				ocean_assert(*mask != maskValue);

				if (lookupTableB->bilinearValue(Scalar(x), Scalar(y), object))
				{
					//object = lookupTableB->binCenterValue(lookupTableB->binX(x), lookupTableB->binY(y));
					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						adjustedFrame[n] = minmax<int>(0, Numeric::round32(Scalar(frame[n]) + object[n]), 255);
					}
				}
				else
				{
					if constexpr (tCopyPixelForInvalidLookup)
					{
						*((PixelType*)adjustedFrame) = *((PixelType*)frame);
					}
				}
			}

			frame += tChannels;
			adjustedFrame += tChannels;
			mask++;
		}
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, AdvancedLookupTable<tChannels>* lookupTable, const uint8_t maskValue, const unsigned int minimalSamples, const unsigned int firstBin, const unsigned int numberBins)
{
	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(minimalSamples >= 1u);

	ocean_assert(firstBin + numberBins <= lookupTable->binsY());

	const unsigned int width = lookupTable->sizeX();

	for (unsigned int yBin = firstBin; yBin < firstBin + numberBins; ++yBin)
	{
		for (unsigned int xBin = 0u; xBin < lookupTable->binsX(); ++xBin)
		{
			const unsigned int left = (unsigned int)lookupTable->binTopLeftX(xBin);
			const unsigned int top = (unsigned int)lookupTable->binTopLeftY(yBin);

			const unsigned int right = (unsigned int)lookupTable->binBottomRightX(xBin);
			const unsigned int bottom = (unsigned int)lookupTable->binBottomRightY(yBin);

			Object<tChannels> value;
			unsigned int samples = 0u;

			for (unsigned int y = top; y <= bottom; ++y)
			{
				for (unsigned int x = left; x <= right; ++x)
				{
					if (mask[y * width + x] == maskValue)
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							value[n] += Scalar(frame[tChannels * (y * width + x) + n]);
						}

						samples++;
					}
				}
			}

			if (samples >= minimalSamples)
			{
				lookupTable->setBinCenterValue(xBin, yBin, value / Scalar(samples), true);
			}
			else
			{
				lookupTable->setBinCenterValue(xBin, yBin, Object<tChannels>(), false);
			}
		}
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* separationMask, AdvancedLookupTable<tChannels>* lookupTableA, AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t separationMaskValue, const unsigned int minimalSamples, const unsigned int firstBin, const unsigned int numberBins)
{
	ocean_assert(frame != nullptr && separationMask != nullptr);
	ocean_assert(lookupTableA != nullptr && !lookupTableA->isEmpty());
	ocean_assert(lookupTableB != nullptr && lookupTableA->sizeX() == lookupTableB->sizeX() && lookupTableA->sizeY() == lookupTableB->sizeY());
	ocean_assert(minimalSamples >= 1u);

	ocean_assert(firstBin + numberBins <= lookupTableA->binsY());

	const unsigned int width = (unsigned int)lookupTableA->sizeX();

	for (unsigned int yBin = firstBin; yBin < firstBin + numberBins; ++yBin)
	{
		for (unsigned int xBin = 0u; xBin < lookupTableA->binsX(); ++xBin)
		{
			const unsigned int left = (unsigned int)lookupTableA->binTopLeftX(xBin);
			const unsigned int top = (unsigned int)lookupTableA->binTopLeftY(yBin);

			const unsigned int right = (unsigned int)lookupTableA->binBottomRightX(xBin);
			const unsigned int bottom = (unsigned int)lookupTableA->binBottomRightY(yBin);

			Object<tChannels> valueA;
			Object<tChannels> valueB;
			unsigned int samplesA = 0u;
			unsigned int samplesB = 0u;

			for (unsigned int y = top; y <= bottom; ++y)
			{
				for (unsigned int x = left; x <= right; ++x)
				{
					if (separationMask[y * width + x] == separationMaskValue)
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							valueA[n] += Scalar(frame[tChannels * (y * width + x) + n]);
						}

						samplesA++;
					}
					else
					{
						for (unsigned int n = 0u; n < tChannels; ++n)
						{
							valueB[n] += Scalar(frame[tChannels * (y * width + x) + n]);
						}

						samplesB++;
					}
				}
			}

			if (samplesA >= minimalSamples)
			{
				lookupTableA->setBinCenterValue(xBin, yBin, valueA / Scalar(samplesA), true);
			}
			else
			{
				lookupTableA->setBinCenterValue(xBin, yBin, Object<tChannels>(), false);
			}

			if (samplesB >= minimalSamples)
			{
				lookupTableB->setBinCenterValue(xBin, yBin, valueB / Scalar(samplesB), true);
			}
			else
			{
				lookupTableB->setBinCenterValue(xBin, yBin, Object<tChannels>(), false);
			}
		}
	}
}

template <unsigned int tChannels>
inline void FrameColorAdjustment::determineMeanFrameMaskAdvanced8BitPerChannelSubset(const uint8_t* frame, const uint8_t* mask, const uint8_t* separationMask, AdvancedLookupTable<tChannels>* lookupTableA, AdvancedLookupTable<tChannels>* lookupTableB, const uint8_t maskValue, const uint8_t separationMaskValue, const unsigned int minimalSamples, const unsigned int firstBin, const unsigned int numberBins)
{
	ocean_assert(frame != nullptr && mask != nullptr && separationMask != nullptr);
	ocean_assert(lookupTableA != nullptr && !lookupTableA->isEmpty());
	ocean_assert(lookupTableB != nullptr && lookupTableA->sizeX() == lookupTableB->sizeX() && lookupTableA->sizeY() == lookupTableB->sizeY());
	ocean_assert(minimalSamples >= 1u);

	ocean_assert(firstBin + numberBins <= lookupTableA->binsY());

	const unsigned int width = (unsigned int)lookupTableA->sizeX();

	for (unsigned int yBin = firstBin; yBin < firstBin + numberBins; ++yBin)
	{
		for (unsigned int xBin = 0u; xBin < lookupTableA->binsX(); ++xBin)
		{
			const unsigned int left = (unsigned int)lookupTableA->binTopLeftX(xBin);
			const unsigned int top = (unsigned int)lookupTableA->binTopLeftY(yBin);

			const unsigned int right = (unsigned int)lookupTableA->binBottomRightX(xBin);
			const unsigned int bottom = (unsigned int)lookupTableA->binBottomRightY(yBin);

			Object<tChannels> valueA;
			Object<tChannels> valueB;
			unsigned int samplesA = 0u;
			unsigned int samplesB = 0u;

			for (unsigned int y = top; y <= bottom; ++y)
			{
				for (unsigned int x = left; x <= right; ++x)
				{
					if (mask[y * width + x] == maskValue)
					{
						if (separationMask[y * width + x] == separationMaskValue)
						{
							for (unsigned int n = 0u; n < tChannels; ++n)
								valueA[n] += Scalar(frame[tChannels * (y * width + x) + n]);

							samplesA++;
						}
						else
						{
							for (unsigned int n = 0u; n < tChannels; ++n)
								valueB[n] += Scalar(frame[tChannels * (y * width + x) + n]);

							samplesB++;
						}
					}
				}
			}

			if (samplesA >= minimalSamples)
			{
				lookupTableA->setBinCenterValue(xBin, yBin, valueA / Scalar(samplesA), true);
			}
			else
			{
				lookupTableA->setBinCenterValue(xBin, yBin, Object<tChannels>(), false);
			}

			if (samplesB >= minimalSamples)
			{
				lookupTableB->setBinCenterValue(xBin, yBin, valueB / Scalar(samplesB), true);
			}
			else
			{
				lookupTableB->setBinCenterValue(xBin, yBin, Object<tChannels>(), false);
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_FRAME_COLOR_ADJUSTMENT_H
