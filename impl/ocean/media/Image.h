/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_H
#define META_OCEAN_MEDIA_IMAGE_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Image;

/**
 * Definition of a smart medium reference holding an image object.
 * @see SmartMediumRef, Image.
 * @ingroup media
 */
typedef SmartMediumRef<Image> ImageRef;

/**
 * This class is the base class for all images.
 * An image holds one single frame only and gets the image framebuffer data from e.g. an image file.<br>
 * @see Movie, LiveVideo, PixelImage
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Image : public virtual FrameMedium
{
	public:

		/**
		 * Definition of a class holding properties.
		 * Note: Depending on the image format, a provided property like a quality value may not have any impact, e.g., png images do not support a loosely compression.
		 */
		class Properties
		{
			public:

				/**
				 * Default constructor.
				 */
				Properties() = default;

				/**
				 * Creates a new properties object.
				 * @param quality The quality to be used, with range [0, 1], -1 if unknown or default quality
				 * @param colorProfileName The optional name of the color profile, e.g., "sRGB IEC61966-2.1", or "Adobe RGB (1998)"
				 */
				explicit inline Properties(const float quality, std::string&& colorProfileName = std::string());

				/**
				 * Returns whether this object holds valid parameters.
				 * @return True, if so
				 */
				inline bool isValid() const;

			public:

				/// The quality to be used when encoding or decoding an image, with range [0, 1], -1 if default quality or unknown.
				float quality_ = -1.0f;

				/// The name of the color profile, empty if unknown.
				std::string colorProfileName_;
		};

	protected:

		/**
		 * Creates a new image by a given url.
		 * @param url Url of the image
		 */
		explicit Image(const std::string& url);
};

inline Image::Properties::Properties(const float quality, std::string&& colorProfileName) :
	quality_(quality),
	colorProfileName_(std::move(colorProfileName))
{
	ocean_assert(isValid());
}

inline bool Image::Properties::isValid() const
{
	return quality_ == -1.0f || (quality_ >= 0.0f && quality_ <= 1.0f);
}

}

}

#endif // META_OCEAN_MEDIA_IMAGE_H
