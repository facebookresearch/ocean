/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_IMAGE_LIST_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_IMAGE_LIST_H

#include "ocean/platform/apple/macos/MacOS.h"
#include "ocean/platform/apple/macos/Image.h"

#include "ocean/base/Frame.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::ImageList.h needs to be included from an ObjectiveC++ file
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class implements an image list.
 * @ingroup platformapplemacos
 */
class ImageList
{
	protected:

		/**
		 * Definition of a vector holding image object.
		 */
		typedef std::vector<Image> Images;

	public:

		/**
		 * Creates a new image list object.
		 */
		inline ImageList();

		/**
		 * Move constructor.
		 * @param imageList The image list to be moved
		 */
		inline ImageList(ImageList&& imageList) noexcept;

		/**
		 * Not existing copy constructor.
		 * @param imageList The image list to be copied
		 */
		inline ImageList(const ImageList& imageList) = delete;

		/**
		 * Creates a new image list object from one large frame containing the individual images within the same row.
		 * The with of the given frame must be a multiple of the height of the given frame.<br>
		 * @param frame The large frame containing all individual images, must be valid
		 * @param scaleFactor The scale factor converting the number of pixels in the (virtual) display coordinate system to the number of pixels in the (native/pyhsical) screen coordinate system, with range (0, infinity)
		 */
		explicit ImageList(const Frame& frame, const double scaleFactor = 1.0);

		/**
		 * Returns the number of images of this list.
		 * @return The number of images
		 */
		inline size_t size() const;

		/**
		 * Clears this image list.
		 */
		inline void clear();

		/**
		 * Returns whether this image list does not hold any image.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Move operator.
		 * @param imageList The image list object to be moved
		 * @return Reference to this object
		 */
		inline ImageList& operator=(ImageList&& imageList) noexcept;

		/**
		 * Not exisiting assign operator.
		 * @param imageList The image list object to be copied
		 * @return Reference to this object
		 */
		inline ImageList& operator=(const ImageList& imageList) = delete;

		/**
		 * The index access object providing access to a specific frame of this frame list.
		 * @param index The index of the frame to access, with range [0, size())
		 * @return The requested frame
		 */
		inline Image& operator[](const size_t index);

	protected:

		/// The images of this list.
		Images listImages;
};

inline ImageList::ImageList()
{
	// nothing to do here
}

inline ImageList::ImageList(ImageList&& imageList) noexcept :
	listImages(std::move(imageList.listImages))
{
	// nothing to do here
}

inline size_t ImageList::size() const
{
	return listImages.size();
}

inline void ImageList::clear()
{
	listImages.clear();
}

inline bool ImageList::isEmpty() const
{
	return listImages.empty();
}

inline ImageList& ImageList::operator=(ImageList&& imageList) noexcept
{
	if (this != &imageList)
	{
		listImages = std::move(imageList.listImages);
	}

	return *this;
}

inline Image& ImageList::operator[](const size_t index)
{
	ocean_assert(index < listImages.size());
	return listImages[index];
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_IMAGE_LIST_H
