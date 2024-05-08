/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_MAPPING_I_H
#define META_OCEAN_CV_SYNTHESIS_MAPPING_I_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Mapping.h"

#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a mapping with integer accuracy.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT MappingI : public Mapping
{
	friend class MappingF1;

	public:

		/**
		 * Destructs a mapping object.
		 */
		inline ~MappingI() override;

		/**
		 * Returns the mapping for a given position.
		 * @param x Horizontal position to return the mapping for, with range [0, width - 1]
		 * @param y Vertical position to return the mapping for, with range [0, height - 1]
		 * @return Current mapping for the specified position
		 */
		inline const PixelPosition& position(const unsigned int x, const unsigned int y) const;

		/**
		 * Returns the mapping for a given position.
		 * @param x Horizontal position to return the mapping for, with range [0, width - 1]
		 * @param y Vertical position to return the mapping for, with range [0, height - 1]
		 * @return Current mapping for the specified position
		 */
		inline PixelPosition& position(const unsigned int x, const unsigned int y);

		/**
		 * Returns the mapping for a given position.
		 * @param location The location for which the mapping will be returned, with range [0, width-1]x[0, height-1]
		 * @return Current mapping for the specified position
		 */
		inline const PixelPosition& position(const CV::PixelPosition& location) const;

		/**
		 * Returns the mapping for a given position.
		 * @param location The location for which the mapping will be returned, with range [0, width-1]x[0, height-1]
		 * @return Current mapping for the specified position
		 */
		inline PixelPosition& position(const CV::PixelPosition& location);

		/**
		 * Sets a new mapping for a specified position.
		 * @param x Horizontal position to set the mapping for, with range [0, width - 1]
		 * @param y Vertical position to set the mapping for, with range [0, height - 1]
		 * @param pixelPosition New mapping to be set
		 */
		inline void setPosition(const unsigned int x, const unsigned int y, const PixelPosition& pixelPosition);

		/**
		 * Returns the pointer to a mapping row.
		 * @param y The index of the row to return, with range [0, height - 1]
		 * @return The mapping row
		 */
		inline const PixelPosition* row(const unsigned int y) const;

		/**
		 * Returns the pointer to a mapping row.
		 * @param y The index of the row to return, with range [0, height - 1]
		 * @return The mapping row
		 */
		inline PixelPosition* row(const unsigned int y);

		/**
		 * Resets the stored mapping.
		 */
		inline void reset();

		/**
		 * Returns the mappings of this object.
		 * @return All mapping
		 */
		inline const PixelPosition* operator()() const;

		/**
		 * Returns the mappings of this object.
		 * @return All mapping
		 */
		inline PixelPosition* operator()();

	protected:

		/**
		 * Creates an empty mapping object.
		 */
		MappingI() = default;

		/**
		 * Copies a mapping from a given mapping object.
		 * @param pixelMapping Pixel mapping to be copied
		 */
		inline MappingI(const MappingI& pixelMapping);

		/**
		 * Moves a mapping from a given mapping object.
		 * @param pixelMapping Pixel mapping to be moved
		 */
		inline MappingI(MappingI&& pixelMapping) noexcept;

		/**
		 * Creates a new mapping object with defined dimension.
		 * Beware: An initial mapping is not provided.<br>
		 * @param width The width of the mapping object in pixel, with range [1, infinity)
		 * @param height The height of the mapping object in pixel, with range [1, infinity)
		 */
		inline MappingI(const unsigned int width, const unsigned int height);

		/**
		 * Calculates the sum of square differences between two 5x5 frame regions in two frames with explicit weighted mask pixels.<br>
		 * Compared to the standard ssd calculation this extended version weights the square difference of mask pixels (value not equal to 0xFF) with a given factor, further the center pixel is not considered.
		 * @param frame0 Pointer to the top left position in the 5x5 region in the first frame
		 * @param frame1 Pointer to the top left position in the 5x5 region in the second frame
		 * @param mask0 Pointer to the top left position in the 5x5 region in the mask frame, with 0xFF defining a non-mask pixel
		 * @param width0 Width of the first frame in pixel, with range [5, infinity)
		 * @param width1 Width of the second frame in pixel, with range [5, infinity)
		 * @param frame0PaddingElements The number of padding elements at the end of each first frame row, in elements, with range [0, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each second frame row, in elements, with range [0, infinity)
		 * @param mask0PaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @return Resulting sum of squared differences
		 * @tparam tChannels Number of frame channels
		 * @tparam tBorderFactor Multiplication factor for squared differences of border pixels, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tBorderFactor>
		static inline unsigned int ssd5x5MaskNoCenter(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const unsigned int width0, const unsigned int width1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const unsigned int mask0PaddingElements);

		/**
		 * Assign operator.
		 * @param pixelMapping Mapping object to be copied
		 * @return Reference to this object
		 */
		inline MappingI& operator=(const MappingI& pixelMapping);

		/**
		 * Move operator.
		 * @param pixelMapping Mapping object to be moved
		 * @return Reference to this object
		 */
		inline MappingI& operator=(MappingI&& pixelMapping) noexcept;

	protected:

		/// Pixel mappings for each pixel.
		PixelPosition* mappingI_ = nullptr;
};

