/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_SEQUENCE_H
#define META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_SEQUENCE_H

#include "ocean/media/imageio/ImageIO.h"
#include "ocean/media/imageio/IIOObject.h"

#include "ocean/media/ImageFileSequence.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

/**
 * This class implements an image class for ImageIO.
 * @ingroup mediaiio
 */
class OCEAN_MEDIA_IIO_EXPORT IIOImageSequence :
	virtual public IIOObject,
	virtual public ImageFileSequence
{
	friend class IIOLibrary;

	public:

		/**
		 * Clones this medium and returns a new independent instance of this medium.
		 * @see Medium::clone().
		 */
		MediumRef clone() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit IIOImageSequence(const std::string& url);

		/**
		 * Destructs an image object.
		 */
		~IIOImageSequence() override;

		/**
		 * Loads a new image specified by the filename.
		 * @see ImageSequence::loadImage().
		 */
		bool loadImage(const std::string& filename, const Timestamp timestamp, Frame* frame = nullptr) override;
};

}

}

}

#endif // META_OCEAN_MEDIA_IMAGEIO_IIO_IMAGE_SEQUENCE_H
