/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_WIC_WIC_IMAGE_SEQUENCE_H
#define META_OCEAN_MEDIA_WIC_WIC_IMAGE_SEQUENCE_H

#include "ocean/media/wic/WIC.h"
#include "ocean/media/wic/WICObject.h"

#include "ocean/media/ImageFileSequence.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

/**
 * This class implements an image sequence for WIC.
 * @ingroup mediawic
 */
class OCEAN_MEDIA_WIC_EXPORT WICImageSequence :
	virtual public WICObject,
	virtual public ImageFileSequence
{
	friend class WICLibrary;

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
		explicit WICImageSequence(const std::string& url);

		/**
		 * Destructs an image object.
		 */
		virtual ~WICImageSequence();

		/**
		 * Loads a new image specified by the filename.
		 * @see ImageSequence::loadImage().
		 */
		bool loadImage(const std::string& filename, const Timestamp timestamp, Frame* frame = nullptr) override;
};

}

}

}

#endif // META_OCEAN_MEDIA_WIC_WIC_IMAGE_SEQUENCE_H