inline MappingI::MappingI(const MappingI& pixelMapping) :
	Mapping(pixelMapping.width_, pixelMapping.height_),
	mappingI_(nullptr)
{
	const unsigned int size = width_ * height_;

	if (size != 0u)
	{
		mappingI_ = (PixelPosition*)malloc(size * sizeof(PixelPosition));
		ocean_assert(mappingI_ != nullptr);

		memcpy(mappingI_, pixelMapping.mappingI_, size * sizeof(PixelPosition));
	}
}

inline MappingI::MappingI(MappingI&& pixelMapping) noexcept :
	Mapping(std::move(pixelMapping)),
	mappingI_(nullptr)
{
	mappingI_ = pixelMapping.mappingI_;
	pixelMapping.mappingI_ = nullptr;
}

inline MappingI::MappingI(const unsigned int width, const unsigned int height) :
	Mapping(width, height),
	mappingI_(nullptr)
{
	const unsigned int size = width * height;

	if (size != 0)
	{
		mappingI_ = (PixelPosition*)malloc(size * sizeof(PixelPosition));
		ocean_assert(mappingI_ != nullptr);
	}
}

inline MappingI::~MappingI()
{
	free(mappingI_);
}

inline const PixelPosition& MappingI::position(const unsigned int x, const unsigned int y) const
{
	ocean_assert(x < width_ && y < height_);
	return mappingI_[y * width_ + x];
}

inline PixelPosition& MappingI::position(const unsigned int x, const unsigned int y)
{
	ocean_assert(x < width_ && y < height_);
	return mappingI_[y * width_ + x];
}

inline const PixelPosition& MappingI::position(const CV::PixelPosition& location) const
{
	return position(location.x(), location.y());
}

inline PixelPosition& MappingI::position(const CV::PixelPosition& location)
{
	return position(location.x(), location.y());
}

inline void MappingI::setPosition(const unsigned int x, const unsigned int y, const PixelPosition& pixelPosition)
{
	ocean_assert(x < width_ && y < height_);
	mappingI_[y * width_ + x] = pixelPosition;
}

inline const PixelPosition* MappingI::row(const unsigned int y) const
{
	ocean_assert(y < height_);

	return mappingI_ + y * width_;
}

inline PixelPosition* MappingI::row(const unsigned int y)
{
	ocean_assert(y < height_);

	return mappingI_ + y * width_;
}

inline void MappingI::reset()
{
	ocean_assert(mappingI_);
	memset(mappingI_, 0xFF, sizeof(PixelPosition) * width_ * height_);
}

inline const PixelPosition* MappingI::operator()() const
{
	return mappingI_;
}

inline PixelPosition* MappingI::operator()()
{
	return mappingI_;
}

template <unsigned int tChannels, unsigned int tBorderFactor>
inline unsigned int MappingI::ssd5x5MaskNoCenter(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const unsigned int width0, const unsigned int width1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const unsigned int mask0PaddingElements)
{
	static_assert(tBorderFactor >= 1u, "Invalid border factor!");

	ocean_assert(frame0 != nullptr && frame1 != nullptr && mask0 != nullptr);
	ocean_assert(width0 >= 5u && width1 >= 5u);

	const unsigned int frame0StrideElements = width0 * tChannels + frame0PaddingElements;
	const unsigned int frame1StrideElements = width1 * tChannels + frame1PaddingElements;
	const unsigned int mask0StrideElements = width0 + mask0PaddingElements;

	unsigned int ssd = 0u;

	for (unsigned int y = 0u; y < 5u; ++y)
	{
		for (unsigned int x = 0u; x < 5u; ++x)
		{
			if (x == 2u && y == 2u)
			{
				// skip the center pixel
				continue;
			}

			unsigned int local = 0u;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const int value = int(frame0[x * tChannels + n] - frame1[x * tChannels + n]);
				local += value * value;
			}

			if (mask0[x] == 0xFFu)
			{
				local *= tBorderFactor;
			}

			ssd += local;
		}

		frame0 += frame0StrideElements;
		frame1 += frame1StrideElements;
		mask0 += mask0StrideElements;
	}

	return ssd;
}

inline MappingI& MappingI::operator=(const MappingI& pixelMapping)
{
	if (this != &pixelMapping)
	{
		if (mappingI_)
		{
			free(mappingI_);
			mappingI_ = nullptr;
		}

		Mapping::operator=(pixelMapping);

		const unsigned int size = width_ * height_;

		if (size != 0u)
		{
			mappingI_ = (PixelPosition*)malloc(size * sizeof(PixelPosition));
			ocean_assert(mappingI_ != nullptr);

			memcpy(mappingI_, pixelMapping.mappingI_, size * sizeof(PixelPosition));
		}
	}

	return *this;
}

inline MappingI& MappingI::operator=(MappingI&& pixelMapping) noexcept
{
	if (this != &pixelMapping)
	{
		Mapping::operator=(std::move(pixelMapping));

		mappingI_ = pixelMapping.mappingI_;
		pixelMapping.mappingI_ = nullptr;
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_MAPPING_I_1_H
