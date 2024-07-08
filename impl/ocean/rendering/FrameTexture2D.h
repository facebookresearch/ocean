/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_FRAME_TEXTURE_2D_H
#define META_OCEAN_RENDERING_FRAME_TEXTURE_2D_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Texture2D.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class FrameTexture2D;

/**
 * Definition of a smart object reference holding a 2D frame texture.
 * @see SmartObjectRef, Texture2D.
 * @ingroup rendering
 */
typedef SmartObjectRef<FrameTexture2D> FrameTexture2DRef;

/**
 * This class is the base class for all 2D textures receiving their image content from a frame or a buffer.
 * @see MediaTexture2D.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT FrameTexture2D : virtual public Texture2D
{
	public:

		/**
		 * Definition of individual compressed texture formats.
		 */
		enum CompressedFormat : uint32_t
		{
			/// Invalid format.
			CF_INVALID = 0u,
			/// Compressed texture with format RGBA ASTC 4x4.
			CF_RGBA_ASTC_4X4,
			/// Compressed texture with format RGBA ASTC 6x6.
			CF_RGBA_ASTC_6X6,
			/// Compressed texture with format RGBA ASTC 8x8.
			CF_RGBA_ASTC_8X8,
			/// Compressed texture with format sRGBA ASTC 4x4.
			CF_SRGBA_ASTC_4X4,
			/// Compressed texture with format sRGBA ASTC 6x6.
			CF_SRGBA_ASTC_6X6,
			/// Compressed texture with format sRGBA ASTC 8x8.
			CF_SRGBA_ASTC_8X8
		};

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * This class implements a container for a compressed texture.
		 */
		class CompressedFrame
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				CompressedFrame() = default;

				/**
				 * Creates a new object based on valid compressed texture information.
				 * @param internalFrameType The texture's internal (uncompressed) frame type, must be valid
				 * @param buffer The memory containing the compressed texture, must not be empty
				 * @param compressedFormat The format of the compressed texture, must be valid
				 * @param mipmapLevels The number of mipmap levels the given buffer contains as consecutive memory blocks, with range [1, infinity)
				 */
				inline CompressedFrame(const FrameType& internalFrameType, Buffer&& buffer, const CompressedFormat compressedFormat, const unsigned int mipmapLevels);

				/**
				 * The texture's internal (uncompressed) frame type.
				 * @return The internal frame type
				 */
				inline const FrameType& internalFrameType() const;

				/**
				 * Returns the memory containing the compressed texture.
				 * @return The compressed texture memory
				 */
				inline const Buffer& buffer() const;

				/**
				 * Returns the format of the compressed texture.
				 * @return The texture's compressed format
				 */
				inline CompressedFormat compressedFormat() const;

				/**
				 * Returns the number of mipmap levels the data of the compressed texture contains.
				 * @return The texture's mipmap levels
				 */
				inline unsigned int mipmapLevels() const;

				/**
				 * Explicitly releases the data of this object.
				 */
				inline void release();

				/**
				 * Returns whether this object contains valid information for a compressed texture.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The texture's internal (uncompressed) frame type.
				FrameType internalFrameType_;

				/// The memory containing the compressed texture.
				Buffer buffer_;

				/// The format of the compressed texture.
				CompressedFormat compressedFormat_ = CF_INVALID;

				/// The number of mipmap levels the data of the compressed texture contains.
				unsigned int mipmapLevels_ = 0u;
		};

	public:

		/**
		 * Sets or updates the texture with a given frame.
		 * @param frame The frame containing the new texture information, must be valid
		 * @return True, if succeeded
		 */
		virtual bool setTexture(Frame&& frame) = 0;

		/**
		 * Sets or updates the texture with a given compressed frame.
		 * @param compressedFrame The compressed frame containing the new texture information, must be valid
		 * @return True, if succeeded
		 */
		virtual bool setTexture(CompressedFrame&& compressedFrame) = 0;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new 2D frame texture object.
		 */
		FrameTexture2D();

		/**
		 * Destructs a 2D frame texture object.
		 */
		~FrameTexture2D() override;
};

inline FrameTexture2D::CompressedFrame::CompressedFrame(const FrameType& internalFrameType, Buffer&& buffer, const CompressedFormat compressedFormat, const unsigned int mipmapLevels) :
	internalFrameType_(internalFrameType),
	buffer_(std::move(buffer)),
	compressedFormat_(compressedFormat),
	mipmapLevels_(mipmapLevels)
{
	ocean_assert(isValid());
}

inline const FrameType& FrameTexture2D::CompressedFrame::internalFrameType() const
{
	return internalFrameType_;
}

inline const FrameTexture2D::Buffer& FrameTexture2D::CompressedFrame::buffer() const
{
	return buffer_;
}

inline FrameTexture2D::CompressedFormat FrameTexture2D::CompressedFrame::compressedFormat() const
{
	return compressedFormat_;
}

inline unsigned int FrameTexture2D::CompressedFrame::mipmapLevels() const
{
	return mipmapLevels_;
}

inline void FrameTexture2D::CompressedFrame::release()
{
	internalFrameType_ = FrameType();
	buffer_.clear();
	compressedFormat_ = CF_INVALID;
	mipmapLevels_ = 0u;
}

inline bool FrameTexture2D::CompressedFrame::isValid() const
{
	return internalFrameType_.isValid() && !buffer_.empty() && compressedFormat_ != CF_INVALID && mipmapLevels_ >= 1u;
}

}

}

#endif // META_OCEAN_RENDERING_FRAME_TEXTURE_2D_H
