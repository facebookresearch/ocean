/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_MAPPING_F_H
#define META_OCEAN_CV_SYNTHESIS_MAPPING_F_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Mapping.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a mapping with float accuracy.
 * @ingroup cvsynthesis
 */
class MappingF : public Mapping
{
	public:

		/**
		 * Destructs a mapping object.
		 */
		inline ~MappingF() override;

		/**
		 * Returns the mapping for a given position.
		 * @param x Horizontal position to return the mapping for, with range [0, width)
		 * @param y Vertical position to return the mapping for, with range [0, height)
		 * @return Current mapping for the specified position
		 */
		inline const Vector2& position(const unsigned int x, const unsigned int y) const;

		/**
		 * Returns the mapping for a given position.
		 * @param x Horizontal position to return the mapping for, with range [0, width)
		 * @param y Vertical position to return the mapping for, with range [0, height)
		 * @return Current mapping for the specified position
		 */
		inline Vector2& position(const unsigned int x, const unsigned int y);

		/**
		 * Sets a new mapping for a specified position.
		 * @param x Horizontal position to set the mapping for, with range [0, width)
		 * @param y Vertical position to set the mapping for, with range [0, height)
		 * @param pixelPosition New mapping to be set
		 */
		inline void setPosition(const unsigned int x, const unsigned int y, const Vector2& pixelPosition);

		/**
		 * Returns the pointer to a mapping row.
		 * @param y The index of the row to return, with range [0, height - 1]
		 * @return The mapping row
		 */
		inline const Vector2* row(const unsigned int y) const;

		/**
		 * Returns the pointer to a mapping row.
		 * @param y The index of the row to return, with range [0, height - 1]
		 * @return The mapping row
		 */
		inline Vector2* row(const unsigned int y);

		/**
		 * Resets the stored mapping.
		 */
		inline void reset();

		/**
		 * Returns the mappings of this object.
		 * @return All mapping
		 */
		inline const Vector2* operator()() const;

		/**
		 * Returns the mappings of this object.
		 * @return All mapping
		 */
		inline Vector2* operator()();

	protected:

		/**
		 * Creates an empty mapping object.
		 */
		inline MappingF();

		/**
		 * Copies a mapping from a given mapping object.
		 * @param mappingObject Pixel mapping to be copied
		 */
		inline MappingF(const MappingF& mappingObject);

		/**
		 * Move constructor.
		 * @param mappingObject Pixel mapping to be moved
		 */
		inline MappingF(MappingF&& mappingObject) noexcept;

		/**
		 * Creates a new mapping object width defined dimension.
		 * Beware: An initial mapping is not provided.<br>
		 * @param width The width of the mapping object in pixel, with range [1, infinity)
		 * @param height The height of the mapping object in pixel, with range [1, infinity)
		 */
		inline MappingF(const unsigned int width, const unsigned int height);

		/**
		 * Assign operator.
		 * @param mappingObject Mapping object to be copied
		 * @return Reference to this object
		 */
		inline MappingF& operator=(const MappingF& mappingObject);

		/**
		 * Move operator.
		 * @param mappingObject Mapping object to be moved
		 * @return Reference to this object
		 */
		inline MappingF& operator=(MappingF&& mappingObject) noexcept;

	protected:

		/// Sub-pixel mappings for each pixel.
		Vector2* mappingF_ = nullptr;
};

inline MappingF::MappingF() :
	Mapping(),
	mappingF_(nullptr)
{
	// nothing to do here
}

inline MappingF::MappingF(const MappingF& mappingObject) :
	Mapping(mappingObject),
	mappingF_(nullptr)
{
	const unsigned int size = width_ * height_;

	if (size != 0u)
	{
		mappingF_ = (Vector2*)malloc(size * sizeof(Vector2));
		ocean_assert(mappingF_ != nullptr);

		memcpy(mappingF_, mappingObject.mappingF_, size * sizeof(Vector2));
	}
}

inline MappingF::MappingF(MappingF&& mappingObject) noexcept
{
	*this = std::move(mappingObject);
}

inline MappingF::MappingF(const unsigned int width, const unsigned int height) :
	Mapping(width, height),
	mappingF_(nullptr)
{
	const unsigned int size = width * height;

	if (size != 0)
	{
		mappingF_ = (Vector2*)malloc(size * sizeof(Vector2));
		ocean_assert(mappingF_ != nullptr);

//#ifdef OCEAN_DEBUG // **TODO** currently, we set the entire mapping information to zero to ensure that we really overwrite the information layer, however that needs to be checked before removing the memset execution
		memset(mappingF_, 0, size * sizeof(Vector2));
//#endif
	}
}

inline MappingF::~MappingF()
{
	free(mappingF_);
}

const Vector2& MappingF::position(const unsigned int x, const unsigned int y) const
{
	ocean_assert(x < width_ && y < height_);

	return mappingF_[y * width_ + x];
}

Vector2& MappingF::position(const unsigned int x, const unsigned int y)
{
	ocean_assert(x < width_ && y < height_);

	return mappingF_[y * width_ + x];
}

inline void MappingF::setPosition(const unsigned int x, const unsigned int y, const Vector2& pixelPosition)
{
	ocean_assert(x < width_ && y < height_);

	mappingF_[y * width_ + x] = pixelPosition;
}

inline const Vector2* MappingF::row(const unsigned int y) const
{
	ocean_assert(y < height_);

	return mappingF_ + y * width_;
}

inline Vector2* MappingF::row(const unsigned int y)
{
	ocean_assert(y < height_);

	return mappingF_ + y * width_;
}

inline void MappingF::reset()
{
	ocean_assert(mappingF_);
	memset(mappingF_, 0, sizeof(Vector2) * width_ * height_);
}

inline const Vector2* MappingF::operator()() const
{
	return mappingF_;
}

inline Vector2* MappingF::operator()()
{
	return mappingF_;
}

inline MappingF& MappingF::operator=(const MappingF& mappingObject)
{
	if (this != &mappingObject)
	{
		Mapping::operator=(mappingObject);

		if (mappingF_)
		{
			free(mappingF_);
			mappingF_ = nullptr;
		}

		const unsigned int size = width_ * height_;

		if (size != 0u)
		{
			mappingF_ = (Vector2*)malloc(size * sizeof(Vector2));
			ocean_assert(mappingF_ != nullptr);

			memcpy(mappingF_, mappingObject.mappingF_, size * sizeof(Vector2));
		}
	}

	return *this;
}

inline MappingF& MappingF::operator=(MappingF&& mappingObject) noexcept
{
	if (this != &mappingObject)
	{
		mappingF_ = mappingObject.mappingF_;
		mappingObject.mappingF_ = nullptr;

		Mapping::operator=(std::move(mappingObject));
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_MAPPING_F_H
