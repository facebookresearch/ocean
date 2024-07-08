/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_MEDIA_TEXTURE_2D_H
#define META_OCEAN_RENDERING_MEDIA_TEXTURE_2D_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Texture2D.h"

#include "ocean/base/Timestamp.h"

#include "ocean/media/MediumRef.h"
#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class MediaTexture2D;

/**
 * Definition of a smart object reference holding a 2D texture.
 * @see SmartObjectRef, Texture2D.
 * @ingroup rendering
 */
typedef SmartObjectRef<MediaTexture2D> MediaTexture2DRef;

/**
 * This class is the base class for all 2D textures receiving their image content from Media objects.
 * @see FrameTexture2D.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT MediaTexture2D : virtual public Texture2D
{
	public:

		/**
		 * Returns the frame medium of this texture.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @return Frame medium providing the image data
		 * @see setMedium(), Media::FrameMedium.
		 */
		virtual Media::FrameMediumRef medium() const;

		/**
		 * Sets the frame medium of this texture.
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @param medium Frame medium to set
		 * @see medium(), Media::FrameMedium.
		 */
		virtual void setMedium(const Media::FrameMediumRef& medium);

		/**
		 * Returns the frame type of this texture.
		 * @return Texture frame type
		 */
		FrameType frameType() const override;

		/**
		 * Returns whether this texture contains at least one transparent pixel.
		 * @return True, if so
		 */
		bool hasTransparentPixel() const override;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new 2D media texture object.
		 */
		MediaTexture2D();

		/**
		 * Destructs a 2D media texture object.
		 */
		~MediaTexture2D() override;

	protected:

		/// Frame medium providing the image data.
		Media::FrameMediumRef frameMedium_;

		/// The frame type of the current frame.
		FrameType frameType_;

		/// The timestamp of the current frame.
		Timestamp frameTimestamp_;
};

}

}

#endif // META_OCEAN_RENDERING_MEDIA_TEXTURE_2D_H
