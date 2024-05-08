/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_MAPPING_I_2_H
#define META_OCEAN_CV_SYNTHESIS_MAPPING_I_2_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/MappingI.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the pixel mapping between source and target frames.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT MappingI2 : public MappingI
{
	public:

		/**
		 * Creates an empty mapping object.
		 */
		MappingI2();

		/**
		 * Copies a mapping from a given mapping object.
		 * @param pixelMapping Pixel mapping to be copied
		 */
		MappingI2(const MappingI2& pixelMapping);

		/**
		 * Move constructor.
		 * @param mapping The mapping to be moved
		 */
		MappingI2(MappingI2&& mapping) noexcept;

		/**
		 * Creates a new mapping object with defined dimension.
		 * An initial mapping is not provided.<br>
		 * @param width The width of the mapping object in pixel, with range [1, infinity)
		 * @param height The height of the mapping object in pixel, with range [1, infinity)
		 */
		MappingI2(const unsigned int width, const unsigned int height);

		/**
		 * Applies the current mapping for two given frames, a target frame receiving the mapping and a source frame providing the visual information.<br>
		 * All target pixels will be updates.
		 * @param target The target frame representing the entire target area, must be valid
		 * @param source The source frame defining the entire source area, must be valid
		 * @param sourceWidth Width of the source frame in pixel, width range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		* @tparam tChannels The number of data channel the frames have, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void applyTwoFrameMapping8BitPerChannel(uint8_t* const target, const uint8_t* const source, const unsigned int sourceWidth, Worker* worker = nullptr) const;

		/**
		 * Assigns another pixel mapping object to this one.
		 * @param pixelMapping Pixel mapping object to be copied
		 * @return Reference to this object
		 */
		inline MappingI2& operator=(const MappingI2& pixelMapping);

		/**
		 * Move operator
		 * @param pixelMapping Right mapping to assign
		 * @return Reference to this mapping
		 */
		inline MappingI2& operator=(MappingI2&& pixelMapping) noexcept;

	private:

		/**
		 * Applies the current mapping to a subset of for two given frames, a target frame receiving the mapping and a source frame providing the visual information.<br>
		 * All subset target pixels will be updates.
		 * @param target The target frame representing the entire target area, must be valid
		 * @param source The source frame defining the entire source area, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param firstRow First row to be updated, with range [0, height)
		 * @param numberRows Number of rows to be handled, with range [1, height]
		 * @tparam tChannels The number of data channel the frames have, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void applyTwoFrameMapping8BitPerChannelSubset(uint8_t* const target, const uint8_t* const source, const unsigned int sourceWidth, const unsigned int firstRow, const unsigned int numberRows) const;
};

inline MappingI2& MappingI2::operator=(const MappingI2& pixelMapping)
{
	Mapping::operator=(pixelMapping);
	return *this;
}

inline MappingI2& MappingI2::operator=(MappingI2&& pixelMapping) noexcept
{
	Mapping::operator=(std::move(pixelMapping));
	return *this;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_MAPPING_I_2_H
