/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_MAPPING_H
#define META_OCEAN_CV_SYNTHESIS_MAPPING_H

#include "ocean/cv/synthesis/Synthesis.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all mappings.
 * A mapping object stores source pixel locations for every target pixel (every pixel) of the work area.
 * @ingroup cvsynthesis
 */
class Mapping
{
	public:

		/**
		 * Returns the width of this mapping object.
		 * @return Mapping width in pixel, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of this mapping object.
		 * @return Mapping height in pixel, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Applies the current mapping for one given frame.<br>
		 * Only mask pixels will be updated in the frame while the specification of a bounding box in which the mapping will be applied is used to improve the performance of the execution.
		 * @param frame The frame holding source and target area, with frame dimension identical to width() x height()
		 * @param mask The 8 bit mask defining source and target area with 0xFF defining a non-mask pixel, with same frame dimension and pixel origin as the provided frame
		 * @param xStart Horizontal start position of the update area in pixel, with range [0, width())
		 * @param xWidth Width of the update area in pixel, with range [1, width() - xStart]
		 * @param yStart Vertical start position of the update area in pixel, with range [0, height())
		 * @param yHeight Height of the update area in pixel, with range [1, height() - yStart]
		 * @param worker Optional worker object to distribute the computation
		 */
		virtual void applyMapping(Frame& frame, const Frame& mask, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker = nullptr) const = 0;

		/**
		 * Returns whether this mapping object is not empty.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Calculates the normalization term for the appearance cost in accordance to the frame dimension of this mapping.
		 * @return The normalization term for the appearance cost, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline unsigned int appearanceCostNormalization() const;

		/**
		 * Calculates the normalization term for the spatial cost in accordance to the frame dimension of this mapping.
		 * @return The normalization term for the spatial cost, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline unsigned int spatialCostNormalization() const;

	protected:

		/**
		 * Creates a new mapping object.
		 */
		Mapping() = default;

		/**
		 * Copies a mapping from a given mapping object.
		 * @param pixelMapping Pixel mapping to be copied
		 */
		inline Mapping(const Mapping& pixelMapping);

		/**
		 * Moves constructor.
		 * @param pixelMapping Pixel mapping to be moved
		 */
		inline Mapping(Mapping&& pixelMapping) noexcept;

		/**
		 * Creates a new mapping object.
		 */
		inline Mapping(const unsigned int width, const unsigned int height);

		/**
		 * Destructs a mapping object.
		 */
		virtual inline ~Mapping();

		/**
		 * Calculates the normalization term for the appearance cost in accordance to a specified frame dimension.
		 * @param width The width of the frame in pixel, with range [0, infinity)
		 * @param height The height of the frame in pixel, with range [0, infinity)
		 * @return The normalization term for the appearance cost, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline unsigned int calculateAppearanceCostNormalization(const unsigned int width, const unsigned int height);

		/**
		 * Calculates the normalization term for the spatial cost in accordance to a specified frame dimension.
		 * @param width The width of the frame in pixel, with range [0, infinity)
		 * @param height The height of the frame in pixel, with range [0, infinity)
		 * @return The normalization term for the spatial cost, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline unsigned int calculateSpatialCostNormalization(const unsigned int width, const unsigned int height);

		/**
		 * Assign operator.
		 * @param mapping The mapping object to be copied
		 * @return Reference to this object
		 */
		inline Mapping& operator=(const Mapping& mapping);

		/**
		 * Move operator.
		 * @param mapping The mapping object to be moved
		 * @return Reference to this object
		 */
		inline Mapping& operator=(Mapping&& mapping) noexcept;

	protected:

		/// Width of this pixel mapping object in pixel.
		unsigned int width_ = 0u;

		/// Height of this pixel mapping object in pixel.
		unsigned int height_ = 0u;

		/// Appearance cost normalization factor for 1 channel 8 bit frames.
		unsigned int appearanceCostNormalizationInt8_ = 0u;

