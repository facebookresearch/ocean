// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_IMAGES_H
#define META_OCEAN_PLATFORM_META_AVATARS_IMAGES_H

#include "ocean/platform/meta/avatars/Avatars.h"
#include "ocean/platform/meta/avatars/Utilities.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/FrameTexture2D.h"

#include <ovrAvatar2/Asset.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class handles all image resources.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT Images
{
	protected:

		/**
		 * Definition of a map mapping image ids to textures.
		 */
		typedef std::unordered_map<ovrAvatar2ImageId, Rendering::TextureRef, Utilities> ImageMap;

	public:

		/**
		 * Loads all images from a given resource and converts the images into textures.
		 * @param engine The Rendering engine to be used
		 * @param resourceId The resource from which the images will be loaded
		 * @return True, if succeeded
		 */
		bool loadImages(const Rendering::Engine& engine, const ovrAvatar2ResourceId resourceId);

		/**
		 * Unloads all images for a given resource.
		 * @param resourceId The id of the resource for which all images will be unloaded
		 * @return True, if succeeded
		 */
		bool unloadImages(const ovrAvatar2ResourceId resourceId);

		/**
		 * Returns an image texture.
		 * @param imageId The id of the image for which the texture will be returned
		 * @return The resulting texture, invalid if no texture exists for the image id
		 */
		Rendering::TextureRef texture(const ovrAvatar2ImageId imageId) const;

		/**
		 * Releases all images.
		 */
		void release();

		/**
		 * Translates the image format of an image.
		 * @param imageFormat The image format of the image to translate
		 * @return The translated image format
		 */
		static std::string translateImageFormat(const ovrAvatar2ImageFormat imageFormat);

	protected:

		/// The map mapping image ids to textures.
		ImageMap imageMap_;
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_IMAGES_H