		/// Appearance cost normalization factor for 2 channel 16 bit frames.
		unsigned int appearanceCostNormalizationInt16_ = 0u;

		/// Appearance cost normalization factor for 3 channel 24 bit frames.
		unsigned int appearanceCostNormalizationInt24_ = 0u;

		/// Appearance cost normalization factor for 4 channel 32 bit frames.
		unsigned int appearanceCostNormalizationInt32_ = 0u;

		/// Spatial cost normalization factor for 1 channel 8 bit frames.
		unsigned int sapatialCostNormalizationInt8_ = 0u;

		/// Spatial cost normalization factor for 2 channel 16 bit frames.
		unsigned int spatialCostNormalizationInt16_ = 0u;

		/// Spatial cost normalization factor for 3 channel 24 bit frames.
		unsigned int spatialCostNormalizationInt24_ = 0u;

		/// Spatial cost normalization factor for 4 channel 32 bit frames.
		unsigned int spatialCostNormalizationInt32_ = 0u;
};

inline Mapping::Mapping(const Mapping& pixelMapping) :
	width_(pixelMapping.width_),
	height_(pixelMapping.height_),
	appearanceCostNormalizationInt8_(pixelMapping.appearanceCostNormalizationInt8_),
	appearanceCostNormalizationInt16_(pixelMapping.appearanceCostNormalizationInt16_),
	appearanceCostNormalizationInt24_(pixelMapping.appearanceCostNormalizationInt24_),
	appearanceCostNormalizationInt32_(pixelMapping.appearanceCostNormalizationInt32_),
	sapatialCostNormalizationInt8_(pixelMapping.sapatialCostNormalizationInt8_),
	spatialCostNormalizationInt16_(pixelMapping.spatialCostNormalizationInt16_),
	spatialCostNormalizationInt24_(pixelMapping.spatialCostNormalizationInt24_),
	spatialCostNormalizationInt32_(pixelMapping.spatialCostNormalizationInt32_)
{
	const unsigned int windowHalf = 2u; // **TODO** find a good solution to be able to use flexible patch sizes (not always 5x5)

	ocean_assert(int64_t(appearanceCostNormalizationInt8_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(sapatialCostNormalizationInt8_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt16_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt16_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 2ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt24_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt24_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 3ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt32_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt32_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 4ll * 26ll < int64_t((unsigned int)(-1)));

	OCEAN_SUPPRESS_UNUSED_WARNING(windowHalf);
}

inline Mapping::Mapping(Mapping&& pixelMapping) noexcept :
	width_(pixelMapping.width_),
	height_(pixelMapping.height_),
	appearanceCostNormalizationInt8_(pixelMapping.appearanceCostNormalizationInt8_),
	appearanceCostNormalizationInt16_(pixelMapping.appearanceCostNormalizationInt16_),
	appearanceCostNormalizationInt24_(pixelMapping.appearanceCostNormalizationInt24_),
	appearanceCostNormalizationInt32_(pixelMapping.appearanceCostNormalizationInt32_),
	sapatialCostNormalizationInt8_(pixelMapping.sapatialCostNormalizationInt8_),
	spatialCostNormalizationInt16_(pixelMapping.spatialCostNormalizationInt16_),
	spatialCostNormalizationInt24_(pixelMapping.spatialCostNormalizationInt24_),
	spatialCostNormalizationInt32_(pixelMapping.spatialCostNormalizationInt32_)
{
	const unsigned int windowHalf = 2u; // **TODO** find a good solution to be able to use flexible patch sizes (not always 5x5)

	ocean_assert(int64_t(appearanceCostNormalizationInt8_) * int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(sapatialCostNormalizationInt8_) +  int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(255)) * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt16_) * int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt16_) +  int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(255)) * 2ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt24_) * int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt24_) +  int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(255)) * 3ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt32_) * int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt32_) +  int64_t(sqr(2 * windowHalf + 1)) * int64_t(sqr(255)) * 4ll * 26ll < int64_t((unsigned int)(-1)));

	OCEAN_SUPPRESS_UNUSED_WARNING(windowHalf);

	pixelMapping.width_ = 0u;
	pixelMapping.height_ = 0u;

	pixelMapping.appearanceCostNormalizationInt8_ = 0u;
	pixelMapping.appearanceCostNormalizationInt16_ = 0u;
	pixelMapping.appearanceCostNormalizationInt24_ = 0u;
	pixelMapping.appearanceCostNormalizationInt32_ = 0u;

	pixelMapping.sapatialCostNormalizationInt8_ = 0u;
	pixelMapping.spatialCostNormalizationInt16_ = 0u;
	pixelMapping.spatialCostNormalizationInt24_ = 0u;
	pixelMapping.spatialCostNormalizationInt32_ = 0u;
}

inline Mapping::Mapping(const unsigned int width, const unsigned int height) :
	width_(width),
	height_(height),
	appearanceCostNormalizationInt8_(calculateAppearanceCostNormalization<1u>(width, height)),
	appearanceCostNormalizationInt16_(calculateAppearanceCostNormalization<2u>(width, height)),
	appearanceCostNormalizationInt24_(calculateAppearanceCostNormalization<3u>(width, height)),
	appearanceCostNormalizationInt32_(calculateAppearanceCostNormalization<4u>(width, height)),
	sapatialCostNormalizationInt8_(calculateSpatialCostNormalization<1u>(width, height)),
	spatialCostNormalizationInt16_(calculateSpatialCostNormalization<2u>(width, height)),
	spatialCostNormalizationInt24_(calculateSpatialCostNormalization<3u>(width, height)),
	spatialCostNormalizationInt32_(calculateSpatialCostNormalization<4u>(width, height))
{
	const unsigned int windowHalf = 2u; // **TODO** find a good solution to be able to use flexible patch sizes (not always 5x5)

	ocean_assert(int64_t(appearanceCostNormalizationInt8_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(sapatialCostNormalizationInt8_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt16_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt16_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 2ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt24_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt24_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 3ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt32_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt32_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255)) * 4ll * 26ll < int64_t((unsigned int)(-1)));

	OCEAN_SUPPRESS_UNUSED_WARNING(windowHalf);
}

inline Mapping::~Mapping()
{
	// nothing to do here
}

inline unsigned int Mapping::width() const
{
	return width_;
}

inline unsigned int Mapping::height() const
{
	return height_;
}

inline Mapping::operator bool() const
{
	return width_ != 0u && height_ != 0u;
}

template <unsigned int tChannels>
inline unsigned int Mapping::appearanceCostNormalization() const
{
	static_assert(oceanFalse<tChannels>(), "Invalid number of frame channels!");

	return 0u;
}

template <>
inline unsigned int Mapping::appearanceCostNormalization<1u>() const
{
	return appearanceCostNormalizationInt8_;
}

template <>
inline unsigned int Mapping::appearanceCostNormalization<2u>() const
{
	return appearanceCostNormalizationInt16_;
}

template <>
inline unsigned int Mapping::appearanceCostNormalization<3u>() const
{
	return appearanceCostNormalizationInt24_;
}

template <>
inline unsigned int Mapping::appearanceCostNormalization<4u>() const
{
	return appearanceCostNormalizationInt32_;
}

template <unsigned int tChannels>
inline unsigned int Mapping::spatialCostNormalization() const
{
	static_assert(oceanFalse<tChannels>(), "Invalid number of frame channels!");

	return 0u;
}

template <>
inline unsigned int Mapping::spatialCostNormalization<1u>() const
{
	return sapatialCostNormalizationInt8_;
}

template <>
inline unsigned int Mapping::spatialCostNormalization<2u>() const
{
	return spatialCostNormalizationInt16_;
}

template <>
inline unsigned int Mapping::spatialCostNormalization<3u>() const
{
	return spatialCostNormalizationInt24_;
}

template <>
inline unsigned int Mapping::spatialCostNormalization<4u>() const
{
	return spatialCostNormalizationInt32_;
}

template <unsigned int tChannels>
unsigned int Mapping::calculateAppearanceCostNormalization(const unsigned int width, const unsigned int height)
{
	if (width == 0u || height == 0u)
		return 1u;

	unsigned int appearanceNormalization = tChannels * 255u * 255u;
	unsigned int spatialCost = width * width + height * height;

	if (appearanceNormalization > spatialCost)
	{
		appearanceNormalization = (appearanceNormalization + spatialCost / 2u) / spatialCost;
		// spatialCost = 1u;
	}
	else
	{
		//spatialCost = (spatialCost + appearanceNormalization / 2u) / appearanceNormalization;
		appearanceNormalization = 1u;
	}

	return appearanceNormalization;
}

template <unsigned int tChannels>
unsigned int Mapping::calculateSpatialCostNormalization(const unsigned int width, const unsigned int height)
{
	if (width == 0u || height == 0u)
		return 1u;

	unsigned int appearanceNormalization = tChannels * 255u * 255u;
	unsigned int spatialCost = width * width + height * height;

	if (appearanceNormalization > spatialCost)
	{
		//appearanceNormalization = (appearanceNormalization + spatialCost / 2u) / spatialCost;
		spatialCost = 1;
	}
	else
	{
		spatialCost = (spatialCost + appearanceNormalization / 2u) / appearanceNormalization;
		//appearanceNormalization = 1;
	}

	return spatialCost;
}

inline Mapping& Mapping::operator=(const Mapping& mapping)
{
	width_ = mapping.width_;
	height_ = mapping.height_;

	appearanceCostNormalizationInt8_ = mapping.appearanceCostNormalizationInt8_;
	appearanceCostNormalizationInt16_ = mapping.appearanceCostNormalizationInt16_;
	appearanceCostNormalizationInt24_ = mapping.appearanceCostNormalizationInt24_;
	appearanceCostNormalizationInt32_ = mapping.appearanceCostNormalizationInt32_;

	sapatialCostNormalizationInt8_ = mapping.sapatialCostNormalizationInt8_;
	spatialCostNormalizationInt16_ = mapping.spatialCostNormalizationInt16_;
	spatialCostNormalizationInt24_ = mapping.spatialCostNormalizationInt24_;
	spatialCostNormalizationInt32_ = mapping.spatialCostNormalizationInt32_;

	return *this;
}

inline Mapping& Mapping::operator=(Mapping&& mapping) noexcept
{
	if (this != &mapping)
	{
		width_ = mapping.width_;
		height_ = mapping.height_;

		appearanceCostNormalizationInt8_ = mapping.appearanceCostNormalizationInt8_;
		appearanceCostNormalizationInt16_ = mapping.appearanceCostNormalizationInt16_;
		appearanceCostNormalizationInt24_ = mapping.appearanceCostNormalizationInt24_;
		appearanceCostNormalizationInt32_ = mapping.appearanceCostNormalizationInt32_;

		sapatialCostNormalizationInt8_ = mapping.sapatialCostNormalizationInt8_;
		spatialCostNormalizationInt16_ = mapping.spatialCostNormalizationInt16_;
		spatialCostNormalizationInt24_ = mapping.spatialCostNormalizationInt24_;
		spatialCostNormalizationInt32_ = mapping.spatialCostNormalizationInt32_;

		mapping.width_ = 0u;
		mapping.height_ = 0u;

		mapping.appearanceCostNormalizationInt8_ = 0u;
		mapping.appearanceCostNormalizationInt16_ = 0u;
		mapping.appearanceCostNormalizationInt24_ = 0u;
		mapping.appearanceCostNormalizationInt32_ = 0u;

		mapping.sapatialCostNormalizationInt8_ = 0u;
		mapping.spatialCostNormalizationInt16_ = 0u;
		mapping.spatialCostNormalizationInt24_ = 0u;
		mapping.spatialCostNormalizationInt32_ = 0u;
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_MAPPING_H
